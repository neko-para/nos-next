#include "lib/task.hpp"

#include "lib/alloc.hpp"
#include "lib/io.hpp"
#include "lib/list.hpp"
#include "lib/page.hpp"
#include "lib/timer.hpp"
#include "std/string.hpp"
#include <algorithm>

kernel::task::ControlBlock* currentTask;

namespace kernel::task
{

static kernel::list::ListHolder<ControlBlock> readyTask;
static kernel::list::ListHolder<ControlBlock> sleepingTask;
static uint32_t lockScheduleCounter;

void lock_schedule()
{
    asm volatile("cli");
    lockScheduleCounter++;
}

void unlock_schedule()
{
    lockScheduleCounter--;
    if (!lockScheduleCounter) {
        asm volatile("sti");
    }
}

struct ScheduleLockGuard
{
    ScheduleLockGuard() { lock_schedule(); }

    ~ScheduleLockGuard() { unlock_schedule(); }
};

uint32_t make_stack(uint32_t entry, uint32_t param)
{
    auto stack = static_cast<uint32_t*>(alloc::frameAllocator.alloc()) + (1 << 10);
    *--stack = param;
    *--stack = 0x12345678; // fake eip
    *--stack = entry;
    *--stack = 0;
    *--stack = 0;
    *--stack = 0;
    *--stack = 0;
    return reinterpret_cast<uint32_t>(stack);
}

ControlBlock* make_kernel_task(void (*entry)(uint32_t param), uint32_t cr3, uint32_t param, const char* name)
{
    ControlBlock* block = new ControlBlock;
    block->esp = make_stack(reinterpret_cast<uint32_t>(entry), param);
    block->cr3 = cr3;
    block->kesp = (block->esp & (alloc::FrameAllocator::frameSize - 1)) + (1 << 10); // 1K kernel stack of 4K stack
    block->state = Pending;
    strcpy(block->name, name);
    return block;
}

void init_yield()
{
    ControlBlock self;
    self.cr3 = page::flatPage->cr3();
    self.state = Running;
    currentTask = &self;

    switchToTask(readyTask.pop_front());
}

void append_task(ControlBlock* task)
{
    ScheduleLockGuard guard;

    readyTask.push_back(task);
}

void schedule()
{
    ScheduleLockGuard guard;

    if (!readyTask.empty()) {
        auto next = readyTask.pop_front();

        if (currentTask->state == Running) {
            readyTask.push_back(currentTask);
            currentTask->state = Pending;
        }
        else {
            if (currentTask->block_reason == BlockReason_Sleep) {
                for (auto it = sleepingTask.begin(); it != sleepingTask.end(); it++) {
                    if (it->sleep > currentTask->sleep) {
                        sleepingTask.insert(it, currentTask);
                        goto inserted;
                    }
                }
                sleepingTask.push_back(currentTask);
inserted:;
            }
        }

        next->state = Running;

        kernel::io::kprint("switch to %s\n", next->name);
        switchToTask(next);
    }
}

void block(ControlBlock* task, uint32_t reason)
{
    ScheduleLockGuard guard;

    currentTask->state = reason;
    schedule();
}

void unblock(ControlBlock* task)
{
    ScheduleLockGuard guard;

    auto it = std::find(sleepingTask.begin(), sleepingTask.end(), task);
    sleepingTask.erase(it);
    task->state = Pending;
    readyTask.push_back(task);
}

void sleep(uint32_t ms)
{
    currentTask->sleep = timer::msSinceBoot + ms;
    block(currentTask);
}

void hit()
{
    while (!sleepingTask.empty()) {
        auto task = sleepingTask.front();
        if (task->sleep <= timer::msSinceBoot) {
            sleepingTask.pop_front();
            unblock(task);
        }
        else {
            break;
        }
    }
}

void dump_ready()
{
    ScheduleLockGuard guard;

    kernel::io::kprint("dump ready:\n");
    for (auto task : readyTask) {
        kernel::io::kprint("%s, ", task->name);
    }
    kernel::io::kprint("\n");
}

}

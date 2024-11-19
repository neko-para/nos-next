#include "lib/task.hpp"
#include "lib/alloc.hpp"
#include "lib/page.hpp"

kernel::task::ControlBlock* currentTask;

namespace kernel::task
{

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

ControlBlock* make_kernel_task(void (*entry)(uint32_t param), uint32_t cr3, uint32_t param)
{
    ControlBlock* block = new ControlBlock;
    block->esp = make_stack(reinterpret_cast<uint32_t>(entry), param);
    block->cr3 = cr3;
    block->kesp = (block->esp & (alloc::FrameAllocator::frameSize - 1)) + (1 << 10); // 1K kernel stack of 4K stack
    block->state = Pending;
    return block;
}

void init(ControlBlock* initTask)
{
    ControlBlock self;
    self.cr3 = page::flatPage->cr3();
    self.state = Running;

    currentTask = &self;
    switchToTask(initTask);
}

}

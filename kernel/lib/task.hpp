#pragma once

#include <stdint.h>

namespace kernel::task
{

enum
{
    Running = 0,
    Pending = 1,
    Blocked = 2,

    BlockReason_Sleep = 1,
};

struct ControlBlock
{
    uint32_t esp;
    uint32_t cr3;
    uint32_t kesp;

    struct
    {
        uint32_t state: 2;
        uint32_t block_reason: 2;
    };

    ControlBlock* next;

    char name[16];

    union
    {
        uint32_t sleep;
    };
};

void init_yield();
ControlBlock* make_kernel_task(void (*entry)(uint32_t param), uint32_t cr3, uint32_t param, const char* name = "untitled");

void append_task(ControlBlock* task);

void schedule();
void block(ControlBlock* task, uint32_t reason = Blocked);
void unblock(ControlBlock* task);
void sleep(uint32_t ms);

void hit(); // timer

void dump_ready();

}

extern "C" void switchToTask(kernel::task::ControlBlock* target);
extern kernel::task::ControlBlock* currentTask;

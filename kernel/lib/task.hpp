#pragma once

#include <stdint.h>

namespace kernel::task
{

enum
{
    Pending,

    Running,
    Blocked,
};

struct ControlBlock
{
    uint32_t esp;
    uint32_t cr3;
    uint32_t kesp;
    uint32_t state;
    ControlBlock* next;
};

void init(ControlBlock* initTask);
ControlBlock* make_kernel_task(void (*entry)(uint32_t param), uint32_t cr3, uint32_t param);

}

extern "C" void switchToTask(kernel::task::ControlBlock* target);
extern kernel::task::ControlBlock* currentTask;

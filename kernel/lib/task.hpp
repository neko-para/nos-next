#pragma once

#include <stdint.h>

namespace kernel::task
{

struct ControlBlock
{
    uint32_t esp;
    uint32_t cr3;
    uint32_t kesp;
    uint32_t state;
    ControlBlock* next;
};

void init();

}

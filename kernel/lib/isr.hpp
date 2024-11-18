#pragma once

#include <stdint.h>

namespace kernel::isr
{

extern uint32_t isrHandler[32];
extern uint32_t isrHandler32; // timer
extern uint32_t isrHandler33; // keyboard

}

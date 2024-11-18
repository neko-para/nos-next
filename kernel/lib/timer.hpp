#pragma once

#include <stdint.h>

namespace kernel::timer
{

extern uint64_t msSinceBoot;

void init(uint32_t hz = 1000);
void hit();

}

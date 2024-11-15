#pragma once

#include <stdint.h>

namespace kernel::idt
{

struct Idt
{
    enum
    {
        A_PRESENT = 1 << 7,
        A_DPL_0 = 0,
        A_DPL_1 = 1 << 5,
        A_DPL_2 = 1 << 6,
        A_DPL_3 = 3 << 5,
        A_GATE_TASK = 0x5,
        A_GATE_INT_16 = 0x6,
        A_GATE_TRAP_16 = 0x7,
        A_GATE_INT_32 = 0xE,
        A_GATE_TRAP_32 = 0xF
    };

    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t _;
    uint8_t attribute;
    uint16_t isr_high;

    void set(uint32_t isr, uint8_t attr)
    {
        isr_low = isr & 0xFFFF;
        kernel_cs = 0x08;
        _ = 0;
        attribute = attr;
        isr_high = (isr >> 16) & 0xFFFF;
    }
};

struct InterruptFrame
{
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t sp;
    uint32_t ss;
};

void init();

void mask(uint8_t irq);
void unmask(uint8_t irq);
void end(uint8_t irq);

}

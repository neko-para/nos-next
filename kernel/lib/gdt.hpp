#pragma once

#include <stdint.h>

namespace kernel::gdt
{

struct Tss
{
    uint16_t link;
    uint16_t link_h;
    uint32_t esp0;
    uint16_t ss0;
    uint16_t ss0_h;
    uint32_t esp1;
    uint16_t ss1;
    uint16_t ss1_h;
    uint32_t esp2;
    uint16_t ss2;
    uint16_t ss2_h;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint16_t es;
    uint16_t es_h;
    uint16_t cs;
    uint16_t cs_h;
    uint16_t ss;
    uint16_t ss_h;
    uint16_t ds;
    uint16_t ds_h;
    uint16_t fs;
    uint16_t fs_h;
    uint16_t gs;
    uint16_t gs_h;
    uint16_t ldt;
    uint16_t ldt_h;
    uint16_t trap;
    uint16_t iomap;
} __attribute__((packed));

struct Gdt
{
    enum
    {
        A_ACCESS = 1 << 0,
        A_READ_WRITE = 1 << 1,
        A_DIRECTION_CONFORMING = 1 << 2,
        A_EXECUTABLE = 1 << 3,
        A_NOT_SYSTEM = 1 << 4,
        A_DPL_0 = 0,
        A_DPL_1 = 1 << 5,
        A_DPL_2 = 1 << 6,
        A_DPL_3 = 3 << 5,
        A_PRESENT = 1 << 7,

        F_LONG = 1 << 1,
        F_SIZE = 1 << 2,
        F_GRANULARITY = 1 << 3
    };

    uint16_t limit_lo;
    uint16_t base_lo;
    uint8_t base_mi;
    uint8_t access;
    uint8_t limit_hi: 4;
    uint8_t flag: 4;
    uint8_t base_hi;

    void setBase(uint32_t base)
    {
        base_lo = base & 0xFFFF;
        base_mi = (base >> 16) & 0xFF;
        base_hi = (base >> 24) & 0xFF;
    }

    void setLimit(uint32_t limit)
    {
        limit_lo = limit & 0xFFFF;
        limit_hi = (limit >> 16) & 0xF;
    }

    void set(uint32_t base, uint32_t limit, uint8_t access, uint8_t flag)
    {
        setBase(base);
        setLimit(limit);
        this->access = access;
        this->flag = flag;
    }
};

void init();

}

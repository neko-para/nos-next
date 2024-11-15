#include "lib/gdt.hpp"

#include <new>

namespace kernel::gdt
{

struct Gdtr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static Tss tss;
static uint8_t gdtData[sizeof(Gdt) * 6];
static Gdtr gdtr;

void init()
{
    tss.ss0 = 0x10;

    auto gdt = new (gdtData) Gdt[6];
    gdt[0].set(0, 0, 0, 0);
    gdt[1].set(
        0,
        0x000FFFFF,
        Gdt::A_PRESENT | Gdt::A_DPL_0 | Gdt::A_NOT_SYSTEM | Gdt::A_EXECUTABLE | Gdt::A_READ_WRITE,
        Gdt::F_GRANULARITY | Gdt::F_SIZE);
    gdt[2].set(0, 0x000FFFFF, Gdt::A_PRESENT | Gdt::A_DPL_0 | Gdt::A_NOT_SYSTEM | Gdt::A_READ_WRITE, Gdt::F_GRANULARITY | Gdt::F_SIZE);
    gdt[3].set(
        0,
        0x000FFFFF,
        Gdt::A_PRESENT | Gdt::A_DPL_3 | Gdt::A_NOT_SYSTEM | Gdt::A_EXECUTABLE | Gdt::A_READ_WRITE,
        Gdt::F_GRANULARITY | Gdt::F_SIZE);
    gdt[4].set(0, 0x000FFFFF, Gdt::A_PRESENT | Gdt::A_DPL_3 | Gdt::A_NOT_SYSTEM | Gdt::A_READ_WRITE, Gdt::F_GRANULARITY | Gdt::F_SIZE);
    gdt[5].set(reinterpret_cast<uint32_t>(&tss), sizeof(Tss), Gdt::A_PRESENT | Gdt::A_EXECUTABLE | Gdt::A_ACCESS, 0);

    gdtr.limit = sizeof(gdtData) - 1;
    gdtr.base = reinterpret_cast<uint32_t>(gdtData);

    asm volatile("lgdt %0" ::"m"(gdtr));

    asm volatile(
        "ljmp $0x08, $1f\n"
        "1:\n"
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        :
        :
        : "ax");

    // asm volatile ( "movw $0x28, %ax; ltr %ax;" );
}

}

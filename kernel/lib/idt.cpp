#include "lib/idt.hpp"

#include "lib/io.hpp"
#include "lib/isr.hpp"

using kernel::io::inb;
using kernel::io::outb;

namespace kernel::idt
{

constexpr uint32_t PIC1 = 0x20;
constexpr uint32_t PIC2 = 0xA0;
constexpr uint32_t PIC1_COMMAND = PIC1;
constexpr uint32_t PIC1_DATA = (PIC1 + 1);
constexpr uint32_t PIC2_COMMAND = PIC2;
constexpr uint32_t PIC2_DATA = (PIC2 + 1);

constexpr uint32_t PIC1_ICW1 = 0x0020;
constexpr uint32_t PIC1_OCW2 = 0x0020;
constexpr uint32_t PIC1_IMR = 0x0021;
constexpr uint32_t PIC1_ICW2 = 0x0021;
constexpr uint32_t PIC1_ICW3 = 0x0021;
constexpr uint32_t PIC1_ICW4 = 0x0021;
constexpr uint32_t PIC2_ICW1 = 0x00a0;
constexpr uint32_t PIC2_OCW2 = 0x00a0;
constexpr uint32_t PIC2_IMR = 0x00a1;
constexpr uint32_t PIC2_ICW2 = 0x00a1;
constexpr uint32_t PIC2_ICW3 = 0x00a1;
constexpr uint32_t PIC2_ICW4 = 0x00a1;

constexpr uint32_t ICW1_ICW4 = 0x01;
constexpr uint32_t ICW1_SINGLE = 0x02;
constexpr uint32_t ICW1_INTERVAL4 = 0x04;
constexpr uint32_t ICW1_LEVEL = 0x08;
constexpr uint32_t ICW1_INIT = 0x10;

constexpr uint32_t ICW4_8086 = 0x01;
constexpr uint32_t ICW4_AUTO = 0x02;
constexpr uint32_t ICW4_BUF_SLAVE = 0x08;
constexpr uint32_t ICW4_BUF_MASTER = 0x0C;
constexpr uint32_t ICW4_SFNM = 0x10;

constexpr uint32_t PIC_READ_IRR = 0x0a;
constexpr uint32_t PIC_READ_ISR = 0x0b;

struct Idtr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

__attribute__((aligned(0x10))) static Idt idt[256];
static Idtr idtr;

void init()
{
    outb(PIC1_IMR, 0xff); // disable interrupts
    outb(PIC2_IMR, 0xff);

    outb(PIC1_ICW1, 0x11);   // edge
    outb(PIC1_ICW2, 0x20);   // remap 0 ~ 7 to 0x20 ~ 0x27
    outb(PIC1_ICW3, 1 << 2); // IRQ2
    outb(PIC1_ICW4, 0x01);   // no buffer

    outb(PIC2_ICW1, 0x11);
    outb(PIC2_ICW2, 0x28); // remap 8 ~ 15 to 0x28 ~ 0x2f
    outb(PIC2_ICW3, 2);
    outb(PIC2_ICW4, 0x01);

    idtr.limit = sizeof(idt) - 1;
    idtr.base = reinterpret_cast<uint32_t>(idt);

    for (uint32_t i = 0; i < 32; i++) {
        idt[i].set(kernel::isr::isrHandler[i], Idt::A_PRESENT | Idt::A_GATE_INT_32 | Idt::A_DPL_0);
    }

    asm volatile("lidt %0" ::"m"(idtr));
}

void mask(uint8_t irq)
{
    uint16_t port;
    if (irq < 8) {
        port = PIC1_DATA;
    }
    else {
        port = PIC2_DATA;
        irq -= 8;
    }
    outb(port, inb(port) | (1 << irq));
}

void unmask(uint8_t irq)
{
    uint16_t port;
    if (irq < 8) {
        port = PIC1_DATA;
    }
    else {
        port = PIC2_DATA;
        irq -= 8;
    }
    outb(port, inb(port) & ~(1 << irq));
}

void end(uint8_t irq)
{
    if (irq >= 8) {
        outb(PIC2_COMMAND, 0x20);
    }
    outb(PIC1_COMMAND, 0x20);
}

}

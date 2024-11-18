#include "lib/isr.hpp"

#include "lib/idt.hpp"
#include "lib/io.hpp"
#include "lib/keyboard.hpp"
#include "lib/timer.hpp"

namespace kernel::isr
{

template <size_t ISR>
__attribute__((interrupt)) void handlerError(kernel::idt::InterruptFrame*, uint32_t error)
{
    if constexpr (ISR == 13) { // GPF
        uint8_t external = error & 1;
        uint8_t table = (error >> 1) & 3;
        uint16_t selector = (error >> 3) & ((1 << 13) - 1);
        kernel::io::kprint(
            "%s%s%u",
            external ? "external " : "",
            table == 0                 ? "GDT "
            : table == 1 || table == 3 ? "IDT "
                                       : "LDT",
            selector);
    }

    KFatal("error: exception isr %u, error: %u", ISR, error);
}

template <size_t ISR>
__attribute__((interrupt)) void handlerNoError(kernel::idt::InterruptFrame*)
{
    KFatal("error: exception isr %u", ISR);
}

__attribute__((interrupt)) void handler32(kernel::idt::InterruptFrame*)
{
    kernel::idt::end(0);
    kernel::timer::hit();
}

__attribute__((interrupt)) void handler33(kernel::idt::InterruptFrame*)
{
    kernel::idt::end(1);
    uint8_t ch = kernel::io::inb(0x60);
    kernel::keyboard::push(ch);
}

uint32_t isrHandler[32] = {
    reinterpret_cast<uint32_t>(&handlerNoError<0>),  reinterpret_cast<uint32_t>(&handlerNoError<1>),
    reinterpret_cast<uint32_t>(&handlerNoError<2>),  reinterpret_cast<uint32_t>(&handlerNoError<3>),
    reinterpret_cast<uint32_t>(&handlerNoError<4>),  reinterpret_cast<uint32_t>(&handlerNoError<5>),
    reinterpret_cast<uint32_t>(&handlerNoError<6>),  reinterpret_cast<uint32_t>(&handlerNoError<7>),
    reinterpret_cast<uint32_t>(&handlerError<8>),    reinterpret_cast<uint32_t>(&handlerNoError<9>),
    reinterpret_cast<uint32_t>(&handlerError<10>),   reinterpret_cast<uint32_t>(&handlerError<11>),
    reinterpret_cast<uint32_t>(&handlerError<12>),
    reinterpret_cast<uint32_t>(&handlerError<13>), // generic protection fault
    reinterpret_cast<uint32_t>(&handlerError<14>), // page fault
    reinterpret_cast<uint32_t>(&handlerNoError<15>), reinterpret_cast<uint32_t>(&handlerNoError<16>),
    reinterpret_cast<uint32_t>(&handlerError<17>),   reinterpret_cast<uint32_t>(&handlerNoError<18>),
    reinterpret_cast<uint32_t>(&handlerNoError<19>), reinterpret_cast<uint32_t>(&handlerNoError<20>),
    reinterpret_cast<uint32_t>(&handlerError<21>),   reinterpret_cast<uint32_t>(&handlerNoError<22>),
    reinterpret_cast<uint32_t>(&handlerNoError<23>), reinterpret_cast<uint32_t>(&handlerNoError<24>),
    reinterpret_cast<uint32_t>(&handlerNoError<25>), reinterpret_cast<uint32_t>(&handlerNoError<26>),
    reinterpret_cast<uint32_t>(&handlerNoError<27>), reinterpret_cast<uint32_t>(&handlerNoError<28>),
    reinterpret_cast<uint32_t>(&handlerError<29>),   reinterpret_cast<uint32_t>(&handlerError<30>),
    reinterpret_cast<uint32_t>(&handlerNoError<31>),
};

uint32_t isrHandler32 = reinterpret_cast<uint32_t>(&handler32);
uint32_t isrHandler33 = reinterpret_cast<uint32_t>(&handler33);

}

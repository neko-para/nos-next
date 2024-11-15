#include "lib/isr.hpp"

#include "lib/idt.hpp"
#include "lib/io.hpp"

namespace kernel::isr
{

template <size_t ISR>
__attribute__((interrupt)) void handlerError(kernel::idt::InterruptFrame*, uint32_t error)
{
    KFatal("error: exception isr %u, error: %u", ISR, error);
}

template <size_t ISR>
__attribute__((interrupt)) void handlerNoError(kernel::idt::InterruptFrame*)
{
    KFatal("error: exception isr %u", ISR);
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

}

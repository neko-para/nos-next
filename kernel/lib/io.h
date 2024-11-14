#pragma once

#include <stdint.h>

namespace kernel::io
{

inline void outb(uint16_t port, uint8_t data)
{
    asm volatile("outb %0, %1" ::"a"(data), "Nd"(port));
}

inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void kprint(const char* format, ...) __attribute__((format(printf, 1, 2)));

}

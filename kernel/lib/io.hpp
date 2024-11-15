#pragma once

#include <stddef.h>
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
void kfatal(const char* file, size_t line, const char* format, ...) __attribute__((format(printf, 3, 4))) __attribute__((noreturn));

}

#define KFatal(format, ...) kernel::io::kfatal(__FILE__, __LINE__, format, ##__VA_ARGS__)

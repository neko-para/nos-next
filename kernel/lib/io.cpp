#include "lib/io.hpp"

#include <printf/printf.h>

namespace kernel::io
{

void kprint(const char* format, ...)
{
    va_list va;
    va_start(va, format);
    vfctprintf([](char ch, void*) { outb(0xe9, ch); }, nullptr, format, va);
    va_end(va);
}

void kfatal(const char* file, size_t line, const char* format, ...)
{
    kprint("fatal: %s %zd\n", file, line);

    va_list va;
    va_start(va, format);
    vfctprintf([](char ch, void*) { outb(0xe9, ch); }, nullptr, format, va);
    va_end(va);
    for (;;) {
        asm volatile("hlt");
    }
}

}

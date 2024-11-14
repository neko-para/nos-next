#include "lib/io.h"

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

}

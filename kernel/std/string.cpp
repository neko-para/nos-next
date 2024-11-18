#include "std/string.hpp"

extern "C" void* memmove(void* dst, const void* src, size_t size)
{
    return __builtin_memmove(dst, src, size);
}

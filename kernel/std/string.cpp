#include "std/string.hpp"

extern "C" void* memmove(void* dst, const void* src, size_t size)
{
    return __builtin_memmove(dst, src, size);
}

extern "C" char* strcpy(char* dst, const char* src)
{
    while ((*dst++ = *src++)) {
        ;
    }
    return dst;
    // return __builtin_strcpy(dst, src);
}

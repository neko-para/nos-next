#pragma once

#include <stdint.h>

namespace kernel::vga
{

enum class Color : uint8_t
{
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHT_GREY = 7,
    DARK_GREY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_MAGENTA = 13,
    LIGHT_BROWN = 14,
    WHITE = 15,
};

struct Attr
{
    Color fore: 4;
    Color back: 4;
};

struct Entry
{
    char ch;
    Attr attr;
};

void setAttr(Attr attr);

void put(char ch);

void print(const char* format, ...) __attribute__((format(printf, 1, 2)));

} // namespace kernel::vga

#include "lib/vga.hpp"

#include "lib/cpp.hpp"
#include "lib/io.hpp"
#include "std/string.hpp"

#include <printf/printf.h>

using kernel::io::inb;
using kernel::io::outb;

namespace kernel::vga
{

struct Buffer
{
    constexpr static uintptr_t address = 0xB8000;
    constexpr static size_t width = 80;
    constexpr static size_t height = 25;

    Entry data[width * height];

    Entry& at(int row, int col) { return data[row * width + col]; }

    void move(int& row, int& col)
    {
        col += 1;
        if (col == width) {
            col = 0;
            row += 1;
        }
    }

    void check_scroll(int& row)
    {
        if (row == height) {
            // why can't use this?
            // std::move(data + width, data + height * width, data);
            for (uint32_t i = 1; i < height; i++) {
                for (uint32_t j = 0; j < width; j++) {
                    at(i - 1, j) = at(i, j);
                }
            }
            row -= 1;
        }
    }

    Buffer() { std::fill(data, data + width * height, Entry { ' ', { Color::LIGHT_GREY, Color::BLACK } }); }
};

static Buffer* buffer_ = new (reinterpret_cast<void*>(Buffer::address)) Buffer();
static Attr attr_ = { Color::LIGHT_GREY, Color::BLACK };
static int row_ptr_ = 0;
static int col_ptr_ = 0;

void setAttr(Attr attr)
{
    attr_ = attr;
}

void put(char ch)
{
    switch (ch) {
    case '\n':
        row_ptr_ += 1;
        col_ptr_ = 0;
        buffer_->check_scroll(row_ptr_);
        break;
    case '\r':
        col_ptr_ = 0;
        break;
    case '\b':
        col_ptr_ = col_ptr_ == 0 ? 0 : col_ptr_ - 1;
        break;
    case '\t':
        col_ptr_ = (col_ptr_ + 4) & (~3);
        break;
    default:
        buffer_->at(row_ptr_, col_ptr_) = { ch, attr_ };
        buffer_->move(row_ptr_, col_ptr_);
        buffer_->check_scroll(row_ptr_);
        break;
    }
    update_cursor(row_ptr_, col_ptr_);
}

void print(const char* format, ...)
{
    va_list va;
    va_start(va, format);
    vfctprintf([](char ch, void*) { put(ch); }, nullptr, format, va);
    va_end(va);
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void disable_cursor()
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void update_cursor(uint8_t r, uint8_t c)
{
    uint16_t pos = r * Buffer::width + c;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

}

void putchar_(char c)
{
    kernel::vga::put(c);
}

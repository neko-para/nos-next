#include "lib/vga.hpp"

#include <printf/printf.h>

#include "lib/new.hpp"

namespace kernel::vga
{

struct Buffer
{
    constexpr static uintptr_t address = 0xB8000;
    constexpr static size_t width = 80;
    constexpr static size_t height = 25;

    Entry data[width * height];

    Entry& at(int row, int col) { return data[row * width + col]; }

    static void move(int& row, int& col)
    {
        col += 1;
        if (col == width) {
            col = 0;
            row += 1;
        }
    }

    // just use auto zero filling
    // Buffer() {}
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
        Buffer::move(row_ptr_, col_ptr_);
        break;
    }
}

void print(const char* format, ...)
{
    va_list va;
    va_start(va, format);
    vfctprintf([](char ch, void*) { put(ch); }, nullptr, format, va);
    va_end(va);
}

}

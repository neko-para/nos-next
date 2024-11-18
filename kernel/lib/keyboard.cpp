#include "lib/keyboard.hpp"

#include "lib/io.hpp"
#include "lib/vga.hpp"

namespace kernel::keyboard
{

static const char SC_to_char[] =
    "\0\0\0\0001!2@3#4$5%6^7&8*9(0)-_=+\b\b"
    "\t\tqQwWeErRtTyYuUiIoOpP[{]}\n\n"
    "\0\0aAsSdDfFgGhHjJkKlL;:'\"`~\0\0\\|"
    "zZxXcCvVbBnNmM,<.>/?\0\0**"
    "\0\0  \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "778899--445566++11223300..\0";

void Message::from(uint16_t scan)
{
    static uint8_t state = 0;

    code = scan;
    flag = 0;
    ch = 0;
    key = scan & 0xFF7F;
    if (scan & 0x80) {
        flag |= RELEASE;
        scan -= 0x80;
    }
    if (scan >> 8) { // 0xE0
        if (scan == SC_KP_SLASH) {
            ch = '/';
        }
    }
    else {
        if (scan < sizeof(SC_to_char) / 2) {
            ch = SC_to_char[scan * 2 + (state & SHIFT ? 1 : 0)];
        }
    }
    switch (scan) {
    case SC_LCTRL:
        if (flag & RELEASE) {
            state &= ~CTRL;
        }
        else {
            state |= CTRL;
        }
        break;
    case SC_LSHIFT:
    case SC_RSHIFT:
        if (flag & RELEASE) {
            state &= ~SHIFT;
        }
        else {
            state |= SHIFT;
        }
        break;
    case SC_LALT:
        if (flag & RELEASE) {
            state &= ~ALT;
        }
        else {
            state |= ALT;
        }
        break;
    }
    flag |= state;
}

void waitReady()
{
    while (kernel::io::inb(0x64) & 0x2) {
        ;
    }
}

void init()
{
    waitReady();
    kernel::io::outb(0x64, 0x60);
    waitReady();
    kernel::io::outb(0x60, 0x47);
}

bool push(uint8_t dat)
{
    static int state = 0;

    Message msg;

    switch (state) {
    case 0:
        if (dat == 0xE0) {
            state = 1;
            return false;
        }
        else {
            msg.from(dat);
            if (!(msg.flag & RELEASE)) {
                kernel::vga::print("%c", msg.ch);
            }
            return true;
        }
    case 1:
        msg.from(0xE0 << 8 | dat);
        state = 0;
        if (!(msg.flag & RELEASE)) {
            kernel::vga::print("%c", msg.ch);
        }
        return true;
    default:
        return false;
    }
}

}

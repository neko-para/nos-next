#include "lib/timer.hpp"
#include "lib/io.hpp"
#include "lib/vga.hpp"

namespace kernel::timer
{

constexpr uint32_t defaultHz = 1'193'182;

uint64_t msSinceBoot;

void init(uint32_t hz)
{
    using kernel::io::outb;

    uint16_t h = defaultHz / hz;
    outb(0x43, 0x34);
    outb(0x40, h & 0xFF);
    outb(0x40, h >> 8);
}

void hit()
{
    msSinceBoot += 1;

    if (msSinceBoot % 1000 == 0) {
        vga::print(".");
    }
}

}

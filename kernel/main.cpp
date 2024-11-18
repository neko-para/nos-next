#include "lib/alloc.hpp"
#include "lib/cpp.hpp"
#include "lib/gdt.hpp"
#include "lib/idt.hpp"
#include "lib/io.hpp"
#include "lib/keyboard.hpp"
#include "lib/vga.hpp"

#include <stddef.h>

extern "C" void kernel_main(void)
{
    kernel::gdt::init();
    kernel::idt::init();
    kernel::alloc::init();

    kernel::keyboard::init();

    kernel::idt::unmask(1);

    asm volatile("sti");

    std::vector<const char*> msgs = { "Hello world!", "NOS here", "Print via for loop" };

    for (auto msg : msgs) {
        kernel::vga::print("%s\n", msg);
    }

    kernel::io::kprint("kernel_main quit");

    for (;;) {
        asm volatile("hlt");
    }
}

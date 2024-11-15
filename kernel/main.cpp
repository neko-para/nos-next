#include "lib/alloc.hpp"
#include "lib/cpp.hpp"
#include "lib/gdt.hpp"
#include "lib/io.hpp"
#include "lib/vga.hpp"

#include <stddef.h>

extern "C" void kernel_main(void)
{
    kernel::gdt::init();

    kernel::alloc::init();

    std::vector<const char*> msgs = { "Hello world!", "NOS here", "Print via for loop" };

    for (auto msg : msgs) {
        kernel::vga::print("%s\n", msg);
    }

    kernel::io::kprint("kernel_main quit");
}

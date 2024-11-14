#include "lib/io.hpp"
#include "lib/vga.hpp"

extern "C" void kernel_main(void)
{
    kernel::vga::print("%s", "Hello, kernel World!\nNOS here!");

    kernel::io::kprint("kernel_main quit");
}

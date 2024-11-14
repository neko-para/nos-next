#include "lib/io.h"
#include "lib/vga.h"

extern "C" void kernel_main(void)
{
    kernel::vga::print("%s", "Hello, kernel World!\nNOS here!");

    kernel::io::kprint("kernel_main quit");
}

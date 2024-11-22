#include "lib/alloc.hpp"
#include "lib/cpp.hpp"
#include "lib/gdt.hpp"
#include "lib/idt.hpp"
#include "lib/io.hpp"
#include "lib/keyboard.hpp"
#include "lib/page.hpp"
#include "lib/task.hpp"
#include "lib/timer.hpp"
#include "lib/vga.hpp"

#include <stddef.h>

template <int I>
void task(uint32_t param)
{
    kernel::task::unlock_schedule();

    kernel::vga::print("task %d started, param %u\n", I, param);

    for (;;) {
        kernel::task::sleep(1000);
        kernel::vga::print("task %d awake\n", I);
    }

    KFatal("Quit");
}

extern "C" void kernel_main(void)
{
    kernel::gdt::init();
    kernel::idt::init();
    kernel::alloc::init();

    kernel::keyboard::init();
    kernel::timer::init();

    kernel::idt::unmask(0);
    kernel::idt::unmask(1);

    asm volatile("sti");

    std::vector<const char*> msgs = { "Hello world!", "NOS here", "Print via for loop" };

    for (auto msg : msgs) {
        kernel::vga::print("%s\n", msg);
    }

    kernel::task::append_task(kernel::task::make_kernel_task(task<1>, kernel::page::flatPage->cr3(), 111, "task1"));
    kernel::task::append_task(kernel::task::make_kernel_task(task<2>, kernel::page::flatPage->cr3(), 222, "task2"));
    kernel::task::append_task(kernel::task::make_kernel_task(task<3>, kernel::page::flatPage->cr3(), 333, "task3"));
    kernel::task::init_yield();

    kernel::io::kprint("kernel_main quit");

    for (;;) {
        asm volatile("hlt");
    }
}

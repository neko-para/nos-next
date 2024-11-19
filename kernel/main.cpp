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

kernel::task::ControlBlock *task1, *task2;

void task_1(uint32_t param)
{
    kernel::vga::print("task 1 started, param %u\n", param);

    for (;;) {
        switchToTask(task2);
        kernel::vga::print("task 1 awake\n");
    }
}

void task_2(uint32_t param)
{
    kernel::vga::print("task 2 started, param %u\n", param);

    for (;;) {
        switchToTask(task1);
        kernel::vga::print("task 2 awake\n");
    }
}

extern "C" void kernel_main(void)
{
    kernel::gdt::init();
    kernel::idt::init();
    kernel::alloc::init();

    kernel::keyboard::init();
    kernel::timer::init();

    kernel::idt::unmask(1);

    asm volatile("sti");

    std::vector<const char*> msgs = { "Hello world!", "NOS here", "Print via for loop" };

    for (auto msg : msgs) {
        kernel::vga::print("%s\n", msg);
    }

    task1 = kernel::task::make_kernel_task(task_1, kernel::page::flatPage->cr3(), 111);
    task2 = kernel::task::make_kernel_task(task_2, kernel::page::flatPage->cr3(), 111);
    kernel::task::init(task1);

    kernel::io::kprint("kernel_main quit");

    for (;;) {
        asm volatile("hlt");
    }
}

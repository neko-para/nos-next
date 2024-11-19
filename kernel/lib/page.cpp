#include "lib/page.hpp"

#include "lib/alloc.hpp"
#include "lib/vga.hpp"

namespace kernel::page
{

void PageDirectory::freeTables()
{
    for (uint32_t i = 0; i < 1024; i++) {
        if (directory[i].attrib & PRESENT) {
            alloc::frameAllocator.free(directory[i].table());
        }
    }
}

PageDirectory* flatPage;

// PageDirectory* upperPage;

void init(uint32_t maxiAddr)
{
    flatPage = new (alloc::frameAllocator.alloc()) PageDirectory();
    uint32_t maxiIndex = (maxiAddr + (largeFrame - 1)) / largeFrame;
    for (uint32_t i = 0; i < maxiIndex; i++) {
        auto table = new (alloc::frameAllocator.alloc()) PageTable();
        for (uint32_t j = 0; j < 1024; j++) {
            table->directory[j] = { READWRITE | PRESENT, j, i };
        }
        flatPage->directory[i] = { READWRITE | PRESENT, table };
    }

    // upperPage = new (alloc::frameAllocator.alloc()) PageDirectory();
    // for (uint32_t i = 0; i < 256; i++) {
    //     auto tableLow = new (alloc::frameAllocator.alloc()) PageTable();
    //     auto tableHigh = new (alloc::frameAllocator.alloc()) PageTable();
    //     for (uint32_t j = 0; j < 1024; j++) {
    //         tableLow->directory[j] = { READWRITE | PRESENT, j, i };
    //         tableHigh->directory[j] = { READWRITE | PRESENT, j, i };
    //     }
    //     upperPage->directory[i] = { READWRITE | PRESENT, tableLow };
    //     upperPage->directory[i + 512] = { READWRITE | PRESENT, tableLow };
    // }

    flatPage->load();
    enable();
}

void enable()
{
    asm volatile(
        "movl %cr0, %eax\n"
        "orl $0x80000000, %eax\n"
        "movl %eax, %cr0\n");
}

}

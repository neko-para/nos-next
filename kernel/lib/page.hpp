#pragma once

#include <stdint.h>

namespace kernel::page
{

enum
{
    PRESENT = 1,
    READWRITE = 2,
    NON_SUPERVISOR = 4,
    PWT = 8,
    PCD = 16,
    ACCESSED = 32,
    DIRTY = 64,
};

constexpr uint32_t largeFrame = 1 << 22; // 4M
constexpr uint32_t smallFrame = 1 << 12; // 4K

template <typename Entry>
struct PageBase
{
    Entry directory[1024];

    uint32_t withAttrib(uint32_t attrib) const { return (reinterpret_cast<uint32_t>(directory) & (smallFrame - 1)) | attrib; }
};

struct PageTableEntry
{
    uint32_t attrib: 12 = 0;
    uint32_t addr4K: 10 = 0;
    uint32_t addr4M: 10 = 0;
};

struct PageTable : public PageBase<PageTableEntry>
{
};

struct PageDirectoryEntry
{
    uint32_t attrib: 12 = 0;
    uint32_t addr: 20 = 0;

    PageDirectoryEntry() = default;

    PageDirectoryEntry(uint32_t attr, PageTable* table)
        : attrib(attr)
        , addr(reinterpret_cast<uint32_t>(table) >> 12)
    {
    }

    PageTable* table() { return reinterpret_cast<PageTable*>(addr << 12); }
};

struct PageDirectory : public PageBase<PageDirectoryEntry>
{
    void freeTables();

    uint32_t cr3() const { return reinterpret_cast<uint32_t>(&directory); }

    void load() { asm volatile("movl %0, %%cr3" ::"a"(directory)); }
};

extern PageDirectory* flatPage;

void init(uint32_t maxiAddr);
void enable();

}

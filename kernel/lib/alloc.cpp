#include "lib/alloc.hpp"

#include "lib/boot.hpp"
#include "lib/io.hpp"
#include "lib/vga.hpp"

namespace kernel::alloc
{

void Allocator::insert(Range range)
{
    if (!usable) {
        usable = pool.acquire();
        *usable = { range, nullptr };
    }
    else if (!usable->next) {
        if (usable->isLeftOf(range)) {
            usable->mergeRight(range);
        }
        else if (usable->isRightOf(range)) {
            usable->mergeLeft(range);
        }
        else {
            auto node = pool.acquire();
            if (usable->isAwayLeftOf(range)) {
                usable->next = node;
                *node = { range, nullptr };
            }
            else if (usable->isAwayLeftOf(range)) {
                *node = { range, usable };
                usable = node;
            }
            else {
                KFatal("error: alloc: occupied insert detected");
            }
        }
    }
    else {
        if (usable->isRightOf(range)) {
            usable->mergeLeft(range);
        }
        else if (usable->isAwayRightOf(range)) {
            auto node = pool.acquire();
            *node = { range, usable };
            usable = node;
        }
        else {
            auto left = usable;
            auto right = usable->next;

            while (right) {
                if (!left->isAwayLeftOf(range)) {
                    KFatal("error: alloc: occupied insert detected");
                }
                if (left->isLeftOf(range)) {
                    left->mergeRight(range);
                    if (left->isLeftOf(*right)) {
                        left->mergeRight(*right);
                        left->next = right->next;
                        pool.release(right);
                    }
                    return;
                }
                else if (right->isRightOf(range)) {
                    right->mergeLeft(range);
                    return;
                }
                else if (right->isAwayRightOf(range)) {
                    auto node = pool.acquire();
                    *node = { range, right };
                    left->next = node;
                    return;
                }
                else {
                    left = right;
                    right = right->next;
                }
            }
            if (left->isLeftOf(range)) {
                left->mergeRight(range);
            }
            else if (left->isAwayLeftOf(range)) {
                auto node = pool.acquire();
                *node = { range, nullptr };
                left->next = node;
            }
            else {
                KFatal("error: alloc: occupied insert detected");
            }
        }
    }
}

void* Allocator::alloc(size_t size)
{
    size = (size + 3) & (~3);
    size += 8;

    auto prev = &usable;
    auto ptr = usable;
    while (ptr) {
        if (ptr->size >= size && ptr->size <= size + 8) {
            *prev = ptr->next;
            auto ret = ptr->ptr();
            pool.release(ptr);
            return ret;
        }
        prev = &ptr->next;
        ptr = ptr->next;
    }

    prev = &usable;
    ptr = usable;
    while (ptr) {
        if (ptr->size >= size) {
            return ptr->splitLeft(size).ptr();
        }
        prev = &ptr->next;
        ptr = ptr->next;
    }

    KFatal("error: alloc: no suitable memory");
}

void Allocator::free(void* ptr)
{
    auto uptr = static_cast<uint32_t*>(ptr) - 1;
    auto size = uptr[0];
    // if (uptr[-1] != magic) {
    //     KFatal("error: alloc: free block with wrong magic");
    // }
    auto tail = uptr[size / 4 - 1];
    if ((tail ^ size) != magic) {
        KFatal("error: alloc: free magic error");
    }
    insert({ reinterpret_cast<uintptr_t>(ptr) - 4, size });
}

static Allocator allocator;

void init()
{
    auto info = bootInfo;

    for (uint32_t i = 0; i < info->mmap_length / sizeof(kernel::boot::MMapEntry); i++) {
        kernel::boot::MMapEntry* me = info->mmap_addr + i;
        if (me->addr_hi || me->len_hi) {
            continue;
        }
        if (me->type == 1 && me->addr_lo > 0) { // skip first
            if (me->addr_lo == 0x100000) {
                vga::print("%u ~ %u, %u\n", me->addr_lo, me->addr_lo + me->len_lo, me->len_lo);

                // acually always one hole
                // use &image_size ~ 1M as small heap, 1M ~ 16M as frame heap, 16M ~ 1G as pages.
                uint32_t size = (uint32_t)&imageSize;
                me->addr_lo = (size + 0xFFF) & (~0xFFF);
                allocator.insert({ me->addr_lo, 0x1000000 - me->addr_lo });
                // 1M - 16M as frame
                // Frame::init(0x1000000, (me->len_lo - 0xF00000) >> 12);
                // Page::init(me->len_lo + 0x100000);
            }
        }
    }
}

}

void* operator new(size_t size)
{
    return kernel::alloc::allocator.alloc(size);
}

void* operator new[](size_t size)
{
    return kernel::alloc::allocator.alloc(size);
}

void operator delete(void* ptr)
{
    kernel::alloc::allocator.free(ptr);
}

void operator delete[](void* ptr)
{
    kernel::alloc::allocator.free(ptr);
}

void operator delete(void* ptr, size_t)
{
    kernel::alloc::allocator.free(ptr);
}

void operator delete[](void* ptr, size_t)
{
    kernel::alloc::allocator.free(ptr);
}

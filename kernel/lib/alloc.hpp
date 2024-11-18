#pragma once

#include <stddef.h>
#include <stdint.h>

#include "lib/cpp.hpp"
#include "lib/pool.hpp"

namespace kernel::alloc
{

struct Allocator
{
    constexpr static size_t nodeCount = 4096;
    constexpr static uint32_t magic = 0x00317839;

    struct Range
    {
        uintptr_t base, size;

        uintptr_t left() const { return base; }

        uintptr_t right() const { return base + size; }

        bool isLeftOf(const Range& range) const { return right() == range.left(); }

        bool isRightOf(const Range& range) const { return range.right() == left(); }

        bool isAwayLeftOf(const Range& range) const { return right() < range.left(); }

        bool isAwayRightOf(const Range& range) const { return range.right() < left(); }

        void mergeLeft(const Range& range)
        {
            base = range.base;
            size += range.size;
        }

        void mergeRight(const Range& range) { size += range.size; }

        void* ptr() const
        {
            auto p = reinterpret_cast<uint32_t*>(base);
            p[0] = size;
            p[size / 4 - 1] = size ^ magic;
            return p + 1;
        }

        Range splitLeft(size_t want)
        {
            Range ret { base, want };
            base += want;
            size -= want;
            return ret;
        }
    };

    using Pool = pool::StaticPool<Range, nodeCount>;
    using Node = Pool::Node;

    Pool pool;
    Node* usable = nullptr;

    void insert(Range range);
    void* alloc(size_t size);
    void free(void* ptr);

    template <size_t N>
    requires((N & 3) == 0)
    void insert(char (&data)[N])
    {
        insert(Range { reinterpret_cast<uintptr_t>(data), N });
    }
};

struct FrameAllocator
{
    constexpr static size_t frameSize = 1 << 12;

    struct Frame
    {
        uint8_t data[frameSize];
    };

    std::vector<bool> bitmap;
    Frame* baseFrame;

    void init(uint32_t base, uint32_t count);
    void* alloc(size_t skip = 0);
    void free(void* ptr);

    // 128MB
    void* allocUpper(uint32_t upperBase = 1 << 27)
    {
        auto skip = (upperBase - reinterpret_cast<uint32_t>(baseFrame)) / frameSize;
        return alloc(skip);
    }
};

extern Allocator allocator;
extern FrameAllocator frameAllocator;

void init();

}

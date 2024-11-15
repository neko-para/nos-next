#pragma once

#include <stddef.h>

#include "lib/io.hpp"

namespace kernel::pool
{

template <typename Type, size_t count>
struct StaticPool
{
    struct Node : public Type
    {
        Node* next;
    };

    Node pool[count];
    Node* free;

    StaticPool()
    {
        for (size_t i = 1; i < count; i++) {
            pool[i - 1].next = &pool[i];
        }
        pool[count - 1].next = nullptr;
        free = &pool[0];
    }

    Node* acquire()
    {
        if (!free) {
            KFatal("error: pool: no free node");
        }
        auto node = free;
        free = free->next;
        return node;
    }

    void release(Node* node)
    {
        node->next = free;
        free = node;
    }
};

}

#pragma once

#include <concepts>
#include <stddef.h>

#include "lib/cpp.hpp"

namespace kernel::list
{

template <typename Node>
concept HasNext = requires(Node* node) {
    { node->next } -> std::same_as<Node*&>;
};

template <HasNext Node>
struct ListHolder
{
    struct iterator
    {
        using difference_type = std::ptrdiff_t;
        using value_type = Node*;

        Node** node_ptr;

        bool operator==(iterator it) const { return node_ptr == it.node_ptr; }

        bool operator!=(iterator it) const { return node_ptr != it.node_ptr; }

        Node* operator*() const { return *node_ptr; }

        Node* operator->() const { return *node_ptr; }

        iterator& operator++()
        {
            node_ptr = &(*node_ptr)->next;
            return *this;
        }

        iterator operator++(int)
        {
            auto self = *this;
            node_ptr = &(*node_ptr)->next;
            return self;
        }
    };

    Node* begin_ = nullptr;
    Node* end_ = nullptr;

    void push_back(Node* node)
    {
        if (!begin_) {
            begin_ = end_ = node;
            node->next = nullptr;
        }
        else {
            end_->next = node;
            end_ = node;
        }
    }

    void push_front(Node* node)
    {
        if (!begin_) {
            begin_ = end_ = node;
            node->next = nullptr;
        }
        else {
            node->next = begin_;
            begin_ = node;
        }
    }

    Node* pop_front()
    {
        if (!begin_) {
            return nullptr;
        }
        else {
            auto node = begin_;
            if (begin_ == end_) {
                begin_ = end_ = nullptr;
            }
            else {
                begin_ = begin_->next;
            }
            return node;
        }
    }

    Node* front() const { return begin_; }

    Node* back() const { return end_; }

    bool empty() const { return !begin_; }

    iterator begin() { return { &begin_ }; }

    iterator end() { return { &end_->next }; }

    iterator insert(iterator it, Node* node)
    {
        node->next = *it.node_ptr;
        *it.node_ptr = node;
        return it;
    }

    iterator erase(iterator it)
    {
        // auto node = *it.node_ptr; this node just dropped
        *it.node_ptr = (*it.node_ptr)->next;
        return it;
    }

    iterator erase(iterator it, Node*& erased)
    {
        erased = *it.node_ptr;
        *it.node_ptr = (*it.node_ptr)->next;
        return it;
    }
};

}

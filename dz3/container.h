#ifndef CONTAINER_H
#define CONTAINER_H

#include <memory>
#include <iostream>

template<typename T, typename Allocator = std::allocator<T>>
class SimpleContainer{
public:
    struct Node
    {
        T data;
        Node* next = nullptr;
    };

    struct iterator{
        iterator(Node *node) : current(node) {}

        bool operator==(const iterator& iter) { return current == iter.current; }
        bool operator!=(const iterator& iter) { return !(*this == iter); }
        iterator& operator++()
        {
            current = current->next;
            return *this;
        }
        T & operator*() { return current->data; }
        T operator*() const { return current->data; }
    private:
        Node *current;
    };

    using node_alloc_t = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    node_alloc_t node_alloc;

    SimpleContainer() { }

    ~SimpleContainer()
    {
        // Destroy and deallocate all elements
        Node *next;
        while (head != nullptr)
        {
            next = head->next;
            node_alloc.destroy(head);
            node_alloc.deallocate(head, 1);
            head = next;
        }
    }

    void push(const T& elem)
    {
        // Allocate node and put elem into node
        Node *node = node_alloc.allocate(1);
        node_alloc.construct(node);
        node->data = elem;

        // Empty check and placing node in list
        if (head == nullptr)
            head = node;
        else
            last->next = node;
        last = node;
        size++;
    }

    iterator begin() { return iterator(head); }
    iterator begin() const { return iterator(head); }
    iterator end() { return iterator(nullptr); }
    iterator end() const { return iterator(nullptr); }

    Node *head = nullptr;
    size_t size;
    Node *last;
};

#endif // CONTAINER_H

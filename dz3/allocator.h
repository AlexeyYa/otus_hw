#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <utility>
#include <exception>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>

template<typename T, size_t T_per_page = 10>
struct SimpleAllocator {
    using value_type = T;

    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    template<typename U>
    struct rebind {
        using other = SimpleAllocator<U, T_per_page>;
    };

    struct FreeBlock {
        pointer p;
        pointer end;

        bool operator<(const SimpleAllocator::FreeBlock& rhs) const
        {
            return this->p < rhs.p;
        }
    };

    SimpleAllocator() {}
    template<typename U>
    SimpleAllocator(const SimpleAllocator<U>&) {}

    ~SimpleAllocator()
    {
            for (auto page : pages){
                std::free(page);
            }
            pages.erase(pages.begin(), pages.end());
    }

    // Returns iterator to new block
    auto new_page(size_t allocCount = T_per_page){
        auto p = std::malloc(allocCount * sizeof(T));
        if (!p)
            throw std::bad_alloc();

        // Push to pages
        pages.emplace_back(reinterpret_cast<pointer>(p));

        // Create free block for new page
        FreeBlock block{ reinterpret_cast<pointer>(p), reinterpret_cast<pointer>(p) + allocCount};
        auto result = free_blocks.emplace(block);
        if (result.second)
            return result.first;
        else
            throw std::bad_alloc();
    }

    T *allocate(size_t count)
    {
        // Find free block with equal or greater size
        auto block = std::find_if(free_blocks.begin(), free_blocks.end(), [count](FreeBlock elem)
                                                    { return count <= static_cast<size_t>(elem.end - elem.p); });
        // Failed to find
        if (block == free_blocks.end()){
            block = new_page( count > T_per_page ?
                      count : T_per_page);
        }

        // Take pointer and move to next free position
        auto new_block = *block;
        free_blocks.erase(block);
        auto ptr = new_block.p;
        new_block.p = new_block.p + count;
        if (new_block.p != new_block.end)
            free_blocks.emplace(new_block);

        return ptr;
    }

    void deallocate(T *p, size_t n){
        FreeBlock block{ p, p + n };
        // Search and merge free blocks
        auto it = free_blocks.upper_bound(block);
        // Check previous block
        if (it != free_blocks.begin())
        {
            auto prev_it = std::prev(it);
            if (prev_it->end == p)
            {
                block.p = prev_it->p;
                free_blocks.erase(prev_it);
            }
        }
        // Check next block
        if (it != free_blocks.end())
        {
            if (it->p == p + n)
            {
                block.end = it->end;
                free_blocks.erase(it);
            }
        }
        free_blocks.emplace(block);
    }

    template<typename U, typename ...Args>
    void construct(U *p, Args &&...args) {
        new(p) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U *p) {
        p->~U();
    }

    void reserve(size_t newSize){
        size_t totalSize = 0;
        for (auto page : pages)
            totalSize += page.size;

        // Create new page if not enough elements
        if (newSize > totalSize)
            new_page(newSize - totalSize);
    }

    std::vector<T*> pages;
    std::set<FreeBlock> free_blocks;
};

#endif

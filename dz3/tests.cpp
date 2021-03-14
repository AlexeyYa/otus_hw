#include "allocator.h"
#include "container.h"

#include <gtest/gtest.h>

TEST(Allocator, Creation) {
    // Arrange
    SimpleAllocator<int> allocator;

    // Act

    // Assert
}

TEST(Allocator, Allocation_one) {
    // Arrange
    SimpleAllocator<int> allocator;

    // Act
    auto ptr = allocator.allocate(1);

    // Assert
    ASSERT_NE(ptr, nullptr);
}

TEST(Allocator, Allocation_one_page) {
    // Arrange
    SimpleAllocator<int> allocator;

    // Act
    auto ptr = allocator.allocate(10);

    // Assert
    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(allocator.pages.size(), 1);
}

TEST(Allocator, Allocation_big_page) {
    // Arrange
    SimpleAllocator<int> allocator;

    // Act
    auto ptr = allocator.allocate(20);

    // Assert
    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(allocator.pages.size(), 1);
}

TEST(Allocator, Per_page_default) {
    // Arrange
    SimpleAllocator<int> allocator;

    // Act
    auto ptr = allocator.allocate(1);

    // Assert
    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(allocator.pages.size(), 1);
    ASSERT_EQ(allocator.free_blocks.begin()->end - allocator.free_blocks.begin()->p, 9);
}


TEST(Allocator, Per_page_custom) {
    // Arrange
    SimpleAllocator<int, 33> allocator;

    // Act
    auto ptr = allocator.allocate(1);

    // Assert
    ASSERT_NE(ptr, nullptr);
    ASSERT_EQ(allocator.pages.size(), 1);
    ASSERT_EQ(allocator.free_blocks.begin()->end - allocator.free_blocks.begin()->p, 32);
}

TEST(Allocator, Allocation_many_pages) {
    // Arrange
    SimpleAllocator<int> allocator;

    // Act
    auto ptr = allocator.allocate(10);
    auto ptr2 = allocator.allocate(10);

    // Assert
    ASSERT_NE(ptr, nullptr);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_EQ(allocator.pages.size(), 2);
}

TEST(Allocator, Allocation_not_full_pages) {
    // Arrange
    SimpleAllocator<int> allocator;

    // Act
    auto ptr = allocator.allocate(6);
    auto ptr2 = allocator.allocate(5);

    // Assert
    ASSERT_NE(ptr, nullptr);
    ASSERT_NE(ptr2, nullptr);
    ASSERT_EQ(allocator.pages.size(), 2);
}

TEST(Allocator, Deallocation) {
    // Arrange
    SimpleAllocator<int> allocator;

    // Act
    auto ptr = allocator.allocate(10);
    auto ptr2 = ptr + 1;
    allocator.deallocate(ptr2, 1);

    // Assert
    ASSERT_EQ(allocator.free_blocks.begin()->p, ptr2);
    ASSERT_EQ(allocator.free_blocks.begin()->end, ptr2 + 1);
}

TEST(Allocator, Deallocation_merge) {
    // Arrange
    SimpleAllocator<int> allocator;

    // Act
    auto ptr = allocator.allocate(10);
    auto ptr2 = ptr + 1;
    auto ptr3 = ptr + 2;
    auto ptr4 = ptr + 3;
    allocator.deallocate(ptr2, 1);
    allocator.deallocate(ptr4, 1);
    allocator.deallocate(ptr3, 1);

    // Assert
    ASSERT_EQ(allocator.free_blocks.begin()->p, ptr2);
    ASSERT_EQ(allocator.free_blocks.begin()->end, ptr2 + 3);
}

TEST(Allocator, Allocation_after_Deallocation) {
    // Arrange
    SimpleAllocator<int> allocator;

    // Act
    auto ptr = allocator.allocate(10);
    auto ptr2 = ptr + 1;
    allocator.deallocate(ptr2, 1);
    auto ptr3 = allocator.allocate(1);

    // Assert
    ASSERT_EQ(ptr2, ptr3);
}

TEST(Container, Creation) {
    // Arrange
    SimpleContainer<int> container;

    // Act

    // Assert
}

TEST(Container, Push) {
    // Arrange
    SimpleContainer<int> container;

    // Act
    container.push(0);

    // Assert
    ASSERT_EQ(container.head->data, 0);
}

TEST(Container, Creation_w_allocator) {
    // Arrange
    SimpleContainer<int, SimpleAllocator<int>> container;

    // Act

    // Assert
}

TEST(Container, Push_w_allocator) {
    // Arrange
    SimpleContainer<int, SimpleAllocator<int>> container;

    // Act
    container.push(0);

    // Assert
    ASSERT_EQ(container.head->data, 0);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*! @file tests.cpp
 *  @brief Тесты
 */

#include <gtest/gtest.h>
#include "bulk.h"
#include "cmd.h"
#include "libasync.h"
#include <sstream>


/*! @brief Тест на смену размера
 */
TEST(BULK, DefaultSize) {
    // Arrange
    std::mutex m;
    Bulk<Command> bulk(std::cout, m, 10, 4);

    // Act

    // Assert
    ASSERT_EQ(bulk.default_size, 4);
}

/*! @brief Тест на парсинг и вывод в консоль
 */
TEST(BULK, Parse) {
    // Arrange
    std::stringstream ss;
    {
        std::mutex m;
        Bulk<Command> bulk(ss, m, 11, 3);

    // Act
        bulk.Parse("a");
        bulk.Parse("b");
        bulk.Parse("c");
    }
    // Assert
    ASSERT_EQ("Bulk: a, b, c\n", ss.str());
}

/*
TEST(LIBASYNC, Connect) {
    // Arrange

    // Act
    auto handle = async::connect(3);

    // Assert
    // First handle value
    ASSERT_EQ(0, handle);
}

TEST(LIBASYNC, ConnectMultiple) {
    // Arrange

    // Act
    auto handle1 = async::connect(3);
    auto handle2 = async::connect(3);

    // Assert
    // Counter increased further
    ASSERT_EQ(1, handle1);
    ASSERT_EQ(2, handle2);
}

TEST(LIBASYNC, Recieve) {
    // Arrange
    std::stringstream ss;
    auto handle = async::connect(3, ss);

    // Act
    char src[4] = "abc";
    async::receive(src, 1, handle);
    async::receive(src + 1, 1, handle);
    async::receive(src + 2, 1, handle);
    // Waiting for output
    async::disconnect(handle);

    // Assert
    ASSERT_EQ("Bulk: a, b, c\n", ss.str());
}

TEST(LIBASYNC, RecieveMultiple) {
    // Arrange
    std::stringstream ss1;
    std::stringstream ss2;
    auto handle1 = async::connect(3, ss1);
    auto handle2 = async::connect(3, ss2);

    // Act
    char src1[4] = "abc";
    char src2[4] = "xyz";
    async::receive(src1, 1, handle1);
    async::receive(src2, 1, handle2);
    async::receive(src1 + 1, 1, handle1);
    async::receive(src2 + 1, 1, handle2);
    async::receive(src1 + 2, 1, handle1);
    async::receive(src2 + 2, 1, handle2);
    // Waiting for output
    async::disconnect(handle1);
    async::disconnect(handle2);

    // Assert
    ASSERT_EQ("Bulk: a, b, c\n", ss1.str());
    ASSERT_EQ("Bulk: x, y, z\n", ss2.str());
}

TEST(LIBASYNC, Disconnect) {
    // Arrange
    std::stringstream ss;
    auto handle = async::connect(3, ss);

    // Act
    char src[4] = "abc";
    async::receive(src, 1, handle);
    async::receive(src + 1, 1, handle);
    async::receive(src + 2, 1, handle);

    async::disconnect(handle);
    async::receive(src, 1, handle);
    async::receive(src + 1, 1, handle);
    async::receive(src + 2, 1, handle);
    // Assert
    ASSERT_EQ("Bulk: a, b, c\n", ss.str());
}
*/

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

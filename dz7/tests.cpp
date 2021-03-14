/*! @file tests.cpp
 *  @brief Тесты
 */

#include <gtest/gtest.h>
#include "bulk.h"
#include "cmd.h"



/*! @brief Тест на смену размера
 */
TEST(BULK, DefaultSize) {
    // Arrange
    Bulk<Command> bulk{4};

    // Act

    // Assert
    ASSERT_EQ(bulk.default_size, 4);
}

/*! @brief Тест на парсинг и вывод в консоль
 */
TEST(BULK, Parse) {
    // Arrange
    Bulk<Command> bulk{3};
    ::testing::internal::CaptureStdout();

    // Act
    bulk.Parse("a");
    bulk.Parse("b");
    bulk.Parse("c");

    // Assert
    ASSERT_EQ(::testing::internal::GetCapturedStdout(), "Bulk: a, b, c\n");
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

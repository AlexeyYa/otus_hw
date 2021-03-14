/*! @file tests.cpp
 *  @brief Тесты
 */

#include <gtest/gtest.h>
#include "bulk.h"
#include "cmd.h"
#include <sstream>


/*! @brief Тест на смену размера
 */
TEST(BULK, DefaultSize) {
    // Arrange
    Bulk<Command> bulk(std::cout, 4);

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
        Bulk<Command> bulk(ss, 3);

    // Act
        bulk.Parse("a");
        bulk.Parse("b");
        bulk.Parse("c");
    }
    // Assert
    // file output to either of threads
    bool result = (ss.str() == "Bulk: a, b, c\nthread\tline count\tcmd count\tblock count\nmain\t3\t\t3\t\t1\nlog\t\t\t3\t\t1\nfile0\t\t\t0\t\t0\nfile1\t\t\t3\t\t1\n"
                   || ss.str() == "Bulk: a, b, c\nthread\tline count\tcmd count\tblock count\nmain\t3\t\t3\t\t1\nlog\t\t\t3\t\t1\nfile0\t\t\t3\t\t1\nfile1\t\t\t0\t\t0\n");
    std::cout << ss.str();
    ASSERT_EQ(1, result);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

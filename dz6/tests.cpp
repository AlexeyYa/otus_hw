/*! @file tests.cpp
 *  @brief Тесты
 */

#include "matrix.h"

#include <gtest/gtest.h>
#include <vector>
#include <algorithm>

#define RANDOM_TEST_COUNT 1000

/*! @brief Тест значения по умолчанию
 */
TEST(MATRIX, DefaultValue) {
    // Arrange
	M2<int, -3> matrix;
    // Act

    // Assert
	for (size_t i = 0; i < RANDOM_TEST_COUNT; i++)
		ASSERT_EQ(matrix[rand()][rand()], -3);
}

/*! @brief Тест значения по умолчанию
 */
TEST(MATRIX, WriteRead) {
    // Arrange
	M2<int, -3> matrix;
    // Act

    // Assert
	for (size_t i = 0; i < RANDOM_TEST_COUNT; i++)
	{
		size_t x = rand();
		size_t y = rand();
		size_t val = rand();
		matrix[x][y] = val;
		ASSERT_EQ(matrix[x][y], val);
	}
}

/*! @brief Тест значения по умолчанию в частично заполненной матрице
 */
TEST(MATRIX, DefaultValueNonEmpty) {
    // Arrange
	M2<int, -3> matrix;
	std::vector<std::pair<size_t, size_t> > values;
    // Act
	for (size_t i = 0; i < RANDOM_TEST_COUNT; i++)
	{
		size_t x = rand();
		size_t y = rand();
		size_t val = rand();
		matrix[x][y] = val;
        values.push_back(std::pair<size_t, size_t>{x, y});
	}
    // Assert
	for (size_t i = 0; i < RANDOM_TEST_COUNT; i++)
	{
		size_t x = rand();
		size_t y = rand();
		if (std::find(values.begin(), values.end(), std::pair<size_t, size_t>{x, y}) != values.end())
			ASSERT_NE(matrix[x][y], -3);
		else
			ASSERT_EQ(matrix[x][y], -3);
	}
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

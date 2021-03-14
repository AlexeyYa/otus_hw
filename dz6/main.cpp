/*! @file main.cpp
 *  @brief Исполняемый файл
 */
 
#include "matrix.h"
#include <iostream>

int main()
{
    M2<int, 0> matrix;
    for (size_t i = 0; i < 10; i++)
    {
        matrix[i][i] = i;
    }
    for (size_t i = 0; i < 10; i++)
    {
        matrix[i][9-i] = 9 - i;
    }

    for (size_t i = 1; i < 9; i++)
    {
        for (size_t j = 1; j < 9; j++)
            std::cout << matrix[i][j] << ' ';
        std::cout << '\n';
    }

    std::cout << matrix.size() << std::endl;

    for (auto elem : matrix)
    {
        for (auto dim : elem.first)
            std::cout << dim << ' ';
        std::cout << elem.second << '\n';
    }

    return 0;
}

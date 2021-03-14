/*! @file main.cpp
 *  @brief Исполняемый файл
 */
 
#include "bulk.h"
#include "cmd.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    Bulk<Command> bulk{};
    if (argc == 2)
    {
        try {
            size_t size = std::stoi(argv[1]);
            bulk.default_size = size;
        } catch (...) {
            std::cout << "Invalid argument, starting with default bulk size(3)" << std::endl;
        }
    }
    std::string input;
    while (std::cin >> input)
    {
        bulk.Parse(input);
    }
    return 0;
}

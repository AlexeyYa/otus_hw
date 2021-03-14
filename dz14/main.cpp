/*! @file main.cpp
 *  @brief Исполняемый файл
 */
 
#include "yamr.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char** argv)
{

    if (argc != 4)
    {
        std::cerr << "Usage: yamr <src> <mnum> <rnum>" << std::endl;
        return 1;
    }

    std::string path(argv[1]);
    int mnum = std::atoi(argv[2]);
    int rnum = std::atoi(argv[3]);

    if (mnum < 1)
    {
        std::cout << "mnum should be higher than 0" << std::endl;
        return 1;
    }
    if (rnum < 1)
    {
        std::cout << "rnum should be higher than 0" << std::endl;
        return 1;
    }

    size_t result = yamr::MapReduce(path, mnum, rnum);

    std::cout << result << std::endl;
    return 0;
}

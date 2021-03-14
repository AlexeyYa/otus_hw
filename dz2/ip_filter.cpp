#include "ip.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

int main()
{
    try
    {
        std::vector<ip_address> ip_pool;

        for(std::string line; std::getline(std::cin, line);)
        {
            ip_pool.emplace_back(split(line));
        }

        // reverse lexicographical sort
        std::sort(ip_pool.begin(), ip_pool.end(),
                  [](ip_address lhs, ip_address rhs){ return lhs > rhs; });

        for(auto ip : ip_pool)
        {
            std::cout << ip << std::endl;
        }

        // filter by first byte and output
        std::vector<ip_address> ip_filtered = filter(1, ip_pool);
        for(auto ip : ip_filtered)
        {
            std::cout << ip << std::endl;
        }

        // filter by first and second bytes and output
        ip_filtered = filter(46, 70, ip_pool);
        for(auto ip : ip_filtered)
        {
            std::cout << ip << std::endl;
        }

        // filter by any byte and output
        ip_filtered = filter_any(46, ip_pool);
        for(auto ip : ip_filtered)
        {
            std::cout << ip << std::endl;
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

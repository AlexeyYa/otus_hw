/*! @file ip_struct.cpp
 *  @brief main execution file
 */

#include "ip_struct.h"

#include <iostream>
#include <vector>
#include <string>

int main()
{
    std::vector<ip_address> ip_pool;

    char c{ -1 };
    short s{0};
    int i{ 2130706433 };
    long long l{ 8875824491850138409 };
    std::string str{"google.com"};
    std::vector v{1, 2, 4, 6};
    std::list list{200, 1, 54, 10, 40, 55};
    std::tuple t{ 127, 0, 0, 1 };

    print_ip(c);
    print_ip(s);
    print_ip(i);
    print_ip(l);
    print_ip(str);
    print_ip(v);
    print_ip(list);
    print_ip(t);

    ip_pool.emplace_back(make_ip_address(c));
    ip_pool.emplace_back(make_ip_address(s));
    ip_pool.emplace_back(make_ip_address(i));
    ip_pool.emplace_back(make_ip_address(l));
    ip_pool.emplace_back(make_ip_address(str));
    ip_pool.emplace_back(make_ip_address(v));
    ip_pool.emplace_back(make_ip_address(list));
    ip_pool.emplace_back(make_ip_address(t));

    for (auto& ip : ip_pool)
        std::cout << ip.to_string() << std::endl;

    return 0;
}

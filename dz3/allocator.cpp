#include "allocator.h"
#include "container.h"

#include <vector>
#include <map>
#include <iostream>



int main()
{
    std::map<int, int, std::less<int>,
            SimpleAllocator<std::pair<const int,int>, 20>> map;
    auto fact = [](auto&& self, int n) -> int
                { return n < 2 ? 1: n * self(self, n-1); };
    for (int i = 0; i < 10; i++)
        map.emplace(std::pair<int,int>{i, fact(fact, i)});
    for (auto elem : map)
        std::cout << elem.first << ' ' << elem.second << std::endl;

    SimpleContainer<int> container;
    for (int i = 0; i < 10; i++)
        container.push(i);

    SimpleContainer<int, SimpleAllocator<int, 10>> containerCustAlloc;
    for (int i = 0; i < 10; i++)
        containerCustAlloc.push(i);

    for (auto elem : containerCustAlloc)
        std::cout << elem << std::endl;

    return 0;
}

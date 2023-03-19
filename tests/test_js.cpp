#include <cassert>
#include <iostream>
#include <vector>

#include <dynamic_typing.hpp>

using namespace DynamicTyping;

int main()
{
    var variable = 1;
    var variable2;
    std::cout << typeof_impl(variable2) << '\n';
    std::cout << variable2 << '\n';
    std::cout << variable << ' ' << typeof_impl(variable) << '\n';
    std::cin >> variable;
    std::cout << variable << ' ' << typeof_impl(variable) << std::endl;
}

#include <cassert>
#include <iostream>
#include <vector>

#include <dynamic_typing.hpp>

using namespace DynamicTyping;

int main()
{
    var variable = 1;
    std::cout << variable << ' ' << DynamicTyping::type(variable) << '\n';
    std::cin >> variable;
    std::cout << variable << ' ' << DynamicTyping::type(variable) << std::endl;
}

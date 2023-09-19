#include <cassert>
#include <iostream>
#include <vector>

#include <dynamic_typing.hpp>

using namespace DynamicTyping;

int main()
{
    var num = -123.3;
    Types::float_t num_typed = num;

    var str = "asdasd";
    Types::string_t s = str;

    var integer = 10;
    Types::integer_t integer_typed = integer;

    if (num == -123.3) std::cout << "Eq\n";
    num = 1;
    var str2 = "1";
    if (num == str2) std::cout << "Eq\n";
}

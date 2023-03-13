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
}

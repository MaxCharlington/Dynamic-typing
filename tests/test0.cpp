#include <iostream>
#include <cassert>

#include <cest/string.hpp>
#include <dynamic_typing.hpp>

using namespace DynamicTyping;

consteval auto test0()
{
    // Integer
    var integer = 1;
    integer = 2ll;
    integer + 5;
    integer - 5;
    integer = integer + 2u;
    integer += 2;
    integer / 2;
    integer % 5;


    // Float
    var floating = 1.5;
    floating = 2.6f;
    floating + 5;
    floating += 2;
    floating / 2;
    // floating % 5;


    //Bool
    var boolean = true;
    boolean = false;
    if (boolean) {
    }

    //String
    cest::string a = "str";
    var string = a;
    // var string2 = std::string("str").data();
    var string3 = "sadasd";
    // string *= 10;
    // string += "helllp";
    // if (string) {}


    // Dinamicnesssss
    var variable;
    variable = 1;
    variable = "string";
    // variable = string;
    // variable = boolean;
    // variable = floating;

    return floating;
}

constexpr auto test1()
{
    var string = "asdasdasd";

    return string;
}

int main()
{
    constexpr auto variable0 = test0();
    std::cout << variable0 << '\n';

    var variable1 = ctr::to_runtime<test1>();
    std::cout << variable1 << '\n';
}

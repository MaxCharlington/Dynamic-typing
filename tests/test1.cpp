#include <iostream>
#include <cassert>

#include "dynamic_typing.hpp"

auto test()
{
    using namespace DynamicTyping;

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
    floating % 5;


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
    string *= 10;
    string += "helllp";
    if (string) {}

    //Array
    var arr1 = array_t{1, 2, "sdfsf", 5.6, nullptr, undefined, NaN};

    // Object
    var obj = object_t{{"name", "Max"}, {"age", 23.3}};

    // Dinamicnesssss
    var variable;
    variable = 1;
    variable = "string";
    variable = string;
    variable = boolean;
    variable = floating;

    return floating;
}

int main()
{
    auto variable = test();
    std::cout << variable << '\n';
}

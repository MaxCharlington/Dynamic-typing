#include <iostream>
#include <cassert>
#include "../src/dynamic_typing.hpp"

int main()
{
    using namespace DynamicTyping;

    // Integer
    var integer = 1;
    integer = 2ll;
    integer + 5;
    integer - 5;
    std::cout << integer;
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
    std::string a = "str";
    var string = a;
    var string2 = std::string("str").data();
    string *= 10;
    string += "helllp";
    if (string) {}


    // Dinamicnesssss
    var variable;
    variable = 1;
    variable = "string";
    variable = string;
    variable = boolean;
    variable = floating;
}

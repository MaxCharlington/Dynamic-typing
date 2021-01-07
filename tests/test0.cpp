#include <iostream>
#include "../src/dynamic_typing.hpp"

int main()
{
    // Integer
    var integer = 1;
    integer = 2ll;
    integer + 5;
    integer = integer + 2u;
    
    //integer += 2;
    //integer / 2;
    //integer % 5;

    // Float
    var floating = 1.5;
    floating = 2.6f;
    floating + 5;
    //floating += 2;
    //floating / 2;
    //floating % 5;

    //Bool
    var boolean = true;
    boolean = false;
    //if (boolean) {}

    //String
    std::string a = "str";
    var string = a;
    //var string2 = "str";
}
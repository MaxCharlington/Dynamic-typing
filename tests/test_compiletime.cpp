#include <cassert>
#include <iostream>
#include <vector>

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
    std::string a = "str";
    var string = a;
    var string3 = "sadasd";
    // string *= 10; emits NaN which is not constexpr convertible to string
    // string += "helllp";
    if (string) {}

    // Array
    var arr = array_t{string, string3, 1, 5.6, undefined, nullptr, NaN};

    // Object
    var obj = object_t{{"name", "Max"}, {"age", 23.3}};

    // Function
    var f = [](object_t&) -> var { return 0; };
    object_t ctx;
    var ret = f(ctx);


    // Dinamicnesssss
    var variable;
    variable = 1;
    variable = "string";
    variable = string;
    variable = boolean;
    variable = floating;

    return floating;
}

// constexpr auto test1()
// {
//     var string = "asdasdasd";

//     return [=]{ return string; };
// }


int main()
{
    constexpr auto variable0 = test0();
    std::cout << variable0 << '\n';

    // var variable1 = to_runtime<test1>();
    // std::cout << variable1 << '\n';
    // test1();


    // Func
    var f = [](object_t&) -> var { return {}; };
    object_t ctx;
    var ret = f(ctx);
}

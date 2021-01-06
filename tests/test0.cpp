#include <iostream>
#include "../src/dynamic_typing.hpp"

int main()
{
    var a = 6;
    a = 9 + a;
    a = a + 9;
    var b = 10;
    a = 10.25 - b - 0.1;

    struct {
        var a;
        var b;
    } str;
    str.a = 0.5;

    std::cout << a << '\n';
}
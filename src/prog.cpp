#include <iostream>
#include "dynamic_typing.hpp"

int main()
{
    var a = 6;
    a = 9 + a;
    var b = 10;
    a = 10.25 - b - 0.1;

    struct {
        var a;
        var b;
    } str;
    str.a = 0.5;
    using std::cout;
    cout << str.a << '\n';
}
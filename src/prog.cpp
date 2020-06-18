#include <iostream>
#include "dynamic_typing.hpp"

int main()
{
    var a = 6;
    a = 9 + a;
    var b = 10;
    a = 10.25 - b - 0.1;
    using std::cout;
    cout << 3 / b << '\n';
}
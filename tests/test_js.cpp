#include <cassert>
#include <iostream>
#include <vector>

#include <dynamic_typing.hpp>

using namespace DynamicTyping;

int main()
{
    var variable;
    std::cout << variable << ' ' << typeof_impl(variable) << '\n';

    #define C ,  // workaround to use commas in macro
    #define test(val)\
        { \
            var variable = val; \
            std::cout << variable << ' ' << typeof_impl(variable) << '\n'; \
        }

    test(1);
    test(1.6);
    test(array_t{1 C 2 C 3 C 4});
    test(object_t{{"name" C "max"} C {"age" C 23}});
    test(nullptr);
    test(NaN);
    test(true);
    test("asdas");
}

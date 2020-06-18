#include <iostream>
#include <variant>
#include "variant_helper.h"
#include <concepts>
#include <type_traits>

//TODO: add support for strings

template <typename T>
concept supported_types_checker = std::is_integral_v<T> || std::is_floating_point_v<T>;
// std::integral includes bool type

template <typename T>
concept type_t = supported_types_checker<T>;

using data_t = std::variant<int64_t, long double, bool>;

class var
{
    data_t data;

public:
    var(type_t auto variable) : data([&]() constexpr {
                                    using Type = decltype(variable);
                                    if constexpr (std::is_same_v<Type, bool>)
                                        return data_t(variable);
                                    else if (std::is_integral_v<Type>)
                                        return data_t(static_cast<int64_t>(variable));
                                    else if (std::is_floating_point_v<Type>)
                                        return data_t(static_cast<long double>(variable));
                                }()) {}

    var(data_t&& data_variant) : data(std::move(data_variant)) {}
    
    // Operations with data types
    auto operator+(const type_t auto &) -> var;
    auto operator-(const type_t auto &number) -> var;

    friend auto operator+(const type_t auto &, const var &) -> var;
    //friend long double operator+(const std::floating_point auto &operand, const var &variable);

    friend auto operator-(const type_t auto &, const var &) -> var;

    //Operations with var

    //IO
    friend std::ostream &operator<<(std::ostream &, const var &);
};

std::ostream &operator<<(std::ostream &os, const var &variable)
{
    std::visit([&](auto &&arg) { os << arg; }, variable.data);
    return os;
}

auto var::operator+(const type_t auto &operand) -> var
{
    auto sum = std::visit([&](auto arg) -> data_t { return operand + arg; }, this->data);
    return sum;
}

auto var::operator-(const type_t auto &operand) -> var
{
    auto difference = std::visit([&](auto arg) -> data_t { return arg - operand; }, this->data);
    return difference;
}

auto operator+(const type_t auto &operand, const var &variable) -> var
{
    auto sum = std::visit([&](auto arg) -> data_t { return operand + arg; }, variable.data);
    return sum;
}

auto operator-(const type_t auto &operand, const var &variable) -> var
{
    auto difference = std::visit([&](auto arg) -> data_t { return operand - arg; }, variable.data);
    return difference;
}

int main()
{
    var a = 6;
    a = 9 + a;
    var b = 10;
    a = 10.25 - b - 0.1;
    using std::cout;
    cout << a;
}
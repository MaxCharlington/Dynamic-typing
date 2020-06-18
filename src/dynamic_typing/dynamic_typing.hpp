#ifndef DYNAMIC_TYPING_H
#define DYNAMIC_TYPING_H

#include <iostream>
#include <variant>
#include "variant_helper.hpp"
#include <concepts>
#include <type_traits>
#include <cmath>

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

    var(data_t &&);

    // Operations with data types
    auto operator+(const type_t auto &) -> var;
    auto operator-(const type_t auto &) -> var;
    auto operator*(const type_t auto &) -> var;
    auto operator/(const type_t auto &) -> var;
    auto operator%(const type_t auto &) -> var;
    friend auto operator+(const type_t auto &, const var &) -> var;
    friend auto operator-(const type_t auto &, const var &) -> var;
    friend auto operator*(const type_t auto &, const var &) -> var;
    friend auto operator/(const type_t auto &, const var &) -> var;
    friend auto operator%(const type_t auto &, const var &) -> var;

    //Operations with var

    //IO
    friend auto operator<<(std::ostream &, const var &) -> std::ostream &;
};

auto var::operator+(const type_t auto &operand) -> var
{
    return std::visit([&](auto arg) -> data_t { return arg + operand; }, this->data);
}

auto var::operator-(const type_t auto &operand) -> var
{
    return std::visit([&](auto arg) -> data_t { return arg - operand; }, this->data);
}

auto var::operator*(const type_t auto &operand) -> var
{
    return std::visit([&](auto arg) -> data_t { return arg * operand; }, this->data);
}

auto var::operator/(const type_t auto &operand) -> var
{
    return std::visit([&](auto arg) -> data_t { return static_cast<long double>(arg) / operand; }, this->data);
}

auto var::operator%(const type_t auto &operand) -> var
{
    return std::visit(std::overloaded{
                          [&](int64_t arg) -> data_t { return arg % operand; },
                          [&](long double arg) -> data_t { return std::fmod(arg, operand); },
                          [&](bool arg) -> data_t { return arg % operand; },
                      },
                      this->data);
}

auto operator+(const type_t auto &operand, const var &variable) -> var
{
    return std::visit([&](auto arg) -> data_t { return operand + arg; }, variable.data);
}

auto operator-(const type_t auto &operand, const var &variable) -> var
{
    return std::visit([&](auto arg) -> data_t { return operand - arg; }, variable.data);
}

auto operator*(const type_t auto &operand, const var &variable) -> var
{
    return std::visit([&](auto arg) -> data_t { return operand * arg; }, variable.data);
}

auto operator/(const type_t auto &operand, const var &variable) -> var
{
    return std::visit([&](auto arg) -> data_t { return static_cast<long double>(operand) / arg; }, variable.data);
}

auto operator%(const type_t auto &operand, const var &variable) -> var
{
    return std::visit(std::overloaded{
                          [&](int64_t arg) -> data_t { return operand % arg; },
                          [&](long double arg) -> data_t { return std::fmod(operand, arg); },
                          [&](bool arg) -> data_t { return operand % arg; },
                      },
                      variable.data);
}

#endif

#ifndef DYNAMIC_TYPING_H
#define DYNAMIC_TYPING_H

#include <iosfwd>
#include <variant>
#include <concepts>
#include <type_traits>
#include <string_view>
#include <cmath>

#include "types.hpp"
#include "variant_helper.hpp"
#include "string_helper.hpp"

class var
{
private:
    data_t data;

public:
    var() = default;
    var(var&&) = default;
    auto operator=(var&&) -> var& = default;
    auto operator=(const var&) -> var& = default;
    
    constexpr var(type_c auto);
    var(data_t &&);
    constexpr auto operator=(const type_c auto) -> var&;
    //var(const type_c auto&); maybe not required


    constexpr auto operator+(const type_c auto &) -> var;
    constexpr auto operator-(const type_c auto &) -> var;
    constexpr auto operator*(const type_c auto &) -> var;
    constexpr auto operator/(const type_c auto &) -> var;
    constexpr auto operator%(const type_c auto &) -> var;
    constexpr friend auto operator+(const type_c auto &, const var &) -> var;
    constexpr friend auto operator-(const type_c auto &, const var &) -> var;
    constexpr friend auto operator*(const type_c auto &, const var &) -> var;
    constexpr friend auto operator/(const type_c auto &, const var &) -> var;
    constexpr friend auto operator%(const type_c auto &, const var &) -> var;

    operator data_t();
    constexpr friend auto operator<<(std::ostream &, const var &) -> std::ostream &;
};

var::var(data_t && data_variant) : data{std::move(data_variant)} {}

constexpr var::var(type_c auto variable) : data{[&]() {
    using Type = decltype(variable);
    if constexpr (std::is_same_v<Type, BOOL>)
        return data_t(variable);
    else if constexpr (std::is_integral_v<Type>)
        return data_t(TO_INTEGER(variable));
    else if constexpr (std::is_floating_point_v<Type>)
        return data_t(TO_FLOAT(variable));
    else if constexpr (is_string_v<Type>) 
        return data_t(TO_STRING(variable));
}()} {}

constexpr auto var::operator=(const type_c auto variable) -> var&
{
    data = [&variable]() {
        using Type = decltype(variable);
        if constexpr (std::is_same_v<Type, BOOL>)
            return data_t(variable);
        else if constexpr (std::is_integral_v<Type>)
            return data_t(TO_INTEGER(variable));
        else if constexpr (std::is_floating_point_v<Type>)
            return data_t(TO_FLOAT(variable));
        else if constexpr (is_string_v<Type>) 
            return data_t(TO_STRING(variable));
    }();
    return *this;
}

constexpr auto var::operator+(const type_c auto &operand) -> var
{
    return std::visit([&operand](auto arg) -> data_t { return arg + operand; }, this->data);
}

constexpr auto var::operator-(const type_c auto &operand) -> var
{
    return std::visit([&operand](auto arg) -> data_t { return arg - operand; }, this->data);
}

constexpr auto var::operator*(const type_c auto &operand) -> var
{
    return std::visit([&operand](auto arg) -> data_t { return arg * operand; }, this->data);
}

constexpr auto var::operator/(const type_c auto &operand) -> var
{
    return std::visit([&operand](auto arg) -> data_t { return TO_FLOAT(arg) / operand; }, this->data);
}

constexpr auto var::operator%(const type_c auto &operand) -> var
{
    return std::visit(overloaded{
                          [&operand](INTEGER arg) -> data_t { return arg % operand; },
                          [&operand](FLOAT arg) -> data_t { return std::fmod(arg, operand); },
                          [&operand](BOOL arg) -> data_t { return arg % operand; },
                      },
                      this->data);
}

constexpr auto operator+(const type_c auto &operand, const var &variable) -> var
{
    return std::visit([&operand](auto arg) -> data_t { return operand + arg; }, variable.data);
}

constexpr auto operator-(const type_c auto &operand, const var &variable) -> var
{
    return std::visit([&operand](auto arg) -> data_t { return operand - arg; }, variable.data);
}

constexpr auto operator*(const type_c auto &operand, const var &variable) -> var
{
    return std::visit([&operand](auto arg) -> data_t { return operand * arg; }, variable.data);
}

constexpr auto operator/(const type_c auto &operand, const var &variable) -> var
{
    return std::visit([&operand](auto arg) -> data_t { return TO_FLOAT(operand) / arg; }, variable.data);  //
}

constexpr auto operator%(const type_c auto &operand, const var &variable) -> var
{
    return std::visit(overloaded{
                          [&operand](INTEGER arg) -> data_t { return operand % arg; },
                          [&operand](FLOAT arg) -> data_t { return std::fmod(operand, arg); },
                          [&operand](BOOL arg) -> data_t { return operand % arg; },
                      },
                      variable.data);
}


constexpr std::ostream &operator<<(std::ostream &os, const var &variable)
{
    std::visit([&os](auto &&arg) { os << arg; }, variable.data);
    return os;
}

var::operator data_t() {
    return data;
}

#endif

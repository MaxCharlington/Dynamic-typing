#ifndef DYNAMIC_TYPING_H
#define DYNAMIC_TYPING_H

#include <iosfwd>
#include <variant>
#include <concepts>
#include <type_traits>
#include <string_view>
#include <cmath>
#include <stdexcept>

#include "types.hpp"
#include "variant_helper.hpp"
#include "string_helper.hpp"
#include "common_helpers.hpp"


namespace DynamicTyping {

namespace dt = DynamicTyping;
namespace sh = StringHelpers;
namespace th = TypeHelpers;
namespace ch = CommonHelpers;

using namespace th;
using namespace ch;

const auto EXCLUDE_STRING = DataType::STRING;

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
    constexpr auto operator=(type_c auto) -> var&;
    //var(const type_c auto&); maybe not required

    // Arithmetic (maths)
    constexpr auto operator+(const type_c auto &) const -> var;
    constexpr auto operator-(const type_c<EXCLUDE_STRING> auto &) const -> var;
    constexpr auto operator*(const type_c<EXCLUDE_STRING> auto &) const -> var;
    constexpr auto operator/(const type_c<EXCLUDE_STRING> auto &) const -> var;
    constexpr auto operator%(const type_c<EXCLUDE_STRING> auto &) const -> var;
    constexpr friend auto operator+(const type_c auto &, const var &) -> var;
    constexpr friend auto operator-(const type_c<EXCLUDE_STRING> auto &, const var &) -> var;
    constexpr friend auto operator*(const type_c auto &, const var &) -> var;
    constexpr friend auto operator/(const type_c<EXCLUDE_STRING> auto &, const var &) -> var;
    constexpr friend auto operator%(const type_c<EXCLUDE_STRING> auto &, const var &) -> var;

    // Assignment arithmetic (maths) (non copy/move)
    constexpr auto operator+=(const type_c auto &) -> var&;
    constexpr auto operator-=(const type_c<EXCLUDE_STRING> auto &) -> var&;
    constexpr auto operator*=(const type_c<EXCLUDE_STRING> auto &) -> var&;
    constexpr auto operator/=(const type_c<EXCLUDE_STRING> auto &) -> var&;
    constexpr auto operator%=(const type_c<EXCLUDE_STRING> auto &) -> var&;
    constexpr friend auto operator+=(const type_c auto & op1, const var &) -> decltype(op1);
    constexpr friend auto operator-=(const type_c<EXCLUDE_STRING> auto & op1, const var &) -> decltype(op1);
    constexpr friend auto operator*=(const type_c auto & op1, const var &) -> decltype(op1);
    constexpr friend auto operator/=(const type_c<EXCLUDE_STRING> auto & op1, const var &) -> decltype(op1);
    constexpr friend auto operator%=(const type_c<EXCLUDE_STRING> auto & op1, const var &) -> decltype(op1);

    operator data_t() const;

    // Universal conversion operator
    template <th::supported_type T>
    operator T() const;

    constexpr friend auto operator<<(std::ostream &, const var &) -> std::ostream &;

    friend auto type(const var&);
};

auto type(const var& variable) {
    return  std::visit(
                overloaded{
                    [](INTEGER arg) { return "integer"; },
                    [](FLOAT arg) { return "float"; },
                    [](BOOL arg) { return "boolean"; },
                    [](const STRING& arg) { return "string"; },
                },
                variable.data
            );
}

var::var(data_t && data_variant) : data{std::move(data_variant)} {}

constexpr var::var(type_c auto variable) : data{[&]() {
    using Type = decltype(variable);
    if constexpr (std::is_same_v<Type, BOOL>)
        return data_t(variable);
    else if constexpr (std::is_integral_v<Type>)
        return data_t(TO_INTEGER(variable));
    else if constexpr (std::is_floating_point_v<Type>)
        return data_t(TO_FLOAT(variable));
    else if constexpr (th::is_string_v<Type>)
        return data_t(TO_STRING(variable));
}()} {}

constexpr auto var::operator=(type_c auto variable) -> var&
{
    data = [&variable]() {
        using Type = decltype(variable);
        if constexpr (std::is_same_v<Type, BOOL>)
            return data_t(variable);
        else if constexpr (std::is_integral_v<Type>)
            return data_t(TO_INTEGER(variable));
        else if constexpr (std::is_floating_point_v<Type>)
            return data_t(TO_FLOAT(variable));
        else if constexpr (th::is_string_v<Type>)
            return data_t(TO_STRING(variable));
    }();
    return *this;
}

constexpr auto var::operator+(const type_c auto &operand) const -> var
{
    return std::visit([&](auto arg) -> data_t { return arg + operand; }, this->data);  // Trash
}

constexpr auto var::operator-(const type_c<EXCLUDE_STRING> auto &operand) const -> var
{
    return  std::visit(
                overloaded{
                    [&](th::arithmetic auto arg) -> data_t { return arg - operand; },
                    [&](const STRING& arg) -> data_t { throw std::invalid_argument{"Cannot use operator- on strings"}; },
                },
                this->data
            );
}

constexpr auto var::operator*(const type_c<EXCLUDE_STRING> auto &operand) const -> var
{
    return  std::visit(
                overloaded{
                    [&](th::arithmetic auto num) -> data_t { return num * operand; },
                    [&](STRING str_copy) -> data_t { return sh::string_multiplication(str_copy, operand); },
                },
                this->data
            );
}

constexpr auto var::operator/(const type_c<EXCLUDE_STRING> auto &operand) const -> var
{
    return  std::visit(
                overloaded{
                    [&](th::arithmetic auto  arg) -> data_t { return arg / operand; },
                    [&](const STRING& arg) -> data_t { throw std::invalid_argument{"Cannot use operator/ on strings"}; },
                },
                this->data
            );
}

constexpr auto var::operator%(const type_c<EXCLUDE_STRING> auto &operand) const -> var
{
    return  std::visit(
                overloaded{
                    [&](INTEGER arg) -> data_t { return arg % operand; },
                    [&](FLOAT arg) -> data_t { return std::fmod(arg, operand); },
                    [&](BOOL arg) -> data_t { return arg % operand; },
                    [&](const STRING& arg) -> data_t { throw std::invalid_argument{"Cannot use operator% on strings"}; },
                },
                this->data
            );
}

constexpr auto operator+(const type_c auto &operand, const var &variable) -> var
{
    return variable + operand;
}

constexpr auto operator-(const type_c<EXCLUDE_STRING> auto &operand, const var &variable) -> var
{
    return  std::visit(
                overloaded{
                    [&](th::arithmetic auto  arg) -> data_t { return operand - arg; },
                    [&](const STRING& arg) -> data_t { throw std::invalid_argument{"Cannot use operator- on strings"}; },
                },
                variable.data
            );
}

constexpr auto operator*(const type_c<EXCLUDE_STRING> auto &operand, const var &variable) -> var
{
    return variable * operand;
}

constexpr auto operator/(const type_c<EXCLUDE_STRING> auto &operand, const var &variable) -> var
{
    return  std::visit(
                overloaded{
                    [&](th::arithmetic auto arg) -> data_t { return operand / arg; },
                    [&](const STRING& arg) -> data_t { throw std::invalid_argument{"Cannot use operator/ on strings"}; },
                },
                variable.data
            );
}

constexpr auto operator%(const type_c<EXCLUDE_STRING> auto &operand, const var &variable) -> var
{
    return  std::visit(
                overloaded{
                    [&](INTEGER arg) { return operand % arg; },
                    [&](FLOAT arg) { return std::fmod(operand, arg); },
                    [&](BOOL arg) { return operand % arg; },
                    [&](const STRING& arg) { throw std::invalid_argument{"Cannot use operator% on strings"}; },
                },
                variable.data
            );
}

constexpr auto var::operator+=(const type_c auto & operand) -> var& {
    std::visit(
        overloaded{
            [&](th::arithmetic auto data) {
                if constexpr (th::is_string_v<decltype(operand)>) throw std::invalid_argument{"Cannot perform addition of arithmetic type and string"};
                this->data = data_t(data + operand);
            },
            [&](STRING& str) {
                if constexpr (th::arithmetic<decltype(operand)>) throw std::invalid_argument{"Cannot perform addition of arithmetic type and string"};
                str += operand;
            },
        },
        this->data
    );
    return *this;
}

constexpr auto var::operator-=(const type_c<EXCLUDE_STRING> auto & operand) -> var& {
    std::visit(
        overloaded{
            [&](th::arithmetic auto data) {
                this->data = data_t(data - operand);
            },
            [&](STRING data) {
                throw std::invalid_argument{"Cannot perform subtraction on strings"};
            },
        },
        this->data
    );
    return *this;
}


constexpr auto var::operator*=(const type_c<EXCLUDE_STRING> auto & operand) -> var& {
    std::visit(
        overloaded{
            [&](th::arithmetic auto num) {
                this->data = data_t(num * operand);
            },
            [&](STRING& str) {
                sh::string_multiplication(str, operand);
            },
        },
        this->data
    );
    return *this;
}

constexpr auto var::operator/=(const type_c<EXCLUDE_STRING> auto & operand) -> var& {
    std::visit(
        overloaded{
            [&](th::arithmetic auto data) {
                this->data = data_t(data / operand);
            },
            [&](STRING data) {
                throw std::invalid_argument{"Cannot use operator/ on strings"};
            },
        },
        this->data
    );
    return *this;
}
// constexpr auto var::operator%=(const type_c<EXCLUDE_STRING> auto &) -> var&;
// constexpr friend auto operator+=(const type_c auto & op1, const var &) -> decltype(op1);
// constexpr friend auto operator-=(const type_c<EXCLUDE_STRING> auto & op1, const var &) -> decltype(op1);
// constexpr friend auto operator*=(const type_c auto & op1, const var &) -> decltype(op1);
// constexpr friend auto operator/=(const type_c<EXCLUDE_STRING> auto & op1, const var &) -> decltype(op1);
// constexpr friend auto operator%=(const type_c<EXCLUDE_STRING> auto & op1, const var &) -> decltype(op1);


constexpr std::ostream &operator<<(std::ostream &os, const var &variable)
{
    std::visit([&os](auto &&arg) { os << arg; }, variable.data);
    return os;
}

template <th::supported_type T>
var::operator T() const {
    if constexpr (std::is_same_v<T, BOOL>) {
        return  std::visit(
                    overloaded{
                        [](th::arithmetic auto arg) { return static_cast<BOOL>(arg); },
                        [](const STRING& arg) { return static_cast<BOOL>(arg != ""); },
                    },
                    this->data
                );
    }
    if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
        return  std::visit(
                    overloaded{
                        [](th::arithmetic auto arg) { return static_cast<T>(arg); },
                        [](const STRING& arg) -> T { throw std::invalid_argument{"Cannot convert strings to numeric type"}; },
                    },
                    this->data
                );
    }
    if constexpr (th::is_string_v<T>) {
        return  std::visit(
                    overloaded{
                        [](th::arithmetic auto arg) { throw std::invalid_argument{"Cannot convert numeric type to string"}; },
                        [](STRING str) -> T { return str; },
                    },
                    this->data
                );
    }
}

var::operator data_t() const {
    return data;
}

}  // namespace DynamicTyping

#endif

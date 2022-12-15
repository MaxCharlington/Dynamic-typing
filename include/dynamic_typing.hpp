#ifndef DYNAMIC_TYPING_H
#define DYNAMIC_TYPING_H

#include <cmath>
#include <concepts>
#include <iosfwd>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <variant>

#include "common_helpers.hpp"
#include "string_helper.hpp"
#include "variant_helper.hpp"
#include "types.hpp"


namespace DynamicTyping {

namespace dt = DynamicTyping;
namespace sh = StringHelpers;
namespace th = Types;
namespace ch = CommonHelpers;

using namespace th;
using namespace ch;

const auto EXCLUDE_STRING = DataType::STRING;

class var
{
private:
    data_t data;

public:
    constexpr var() = default;
    constexpr var(var&&) = default;
    constexpr auto operator=(var&&) -> var& = default;
    constexpr auto operator=(const var&) -> var& = default;

    constexpr var(CType auto);
    constexpr var(data_t&&);
    constexpr auto operator=(CType auto) -> var&;
    //var(const CType auto&); maybe not required

    // Arithmetic (maths)
    constexpr auto operator+(const CType auto &) const -> var;
    constexpr auto operator-(const CType<EXCLUDE_STRING> auto &) const -> var;
    constexpr auto operator*(const CType<EXCLUDE_STRING> auto &) const -> var;
    constexpr auto operator/(const CType<EXCLUDE_STRING> auto &) const -> var;
    constexpr auto operator%(const CType<EXCLUDE_STRING> auto &) const -> var;
    constexpr friend auto operator+(const CType auto &, const var &) -> var;
    constexpr friend auto operator-(const CType<EXCLUDE_STRING> auto &, const var &) -> var;
    constexpr friend auto operator*(const CType auto &, const var &) -> var;
    constexpr friend auto operator/(const CType<EXCLUDE_STRING> auto &, const var &) -> var;
    constexpr friend auto operator%(const CType<EXCLUDE_STRING> auto &, const var &) -> var;

    // Assignment arithmetic (maths) (non copy/move)
    constexpr auto operator+=(const CType auto &) -> var&;
    constexpr auto operator-=(const CType<EXCLUDE_STRING> auto &) -> var&;
    constexpr auto operator*=(const CType<EXCLUDE_STRING> auto &) -> var&;
    constexpr auto operator/=(const CType<EXCLUDE_STRING> auto &) -> var&;
    constexpr auto operator%=(const CType<EXCLUDE_STRING> auto &) -> var&;
    constexpr friend auto operator+=(const CType auto & op1, const var &) -> decltype(op1);
    constexpr friend auto operator-=(const CType<EXCLUDE_STRING> auto & op1, const var &) -> decltype(op1);
    constexpr friend auto operator*=(const CType auto & op1, const var &) -> decltype(op1);
    constexpr friend auto operator/=(const CType<EXCLUDE_STRING> auto & op1, const var &) -> decltype(op1);
    constexpr friend auto operator%=(const CType<EXCLUDE_STRING> auto & op1, const var &) -> decltype(op1);

    constexpr operator data_t() const;

    // Universal conversion operator
    template <th::CSupportedType T>
    constexpr operator T() const;

    friend auto operator<<(std::ostream &, const var &) -> std::ostream &;

    friend constexpr auto type(const var&);
};

constexpr auto type(const var& variable) {
    return  std::visit(
                overloaded{
                    [](integer_t) { return "integer"; },
                    [](float_t) { return "float"; },
                    [](bool_t) { return "boolean"; },
                    [](const string_t&) { return "string"; },
                },
                variable.data
            );
}

constexpr var::var(data_t&& data_variant) : data{std::move(data_variant)} {}

constexpr var::var(CType auto variable) : data{[&]() -> data_t {
    using Type = decltype(variable);
    if constexpr (std::is_same_v<Type, bool_t>)
        return variable;
    else if constexpr (std::is_integral_v<Type>)
        return TO_INTEGER(variable);
    else if constexpr (std::is_floating_point_v<Type>)
        return TO_FLOAT(variable);
    else if constexpr (th::is_string_v<Type>)
        return TO_STRING(variable);
}()} {}

constexpr auto var::operator=(CType auto variable) -> var&
{
    using Type = decltype(variable);
    if constexpr (std::is_same_v<Type, bool_t>)
        data = variable;
    else if constexpr (std::is_integral_v<Type>)
        data = TO_INTEGER(variable);
    else if constexpr (std::is_floating_point_v<Type>)
        data = TO_FLOAT(variable);
    else if constexpr (th::is_string_v<Type>)
        data = TO_STRING(variable);
    return *this;
}

constexpr auto var::operator+(const CType auto &operand) const -> var
{
    using Type = std::remove_cvref_t<decltype(operand)>;
    return  std::visit(
                overloaded{
                    [&](CArithmetic auto arg) -> data_t {
                        if constexpr (std::is_same_v<Type, string_t>)
                            return operand + sh::to_string(arg);
                        else
                            return operand + arg;
                    },
                    [&](const string_t& arg) -> data_t {
                        if constexpr (std::is_same_v<Type, string_t>)
                            return operand + arg;
                        else
                            return sh::to_string(operand).append(arg);
                    }
                },
                this->data
            );
}

constexpr auto var::operator-(const CType<EXCLUDE_STRING> auto &operand) const -> var
{
    return  std::visit(
                overloaded{
                    [&](CArithmetic auto arg) -> data_t { return arg - operand; },
                    [&](const string_t&) -> data_t { throw std::invalid_argument{"Cannot use operator- on strings"}; },
                },
                this->data
            );
}

constexpr auto var::operator*(const CType<EXCLUDE_STRING> auto &operand) const -> var
{
    return  std::visit(
                overloaded{
                    [&](CArithmetic auto num) -> data_t { return num * operand; },
                    [&](string_t str_copy) -> data_t { return sh::string_multiplication(str_copy, operand); },
                },
                this->data
            );
}

constexpr auto var::operator/(const CType<EXCLUDE_STRING> auto &operand) const -> var
{
    return  std::visit(
                overloaded{
                    [&](CArithmetic auto  arg) -> data_t { return arg / operand; },
                    [&](const string_t&) -> data_t { throw std::invalid_argument{"Cannot use operator/ on strings"}; },
                },
                this->data
            );
}

constexpr auto var::operator%(const CType<EXCLUDE_STRING> auto &operand) const -> var
{
    return  std::visit(
                overloaded{
                    [&](integer_t arg) -> data_t { return arg % operand; },
                    [&](float_t arg) -> data_t {
                        if consteval { throw std::invalid_argument{"Cannot use operator% on doubles in compiletime"}; }
                        else { return std::fmod(arg, operand); }
                    },  // TODO:
                    [&](bool_t arg) -> data_t { return arg % operand; },
                    [&](const string_t&) -> data_t { throw std::invalid_argument{"Cannot use operator% on strings"}; },
                },
                this->data
            );
}

constexpr auto operator+(const CType auto &operand, const var &variable) -> var
{
    return variable + operand;
}

constexpr auto operator-(const CType<EXCLUDE_STRING> auto &operand, const var &variable) -> var
{
    return  std::visit(
                overloaded{
                    [&](CArithmetic auto  arg) -> data_t { return operand - arg; },
                    [&](const string_t& arg) -> data_t { throw std::invalid_argument{"Cannot use operator- on strings"}; },
                },
                variable.data
            );
}

constexpr auto operator*(const CType<EXCLUDE_STRING> auto &operand, const var &variable) -> var
{
    return variable * operand;
}

constexpr auto operator/(const CType<EXCLUDE_STRING> auto &operand, const var &variable) -> var
{
    return  std::visit(
                overloaded{
                    [&](CArithmetic auto arg) -> data_t { return operand / arg; },
                    [&](const string_t& arg) -> data_t { throw std::invalid_argument{"Cannot use operator/ on strings"}; },
                },
                variable.data
            );
}

constexpr auto operator%(const CType<EXCLUDE_STRING> auto &operand, const var &variable) -> var
{
    return  std::visit(
                overloaded{
                    [&](integer_t arg) { return operand % arg; },
                    [&](float_t arg) {
                        if consteval { throw std::invalid_argument{"Cannot use operator% on doubles in compiletime"}; }
                        else { return std::fmod(operand, arg); }
                    },  // TODO:
                    [&](bool_t arg) { return operand % arg; },
                    [&](const string_t&) { throw std::invalid_argument{"Cannot use operator% on strings"}; },
                },
                variable.data
            );
}

constexpr auto var::operator+=(const CType auto & operand) -> var& {
    std::visit(
        overloaded{
            [&](CArithmetic auto num) {
                if constexpr (th::is_string_v<decltype(operand)>) throw std::invalid_argument{"Cannot perform addition of arithmetic type and string"};  // TODO: js style
                else data = data_t(num + operand);
            },
            [&](string_t& str) {
                if constexpr (CArithmetic<decltype(operand)>) throw std::invalid_argument{"Cannot perform addition of arithmetic type and string"};  // TODO: js style
                else str += operand;
            },
        },
        this->data
    );
    return *this;
}

constexpr auto var::operator-=(const CType<EXCLUDE_STRING> auto & operand) -> var& {
    std::visit(
        overloaded{
            [&](CArithmetic auto data) {
                this->data = data_t(data - operand);
            },
            [&](string_t) {
                throw std::invalid_argument{"Cannot perform subtraction on strings"};
            },
        },
        this->data
    );
    return *this;
}


constexpr auto var::operator*=(const CType<EXCLUDE_STRING> auto & operand) -> var& {
    std::visit(
        overloaded{
            [&](CArithmetic auto num) {
                this->data = data_t(num * operand);
            },
            [&](string_t& str) {
                sh::string_multiplication(str, operand);
            },
        },
        this->data
    );
    return *this;
}

constexpr auto var::operator/=(const CType<EXCLUDE_STRING> auto & operand) -> var& {
    std::visit(
        overloaded{
            [&](CArithmetic auto data) {
                this->data = data_t(data / operand);
            },
            [&](string_t) {
                throw std::invalid_argument{"Cannot use operator/ on strings"};
            },
        },
        this->data
    );
    return *this;
}
// constexpr auto var::operator%=(const CType<EXCLUDE_STRING> auto &) -> var&;
// constexpr friend auto operator+=(const CType auto & op1, const var &) -> decltype(op1);
// constexpr friend auto operator-=(const CType<EXCLUDE_STRING> auto & op1, const var &) -> decltype(op1);
// constexpr friend auto operator*=(const CType auto & op1, const var &) -> decltype(op1);
// constexpr friend auto operator/=(const CType<EXCLUDE_STRING> auto & op1, const var &) -> decltype(op1);
// constexpr friend auto operator%=(const CType<EXCLUDE_STRING> auto & op1, const var &) -> decltype(op1);


std::ostream &operator<<(std::ostream &os, const var &variable)
{
    std::visit([&os](auto &&arg) { os << arg; }, variable.data);
    return os;
}

template <th::CSupportedType T>
constexpr var::operator T() const {
    if constexpr (std::is_same_v<T, bool_t>) {
        return  std::visit(
                    overloaded{
                        [](CArithmetic auto arg) { return static_cast<bool_t>(arg); },
                        [](const string_t& arg) { return arg != ""; },
                    },
                    this->data
                );
    }
    if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
        return  std::visit(
                    overloaded{
                        [](CArithmetic auto arg) { return static_cast<T>(arg); },
                        [](const string_t&) -> T { throw std::invalid_argument{"Cannot convert strings to numeric type"}; },
                    },
                    this->data
                );
    }
    if constexpr (th::is_string_v<T>) {
        return  std::visit(
                    overloaded{
                        [](CArithmetic auto) { throw std::invalid_argument{"Cannot convert numeric type to string"}; },
                        [](string_t str) -> T { return str; },
                    },
                    this->data
                );
    }
}

constexpr var::operator data_t() const {
    return data;
}

}  // namespace DynamicTyping

#endif
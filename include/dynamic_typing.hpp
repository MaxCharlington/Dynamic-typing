#ifndef DYNAMIC_TYPING_H
#define DYNAMIC_TYPING_H

#include <cmath>
#include <concepts>
#include <iosfwd>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include "constexpr_to_runtime_helper.hpp"
#include "string_helper.hpp"
#include "overloaded.hpp"
#include "primitive_types.hpp"
#include "types.hpp"


namespace DynamicTyping {

namespace dt = DynamicTyping;
namespace sh = StringHelpers;
namespace th = Types;
namespace ctr = CTRHelper;

using namespace th;

static constexpr auto EXCLUDE_STRING = DataType::STRING;

class var
{
public:
    using data_t = std::variant<integer_t, float_t, bool_t, string_t>;

    constexpr var() = default;
    constexpr var(var&&) = default;
    constexpr auto operator=(var&&) -> var& = default;
    constexpr auto operator=(const var&) -> var& = default;

    constexpr var(CType auto);
    constexpr var(data_t&&);
    constexpr auto operator=(CType auto) -> var&;
    //var(const CType auto&); maybe not required

    constexpr var(CObject auto);

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

    // TODO: Implement spaceship operator

    constexpr operator data_t() const;

    // Universal conversion operator
    template <th::CSupportedType T>
    constexpr operator T() const;

    constexpr auto to_runtime() const;

    friend auto operator<<(std::ostream &, const var &) -> std::ostream &;

    friend constexpr auto type(const var&);

private:
    data_t data;
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

constexpr var::var(CObject auto o)
{
    const auto& raw_data = o.template get<"data">();
    const auto data_type = o.template get<"type">();
    assert(o.size() == 2);

    if (data_type == DataType::STRING)
    {
        data = string_t{raw_data.data()};
    }
    else
    {
        if (data_type == DataType::INTEGER) {
            const auto size = sizeof(integer_t);
            std::array<char, size> integer_repr;
            std::copy_n(raw_data.begin(), size, integer_repr.data());
            data = std::bit_cast<integer_t>(integer_repr);
        }
        else if (data_type == DataType::FLOAT) {
            const auto size = sizeof(float_t);
            std::array<char, size> float_repr;
            std::copy_n(raw_data.begin(), size, float_repr.data());
            data = std::bit_cast<float_t>(float_repr);
        }
        else if (data_type == DataType::BOOL) {
            const auto size = sizeof(bool_t);
            std::array<char, size> bool_repr;
            std::copy_n(raw_data.begin(), size, bool_repr.data());
            data = std::bit_cast<bool_t>(bool_repr);
        }
    }
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
                    [&](CArithmetic auto  arg) -> var::data_t { return operand - arg; },
                    [&](const string_t& arg) -> var::data_t { throw std::invalid_argument{"Cannot use operator- on strings"}; },
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
                    [&](CArithmetic auto arg) -> var::data_t { return operand / arg; },
                    [&](const string_t& arg) -> var::data_t { throw std::invalid_argument{"Cannot use operator/ on strings"}; },
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
            [&](const string_t&) {
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
            [&](const string_t&) {
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

constexpr auto var::to_runtime() const
{
    std::array<char, 1024> data;
    return std::visit(
        overloaded{
            [&](CArithmetic auto val) {
                const auto size = sizeof(decltype(val));
                const auto val_data = std::bit_cast<std::array<char, size>>(val);
                std::copy(std::begin(val_data), std::end(val_data), data.data());
                return make_obj(
                    make_field<DataType::NATIVE, "data">(data),
                    make_field<DataType::NATIVE, "len">(size),
                    make_field<DataType::NATIVE, "type">(data_type<decltype(val)>())
                );
            },
            [&](const string_t& str) {
                const auto size = str.length() + 1;  // For \0
                std::copy_n(str.begin(), size, data.data());
                return make_obj(
                    make_field<DataType::NATIVE, "data">(data),
                    make_field<DataType::NATIVE, "len">(size),
                    make_field<DataType::NATIVE, "type">(DataType::STRING)
                );
            }
        },
        this->data
    );
}

std::ostream &operator<<(std::ostream &os, const cest::string &str)
{
    // cest::string is not null terminated for some reason
    os << std::string{str.begin(), str.end()};
    return os;
}

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
                        [](const string_t& str) -> T { return str; },
                    },
                    this->data
                );
    }
}

constexpr var::operator var::data_t() const {
    return data;
}

}  // namespace DynamicTyping

#endif

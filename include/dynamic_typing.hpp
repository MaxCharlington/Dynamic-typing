#ifndef DYNAMIC_TYPING_H
#define DYNAMIC_TYPING_H

#include <cmath>
#include <concepts>
#include <iosfwd>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <variant>

#include <ct2rt.hpp>
#include <overloaded.hpp>

#include <blob_types.hpp>
#include <string_helper.hpp>
#include <types.hpp>


namespace DynamicTyping {

namespace sh = StringHelpers;

using namespace Types;

using data_t = std::variant<
    undefined_t
    , null_t
    , integer_t
    , float_t
    , bool_t
    , string_t
    , array_t
    , object_t
    , function_t
>;

class var
{
public:
    constexpr var() = default;
    constexpr var(var&&) = default;
    constexpr var(const var&) = default;
    constexpr auto operator=(var&&) -> var& = default;
    constexpr auto operator=(const var&) -> var& = default;

    constexpr var(CType auto&&);
    constexpr var(std::same_as<data_t> auto&&);
    constexpr auto operator=(CType auto&&) -> var&;

    // ct2rt support
    constexpr auto sizes_count() const -> std::size_t;
    template<std::size_t Count>
    constexpr auto sizes() const;
    template<auto Sizes>
    constexpr auto serialize() const;
    var(const Blob::CIsTuple auto& repr);

    // Arithmetic
    constexpr auto operator+(const CType<EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto&) const -> var;
    constexpr auto operator-(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto&) const -> var;
    constexpr auto operator*(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto&) const -> var;
    constexpr auto operator/(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto&) const -> var;
    constexpr auto operator%(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto&) const -> var;
    constexpr friend auto operator+(const CType auto&, const var&) -> var;
    constexpr friend auto operator-(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto&, const var&) -> var;
    constexpr friend auto operator*(const CType auto&, const var&) -> var;
    constexpr friend auto operator/(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto&, const var&) -> var;
    constexpr friend auto operator%(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto&, const var&) -> var;

    // Assignment arithmetic (maths) (non copy/move)
    constexpr auto operator+=(const CType<EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto&) -> var&;
    constexpr auto operator-=(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto&) -> var&;
    constexpr auto operator*=(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto&) -> var&;
    constexpr auto operator/=(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto&) -> var&;
    constexpr auto operator%=(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto&) -> var&;

    constexpr std::strong_ordering operator<=>(const var&) const;

    constexpr operator data_t() const;

    // Universal conversion operator
    template <CType T>
    constexpr operator T() const;

    constexpr auto operator()(object_t& args) -> var;
    constexpr auto operator[](std::string_view) -> var&;
    constexpr auto operator[](std::string_view) const -> const var&;
    constexpr auto operator[](std::size_t) -> var&;
    constexpr auto operator[](std::size_t) const -> const var&;

    // consteval auto to_runtime() const;

    friend auto operator<<(std::ostream&, const var&) -> std::ostream&;
    friend auto operator>>(std::istream&, var&) -> std::istream&;

    friend constexpr auto typeof_impl(const var&);

private:
    data_t data;
};

constexpr auto typeof_impl(const var& variable)
{
    // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/typeof
    return  std::visit(
                overloaded{
                    [](undefined_t) { return "undefined"; },
                    [](null_t) { return "object (NULL)"; },
                    [](integer_t) { return "number (integer)"; },
                    [](float_t) { return "number (float)"; },
                    [](bool_t) { return "boolean"; },
                    [](const string_t&) { return "string"; },
                    [](const array_t&) { return "array"; },
                    [](const object_t&) { return "object"; },
                    [](const function_t&) { return "function"; },
                },
                variable.data
            );
}

constexpr var::var(std::same_as<data_t> auto&& data_variant) : data{std::forward<data_t>(data_variant)} {}

constexpr var::var(CType auto&& variable) : data{[&]() -> data_t
{
    using ForwardType = std::remove_reference_t<decltype(variable)>;
    using Type = std::remove_cvref_t<decltype(variable)>;
    if constexpr (std::same_as<Type, bool_t>)
        return variable;
    else if constexpr (std::is_integral_v<Type>)
        return TO_INTEGER(variable);
    else if constexpr (std::is_floating_point_v<Type>)
        return TO_FLOAT(variable);
    else if constexpr (CString<Type>)
        return TO_STRING(std::forward<ForwardType>(variable));
    else if constexpr (std::same_as<Type, null_t>
        or std::same_as<Type, undefined_t>
        or std::is_convertible_v<Type, array_t>
        or std::is_convertible_v<Type, function_t>
        or std::is_convertible_v<Type, object_t>
    )
        return std::forward<ForwardType>(variable);
}()} {}

constexpr auto var::operator=(CType auto&& variable) -> var&
{
    using Type = std::remove_cvref_t<decltype(variable)>;
    if constexpr (std::same_as<Type, bool_t>)
        data = variable;
    else if constexpr (std::is_integral_v<Type>)
        data = TO_INTEGER(variable);
    else if constexpr (std::is_floating_point_v<Type>)
        data = TO_FLOAT(variable);
    else if constexpr (CString<Type>)
        data = TO_STRING(variable);
    else if constexpr (std::same_as<Type, null_t>
        or std::same_as<Type, undefined_t>
        or std::is_convertible_v<Type, array_t>
        or std::is_convertible_v<Type, function_t>
        or std::is_convertible_v<Type, object_t>
    )
        data = variable;
    return *this;
}

namespace detail
{
    template<typename T>
    void init_with(auto& data, const auto& raw_data) {
        std::array<char, sizeof(T)> repr;
        std::copy_n(raw_data.begin(), repr.size(), repr.data());
        data = std::bit_cast<T>(repr);
    };
};

var::var(const Blob::CIsTuple auto& repr)
{
    Blob::Object obj_repr{repr};
    const auto data_type = obj_repr.template get<"type">();
    const auto raw_data = obj_repr.template get<"data">();

    switch (data_type)
    {
    break; case DataType::UNDEFINED:
        data = undefined;
    break; case DataType::NILL:
        data = nullptr;
    break; case DataType::INTEGER:
        detail::init_with<integer_t>(data, raw_data);
    break; case DataType::FLOAT:
        detail::init_with<float_t>(data, raw_data);
    break; case DataType::BOOL:
        detail::init_with<bool_t>(data, raw_data);
    break; case DataType::STRING:
        data = string_t{raw_data.data()};
    break; default:
        throw std::invalid_argument{"Unsupported type for deserialization: " + std::to_string(static_cast<uint8_t>(data_type))};
    }
    // TODO: support array and function
}

constexpr auto var::sizes_count() const -> std::size_t
{
    return  std::visit(
        overloaded{
            [&](undefined_t) { return 1uz; },
            [&](null_t) { return 1uz; },
            [&](CArithmetic auto) { return 1uz; },
            [&](function_t) { return 1uz; },
            [&](const string_t&) { return 1uz; },
            invalid_type<std::size_t, array_t>(),  // TODO: implement sizes_count
            invalid_type<std::size_t, object_t>(),  // TODO: implement sizes_count
        },
        this->data
    );
}

template<std::size_t Count>
constexpr auto var::sizes() const
{
    std::array<std::size_t, Count> sizes;
    std::visit(
        overloaded{
            [&](undefined_t) { sizes[0] = 0; },
            [&](null_t) { sizes[0] = 0; },
            [&](bool_t num) { sizes[0] = sizeof(num); },
            [&](CArithmetic auto num) { sizes[0] = sizeof(num); },
            [&](function_t func) { sizes[0] = sizeof(func); },
            [&](const string_t& str) { sizes[0] = (str.length() + 1) * sizeof(string_t::value_type); },
            invalid_type<void, array_t>(),  // TODO: implement sizes
            invalid_type<void, object_t>(),  // TODO: implement sizes
        },
        this->data
    );
    return sizes;
}

template<auto Sizes>
constexpr auto var::serialize() const
{
    std::array<char, Sizes[0]> data;
    return std::visit(
        overloaded{
            [&](undefined_t val) {
                return Blob::make_obj(
                    Blob::make_field<DataType::NATIVE, "type">(data_type<decltype(val)>()),
                    Blob::make_field<DataType::NATIVE, "data">(data)  // To unify return type
                );
            },
            [&](null_t val) {
                return Blob::make_obj(
                    Blob::make_field<DataType::NATIVE, "type">(data_type<decltype(val)>()),
                    Blob::make_field<DataType::NATIVE, "data">(data)  // To unify return type
                );
            },
            [&](CArithmetic auto val) {
                auto raw_data = std::bit_cast<std::array<char, sizeof(val)>>(val);
                std::copy_n(raw_data.begin(), raw_data.size(), data.data());
                return Blob::make_obj(
                    Blob::make_field<DataType::NATIVE, "type">(data_type<decltype(val)>()),
                    Blob::make_field<DataType::NATIVE, "data">(data)
                );
            },
            [&](const string_t& str) {
                std::copy_n(str.begin(), Sizes[0], data.data());
                return Blob::make_obj(
                    Blob::make_field<DataType::NATIVE, "type">(DataType::STRING),
                    Blob::make_field<DataType::NATIVE, "data">(data)
                );
            },
            [&](const array_t&) {
                return Blob::make_obj(
                    Blob::make_field<DataType::NATIVE, "type">(DataType::ARRAY),
                    Blob::make_field<DataType::NATIVE, "data">(data)
                );
            },
            [&](const function_t&) {
                return Blob::make_obj(
                    Blob::make_field<DataType::NATIVE, "type">(DataType::FUNCTION),
                    Blob::make_field<DataType::NATIVE, "data">(data)
                );
            },
            [&](const object_t&) {
                return Blob::make_obj(
                    Blob::make_field<DataType::NATIVE, "type">(DataType::OBJECT),
                    Blob::make_field<DataType::NATIVE, "data">(data)
                );
            },
            // TODO: implement
        },
        this->data
    );
}

constexpr auto var::operator+(const CType<EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto& operand) const -> var
{
    using Type = std::remove_cvref_t<decltype(operand)>;
    return  std::visit(
                overloaded{
                    invalid_type<data_t, undefined_t, Type>(),
                    invalid_type<data_t, null_t, Type>(),
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
                    },
                    invalid_type<data_t, array_t, Type>(),
                    invalid_type<data_t, object_t, Type>(),
                    invalid_type<data_t, function_t, Type>(),
                },
                this->data
            );
}

constexpr auto var::operator-(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto& operand) const -> var
{
    using Type = std::remove_cvref_t<decltype(operand)>;
    return  std::visit(
                overloaded{
                    invalid_type<data_t, undefined_t, Type>(),
                    invalid_type<data_t, null_t, Type>(),
                    [&](CArithmetic auto arg) -> data_t { return arg - operand; },
                    [](const string_t&) -> data_t { throw std::invalid_argument{"Cannot use operator- on strings"}; },
                    invalid_type<data_t, array_t, Type>(),
                    invalid_type<data_t, object_t, Type>(),
                    invalid_type<data_t, function_t, Type>(),
                },
                this->data
            );
}

constexpr auto var::operator*(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto& operand) const -> var
{
    using Type = std::remove_cvref_t<decltype(operand)>;
    return  std::visit(
                overloaded{
                    invalid_type<data_t, undefined_t, Type>(),
                    invalid_type<data_t, null_t, Type>(),
                    [&](CArithmetic auto num) -> data_t { return num * operand; },
                    [&](string_t str_copy) -> data_t { return sh::string_multiplication(str_copy, operand); },
                    invalid_type<data_t, array_t, Type>(),
                    invalid_type<data_t, object_t, Type>(),
                    invalid_type<data_t, function_t, Type>(),
                },
                this->data
            );
}

constexpr auto var::operator/(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto& operand) const -> var
{
    using Type = std::remove_cvref_t<decltype(operand)>;
    return  std::visit(
                overloaded{
                    invalid_type<data_t, undefined_t, Type>(),
                    invalid_type<data_t, null_t, Type>(),
                    [&](CArithmetic auto arg) -> data_t { return arg / operand; },
                    [](const string_t&) -> data_t { throw std::invalid_argument{"Cannot use operator/ on strings"}; },
                    invalid_type<data_t, array_t, Type>(),
                    invalid_type<data_t, object_t, Type>(),
                    invalid_type<data_t, function_t, Type>(),
                },
                this->data
            );
}

constexpr auto var::operator%(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto& operand) const -> var
{
    using Type = std::remove_cvref_t<decltype(operand)>;
    return  std::visit(
                overloaded{
                    invalid_type<data_t, undefined_t, Type>(),
                    invalid_type<data_t, null_t, Type>(),
                    [&](bool_t arg) -> data_t { return arg % operand; },
                    [&](integer_t arg) -> data_t { return arg % operand; },
                    [&](float_t arg) -> data_t {
                        if consteval { throw std::invalid_argument{"Cannot use operator% on doubles in compiletime"}; }
                        else { return std::fmod(arg, operand); }
                    },  // TODO:
                    invalid_type<data_t, string_t, Type>(),
                    invalid_type<data_t, array_t, Type>(),
                    invalid_type<data_t, object_t, Type>(),
                    invalid_type<data_t, function_t, Type>(),
                },
                this->data
            );
    return {};
}

constexpr auto operator+(const CType auto& operand, const var& variable) -> var
{
    return variable + operand;
}

constexpr auto operator-(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto& operand, const var& variable) -> var
{
    using Type = std::remove_cvref_t<decltype(operand)>;
    return  std::visit(
                overloaded{
                    invalid_type<data_t, undefined_t, Type>(),
                    invalid_type<data_t, null_t, Type>(),
                    [&](CArithmetic auto arg) -> data_t { return operand - arg; },
                    [](const string_t&) -> data_t { throw std::invalid_argument{"Cannot use operator- on strings"}; },
                    invalid_type<data_t, array_t, Type>(),
                    invalid_type<data_t, object_t, Type>(),
                    invalid_type<data_t, function_t, Type>(),
                },
                variable.data
            );
}

constexpr auto operator*(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto& operand, const var& variable) -> var
{
    return variable * operand;
}

constexpr auto operator/(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto& operand, const var& variable) -> var
{
    using Type = std::remove_cvref_t<decltype(operand)>;
    return  std::visit(
                overloaded{
                    invalid_type<data_t, undefined_t, Type>(),
                    invalid_type<data_t, null_t, Type>(),
                    [&](CArithmetic auto arg) -> data_t { return operand / arg; },
                    [](const string_t&) -> data_t { throw std::invalid_argument{"Cannot use operator/ on strings"}; },
                    invalid_type<data_t, array_t, Type>(),
                    invalid_type<data_t, object_t, Type>(),
                    invalid_type<data_t, function_t, Type>(),
                },
                variable.data
            );
}

constexpr auto operator%(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto& operand, const var& variable) -> var
{
    using Type = std::remove_cvref_t<decltype(operand)>;
    return  std::visit(
                overloaded{
                    invalid_type<data_t, undefined_t, Type>(),
                    invalid_type<data_t, null_t, Type>(),
                    [&](integer_t arg) -> data_t { return operand % arg; },
                    [&](float_t arg) -> data_t  {
                        if consteval { throw std::invalid_argument{"Cannot use operator% on doubles in compiletime"}; }
                        else { return std::fmod(operand, arg); }
                    },  // TODO:
                    [&](bool_t arg)-> data_t  { return operand % arg; },
                    [](const string_t&)-> data_t  { throw std::invalid_argument{"Cannot use operator% on strings"}; },
                    invalid_type<data_t, array_t, Type>(),
                    invalid_type<data_t, object_t, Type>(),
                    invalid_type<data_t, function_t, Type>(),
                },
                variable.data
            );
}

constexpr auto var::operator+=(const CType<EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto& operand) -> var&
{
    using Type = std::remove_cvref_t<decltype(operand)>;
    std::visit(
        overloaded{
            invalid_type<void, undefined_t, Type>(),
            invalid_type<void, null_t, Type>(),
            [&](CArithmetic auto num) {
                if constexpr (CString<Type>) throw std::invalid_argument{"Cannot perform addition of arithmetic type and string"};  // TODO: js style
                else data = data_t(num + operand);
            },
            [&](string_t& str) {
                if constexpr (CArithmetic<Type>) throw std::invalid_argument{"Cannot perform addition of arithmetic type and string"};  // TODO: js style
                else str += operand;
            },
            invalid_type<void, array_t, Type>(),
            invalid_type<void, object_t, Type>(),
            invalid_type<void, function_t, Type>(),
        },
        this->data
    );
    return *this;
}

constexpr auto var::operator-=(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto& operand) -> var&
{
    using Type = std::remove_cvref_t<decltype(operand)>;
    std::visit(
        overloaded{
            invalid_type<void, undefined_t, Type>(),
            invalid_type<void, null_t, Type>(),
            [&](CArithmetic auto data) { this->data = data_t(data - operand); },
            [](const string_t&) { throw std::invalid_argument{"Cannot perform subtraction on strings"}; },
            invalid_type<void, array_t, Type>(),
            invalid_type<void, object_t, Type>(),
            invalid_type<void, function_t, Type>(),
        },
        this->data
    );
    return *this;
}


constexpr auto var::operator*=(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto& operand) -> var&
{
    using Type = std::remove_cvref_t<decltype(operand)>;
    std::visit(
        overloaded{
            invalid_type<void, undefined_t, Type>(),
            invalid_type<void, null_t, Type>(),
            [&](CArithmetic auto num) { this->data = data_t(num * operand); },
            [&](string_t& str) { sh::string_multiplication(str, operand); },
            invalid_type<void, array_t, Type>(),
            invalid_type<void, object_t, Type>(),
            invalid_type<void, function_t, Type>(),
        },
        this->data
    );
    return *this;
}

constexpr auto var::operator/=(const CType<EXCL_STRING, EXCL_ARRAY, EXCL_FUNCTION, EXCL_OBJECT, EXCL_UNDEFINED> auto& operand) -> var&
{
    using Type = std::remove_cvref_t<decltype(operand)>;
    std::visit(
        overloaded{
            invalid_type<void, undefined_t, Type>(),
            invalid_type<void, null_t, Type>(),
            [&](CArithmetic auto data) { this->data = data_t(data / operand); },
            [](const string_t&) { throw std::invalid_argument{"Cannot use operator/ on strings"}; },
            invalid_type<void, array_t, Type>(),
            invalid_type<void, object_t, Type>(),
            invalid_type<void, function_t, Type>(),
        },
        this->data
    );
    return *this;
}

constexpr std::strong_ordering var::operator<=>(const var& other) const
{
    return this->data.index() <=> other.data.index();  // TODO:
}

constexpr auto var::operator()(object_t& args) -> var
{
    return std::visit(
        overloaded{
            [&](function_t f) { return f(args); },
            [](const CArithmetic auto) -> var { throw std::invalid_argument{"Not callable"}; },
            [](const string_t&) -> var { throw std::invalid_argument{"Not callable"}; },
            [](const array_t&) -> var { throw std::invalid_argument{"Not callable"}; },
            [](const object_t&) -> var { throw std::invalid_argument{"Not callable"}; },
            [](const null_t) -> var { throw std::invalid_argument{"Not callable"}; },
            [](const undefined_t) -> var { throw std::invalid_argument{"Not callable"}; },
        },
        this->data
    );
}

std::ostream& operator<<(std::ostream& os, const var& variable)
{
    auto print_arr_impl = [](std::ostream& os, const var& variable)
    {
        std::visit(
            overloaded {
                [&](const null_t arg) { os << arg; },
                [&](const undefined_t arg) { os << arg; },
                [&](const CArithmetic auto arg) { os << arg; },
                [&](const string_t& str) { os << '\'' << str << '\''; },
                [&](const array_t& arg) { os << arg; },
                [&](const object_t& arg) { os << arg; },
                [&](const function_t& arg) { os << arg; },
            },
            variable.data
        );
    };
    std::visit(
        overloaded {
            [&](const array_t& arr) {
                os << '[';
                for (size_t i = 0; i < arr.size() - 1; i++)
                {
                    print_arr_impl(os, arr[i]);
                    os << ", ";
                }
                print_arr_impl(os, arr[arr.size() - 1]);
                os << ']';
            },
            [&](const string_t& str) { os << std::string{str.begin(), str.end()}; },
            [&](const function_t&) { os << "function"; },  // TODO:
            [&](const object_t&) { os << "object"; },  // TODO:
            [&](const CArithmetic auto arg) { os << arg; },
            [&](const null_t) { os << "null"; },
            [&](const undefined_t) { os << "undefined"; },
        },
        variable.data
    );
    return os;
}

std::istream& operator>>(std::istream& is, var& variable)
{
    const auto is_integer = [](const std::string& str)
    {
        // TODO: add length check
        if (str.length() == 0) return false;
        if (str.length() == 1 and str[0] >= '0' and str[0] <= '9') return true;
        return (str[0] == '-' or (str[0] >= '0' and str[0] <= '9')) and std::all_of(str.begin() + 1, str.end(), [](char symb){ return symb >= '0' and symb <= '9'; });
    };

    const auto is_float = [](const std::string& str)
    {
        // TODO: add length check
        if (str.length() == 0) return false;
        else if (str.length() == 1 and str[0] >= '0' and str[0] <= '9') return true;
        else {
            return (str[0] == '-' or (str[0] >= '0' and str[0] <= '9')) and std::all_of(str.begin() + 1, str.end(), [met_dot = false](char symb){
                if (symb >= '0' and symb <= '9') return true;
                if (symb == '.' and not met_dot) return true;
                return false;
            });
        }
    };

    std::string input;
    std::getline(is, input);
    if (is_integer(input))
    {
        variable = std::stol(input);
    }
    else if (is_float(input))
    {
        variable = std::stod(input);
    }
    else
    {
        variable = input.c_str();
    }
    return is;
}

template <CType T>
constexpr var::operator T() const {
    if constexpr (std::same_as<T, bool_t>) {
        return  std::visit(
                    overloaded{
                        [](const null_t) { return false; },
                        [](const undefined_t) { return false; },
                        [](const CArithmetic auto arg) { return static_cast<bool_t>(arg); },
                        [](const string_t& arg) { return arg != ""; },
                        invalid_type<T, array_t, T>(),
                        invalid_type<T, object_t, T>(),
                        invalid_type<T, function_t, T>(),
                    },
                    this->data
                );
    }
    else if constexpr (CArithmetic<T>) {
        return  std::visit(
                    overloaded{
                        [](null_t) { return 0; },
                        invalid_type<T, undefined_t, T>(),
                        [](CArithmetic auto arg) { return static_cast<T>(arg); },
                        [](const string_t&) -> T { throw std::invalid_argument{"Cannot convert strings to numeric type"}; },
                        invalid_type<T, array_t, T>(),
                        invalid_type<T, object_t, T>(),
                        invalid_type<T, function_t, T>(),
                    },
                    this->data
                );
    }
    else if constexpr (CString<T>) {
        return  std::visit(
                    overloaded{
                        invalid_type<T, null_t, T>(),
                        [](CArithmetic auto) { throw std::invalid_argument{"Cannot convert numeric type to string"}; },
                        [](const string_t& str) -> T { return str; },
                        invalid_type<T, array_t, T>(),
                        invalid_type<T, object_t, T>(),
                        invalid_type<T, function_t, T>(),
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

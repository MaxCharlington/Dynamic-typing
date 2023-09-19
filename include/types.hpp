#pragma once

#include <cstdint>
#include <functional>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>

#include <remove_all_const.hpp>

namespace DynamicTyping {

class var;  // Forward declaration

}

namespace DynamicTyping::Types {

namespace th = Types;

enum class DataType : std::uint8_t {
    NILL,
    UNDEFINED,
    INTEGER,
    FLOAT,
    BOOL,
    STRING,
    ARRAY,
    OBJECT,
    FUNCTION,
    NATIVE  // For serialization
};

using null_t      = std::nullptr_t;
using integer_t   = int64_t;  // int128_t?
using float_t     = long double;
using string_t    = std::string;
using bool_t      = bool;
using undefined_t = std::monostate;
using variable_t  = std::pair<std::string, var>;
using array_t     = std::vector<var>;
using field_t     = variable_t;
using object_t    = std::vector<field_t>;
using function_t  = var(*)(object_t&);

// Constants
constexpr float_t NaN = std::numeric_limits<float_t>::quiet_NaN();
constexpr undefined_t undefined;

// Arithmetic concept
template <typename T>
concept Arithmetic = std::is_arithmetic_v<std::remove_cvref_t<T>>;

// String like types
template <typename T>
concept StringLike = std::is_same_v<std::remove_cvref_t<T>, string_t> ||  // std::string_view?
                     std::is_same_v<remove_all_const_t<std::decay_t<T>>, char*>;

template <typename T>
consteval auto data_type() -> DataType {
    if constexpr      (std::same_as<T, bool_t>)            return DataType::BOOL;
    else if constexpr (std::same_as<T, null_t>)            return DataType::NILL;  // Should be before DataType::FUNCTION as it's convertible to func ptr
    else if constexpr (std::is_integral_v<T>)              return DataType::INTEGER;
    else if constexpr (std::is_floating_point_v<T>)        return DataType::FLOAT;
    else if constexpr (StringLike<T>)                      return DataType::STRING;
    else if constexpr (std::convertible_to<T, array_t>)    return DataType::ARRAY;
    else if constexpr (std::same_as<T, object_t>)          return DataType::OBJECT;
    else if constexpr (std::convertible_to<T, function_t>) return DataType::FUNCTION;
    else                                                   return DataType::UNDEFINED;
}

namespace type_impl {

template<DataType dt>
constexpr auto make_type()
{
    using enum DataType;
    if constexpr      (dt == INTEGER)   return integer_t{};
    else if constexpr (dt == FLOAT)     return float_t{};
    else if constexpr (dt == BOOL)      return bool_t{};
    else if constexpr (dt == STRING)    return string_t{};
    else if constexpr (dt == ARRAY)     return array_t{};
    else if constexpr (dt == OBJECT)    return object_t{};
    else if constexpr (dt == FUNCTION)  return function_t{};
    else if constexpr (dt == NILL)      return null_t{};
    else if constexpr (dt == UNDEFINED) return undefined_t{};
    else                                return;  // void for NATIVE
}

}  // namespace type_impl

template<DataType dt>
using type = decltype(type_impl::make_type<dt>());

template<typename T>
using internal_t = type<data_type<T>()>;

template <typename T>
concept SupportedInternalType = std::same_as<std::remove_cvref_t<T>, undefined_t> or
                                std::same_as<std::remove_cvref_t<T>, null_t> or
                                Arithmetic<std::remove_cvref_t<T>> or
                                StringLike<std::remove_cvref_t<T>> or
                                std::is_convertible_v<std::remove_cvref_t<T>, array_t> or
                                std::is_convertible_v<std::remove_cvref_t<T>, object_t> or
                                std::is_convertible_v<std::remove_cvref_t<T>, function_t>;

/// @brief Concept that checks if a given is supported
/// @details Supported means one of a listed types or convertible to in
///          some cases
template <typename T, typename ...Ts>
concept SupportedType = SupportedInternalType<T> and
                        (sizeof...(Ts) == 0 or
                         (... or (std::same_as<std::remove_cvref_t<T>, Ts>)) or
                         (... or (std::is_convertible_v<std::remove_cvref_t<T>, Ts>)));

/// @brief Helpers to throw if unsupported types provided a given binary operation
/// @return Callable for 'overloaded' to inherit
template<typename Ret, typename T1, typename T2>
consteval auto invalid_type()
{
    using Arg = decltype(std::is_class_v<T1> ? std::declval<std::add_lvalue_reference_t<T1>>() : std::declval<T1>());
    return [](Arg) -> Ret { throw std::invalid_argument{"Unsupported operands: " + std::string{typeid(T1).name()} + " and " + std::string{typeid(T2).name()}}; };
}

/// @brief Helpers to throw if unsupported types provided a given unary operation
/// @return Callable for 'overloaded' to inherit
template<typename Ret, typename T>
consteval auto invalid_type()
{
    using Arg = decltype(std::is_class_v<T> ? std::declval<std::add_lvalue_reference_t<T>>() : std::declval<T>());
    return [](Arg) -> Ret { throw std::invalid_argument{"Unsupported operand: " + std::string{typeid(T).name()}}; };
}

// Compile time assertions to check any addition for this core functionality does no break something
static_assert(std::same_as<internal_t<int>, integer_t>);
static_assert(std::same_as<internal_t<uint8_t>, integer_t>);
static_assert(std::same_as<internal_t<float>, float_t>);
static_assert(std::same_as<internal_t<bool>, bool_t>);
static_assert(std::same_as<internal_t<const char*>, string_t>);
static_assert(std::same_as<internal_t<char*>, string_t>);
static_assert(std::same_as<internal_t<char[]>, string_t>);
static_assert(std::same_as<internal_t<std::string>, string_t>);
static_assert(std::same_as<internal_t<std::string&>, string_t>);
// static_assert(std::same_as<internal_t<std::string_view>, string_t>);
static_assert(std::same_as<internal_t<const std::string&>, string_t>);
static_assert(std::same_as<internal_t<nullptr_t>, null_t>);
static_assert(std::same_as<internal_t<std::array<int, 5>>, undefined_t>);  // anything unsupported is undefined_t
// TODO: test array, object, function
static_assert(SupportedType<int>);
static_assert(SupportedType<int, integer_t>);
static_assert(!SupportedType<int, string_t>);

}  // namespace DynamicTyping::Types

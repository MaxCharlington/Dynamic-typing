#ifndef TYPES_H
#define TYPES_H

#include <functional>
#include <string>
#include <type_traits>
#include <vector>

#include <primitive_types.hpp>


namespace DynamicTyping {

class var;  // Forward declaration

}

namespace DynamicTyping::Types {

namespace th = Types;

enum class DataType {
    NILL,
    UNDEFINED,
    INTEGER,
    FLOAT,
    BOOL,
    STRING,
    ARRAY,
    OBJECT,
    FUNCTION,
    NATIVE
};

using variable_t = std::pair<std::string, var>;
using array_t =    std::vector<var>;
using field_t =    variable_t;
using object_t =   std::vector<field_t>;
using function_t = std::function<var(array_t)>;

template <typename T>
consteval auto data_type() -> DataType {
    if constexpr      (std::is_integral_v<T>)              return DataType::INTEGER;
    else if constexpr (std::is_floating_point_v<T>)        return DataType::FLOAT;
    else if constexpr (std::same_as<T, bool_t>)            return DataType::BOOL;
    else if constexpr (CString<T>)                         return DataType::STRING;
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
    if constexpr      (dt == INTEGER)  return integer_t{};
    else if constexpr (dt == FLOAT)    return float_t{};
    else if constexpr (dt == BOOL)     return bool_t{};
    else if constexpr (dt == STRING)   return string_t{};
    else if constexpr (dt == ARRAY)    return array_t{};
    else if constexpr (dt == OBJECT)   return object_t{};
    else if constexpr (dt == FUNCTION) return function_t{};
    else                               return;
}

}  // namespace type_impl

template<DataType dt>
using type = decltype(type_impl::make_type<dt>());

template<typename T>
using internal_t = type<data_type<T>()>;


namespace excluded_impl {

template <typename T, DataType ...Exclude>
consteval bool IsExcluded() {
    if constexpr (std::same_as<T, null_t>            and (... or (Exclude == DataType::NILL))) return true;
    if constexpr (std::same_as<T, bool_t>            and (... or (Exclude == DataType::BOOL))) return true;
    if constexpr (std::is_integral_v<T>              and (... or (Exclude == DataType::INTEGER))) return true;
    if constexpr (std::is_floating_point_v<T>        and (... or (Exclude == DataType::FLOAT))) return true;
    if constexpr (CString<T>                         and (... or (Exclude == DataType::STRING))) return true;
    if constexpr (std::convertible_to<T, array_t>    and (... or (Exclude == DataType::ARRAY))) return true;
    if constexpr (std::convertible_to<T, object_t>   and (... or (Exclude == DataType::OBJECT))) return true;
    if constexpr (std::convertible_to<T, function_t> and (... or (Exclude == DataType::FUNCTION))) return true;
    else return false;
}

}  // namespace excluded_impl

/// @brief Concept that checks if provided C++ type is excluded for function
template <typename T, DataType ...Exclude>
constexpr bool CExcluded = excluded_impl::IsExcluded<T, Exclude...>();

/// @brief Concept to provide supported type exclusion for functions
template <typename T, DataType ...Exclude>
concept CType =
    (
        std::same_as<T, null_t>
        or CArithmetic<T>
        or CString<T>
        or std::is_convertible_v<T, array_t>
        or std::is_convertible_v<T, object_t>
        or std::is_convertible_v<T, function_t>
    )
    and not CExcluded<T, DataType::UNDEFINED, Exclude...>;

static constexpr auto EXCL_NILL = DataType::NILL;
static constexpr auto EXCL_STRING = DataType::STRING;
static constexpr auto EXCL_ARRAY = DataType::ARRAY;
static constexpr auto EXCL_OBJECT = DataType::OBJECT;
static constexpr auto EXCL_FUNCTION = DataType::FUNCTION;

template<typename Ret, typename T1, typename T2>
consteval auto invalid_type()
{
    using Arg = decltype(std::is_class_v<T1> ? std::declval<std::add_lvalue_reference_t<T1>>() : std::declval<T1>());
    return [](Arg) -> Ret { throw std::invalid_argument{"unsupported operands: " + std::string{typeid(T1).name()} + " and " + std::string{typeid(T2).name()}}; };
}

// template<typename Ret, typename T>
// consteval auto invalid_type(std::string_view str = "")
// {
//     return [=](const T&) -> Ret {
//         if (str == "")
//             throw std::invalid_argument{"unsupported type: " + std::string{typeid(T).name()};
//         else
//             throw std::invalid_argument{str};
//     };
// }

}  // namespace DynamicTyping::Types

#endif  // TYPES_H

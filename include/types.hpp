#ifndef TYPES_H
#define TYPES_H

#include <functional>
#include <string>
#include <type_traits>
#include <vector>

#include "primitive_types.hpp"


namespace DynamicTyping {

class var;  // Forward declaration

}

namespace DynamicTyping::Types {

namespace th = Types;

using variable_t = std::pair<std::string, var>;
using array_t = std::vector<var>;
using field_t = variable_t;
using object_t = std::vector<field_t>;
using function_t = std::function<var(array_t)>;

template <typename T>
consteval auto data_type() -> DataType {
    if constexpr (std::is_same_v<T, integer_t>) return DataType::INTEGER;
    else if constexpr (std::is_same_v<T, float_t>) return DataType::FLOAT;
    else if constexpr (std::is_same_v<T, bool_t>) return DataType::BOOL;
    else if constexpr (std::is_same_v<T, string_t>) return DataType::STRING;
    else if constexpr (std::is_same_v<T, array_t>) return DataType::ARRAY;
    else if constexpr (std::is_same_v<T, object_t>) return DataType::OBJECT;
    else if constexpr (std::is_same_v<T, function_t>) return DataType::FUNCTION;
    else return DataType::UNDEFINED;
}

namespace type_impl {

template<DataType dt>
constexpr auto make_type()
{
    using enum DataType;
    if constexpr (dt == INTEGER)
        return integer_t{};
    else if constexpr (dt == FLOAT)
        return float_t{};
    else if constexpr (dt == BOOL)
        return bool_t{};
    else if constexpr (dt == STRING)
        return string_t{};
    else if constexpr (dt == ARRAY)
        return array_t{};
    else if constexpr (dt == OBJECT)
        return object_t{};
    else if constexpr (dt == FUNCTION)
        return function_t{};
    else
        return;
}

}  // namespace type_impl

template<DataType dt>
using type = decltype(type_impl::make_type<dt>());

/// @brief Checks if provided C++ type is excluded for function
template <typename T, DataType ...Exclude>
consteval bool IsExcluded() {
    if constexpr (std::is_same_v<T, bool> and (... or (Exclude == DataType::BOOL))) return true;
    if constexpr (std::is_integral_v<T> and (... or (Exclude == DataType::INTEGER))) return true;
    if constexpr (std::is_floating_point_v<T> and (... or (Exclude == DataType::FLOAT))) return true;
    if constexpr (is_string_v<T> and (... or (Exclude == DataType::STRING))) return true;
    if constexpr (std::is_same_v<T, array_t> and (... or (Exclude == DataType::ARRAY))) return true;
    if constexpr (std::is_same_v<T, object_t> and (... or (Exclude == DataType::OBJECT))) return true;
    if constexpr (std::is_same_v<T, function_t> and (... or (Exclude == DataType::FUNCTION))) return true;
    else return false;
}

/// @brief Concept to provide supported type exclusion for functions
template <typename T, DataType ...Exclude>
concept CType = (CArithmetic<T> ||
                 CString<T>) &&
                 !IsExcluded<T, DataType::UNDEFINED, Exclude...>();

static constexpr auto EXCL_STRING = DataType::STRING;
static constexpr auto EXCL_ARRAY = DataType::ARRAY;
static constexpr auto EXCL_OBJECT = DataType::OBJECT;
static constexpr auto EXCL_FUNCTION = DataType::FUNCTION;

}  // namespace DynamicTyping::Types

#endif  // TYPES_H

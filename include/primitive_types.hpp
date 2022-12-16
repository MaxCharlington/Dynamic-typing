#ifndef PRIM_TYPES_H
#define PRIM_TYPES_H

#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>

#include <template_strings.hpp>
#include <cest/string.hpp>

#include "remove_all_const.hpp"

namespace DynamicTyping::Types {

namespace th = Types;

using integer_t   = int64_t; // int128_t?
using float_t     = long double;
using string_t    = cest::string;
using bool_t      = bool;

// Arithmetic concept
template <typename T> concept CArithmetic = std::is_arithmetic_v<T>;

// String like types
template <typename T>
inline constexpr bool is_string_v = std::is_same_v<T, string_t> ||
                                    std::is_same_v<remove_all_const_t<std::decay_t<T>>, char *>;

template <typename T>
concept CString = is_string_v<T>;


// Conversions
constexpr integer_t TO_INTEGER(std::integral auto var) {
    return static_cast<integer_t>(var);
}

constexpr float_t TO_FLOAT(std::floating_point auto var) {
    return static_cast<float_t>(var);
}

constexpr string_t TO_STRING(CString auto var) {
    return var;
}

// Exclusion mechanism
enum class DataType {
    NONE,
    NILL,
    UNDEFINED,
    INTEGER,
    FLOAT,
    BOOL,
    STRING,
    ARRAY,
    OBJECT,
    NATIVE
};

constexpr const char* DataTypeStrRepr(DataType t) {
    switch (t) {
        case DataType::NONE: return "NONE";
        case DataType::NILL: return "NILL";
        case DataType::UNDEFINED: return "UNDEFINED";
        case DataType::INTEGER: return "INTEGER";
        case DataType::FLOAT: return "FLOAT";
        case DataType::BOOL: return "BOOL";
        case DataType::STRING: return "STRING";
        default: return nullptr;
    }
}

template <typename T>
consteval auto data_type() -> DataType {
    if constexpr (std::is_same_v<T, integer_t>) return DataType::INTEGER;
    else if constexpr (std::is_same_v<T, float_t>) return DataType::FLOAT;
    else if constexpr (std::is_same_v<T, bool_t>) return DataType::BOOL;
    else if constexpr (std::is_same_v<T, string_t>) return DataType::STRING;
    else return DataType::NONE;
}

template <typename T, DataType Exclude>
consteval bool IsExcluded() {
    if constexpr (std::is_same_v<T, bool> && (Exclude == DataType::BOOL)) return true;
    if constexpr (std::is_integral_v<T> && (Exclude == DataType::INTEGER)) return true;
    if constexpr (std::is_floating_point_v<T> && (Exclude == DataType::FLOAT)) return true;
    if constexpr (is_string_v<T> && (Exclude == DataType::STRING)) return true;
    else return false;
}


template <typename T, DataType Exclude = DataType::NONE>
concept CSupportedType =  (CArithmetic<T> ||
                           CString<T>) &&
                           !IsExcluded<T, Exclude>();

template <typename Type, DataType Exclude = DataType::NONE>
concept CType = CSupportedType<Type, Exclude>;

template<DataType dt>
constexpr auto make_type()
{
    using enum DataType;
    if constexpr (dt == NONE or dt == NILL or dt == UNDEFINED)
        return;
    else if constexpr (dt == INTEGER)
        return integer_t{};
    else if constexpr (dt == FLOAT)
        return float_t{};
    else if constexpr (dt == BOOL)
        return bool_t{};
    else if constexpr (dt == STRING)
        return string_t{};
}

template<DataType dt>
using type = decltype(make_type<dt>());

// template<typename T>
// constexpr auto make_type()
// {
//     if constexpr (std::is_same_v<T, bool>) return bool_t{};
//     if constexpr (std::is_integral_v<T>) return integer_t{};
//     if constexpr (std::is_floating_point_v<T>) return float_t{};
//     if constexpr (is_string_v<T>) return string_t{};
//     else return;
// }

// template<typename T>
// using type = decltype(make_type<T>());

}  // namespace DynamicTyping::Types

#endif  // PRIM_TYPES_H

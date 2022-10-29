#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <string>
#include <variant>

#include "common_helpers.hpp"

namespace DynamicTyping::TypeHelpers {

namespace th = TypeHelpers;
namespace ch = CommonHelpers;

using INTEGER   = int64_t; // int128_t?
using FLOAT     = long double;
using STRING    = std::string;
using BOOL      = bool;
using data_t    = std::variant<INTEGER, FLOAT, BOOL, STRING>;


// Arithmetic concept
template <typename T> concept arithmetic = std::is_arithmetic_v<T>;

// String like types
template <typename T>
inline constexpr bool is_string_v = std::is_same_v<T, std::string> ||
                                    std::is_same_v<ch::remove_const_t<std::decay_t<T>>, char *>;

template <typename T>
concept string = is_string_v<T>;


// Conversions
constexpr INTEGER TO_INTEGER(std::integral auto var) {
    return static_cast<INTEGER>(var);
}

constexpr FLOAT TO_FLOAT(std::floating_point auto var) {
    return static_cast<FLOAT>(var);
}

constexpr STRING TO_STRING(th::string auto var) {
    return static_cast<STRING>(var);
}

// Exclusion mechanism
enum class DataType {
    NONE,
    INTEGER,
    FLOAT,
    BOOL,
    STRING
};

constexpr const char* DataTypeStrRepr(DataType t) {
    switch (t) {
        case DataType::NONE: return "NONE";
        case DataType::INTEGER: return "INTEGER";
        case DataType::FLOAT: return "FLOAT";
        case DataType::BOOL: return "BOOL";
        case DataType::STRING: return "STRING";
        default: return nullptr;
    }
}

template <typename T>
consteval auto data_type() -> DataType{
    if constexpr (std::is_same_v<T, INTEGER>) return DataType::INTEGER;
    else if constexpr (std::is_same_v<T, FLOAT>) return DataType::FLOAT;
    else if constexpr (std::is_same_v<T, BOOL>) return DataType::BOOL;
    else if constexpr (std::is_same_v<T, STRING>) return DataType::STRING;
    else return DataType::NONE;
}

template <typename T, DataType Exclude>
consteval bool IsExcluded() {
    if constexpr (std::is_same_v<T, bool> && (Exclude == DataType::BOOL)) return true;
    if constexpr (std::is_integral_v<T> && (Exclude == DataType::INTEGER)) return true;
    if constexpr (std::is_floating_point_v<T> && (Exclude == DataType::FLOAT)) return true;
    if constexpr (th::is_string_v<T> && (Exclude == DataType::STRING)) return true;
    else return false;
}



template <typename T, DataType Exclude = DataType::NONE>
concept supported_type =  (std::is_arithmetic_v<T> ||
                          th::string<T>) &&
                          !IsExcluded<T, Exclude>();

template <typename Type, DataType Exclude = DataType::NONE>
concept type_c = supported_type<Type, Exclude>;

}  // namespace DynamicTyping::TypeHelpers

#endif  // TYPES_H

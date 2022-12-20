#ifndef PRIM_TYPES_H
#define PRIM_TYPES_H

#include <cstdint>
#include <type_traits>

#include <cest/string.hpp>

#include <remove_all_const.hpp>

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
inline constexpr bool is_string_v = std::is_same_v<std::remove_cvref_t<T>, string_t> ||
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

constexpr string_t TO_STRING(CString auto&& var) {
    return std::forward<decltype(var)>(var);
}

}  // namespace DynamicTyping::Types

#endif  // PRIM_TYPES_H

#ifndef PRIM_TYPES_H
#define PRIM_TYPES_H

#include <cstdint>
#include <limits>
#include <type_traits>
#include <variant>

#include <cest/string.hpp>

#include <remove_all_const.hpp>

namespace DynamicTyping::Types {

namespace th = Types;

using null_t      = std::nullptr_t;
using integer_t   = int64_t;  // int128_t?
using float_t     = long double;
using string_t    = cest::string;  // constexpr patched std::string
using bool_t      = bool;
using undefined_t = std::monostate;

constexpr float_t NaN = std::numeric_limits<float_t>::quiet_NaN();
constexpr undefined_t undefined;

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

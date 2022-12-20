#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <cstdint>
#include <charconv>
#include <concepts>
#include <type_traits>

#include <types.hpp>

namespace DynamicTyping::StringHelpers {

using namespace Types;

/// @brief Multiplies string implace
/// @param str string to multiply
/// @param times count of repetitions
constexpr void string_multiplication(string_t& str, std::integral auto times) {
    if (times <= 0) throw std::invalid_argument{"Cannot multiply string by non positive integral value"};

    string_t orig{str};
    str.clear();
    str.reserve(orig.length() * times);
    for (std::size_t i = 0; i < static_cast<std::size_t>(times); i++) str += orig;
}

/// @brief Compiletime capable std::to_string
/// @param num arithmetic type to represent as string
/// @return string representation of a given value
constexpr string_t to_string(th::CArithmetic auto num) {
    char str[32];
    std::to_chars(str, std::end(str), num);
    return str;
}

}  // namespace

#endif  // STRING_HELPER_H

#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <charconv>
#include <type_traits>

#include "types.hpp"
#include "common_helpers.hpp"

namespace DynamicTyping::StringHelpers {

using namespace TypeHelpers;

constexpr void string_multiplication(STRING& str, std::integral auto times) {
    if (times <= 0) throw std::invalid_argument{"Cannot multiply string by non positive integral value"};

    STRING orig{str};
    str.clear();
    str.reserve(orig.length() * times);
    for (size_t i = 0; i < times; i++) str += orig;
}

constexpr STRING to_string(th::arithmetic auto num) {
    char str[32];
    std::to_chars(str, std::end(str), num);
    return str;
}

}  // namespace

#endif  // STRING_HELPER_H

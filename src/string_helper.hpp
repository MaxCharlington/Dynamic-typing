#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <string_view>
#include <type_traits>

#include "types.hpp"
#include "common_helpers.hpp"

namespace DynamicTyping::StringHelpers {

using namespace TypeHelpers;


void string_multiplication(STRING& str, std::integral auto times) {
    if (times <= 0) throw std::invalid_argument{"Cannot multiply string by non positive integral value"};

    STRING orig{str};
    str.clear();
    str.reserve(orig.length() * times);
    for (size_t i = 0; i < times; i++) str += orig;
}

}  // namespace

#endif  // STRING_HELPER_H

#pragma once

#include <cstdint>
#include <charconv>
#include <concepts>
#include <type_traits>

#include <types.hpp>

namespace DynamicTyping::StringHelpers {

using namespace Types;

// TODO: str is number check func

// TODO: implement with from_chars with g++13
constexpr std::optional<integer_t> stoll(const char* str, int base = 10) noexcept
{
    integer_t result = 0;
    bool is_negative = false;
    while (*str == ' ' || *str == '\t') ++str; // skip whitespace
    if (*str == '+') ++str;
    else if (*str == '-')
    {
        ++str;
        is_negative = true;
    }
    while (*str != '\0')
    {
        if (*str == ' ' || *str == '\t') break;  // stop if whitespace
        if (*str < '0' || *str > '9') return std::nullopt;
        int digit = *str - '0';
        if (digit >= base) return std::nullopt;
        integer_t new_result = result * base + digit;
        if (new_result < result || new_result > std::numeric_limits<integer_t>::max()) return std::nullopt;
        result = new_result;
        ++str;
    }
    return is_negative ? -result : result;
}

constexpr std::optional<float_t> stold(const char* str) noexcept {
    const char* p = str;
    bool negative = false;
    while (std::isspace(*p)) {
        ++p;
    }
    if (*p == '-') {
        negative = true;
        ++p;
    } else if (*p == '+') {
        ++p;
    }
    float_t val = 0;
    bool has_digits = false;
    while (std::isdigit(*p)) {
        has_digits = true;
        val = val * 10 + (*p - '0');
        ++p;
    }
    if (*p == '.') {
        ++p;
        float_t frac = 0.1;
        while (std::isdigit(*p)) {
            has_digits = true;
            val += frac * (*p - '0');
            frac *= 0.1;
            ++p;
        }
    }
    if (!has_digits) {
        return std::nullopt;
    }
    while (std::isspace(*p)) {
        ++p;
    }
    if (*p != '\0') {
        return std::nullopt;
    }
    if (negative) {
        val = -val;
    }
    if (val > std::numeric_limits<float_t>::max() ||
        val < std::numeric_limits<float_t>::lowest()) {
        return std::nullopt;
    }
    return val;
}

constexpr std::optional<integer_t> stoll(const string_t& str, int base = 10) { return stoll(str.c_str(), base); }

constexpr std::optional<float_t> stold(const string_t& str) noexcept { return stold(str.c_str()); }

/// @brief Compiletime capable std::to_string
/// @param num arithmetic type to represent as string
/// @return string representation of a given value
constexpr string_t to_string(th::Arithmetic auto num) {
    if constexpr (std::same_as<bool_t, decltype(num)>)
    {
        return num ? "true" : "false";
    }
    else if (std::same_as<float_t, decltype(num)> and num == NaN)
    {
        return "NaN";
    }
    else {
        char str[32];
        std::to_chars(str, std::end(str), num);
        return str;
    }
}

}  // namespace

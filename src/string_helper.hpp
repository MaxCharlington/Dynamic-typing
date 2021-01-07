#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <string_view>
#include <type_traits>

template <typename T>
inline constexpr bool is_string_v = std::is_same_v<T, std::string> ||
                                    std::is_same_v<T, char *>;

template <typename T>
concept string = is_string_v<T>;

#endif  // STRING_HELPER_H

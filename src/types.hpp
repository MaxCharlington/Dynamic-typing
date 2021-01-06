#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <string>
#include <variant>

#include "string_helper.hpp"

using INTEGER = int64_t;
using FLOAT = long double;
using STRING = std::string;
using BOOL = bool;

#define TO_INTEGER(var) (static_cast<INTEGER>(var))
#define TO_FLOAT(var)   (static_cast<FLOAT>(var))
#define TO_STRING(var)  (static_cast<STRING>(var))
#define TO_BOOL(var)    (static_cast<BOOL>(var))


template <typename T>
concept supported_types_checker = std::integral<std::remove_cvref_t<T>> || 
                                  std::floating_point<std::remove_cvref_t<T>> ||
                                  string<std::remove_cvref_t<T>>;
// std::integral includes bool type

template <typename T>
concept type_c = supported_types_checker<T>;

using data_t = std::variant<INTEGER, FLOAT, BOOL, STRING>;

#endif  // TYPES_H

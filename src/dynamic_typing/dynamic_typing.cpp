#include <iostream>
#include <variant>
#include <concepts>
#include <type_traits>

#include "dynamic_typing.hpp"

var::var(data_t &&data_variant) : data(std::move(data_variant)) {}

std::ostream &operator<<(std::ostream &os, const var &variable)
{
    std::visit([&](auto &&arg) { os << arg; }, variable.data);
    return os;
}

#ifndef COMMON_HELPERS_H
#define COMMON_HELPERS_H

#include <type_traits>
#include <stdexcept>

namespace DynamicTyping::CommonHelpers {

template <typename T>
struct remove_all_const : std::remove_const<T> {};

template <typename T>
struct remove_all_const<T*> {
    using type = remove_all_const<T>::type*;
};

template <typename T>
struct remove_all_const<T * const> {
    using type = remove_all_const<T>::type*;
};

template <typename T>
using remove_const_t = remove_all_const<T>::type;


template <bool condition>
consteval auto assert_impl(auto msg) {
    if constexpr (!condition) {
        throw std::invalid_argument{msg};
    }
}

#define dt_assert(condition, msg) assert_impl<condition>(msg)

}  // namespace DynamicTyping::CommonHelpers

#endif  // COMMON_HELPERS_H

#pragma once

#include <types.hpp>

namespace DynamicTyping::CTRHelper {

using namespace Types;

template<typename Arr>
concept is_array = requires(Arr arr) {  // TODO: improve
    std::begin(arr);
    std::end(arr);
    arr[0];
};

/// @brief Trims excess data from fields representing arrays
/// @tparam Index current field index
/// @param obj current object representation tuple
/// @return Trimmed tuple representing compiletime object
template<std::invocable auto F, std::size_t Index = 0>
consteval auto to_runtime_transform(auto obj)
{
    static_assert(requires{F().to_runtime();}, "F callable should return object that has method to_runtime() returning tuple representing object");

    constexpr auto data = Object(F().to_runtime());
    if constexpr (Index < data.size())
    {
        constexpr auto field = data.template get_field<Index>();
        constexpr auto field_val = Field{field}.value();
        constexpr auto field_name = Field{field}.name().get();
        if constexpr (is_array<decltype(field_val)>)
        {
            static_assert(Index + 1 < data.size());
            constexpr auto array_size = data.template get<Index + 1>();
            std::array<char, array_size> tmp_arr;
            std::copy_n(std::begin(field_val), array_size, tmp_arr.data());
            auto arr_field  = make_field<DataType::NATIVE, field_name>(tmp_arr);

            return to_runtime_transform<F, Index + 2>(append(obj, arr_field));
        }
        else  // if trivial
        {
            constexpr auto field = data.template get_field<Index>();
            return to_runtime_transform<F, Index + 1>(append(obj, field));
        }
        // non recursive for now
    }
    else
    {
        return Object{obj};
    }
}

/// @brief Implements compile time known data conversion to runtime
/// @return tuple representing compiletime object with trimmed data, from which you
///         can create runtime object
template<std::invocable auto F>
consteval auto to_runtime()
{
    return to_runtime_transform<F>(make_obj());
}

}  // DynamicTyping::CTRHelper

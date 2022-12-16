#pragma once

#include <array>

#include <types.hpp>

namespace DynamicTyping::CTRHelper {

using namespace Types;

template<typename Arr>
concept CStdArray = requires(Arr arr) {
    std::begin(arr);
    std::end(arr);
};

/// @brief Trims excess data from fields representing arrays
/// @tparam Index current field index
/// @param obj current object representation tuple
/// @return Trimmed tuple representing compiletime object
template<std::invocable auto F, std::size_t Index = 0> requires requires{ {F().to_runtime()} -> CObjectData;}
constexpr auto to_runtime_transform(CObjectData auto obj)
{
    constexpr auto data = Object(F().to_runtime());
    if constexpr (Index < data.size())
    {
        constexpr auto field = data.template get_field<Index>();
        constexpr auto field_val = Field{field}.value();
        using field_type = decltype(field_val);

        if constexpr (CStdArray<field_type>)
        {
            static_assert(Index + 1 < data.size(), "Array is required to be followed by size field");
            constexpr auto array_size = data.template get<Index + 1>();
            std::array<typename field_type::value_type, array_size> tmp_arr;
            std::copy_n(std::begin(field_val), array_size, tmp_arr.data());
            constexpr auto field_name = Field{field}.name().get();
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
template<std::invocable auto F> requires requires{ {F().to_runtime()} -> CObjectData;}
constexpr auto to_runtime()
{
    return to_runtime_transform<F>(make_obj());
}

}  // DynamicTyping::CTRHelper

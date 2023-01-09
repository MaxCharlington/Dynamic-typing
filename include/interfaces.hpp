#pragma once

#include <dynamic_typing.hpp>

namespace DynamicTyping::Interfaces {

template<typename T>
concept CVariable = requires (T v){
    {v + 1} -> std::same_as<data_t>;
};

template<typename T>
concept CArray = requires (T arr){
    {arr[0]} -> std::same_as<const var&>;
};

template<typename T>
concept CObject  = requires (T obj, const char* field_name){
    {obj[field_name]} -> std::same_as<const var&>;
};

#define IVariable CVariable auto
#define IArray CArray auto
#define IObject CObject auto

}  // namespace DynamicTyping::Interfaces

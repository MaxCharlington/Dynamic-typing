#pragma once

#include <dynamic_typing.hpp>

namespace DynamicTyping::Interfaces {

template<typename T>
concept CArray = requires (T arr){
    {arr[0]} -> std::same_as<var&>;
    {arr.at(-1)} -> std::same_as<var&>;
    {arr.length()} -> std::same_as<var>;
};

template<typename T>
concept CObject  = requires (T obj, const char* field_name){
    {obj[field_name]} -> std::same_as<var&>;
    {obj.keys()} -> std::same_as<var>;
};

// #define IArray CArray auto
// #define IObject CObject auto

}  // namespace DynamicTyping::Interfaces

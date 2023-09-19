#pragma once

#include <dynamic_typing.hpp>

// Interfaces to check JS functions complience
namespace DynamicTyping::Interfaces {

template<typename T>
concept Array = requires (T arr){
    {arr[0]} -> std::same_as<var&>;
    {arr.at(-1)} -> std::same_as<var&>;
    {arr.length()} -> std::same_as<var>;
};

template<typename T>
concept Object  = requires (T obj, const char* field_name){
    {obj[field_name]} -> std::same_as<var&>;
    {obj.keys()} -> std::same_as<var>;
};

template<typename T>
concept String = requires (T arr){
    {arr.length()} -> std::same_as<var>;
};

}  // namespace DynamicTyping::Interfaces

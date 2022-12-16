#ifndef TYPES_H
#define TYPES_H

#include <algorithm>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

#include <template_strings.hpp>

#include "primitive_types.hpp"


namespace DynamicTyping::Types {

namespace th = Types;


template<typename>
struct is_tuple: std::false_type {};

template<typename ...T>
struct is_tuple<std::tuple<T...>>: std::true_type {};

template<typename T>
constexpr bool is_tuple_v = is_tuple<T>::value;

template<typename T>
concept CIsTuple = is_tuple_v<T>;


using ObjTag = struct{};
using FieldTag = struct{};
using ArrayTag = struct{};

static constexpr bool is_object_data_v(auto) { return false; }

template<typename ...T>
static constexpr bool is_object_data_v(std::tuple<ObjTag, T...>) { return true; }

template<typename T>
concept CObjectData = is_object_data_v(T{});


static constexpr bool is_field_data_v(auto) { return false; }

template<typename ...T>
static constexpr bool is_field_data_v(std::tuple<FieldTag, T...>) { return true; }

template<typename T>
concept CFieldData = is_field_data_v(T{});


static constexpr bool is_array_data_v(auto) { return false; }

template<typename ...T>
static constexpr bool is_array_data_v(std::tuple<ArrayTag, T...>) { return true; }

template<typename T>
concept CArrayData = is_array_data_v(T{});


template<DataType dt>
constexpr auto make_value(auto value)
{
    if constexpr (dt == DataType::NATIVE) return value;
    else return type<dt>{value};
}

template<DataType dt, StringLiteral name>
constexpr auto make_field(auto field_value)
{
    return std::make_tuple(FieldTag{}, String<name>{}, make_value<dt>(field_value));
}

template<StringLiteral name>
constexpr auto make_field(auto value)
{
    return std::make_tuple(FieldTag{}, String<name>{}, value);
}

template<typename ...Fields>
constexpr auto make_obj(Fields ...fs)
{
    return std::make_tuple(ObjTag{}, fs...);
}

template<typename ...Vals>
constexpr auto make_array(Vals... vals)
{
    return std::make_tuple(ArrayTag{}, vals...);
}

template<typename Appendee, typename New>
constexpr auto append(Appendee a, New new_el)
{
    if ((CObjectData<Appendee> and CFieldData<New>) or CArrayData<Appendee>)
        return std::tuple_cat(a, std::make_tuple(new_el));
}


template<CFieldData F>
class Field
{
public:
    constexpr Field(F field) : m_field{field} {}

    constexpr auto name() const noexcept
    {
        return std::get<1>(m_field);
    }

    constexpr auto value() const noexcept
    {
        return std::get<2>(m_field);
    }

private:
    F m_field;
};


template<CArrayData Arr>
class Array
{
public:
    constexpr Array(Arr array) : m_array{array} {}

    template<size_t I>
    constexpr auto get() const
    {
        static_assert(I + 1 < std::tuple_size_v<Arr>);
        return get_impl<I>(m_array);
    }

    constexpr size_t size() const noexcept
    {
        return size_impl();
    }

private:
    template<size_t Index, size_t I = 1>
    constexpr static auto get_impl(Arr arr)
    {
        if constexpr (I == Index + 1)
        {
            return std::get<I>(arr);
        }
        else
        {
            return get_impl<Index, I + 1>(arr);
        }
    }

    constexpr static size_t size_impl()
    {
        return std::tuple_size_v<Arr> - 1 /* -1 for tag */;
    }

    const Arr m_array;
};


template<CObjectData Obj>
class Object
{
public:
    constexpr Object(Obj object) : m_object{object} {}

    template<StringLiteral field_name>
    constexpr auto get() const
    {
        return get_impl<field_name>(m_object);
    }

    template<size_t I>
    constexpr auto get() const
    {
        static_assert(I + 1 < std::tuple_size_v<Obj>);
        return get_impl<I>(m_object);
    }

    template<size_t I>
    constexpr auto get_field() const
    {
        static_assert(I + 1 < std::tuple_size_v<Obj>);
        return get_field_impl<I>(m_object);
    }

    constexpr size_t size() const noexcept
    {
        return size_impl();
    }

private:
    template<StringLiteral field_name, size_t I = 1>
    constexpr static auto get_impl(Obj obj)
    {
        if constexpr (I <= size_impl())
        {
            constexpr auto field = Field{std::get<I>(Obj{})};
            if constexpr (field_name == field.name())
            {
                return Field{std::get<I>(obj)}.value();
            }
            else
            {
                return get_impl<field_name, I + 1>(obj);
            }
        }
    }

    template<size_t Index, size_t I = 1>
    constexpr static auto get_impl(Obj obj)
    {
        if constexpr (I == Index + 1)
        {
            return Field{std::get<I>(obj)}.value();
        }
        else
        {
            return get_impl<Index, I + 1>(obj);
        }
    }

    template<size_t Index, size_t I = 1>
    constexpr static auto get_field_impl(Obj obj)
    {
        if constexpr (I == Index + 1)
        {
            return std::get<I>(obj);
        }
        else
        {
            return get_field_impl<Index, I + 1>(obj);
        }
    }

    constexpr static size_t size_impl()
    {
        return std::tuple_size_v<Obj> - 1 /* -1 for tag */;
    }

    const Obj m_object;
};

template<typename>
struct is_field: std::false_type {};

template<CFieldData T>
struct is_field<Field<T>>: std::true_type {};

template<typename T>
constexpr bool is_field_v = is_field<T>::value;

template<typename T>
concept CField = is_field_v<T>;


template<typename>
struct is_array: std::false_type {};

template<CArrayData T>
struct is_array<Array<T>>: std::true_type {};

template<typename T>
constexpr bool is_array_v = is_array<T>::value;

template<typename T>
concept CArray = is_array_v<T>;


template<typename>
struct is_object: std::false_type {};

template<CObjectData T>
struct is_object<Object<T>>: std::true_type {};

template<typename T>
constexpr bool is_object_v = is_object<T>::value;

template<typename T>
concept CObject = is_object_v<T>;


}  // namespace DynamicTyping::Types

#endif  // TYPES_H

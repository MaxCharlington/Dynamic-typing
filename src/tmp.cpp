#include "types.hpp"
#include <template_strings.hpp>

#include <algorithm>
#include <cstdint>
#include <functional>
// #include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

using namespace DynamicTyping::TypeHelpers;

template<DataType dt>
consteval auto make_type()
{
    if constexpr (dt == DataType::NONE)
    {
        return;
    }
    else if constexpr (dt == DataType::INTEGER)
    {
        return INTEGER{};
    }
    else if constexpr (dt == DataType::FLOAT)
    {
        return FLOAT{};
    }
    else if constexpr (dt == DataType::BOOL)
    {
        return BOOL{};
    }
    else if constexpr (dt == DataType::STRING)
    {
        return STRING{};
    }
}

template<DataType dt>
using type = decltype(make_type<dt>());

using ObjTag = struct{};
using FieldTag = struct{};
using ArrayTag = struct{};


template<typename ...T>
static consteval bool is_object(std::tuple<T...>) { return false; }

template<typename ...T>
static consteval bool is_object(std::tuple<ObjTag, T...>) { return true; }

template<typename ...T>
static consteval bool is_field(std::tuple<T...>) { return false; }

template<typename ...T>
static consteval bool is_field(std::tuple<FieldTag, T...>) { return true; }

template<typename ...T>
static consteval bool is_array(std::tuple<T...>) { return false; }

template<typename ...T>
static consteval bool is_array(std::tuple<ArrayTag, T...>) { return true; }

template<DataType dt>
consteval auto make_value(auto value)
{
    return type<dt>{value};
}

template<StringLiteral s>
using FieldNameHash = HashedStr<s>;

template<DataType dt, StringLiteral name>
consteval auto make_field(auto field_value)
{
    return std::make_tuple(FieldTag{}, FieldNameHash<name>{}, std::string_view{name.value}, make_value<dt>(field_value));
}

template<StringLiteral name>
consteval auto make_field(auto value)
{
    return std::make_tuple(FieldTag{}, FieldNameHash<name>{}, std::string_view{name.value}, value);
}

template<typename ...Fields>
consteval auto make_obj(Fields ...fs)
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
    if ((is_object(Appendee{}) and is_field(New{})) or is_array(Appendee{}))
        return std::tuple_cat(a, std::make_tuple(new_el));
}

template<typename F>
class Field
{
public:
    constexpr Field(F field) : m_field{field} { static_assert(is_field(F{})); }

    constexpr size_t name_hash() const noexcept
    {
        return std::get<1>(F{}).hash();
    }

    constexpr auto name() const noexcept -> std::string_view
    {
        return std::get<2>(m_field);
    }

    constexpr auto value() const noexcept
    {
        return std::get<3>(m_field);
    }

private:
    F m_field;
};

template<typename Arr>
class Array
{
public:
    constexpr Array(Arr array) : m_array{array} { static_assert(is_array(Arr{})); }

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

template<typename Obj>
class Object
{
public:
    constexpr Object(Obj object) : m_object{object} { static_assert(is_object(Obj{})); }

    template<StringLiteral field_name>
    constexpr auto get() const
    {
        return get_impl<field_name>(m_object);
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
            if constexpr (FieldNameHash<field_name>{}.hash() == field.name_hash())
            {
                return Field{std::get<I>(obj)}.value();
            }
            else
            {
                return get_impl<field_name, I + 1>(obj);
            }
        }
    }

    constexpr static size_t size_impl()
    {
        return std::tuple_size_v<Obj> - 1 /* -1 for tag */;
    }

    const Obj m_object;
};

int main()
{
    constexpr auto num_val    = make_value<DataType::INTEGER>(10);

    constexpr auto num_field  = make_field<"num">(num_val);
    static_assert(is_field(num_field));

    constexpr auto flag_field = make_field<DataType::BOOL, "flag">(false);
    static_assert(is_field(flag_field));

    constexpr auto obj        = make_obj(num_field, flag_field);
    static_assert(is_object(obj));
    // std::cout << std::get<1>(std::get<0>(obj)) << '\n';

    constexpr auto some_bool  = make_value<DataType::BOOL>(true);
    constexpr auto arr        = make_array(num_val, some_bool);
    constexpr auto a = Array(arr);
    static_assert(a.get<1>() == true);

    constexpr auto arr_field  = make_field<"array">(arr);

    constexpr auto obj2 = append(obj, arr_field);

    static_assert(is_object(obj2));

    constexpr auto o = Object(obj2);
    static_assert(o.get<"num">() == 10);
}

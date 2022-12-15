#ifndef TYPES_H
#define TYPES_H

#include <algorithm>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include <template_strings.hpp>
#include <cest/string.hpp>

#include "common_helpers.hpp"

namespace DynamicTyping::Types {

namespace th = Types;
namespace ch = CommonHelpers;


using integer_t   = int64_t; // int128_t?
using float_t     = long double;
using string_t    = cest::string;
using bool_t      = bool;

using data_t    = std::variant<integer_t, float_t, bool_t, string_t>;


// Arithmetic concept
template <typename T> concept CArithmetic = std::is_arithmetic_v<T>;

// String like types
template <typename T>
inline constexpr bool is_string_v = std::is_same_v<T, string_t> ||
                                    std::is_same_v<ch::remove_const_t<std::decay_t<T>>, char *>;

template <typename T>
concept CString = is_string_v<T>;


// Conversions
constexpr integer_t TO_INTEGER(std::integral auto var) {
    return static_cast<integer_t>(var);
}

constexpr float_t TO_FLOAT(std::floating_point auto var) {
    return static_cast<float_t>(var);
}

constexpr string_t TO_STRING(CString auto var) {
    return var;
}

// Exclusion mechanism
enum class DataType {
    NONE,
    NILL,
    UNDEFINED,
    INTEGER,
    FLOAT,
    BOOL,
    STRING,
    ARRAY,
    OBJECT,
    NATIVE
};

constexpr const char* DataTypeStrRepr(DataType t) {
    switch (t) {
        case DataType::NONE: return "NONE";
        case DataType::NILL: return "NILL";
        case DataType::UNDEFINED: return "UNDEFINED";
        case DataType::INTEGER: return "INTEGER";
        case DataType::FLOAT: return "FLOAT";
        case DataType::BOOL: return "BOOL";
        case DataType::STRING: return "STRING";
        default: return nullptr;
    }
}

template <typename T>
consteval auto data_type() -> DataType {
    if constexpr (std::is_same_v<T, integer_t>) return DataType::INTEGER;
    else if constexpr (std::is_same_v<T, float_t>) return DataType::FLOAT;
    else if constexpr (std::is_same_v<T, bool_t>) return DataType::BOOL;
    else if constexpr (std::is_same_v<T, string_t>) return DataType::STRING;
    else return DataType::NONE;
}

template <typename T, DataType Exclude>
consteval bool IsExcluded() {
    if constexpr (std::is_same_v<T, bool> && (Exclude == DataType::BOOL)) return true;
    if constexpr (std::is_integral_v<T> && (Exclude == DataType::INTEGER)) return true;
    if constexpr (std::is_floating_point_v<T> && (Exclude == DataType::FLOAT)) return true;
    if constexpr (is_string_v<T> && (Exclude == DataType::STRING)) return true;
    else return false;
}


template <typename T, DataType Exclude = DataType::NONE>
concept CSupportedType =  (CArithmetic<T> ||
                           CString<T>) &&
                           !IsExcluded<T, Exclude>();

template <typename Type, DataType Exclude = DataType::NONE>
concept CType = CSupportedType<Type, Exclude>;

template<DataType dt>
constexpr auto make_type()
{
    using enum DataType;
    if constexpr (dt == NONE or dt == NILL or dt == UNDEFINED)
        return;
    else if constexpr (dt == INTEGER)
        return integer_t{};
    else if constexpr (dt == FLOAT)
        return float_t{};
    else if constexpr (dt == BOOL)
        return bool_t{};
    else if constexpr (dt == STRING)
        return string_t{};
}

template<DataType dt>
using type = decltype(make_type<dt>());

// template<typename T>
// constexpr auto make_type()
// {
//     if constexpr (std::is_same_v<T, bool>) return bool_t{};
//     if constexpr (std::is_integral_v<T>) return integer_t{};
//     if constexpr (std::is_floating_point_v<T>) return float_t{};
//     if constexpr (is_string_v<T>) return string_t{};
//     else return;
// }

// template<typename T>
// using type = decltype(make_type<T>());

using ObjTag = struct{};
using FieldTag = struct{};
using ArrayTag = struct{};

static constexpr bool is_object_data(auto) { return false; }

template<typename ...T>
static constexpr bool is_object_data(std::tuple<ObjTag, T...>) { return true; }

template<typename T>
concept CObjectData = is_object_data(T{});


static constexpr bool is_field_data(auto) { return false; }

template<typename ...T>
static constexpr bool is_field_data(std::tuple<FieldTag, T...>) { return true; }

template<typename T>
concept CFieldData = is_field_data(T{});


static constexpr bool is_array_data(auto) { return false; }

template<typename ...T>
static constexpr bool is_array_data(std::tuple<ArrayTag, T...>) { return true; }

template<typename T>
concept CArrayData = is_array_data(T{});

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

// template<typename T, typename FieldData>
// concept CField = std::is_same_v<T, Field<FieldData>> and CFieldData<FieldData>;

// template<typename T, typename ArrayData>
// concept CArray = std::is_same_v<T, Array<ArrayData>> and CArrayData<ArrayData>;

// template<typename T, typename ObjData>
// concept CObject = std::is_same_v<T, Object<ObjData>> and CObjectData<ObjData>;

}  // namespace DynamicTyping::Types

#endif  // TYPES_H

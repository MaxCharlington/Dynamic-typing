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

enum class DataType {
    NONE,
    INTEGER,
    FLOAT,
    BOOL,
    STRING
};

using INTEGER   = int64_t; // int128_t?
using FLOAT     = long double;
using STRING    = std::string;
using BOOL      = bool;

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

template<DataType dt>
consteval auto make_field(std::string_view name, auto field_value)
{
    return std::make_tuple(FieldTag{}, name, make_value<dt>(field_value));
}

consteval auto make_field(std::string_view name, auto value)
{
    return std::make_tuple(FieldTag{}, name, value);
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

/**
 * Literal class type that wraps a constant expression string
 *
 * Uses implicit conversion to allow templates to accept constant strings
 */
template<std::size_t N>
struct StringLiteral {
    constexpr StringLiteral(const char (&str)[N]) noexcept
    {
        std::copy_n(str, N, value);
    }

    constexpr explicit StringLiteral(std::string_view str) noexcept 
    {
        if (std::is_constant_evaluated())
        {
            if (str.size() + 1 != N) throw "Wrong length template param provided";
        }
        std::copy_n(str.data(), N, value);
    }

    constexpr bool operator==(std::string_view str) const
    {
        return str == value;
    }

    constexpr const char* c_str() const noexcept
    {
        return value;
    }

    char value[N];
};

template<typename F>
class Field
{
public:
    constexpr Field(F field) : m_field{field} { static_assert(is_field(F{})); }

    constexpr auto name() const noexcept -> std::string_view
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

template<typename Obj>
class Object
{
public:
    constexpr Object(Obj object) : m_object{object} { static_assert(is_object(Obj{})); }

    template<typename T, StringLiteral field_name>
    constexpr auto get() const
    {
        auto val = get_impl<T, field_name>(m_object);
        if (val.has_value())
        {
            return *val;
        }
    }

private:
    template<typename T, StringLiteral field_name, size_t I = 1>
    constexpr static auto get_impl(Obj obj) -> std::optional<T>
    {
        if constexpr (I < std::tuple_size_v<Obj>)
        {
            auto field = Field{std::get<I>(obj)};
            if (field_name == field.name())
            {
                if constexpr (std::is_same_v<decltype(field.value()), T>)
                    return field.value();
                else
                    return std::nullopt;
            }
            else
            {
                return get_impl<T, field_name, I + 1>(obj);
            }
        }
        return std::nullopt;
    }

    const Obj m_object;
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

    const Arr m_array;
};

int main()
{
    constexpr auto num_val    = make_value<DataType::INTEGER>(10);

    constexpr auto num_field  = make_field("num", num_val);
    static_assert(is_field(num_field));

    constexpr auto flag_field = make_field<DataType::BOOL>("flag", false);
    static_assert(is_field(flag_field));

    constexpr auto obj        = make_obj(num_field, flag_field);
    static_assert(is_object(obj));
    // std::cout << std::get<1>(std::get<0>(obj)) << '\n';

    constexpr auto some_bool  = make_value<DataType::BOOL>(true);
    constexpr auto arr        = make_array(num_val, some_bool);
    constexpr auto a = Array(arr);
    static_assert(a.get<1>() == true);

    constexpr auto arr_field  = make_field("array", arr);

    // std::cout << std::get<1>(arr);

    constexpr auto obj2 = append(obj, arr_field);

    // std::cout << std::get<1>(std::get<1>(std::get<2>(obj2)));

    static_assert(is_object(obj2));

    constexpr auto o = Object(obj2);
    static_assert(o.get<INTEGER, "num">() == 10);
}

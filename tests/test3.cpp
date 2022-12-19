#include <template_strings.hpp>
#include <types.hpp>
#include <blob_types.hpp>

using namespace DynamicTyping::Types;
using namespace DynamicTyping::Types::Blob;

int main()
{
    constexpr auto num_val    = make_value<DataType::INTEGER>(10);

    constexpr auto num_field  = make_field<"num">(num_val);
    static_assert(is_field_data_v(num_field));

    constexpr auto flag_field = make_field<DataType::BOOL, "flag">(false);
    static_assert(is_field_data_v(flag_field));

    constexpr auto obj        = make_obj(num_field, flag_field);
    static_assert(is_object_data_v(obj));
    // std::cout << std::get<1>(std::get<0>(obj)) << '\n';

    constexpr auto some_bool  = make_value<DataType::BOOL>(true);
    constexpr auto arr        = make_array(num_val, some_bool);
    constexpr auto a = Array(arr);
    static_assert(a.get<1>() == true);

    constexpr auto arr_field  = make_field<"array">(arr);

    constexpr auto obj2       = append(obj, arr_field);
    make_obj();

    static_assert(is_object_data_v(obj2));

    constexpr auto o = Object(obj2);
    static_assert(o.get<"num">() == 10);
    static_assert(o.get<0>() == 10);
}

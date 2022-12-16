#include <algorithm>
#include <concepts>
#include <iostream>
#include <tuple>

#include <types.hpp>
#include <constexpr_to_runtime_helper.hpp>

using namespace DynamicTyping::Types;
namespace ctr = DynamicTyping::CTRHelper;

struct S
{
    constexpr S() = default;
    constexpr S(const std::string& s, int n) : str{s}, num{n} {}

    constexpr S(CObject auto data)  // no length for str as it was trimmed
        : str{data.template get<"str">().data()}
        , num{data.template get<"num">()}
    {}

    std::string str;
    int num;

    // Special method to create constexpr capable representation of object
    consteval auto to_runtime() const
    {
        auto num_field  = make_field<DataType::NATIVE, "num">(num);

        const std::size_t size = str.length() + 1;  // Null terminator
        std::array<char, 64> tmp_str;  // Size should be as small as possible
        std::copy_n(str.begin(), size, tmp_str.data());
        auto str_field  = make_field<DataType::NATIVE, "str">(tmp_str);

        // after array add length field
        auto strlen_field  = make_field<DataType::NATIVE, "strlen">(size);

        return make_obj(num_field, str_field, strlen_field);
    }
};

constexpr auto f()
{
    S s{"asdas", 5};
    s.str = "mama";
    return s;
}

int main()
{
    constexpr auto d = ctr::to_runtime<f>();
    std::cout << sizeof(d) << '\n';
    S s = d;
    puts(s.str.data());
    return s.num;
}

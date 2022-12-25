#include <ast_impl.hpp>
#include <ast.hpp>
#include "runtime.hpp"

consteval auto prepare_runtime()  // -> std::array<std::function<void(void)>, COUNT_OF_OPS>
{
    constexpr auto& prog = compiled_json::prog::impl::document;
    static_assert(prog["type"].get<std::string_view>() == "Program");
    Runtime rntm{prog["body"]};
    return to_runtime(rntm);
    // static_assert(prog["body"][0]["type"].get<std::string_view>() == "VariableDeclaration");
}

auto [r, ops, repr] = prepare_runtime();
using R = std::remove_cvref_t<decltype(r)>;

template<>
std::array<integer_t, 1> R::s_integral_constants = repr;

int main()
{
    for (auto &op : ops)
    {
        op.template operator()<R>();
    }
}

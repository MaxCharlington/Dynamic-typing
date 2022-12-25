#pragma once

#include <vector>
#include <dynamic_typing.hpp>

using namespace DynamicTyping;
using namespace DynamicTyping::Types;


struct runtime_op {
    std::size_t i;

    template<class R>
    void operator()()
    {
        std::cout << R::s_integral_constants[i];
    }
};

template<std::size_t OpSize = 0, std::size_t IntSize = 0>
class Runtime
{
    struct runtime_op_creator
    {
        std::string_view name;

        constexpr runtime_op operator()() const
        {
            return runtime_op{0};
        }
    };
    std::vector<std::pair<std::string_view, std::size_t>> integral_constants_indexes;
    std::vector<integer_t> integral_constants;
public:
    std::vector<runtime_op_creator> ops;
    static std::array<integer_t, IntSize> s_integral_constants;

    constexpr Runtime() = default;

    constexpr Runtime(const auto& prog_body)
    {
        for (auto &body_item : prog_body)
        {
            if (body_item["type"].template get<std::string_view>() == "VariableDeclaration")
            {
                integral_constants.push_back(body_item["declarations"][0]["init"]["value"].template get<integer_t>());
                integral_constants_indexes.push_back({body_item["declarations"][0]["id"]["name"].template get<std::string_view>(), integral_constants.size() - 1});
            }
            else if (body_item["type"].template get<std::string_view>() == "ExpressionStatement")
            {
                ops.push_back(runtime_op_creator{body_item["expression"]["arguments"][0]["name"].template get<std::string_view>()});
            }
        }
    }

    constexpr auto to_runtime() const
    {
        return std::array<integer_t, 1>{integral_constants[0]};
    }
};

constexpr auto to_runtime(const Runtime<>& r)
{
    auto repr = r.to_runtime();
    std::array<runtime_op, 1> ops = {r.ops[0]()};
    return std::make_tuple(Runtime<1, 1>{}, ops, repr);
}

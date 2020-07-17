#ifndef VARIANT_HELPER_H
#define VARIANT_HELPER_H

namespace std
{
    template <class... Ts>
    struct overloaded : Ts...
    {
        using Ts::operator()...;
    };

    template <class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;

} // namespace std

#endif
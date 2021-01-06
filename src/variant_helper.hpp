#ifndef VARIANT_HELPER_H
#define VARIANT_HELPER_H

template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};

#endif
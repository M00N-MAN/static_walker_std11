#pragma once

#include <tuple>
#include <array>
#include <cassert>

namespace under
{
template<std::size_t LEFT, std::size_t RIGHT>
struct get_impl
{
    template<typename Tuple, typename Functor, typename ...Args>
    static void logWalk(Tuple const &tuple, std::size_t index, Functor &fun, Args &&...args)
    {
        static constexpr std::size_t MIDDLE = (RIGHT - LEFT) / 2 + LEFT;
        if      (index < MIDDLE)     get_impl<LEFT , MIDDLE>::logWalk(tuple, index, fun, std::forward<Args>(args)...);
        else if (index > MIDDLE)     get_impl<MIDDLE, RIGHT>::logWalk(tuple, index, fun, std::forward<Args>(args)...);
        else /* (index == MIDDLE) */ fun(std::get<MIDDLE>(tuple), std::forward<Args>(args)...);
    }
};

template<std::size_t LEFT, std::size_t RIGHT>
struct range_impl
{
    template<typename Tuple, typename Functor, typename ...Args>
    static void lineWalk(Tuple const &, std::size_t, std::size_t, Functor &,std::integral_constant<std::size_t, RIGHT>, Args &&...)
    {}

    template<typename Tuple, typename Functor, typename ...Args,
        std::size_t CURRENT = LEFT>
    static void lineWalk(Tuple const &tuple, std::size_t begin, std::size_t end, Functor &fun,
        std::integral_constant<std::size_t, CURRENT> /* = std::integral_constant<std::size_t, 0>()*/, Args &&...args)
    {
        if (CURRENT >= begin && CURRENT <= end)
            fun(std::get<CURRENT>(tuple), std::forward<Args>(args)...);

        if(CURRENT <= end)
            lineWalk(tuple,begin,end,fun
                ,std::integral_constant<std::size_t, CURRENT + 1>(),std::forward<Args>(args)...);
    }
};
}//under

template<typename Tuple,typename Functor, typename ...Args>
void runtime_get(const Tuple& tup, std::size_t idx, Functor fun, Args &&...args)
{
    assert(idx < std::tuple_size<Tuple>::value);
    under::get_impl<0,std::tuple_size<Tuple>::value>::logWalk(tup, idx, fun, std::forward<Args>(args)...);
}
template<typename Tuple,typename Functor, typename ...Args>
void runtime_get(Tuple& tup, std::size_t idx, Functor fun, Args &&...args)
{
    assert(idx < std::tuple_size<Tuple>::value);
    under::get_impl<0,std::tuple_size<Tuple>::value>::logWalk(tup, idx, fun, std::forward<Args>(args)...);
}

template<typename Tuple,typename Functor, typename ...Args>
void runtime_range(const Tuple& tup, std::size_t begin, std::size_t end, Functor fun, Args &&...args)
{
    assert(begin < std::tuple_size<Tuple>::value && end < std::tuple_size<Tuple>::value);
    under::range_impl<0u,std::tuple_size<Tuple>::value>::lineWalk(tup, begin, end, fun,std::integral_constant<std::size_t, 0>(), std::forward<Args>(args)...);
}
template<typename Tuple,typename Functor, typename ...Args>
void runtime_range(Tuple& tup, std::size_t begin, std::size_t end, Functor fun, Args &&...args)
{
    assert(begin < std::tuple_size<Tuple>::value && end < std::tuple_size<Tuple>::value);
    under::range_impl<0u,std::tuple_size<Tuple>::value>::lineWalk(tup, begin, end, fun,std::integral_constant<std::size_t, 0>(), std::forward<Args>(args)...);
}

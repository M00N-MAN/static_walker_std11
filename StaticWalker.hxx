#pragma once

#include <tuple>
#include <array>
#include <cassert>
#include <utility>

namespace StaticWalkerImplNs_
{
template<std::size_t LEFT, std::size_t RIGHT>
struct get_impl
{
    template<typename Container, typename Functor, typename ...Args>
    static typename std::enable_if<std::is_same<typename std::result_of<Functor(/*1st type is dummy*/char,Args...)>::type, void>::value, void>::type
    logWalk(Container &&tuple, std::size_t index, Functor &&fun, Args &&...args)
    {
        static constexpr std::size_t MIDDLE = (RIGHT - LEFT) / 2 + LEFT;
        if      (index < MIDDLE)     get_impl<LEFT , MIDDLE>::logWalk(tuple, index, fun, std::forward<Args>(args)...);
        else if (index > MIDDLE)     get_impl<MIDDLE, RIGHT>::logWalk(tuple, index, fun, std::forward<Args>(args)...);
        else /* (index == MIDDLE) */ std::forward<Functor>(fun)(std::get<MIDDLE>(std::forward<Container>(tuple)), std::forward<Args>(args)...);
    }

    template<typename Container, typename Functor, typename ...Args>
    static typename std::enable_if<!std::is_same<typename std::result_of<Functor(/*1st type is dummy*/char,Args...)>::type, void>::value, typename std::result_of<Functor(/*1st type is dummy*/char,Args...)>::type>::type
    logWalk(Container &&tuple, std::size_t index, Functor &&fun, Args &&...args)
    {
        static constexpr std::size_t MIDDLE = (RIGHT - LEFT) / 2 + LEFT;
        if      (index < MIDDLE)     return get_impl<LEFT , MIDDLE>::logWalk(tuple, index, fun, std::forward<Args>(args)...);
        else if (index > MIDDLE)     return get_impl<MIDDLE, RIGHT>::logWalk(tuple, index, fun, std::forward<Args>(args)...);
        //else /* (index == MIDDLE) */
        return std::forward<Functor>(fun)(std::get<MIDDLE>(std::forward<Container>(tuple)), std::forward<Args>(args)...);
    }
};

template<std::size_t LEFT, std::size_t RIGHT>
struct range_impl
{
    template<typename Container, typename Functor, typename ...Args>
    static void lineWalk(Container &&, std::size_t, std::size_t, Functor &&,std::integral_constant<std::size_t, RIGHT>, Args &&...)
    {}

    template<typename Container, typename Functor, typename ...Args, std::size_t CURRENT = LEFT>
    static void lineWalk(Container &&tuple, std::size_t begin, std::size_t end, Functor &&fun,
        std::integral_constant<std::size_t, CURRENT> /* = std::integral_constant<std::size_t, 0>()*/, Args &&...args)
    {
        if (CURRENT >= begin && CURRENT <= end)
            std::forward<Functor>(fun)(std::get<CURRENT>(std::forward<Container>(tuple)),CURRENT, std::forward<Args>(args)...);

        if(CURRENT <= end)
            lineWalk(tuple,begin,end,fun
                ,std::integral_constant<std::size_t, CURRENT + 1>(),std::forward<Args>(args)...);
    }
};
}//under

namespace StaticWalker
{
template<typename Container, typename Functor, typename ...Args>
typename std::enable_if<std::is_same<typename std::result_of<Functor(/*1st type is dummy*/char,Args...)>::type, void>::value, void>::type
runtime_get(Container &&tuple, std::size_t idx, Functor &&fun, Args &&...args)
{
    static constexpr std::size_t tupleSize=std::tuple_size<typename std::decay<Container>::type>::value;
    assert(tupleSize && idx < tupleSize);
    StaticWalkerImplNs_::get_impl<0,tupleSize>::logWalk(tuple, idx, fun, std::forward<Args>(args)...);
}

template<typename Container, typename Functor, typename ...Args>
typename std::enable_if<!std::is_same<typename std::result_of<Functor(/*1st type is dummy*/char,Args...)>::type, void>::value, typename std::result_of<Functor(/*1st type is dummy*/char,Args...)>::type>::type
runtime_get(Container &&tuple, std::size_t idx, Functor &&fun, Args &&...args)
{
    static constexpr std::size_t tupleSize=std::tuple_size<typename std::decay<Container>::type>::value;
    assert(tupleSize && idx < tupleSize);
    return StaticWalkerImplNs_::get_impl<0,tupleSize>::logWalk(tuple, idx, fun, std::forward<Args>(args)...);
}

template<typename Container,typename Functor, typename ...Args>
void runtime_range(Container &&tuple, std::size_t begin, std::size_t end, Functor &&fun, Args &&...args)
{
    static constexpr std::size_t tupleSize=std::tuple_size<typename std::decay<Container>::type>::value;
    assert(tupleSize && begin < tupleSize && end < tupleSize);
    StaticWalkerImplNs_::range_impl<0u,tupleSize>::lineWalk(tuple, begin, end, fun,std::integral_constant<std::size_t, 0>(), std::forward<Args>(args)...);
}
}//StaticWalker

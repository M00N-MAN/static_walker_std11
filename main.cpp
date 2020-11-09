#include <tuple>
#include <cassert>
#include <iostream>
#include <type_traits>

namespace under
{
template<std::size_t LEFT, std::size_t RIGHT>
struct get_impl
{
    template<typename Tuple, typename Functor, typename ...Args>
    static void logWalk(Tuple const &tuple, std::size_t index, Functor &fun, Args &&...args)
    {
        static constexpr std::size_t MIDDLE = (RIGHT - LEFT) / 2 + LEFT;
        if (index > MIDDLE) get_impl<MIDDLE, RIGHT>::logWalk(tuple, index, fun, std::forward<Args>(args)...);
        else if (index < MIDDLE) get_impl<LEFT, MIDDLE>::logWalk(tuple, index, fun, std::forward<Args>(args)...);
        else fun(std::get<MIDDLE>(tuple), std::forward<Args>(args)...);
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

template <typename DST>
class Streamer
{
    DST &dst;

public:
    explicit Streamer(const DST &d):dst(d){}
    template <class T>
    void/*T*/ operator()(const T &v) const
    {
        dst << v << "\n";
        //return v;
    }

    template <class T,class V>
    void/*T*/ operator()(const T &t,const V &v) const
    {
        dst << t << " " << v << "\n";
        //return v;
    }
}; 

template <typename T>
class has_size
{
    typedef char YES;
    typedef YES NO[2];
    template<typename C> static auto Test(void*) -> decltype(size_t{std::declval<C const>().size()}, YES{});
    template<typename> static NO& Test(...);

public:
    static constexpr bool value = (sizeof(Test<T>(0)) == sizeof(YES));
};


struct GetSize
{
    template<typename TypeWithSize>
    constexpr typename std::enable_if<has_size<TypeWithSize>::value, size_t>::type operator()(const TypeWithSize &c) const
    {
        return c.size();
    }
    template<typename TypeWithoutSize> 
    constexpr typename std::enable_if<!has_size<TypeWithoutSize>::value, size_t>::type operator()(const TypeWithoutSize &) const
    {//let's assume it is at least a tuple
        return std::tuple_size<TypeWithoutSize>::value;
    }
};

template<typename Container>
void get_test(const Container &c)
{
    for (int i=GetSize()(c)-1; i>=0; --i)
		runtime_get(c, i, Streamer<std::ostream &>(std::cout));
}
template<typename Container>
void get_test_v(const Container &c)
{
    for(size_t i=0;i!=GetSize()(c);++i)
		runtime_get(c, i, Streamer<std::ostream &>(std::cout),1);
}

template<typename Container>
void range_test(const Container &c)
{//issue: range is only ascending by impl. need something like iterable ranges
	runtime_range(c, 0+1,GetSize()(c)-1, Streamer<std::ostream &>(std::cout));
}
template<typename Container>
void range_test_v(const Container &c)
{//issue: range is only ascending by impl. need something like iterable ranges
    runtime_range(c, 0+1,GetSize()(c)-1, Streamer<std::ostream &>(std::cout),1);
}

int main()
{
    get_test(std::make_tuple(1,'a', 42, 13.37));
    std::cout << std::endl;
    get_test(std::array<int, 5U>{ 6, 7, 8, 9, 10 });
    std::cout << std::endl;
    get_test_v(std::make_tuple(1,'a', 42, 13.37));
    std::cout << std::endl;
    get_test_v(std::array<int, 5U>{ 6, 7, 8, 9, 10 });
    std::cout << std::endl;
    range_test(std::make_tuple(1,'a', 42, 13.37));
    std::cout << std::endl;
    range_test(std::array<int, 5U>{ 6, 7, 8, 9, 10 });
    std::cout << std::endl;
    range_test_v(std::make_tuple(1,'a', 42, 13.37));
    std::cout << std::endl;
    range_test_v(std::array<int, 5U>{ 6, 7, 8, 9, 10 });

    return 0;
}
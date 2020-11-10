#pragma once
#include <type_traits>
#include <tuple>

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

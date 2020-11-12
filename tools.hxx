#pragma once
#include <type_traits>
#include <tuple>
#include <utility>

#ifdef DEBUG
#include <typeinfo>
#include <cxxabi.h>
#include <cstdlib>
#include <memory>
#include <string>

namespace rtti
{
template <typename Type>
std::string TypeName()
{
    auto demangle=[](char const* mangled) -> std::string {
        auto ptr = std::unique_ptr<char, decltype(& std::free)>{
            abi::__cxa_demangle(mangled, nullptr, nullptr, nullptr),
            std::free
        };
        return {ptr.get()};
    };
    return demangle(typeid(Type).name());
}
}//rtti
#endif //DEBUG

namespace sfinae
{

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

}//sfinae

struct GetSize
{
    template<typename TypeWithSize>
    constexpr typename std::enable_if<sfinae::has_size<TypeWithSize>::value, size_t>::type operator()(TypeWithSize &&c) const
    {
        return c.size();
    }
    template<typename TypeWithoutSize> 
    constexpr typename std::enable_if<!sfinae::has_size<TypeWithoutSize>::value, size_t>::type operator()(TypeWithoutSize &&) const
    {//let's assume it is at least a tuple
        return std::tuple_size<typename std::decay<TypeWithoutSize>::type>::value;
    }
};//GetSize

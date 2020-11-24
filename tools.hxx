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
#include <stdio.h>

namespace rtti
{
template <typename Type>
std::string TypeName()
{
    static auto demangle=[](char const* mangled) -> std::string {
        auto ptr = std::unique_ptr<char, decltype(& std::free)>{
            abi::__cxa_demangle(mangled, nullptr, nullptr, nullptr),
            std::free
        };
        return {ptr.get()};
    };
    return demangle(typeid(Type).name());
}
}//rtti
#define PRINT_TYPE(TYPE) printf("%d: %s\n",__LINE__,rtti::TypeName<TYPE>().c_str())
#else
#define PRINT_TYPE(TYPE)
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

template<typename, typename T>
struct has_StreamOut
{
    // specialization that doesn't make checking
    static_assert(std::integral_constant<T, false>::value,
        "Second template parameter must be a function type.");
};

template<typename C, typename Ret, typename... Args>
class has_StreamOut<C, Ret(Args...)>
{
    template<typename T>
    static constexpr std::true_type check(T, typename
        std::enable_if<
            std::is_same<
                decltype(std::declval<T>()->StreamOut(std::declval<Args>()...)),Ret
            >::value>::type *def = nullptr);

    template<typename>
    static constexpr std::false_type check(...);
    typedef decltype(check<typename std::remove_reference<C>::type *>(0)) type;
    has_StreamOut() = delete;
    has_StreamOut(const has_StreamOut &) = delete;
    has_StreamOut(has_StreamOut &&) = delete;
    has_StreamOut &operator=(const has_StreamOut &) = delete;
    has_StreamOut &operator=(has_StreamOut &&) = delete;

public:
    static constexpr bool value = type::value;
};//has_StreamOut

template<typename C>
struct isConvertibleToString {
private:
    template<typename T>
    static constexpr std::true_type check(T, typename
        std::enable_if<
            std::is_same<
                decltype(std::declval<std::to_string(*std::declval<T *>())>),std::string
            >::value>::type *def = nullptr);

    template<typename>
    static constexpr std::false_type check(...);
    typedef decltype(check<typename std::remove_reference<C>::type *>(0)) type;

public:
    static constexpr bool value = type::value;
};//isConvertibleToString
}//sfinae

class GetSize
{
    GetSize(const GetSize &) = delete;
    GetSize(GetSize &&) = delete;
    GetSize &operator=(const GetSize &) = delete;
    GetSize &operator=(GetSize &&) = delete;

public:
    GetSize() = default;
    template<typename TypeWithSize>
    constexpr typename std::enable_if<
        sfinae::has_size<TypeWithSize>::value, size_t>::type operator()(TypeWithSize &&c) const
    {
        return c.size();
    }
    template<typename TypeWithoutSize> 
    constexpr typename std::enable_if<!
        (std::is_arithmetic<typename std::remove_reference<TypeWithoutSize>::type>::value
        ||sfinae::has_size<TypeWithoutSize>::value), size_t>::type operator()(TypeWithoutSize &&) const
    {//let's assume it is at least a tuple
        return std::tuple_size<typename std::decay<TypeWithoutSize>::type>::value;
    }
};//GetSize

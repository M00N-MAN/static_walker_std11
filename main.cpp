#include <iostream>
#include "tools.hxx"
#include "StaticWalker.hxx"

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
    std::cout << '\n';
    get_test(std::array<int, 5U>{ 6, 7, 8, 9, 10 });
    std::cout << '\n';
    get_test_v(std::make_tuple(1,'a', 42, 13.37));
    std::cout << '\n';
    get_test_v(std::array<int, 5U>{ 6, 7, 8, 9, 10 });
    std::cout << '\n';
    range_test(std::make_tuple(1,'a', 42, 13.37));
    std::cout << '\n';
    range_test(std::array<int, 5U>{ 6, 7, 8, 9, 10 });
    std::cout << '\n';
    range_test_v(std::make_tuple(1,'a', 42, 13.37));
    std::cout << '\n';
    range_test_v(std::array<int, 5U>{ 6, 7, 8, 9, 10 });

    return 0;
}

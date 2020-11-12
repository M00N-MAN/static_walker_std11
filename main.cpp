#include <iostream>
#include <strstream>

#include "tools.hxx"
#include "StaticWalker.hxx"

// demo code
template <typename DST,char del='\n'>
class StreamerVOID
{
    DST &dst;
    
    StreamerVOID(const StreamerVOID &r) = delete;
    //    :dst(r.dst)
    //{
    //    std::cout<<__LINE__<<std::endl;
    //    assert(false);
    //}
    
    StreamerVOID(StreamerVOID &&r) = delete;
    //{
    //    dst=std::move(r);
    //    std::cout<<__LINE__<<std::endl;
    //    assert(false);
    //}
public:
    explicit StreamerVOID(DST &d)
        :dst(d)
    {}
    
    template <class T>
    void operator()(const T &t) const
    {
        dst << t << del;
    }
    
    template <class T1,class V1>
    void operator()(const T1 &t,const V1 &v) const
    {
        dst << v << ": " << t << del;
    }
};//StreamerVOID

template <typename DST,char del='\n'>
class Streamer
{
    DST &dst;
    
    Streamer(const Streamer &r) = delete;
    //    :dst(r.dst)
    //{
    //    std::cout<<__LINE__<<std::endl;
    //    assert(false);
    //}
    
    Streamer(Streamer &&r) = delete;
    //{
    //    dst=std::move(r);
    //    std::cout<<__LINE__<<std::endl;
    //    assert(false);
    //}

public:
    explicit Streamer(DST &d):dst(d){}
    
    template <class T>
    char operator()(const T &t) const
    {
        dst << t;
        return del;//some dummy result
    }
    
    //template <typpename T,typename... Args>
    //size_t operator()(T &&t,Args &&...args) const
    template <class T,class V>
    DST &operator()(const T &t,const V &v) const
    {
        //std::cout << "line: "<< __LINE__ << std::endl;
        dst << v << ": " << t;
        //return sizeof(T);//some dummy result
        return dst;
    }

    template <class T,class N,class V>
    char operator()(const T &t,const N &n,const V &v) const
    {
        //std::cout << "line: "<< __LINE__ << std::endl;
        dst << n << ": " << t << " " << v;
        //return sizeof(T);//some dummy result
        return del;
    }
};//Streamer

template<typename Container, typename... Args>
void get_test(Container &&c,Args &&...args)
{
    for (int i=GetSize()(c)-1; i>=0; --i)
        runtime_get(c, i, StreamerVOID<std::ostream>(std::cout),i+1,args...);
}

template<typename Container, typename... Args>
void get_test_v(Container &&c,Args &&...args)
{
    Streamer<std::ostream> sS(std::cout);
    for(size_t i=0;i!=GetSize()(c);++i)
        std::cout<<"r:"<<runtime_get(c, i, sS,i+1,args...);
}

template<typename Container, typename... Args>
void range_test(Container &&c,Args &&...args)
{//issue: range is only ascending by impl. need something like iterable ranges

    Streamer<std::ostream> s(std::cout);
    runtime_range(c, 0+1,GetSize()(c)-1, s,args...);
}
template<typename Container, typename... Args>
void range_test_v(Container &&c,Args &&...args)
{//issue: range is only ascending by impl. need something like iterable ranges
    runtime_range(c, 0+1,GetSize()(c)-1, Streamer<std::ostream>(std::cout),args...);
}

int main()
{
    get_test(std::make_tuple((void *)0xFF, 'a', 3.14, -4, "abc"));
    std::cout << '\n';
    get_test(std::array<int, 5U>{ 6, 7, 8, 9, 10 });
    std::cout << '\n';
    get_test_v(std::make_tuple("xyz", 3.14, 8, 'x'),__LINE__);
    std::cout << '\n';
    get_test_v(std::array<int, 5U>{ 1, 2, 3, 4, 5 },__LINE__);
    std::cout << '\n';
    range_test(std::make_tuple((void *)0xFF, 'a', 3.14, -4, "abc"));
    std::cout << '\n';
    range_test(std::array<int, 5U>{ 6, 7, 8, 9, 10 });
    std::cout << '\n';
    range_test_v(std::make_tuple("xyz", 3.14, 8, 'x'));
    std::cout << '\n';
    range_test_v(std::array<int, 5U>{ 1, 2, 3, 4, 5 });

    return 0;
}

#include "tools.hxx"
#include "StaticWalker.hxx"
#include "StreamSerialize.hxx"

template<typename Container, typename... Args>
void test_runtime_get_no_return(Container &&c,Args &&...args)
{
    StreamOutFunctor_void<std::ostream,','> s(std::cout);
    for (int i=GetSize()(c)-1; i>=1; --i)
        StaticWalker::runtime_get(c, i,s,i+1,args...);
    StaticWalker::runtime_get(c, 0,StreamOutFunctor_void<std::ostream>(std::cout),1,args...);
}

template<typename Container, typename... Args>
void test_runtime_get_return(Container &&c,Args &&...args)
{
    StreamOutFunctor_non_void<std::ostream,','> sS(std::cout);
    for(size_t i=0;i!=GetSize()(c)-1;++i)
        std::cout<<"r:"<<StaticWalker::runtime_get(c, i, sS,i+1,args...);
    std::cout<<"r:"<<StaticWalker::runtime_get(c, GetSize()(c)-1, StreamOutFunctor_non_void<std::ostream>(std::cout),GetSize()(c),args...);
}

template<typename Container, typename... Args>
void test_runtime_range(Container &&c,Args &&...args)
{//issue: range is only ascending by streamingImplNs_. need something like iterable diapason
    StaticWalker::runtime_range(c, 0,GetSize()(c)-2, StreamOutFunctor_void<std::ostream,','>(std::cout),args...);
    StaticWalker::runtime_get(c, GetSize()(c)-1, StreamOutFunctor_void<std::ostream>(std::cout),GetSize()(c),args...);
}

void runWalkerTests()
{
    test_runtime_get_no_return(std::make_tuple((void *)0xFF, 'a', 3.14, -4, "abc"),' ');
    test_runtime_get_no_return(std::array<int, 5U>{ 6, 7, 8, 9, 10 },std::make_tuple('-',0));
    test_runtime_get_no_return(std::make_tuple((void *)0xFF, 3.14,std::array<char, 3>{ 'x','y','z' }, 8, 'x'),' ');

    test_runtime_get_return(std::make_tuple("xyz", 3.14, 8, 'x'),"line",__LINE__);
    test_runtime_get_return(std::array<int, 5U>{ 1, 2, 3, 4, 5 },"line",__LINE__);

    test_runtime_range(std::make_tuple(3.14, '3', "DA",11,(void *)0xFF));
    test_runtime_range(std::array<int, 5U>{ 6, 7, 8, 9, 10 });
}
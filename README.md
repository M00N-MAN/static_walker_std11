# Static Walker (C++11)

Project: [static_walker_std11][Open online]

slogan: simple runtime enumeration of std::tuple and std::array via optimized recursion using pure C++11

Description:
this static_walker allows almost the same feature as std::integer_sequence does in C++14, but:
 - based on pattern visitor with the compile time recursion (access time O(log2(n))
 - it walks over the tuple and array, but with C++11

Capabilities:
* custom & range based
* std::tuple and std::array
* log2n and linear
* lambdas & void/non-void functors
* passing variadic parameters
* std::ostream
* NVI for childs
* pure C++11  

Examples:

sending tuple/array to a stream:  
```cpp
#include "StreamSerialize.hxx" //includes StaticWalker.hxx
//...
std::cout<<std::make_tuple(.1923, 42, std::make_tuple((void *)0xB00B1E5,'|', 3.14, -4, std::array<char, 3>{'a','b','c'}), "cba")<<'\n';
```  

[Open online] or clone & run demo with:

```bash
make clean && make -j && bin/walker.exe
```  
or just
```bash
rm -vf *.o walker.exe && g++ *.cpp -Wall -Wextra -std=c++11 -pedantic -o walker.exe && ./walker.exe
```

walking via tuple/array with custom order:  
```cpp
#include "StreamSerialize.hxx" //includes StaticWalker.hxx

template<typename Container, typename... Args>
void test_runtime_get_return(Container &&c,Args &&...args)
{
    Streamer<std::ostream,','> sS(std::cout);                              //you can define your own callback
    for(size_t i=0;i!=GetSize()(c)-1;++i)                                  //define any order you want
        std::cout<<"r:"<<StaticWalker::runtime_get(c, i, sS,i+1,args...);  //<--KEY POINT OF EXAMPLE: invoke the callback
    std::cout<<"r:"<<StaticWalker::runtime_get(c, GetSize()(c)-1, Streamer<std::ostream>(std::cout),GetSize()(c),args...);
}
//...
test_runtime_get_no_return(std::make_tuple((void *)0xFF, 3.14,std::array<char, 3>{ 'x','y','z' }, 8, 'x'),' ');
```

btw, if you need just to serialize data in usual direct order then it is better to use StaticWalker::runtime_range(..) instead of calling StaticWalker::runtime_get(..) in a loop becouse it will reduce the stack:
```cpp
#include "StreamSerialize.hxx"
...
template<typename Container, typename... Args>
void test_runtime_range(Container &&c,Args &&...args)
{//issue: range is only ascending by streamingImplNs_. need something like iterable diapason
    StaticWalker::runtime_range(c, 0,GetSize()(c)-2, StreamerVOID<std::ostream,','>(std::cout),args...);
    StaticWalker::runtime_get(c, GetSize()(c)-1, StreamerVOID<std::ostream>(std::cout),GetSize()(c),args...);
}
...
test_runtime_range(std::make_tuple(3.14, '3', "DA",11,(void *)0xFF));
```

for more details, please, refer to a code of RunDemo() function at main.cpp...

Good luck and never stop:)

[Open online]: https://wandbox.org/permlink/QDKQv2EK8Y1OiIxm

#runtime #variadic #template #recoursive #enumerate #std::tuple #std::array #C++11

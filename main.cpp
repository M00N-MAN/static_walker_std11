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
    StreamerVOID(StreamerVOID &&r) = delete;

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

    template <class T,class N, typename... Args>
    void operator()(T && t,const N &n, Args&&... args) const
    {
        dst << n << ": " << std::forward<T>(t);
        using expander = int[];
        (void)expander{0, (void(dst << std::forward<Args>(args)<<del), 0)...};
    }
};//StreamerVOID

template <typename DST,char del='\n'>
class Streamer
{
    DST &dst;
    
    Streamer(const Streamer &r) = delete;
    Streamer(Streamer &&r) = delete;

public:
    explicit Streamer(DST &d):dst(d){}
    
    template <class T>
    char operator()(T && t) const
    {
        dst << std::forward<T>(t);
        return del;//some dummy result
    }
    
    template <class T,class V>
    DST &operator()(T && t,const V &v) const
    {
        dst << v << ": " << std::forward<T>(t);
        return dst;
    }

    template <class T,class N, typename... Args>
    char operator()(T && t,const N &n, Args&&... args) const
    {
        dst << n << ": " << std::forward<T>(t);
        using expander = int[];
        (void)expander{0, (void(dst <<" + "<< std::forward<Args>(args)), 0)...};
        return del;
    }
};//Streamer

template<typename Container, typename... Args>
void get_test(Container &&c,Args &&...args)
{
    StreamerVOID<std::ostream,','> s(std::cout);
    for (int i=GetSize()(c)-1; i>=1; --i)
        runtime_get(c, i,s,i+1,args...);
    runtime_get(c, 0,StreamerVOID<std::ostream>(std::cout),1,args...);
}

template<typename Container, typename... Args>
void get_test_v(Container &&c,Args &&...args)
{
    Streamer<std::ostream,','> sS(std::cout);
    for(size_t i=0;i!=GetSize()(c)-1;++i)
        std::cout<<"r:"<<runtime_get(c, i, sS,i+1,args...);
    std::cout<<"r:"<<runtime_get(c, GetSize()(c)-1, Streamer<std::ostream>(std::cout),GetSize()(c),args...);
}

template<typename Container, typename... Args>
void range_test(Container &&c,Args &&...args)
{//issue: range is only ascending by impl. need something like iterable ranges

    Streamer<std::ostream> s(std::cout);
    runtime_range(c, 0,GetSize()(c)-1, s,args...);
}
template<typename Container, typename... Args>
void range_test_v(Container &&c,Args &&...args)
{//issue: range is only ascending by impl. need something like iterable ranges
    runtime_range(c, 0+1,GetSize()(c)-1, Streamer<std::ostream>(std::cout),args...);
}

namespace impl
{
template <class Char, class Traits, class T,
    typename std::enable_if<
        (std::is_pointer<typename std::remove_reference<T>::type>::value ||
        std::is_arithmetic<typename std::remove_reference<T>::type>::value)
    , bool>::type * = nullptr> inline
void streamOut(std::basic_ostream<Char, Traits>& out, T && v, int)
{
    out.operator<<(v); //do not use out<<v to avoid recursion
}

template <class Char, class Traits, class T,
    typename std::enable_if<!
        (std::is_pointer<typename std::remove_reference<T>::type>::value ||
        std::is_arithmetic<typename std::remove_reference<T>::type>::value), void>::type * = nullptr> inline
void streamOut(std::basic_ostream<Char, Traits>& out, T && v,long)
{
    out << '{';
    StreamerVOID<std::basic_ostream<Char, Traits>,','> s(out);
    for(size_t i=0;i!=GetSize()(v)-1;++i)
    {
        runtime_get(v, i, s);
    }
    runtime_get(v,GetSize()(v)-1, StreamerVOID<std::basic_ostream<Char, Traits>,'}'> (out));
    //runtime_range(v, 0,GetSize()(c)-1, StreamerVOID<std::basic_ostream<Char, Traits>,','>(out));
    //out << '}';
}

template <class Char, class Traits, class T>
void streamOut(std::basic_ostream<Char, Traits>& out, T && value)
{
    streamOut(out,value, 0);
}

}//impl


template <class Char, class Traits, class T>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& out, T && value)
{
    impl::streamOut(out, value);
    return out;
}

template <typename ContentE,typename... ArgsT>
class Content
{
    std::tuple<ArgsT...> members_;

public:
    explicit Content(const ArgsT&... t)
        :members_(t...)
    {
    }

    template<typename T,ContentE eContent>
    void Set(T && v)
    {
        std::get<eContent>(members_)=v;
    }

    template<typename T,ContentE eContent>
    const T &Get() const
    {
        return std::get<eContent>(members_);
    }

    void streamOut(std::basic_ostream<char>& out)
    {
        out<<members_;
    }
};//Content

enum MyClassMembers_e
{
     eMyClassMember_Age
    ,eMyClassMember_Name
    ,eMyClassMember_Special
};

typedef std::tuple<std::string,time_t,int> CourseDetails_t;

class Student
    :public Content<MyClassMembers_e
                   ,int             /*age*/
                   ,std::string     /*name*/
                   ,CourseDetails_t /*courses<title,time,building>*/>
{
    Student() = delete;

public:
	Student(int age,const std::string &name,const CourseDetails_t &special)
        :Content(age,name,special)
    {
    }
	void Show()
    {
        std::cout<<"Hi, my name is "<< Get<std::string,eMyClassMember_Name>()<<'\n';
        int age=Get<int,eMyClassMember_Age>();
        std::cout<<"i'm "<< age <<"y.o."<<'\n';
        Set<int,eMyClassMember_Age>(age+1);
        std::cout<<"oops just now i'm "<< Get<int,eMyClassMember_Age>()<<"y.o. because HappyBirthDay"<<'\n';
        std::cout<<"and this is my course for today: "<<Get<CourseDetails_t,eMyClassMember_Special>()<<'\n';
        std::cout<<"Bye"<<'\n';
        //;
    }

};//Student

int main()
{
    get_test(std::make_tuple((void *)0xFF, 'a', 3.14, -4, "abc"),' ');
    get_test(std::array<int, 5U>{ 6, 7, 8, 9, 10 },std::make_tuple('-',0));
    get_test(std::make_tuple((void *)0xFF, 3.14,"xyz", 8, 'x'),' ');
    get_test_v(std::make_tuple("xyz", 3.14, 8, 'x'),"line",__LINE__);
    get_test_v(std::array<int, 5U>{ 1, 2, 3, 4, 5 },"line",__LINE__);
    
    //temporary doesn't work
    //range_test(std::make_tuple((void *)0xFF, 'a', 3.14, -4, "abc"));
    //std::cout << '\n';
    //range_test(std::array<int, 5U>{ 6, 7, 8, 9, 10 });
    //std::cout << '\n';
    //range_test_v(std::make_tuple("xyz", 3.14, 8, 'x'));
    //std::cout << '\n';
    //range_test_v(std::array<int, 5U>{ 1, 2, 3, 4, 5 });

    std::cout<<std::make_tuple(41.3, 4, std::make_tuple((void *)0xB00B1E5,'|', 3.14, -4, "abc"), "cba")<<'\n';
    std::cout<<std::array<int, 5U>{ 1, 2, 3, 4, 5 }<<'\n';

    Student a(19,"Albert",std::make_tuple("physics",time_t(0),42));
    a.Show();
    a.Set<CourseDetails_t,eMyClassMember_Special>(std::make_tuple("chemistry",time_t(19),23));
    a.Show();

    //std::cout<<A<<'\n'; //temporary doesn't work
    return 0;
}

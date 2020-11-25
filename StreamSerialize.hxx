#pragma once

#include <iostream>
#include <sstream>
#include "StaticWalker.hxx"
#include "tools.hxx"

//#define PRINT_TYPE_serialize(T) PRINT_TYPE(T)
#define PRINT_TYPE_serialize(T)

//#define PRINT_TYPE_adapter(T) PRINT_TYPE(T)
#define PRINT_TYPE_adapter(T)
//========================================================================

template <typename DST,char del='\n'>
class StreamOutFunctor_void
{
    DST &dst;
    
    StreamOutFunctor_void(const StreamOutFunctor_void &r) = delete;
    StreamOutFunctor_void(StreamOutFunctor_void &&r) = delete;

public:
    explicit StreamOutFunctor_void(DST &d)
        :dst(d)
    {}
    
    template <class T>
    void operator()(const T &t) const
    {
        dst << t << del;
    }

    template <class T,class V>
    void operator()(T && t,const V &v) const
    {
        dst << v << ": " << std::forward<T>(t)<<del;
    }

    template <class T,class N, typename... Args>
    void operator()(T && t,const N &n, Args&&... args) const
    {
        dst << n << ": " << std::forward<T>(t);
        using expander = int[];
        (void)expander{0, (void(dst << std::forward<Args>(args)<<del), 0)...};
    }
};//StreamOutFunctor_void

template <typename SRC,char del='\n'>
class StreamInFunctor_void
{//tbd: https://stackoverflow.com/questions/4981522/reading-a-fixed-number-of-chars-with-on-an-istream
    SRC &src;
    
    StreamInFunctor_void(const StreamInFunctor_void &r) = delete;
    StreamInFunctor_void(StreamInFunctor_void &&r) = delete;

public:
    explicit StreamInFunctor_void(SRC &s)
        :src(s)
    {}
    
    template <class T>
    void operator()(T & t)
    {
        (void)src;
        (void)t;
    }

};//StreamInFunctor_void
//========================================================================

template <typename DST,char del='\n'>
class StreamOutFunctor_non_void
{
    DST &dst;
    
    StreamOutFunctor_non_void(const StreamOutFunctor_non_void &r) = delete;
    StreamOutFunctor_non_void(StreamOutFunctor_non_void &&r) = delete;

public:
    explicit StreamOutFunctor_non_void(DST &d):dst(d){}
    
    template <class T>
    char operator()(T && t) const
    {
        dst << std::forward<T>(t);
        return del;//some dummy result, define your own in same way
    }
    
    template <class T,class V>
    DST &operator()(T && t,const V &v) const
    {
        dst << v << ": " << std::forward<T>(t)<<del;
        return dst;//some dummy result, define your own in same way
    }

    template <class T,class N, typename... Args>
    char operator()(T && t,const N &n, Args&&... args) const
    {
        dst << n << ": " << std::forward<T>(t);
        using expander = int[];
        (void)expander{0, (void(dst <<" + "<< std::forward<Args>(args)), 0)...};
        return del;//some dummy result, define your own in same way
    }
};//StreamOutFunctor_non_void

template <typename SRC,char del='\n'>
class StreamInFunctor_non_void
{
    SRC &src;
    
    StreamInFunctor_non_void(const StreamInFunctor_non_void &r) = delete;
    StreamInFunctor_non_void(StreamInFunctor_non_void &&r) = delete;

public:
    explicit StreamInFunctor_non_void(SRC &s):src(s){}
    
    template <class T>
    char operator()(T & t) const
    {
        (void)src;
        (void)t;
        return del;//some dummy result, define your own in same way
    }
};//StreamInFunctor_non_void

namespace streamingImplNs_
{
template <class Char, class Traits, class T,
    typename std::enable_if<
        (std::is_same<typename std::remove_reference<T>::type,std::string>::value)
    , char>::type * = nullptr> inline
void streamOut_(std::basic_ostream<Char, Traits>& dst, T && v, char)
{
    PRINT_TYPE_serialize(T);
    operator<<(dst,v);
}
template <class Char, class Traits, class T,
    typename std::enable_if<
        (std::is_same<typename std::remove_reference<T>::type,std::string>::value)
    , char>::type * = nullptr> inline
void streamIn_(std::basic_istream<Char, Traits>& src, T & v, char)
{
    PRINT_TYPE_serialize(T);
    (void)src;
    (void)v;
}
//========================================================================

template <class Char, class Traits, class T,
    typename std::enable_if<
        (std::is_pointer<typename std::remove_reference<T>::type>::value ||
        std::is_arithmetic<typename std::remove_reference<T>::type>::value
        /*sfinae::isConvertibleToString<T>::value*/)
    , int>::type * = nullptr> inline
void streamOut_(std::basic_ostream<Char, Traits>& dst, T && v, int)
{
    PRINT_TYPE_serialize(T);
    dst.operator<<(std::forward<T>(v)); //do not use out<<v to avoid recursion
}
template <class Char, class Traits, class T,
    typename std::enable_if<
        (std::is_pointer<typename std::remove_reference<T>::type>::value ||
        std::is_arithmetic<typename std::remove_reference<T>::type>::value
        /*sfinae::isConvertibleToString<T>::value*/)
    , int>::type * = nullptr> inline
void streamIn_(std::basic_istream<Char, Traits>& src, T & v, int)
{
    PRINT_TYPE_serialize(T);
    (void)src;
    (void)v;
}
//========================================================================

template <class Char, class Traits, class T,
    typename std::enable_if<
        !( std::is_pointer<typename std::remove_reference<T>::type>::value
        || std::is_arithmetic<typename std::remove_reference<T>::type>::value)
        &&!std::is_same<typename std::remove_reference<T>::type,std::string>::value, void>::type * = nullptr> inline
void streamOut_(std::basic_ostream<Char, Traits>& dst, T && v,long)
{
    dst << '{';
    StreamOutFunctor_void<std::basic_ostream<Char, Traits>,','> s(dst);
    for(size_t i=0;i!=GetSize()(v)-1;++i)
    {
        StaticWalker::runtime_get(v, i, s);
    }
    StaticWalker::runtime_get(v,GetSize()(v)-1, StreamOutFunctor_void<std::basic_ostream<Char, Traits>,'}'> (dst));
}
template <class Char, class Traits, class T,
    typename std::enable_if<
        !( std::is_pointer<typename std::remove_reference<T>::type>::value
        || std::is_arithmetic<typename std::remove_reference<T>::type>::value)
        &&!std::is_same<typename std::remove_reference<T>::type,std::string>::value, void>::type * = nullptr> inline
void streamIn_(std::basic_istream<Char, Traits>& src, T && v,long)
{
    (void)src;
    (void)v;
}

//========================================================================
template <class Char, class Traits, class T> inline
typename std::enable_if<
    !sfinae::has_StreamOut<T,void(std::basic_ostream<Char, Traits>&)
>::value, void>::type
streamOut(std::basic_ostream<Char, Traits>& dst, T && value,int)
{
    PRINT_TYPE_serialize(T);
    streamOut_(dst,value, 0);
}
template <class Char, class Traits, class T> inline
typename std::enable_if<
    !sfinae::has_StreamIn<T,void(std::basic_istream<Char, Traits>&)
>::value, void>::type
streamIn(std::basic_istream<Char, Traits>& src, T & value,int)
{
    PRINT_TYPE_serialize(T);
    streamIn_(src,value, 0);
}
//========================================================================
template <class Char, class Traits, class T> inline
typename std::enable_if<
    sfinae::has_StreamOut<T,void(std::basic_ostream<Char, Traits>&)
>::value, void>::type
streamOut(std::basic_ostream<Char, Traits>& dst, T && value,long)
{
    PRINT_TYPE_serialize(T);
    value.StreamOut(dst);
}
template <class Char, class Traits, class T> inline
typename std::enable_if<
    sfinae::has_StreamIn<T,void(std::basic_istream<Char, Traits>&)
>::value, void>::type
streamIn(std::basic_istream<Char, Traits>& src, T && value,long)
{
    PRINT_TYPE_serialize(T);
    value.StreamIn(src);
}
}//streamingImplNs_

//========================================================================
template <typename Char, typename Traits, typename T> inline
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& dst, T && value)
{
    PRINT_TYPE_serialize(T);
    streamingImplNs_::streamOut(dst, value,0);
    return dst;
}
template <typename Char, typename Traits, typename T> inline
std::basic_istream<Char, Traits>&
operator>>(std::basic_istream<Char, Traits>& src, T & value)
{
    PRINT_TYPE_serialize(T);
    streamingImplNs_::streamIn(src, value,0);
    return src;
}
//========================================================================
template <typename Char=char, typename Traits=std::char_traits<char>>
class StreamableNVI
{

protected:
    typedef std::basic_ostream<Char,Traits> OStream_t;
    typedef std::basic_istream<Char,Traits> IStream_t;

    virtual void doStreamOut(OStream_t &) = 0;
    virtual void doStreamIn (IStream_t &) = 0;

public:
    virtual ~StreamableNVI() = default;
    void StreamOut(OStream_t& dst){doStreamOut(dst);}
    void StreamIn (IStream_t& src){doStreamIn (src);}
};//StreamableNVI

template <typename ContentE,typename... ArgsT>
class StreamableMembers
    :public StreamableNVI<char,std::char_traits<char>>
{
    std::tuple<ArgsT...> members_;

protected:    
    void doStreamOut(StreamableNVI::OStream_t& dst) override
    {
        PRINT_TYPE_adapter(StreamableMembers());
        dst<<members_;
    }

    void doStreamIn (StreamableNVI::IStream_t& src) override
    {
        PRINT_TYPE_adapter(StreamableMembers());
        src>>members_;
    }

public:
    explicit StreamableMembers(const ArgsT&... t):members_(t...){}

    template<typename T,ContentE eContent> void     Set(T && v){       std::get<eContent>(members_)=v;}
    template<typename T,ContentE eContent> const T &Get() const{return std::get<eContent>(members_);  }
};//StreamableMembers

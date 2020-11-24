#pragma once

#include <iostream>
#include <sstream>
#include "StaticWalker.hxx"
#include "tools.hxx"

//#define PRINT_TYPE_serialize(T) PRINT_TYPE(T)
#define PRINT_TYPE_serialize(T)

//#define PRINT_TYPE_adapter(T) PRINT_TYPE(T)
#define PRINT_TYPE_adapter(T)

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
};//Streamer

namespace streamingImplNs_
{
template <class Char, class Traits, class T,
    typename std::enable_if<
        (std::is_same<typename std::remove_reference<T>::type,std::string>::value)
    , char>::type * = nullptr> inline
void streamOut_(std::basic_ostream<Char, Traits>& out, T && v, char)
{
    PRINT_TYPE_serialize(T);
    operator<<(out,v);
}

template <class Char, class Traits, class T,
    typename std::enable_if<
        (std::is_pointer<typename std::remove_reference<T>::type>::value ||
        std::is_arithmetic<typename std::remove_reference<T>::type>::value
        /*sfinae::isConvertibleToString<T>::value*/)
    , int>::type * = nullptr> inline
void streamOut_(std::basic_ostream<Char, Traits>& out, T && v, int)
{
    PRINT_TYPE_serialize(T);
    out.operator<<(v); //do not use out<<v to avoid recursion
}

template <class Char, class Traits, class T,
    typename std::enable_if<
        !( std::is_pointer<typename std::remove_reference<T>::type>::value
        || std::is_arithmetic<typename std::remove_reference<T>::type>::value)
        &&!std::is_same<typename std::remove_reference<T>::type,std::string>::value, void>::type * = nullptr> inline
void streamOut_(std::basic_ostream<Char, Traits>& out, T && v,long)
{
    out << '{';
    StreamerVOID<std::basic_ostream<Char, Traits>,','> s(out);
    for(size_t i=0;i!=GetSize()(v)-1;++i)
    {
        StaticWalker::runtime_get(v, i, s);
    }
    StaticWalker::runtime_get(v,GetSize()(v)-1, StreamerVOID<std::basic_ostream<Char, Traits>,'}'> (out));
}

template <class Char, class Traits, class T> inline
typename std::enable_if<
    !sfinae::has_StreamOut<T,void(std::basic_ostream<Char, Traits>&)
>::value, void>::type
streamOut(std::basic_ostream<Char, Traits>& out, T && value,int)
{
    PRINT_TYPE_serialize(T);
    streamOut_(out,value, 0);
}

template <class Char, class Traits, class T> inline
typename std::enable_if<
    sfinae::has_StreamOut<T,void(std::basic_ostream<Char, Traits>&)
>::value, void>::type
streamOut(std::basic_ostream<Char, Traits>& out, T && value,long)
{
    PRINT_TYPE_serialize(T);
    value.StreamOut(out);
}
}//streamingImplNs_

template <typename Char, typename Traits, typename T> inline
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& out, T && value)
{
    PRINT_TYPE_serialize(T);
    streamingImplNs_::streamOut(out, value,0);
    return out;
}

template <typename Char=char, typename Traits=std::char_traits<char>>
class StreamableNVI
{

protected:
    typedef std::basic_ostream<Char,Traits> OStream_t;
    virtual void doStreamOut(OStream_t& out) = 0;

public:
    virtual ~StreamableNVI() = default;
    void StreamOut(OStream_t& out){doStreamOut(out);}
};//StreamableNVI

template <typename ContentE,typename... ArgsT>
class StreamableMembers
    :public StreamableNVI<char,std::char_traits<char>>
{
    std::tuple<ArgsT...> members_;

protected:    
    void doStreamOut(StreamableNVI::OStream_t& out) override
    {
        PRINT_TYPE_adapter(StreamableMembers());
        out<<members_;
    }

public:
    explicit StreamableMembers(const ArgsT&... t):members_(t...){}

    template<typename T,ContentE eContent> void     Set(T && v){std::get<eContent>(members_)=v;}
    template<typename T,ContentE eContent> const T &Get() const{return std::get<eContent>(members_);}
};//StreamableMembers

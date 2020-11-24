#include <iostream>
#include "StreamSerialize.hxx"

// demo code

extern void runWalkerTests(); //StaticWalker_tests.cpp

void runStreamingTests()
{
    auto t = std::make_tuple(3.14, '3', "DA",11,(void *)0xFF);
    std::cout<<t<<'\n';

    std::cout<<std::make_tuple(.1923, 42, std::make_tuple((void *)0xB00B1E5,'|', 3.14, -4, std::array<char, 3>{'a','b','c'}), "cba")<<'\n';

    std::array<int, 5U> a{ 1, 2, 3, 4, 5};

    std::cout<<a<<" "<<std::array<unsigned, 7>{ 1, 2, 3, 5, 8, 13, 21 }<<'\n';
}

void runDemo()
{

enum MyClassMembers_e
{
     eMyClassMember_Age
    ,eMyClassMember_Name
    ,eMyClassMember_Special
};

typedef std::tuple<std::string,time_t,int> CourseDetails_t;

class Student
    :public StreamableMembers<MyClassMembers_e
        ,int             /*age*/
        ,std::string     /*name*/
        ,CourseDetails_t /*courses<title,time,building>*/
    >
{
    Student() = delete;

public:
	Student(int age,const std::string &name,CourseDetails_t &&special)
        :StreamableMembers(age,name,special)
    {
        std::cout<<Get<std::string,eMyClassMember_Name>()<<": Hi, my name is "<<'\n';
    }
    ~Student()
    {
        std::cout<<Get<std::string,eMyClassMember_Name>()<<": Bye..."<<'\n';
    }
	void Say()
    {
        int age=Get<int,eMyClassMember_Age>();
        std::cout<<"i'm "<< age <<"y.o."<<'\n';
        Set<int,eMyClassMember_Age>(age+1);
        std::cout<<"oops just now i'm "<< Get<int,eMyClassMember_Age>()<<"y.o. because HappyBirthDay"<<'\n';
        std::cout<<"and this is my course for today: "<<Get<CourseDetails_t,eMyClassMember_Special>()<<'\n';
    }

};//Student
    Student a(19,"RO",std::make_tuple("physics",time_t(0),42));
    a.Say();
    a.Set<CourseDetails_t,eMyClassMember_Special>(std::make_tuple("chemistry",time_t(19),23));
    a.Say();

    std::cout<<rtti::TypeName<Student>()<<": ["<<a<<"]"<<'\n';
}

int main()
{
    runWalkerTests();
    runStreamingTests();
    runDemo();

    return 0;
}

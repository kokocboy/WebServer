#include<iostream>
#include<fstream>
#include<string.h>
using namespace std;
#define FILE_INFO   "["<<__DATE__<<" "<<__TIME__<<" " << __FILE__ << '@' << __FUNCTION__ << '#' << __LINE__  <<"]"
#define logs(x)     std::cout << FILE_INFO << #x":" << x << std::endl
#include<initializer_list>
template<typename T>
class Log
{
private:
    void out(T val);
    void out(T first,Args... args);
public:
    void info(Args... args);
    void error(Args... args);
    Log();
    ~Log();
};
Log::Log()
{
    
}

Log::~Log()
{

}

template<typename T>
void Log::out(T val)
{
    cout<<val<<endl;
}
template<typename T,typename ... Args>
void Log::out(T first,Args... args)
{
    cout<<first;
	out(args...);
}
template<typename ... Args>
void Log::info(Args... args)
{
    cout<<"Info "<<FILE_INFO;
    out(args...);
}
template<typename ... Args>
void Log::error(Args... args)
{
    cout<<"Error "<<FILE_INFO;
    out(args...);
}
int main()
{
    // Log *log=new Log();
    // string str="heh";
    // //logs(str);
    // log->info("name=",str);
    // log->error("failed=",str);



    return 0;
}
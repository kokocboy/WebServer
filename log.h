#ifndef LOG_H
#define LOG_H
#include<iostream>
#include<fstream>
#include<string.h>
#include <stdarg.h> 
#include<initializer_list>
#define logs(x)     std::cout << FILE_INFO << #x":" << x << std::endl
#define pre         (char*)(logform(init1,init2).data())
#define init1       "[Time=%s File=%s Function=%s Line=%d]"
#define init2       (char*)(getCurrentTimeStr().data()),__FILE__,__FUNCTION__,__LINE__
using namespace std;
class Log
{
private:
    /* data */
public:
    Log(/* args */);
    ~Log();
};

Log::Log(/* args */)
{

}

Log::~Log()
{

}
string logform(char const *form,...)
{
    char buf[250];
    va_list valst;
    va_start(valst, form);
	int len=vsnprintf(buf , 250, form, valst);
    buf[len+1]=0;
    return buf;
}
string  getCurrentTimeStr()
{
	time_t t = time(NULL);
	char ch[64] = {0};
	strftime(ch, sizeof(ch) - 1, "%Y-%m-%d %H:%M:%S", localtime(&t));     //年-月-日 时-分-秒
	return (string)ch;
}
void info(char const *form,...)
{
    char buf[250];
    va_list valst;
    va_start(valst, form);
	int len=vsnprintf(buf , 250, form, valst);
	buf[len]='\n';
    buf[len+1]=0;
    cout<<buf<<endl;
}
void error(char const *form,...)
{
    char buf[250];
    va_list valst;
    va_start(valst, form);
	int len=vsnprintf(buf , 250, form, valst);
	buf[len]='\n';
    buf[len+1]=0;
    cout<<buf<<endl;
}
#endif
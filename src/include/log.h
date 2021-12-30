#ifndef LOG_H
#define LOG_H
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdarg.h>
#include "lock.h"
#include <pthread.h>
#include <initializer_list>
#include <queue>
#include <string>
#define logs(x) std::cout << FILE_INFO << #x ":" << x << std::endl
#define pre (char *)(logform(init1, init2).data())
#define init1 "[Time=%s File=%s Function=%s Line=%d]"
#define init2 (char *)(getCurrentTimeStr().data()), __FILE__, __FUNCTION__, __LINE__
#define rollLen 20 //必须大于所有线程数量，避免多个线程操作同一个索引
using namespace std;
class Log
{
private:
    /* data */
    pthread_t *pthread;
    static void *worker(void *arg);
    static queue<string> que[rollLen];
    static Sem sem;
    static int runTop;
    static int runIndex;
    static unsigned int rollIndex;
    void run();

public:
    Log(/* args */);
    ~Log();
    static void push(string str);
};
queue<string> Log::que[rollLen];
Sem Log::sem;
unsigned int Log::rollIndex = 0;
int Log::runTop = 1;
int Log::runIndex=0;
Log::Log(/* args */)
{
    pthread = new pthread_t();
    pthread_create(pthread, NULL, worker, this);
    pthread_detach(*pthread);
}

Log::~Log()
{
}
void Log::push(string str)
{
    //cout<<str<<endl;
   // return ;
    int index = __sync_fetch_and_add(&rollIndex, 1) % rollLen;
    que[index].push(str);
    if(runTop&&index==runIndex){
        sem.post();
    }
}
void *Log::worker(void *arg)
{
    Log *log = (Log *)arg;
    log->run();
    return log;
}
void Log::run()
{
    while (true)
    {
        if (Log::que[runIndex].empty())
        {
            runTop = __sync_or_and_fetch(&runTop, 1);
            sem.wait();
            runTop=__sync_and_and_fetch(&runTop,0);
        }
        cout << Log::que[runIndex].front() << endl; ///可以考虑写到文件中
        Log::que[runIndex].pop();
        runIndex = (runIndex + 1) % rollLen;
    }
}
string logform(char const *form, ...)
{
    char buf[250];
    va_list valst;
    va_start(valst, form);
    int len = vsnprintf(buf, 250, form, valst);
    buf[len + 1] = 0;
    return buf;
}
string getCurrentTimeStr()
{
    time_t t = time(NULL);
    char ch[64] = {0};
    strftime(ch, sizeof(ch) - 1, "%Y-%m-%d %H:%M:%S", localtime(&t)); //年-月-日 时-分-秒
    return (string)ch;
}
void info(char const *form, ...)
{
    char buf[250];
    va_list valst;
    va_start(valst, form);
    int len = vsnprintf(buf, 250, form, valst);
    buf[len] = 0;
    string str = "INFO ";
    str += buf;
    Log::push(str);
}
void error(char const *form, ...)
{
    char buf[250];
    va_list valst;
    va_start(valst, form);
    int len = vsnprintf(buf, 250, form, valst);
    buf[len] = 0;
    string str = "ERROR ";
    str += buf;
    Log::push(str);
}
#endif
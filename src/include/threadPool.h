#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include <queue>
#include "lock.h"
#include "client.h"
#include "log.h"
using namespace std;
struct Task
{
    Client *client;
    int tag=0;
    Task(Client *c,int t)
    {
        client=c;
        tag=t;
    }
};
class ThreadPool {
public:
    ThreadPool();
    ~ThreadPool();
    bool append(Task request);

private:
    static void* worker(void* arg);
    void run();

private:
    pthread_t *m_threads;
    queue<Task>que; 
    Locker locker;   
    Sem sem;                   
};
ThreadPool::ThreadPool() {


    m_threads = new pthread_t[4];

    for ( int i = 0; i < 4; ++i ) {
        info("%s create the %dth thread",pre,i);
        pthread_create(m_threads + i, NULL, worker, this );
        pthread_detach( m_threads[i]); 
    }
}

ThreadPool::~ThreadPool() {
    delete [] m_threads;
}

bool ThreadPool::append( Task request )
{
    locker.lock();
        que.push(request);
    locker.unlock();
    sem.post();
    return true;
}

void* ThreadPool::worker( void* arg )
{
    ThreadPool* pool = ( ThreadPool* )arg;
    pool->run();
    return pool;
}

void ThreadPool::run() {

    while (true) {
        sem.wait();
        locker.lock();
        if ( que.empty() ) {
            locker.unlock();
            continue;
        }
        Task request = (Task)que.front();
        que.pop();
        locker.unlock();
        switch (request.tag)
        {
        case 1:
            request.client->Read();
            break;
        case 2:
            request.client->Write();
        default:
            break;
        }
    }
}

#endif

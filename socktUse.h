#ifndef temp
#define temp
#include<iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include<fstream>
using namespace std;
class SocktUse
{
private:
    int fd;
    struct sockaddr_in init(int fd);
public:
    SocktUse();
    ~SocktUse();
    int get_fd();
};
int SocktUse::get_fd()
{
    return fd;
}
SocktUse::SocktUse()
{
    fd=socket(PF_INET,SOCK_STREAM,0);
	struct sockaddr_in address=SocktUse::init(fd);
    int status=bind( fd, ( struct sockaddr* )&address, sizeof( address ) );//0成功 -1失败
    if(status==-1)
    {
        cout<<"bind failed "<<endl;
        exit(0);
        return ;
    }
    status=listen( fd, 1 );//0成功 -1失败
    if(status==-1)
    {
        cout<<"listen failed"<<endl;
         exit(0);
        return ;
    }
    cout<<"main_sockt successful"<<endl;
}

SocktUse::~SocktUse()
{
    cout<<"main_scokt close"<<endl;
    close(fd);
}
struct sockaddr_in SocktUse::init(int fd)
{
	int reuse = 1;
    setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof( reuse ) );
	struct sockaddr_in address;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;
    address.sin_port = htons( 80 );
	return address;
}


#endif
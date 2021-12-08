#ifndef CLIENT_H
#define CLIENT_H
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
#include "log.h"
using namespace std;
class Client
{
private:
	char *IP;
	void set_fd(int fd);
	char* get_file();
public:
    Client();
    ~Client();
    void get_fd(int main_fd);
    bool Read();
    bool Write();
    int fd;
};
Client::Client()
{

}

Client::~Client()
{
	info("%s IP=%s client end",pre,IP);
	close(fd);
}
void Client::get_fd(int main_fd)
{
	struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof( client_address );
	fd = accept( main_fd, ( struct sockaddr* )&client_address, &client_addrlength );
	IP=inet_ntoa(client_address.sin_addr);
	info("%s IP=%s States=Accept ",pre,IP);
	set_fd(fd);
}
bool Client::Read()
{
	int limit=0;
	string readStr;
	while(true)
	{
		char buf[100];
		memset(buf,0,sizeof(buf));
		int read_bytes = recv(fd,buf,100,0);
		if(read_bytes==-1)
		{
			if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
			{
				int len=readStr.size();
				if(len>=4&&readStr.substr(len-4,4)=="\r\n\r\n")
					break;
				continue;
			}
			error("%s IP=%s States=error",pre,IP);
			return false;
		}
		if(read_bytes==0)
			break;
		for(int i=0;i<strlen(buf);i++)
			readStr+=buf[i];
	}
	info("%s IP=%s readLen=%d",pre,IP,readStr.size());
	return true;
}
bool Client::Write()
{
	char *buf=get_file();
	int write_bytes=send(fd,buf,strlen(buf),0);
	info("%s IP=%s writeLen=%d",pre,IP,write_bytes);
	return true;
}
char* Client::get_file()
{
    filebuf *pbuf;
    ifstream filestr;
    long size;
    filestr.open ("content", ios::binary);
    // 获取filestr对应buffer对象的指针 
    pbuf=filestr.rdbuf();
    // 调用buffer对象方法获取文件大小
    size=pbuf->pubseekoff (0,ios::end,ios::in);
    pbuf->pubseekpos (0,ios::in);
    // 分配内存空间 +1是关键，不然又些文件会乱码
    char *buffer=new char[size+1];
    // 获取文件内容
    pbuf->sgetn (buffer,size);
    buffer[size]='\0';//0这是关键
    filestr.close();
	string s=buffer;
	int pos=s.find("rand=");
	s=s.substr(0,pos+5)+char('0'+rand()%10)+s.substr(pos+5,s.size()-pos-1);
	delete []buffer;
    return (char*)s.data();
}
void Client::set_fd(int fd)
{
	int flag=fcntl(fd,F_GETFL,0);
	flag|=O_NONBLOCK;
	fcntl(fd,F_SETFL,flag);
}
#endif
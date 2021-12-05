#ifndef Client_h
#define Client_h
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
class Client
{
private:
	void set_fd(int fd);
	char* get_file();
public:
    Client();
    ~Client();
    void get_fd(int main_fd);
    void Read();
    void Write();
    int fd;
};
Client::Client()
{

}

Client::~Client()
{
	cout<<"client end"<<endl;
	close(fd);

}
void Client::get_fd(int main_fd)
{
	struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof( client_address );
	fd = accept( main_fd, ( struct sockaddr* )&client_address, &client_addrlength );
	cout<<"accept"<<endl;
	set_fd(fd);
}
void Client::Read()
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
			cout<<"意外错误"<<endl;
			break;
		}
		if(read_bytes==0)
			break;
		for(int i=0;i<strlen(buf);i++)
			readStr+=buf[i];
	}
	cout<<"------------"<<endl;
	cout<<"readLen="<<readStr.size()<<endl;
	cout<<"------------"<<endl;
	cout<<readStr<<endl;
	cout<<"------------"<<endl;
}
void Client::Write()
{
	char *buf=get_file();
	int write_bytes=send(fd,buf,strlen(buf),0);
	cout<<"------------"<<endl;
	cout<<"writeLen="<<write_bytes<<endl;
	cout<<"------------"<<endl;
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
	int num=rand()%10;
	char *bufferTemp=new char[size+4];
	for(int i=0,j=0;i<strlen(buffer);i++,j++)
	{
		bufferTemp[j]=buffer[i];
		if(i>=1&&buffer[i-1]=='d'&&buffer[i]=='=')
		{
			bufferTemp[++j]=' ';
			bufferTemp[++j]='0'+num;
			bufferTemp[++j]=' ';
		}
	}
	bufferTemp[size+3]='\0';
	delete []buffer;
    return bufferTemp;
}
void Client::set_fd(int fd)
{
	int flag=fcntl(fd,F_GETFL,0);
	if(flag==-1)
	{
		cout<<"set_fd failed="<<endl;
		return ;
	}
	flag|=O_NONBLOCK;
	fcntl(fd,F_SETFL,flag);
}
#endif
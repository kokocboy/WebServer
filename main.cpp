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
struct sockaddr_in init(int fd)
{
	struct sockaddr_in address;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;
    address.sin_port = htons( 80 );
	int reuse = 1;
    setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof( reuse ) );
	return address;
}
int get_client(int fd)
{
	struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof( client_address );
	int connfd = accept( fd, ( struct sockaddr* )&client_address, &client_addrlength );
	return connfd;
}
void set_fd(int fd)
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
string get_file()
{
    filebuf *pbuf;
    ifstream filestr;
    long size;
    char * buffer;
    filestr.open ("content", ios::binary);
    // 获取filestr对应buffer对象的指针 
    pbuf=filestr.rdbuf();
    // 调用buffer对象方法获取文件大小
    size=pbuf->pubseekoff (0,ios::end,ios::in);
    pbuf->pubseekpos (0,ios::in);
    // 分配内存空间 +1是关键，不然又些文件会乱码
    buffer=new char[size+1];
    // 获取文件内容
    pbuf->sgetn (buffer,size);
    buffer[size]='\0';//0这是关键
    filestr.close();
    // 输出到标准输出
    string htmlContent;
    for(int i=0;i<size;i++)
        htmlContent+=buffer[i];
    delete []buffer;
    return htmlContent;
}
const int N=1e6+5;
char str[N];
void Read(int client_fd)
{
	int limit=0;
	string readStr;
	while(true)
	{
		char buf[100];
		memset(buf,0,sizeof(buf));
		int read_bytes = recv(client_fd,buf,100,0);
		if(read_bytes==-1)
		{
			if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
			{
				limit++;
				cout<<"无数据可读"<<endl;
				int len=readStr.size();
				if(len>=4&&readStr.substr(len-4,4)=="\r\n\r\n"||limit==1000)
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
void Write(int client_fd,const char *str,int len)
{
	int write_bytes=send(client_fd,str,len,0);
	cout<<"------------"<<endl;
	cout<<"writeLen="<<write_bytes<<endl;
	cout<<"------------"<<endl;
	cout<<str<<endl;
	cout<<"------------"<<endl;
}
int main()
{
	int fd=socket(PF_INET,SOCK_STREAM,0);
	cout<<"fd="<<fd<<endl;
	struct sockaddr_in address=init(fd);
    int flag=bind( fd, ( struct sockaddr* )&address, sizeof( address ) );//0成功 -1失败
	cout<<"bind= "<<flag<<endl;
    int flag2=listen( fd, 1 );//0成功 -1失败
	cout<<"listen="<<flag2<<endl;
	string html=get_file();
	int n=html.size();
	for(int i=0;i<n;i++)
		str[i]=html[i];
	str[n]=0;
	cout<<"sleep 20s"<<endl;
	cout<<"end"<<endl;
	while (true)
	{
		int client_fd=get_client(fd);
		set_fd(client_fd);
		cout<<"accpet"<<endl;
		Read(client_fd);
		sleep(5);
		Write(client_fd,str,n);
		close(client_fd);
	}
	close(fd);
	return 0;
}

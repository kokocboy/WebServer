#ifndef CLIENT_H
#define CLIENT_H
#include <iostream>
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
#include <fstream>
#include <set>
#include <vector>
#include "log.h"
#include "epoll.h"
using namespace std;
class Client
{
private:
	void get_file();
	set<string> se;

public:
	Client();
	~Client();
	bool Read();
	bool Write();
	void Init();
	void Close();
	void Solve();
	void process();
	int fd;
	char *IP;
	string readStr;
	string writeStr;
	vector<string> requseHead;
};
Client::Client()
{
}
void Client::Init()
{
	EPOLL::addfd(fd, true);
}

Client::~Client()
{
}
bool Client::Read()
{
	info("%s IP=%s States=Read fd=%d", pre, IP, fd);
	char buf[100];
	memset(buf, 0, sizeof(buf));
	int read_bytes = recv(fd, buf, 100, 0);
	if (read_bytes == -1)
	{
		if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
		{
			int len = readStr.size();
			if (len >= 4 && readStr.substr(len - 4, 4) == "\r\n\r\n")
			{
				EPOLL::modfd(fd, EPOLLOUT);
				info("%s IP=%s readLen=%d", pre, IP, readStr.size());
				Solve();
			}
			return true;
		}
		Close();
		error("%s IP=%s States=error", pre, IP);
		return false;
	}
	if (read_bytes == 0)
		return false;
	readStr += buf;
	if (readStr.size() >= 4 && readStr.substr(readStr.size() - 4, 4) == "\r\n\r\n")
	{
		EPOLL::modfd(fd, EPOLLOUT);
		info("%s IP=%s readLen=%d", pre, IP, readStr.size());
		Solve();
	}
	else
	{
		EPOLL::modfd(fd, EPOLLIN);
	}
	return true;
}
bool Client::Write()
{
	cout<<readStr<<endl;
	info("%s IP=%s States=Write fd=%d", pre, IP, fd);
	char *buf = (char *)writeStr.data();
	if (!writeStr.size())
	{
		error("%s IP=%s noData", pre, IP);
		Close();
		return true;
	}
	int write_bytes = send(fd, buf, strlen(buf), 0);
	if (write_bytes <= 0)
	{
		error("%s IP=%s sendError", pre, IP);
		Close();
		return false;
	}
	info("%s IP=%s writeLen=%d", pre, IP, write_bytes);
	Close();
	return true;
}
void Client::Solve()
{
	info("%s IP=%s readContent=%d", pre, IP, readStr);
	string str;
	for (int i = 0, flag = 1; i < readStr.size(); i++)
	{
		if (readStr[i] == '\r')
		{
			if (flag)
			{
				str += " ";
				while (str.size())
				{
					int pos = str.find(" ");
					requseHead.push_back(str.substr(0, pos));
					str = str.substr(pos + 1, str.size() - pos - 1);
				}
				flag--;
				i++;
				continue;
			}
			//se.insert(str);
			str.clear();
			i++;
			continue;
		}
		str += readStr[i];
	}
	get_file();
}
void Client::get_file()
{
	filebuf *pbuf;
	ifstream filestr;
	long size;
	string resourcesUrl = "../resources";
	if(requseHead.size()<3){
		error("%s IP=%s noUrl", pre, IP);
		return ;
	}
	info("%s IP=%s url=%s", pre, IP,requseHead[1]);
	
	std::ifstream ifs(resourcesUrl + requseHead[1]);
	info("%s IP=%s url=%s", pre, IP, (char *)requseHead[1].data());
	if (!ifs.is_open() || requseHead[1] == "/"){
		requseHead[1] = "/404";
		if (requseHead[1] == "/favicon.ico"){
			return;
		}
	}
	resourcesUrl += requseHead[1];
	filestr.open((char *)resourcesUrl.data(), ios::binary);
	pbuf = filestr.rdbuf();
	size = pbuf->pubseekoff(0, ios::end, ios::in);
	pbuf->pubseekpos(0, ios::in);
	char *buffer = new char[size + 1];
	pbuf->sgetn(buffer, size);
	buffer[size] = '\0';
	filestr.close();
	writeStr = buffer;
	delete[] buffer;
}
void Client::Close()
{
	info("%s IP=%s client end", pre, IP);
	readStr.clear();
	se.clear();
	requseHead.clear();
	writeStr.clear();
	EPOLL::removefd(fd);
}
#endif
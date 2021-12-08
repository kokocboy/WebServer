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
#include "serverSockt.h"
#include "client.h"
using namespace std;
int main()
{
	ServerSockt *serverSockt=new ServerSockt();
	while (true)
	{
		Client *client=new Client();
		client->get_fd(serverSockt->get_fd());
		if(!client->Read()){
			delete client;
			continue;
		}
		client->Write();
		delete client;
	}
	delete serverSockt;
	return 0;
}

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
#include "socktUse.h"
#include "client.h"
using namespace std;
int main()
{
	SocktUse *mainSockt=new SocktUse();
	while (true)
	{
		Client *client=new Client();
		client->get_fd(mainSockt->get_fd());
		client->Read();
		client->Write();
		delete client;
	}
	delete mainSockt;
	return 0;
}

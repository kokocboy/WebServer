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
#include <fstream>
#include "../include/serverSockt.h"
#include "../include/client.h"
#include "../include/epoll.h"
#include "../include/threadPool.h"
#define MAX_FD 65536
using namespace std;
int main()
{
	ServerSockt *serverSockt=new ServerSockt();
	ThreadPool *threadPoll=new ThreadPool();
	epoll_event events[ 10 ];
	Client clinet[40];
	EPOLL::epollfd=epoll_create(5);
	EPOLL::addfd( serverSockt->get_fd(), false );
	while (true)
	{
		info("%s epollBlock",pre);
		int number= epoll_wait( EPOLL::epollfd, events, 10, -1 );
		 if ( ( number < 0 ) && ( errno != EINTR ) ) {
			 info("%s epollFailure",pre);
            break;
        }
		for ( int i = 0; i < number; i++ ) {

            int sockfd = events[i].data.fd;
			if(sockfd==serverSockt->get_fd())
			{
				struct sockaddr_in client_address;
    			socklen_t client_addrlength = sizeof( client_address );
				int fd = accept( serverSockt->get_fd(), ( struct sockaddr* )&client_address, &client_addrlength );
				char *IP=inet_ntoa(client_address.sin_addr);
				clinet[fd].fd=fd;
				clinet[fd].IP=IP;
				clinet[fd].Init();
				info("%s IP=%s States=Accept fd=%d",pre,IP,fd);
			}
			else if(events[i].events&EPOLLIN)
			{
				info("%s IP=%s States=ReadEvents fd=%d",pre,clinet[sockfd].IP,sockfd);
				threadPoll->append(Task(clinet+sockfd,1));
				//clinet[sockfd].Read();
			}
			else if(events[i].events&EPOLLOUT){
				info("%s IP=%s States=WriteEvents fd=%d",pre,clinet[sockfd].IP,sockfd);
				threadPoll->append(Task(clinet+sockfd,2));
				//clinet[sockfd].Write();
			}else{
				info("%s IP=%s States=SocktError fd=%d",pre,clinet[sockfd].IP,sockfd);
				clinet[sockfd].Close();
			}
		}
		info("%s epollNumbers=%d",pre,number);
	}
	delete serverSockt;
	//delete threadPoll;
	return 0;
}

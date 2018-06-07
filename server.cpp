#include "server.h"
#include "dbg.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define ERROR -1
Server::Server():
				epollfd_(0),
				listenfd_(0)
{
	if(ERROR == init())
	{
		log_err("failed to init server");
		exit(1);
	}
}
Server::~Server()
{
	/*if connected */
}

int Server::run()
{
	return 0;
}

int Server::init()
{
	listenfd_ = socket(AF_INET,SOCK_STREAM,0);	if(listenfd_ == ERROR)
	{
		log_err("fail init socket");
		return ERROR;
	}
	log_info("socket init success");

	struct sockaddr_in server_addr;
	memset(&server_addr,0,sizeof(sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);
	/*bind */
	if(ERROR == bind(listenfd_, (struct sockaddr*)(&server_addr), sizeof(server_addr)))
	{
		log_err("fail on bind");
		return ERROR;
	}
	log_info("bind success");
	/*isten*/
	if(ERROR == listen(listenfd_,5))
	{
		log_err("fail on listen");
		return ERROR;
	}
	log_info("listen success");
	epollfd_ = epoll_create(MAXEVENTSIZE);
	log_info("epoll_create success");
//	ctl_event(listenfd_,true);
	return 0;
}

int set_noblocking(int fd)
{
	int flags;
	if((flags = fcntl(fd, F_GETFL,0))==-1)
					flags = 0;
	return fcntl(fd,F_SETFL,flags | O_NONBLOCK);
}

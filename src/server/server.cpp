#include "server.h"


Server::Server():
				epfd_(0),
				listenfd_(0),
				epoll(NULL),
				sock(NULL)
{

	if(ERROR == init())
	{
		log_err("failed to init server");
		exit(1);
	}
}
Server::~Server()
{
				if(epoll != NULL) delete epoll;
				if(sock != NULL) delete sock;
	/*if connected */
}

int Server::run()
{
	epoll->do_epoll(listenfd_);
	return 0;
}

int Server::init()
{
	//socket
	sock = new Socket();
	listenfd_ = sock->init(); 
	if(listenfd_ == -1)
	{
		return -1;
	}
	//epoll
	epoll = new Epoll();
	if(epoll->init(listenfd_) == ERROR)
	{	
		log_err("epoll initialization");
	}

	printf("epoll_create success\n");
	return 0;
}

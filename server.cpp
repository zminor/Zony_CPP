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

	log_info("epoll_create success");
//	ctl_event(listenfd_,true);
//
	return 0;
}

/*
void Server::epoll_loop()
{
	struct sockaddr_in client_addr;
	socklen_t cli_len = sizeof(sockaddr);
	int numfds=0;
	int cli =0;
	int bufflen = 0;
	struct epoll_event events[MAXEVENTSIZE];

	char msg[] = "Hello and fk u!\n";
	int sent;
	while(true)
	{
					printf("start accepting...");
					if( (cli = accept(listenfd_, (struct sockaddr *)&client_addr,&cli_len))== ERROR)
					{
						log_err("accept err");
						exit(1);
					}
					sent = send(cli,msg, strlen(msg),0);
					printf("%d bytes sent: %s\n", sent, inet_ntoa(client_addr.sin_addr));
					close(cli);		
	}
}
*/

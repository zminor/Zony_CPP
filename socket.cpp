#include "socket.h"

Socket::Socket()
{

}

Socket::~Socket()
{

}

int Socket::init()
{
	int listenfd;
	struct sockaddr_in server_addr;
	/*socket*/
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd == -1)
	{
		log_err("socket error");
		exit(1);
	}
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);
	/*bind*/
	if(bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		log_err("bind error")i;
		exit(1);
	}
	/*listen*/
	if(listen(listenfd,5) == -1)
	{
		log_err("listen error");
		exit(1);
	}
	return listenfd;
}


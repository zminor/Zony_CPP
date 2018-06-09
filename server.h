#ifndef __SERVER_H__
#define __SERVER_H__

#include "socket.h"
#include "epoll.h"
#include "dbg.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

class Server
{
	public:
		Server();
		virtual ~Server();
		int run();
		int init();

	private:
		int epfd_;
		int listenfd_;

		Epoll *epoll;
		Socket * sock;

};
#endif

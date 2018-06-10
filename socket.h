#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "dbg.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

class Socket
{
public:
	Socket();
	virtual ~Socket();

	int init();
	int sock_non_blocking(int fd);
};

#endif

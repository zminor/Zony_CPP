#ifndef __EPOLL_H__
#define __EPOLL_H__

#include <sys/epoll.h>
#include "dbg.h"
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define ERROR -1

#define EPOLLEVENTS_NUM 100
#define BUFF_SIZE 1024
#define FD_SIZE 1024

#define IP_ADDR "127.0.0.1"

class Epoll
{
public:
	Epoll();
	virtual ~Epoll();
	
	int init(int listenfd);
	void run();

	int create();
	void handle_accept(int epfd, int listenfd,epoll_event * event);
	
	void handle_events(int epfd, struct epoll_event *events,int num, int listenfd, char* buf);
	void do_epoll (int listenfd);
	void add_event(int epfd, int fd, int state);
	void mod_event(int epfd, int fd, int state);
	void del_event(int epfd, int fd, int state);

	void do_write(int epfd, int fd, char *buf);
	void do_read (int epfd, int fd, char *buf);
	
private:
	int set_noblocking(int fd);

	int listenfd_;
	int epfd_;
};


#endif

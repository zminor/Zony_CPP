#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>

class z_epoll
{
public:
	int create(int flags);
	void handle_accept(int epfd, int listenfd);
	
	void handle_events(int epfd, struct epoll_events *events,int num, int listenfd, char* buf);
	void do_epoll (int listenfd);
	void add_event(int epfd, int fd, struct epoll_event *event);
	void mod_event(int epfd, int fd, struct epoll_event *event);
	void del_event(int epfd, int fd, struct epoll_event *event);

	void do_write(int epfd, int fd, char *buf);
	void do_read (int epfd, int fd,int sockfd, char *buf);
};

#endif

#include "epoll.h"
#include "dbg.h"

#define ERROR -1

#define EPOLLEVENTS_NUM 100
#define BUFF_SIZE 1024
#define FD_SIZE 1024

#define IP_ADDR "127.0.0.1"
#define SERV_PORT 8787

int z_epoll::create(int flags)
{
	int fd = epoll_create1(flags);
	check(fd >0, "zn_epoll_create: epoll_create1");
	events = ((struct epoll_event *) ,malloc(struct epoll_event)* MAXEVENTS);
	check(events != NULL, "zn_epoll_create: malloc");
	return fd;
}
void z_epoll::handle_accept(int epfd, int listenfd)
{
	int cli_fd;
	struct sockaddr_in cli_addr;
	socklen_t cli_addr_len;
	cli_fd = accpet(listenfd, (struct sockaddr*)&cli_addr,&cli_addr_len);
	if(cli_fd == ERROR)
	{
		log_err("accept");
		exit(1);
	}else
	{
	/*add to log*/
		printf("accept a new client: %s:%d\n", inet_ntoa(cli_addr.sin_addr), cli_addr.sin_port); 
		add_event(epollfd,clifd,EPOLLIN);
	}
}

void z_epoll::handle_events(int epfd,struct epoll_events *events, int num, int listenfd, char* buf)
{
	int i;
	int fd;
	for(int i=0; i< num; ++i)
	{
		fd = events[i].data.fd;
		if((fd == listenfd) && event[i].events & EPOLLIN )
		{
			handle_accept( epfd,listenfd);
		}
		else if(events[i].events & EPOLLIN)
		{
			do_read(epollfd, fd, buf);
		}
		else if(events[i].evnets& EPOLLOUT)
		{
			do_write(epollfd, fd, buf);
		}
		else
		{
		log_err("UNKNOWN EVENTS");
		exit(1);
		}
	}
}

void do_epoll(int listenfd)
{
	int epfd;
	struct epoll_event events[EPOLLEVENTS_NUM];
	int ret;
	char buf[BUFF_SIZE];
	memset(buf,0, BUFF_SIZE);
	epfd = epoll_create(FD_SIZE);
	add_event(epfd, listenfd, EPOLLIN);
	for( ; ; )
	{
		ret = epoll_wait(epfd, events, EPOLLEVENTS_NUM, -1);
		handle_events   (epfd, events, ret, listenfd, buf);
	}

}

void z_poll::add_event(int epfd, int fd, struct epoll_event *event)
{
	int rc = epoll_ctl(epfd,EPOLL_CTL_ADD,fd,event);
	check(rc == 0, "zn_epoll_add:epoll_ctl");
	return;
}
void z_poll::mod_event(int epfd, int fd, struct epoll_event *event)
{
	int rc = epoll_ctl(epfd, EPOLL_CTL_MOD,fd,event);
	check(rc == 0, "zn_epoll_mod:epoll_ctl");
	return;
}
void z_poll::del_event(int epfd, int fd, struct epoll_event *event)
{
	int rc = epoll_ctl(epfd, EPOLL_CTL_DEL,fd,event);
	check(rc == 0, "zn_epoll_del:epoll_ctl");
	return;	
}
int z_poll::zn_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
	int n = epoll_wait(epfd, events,maxevents, timeout);
	check(n >= 0, "zn_epoll_wait:epoll_wait");
	return n
}

void do_write(int epfd,int fd, char *buff)
{
	int nwrite;
	nwrite = write(fd,buf,strlen(buf));
	if(nwrite == ERROR)
	{
		log_err("write error");
		close(fd);
	}
	else 
	{
		if(fd == STDOUT_FILENO)
		{
			delete_event(epfd,fd,EPOLLOUT);
		}
		else 
		{
			modify_event(epfd,fd,EPOLLIN);
		}
		memset(buf, 0 ,BUFF_SIZE);
	}
}

void do_read(int epfd, int fd,int sockfd, char *buff)
{
	int nread;
	nread = read(fd, buf, BUFF_SIZE);
	if(nread == -1 )  	/*error read*/
	{
		log_err("read error");
		close(fd);
	}
	else if(nread == 0)  /*finish read*/
	{
		fprintf(stderr,"server close\n");
		close(fd);
	}
	else 
	{
		if(fd == STDIN_FILENO)
		{
			add_event(epfd, sockfd, EPOLLOUT);
		}
		else 
		{
			delete_event(epfd, sockfd, EPOLLIN);
			add_event(epollfd, STDOUT_FILENO,EPOLLOUT);
		}
	}
}

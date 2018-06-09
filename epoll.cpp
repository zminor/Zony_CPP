#include "epoll.h"

Epoll::Epoll():listenfd_(0),epfd_(0)
{
}

Epoll::~Epoll()
{
}
void Epoll::handle_events(int epfd, struct epoll_event *events,int num, int listenfd, char* buf)
{
		int fd;
	for(int i=0; i< num; ++i)
	{	
		fd = events[i].data.fd;
		if((fd == listenfd)&& (events[i].events & EPOLLIN))
		{
			handle_accept(epfd,listenfd);
		}
		else if(events[i].events & EPOLLIN)
		{
			do_read(epfd_, fd, buf);
		}
		else if(events[i].events & EPOLLOUT)
			do_write(epfd_, fd, buf);
	}
}
int Epoll::set_noblocking(int fd)
{
	int flags;
	if((flags = fcntl(fd, F_GETFL,0))==-1)
	{	
		flags = 0;
	}
	return fcntl(fd,F_SETFL,flags | O_NONBLOCK);
}

int Epoll::init(int listenfd)
{
	listenfd_ = listenfd;
	epfd_ = epoll_create(FD_SIZE);
	if(epfd_ == ERROR)
	{
		log_err("epoll create");
		return ERROR;
	}

	add_event(epfd_,listenfd_,EPOLLIN);
	return 0;
}
void Epoll::add_event(int epfd,int fd,int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
}
void Epoll::del_event(int epollfd,int fd,int state)
{
  struct epoll_event ev;
  ev.events = state;
  ev.data.fd = fd;
  epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
}

void Epoll::mod_event(int epollfd,int fd,int state)
{
  struct epoll_event ev;
  ev.events = state;
  ev.data.fd = fd;
  epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
}
void Epoll::run()
{
	do_epoll(listenfd_);
}

void Epoll::do_epoll(int listenfd)
{
	char buf[BUFF_SIZE];
	memset(buf,0,BUFF_SIZE);
	int ret;
	struct epoll_event events[EPOLLEVENTS_NUM];
	for(;;)
	{
		ret = epoll_wait(epfd_,events,EPOLLEVENTS_NUM,-1);
		handle_events(epfd_,events,ret,listenfd,buf);
	} 
	close(epfd_);
}

void Epoll::do_read(int epfd, int fd, char*buf)
{
		int nread;
		nread = read(fd,buf,BUFF_SIZE);
		if(nread == -1)
		{
			log_err("read error");
			close(fd);
			del_event(epfd, fd ,EPOLLIN);
		}
		else if (nread == 0)
		{
			printf("client close");
			close(fd);
			del_event(epfd, fd, EPOLLIN);
		}
		else
		{
			printf("message read...\n");
			mod_event(epfd,fd,EPOLLOUT);
		}
}

void Epoll::handle_accept(int epfd,int listenfd)
{
	int clifd;
	struct sockaddr_in cliaddr;
	socklen_t  cliaddrlen;
	clifd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
	if(clifd == ERROR)
	{
		 log_err("accept error:");	
	}
	else
	{
		 printf("accept a new client: %s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
     add_event(epfd_,clifd,EPOLLIN);
	}
}
void Epoll::do_write(int epfd, int fd, char *buf)
{
	int nwrite=0;
	nwrite = write(fd,buf,strlen(buf));
	if(nwrite == -1)
	{
		log_err("write error");
		del_event(epfd,fd,EPOLLOUT);
		close(fd);
	}
	else
	{
		mod_event(epfd,fd,EPOLLIN);
		memset(buf,0,BUFF_SIZE);
	}
	
	
}

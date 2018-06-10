#include "epoll.h"

Epoll::Epoll():listenfd_(0),epfd_(0)
{
}

Epoll::~Epoll()
{
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
	int event_num;
	struct epoll_event events[EPOLLEVENTS_NUM];
	for(;;)
	{
		event_num = epoll_wait(epfd_,events,EPOLLEVENTS_NUM,-1);
		handle_events(epfd_,events,event_num,listenfd,buf);
	} 
	close(epfd_);
	close(listenfd);
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
			printf("message read: %s\n",buf);
			mod_event(epfd,fd,EPOLLOUT);
		}
}

void Epoll::handle_events(int epfd, struct epoll_event *events,int num, int listenfd, char* buf)
{
		int fd;
	for(int i=0; i< num; ++i)
	{	
		fd = events[i].data.fd;
		if((fd == listenfd)&& (events[i].events & EPOLLIN))
		{
			handle_accept(epfd,listenfd,&(events[i]));
		}
		else if(events[i].events & EPOLLIN)
		{
			do_read(epfd_, fd, buf);
		}
		else if(events[i].events & EPOLLOUT)
			do_write(epfd_, fd, buf);		
		else if((events[i].events & EPOLLERR) ||(events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))
		{
			log_err("epoll err fd : %d", fd);
			close(fd);
			continue;
		}
	}
}

void Epoll::handle_accept(int epfd,int listenfd, epoll_event* event)
{
	int clifd;
	struct sockaddr_in cliaddr;
	socklen_t  cliaddrlen;
	int rc;
	while(1)
	{
		clifd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);	
		if(clifd == ERROR)
		{
			if((errno == EAGAIN)||(errno == EWOULDBLOCK))		break;
			else
			{			
	 			log_err("accept error:");
				break;
			}
		}
		rc = set_noblocking(listenfd);
		check(rc == 0, "make_socket_non_blocking");
		
	  printf("accept a new client: %s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
		event->events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    add_event(epfd_,clifd,event->events);

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
		printf("message sent %s", buf);
		mod_event(epfd,fd,EPOLLIN);
		memset(buf,0,BUFF_SIZE);
	}	
}

int Epoll::set_noblocking(int listenfd)
{	
	int flags, ret;
	flags = fcntl(listenfd, F_GETFL,0);
	if(flags == ERROR)
	{
		log_err("fcntl");
		return ERROR;
	}
	flags |= O_NONBLOCK;
	ret = fcntl( listenfd, F_SETFL, flags);
	if(ret == ERROR)
	{
		log_err("fcntl");
		return ERROR;
	}
	return 0;
}

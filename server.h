#ifndef __SERVER_H__
#define __SERVER_H__

#define PORT 3000
#define TIMEWAIT 100
#define BUFFLEN 2048
#define MAXEVENTSIZE 10

class Server
{
	public:
		Server();
		virtual ~Server();
		int run();
		int init();
	private:
		int set_noblocking(int fd);
		int epollfd_;
		int listenfd_;

};
#endif

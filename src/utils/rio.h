#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <cstring>
//a buffer named rio_t
#define RIO_BUFSIZE 8192
typedef struct rio_t{
    int rio_fd; 
    int rio_cnt; //unread bytes in rio_buf
    char *rio_bufptr; //next unread byte in rio_buf
    char rio_buf[RIO_BUFSIZE]; //internal buffer
    
    rio_t(int fd)
        :rio_fd(fd),rio_cnt(0),rio_bufptr(rio_buf){ }
} rio_t;
class Rio{
public:
    //io function without internal buffer
    //Succeed: return the number of byte; EOF: return 0; Fail: return -1
    ssize_t readn(int fd,void *usrbuf,size_t n);
    ssize_t writen(int fd,void *usrbuf,size_t n);    

    //io function with internal buffer
    //Succeed: return the number of byte; EOF: return 0; Fail: return -1
    ssize_t readlineb(rio_t *rp,void *usrbuf,size_t maxlen);
    ssize_t readnb(rio_t *rp,void *usrbuf,size_t n);

private:
    //read with buffer
    ssize_t rio_read(rio_t *rp,char *usrbuf,size_t n);
};
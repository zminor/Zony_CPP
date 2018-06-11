#include "myrio.h"

//the difference between these function and system's is that:
//this function can deal with the interrupt. 
ssize_t Rio::readn(int fd,void *usrbuf,size_t n){
    size_t nleft = n;
    ssize_t nread;
    char *buf = static_cast<char*>(usrbuf);
    while(nleft > 0){
        if((nread = read(fd,buf,nleft))<0){
            if(errno ==EINTR) nread = 0;
            else return -1;
        }else if(nleft == 0){
            break;
        }else{
            nleft -= nread;
            buf += nread;
        }
    }
    return (n - nleft);
}
ssize_t Rio::writen(int fd,void *usrbuf,size_t n){
    size_t nleft = n;
    ssize_t nwriten;
    char *buf = static_cast<char*>(usrbuf);

    while(nleft > 0){
        if((nwriten = write(fd,buf,nleft)) <= 0){
            if(errno == EINTR) nwriten = 0;
            else return -1;
        }else{
            nleft -= nwriten;
            buf += nwriten;
        }
        return n;
    }
}

ssize_t Rio::rio_read(rio_t *rp,char *usrbuf,size_t n){
    int cnt;
    while(rp->rio_cnt <= 0){ //refill rio_buf if buf is empty
        rp->rio_cnt = read(rp->rio_fd,rp->rio_buf,sizeof(rp->rio_buf));        
        if(rp->rio_cnt < 0){
            if(errno != EINTR) return -1;
        }else if(rp->rio_cnt == 0){
            return 0;
        }else{
            rp->rio_bufptr = rp->rio_buf;
        }
    }
    cnt = n;
    if(rp->rio_cnt < n)
        cnt = rp->rio_cnt;
    memcpy(usrbuf,rp->rio_bufptr,cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;

    return cnt;
}
ssize_t Rio::readlineb(rio_t *rp,void *usrbuf,size_t maxlen){
    char c;
    char *buf = static_cast<char*>(usrbuf);
    int n,rc;
    for(n=1;n<maxlen;n++){
        if((rc = rio_read(rp,&c,1)) == 1){
            *buf = c;
            buf++;
            if(c == '\n')
                break;
        }else if(rc == 0){
            if(n == 1) return 0;
            else break;
        }else{
            return -1;
        }
    }
    *buf = '\0';
    return n;
}
ssize_t Rio::readnb(rio_t *rp,void *usrbuf,size_t n){
    size_t nleft = n;
    ssize_t nread;
    char *buf = static_cast<char*>(usrbuf);
    while(nleft > 0){
        if((nread = rio_read(rp,buf,nleft)) < 0){
            if(errno == EINTR) nread = 0;
            else return -1;
        }else if(nread == 0){
            break;
        }else{
            nleft -= nread;
            buf += nread;
        }
    }
    return (n-nleft);
}
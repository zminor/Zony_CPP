#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#define BUFSIZE 41000
#define URLSIZE 1024

#define INVALID_SOCKET -1

#define ERROR -1
#define SUCESS 0

class Http_request
{
public:
Http_request();
~Http_request();
int Http_get(const char* str_method, const char* str_url, const char* str_data, char* str_response);
int Http_post(const char*, const char* strData, char* str_response);

private:
int Http_request_exec(const cahr* str_method, const char* str_url, const char* str_data, char* str_response);
char* Http_header_create(const char* str_method, const char* str_url, const char* strData);
char* Http_data_transmit(char* str_http_head, const int listenfd);

int Get_port_from_url(const char* str_url);
char* Get_ip_from_url(const char* str_url);
char* Get_param_from_url(cosnt char* str_url);
char* Get_host_addr_from_url(const char* str_url);

void check_addr(char* str_addr, char* url);

int sock_check(int listenfd);
int init_socket();
void close_sock(); // TODO what if close fail
int connect_sock();
int commu_sock();

int listenfd_ = INVALID_SOCKET;
};



#endif

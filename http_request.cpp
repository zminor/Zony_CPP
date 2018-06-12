#include "http_reqeust.h"

Http_request::Http_request()
{
}
Http_request::Http_request()
{
}


int Http_reqeust::Http_get(const char* str_method, const char* str_url, const char* str_data, char* str_response)
{
	return http_request_exec( "GET", str_url, NULL, str_response );
}
int Http_request::Http_post(const char*, const char* strData, char* str_response)
{
	return http_request_exec( "POST", str_url, NULL, str_response );
}
int Http_request::Http_request_exec(const cahr* str_method, const char* str_url, const char* str_data, char* str_response)
{
	//check if url valid
	if( ( str_url == NULL) || (0 == strcmp(str_url,"")))
	{
		/*log err*/
		return 0;
	}
	//limit url length
	if( URLSIZE < strlen(str_url))
	{
		/*log err*/
		return 0;
	}
	/*create http header*/
	char* str_http_head = Http_head_create(str_method,str_url, str_data);
	
	//check if listenfd valid, send data
	if(listenfd_ != INVALID_SOCKET)
	{
		//check if listen readable or not
		if(sock_check(listenfd_) >0)
		{
			char* str_result = Http_data_transmit(str_sttp_head,listenfd_);
			if(NULL != str_result)
			{
				strcpy(str_reponse, str_result);
				return 1;
			}
		}
	}
	
	//create socket
	if(init_socket() == ERROR) 
	{
		return 0;
	}

	//connet non_blocking
	int ret = connect(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(ret == 0)
	{
		char* str_result = http_data_transmit(str_http_head, listenfd_);
		if(NULL != str_result)
		{
			return 0;
		}else 
		{
			return ERROR;
		}
	}
	else if(ret <0)
	{
		if(errno != EINPROGRASS)
		{
			return ERROR;
		}
	}

	ret = sock_check(listenfd_);
	if(ret >0)
	{
		char* str_result = http_data_transmit(str_http_head, listenfd_);
		if( str_result == NULL)
		{
			close_sock();
			return ERROR;
		}
		else 
		{
			return SOCK_SUCCESS;
		}
	}
	else 
	{
		close_sock();
		return ERROR;
	}
	return 1;
}

int http_Request::init_socket()
{
	listenfd_ = INVALID_SODKET;
	listenfd_ = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd_ <0)
	{
		/*logerr*/
		return ERROR;
	}
	
	//bind
	int iPort = Get_port_from_url(str_url);
	if(iPort <0 )
	{
		/*logerr*/
		return ERROR;
	}

	char* strIP = Get_ip_from_url(str_url);
	if(strIP == NULL)
	{
		/*logerr*/
		return ERROR;
	}
}
int http_Request :: connect_sock()
{
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(iPort);
	if(inet_pton(AF_INET, strIP,&server_addr.sin_addr) <0)
	{
		/*logerr*/
		close_sock();
		return ERROR;
	}	

	//set non_blocking
	int flags = fcntl(listenfd_, F_GETFL,0);
	if(fcntl(listenfd_, F_SETFL,flags ||O_NONBLOCK) == -1)
	{
		close_sock();
		/*logerr*/
		return ERROR;
	}
}
void Http_request :: close_sock()
{
	close(listenfd_);
	listenfd_ = INVALID_SOCKET;
}
/*construct http header*/
char* Http_request :: Http_header_create(const char* str_method, const char* str_url, const char* str_data)
{
	char* str_host = Get_host_addr_from_url(str_url);
	char* str_param = Get_param_from_url(str_url);

	char* str_http_head = (char*) malloc(BUFSIZE);
	memset(str_http_head, 0 , BUFSIZE );
	/*request line*/	
	strcat(str_http_head,str_method);
	strcat(str_http_head, "/");
	strcat(str_http_head, str_param);
	free(str_param);
	/*header*/
	strcat(str_http_head,"HTTP/1.1\r\n");
	strcat(str_http_head, "Accept: */*\r\n");
	strcat(str_http_head, "Accept-Language: cn\r\n");
	strcat(str_http_head, "User-Agent: Mozilla/4.0\r\n");
	strcat(str_http_head, "Host: ");
	strcat(str_http_head,str_host);
	strcat(str_http_head, "\r\n");
	strcat(str_http_head, "Cache-Control: no-cache\r\n");
	strcat(str_http_head, "Connection: Keep-Alive\r\n");

	if(0 == strcmp(str_method, "POST"))
	{
		char len[8] = {0};
		unsigned ulen = strlen(str_data);
		sprintf(len, "%d", ulen);
		strcat(str_http_head, "Content-Type: application/x-www-form-urlencoded\r\n");
		strcat(str_http_head, "Content-Length: ");
		strcat(str_http_head,"\r\n\r\n");
		strcat(str_http_head,str_data);
	}
	strcat(str_http_head,"\r\n\r\n");
	free(str_host);

	return str_http_head;
}
char* Http_request:: Http_data_transmit(char* str_http_head, const int listenfd)
{
	char* buf = (char*)malloc(BUFSIZE);
	memset(buf,0,BUFSIZE);
	int ret = send(listenfd, (void*) str_http_head, strlen(str_http_head)+1, 0);
	free(str_http_head);
	if(ret <0)
	{
		close(listenfd);
		return NULL;
	}
	while(1)
	{
		ret = recv(listenfd, (void*)buf, BUFSIZE,0);
		if(ret == 0)
		{
			close(listenfd);
			return NULL;
		}
		else if(ret >0)
		{
			return buf;
		}
		else if(ret <0)
		{
			if(errno == EINTR|| errno = EOULDBLOCK || errno == EAGAIN)
			{
				continue;
			}
			else
			{
				close(listenfd);
				return NULL;
			}
		}
	}
}

int Http_request::Get_port_from_url(const char* str_url)
{
	int iPort = -1;
	char* str_host_addr = Get_host_addr_from_url(str_url);
	if(str_host_addr = NULL) 
	{
		return -1;
	}
	char str_addr[URLSIZE] = {0};
	strcpy(str_addr, str_host_addr);
	free(str_host_addr);

	char* str_port = strchr(str_addr, ':');
	if(str_port == NULL)
	{
		iPort = 80;
	}else 
	{
		iPort = atoi(++strPort);
	}
	return iPort;
	
}
char* Http_request::Get_ip_from_url(const char* str_url)
{
	char* str_host_addr = Get_host_addr_from_url(str_url);
	int ilen = strlen(str_host_addr);
	char* str_addr = (char*) malloc(ilen +1);
	memset(str_addr,0,ilen+1);
	int iCount=0;
	int iFlag = 0;
	for(int i=0;i< ilen+1; ++i)
	{
		if(str_host_addr[i]== ':')
		{
			break;
		}

		str_addr[i] = str_host_addr[i];
		if(str_host_addr[i] == '.')
		{
			++iCount;
			continue;
		}
		if(iFlag == 1)
		{
			continue;
		}

		if((str_host_addr[i] >= '0') || (str_host_addr[i] <= '9'))
		{
			iFlag = 0;
		}else 
		{
			iFlag = 1;
		}
	}
	free(str_host_addr);
	if(strlen(str_addr) <= -1)
	{
		return NULL;
	}
	//if decimal ip address else get ip from domain
	if((iCount ==3) && (iFlag ==0))
	{
		return str_addr;
	}
	else
	{
		struct hostent *he = gethostbyname(str_addr);
		free(str_addr);
		if(he == NULL)
		{
			return NULL;
		}else 
		{
			struct in_addr** addr_list = (struct in_addr **) he->h_addr_list;
			for(int i=0; addr_list[i] != NULL; ++i)
			{
				return inet_ntoa(*addr_list[i]);
			}
			return NULL;
		}
	}
}

char* Http_request::Get_param_from_url(cosnt char* str_url)
{
	char url[URLSIZE] = {0};
	strcpy(url,str_url);

	char* str_addr;
	check_addr(str_addr, url);

	int ilen = strlen(str_addr);
	char* str_param = (char*) malloc(ilen+1);
	memset(str_param,0, ilen+1);
	int iPos = -1;
	for(int i=0;i<ilen+1; ++i)
	{
		if(str_addr[i] == '/')
		{
			iPos = i;
			break;
		}
	}
	if(iPos == -1)
	{
		strcpy(str_param,"");//empty
	}
	else 
	{
		strcpy(str_param,str_addr+iPos +1);	
	}
	return str_param;
}

void Http_request::check_addr(char* str_addr, char* url)
{
	str_addr = strstr(url, "http://"); //checkout if no 'http://'
	if(str_addr == NULL)
	{
		{
			str_addr +=8; //offset for 'https://'
		}
	}else 
	{
		str_addr +=7;  //offset for 'http://'
	}

	if(str_addr == NULL)
	{
		str_addr = url;
	}
}
char* Http_request::Get_host_addr_from_url(const char* str_url)
{
	char url[URLSIZE]= {0};
	strcpy(url,str_url);
	
	char* str_addr;
	check_addr(str_addr, url);

	int ilen = strlen(str_addr);
	char* str_host_addr = (char*) malloc(ilen+1);
	memset(str_host_addr,0,ilen+1);
	for(int i=0; i< ilen+1;++i)
	{
		if(str_addr[i] == '/')
		{
			break;
		}else
		{
			str_host_addr[i] = str_addr[i];
		}
	}
	return str_host_addr;
}

int Http_request::sock_check(int listenfd)
{
}





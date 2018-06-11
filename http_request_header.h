#ifndef HTTP_REQUEST_HEADER
#define HTTP_REQUEST_HEADER

#include "http_header.h"

class Http_request_header:public Http_header
{
public:
	Http_request_header();
	virtual ~Http_request_header();
}
#endif 

#ifndef HTTP_REQUEST_HEADER_H
#define HTTP_REQUEST_HEADER_H

#include "http_request.h"

class Http_request_header: public Http_header
{
public:
	Http_request_header();
	Http_request_header(char* buffer);
	~Http_request_header();

	int Get_status()
	{
		return m_status;
	}
	void Set_status(int value)
	{
		m_status = value;
	}
	string Get_status_string()
	{
		return m_status_string;
	}
	void Set_status_string(string value)
	{
		m_status_string = value;
	}
private:
	int m_status;
	string m_status_string;
};


#endif

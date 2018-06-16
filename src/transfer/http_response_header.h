#ifndef HTTP_RESPONSE_HEADER_H
#define HTTP_RESPONSE_HEADER_H

#include "http_response.h"

class Http_response_header: public Http_header
{
public:
	Http_response_header();
	Http_response_header(char* buffer);
	~Http_response_header();

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

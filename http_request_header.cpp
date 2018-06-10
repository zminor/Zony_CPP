#include "http_request_header.h"

Http_request_header:: Http_request_header(): m_status(-1), m_status_string("")
{

}
Http_request_header::Http_request_header(char* buffer): Http_header(buffer)
{
		string line = Get_first_line();
		
		m_status = -1;
	
}
Http_request_header::~Http_request_header()
{

}



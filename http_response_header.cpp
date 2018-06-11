#include "http_response_header.h"

Http_response_header:: Http_response_header(): m_status(-1), m_status_string("")
{

}
Http_response_header::Http_response_header(char* buffer): Http_header(buffer)
{
		string line = Get_first_line();
		string[] str_arr = line.split(' ');
		m_status = -1;
		if(sizeof(str_arr) >1)
		{
			bool success = false;
			m_status = std::stoi(str_arr[i]);
		}
		if(sizeof(str_arr)>2)	
		{
			m_statusString = arr[2];
		}
}
Http_response_header::~Http_response_header()
{

}



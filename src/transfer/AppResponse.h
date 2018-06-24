#ifndef __APPRESPONSE_H__
#define __APPRESPONSE_H__

#include <map>
#include <string>

namespace Transfer
{
	struct app_response 
	{
		void *response_data;
		size_t data_size;
	};
}

#endif

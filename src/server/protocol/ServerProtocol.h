#ifndef __SERVERPROTOCOL_H__
#define __SERVERPROTOCOL_H__

#include "../Request.h"
#include "../ServerControls.h"
#include "../ServerSettings.h"

#include "../../transfer/HttpStatusCode.h"
namespace HttpServer
{

	class ServerProtocol
	{
	public:
		virtual	ServerProtocol *process() = 0;
		virtual void close() = 0;
	
	};


}
#endif

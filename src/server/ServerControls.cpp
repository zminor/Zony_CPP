#include "ServerControls.h"

namespace HttpServer
{
	ServerControls::ServerControls():
					eventProcessQueue(nullptr),
					eventNotFullQueue(nullptr),
					eventUpdateModule(nullptr)
	{	
	}

	ServerControls::~ServerControls()
	{
		this->clear();
	}
	

	void ServerControls:: clear()
	{
	
	}

	void ServerControls::setProcess(const bool flag)
	{
	}

	void ServerControls::stopProcess()
	{
	
	}

	void ServerControls::setRestart(const bool flag)
	{
	
	}

	void ServerControls::setUpdateModule()
	{
	
	}

	void ServerControls::setProcessQueue()
	{
	
	}
}

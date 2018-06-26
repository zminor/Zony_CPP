#include "ServerSettings.h"

namespace HttpServer
{
	ServerSettings::~ServerSettings()
	{
		this->clear();
	}

	void ServerSettings::clear()
	{
			if(this-> global.empty() == false)
			{
					this->global.clear();
			}
	}


	void ServerSettings:: addDataVariant(DataVariant::Abstract *dataVariant)
	{
	
	}

}

#ifndef __SERVERSETTINGS_H__
#define __SERVERSETTINGS_H__

#include <unordered_map>
#include "data-variant/Abstract.h"
#include "ServerApplicationsTree.h"

namespace HttpServer
{
	class ServerSettings
	{
	public:
			std::unordered_map<std::string, std::string> global;  							//Global settings
			std::unordered_map<std::string, DataVariant::Abstract *> variants;	//Data
			std::unordered_map<std::string, std::string> mimes_types;						//Mime types
			ServerApplicationsTree apps_tree;
	public:
			ServerSettings() = default;
			~ServerSettings();

			void addDataVariant(DataVariant::Abstract *dataVariant);

			void clear();
	};

}
#endif

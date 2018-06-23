#ifndef __SERVERSETTINGS_H__
#define __SERVERSETTINGS_H__

#include <unordered_map>

namespace HttpServer
{
	class ServerSettings
	{
	public:
			std::unordered_map<std::string, std::string> global;  //Global settings
	//		std::unordered_map<std::string, DataVariant::Abstract *> variants;
	//		std::unordered_map<std::string, std::string> mimes_types;
	//		ServerApplicationsTree apps_tree;
	public:
			ServerSettings() = default;
			~ServerSettings();

//			void addDataVariant(DataVariant::Abstract *dataVariant);

			void clear();
	};

}
#endif

#ifndef __CONFIGPARSER_H__
#define __CONFIGPARSER_H__

#include "../ServerSettings.h"
#include "../../system/Module.h"
#include "../ServerApplicationsTree.h"
#include "../../utils/Utils.h"

#include <iostream>
#include <fstream>
#include <vector>

namespace HttpServer
{
	class ConfigParser
	{
	public:
		bool loadConfig(
			const std::string &conf,
			ServerSettings &settings,
			std::vector <System::Module> &modules
		);

	private:
		struct ServerApplicationDefaultSettings
		{
			std::string temp_dir;
			size_t request_max_size;
		};
		
		static bool includeConfigFile(
			const std::string &fileName,
			std::string &strBuf,
			const size_t offset = 0
		);

		static bool addApplication(
			const std::unordered_multimap <std::string, std::string> &app,
			const ServerApplicationDefaultSettings & defaults,
			std::vector <System::Module> &modules,
			ServerApplicationsTree & apps_tree
		);

		static bool parseMimes(										
										const std::string &fileName,
										std::unordered_map<std::string, std::string> &mimes_types
										);

	};
}
#endif




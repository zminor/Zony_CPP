#include "ConfigParser.h"

namespace HttpServer
{
	bool ConfigParser:: loadConfig(
			const std::string &conf,
			ServerSettings &settings,
			std::vector <System::Module> &modules
	)
	{
		return false;
	}


	bool ConfigParser:: includeConfigFile(
		const std::string &filename,
		std::string &strBuf,
		const size_t offset
	)
	{
		return false;
	}

	bool ConfigParser:: addApplication( 
			const std::unordered_multimap <std::string, std::string> &app,
			const ServerApplicationDefaultSettings & defaults,
			std::vector<System::Module> &modules,
			ServerApplicationsTree & apps_tree
	)
	{
		return false;
	}
	
	
	bool ConfigParser:: parseName(
		const std::string &fileName,			
		std::unordered_map <std::string , std::string> &mimes_types
	)
	{
		return false;
	}

}

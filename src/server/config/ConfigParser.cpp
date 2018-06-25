#include "ConfigParser.h"

namespace HttpServer
{
	bool loadConfig(
			const std::string &conf,
			ServerSetting &settings,
			std::vector <System::Module> &modules
	)
	{
		return false;
	}


	bool ConfigParser:: includeConfigFile(
		const std::string temp_dir,
		std::string &strBuf,
		const size_t offset
	)
	{
		return false;
	}

	bool addApplication( 
			const std::unordered_multimap <std::string, std::string> &app,
			const ServerApplicationDefaultSettings & defaults,
			std::vector<Sytem::Module> &modules,
			ServerApplicationTree & apps_tree
	)
	{
		return false;
	}
	
	
	bool parseName(
		const std::string &fileName,			
		std::unordered_map <std::string , std::string> &mimes_types
	)
	{
		return false;
	}

}

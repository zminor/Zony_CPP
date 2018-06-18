#include "server/Server.h"
#include <iostream>
#include <unordered_map>
#include <functional>

int main(const int argc,const char* argv[])
{
	//initialize Command list
	const std::unordered_map <std::string, std::function<int(HttpServer::Server*, const int, const char*[])>> commands = 
	{
		{"--help",std::mem_fn(&HttpServer::Server::command_help)},
		{"--start", std::mem_fn(&HttpServer::Server::command_start)},
		{"--restart", std::mem_fn(&HttpServer::Server::command_restart)},
		{"--kill", std::mem_fn(&HttpServer::Server::command_terminate)},
		{"--update-module", std::mem_fn(&HttpServer::Server::command_update_module)},
	};
	//initialize ExitCode
	int exitcode = EXIT_FAILURE;
	
	if( 1 < argc )
	{
		auto const command = commands.find(argv[1]);	

		if(commands.cend() != command)
		{
			HttpServer::Server server;
			exitcode = command-> second(&server,argc,argv);		
		}
		else 
		{
			std::cout<<"Unknown parameter, please checkout --help"<<std::endl;
		}
	}	
	else if( 1 == argc )
	{
		std::cout << "Try to run with parameter --help"<<std::endl;
	}
	else
	{
		std::cout << "The number of arguments cannot be zero,Enter parameter --help"<<std::endl;	
	}

	return exitcode;
}

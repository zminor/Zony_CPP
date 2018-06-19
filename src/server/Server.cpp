#include "Server.h"
#include "ServerStructuresArguments.h"


namespace HttpServer
{

int Server::run()
{
		return 0;
}
void Server::stop()
{

}

void Server:: restart()
{

}

void Server::update()
{

}
bool Server::init()
{
	return true;
}

int Server:: command_help(const int argc, const char*argv[]) const
{
  std::cout<<"help"<<std::endl;
	return 0;
}

int Server::command_start(const int argc, const char*argv[])
{
	struct server_start_args st = {};

	if(Server::get_start_args(argc,argv,&st) == false)
	{
		return 0x1;
	}

	std::cout<<"start"<<std::endl;
	return 0;
}

int Server::command_restart(const int argc, const char*argv[]) const
{
	std::cout << "restart"<<std::endl;
	return 0;
}

int Server::command_terminate(const int argc, const char*argv[]) const
{
	std::cout<<"terminate"<< std::endl;
	return 0;
}

int Server::command_update_module(const int argc, const char*argv[]) const
{
	std::cout<< "update module:"<< std::endl;
	return 0;
}

bool Server::get_start_args(
				const int argc,
				const char*argv[],
				struct server_start_args * st) 
{
	return false;
}
}

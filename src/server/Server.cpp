#include "Server.h"
#include "ServerStructuresArguments.h"
#include "../SignalHandler.h"

namespace HttpServer
{

static std::string get_server_name(const int argc, const char* argv[])
{
	std::string server_name;

	for(int i=0; i< argc; ++i)
	{
		if(argv[i] == ::strstr(argv[i], "--server-name=") )
		{
			//Server name is typed in as parameter
			server_name = std::string(argv[i] + sizeof("--server-name=")-1);
			break;
		}
	}
		if(server_name.empty())
		{
			//Take first program name as Server_name
			server_name = argv[0];
		}

		System::filterSharedMemoryName(server_name);
		return server_name;
	
}

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
System::native_processid_type Server::getServerProcessId(const std::string &serverName)
{
	System::native_processid_type pid =0;
	
	System::GlobalMutex glob_mtx;

	if(glob_mtx.open(serverName))
	{
		
	}
	/* to Add*/
	return pid;
}

int Server:: command_help(const int argc, const char*argv[]) const
{
  std::cout<<"fetching help info..."<<std::endl;

	std::cout << std::left << "Available arguments:" << std::endl
			<< std::setw(4) << ' ' << std::setw(26) << "--start" << "Start http server" << std::endl
			<< std::setw(8) << ' ' << std::setw(22) << "[options]" << std::endl
			<< std::setw(8) << ' ' << std::setw(22) << "--force" << "Forcibly start http server (ignore existing instance)" << std::endl
			<< std::setw(8) << ' ' << std::setw(22) << "--config-path=<path>" << "Path to directory with configuration files" << std::endl
			<< std::endl
			<< std::setw(4) << ' ' << std::setw(26) << "--restart" << "Restart http server" << std::endl
			<< std::setw(4) << ' ' << std::setw(26) << "--update-module" << "Update applications modules" << std::endl
			<< std::setw(4) << ' ' << std::setw(26) << "--kill" << "Shutdown http server" << std::endl
			<< std::setw(4) << ' ' << std::setw(26) << "--help" << "This help" << std::endl
			<< std::endl<< "Optional arguments:" << std::endl
			<< std::setw(4) << ' ' << std::setw(26) << "--server-name=<name>" << "Name of server instance" << std::endl;

		return EXIT_SUCCESS;
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
	std::cout << "restart..."<<std::endl;

	const System::native_processid_type pid = Server::getServerProcessId(get_server_name(argc,argv));
	if( 1< pid && System::sendSignal(pid,SIGUSR1))
	{
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

int Server::command_terminate(const int argc, const char*argv[]) const
{
	std::cout<<"Terminate..."<< std::endl;

	const System::native_processid_type pid = Server::getServerProcessId(get_server_name(argc,argv));
	if( 1<pid && System::sendSignal(pid, SIGTERM))   //Pid=1: Init process
	{
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

int Server::command_update_module(const int argc, const char*argv[]) const
{
	std::cout<< "update module..."<< std::endl;
	const System::native_processid_type pid = Server::getServerProcessId(get_server_name(argc,argv));
	if( 1< pid && System::sendSignal(pid,SIGUSR2))
	{
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

bool Server::get_start_args(
				const int argc,
				const char*argv[],
				struct server_start_args * st) 
{
	return false;
}

}

#include "Server.h"
#include "ServerStructuresArguments.h"
#include "../SignalHandler.h"

namespace HttpServer
{

	//---------------------Port------------------//
	bool Server::tryBindPort(
			const int port,
			std::unordered_set <int> & ports
			)	
	{
		return false;	
	}

	void Server::initAppsPorts()
	{
	}
	//---------------------Main Func-------------------//
	int Server::run()
	{
			std::cout<<"running..." << std::endl;
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
	//------------------------Other-----------------------//
	System::native_processid_type Server::getServerProcessId(const std::string &serverName)
	{
		System::native_processid_type pid =0;
	
		System::GlobalMutex glob_mtx;

		if(glob_mtx.open(serverName))
		{
			System::SharedMemory glob_mem;

			glob_mtx.lock();

			if(glob_mem.open(serverName))	//Set shm_desc with serverName
			{
				glob_mem.read(&pid, sizeof(pid)); //??? 
			}
			glob_mtx.unlock();
		}

		return pid;
	}

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

	bool Server::get_start_args(
				const int argc,
				const char*argv[],
				struct server_start_args * st) 
	{
		for(int i =1; i< argc; ++i)
		{
			if( 0 == ::strcmp(argv[i], "--start"))
			{
			}
			else if(0 == ::strcmp(argv[i], "--force"))
			{
					st->force = true;	
			}
			else if(argv[i] == ::strstr(argv[i], "--config-path=")) //Config path,following '--config-path='
			{	
				st->config_path = std::string(argv[i] + sizeof("--config-path=") -1);
			}
			else if(argv[i] == ::strstr(argv[i], "--server-name"))	//Server name
			{
				st->server_name = std::string(argv[i] + sizeof("--server-name=")-1);
			}
			else 
			{
				std::cout<< "Argument '" << argv[i] << "' can`t be applied with --start;" << std::endl;
				return false;
			}

			if(st -> server_name.empty())
			{
				st -> server_name = argv[0];
			}
			System::filterSharedMemoryName(st->server_name);	
		}
			return true;
	}
	//----------------------Command_line----------------------------//

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
		std::cout<<"starting..." << std::endl;

		struct server_start_args st = {};
		//Check Start Command 
		if(Server::get_start_args(argc,argv,&st) == false)
		{
			return 0x1;
		}
		//Change Current Directory
		if(st.config_path.empty() == false )		
		{
			if(System::changeCurrentDirectory() == false)
			{
				std::cout << "Configuration path "<< st.config_path<<" not found ..." << std::endl;
				return 0x2;
			}
		}	

		//If force, destroy anything exists and start
		if(st.force)
		{
			System::SharedMemory :: destroy (st.server_name);
			System::GlobalMutex :: destroy (st.server_name); 
		}
		//Check is already on running
		bool is_exists = false;

		if(glob_mtx.open(st.server_name))
		{
			glob_mtx.lock();

			if(glob_mem.open(st.server_name))
			{
				System::native_processid_type pid = 0;
				if(glob_mem.read( &pid, sizeof(pid)))
				{
					is_exists = System::isProcessExists(pid);
				}
			}
			glob_mtx.unlock();
		}

		if(is_exists) 
		{
			std::cout<< "Server Instance with name " << st.server_name << " Already running"<<std::endl;
			return 0x3'
		}
		//Creating GlobalMutex
		if(glob_mtx.open(st.server_name) == false)
		{
			if(glob_mtx.create(st.server_name) == false)
			{
				std::cout<<"Global mutex could not be created..."<<std::endl;
				return 0x4;
			}
		}
		//Creating SharedMemory
		glob_mtx.lock();

		if(glob_mem.open(std.server_name) == false)
		{
			if(glob_mem.create(st.server_name) == false)
			{
				glob_mtx.unlock();
				std::cout<<"Shared memory could not be allocated..."<<std::endl;
				return 0x5;
			}
		}
		//Store PID at front of sharedMemory
		System::native_processid_type pid = System::getProcessId();

		if(glob_mem.write(&pid, sizeof(pid)) == false)
		{
			glob_mem.destroy();
			glob_mtx.unlock();
			std::cout<<"Writing data to shared memory failed..."<<std::endl;
			return 0x6;
		}
		glob_mtx.unlock();
		//Start running
		int exitcode = EXIT_FAILTURE;

		do{
			this->controls.setProcess(false);
			this->controls.setRestart(false);
			
			exitcode = this->run();
		}
		while(this->controls.process_flag || this->controls.restart_flag);
		//Clear
		glob_mem.destroy();
		glob_mtx.destroy();

		return exitcode;
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


}

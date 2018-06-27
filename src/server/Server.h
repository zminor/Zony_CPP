#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include "SocketsQueue.h"

#include "../system/System.h"
#include "../system/GlobalMutex.h"
#include "../system/SharedMemory.h"
#include "ServerStructuresArguments.h"
#include "config/ConfigParser.h"
#include "ServerControls.h"
#include "../socket/Socket.h"
#include "ServerSettings.h"

#include <string.h>
#include <iomanip>
#include <unordered_set>
#include <vector>


namespace HttpServer
{
	class Server
	{
	public:
		Server() = default;
		void stop();
		void restart();
		void update();

		int command_help(const int argc, const char*argv[]) const;
		int command_start(const int argc, const char*argv[]);
		int command_restart(const int argc, const char*argv[]) const;
		int command_terminate(const int argc, const char*argv[]) const;
		int command_update_module(const int argc, const char*argv[]) const;

	public:
		mutable ServerControls controls;

	protected:
		int cycleQueue(SocketsQueue &sockets);
		ServerSettings settings;		
		bool tryBindPort(
				const int port,
				std::unordered_set<int> &ports
		);
		
		void initAppsPorts();

		int run();
		bool init();
		void clear();

		static System::native_processid_type getServerProcessId(const std::string &serverName);

	private:
		static bool get_start_args(
				const int,
				const char*[],
				struct server_start_args*
		);
	protected:
		std::vector<Socket::Socket> listeners;
		std::vector<System::Module> modules;
		System::CachePadding<std::atomic_size_t> padding_1;
	};
}
#endif

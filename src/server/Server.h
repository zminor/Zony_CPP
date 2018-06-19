#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include "../system/System.h"
#include <string.h>

namespace HttpServer
{
	class Server
	{
	protected:
		int run();
		bool init();

	public:

	protected:
	static System::native_processid_type getServerProcessId(const std::string &serverName);
	
	private:
		static bool get_start_args(
				const int,
				const char*[],
				struct server_start_args*
		);
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

	};
}
#endif

#include "System.h"
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <csignal>

#define MAX_PATH 4096
namespace System{

native_processid_type getProcessId() noexcept
{
	return ::getpid();
}

bool sendSignal( const native_processid_type pid, const int signal) noexcept
{
	return 0==::kill(pid,signal);
}

void filterSharedMemoryName( std::string & memName)
{
	//Insert '/' at front of memName
	if(memName.front() != '/')
	{
		memName = '/' + memName;
	}
	//Replace '/' or '\\' with '-'
	for(size_t i = 1; i< memName.length(); ++i )
	{
		if('/' == memName[i] || '\\' == memName[i])
		{
			memName[i] = '-';
		}
	}
}
}

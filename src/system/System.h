#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <sys/types.h>
#include <string>

namespace System
{
	typedef ::pid_t native_processid_type;  //Process Id

	native_processid_type getProcessId() noexcept;

	bool sendSignal( const native_processid_type pid, const int signal) noexcept;

	void filterSharedMemoryName( std::string &memName);
}



#endif

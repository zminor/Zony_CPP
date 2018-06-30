#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <sys/types.h>

#include <string>
#include <ctime>
#include <thread>

namespace System
{
	typedef int native_socket_type;
	
	typedef ::pid_t native_processid_type;
	
	native_processid_type getProcessId() noexcept;
	
	bool changeCurrentDirectory(const std::string &dir);
	
	bool isProcessExists(const native_processid_type pid) noexcept;
	
	bool sendSignal(const native_processid_type pid, const int signal) noexcept;
	
	bool isDoneThread(const std::thread::native_handle_type handle) noexcept;
	
	std::string getTempDir();
	
	bool isFileExists(const std::string &fileName);
	
	bool getFileSizeAndTimeGmt(const std::string &filePath, size_t *fileSize, time_t *fileTime);
	
	void filterSharedMemoryName(std::string &memName);

}



#endif

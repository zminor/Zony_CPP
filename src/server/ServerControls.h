#ifndef __SERVERCONTROLS_H__
#define __SERVERCONTROLS_H__

#include "../util/Event.h"
#include "../system/Cache.h"

#include <atomic>
#include <csignal>

namespace HttpServer
{
	class ServerControls
	{
		public:
		ServerControls();
		~ServerControls();
		
		void clear();

		void setProcess(const bool flag = true);
		void stopProcess();
		void setRestart(const bool flag = true);
		void setUpdateModule();
		void setProcessQueue();

		public:
		Utils::Event * eventProcessQueue;
		Utils::Event * eventNotFullQueue;
		Utils::Event * eventUpdateModule;

		System::CachePaddingSize<sizeof(void *) * 3> padding_1;

		sig_atomic_t process_flag;
		sig_atomic_t restart_flag;
	};

}


#endif

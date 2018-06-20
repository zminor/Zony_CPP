#ifndef __GLOBAL_MUTEX_H__
#define __GLOBAL_MUTEX_H__

#include <semaphore.h>

#include <string.h>

namespace System
{
	class GlobalMutex
	{
	public:
		GlobalMutex() noexcept;
		~GlobalMutex() noexcept;

		bool create(const std::string &mutexName);
		bool destroy();
		
		static bool destroy( const std::string &mutexName );

		bool open(const std::string &mutexName);
		bool close() noexcept;

		bool is_open() const noexcept;

		bool lock() const noexcept;
		bool try_lock() const noexcept;
		bool unlock() const noexcept;

	private:
		::sem_t *mtx_desc;
		std::string mtx_name;
	};

}

#endif

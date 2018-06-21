#include "SharedMemory.h"
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstring>

namespace System{
	SharedMemory::SharedMemory() noexcept :shm_desc(-1)
	{
	}
	SharedMemory::~SharedMemory() noexcept
	{
		this->close();
	}
	bool SharedMemory ::create(
			const std::string &memName,
			const size_t memSize
			)
	{
		this->close();
		this->shm_desc = ::shm_open(
				memName.c_str(),
				O_CREAT | O_RDWR,
				0666
	);
		if( -1 == this->shm_desc)
		{
			return false;
		}

		if(::ftruncate64( this->shm_desc, long(memSize) == -1))
		{
			this->destroy(memName);
			return false;
		}

		this->shm_name = memName;
		return true;
	}

	bool SharedMemory:: open(const std::string &memName)
	{
		this->close();

		this->shm_desc = ::shm_open(
				memName.c_str(),
				O_RDWR,
				0666
		);

		if( -1 == this->shm_desc)
		{
			return false;
		}

		this->shm_name = memName;
		return true;
	}

	bool SharedMemory:: is_open() const noexcept
	{
		return -1 != this->shm_desc;
	}	

	bool SharedMemory:: write(
				const void *src,
				const size_t size,
				const size_t offset
			)const noexcept
	{
		void * const addr = ::mmap(
				nullptr,
				size,
				PROT_WRITE,
				MAP_SHARED,
				this->shm_desc,
				long(offset)
		);	
		if(reinterpret_cast<const void *> (-1) == addr)
		{
			return false;
		}
		std::memcpy(addr, src, size);
		::munmap(addr,size);
		return true;
	}
		
	 bool SharedMemory:: read(
				void *dest,
				const size_t size,
				const size_t offset
			)const noexcept
	{
		void * const addr = ::mmap(
					nullptr,
					size,
					PROT_READ,
					MAP_SHARED,
					this->shm_desc,
					long(offset)
		);		

		if(reinterpret_cast<const void *>(-1) == addr )
		{
			return false;
		}

		std::memcpy( dest, addr, size);
		::munmap( addr, size);
		return true;
	}	

		
	bool SharedMemory::close() noexcept
	{
		if( this-> is_open())
		{
			::close(this->shm_desc);
			this->shm_desc = ~0;
			this->shm_name.clear();

			return true;
		}
		return false;
	}

	bool SharedMemory:: destroy()
	{
		const int ret = ::shm_unlink(
				this->shm_name.c_str()
		);

		this->close();
		return 0 == ret;
	}

 	bool SharedMemory::destroy(const std::string &memName)
	{
		return 0 == ::shm_unlink( memName.c_str() );
	}
}

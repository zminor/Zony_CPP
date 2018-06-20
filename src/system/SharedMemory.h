#ifndef __SHAREDMEMORY_H__
#define __SHAREDMEMORY_H__

#include <string>

namespace System
{
	class SharedMemory
	{
	public:
		SharedMemory() noexcept;
		~SharedMemory() noexcept;

		bool create(
			const std::string &memName,
			const size_t memSize
			);

		bool open(const std::string &memName);

		bool is_open() const noexcept;

		bool write(
			const void *data,
			const size_t size,
			const size_t offset =0
		)const noexcept;
		
		bool read(
			void *dest,
			const size_t size,
			const size_t offset = 0
		)const noexcept;
		
		bool close() noexcept;		
		bool destroy();

		static bool destroy(const std::string &memName);

	private:
		int shm_desc;
		std::string shm_name;
	};
}

#endif

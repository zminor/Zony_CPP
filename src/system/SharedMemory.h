#ifndef __SHAREDMEMORY_H__
#define __SHAREDMEMORY_H__

<<<<<<< HEAD
#include <string>
=======
#include <string.h>
>>>>>>> master
namespace System
{
	class SharedMemory
	{
	public:
		SharedMemory() noexcept;
		~SharedMemory() noexcept;

		bool create(
<<<<<<< HEAD
			const std::string &memName,
=======
			const std::string &memName
>>>>>>> master
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
<<<<<<< HEAD
			void *dest,
			const size_t size,
=======
			void *dest;
			const size_t size;
>>>>>>> master
			const size_t offset = 0
		)const noexcept;
		
		bool close() noexcept;		
		bool destroy();

<<<<<<< HEAD
		static bool destroy(const std::string &memName);
=======
		static bool destroy(const std::string &memName;
>>>>>>> master

	private:
		int shm_desc;
		std::string shm_name;
	};
}

#endif

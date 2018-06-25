#ifndef __MODULE_H__
#define __MODULE_H__

#include <string>

namespace System
{

	class Module
	{
		public:
			Module() noexcept;
			Module(const std::string &libPath);
			Module(const Module &obj) noexcept;
			Module(Module &&obj) noexcept;

			~Module() noexcept = default;

			bool is_open() const noexcept;

			bool open(const std::string &libPath);
			void close() noexcept;

			bool find(
				const std::string &symbolName,
				void *(**addr)(void *)
			) const noexcept;

			bool find(
				const char *symbolName,
				void *(**addr)(void *)
			) const noexcept;

			bool operator ==(const Module &obj) const noexcept;
			bool operator !=(const Module &obj) const noexcept;

			Module &operator =(const Module &obj) noexcept;
			Module &operator =(Module &&obj) noexcept;
			
		protected:

			void *lib_handle;	
	};
}
#endif

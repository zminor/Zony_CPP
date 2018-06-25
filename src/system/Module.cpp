#include "Module.h"
#include <dlfcn.h>
#include <iostream>

namespace System
{
	Module::Module() noexcept
	{
	}

	Module::Module(const std::string &libPath)
	{
	
	}

	Module::Module(const Module &obj) noexcept
	{
	
	}

	Module::Module(Module &&obj) noexcept
	{
	
	}

	bool Module::is_open() const noexcept 
	{
		return false;
	}

	bool Module::open(const std::string &libPath)
	{
		return false;
	}

	void Module::close() noexcept
	{
	}

	bool Module::find(
		const std::string &symbolName,
		void *(**addr)(void *)
	) const noexcept 
	{
		return false;
	}

	bool Module::find(
		const char *symbolName,
		void *(**addr)(void *)
	) const noexcept 
	{
		return false;
	}

	bool Module::operator ==(const Module &obj) const noexcept 
	{
		return false;
	}

	bool Module::operator !=(const Module &obj) const noexcept 
	{
		return false;
	}

	Module &Module::operator =(const Module &obj) noexcept
	{
		return *this;	
	}

	Module &Module::operator =(Module &&obj) noexcept
	{
		return *this;
	}

}

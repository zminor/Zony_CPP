#include "GlobalMutex.h"

namespace System{

GlobalMutex::GlobalMutex() noexcept
				: mtx_desc(nullptr)
{

}

GlobalMutex::~GlobalMutex() noexcept
{
	this->close();
}

bool GlobalMutex::create(const std::string &mutexName)
{
	this->close();
	sem_t * sem = ::sem_open(
						mutexName.c_str(),
						O_CREAT,
						0666,
						1
						);
	if(sem == SEM_FAILED)
	{
		return false;
	}

	this->mtx_desc = sem;
	this->mtx_name = mutexName;
	return true;
}

bool GlobalMutex::destroy()
{
	const int ret = ::sem_unlink( 
			this -> mtx_name.c_str()
			);
	this->close();

	return 0 ==ret;
}

bool GlobalMutex:: destroy(const std::string &mutexName)
{
	return 0 == ::sem_unlink(mutexName.c_str());
}

bool GlobalMutex:: open(const std::string &mutexName)
{
	this->close();
	::sem_t * sem = sem_open(
							mutexName.c_str(),
							0
							);
	if(sem == SEM_FAILED)
	{
		return false;
	}
	this->mtx_desc = sem;
	this->mtx_name = mutexName;
	return true;
}

bool GlobalMutex::close() noexcept
{
	if(this->mtx_desc)
	{
		::sem_close( this-> mtx_desc);
		this->mtx_desc = nullptr;
		this->mtx_name.clear();
		return true;
	}
	return false;
}

bool GlobalMutex::is_open() const noexcept
{
	return nullptr != this->mtx_desc;
}

bool GlobalMutex:: lock() const noexcept
{
	return 0 == ::sem_wait(this->mtx_desc);
}

bool GlobalMutex:: try_lock() const noexcept
{
	return 0 == ::sem_trywait(this->mtx_desc);
}
bool GlobalMutex:: unlock() const noexcept
{
	return 0 == ::sem_post(this->mtx_desc);
}

}


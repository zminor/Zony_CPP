#include "List.h"

namespace Socket{
		List:: List() noexcept : obj_list(~0)
		{
		}
		List:: List(List &&obj) noexcept : obj_list(obj.obj_list)
		{
			obj.obj_list = ~0;
			obj.epoll_events.swap(this->epoll_events);
		}
		List:: ~List() noexcept
		{
				this->destroy();
		}
		
		bool List:: create(const size_t startListSize )
		{
			return false;
		}
		void List:: destroy() noexcept
		{
			if(this->is_created)
			{
				::close(this->obj_list);
				this -> obj_list = ~0;
				this ->	epoll_events.clear();
			}
		}

		bool List:: is_created()const noexcept
		{
			return false;
		}

		bool List:: addSocket(const Socket &sock) noexcept
		{
			return false;
		}
		bool List:: removeSocket(const Socket &sock) noexcept
		{
			return false;
		}

		bool List:: accept( std::vector<Socket> &sockets) const noexcept
		{
			return false;
		}

		bool List:: accept(
					std::vector <Socket> &sockets,
					std::vector <struct sockaddr_in> &socketsAddress
					)const noexcept
		{
			return false;
		}

		bool List:: recv(
					std::vector<Socket> &sockets,
					std::vector<Socket> &errors,
					std::chrono::milliseconds timeout = std::chrono::milliseconds(~0)
					)	const noexcept
		{
			return false;			
		}
}

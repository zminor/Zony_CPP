#include "List.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

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
			this->destroy();
			this->obj_list = ::epoll_create(startListSize);

			if(obj_list == ~0) 
			{
				return false;
			}

			if(startListSize >0)
			{
				epoll_events.reserve(startListSize);
			}
			return false;
		}

		void List:: destroy() noexcept
		{
			if(this->is_created())
			{
				::close(this->obj_list);
				this -> obj_list = ~0;
				this ->	epoll_events.clear();
			}
		}

		bool List:: is_created()const noexcept
		{
			return obj_list != ~0;
		}

		bool List:: addSocket(const Socket &sock) noexcept
		{
			if(this->is_created() == false)
			{
				return false;
			}
			struct ::epoll_event event{
						EPOLLIN | EPOLLET | EPOLLRDHUP,
						reinterpret_cast <void*> (sock.get_handle())
			};

			const int result = ::epoll_ctl(
							this->obj_list,
							EPOLL_CTL_ADD,
							sock.get_handle(),
							&event
			);

			if(result == ~0)
			{
				return false;
			}

			this->epoll_events.emplace_back(event);
			
			return true;
		}
		bool List:: removeSocket(const Socket &sock) noexcept
		{
			if(this->is_created() == false) 
			{
					return false;
			}

			const int result = ::epoll_ctl(
					this->obj_list,
					EPOLL_CTL_DEL,
					sock.get_handle(),
					nullptr
			);

			if(result == ~0)
			{
				return false;
			}
			this->epoll_events.pop_back();
			return true;
		}

		bool List:: accept( std::vector<Socket> &sockets) const noexcept
		{
			if(this -> is_created())
			{
					const int count = ::epoll_wait(
										this->obj_list,
										this->epoll_events.data(),
										this->epoll_events.size(),
										~0
					);

					if(count == ~0)
					{
						return false;
					}

					for(int i=0; i< count; ++i)
					{
						const epoll_event & event = this->epoll_events[i];

						if(event.events & EPOLLIN)
						{
							System::native_socket_type client_socket = ~0;
							do
							{
								client_socket = ::accept(
																event.data.fd,
																static_cast <sockaddr *> (nullptr),
																static_cast <socklen_t *>(nullptr)
								);
								
								if(~0 != client_socket)
								{
									sockets.emplace_back(Socket(client_socket));
								}
							}
							while( ~0 != client_socket);
						}
					}
				return false == sockets.empty();
			}
			return false;
		}

		bool List:: accept(
					std::vector <Socket> &sockets,
					std::vector <struct sockaddr_in> &socketsAddress
					)const noexcept
		{
			if(this->is_created())
			{
				const int count = ::epoll_wait(
												this->obj_list,
												this->epoll_events.data(),
												this->epoll_events.size(),
												~0
												);
				if(count ==~0)
				{
					return false;
				}

				for(int i=0; i< count; ++i)
				{
					const epoll_event &event = this->epoll_events[i];
					if(event.events & EPOLLIN)
					{
						System::native_socket_type client_socket = ~0;

						do{
							::sockaddr_in client_addr{};
							socklen_t client_addr_len = sizeof(client_addr);

							client_socket = ::accept(
												event.data.fd,
												reinterpret_cast <::sockaddr*> (&client_addr),
												&client_addr_len
															);
							if(~0 != client_socket)
							{
									sockets.emplace_back(Socket(client_socket));
									socketsAddress.emplace_back(client_addr);
							}
						}
						while( ~0 != client_socket);
					}
				}
				return false == sockets.empty();
			}
			return false;
		}

		bool List:: recv(
					std::vector<Socket> &sockets,
					std::vector<Socket> &disconnected,
					std::chrono::milliseconds timeout 
					)	const noexcept
		{
						if(this-> is_created() == false)
						{
							return false;
						}

						const int count = ::epoll_wait(
							this->obj_list,
							this->epoll_events.data(),
							this->epoll_events.size(),
							timeout.count()
						);

						if(count == ~0)
						{
							return false;
						}

						for(int i=0; i< count; ++i)
						{
							const epoll_event & event = this->epoll_events[i];

							if(event.events & EPOLLIN)
							{
								sockets.emplace_back(Socket(event.data.fd));
							}
							else if(event.events & EPOLLRDHUP)
							{
								disconnected.emplace_back(Socket(event.data.fd));
							}
						}

			return false== sockets.empty() || false == disconnected.empty();			
		}
}

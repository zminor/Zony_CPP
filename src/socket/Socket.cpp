#include "Socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>

namespace Socket
{
		//---------------------Static---------------------//
		bool Socket:: Startup() noexcept
		{
			return true;
		}
		bool Socket:: Cleanup() noexcept
		{
			return true;
		}
		int Socket:: getLastError() noexcept
		{
			return errno;
		}
		//-------------------Constructor-------------------//
		Socket:: Socket() noexcept: socket_handle(~0){}

		Socket:: Socket(const System::native_socket_type fd) noexcept: socket_handle(fd){}
			
		Socket:: Socket(const Socket &obj) noexcept:socket_handle(obj.socket_handle){}

		Socket:: Socket(Socket && obj) noexcept : socket_handle(obj.socket_handle)
		{
			obj.socket_handle = ~0;
		}

		//---------------Set & Get -------------//
		bool Socket::open() noexcept
		{
			this->close();
			this->socket_handle = ::socket(AF_INET,SOCK_STREAM,0);
			return this->is_open();
		}

		bool Socket::close() noexcept
		{
			if(is_open())
			{
				const int ret = ::close(this->socket_handle);
				if(ret == 0)
				{
					this->socket_handle = ~0;
					return true;
				}
			}
			return false;
		}

		bool Socket::is_open() const noexcept
		{
			return this->socket_handle != ~0;
		}
		System::native_socket_type Socket::get_handle() const noexcept
		{
			return this->socket_handle;
		}

		bool Socket::bind(const int port) const noexcept
		{
			const ::sockaddr_in sock_addr{
				AF_INET,
				::htons(port),
				::htonl(INADDR_ANY).
				0
			};

			return 0 == ::bind(
					this->socket_handle,
					reinterpret_cast <const sockaddr *> (&sock_addr),
					sizeof(sockadd_in)
			);
		}

		bool Socket::listen() const noexcept
		{
				return  0 == ::listen(this->socket_handle, SOMAXCONN);
		}

		Socket::accept() const noexcept
		{
			System::native_socket_type client_socket = ::accept(
				this->socket_handle,
				static_cast <sockaddr *> (nullptr),
				static_cast <socklen_t *> (nullptr)
			);
			return Socket(client_socket);
		}
		Socket nonblock_accept() const noexcept;

		Socket nonblock_accept(
			const std::chrono::milliseconds &timeout
		) const noexcept;
		
		bool Socket::shutdown() const noexcept
		{
			if(is_open())
			{
				return 0 == ::shutdown(this->socket_handle,SHUT_RDWR	);
			}
			return false;
		}

		bool Socket::nonblock(const bool isNonBlock) const noexcept
		{
			return ~0 == fcntl(
					this->socket_handle,
					F_SETFL,
					isNonBlock
					? O_NONBLOCK
					: O_SYNC
			);
		}
		//bool i_nonblock() const noexcept;
		bool Socket:: tcp_nodelay(const bool nodelay) const noexcept
		{
			int flags = nodelay? 1:0;
			//Nagle Algorithm on/off
			return 0 == ::setsockopt(
					this->socket_handle,
					IPPROTO_TCP,
					TCP_NODELAY,
					&flags,
					sizeof(flags)
			);
		}
		//-------------Receive--------------//
		long Socket::recv(
			std::vector<std::string :: value_type>&buf
			) const noexcept
		{
			rreturn this->recv(buf.data(), buf.size());
		}

		long Socket::recv(
			void *buf, const size_t length
		) const noexcept
		{
			return ::recv(this->socket_handle, buf,length,0);
		}

		long Socket::nonblock_recv(
			std::vector<std::string ::value_type> &buf,
			const std::chrono::milliseconds &timeout
		) const noexcept
		{
			return this->nonblock_recv(
					buf.data(),
					buf.size(),
					timeout
			);
		}

		long Socket::nonblock_recv(
			void *buf,
			const size_t length,
			const std::chrono::milliseconds &timeout
		) const noexcept
		{
			long recv_len = ~0;
			struct ::pollfd event{
					this->socket_handle,
					POLLIN,
					0
			};

			if(1 == ::poll(&event, 1, int(timeout.count())) && event.revents & POLLIN)
			{
				recv_len = ::recv(this->socket_handle, buf,length,0)
			}
			return recv_len;
		}

		bool Socket::nonblock_recv_sync(
			const std::chrono::milliseconds & timeout
		) const noexcept
		{
			struct ::pollfd event{
					this->socket_handle,
					POLLIN,
					0
				};
			return ::poll(&event, 1, int(timeout.cout())) == 1;
		}

		//-----------Send----------------//
		long Socket:: send(const std::string & buf) const noexcept
		{
			return this->send( buf.data(), buf.length() );
		}

		long Socket::send(const void *buf, const size_t length) const noexcept
		{
			return send_all( this->socket_handle, buf, length);
		}
		
		static long send_all(
				const System::native_socket_type socket_handle,
				const void *data,
				const size_t length
		) noexcept
		{
			size_t total =0;

			while(total < length)
			{
				const long send_size = ::send(
						socket_handle,
						reinterpret_cast <const char *> (data) + total,
						length - total,
						0
				);
				if(send_size <0 )
				{
					return send_size;
				}
				total += size_t(send_size);
			}
			return static_cast<long> (total);
		}
	
		long Socket::nonblock_send(
			const std::string &buf,
			const std::chrono::milliseconds &timeout
		) const noexcept
		{
			return this->nonblock_send(
					buf.data(),
					buf.length(),
					timeout
			);
		}
		
		long Socket::nonblock_send(
			const void *buf,
			const size_t length,
			const std::chrono::milliseconds &timeout
		) const noexcept
		{
			return nonblock_send_all(
					this->socket_handle,
					buf,
					length,
					timeout
			);
		}

		void Socket::nonblock_send_sync() const noexcept
		{
			struct ::pollfd event{
					this->socket_handle,				//fd
					POLLOUT,										//request event
					0														//return event
			};
			::poll(&event, 1, ~0);			//event, nfds, timeout
		}
		//------------Operator----------------//
	  Socket ::Socket &operator = (const Socket &obj) noexcept
		{
			this-> socket_handle = obj.socket_handle;
			return *this;
		}

		bool operator == (const Socket &obj) const noexcept
		{
			return this->socket_handle == obj.socket_handle;
		}
		
		bool operator != (const Socket &obj) const noexcept
		{
			return this->socket_handle != obj.socket_handle;
		}
}

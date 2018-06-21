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
						/*to add*/
			return false;
		}
		bool Socket::listen() const noexcept
		{
						/*to add*/
			return false;
		}

/*		Socket::accept() const noexcept
		{
			retu
		}
		Socket nonblock_accept() const noexcept;

		Socket nonblock_accept(
			const std::chrono::milliseconds &timeout
		) const noexcept;
	*/	
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
						/*to add*/
			return false;
		}
		//bool i_nonblock() const noexcept;
		bool Socket:: tcp_nodelay(const bool nodelay) const noexcept
		{
			/**/
			return false;
		}
		//-------------Receive--------------//
		long Socket::recv(
			std::vector<std::string :: value_type>&buf
			) const noexcept
		{
			return 0;
		}

		long Socket::recv(
			void *buf, const size_t length
		) const noexcept
		{
			return 0;
		}

		long Socket::nonblock_recv(
			std::vector<std::string ::value_type> &buf,
			const std::chrono::milliseconds &timeout
		) const noexcept
		{
			return 0;
		}

		long Socket::nonblock_recv(
			void *buf,
			const size_t length,
			const std::chrono::milliseconds &timeout
		) const noexcept
		{
			return 0;
		}

		bool Socket::nonblock_recv_sync(
			const std::chrono::milliseconds & timeout
		) const noexcept
		{
			return false;
		}
		//-----------Send----------------//
		long Socket:: send(const std::string & buf) const noexcept
		{
			return 0;
		}
		long Socket::send(const void *buf, const size_t length) const noexcept
		{
			return 0;
		}
	
		long Socket::nonblock_send(
			const std::string &buf,
			const std::chrono::milliseconds &timeout
		) const noexcept
		{
			return 0;
		}

		void Socket::nonblock_send_sync() const noexcept
		{
			
		}
		//------------Operator----------------//
	/*	Socket ::Socket &operator = (const Socket &obj) noexcept;

		bool operator == (const Socket &obj) const noexcept;
		bool operator != (const Socket &obj) const noexcept;
		*/
}

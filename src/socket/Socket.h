#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "../system/System.h"
#include <vector>
#include <chrono>
#include <string>

namespace Socket
{
	class Socket
	{
	public:
		Socket() noexcept;
		Socket(const System::native_socket_type fd) noexcept;
		Socket(const Socket &obj) noexcept;
		Socket(Socket && obj) noexcept;

		~Socket() noexcept = default;
		//---------------Set & Get -------------//
		bool open() noexcept;
		bool close() noexcept;

		bool is_open() const noexcept;
		System::native_socket_type get_handle() const noexcept;

		bool bind(const int port) const noexcept;
		bool listen() const noexcept;

		Socket accept() const noexcept;
		Socket nonblock_accept() const noexcept;

		Socket nonblock_accept(
			const std::chrono::milliseconds &timeout
		) const noexcept;
		
		bool shutdown() const noexcept;

		bool nonblock(const bool isNonBlock = true) const noexcept;
		//bool i_nonblock() const noexcept;
		bool tcp_nodelay(const bool nodelay = true) const noexcept;
		//-------------Receive--------------//
		long recv(
			std::vector<std::string :: value_type>&buf
			) const noexcept;

		long recv(
			void *buf, const size_t length
		) const noexcept;

		long nonblock_recv(
			std::vector<std::string ::value_type> &buf,
			const std::chrono::milliseconds &timeout
		) const noexcept;

		long nonblock_recv(
			void *buf,
			const size_t length,
			const std::chrono::milliseconds &timeout
		) const noexcept;

		bool nonblock_recv_sync(
			const std::chrono::milliseconds & timeout
		) const noexcept;
		//-----------Send----------------//
		long send(const std::string & buf) const noexcept;
		long send(const void *buf, const size_t length) const noexcept;

		long nonblock_send(
			const std::string &buf,
			const std::chrono::milliseconds &timeout
		) const noexcept;

		void nonblock_send_sync() const noexcept;
		//------------Operator----------------//
		Socket &operator = (const Socket &obj) noexcept;

		bool operator == (const Socket &obj) const noexcept;
		bool operator != (const Socket &obj) const noexcept;
		protected:
		System::native_socket_type socket_handle;
	};
}
namespace std
{
	//Hash for Socket
	template<> struct hash<Socket::Socket>
	{
		std::size_t operator()(const Socket::Socket &obj) const noexcept
		{
			return std::hash<System::native_socket_type>{} (obj.get_handle() );
		}
	};
}
#endif

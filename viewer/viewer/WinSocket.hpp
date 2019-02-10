#ifndef _WINSOCKET_HPP_
#define _WINSOCKET_HPP_

#include <WinSock2.h>
#include <iostream>

class Socket
{
public:
	/*
	* 创建Socket
	* const int af:协议类型
	* const int type:传输类型
	* const int protocol:协议代码
	* 成功返回true
	* 失败返回false并抛出char * 异常
	*/
	Socket() :socket_(INVALID_SOCKET)
	{}

	Socket(const SOCKET& socket) :socket_(socket)
	{}

	bool Create(const int af, const int type, const int protocol)
	{
		socket_ = ::socket(af, type, protocol);
		if (socket_ == INVALID_SOCKET)
		{
			//throw "Create error!";
			std::cout << "Create error!\n";
			return false;
		}
		return true;
	}

	bool Bind(const int port, const char* ip = nullptr)
	{
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		if (nullptr != ip)
		{
			addr.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else
		{
			addr.sin_addr.S_un.S_addr = INADDR_ANY;
		}

		if (::bind(socket_, (sockaddr*)& addr, sizeof(addr)) == SOCKET_ERROR)
		{
			//throw "Bind error!";
			std::cout << "Bind error!\n";
			return false;
		}

		return true;
	}

	bool Listen(int backlog)
	{
		if (::listen(socket_, backlog) == SOCKET_ERROR)
		{
			//throw "Listen error!";
			std::cout << "Listen error!\n";

			return false;
		}

		return true;
	}

	bool Connect(const int port, const char* ip)
	{
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.S_un.S_addr = inet_addr(ip);

		while (::connect(socket_, (sockaddr*)& addr, sizeof(addr)) == SOCKET_ERROR)
		{
			return false;
		}

		return true;
	}

	SOCKET Accept(sockaddr * clientAddr, int addrLen)
	{
		return ::accept(socket_, clientAddr, &addrLen);
	}

	int Send(const char* buf, int size)
	{
		int ret = ::send(socket_, buf, size, 0);
		if (SOCKET_ERROR == ret)
		{
			//throw "Send error!";
			std::cout << "Send error!\n";
			return -1;
		}

		return ret;
	}

	int Recv(char *buf, int size)
	{
		int ret = ::recv(socket_, buf, size, 0);
		if (SOCKET_ERROR == ret)
		{
			//throw "Recv error!";
			std::cout << "Recv error!\n";

			return -1;
		}

		return ret;
	}

	void Close()
	{
		closesocket(socket_);
	}

	SOCKET getSocket()
	{
		return socket_;
	}

	void setSocket(SOCKET socket)
	{
		socket_ = socket;
	}
private:
	SOCKET socket_;
};

#endif
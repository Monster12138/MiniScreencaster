#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

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
	Socket():sockfd_(0)
	{}

	Socket(const int& socket) :sockfd_(socket)
	{}

	bool Create(const int af, const int type, const int protocol)
	{
		sockfd_ = ::socket(af, type, protocol);
		if (sockfd_ < 0)
        {
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
			addr.sin_addr.s_addr = inet_addr(ip);
		}
		else
		{
			addr.sin_addr.s_addr = INADDR_ANY;
		}

		if (::bind(sockfd_, (sockaddr*)& addr, sizeof(addr)) < 0)
		{
            std::cout << "Bind error!\n"; 
			return false;
		}

		return true;
	}

	bool Listen(int backlog)
	{
		if (::listen(sockfd_, backlog) < 0 )
		{
            std::cout << "Listen error!\n"; 
			return false;
		}

		return true;
	}

	bool Connect( const int port, const char* ip)
	{
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ip);

		while (::connect(sockfd_, (sockaddr*)& addr, sizeof(addr)) < 0)
		{

            std::cout << "Connect error!\n"; 
            return false;
		}
		return true;
	}

	int Accept(sockaddr * clientAddr, socklen_t &addrLen)
	{
		return ::accept(sockfd_, clientAddr, &addrLen);
	}

	int Send(void* buf, int size)
	{
		int ret = ::send(sockfd_, buf, size, 0);
        if(ret < 0)
            std::cout << "Send error!\n"; 
		return ret;
	}

	int Recv(void *buf, int size)
	{
		int ret = ::recv(sockfd_, buf, size, 0);

        if(ret < 0)
            std::cout << "Recv error!\n"; 
		return ret;
	}

	void Close()
	{
		close(sockfd_);
	}

	int getSocket()
	{
		return sockfd_;
	}

	void setSocket(int socket)
	{
		sockfd_ = socket;
	}
private:
    int sockfd_;
};

#endif

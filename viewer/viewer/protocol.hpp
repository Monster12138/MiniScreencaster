#pragma once
#include <WinSock2.h>
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

struct PackInfo
{
	long long id;
	long long length;
	bool isLast;
};

struct SendPack
{
	PackInfo head;
	char data[1024];
};

typedef SendPack RecvPack;
typedef std::vector<RecvPack> RP_vector;

class UDP
{
public:
	static int Create()
	{
		int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sockfd == -1)
		{
			std::cout << "Create error!\n";
			exit(1);
		}
		return sockfd;
	}

	static void Bind(int sockfd, const int port, const char* ip = nullptr)
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

		if (::bind(sockfd, (sockaddr*)& addr, sizeof(addr)) == SOCKET_ERROR)
		{
			//throw "Bind error!";
			std::cout << "Bind error!\n";
			exit(2);
		}
	}

	static void RecvVideo(int socket, const char *FileName, struct sockaddr_in& PeerAddr)
	{
		std::ofstream video(FileName, std::ios::out | std::ios::binary);
		if (!video.is_open())
		{
			exit(-1);
		}

		RP_vector rp_vec;
		RecvPack rp;
		int addr_len = sizeof(PeerAddr);
		int sum_size = 0;

		while (recvfrom(socket, (char *)&rp, sizeof(rp), 0, (struct sockaddr*)&PeerAddr, &addr_len))
		{
			rp_vec.push_back(rp);
			std::cout << "id:" << rp.head.id <<
				" length:" << rp.head.length <<
				" isLast: " << rp.head.isLast << std::endl;
			sum_size += rp.head.length;

			sendto(socket, (char*)&rp.head.id, sizeof(rp.head.id), 0, (struct sockaddr*)&PeerAddr, addr_len);
		}

		sort(rp_vec.begin(), rp_vec.end(),
			[](const RecvPack &rp1, const RecvPack &rp2) {return rp1.head.id < rp2.head.id; });

		for (const auto e : rp_vec)
		{
			std::cout << e.head.id << " ";
			video.write(e.data, e.head.length);
		}
		std::cout << std::endl;

		std::cout << "file size: " << sum_size << std::endl;
		video.close();
	}
};

class TCP
{
public:
	static int Create()
	{
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
		{
			std::cout << "Create error!\n";
			exit(1);
		}
		return sockfd;
	}

	static void Bind(int listen_sockfd, const int port, const char* ip = nullptr)
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

		if (::bind(listen_sockfd, (sockaddr*)& addr, sizeof(addr)) == SOCKET_ERROR)
		{
			std::cout << "Bind error!\n";
			exit(2);
		}
	}

	static void Listen(int listen_sockfd, int backlog)
	{
		if (::listen(listen_sockfd, backlog) < 0)
		{
			std::cout << "Listen error!\n";
			exit(3);
		}
	}

	static bool Connect(int sockfd, const int port, const char* ip)
	{
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.S_un.S_addr = inet_addr(ip);

		while (::connect(sockfd, (sockaddr*)& addr, sizeof(addr)) == SOCKET_ERROR)
		{
			return false;
		}

		return true;
	}

	static int Accept(int listen_sockfd, sockaddr * clientAddr, int& addrLen)
	{
		return ::accept(listen_sockfd, clientAddr, &addrLen);
	}

	static void SendVideo(int sockfd, const char *FileName)
	{
		std::ifstream video(FileName, std::ios::in | std::ios::binary);
		char buffer[10240] = { 0 };
		int file_size = 0;
		while (int len = video.read(buffer, 10240).gcount())
		{
			send(sockfd, buffer, len, 0);
			file_size += len;
		}
		send(sockfd, buffer, 0, 0);

		std::cout << "send file: " << file_size << std::endl;
	}

	static void RecvVideo(int sockfd, const char *FileName)
	{
		std::ofstream video(FileName, std::ios::out | std::ios::binary);
		char buffer[614400] = { 0 };
		int file_size = 0;
		int len;
		while ((len = recv(sockfd, buffer, 614400, 0)) > 0)
		{
			video.write(buffer, len);
			file_size += len;
			//std::cout << "recv " << len << "Byte datas\n";
		}

		std::cout << "recv file: " << file_size << std::endl;
	}
};
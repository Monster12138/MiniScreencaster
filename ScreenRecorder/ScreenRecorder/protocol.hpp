#pragma once
#include <WinSock2.h>
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

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

	static void SendVideo(int socket, const char *FileName,const struct sockaddr_in &PeerAddr)
	{
		std::ifstream video(FileName, std::ios::in | std::ios::binary);

		SendPack sp;
		sp.head.id = 0;
		
		timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		int file_size = 0, resent = 0;
		while (sp.head.length = video.read(sp.data, 1024).gcount())
		{
			++sp.head.id;
			if (sp.head.length < 1024)sp.head.isLast = true;
			else sp.head.isLast = false;

			RESENT:
			sendto(socket, 
				(char*)&sp, 
				sizeof(PackInfo) + sp.head.length, 
				0, 
				(struct sockaddr*)&PeerAddr, 
				sizeof(PeerAddr)
			);

			std::cout << "id:" << sp.head.id << 
				" length:" << sp.head.length << 
				" isLast: " << sp.head.isLast << std::endl;

			fd_set fs;
			int response_id = 0;
			int addr_len = sizeof(PeerAddr);

			FD_SET(socket, &fs);
			if(select(socket + 1, nullptr, &fs, nullptr, &tv))
				recvfrom(socket, (char *)&response_id, sizeof(response_id), 0, (struct sockaddr*)&PeerAddr, &addr_len);
			else 
			{
				++resent;
				goto RESENT;
			}
			
			file_size += sp.head.length;
		}

		sendto(socket,
			(char*)&sp,
			0,
			0,
			(struct sockaddr*)&PeerAddr,
			sizeof(PeerAddr));
		
		std::cout << "file size: " << file_size << std::endl;
		std::cout << "resent: " << resent << std::endl;
		video.close();
	}
};
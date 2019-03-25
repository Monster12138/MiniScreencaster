#pragma once
#include <WinSock2.h>
#include <stdlib.h>
#include <cstdio>
#include <iostream>
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
	static void SendVideo(int socket, const char *FileName, struct sockaddr_in PeerAddr)
	{
		FILE* fp = fopen(FileName, "r");

		SendPack sp;
		sp.head.id = 0;
		while (sp.head.length = fread(sp.data, 1, 1024, fp))
		{
			++sp.head.id;
			if (sp.head.length < 1024)sp.head.isLast = true;
			else sp.head.isLast = false;

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
		}

		sendto(socket,
			(char*)&sp,
			0,
			0,
			(struct sockaddr*)&PeerAddr,
			sizeof(PeerAddr));
		fclose(fp);
	}

	static void RecvVideo(int socket, const char *FileName, struct sockaddr_in PeerAddr)
	{
		FILE* fp = fopen(FileName, "r");
		
		RP_vector rp_vec;
		RecvPack rp;
		int addr_len = sizeof(PeerAddr);
		while (recvfrom(socket, (char *)&rp, sizeof(rp), 0, (struct sockaddr*)&PeerAddr, &addr_len))
		{
			rp_vec.push_back(rp);
		}

		sort(rp_vec.begin(), rp_vec.end(),
			[](const RecvPack &rp1, const RecvPack &rp2) {return rp1.head.id > rp2.head.id; });

		for (const auto e : rp_vec)
		{
			fwrite(e.data, 1, e.head.length, fp);
		}

		fclose(fp);
	}
};
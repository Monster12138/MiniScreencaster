#pragma once
#include "WinSocket.hpp"
#include "Screen.hpp"
#include "protocol.hpp"
#include <queue>
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <thread>
#include <time.h>
#include <sstream>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#pragma warning(disable : 4996)



using namespace cv;

typedef std::queue<std::string> SendQueue;

std::string int2str(int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

void getFileName(std::string &filename)
{
	filename = "cache\\videocache";
	time_t nowtime;
	time_t timestamp = time(&nowtime);
	tm localtm;
	localtime_s(&localtm, &timestamp);

	filename += int2str(localtm.tm_year + 1900) + "_" + 
		int2str(localtm.tm_mon + 1) + "_" +
		int2str(localtm.tm_mday) + "_" +
		int2str(localtm.tm_hour) + "_" +
		int2str(localtm.tm_min) + "_" +
		int2str(localtm.tm_sec) + ".mp4";
}

void PrintFourCC(VideoCapture &capture)
{
	unsigned int f = (unsigned)capture.get(cv::CAP_PROP_FOURCC);
	char fourcc[] = {
		(char)f, // First character is lowest bits
		(char)(f >> 8), // Next character is bits 8-15
		(char)(f >> 16), // Next character is bits 16-23
		(char)(f >> 24), // Last character is bits 24-31
		'\0' // and don't forget to terminate
	};
	std::cout << "FourCC for this video was: " << fourcc << std::endl;
}

int Producer(SendQueue &sq)
{
	int listen_sock = TCP::Create();
	TCP::Bind(listen_sock, 8777);
	TCP::Listen(listen_sock, 5);

	struct sockaddr_in peer_addr;
	int len = sizeof(peer_addr);
	int work_sock = TCP::Accept(listen_sock, (struct sockaddr*)&peer_addr, len);

	std::cout << "accept peer ip:" << peer_addr.sin_addr.S_un.S_addr << " port:" << ntohs(peer_addr.sin_port) << std::endl;

	char *buffer = new char[614400];
	while (1)
	{
		std::string filename;
		getFileName(filename);
		if (TCP::RecvVideo(work_sock, filename.c_str(), buffer) == 0)
			break;
	}

	return 0;
}

int Consumer(SendQueue &sq)
{
	int quit_flag = false;
	while (!quit_flag)
	{
		if (sq.empty())
		{
			std::cout << "recv queue empty...\n";
			Sleep(1000);
			continue;
		}
		std::string filename = sq.front();
		sq.pop();

		VideoCapture video(filename.c_str());
		if (!video.isOpened())
		{
			std::cout << "video open failed!\n";
			return -1;
		}

		Mat frame;

		while (1)
		{
			video >> frame;
			if (frame.empty())
				break;
			imshow("实时画面", frame);
			if (waitKey(33) == 27)
			{
				quit_flag = true;
				break;
			}
		}
	}

	return 0;
}

void Test()
{
	SendQueue sq;
	std::thread pth(Producer, ref(sq));
	std::thread cth(Consumer, ref(sq));
	cth.join();
	pth.join();
}


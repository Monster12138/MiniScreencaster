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
	VideoCapture capture(0);
	if (!capture.isOpened())
		return -1;

	double dWidth = capture.get(CAP_PROP_FRAME_WIDTH); //get the width of frames of the video  
	double dHeight = capture.get(CAP_PROP_FRAME_HEIGHT);

	char *pbuf = new char[10000000];
	std::string filename;
	while (1)
	{
		getFileName(filename);
		std::cout << "Screening:" << filename << "..." << std::endl;
		VideoWriter writer(filename.c_str(), MAKEFOURCC('D', 'I', 'V', 'X'), 15.0, Size(static_cast<int>(dWidth), static_cast<int>(dHeight)), true);
		//VideoWriter writer("VideoTest.avi", MAKEFOURCC('M', 'J', 'P', 'G'), 15.0, Size(dWidth, dHeight), true);
		if (!writer.isOpened())
		{
			std::cout << "writer open failed!\n";
			return -1;
		}
		Mat frame;
		int frameNum = 100;
		while (frameNum--) {
			capture >> frame;
			imshow("读取视频", frame);
			writer << frame;
			if (waitKey(33) == 27)
				break;//给图像绘制留点时间
		}

		sq.push(filename);
		std::cout << "Screening:" << filename << "done" << std::endl;
		writer.release();
	}

	capture.release();
	destroyAllWindows();

	return 0;
}

int Consumer(SendQueue &sq)
{
	int sockfd = TCP::Create();
	TCP::Bind(sockfd, 8888);

	struct sockaddr_in peer_addr;
	int len = sizeof(peer_addr);
	if (TCP::Connect(sockfd, 8777, "127.0.0.1"))
	{
		while (1)
		{

			if (sq.empty())
			{
				std::cout << "queue empty...\n";
				Sleep(1);
			}
			else
			{
				string filename(sq.front());
				sq.pop();
				TCP::SendVideo(sockfd, filename.c_str());
			}

		}
	}
	else
	{
		std::cout << "Connect failed!\n";
	}

	return 0;
}

void Test()
{
	SendQueue sq;
	std::thread pth(Producer, sq);
	pth.join();
}


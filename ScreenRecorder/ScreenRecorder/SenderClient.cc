#if 0

#define WIN32_LEAN_AND_MEAN
#include "GlobalData.h"
#include "Sender.hpp"
#include "ThreadBase.hpp"
#include "RWLock.hpp"
#include "Screen.hpp"
#include "Screener.hpp"
#include "Displayer.hpp"
#include <iostream>

#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#pragma warning(disable : 4996)

using namespace std;

int main()
{
	GlobalData data;

	GetWidthAndHeight(data.width_, data.height_);

	Screener screener(&data);
	Displayer displayer(&data);
	Sender sender(&data);
	screener.start();
	displayer.start();
	sender.start();

	return 0;
}

#else

#include "WinSocket.hpp"
#include "Screen.hpp"
#include "protocol.hpp"
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <thread>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#pragma warning(disable : 4996)

using namespace cv;
using namespace std;


int main() {
	//初始化 DLL
	WSADATA data;
	WORD w = MAKEWORD(2, 0);
	::WSAStartup(w, &data);

#if 0
	VideoCapture capture(0);
	if (!capture.isOpened())
		return -1;


	unsigned int f = (unsigned)capture.get(cv::CAP_PROP_FOURCC);
	char fourcc[] = {
		(char)f, // First character is lowest bits
		(char)(f >> 8), // Next character is bits 8-15
		(char)(f >> 16), // Next character is bits 16-23
		(char)(f >> 24), // Last character is bits 24-31
		'\0' // and don't forget to terminate
	};
	cout << "FourCC for this video was: " << fourcc << endl;

	
	double dWidth = capture.get(CAP_PROP_FRAME_WIDTH); //get the width of frames of the video  
	double dHeight = capture.get(CAP_PROP_FRAME_HEIGHT);
	cout << dWidth << "x" << dHeight << endl;

	char *pbuf = new char[10000000];
	VideoWriter writer("VideoTest.mp4", MAKEFOURCC('D', 'I', 'V', 'X'), 15.0, Size(static_cast<int>(dWidth), static_cast<int>(dHeight)), true);
	//VideoWriter writer("VideoTest.avi", MAKEFOURCC('M', 'J', 'P', 'G'), 15.0, Size(dWidth, dHeight), true);
	if (!writer.isOpened())
	{
		cout << "writer open failed!\n";
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

	

	writer.release();
	
	capture.release();
	destroyAllWindows();
#endif
	cout << "视频录制完成...\n";

	//ifstream video("VideoTest.mp4", ios::in | ios::binary);
	//ofstream video2("VideoTest2.mp4", ios::out | ios::binary);
	//if (!video.is_open())
	//{
	//	cout << "can' open file\n";
	//	return -1;
	//}
	//char *buf = new char[1024];
	//
	//int count = 0;
	//while (video.read(buf, 100))
	//{
	//	count += 100;
	//	cout << "read 100 byte\n";
	//	video2.write(buf, 100);
	//	memset(buf, 0, 100);
	//}
	//
	//cout << count << endl;
	//video.close();
	//video2.close();
	//delete buf;
	
#if 0 //UDP发送
	int sockfd = UDP::Create();
	UDP::Bind(sockfd, 8888);
	struct sockaddr_in peer_addr;
	peer_addr.sin_family = AF_INET;
	peer_addr.sin_addr.S_un.S_addr = inet_addr("39.108.227.206");
	peer_addr.sin_port = htons(8777);

	UDP::SendVideo(sockfd, "VideoTest.mp4", peer_addr);
#endif 

	int sockfd = TCP::Create();
	TCP::Bind(sockfd, 8888);

	struct sockaddr_in peer_addr;
	int len = sizeof(peer_addr);
	if (TCP::Connect(sockfd, 8777, "127.0.0.1"))
	{
		TCP::SendVideo(sockfd, "VideoTest.mp4");
	}
	else
	{
		std::cout << "Connect failed!\n";
	}
	WSACleanup();
	return 0;
}

#endif
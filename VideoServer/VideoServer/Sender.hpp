#ifndef _SENDER_HPP_
#define _SENDER_HPP_

#include "WinSocket.hpp"
#include "ThreadBase.hpp"
#include "opencv2/opencv.hpp"
#include <Windows.h>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#pragma warning(disable : 4996)

//待传输图像默认大小为 640*480，可修改
#define IMG_WIDTH 640	// 需传输图像的宽
#define IMG_HEIGHT 480	// 需传输图像的高
//默认格式为CV_8UC3
#define BUFFER_SIZE IMG_WIDTH*IMG_HEIGHT*3/32 
using namespace cv;

struct sentbuf {
	char buf[BUFFER_SIZE];
	int flag;
};

class Sender :public ThreadBase
{
public:
	Sender():Sender_quit(false),ServSocket_(), ClntSocket_() {}

	~Sender()
	{}

	virtual void start()
	{
		Sender_quit = false;

		try
		{
			ServSocket_.Create(AF_INET, SOCK_STREAM, 0);

			//绑定套接字
			ServSocket_.Bind(9999);
			//进入监听状态
			ServSocket_.Listen(5);
		}
		catch (char* e)
		{
			std::cout << "Socket error:" << e << std::endl;
		}

		ThreadBase::start();
	}

	virtual void quit()
	{
		Sender_quit = true;
		ThreadBase::quit();
	}

	void sendMat(Mat image) {
		for (int k = 0; k < 32; k++) {
			int num1 = IMG_HEIGHT / 32 * k;
			for (int i = 0; i < IMG_HEIGHT / 32; i++) {
				int num2 = i * IMG_WIDTH * 3;
				uchar* ucdata = image.ptr<uchar>(i + num1);
				for (int j = 0; j < IMG_WIDTH * 3; j++) {
					data.buf[num2 + j] = ucdata[j];
				}
			}
			if (k == 31)
				data.flag = 2;
			else
				data.flag = 1;

			ClntSocket_.Send((char*)(&data), sizeof(data));
			//send(sockClient, (char*)(&data), sizeof(data), 0);
		}
	}

private:
	virtual void threadMain()override
	{
		VideoCapture cap(0); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
		{
			std::cout << "Capture open failed!\n";
			quit();
		}
		Mat frame;


		SOCKADDR clntAddr;
		int nSize = sizeof(SOCKADDR);
		ClntSocket_.setSocket(ServSocket_.Accept((sockaddr*)& clntAddr, nSize));

		//SOCKET clntSock = accept(servSock, (SOCKADDR*)& clntAddr, &nSize);
		std::cout << "linked\n";

		while (!Sender_quit)
		{
			cap >> frame; // get a new frame from camera
			sendMat(frame);
		}

		ServSocket_.Close();
		ClntSocket_.Close();

	}

	Socket ServSocket_;
	Socket ClntSocket_;
	bool Sender_quit;
	sentbuf data;
};

#endif // !_SENDER_HPP_

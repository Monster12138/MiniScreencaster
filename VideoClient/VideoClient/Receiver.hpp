#ifndef _RECEIVER_HPP_
#define _RECEIVER_HPP_

#include "opencv2/opencv.hpp"
#include <WinSock2.h>
#include <Windows.h>
#include "WinSocket.hpp"
#include "ThreadBase.hpp"
#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable : 4996)
using namespace cv;

//待传输图像默认大小为 640*480，可修改
#define IMG_WIDTH 640	// 需传输图像的宽
#define IMG_HEIGHT 480	// 需传输图像的高
//默认格式为CV_8UC3
#define BUFFER_SIZE IMG_WIDTH*IMG_HEIGHT*3/32 

struct recvbuf
{
	char buf[BUFFER_SIZE];
	int flag;
};



class Receiver :public ThreadBase
{
public:
	Receiver() :Recver_quit(false) {}

	virtual void start()
	{
		Recver_quit = false;

		// 创建套接字
		try
		{
			ClntSocket.Create(AF_INET, SOCK_STREAM, 0);

			// 构造ip地址
			std::cout << "connecting\n";
			ClntSocket.Connect(AF_INET, 9999, "127.0.0.1");
		}
		catch (char* e)
		{
			std::cout << "Socket error:" << e << std::endl;
			exit(0);
		}

		std::cout << "linked\n";

		ThreadBase::start();
	}

	virtual void quit()
	{
		Recver_quit = true;
		ThreadBase::quit();
	}

	Mat recieveMat() 
	{
		Mat img(IMG_HEIGHT, IMG_WIDTH, CV_8UC3, cv::Scalar(0));
		int needRecv = sizeof(recvbuf);
		int count = 0;
		for (int i = 0; i < 32; i++) 
		{
			int pos = 0;
			int len0 = 0;
			while (pos < needRecv) 
			{
				len0 = ClntSocket.Recv((char*)(&data_recv) + pos, needRecv - pos);
				//len0 = recv(sockServer, (char*)(&data_recv) + pos, needRecv - pos, 0);
				pos += len0;
			}
			count = count + data_recv.flag;
			int num1 = IMG_HEIGHT / 32 * i;
			for (int j = 0; j < IMG_HEIGHT / 32; j++) 
			{
				int num2 = j * IMG_WIDTH * 3;
				uchar* ucdata = img.ptr<uchar>(j + num1);
				for (int k = 0; k < IMG_WIDTH * 3; k++) 
				{
					ucdata[k] = data_recv.buf[num2 + k];
				}
			}
			if (data_recv.flag == 2) 
			{
				if (count == 33) 
				{
					return img;
				}
				else 
				{
					count = 0;
					i = 0;
				}
			}
		}

		return img;
	}

private:
	virtual void threadMain()override
	{
		while (Recver_quit) 
		{
			recvMat = recieveMat();
			if (recvMat.data) 
			{
				resize(recvMat, dstMat, Size(800, 600), 0, 0);
				imshow("Desktop", dstMat);
			}
			//if (waitKey(1) >= 0)break;
		}

		ClntSocket.Close();
	}

	recvbuf data_recv;
	Socket ClntSocket;
	bool Recver_quit;
	Mat recvMat;
	Mat dstMat;
};

#endif // !_RECEIVER_HPP_

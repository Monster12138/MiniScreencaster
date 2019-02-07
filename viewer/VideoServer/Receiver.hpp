#ifndef _RECEIVER_HPP_
#define _RECEIVER_HPP_

#include "opencv2/opencv.hpp"
#include <WinSock2.h>
#include <Windows.h>
#include "WinSocket.hpp"
#include "ThreadBase.hpp"
#include "Screen.hpp"
#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable : 4996)
using namespace cv;

int IMG_WIDTH;
int IMG_HEIGHT;
int BUFFER_SIZE = 0;

struct recvbuf {
	int flag;
	int size;
	uchar buf[];
	recvbuf() :flag(0), size(sizeof(recvbuf) + BUFFER_SIZE) {}
};



class Receiver :public ThreadBase
{
public:
	Receiver() :data_recv((recvbuf*)new char*[sizeof(recvbuf) + BUFFER_SIZE]), Recver_quit(false),recvMat(IMG_HEIGHT, IMG_WIDTH, CV_8UC4) 
	{
		data_recv->size = sizeof(data_recv) + BUFFER_SIZE;
	}


	~Receiver()
	{
		if (data_recv)
		{
			delete[] data_recv;
			data_recv = nullptr;
		}
	}

	virtual void start()
	{
		Recver_quit = false;

		// 创建套接字
		try
		{
			ClntSocket.Create(AF_INET, SOCK_STREAM, 0);

			// 构造ip地址
			std::cout << "connecting\n";
			ClntSocket.Connect(AF_INET, 8888, "127.0.0.1");
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

	bool recieveMat()
	{
		int needRecv = data_recv->size;
		int count = 0;

		for (int i = 0; i < 32; i++)
		{
			int pos = 0;
			int len0 = 0;
			while (pos < needRecv)
			{
				len0 = ClntSocket.Recv((char*)data_recv + pos, needRecv - pos);

				std::cout << "Receive " << len0/1024 << " KByte data\n";
				if (len0 < 0)
				{
					Recver_quit = true;
					return false;
				}
				pos += len0;
			}

			count = count + data_recv->flag;
			int num1 = IMG_HEIGHT / 32 * i;
			for (int j = 0; j < IMG_HEIGHT / 32; j++)
			{
				int num2 = j * IMG_WIDTH * 4;
				uchar* ucdata = recvMat.ptr<uchar>(j + num1);

				for (int k = 0; k < IMG_WIDTH * 4; k++)
				{
					ucdata[k] = data_recv->buf[num2 + k];
				}
			}

			if (data_recv->flag == 2)
			{
				if (count == 33)
				{
					std::cout << sum << std::endl;
					return true;
				}
				else
				{
					count = 0;
					i = 0;
				}
			}
		}
		return false;
	}

	int64_t CheckMat()
	{
		int64_t sum = 0;
		for (int i = 0; i < IMG_HEIGHT; ++i)
		{ 
			uchar* ucdata = recvMat.ptr<uchar>(i);
			for (int j = 0; j < IMG_WIDTH; ++j)
			{
				sum += ucdata[j];
			}
		}

		return sum;
	}

private:
	virtual void threadMain()override
	{
		Mat dstMat;
		while (!Recver_quit)
		{
			recieveMat();
			std::cout << CheckMat() << std::endl;

			if (recvMat.data)
			{
				resize(recvMat, dstMat, Size(1280, 720), 0, 0);
				imshow("Receiver", dstMat);
				waitKey(30);
			}
			//if (waitKey(1) >= 0)break;
		}

		ClntSocket.Close();
	}

	recvbuf* data_recv;
	Socket ClntSocket;
	bool Recver_quit;
	HBITMAP hBmp;
	Mat recvMat;
};

#endif // !_RECEIVER_HPP_

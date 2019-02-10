#ifndef _SENDER_HPP_
#define _SENDER_HPP_

#include "WinSocket.hpp"
#include "ThreadBase.hpp"
#include "opencv2/opencv.hpp"
#include "Screen.hpp"
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#pragma warning(disable : 4996)

//默认格式为CV_8UC4
using namespace cv;

typedef unsigned char uchar;

class Sender :public ThreadBase
{
public:
	Sender()
	{}

	~Sender()
	{
		ServSocket_.Close();
		ClntSocket_.Close();
	}

	virtual void start()
	{
		Sender_quit = false;

		try
		{
			std::cout << "Create Socket...\n";
			ServSocket_.Create(AF_INET, SOCK_STREAM, 0);

			int contain = 0;
			
			setsockopt(ServSocket_.getSocket(), SOL_SOCKET, SO_REUSEADDR, (char *)&contain, sizeof(int));
			//绑定套接字
			std::cout << "Binding...\n";
			ServSocket_.Bind(8888);
			//进入监听状态
			std::cout <<"Listening...\n";
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

private:
	virtual void threadMain()override
	{
		SOCKADDR clntAddr;
		int nSize = sizeof(SOCKADDR);
		while (1)
		{
			Sender_quit = false;

			Mat dstMat;
			HBITMAP hBmp;
			while (!Sender_quit)
			{
				Screen(hBmp, width, height);
				HBitmapToMat(hBmp, screen_); 
				resize(screen_, dstMat, Size(width *2/3, height *2/3), 0, 0);
				
				//std::cout << CheckMat() << std::endl;
				//sendMat(screen_);	
				
				imshow("Sender", dstMat);
				waitKey(30);
			}
			std::cout << "Disconnect!\n";
		}
		
	}

	int width;
	int height;
	Socket ServSocket_;
	Socket ClntSocket_;
	bool Sender_quit;
	BITMAP	Bmp_;
	Mat screen_;
};

#endif // !_SENDER_HPP_

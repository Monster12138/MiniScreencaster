#ifndef _SENDER_HPP_
#define _SENDER_HPP_

#include "WinSocket.hpp"
#include "ThreadBase.hpp"
#include "opencv2/opencv.hpp"
#include "Screen.hpp"
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#pragma warning(disable : 4996)

//待传输图像默认大小为 640*480，可修改
#define IMG_WIDTH 1536	// 需传输图像的宽
#define IMG_HEIGHT 864	// 需传输图像的高
//默认格式为CV_8UC4
#define BUFFER_SIZE IMG_WIDTH*IMG_HEIGHT*4/32 
using namespace cv;

struct sentbuf {
	uchar buf[BUFFER_SIZE];
	int flag;
};

class Sender :public ThreadBase
{
public:
	Sender() :Sender_quit(false){}

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
			ServSocket_.Create(AF_INET, SOCK_STREAM, 0);

			int contain = 0;
			setsockopt(ServSocket_.getSocket(), SOL_SOCKET, SO_REUSEADDR, (char *)&contain, sizeof(int));
			//绑定套接字
			ServSocket_.Bind(8888);
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

	void sendMat(Mat image) 
	{
		for (int k = 0; k < 32; k++) 
		{
			int num1 = IMG_HEIGHT / 32 * k;
			for (int i = 0; i < IMG_HEIGHT / 32; i++) 
			{
				int num2 = i * IMG_WIDTH * 4;
				uchar* ucdata = image.ptr<uchar>(i + num1);

				for (int j = 0; j < IMG_WIDTH * 4; j++) 
				{
					data_.buf[num2 + j] = ucdata[j];
				}
			}
			
			if (k == 31)
				data_.flag = 2;
			else
				data_.flag = 1;

			int ret;
			if ((ret = ClntSocket_.Send((char*)(&data_), sizeof(data_))) < 0)
			{
				Sender_quit = true;
			}
			std::cout << "Send " << ret << " Byte data\n";
			//send(sockClient, (char*)(&data), sizeof(data), 0);
		}
	}


	int64_t CheckMat()
	{
		int64_t sum = 0;
		for (int i = 0; i < IMG_HEIGHT; ++i)
		{
			uchar* ucdata = screen_.ptr<uchar>(i);
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
		SOCKADDR clntAddr;
		int nSize = sizeof(SOCKADDR);
		while (1)
		{
			Sender_quit = false;
			std::cout << "Waiting Connect...\n";
			ClntSocket_.setSocket(ServSocket_.Accept((sockaddr*)& clntAddr, nSize));
			std::cout << "linked\n";

			Mat dstMat;
			HBITMAP hBmp;
			//while (!Sender_quit)
			{
				Screen(hBmp);
				HBitmapToMat(hBmp, screen_); 
				//resize(screen_, dstMat, Size(640, 480), 0, 0);
				
				std::cout << CheckMat() << std::endl;
				sendMat(screen_);	
				
				imshow("Sender", screen_);
				waitKey(0);
			}
			std::cout << "Disconnect!\n";
		}
		
	}

	Socket ServSocket_;
	Socket ClntSocket_;
	bool Sender_quit;
	sentbuf data_;
	BITMAP	Bmp_;
	Mat screen_;
};

#endif // !_SENDER_HPP_

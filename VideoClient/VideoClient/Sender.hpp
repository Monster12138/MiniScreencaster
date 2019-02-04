#ifndef _SENDER_HPP_
#define _SENDER_HPP_

#include "WinSocket.hpp"
#include "ThreadBase.hpp"
#include "opencv2/opencv.hpp"
#include "Screen.hpp"
#pragma comment (lib, "ws2_32.lib")  //���� ws2_32.dll
#pragma warning(disable : 4996)

//������ͼ��Ĭ�ϴ�СΪ 640*480�����޸�
#define IMG_WIDTH 640	// �贫��ͼ��Ŀ�
#define IMG_HEIGHT 480	// �贫��ͼ��ĸ�
//Ĭ�ϸ�ʽΪCV_8UC3
#define BUFFER_SIZE IMG_WIDTH*IMG_HEIGHT*3/32 
using namespace cv;

struct sentbuf {
	char buf[BUFFER_SIZE];
	int flag;
};

class Sender :public ThreadBase
{
public:
	Sender() :Sender_quit(false){}

	~Sender()
	{}

	virtual void start()
	{
		Sender_quit = false;

		try
		{
			ServSocket_.Create(AF_INET, SOCK_STREAM, 0);

			//���׽���
			ServSocket_.Bind(8888);
			//�������״̬
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
				int num2 = i * IMG_WIDTH * 3;
				uchar* ucdata = image.ptr<uchar>(i + num1);
				for (int j = 0; j < IMG_WIDTH * 3; j++) 
				{
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

		Mat tmp;
		SOCKADDR clntAddr;
		int nSize = sizeof(SOCKADDR);
		ClntSocket_.setSocket(ServSocket_.Accept((sockaddr*)& clntAddr, nSize));

		//SOCKET clntSock = accept(servSock, (SOCKADDR*)& clntAddr, &nSize);
		std::cout << "linked\n";

		while (!Sender_quit)
		{
			Screen(hBmp);
			HBitmapToMat(hBmp, screen);

			resize(screen, tmp, Size(800, 600), 0, 0);
			imshow("Desktop", tmp);
			sendMat(screen);
			std::cout << "Send a Mat\n";
		}

		DeleteObject(hBmp);
		ServSocket_.Close();
		ClntSocket_.Close();

	}

	Socket ServSocket_;
	Socket ClntSocket_;
	bool Sender_quit;
	sentbuf data;
	HBITMAP	hBmp;
	Mat screen;
};

#endif // !_SENDER_HPP_

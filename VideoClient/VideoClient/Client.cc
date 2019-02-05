#if 0

#include "opencv2/opencv.hpp"
#include <WinSock2.h>
#include <Windows.h>
#include "WinSocket.hpp"
#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable : 4996)
using namespace cv;

//待传输图像默认大小为 640*480，可修改
#define IMG_WIDTH 1920	// 需传输图像的宽
#define IMG_HEIGHT 1080	// 需传输图像的高
//默认格式为CV_8UC3
#define BUFFER_SIZE IMG_WIDTH*IMG_HEIGHT*3/32 
struct recvbuf
{
	char buf[BUFFER_SIZE];
	int flag;
};
recvbuf data_recv;

Mat recieveMat(Socket sockServer) {
	Mat img(IMG_HEIGHT, IMG_WIDTH, CV_8UC3, cv::Scalar(0));
	int needRecv = sizeof(recvbuf);
	int count = 0;
	for (int i = 0; i < 32; i++) {
		int pos = 0;
		int len0 = 0;
		while (pos < needRecv) {
			len0 = sockServer.Recv((char*)(&data_recv) + pos, needRecv - pos);
			//len0 = recv(sockServer, (char*)(&data_recv) + pos, needRecv - pos, 0);
			pos += len0;
		}
		count = count + data_recv.flag;
		int num1 = IMG_HEIGHT / 32 * i;
		for (int j = 0; j < IMG_HEIGHT / 32; j++) {
			int num2 = j * IMG_WIDTH * 3;
			uchar* ucdata = img.ptr<uchar>(j + num1);
			for (int k = 0; k < IMG_WIDTH * 3; k++) {
				ucdata[k] = data_recv.buf[num2 + k];
			}
		}
		if (data_recv.flag == 2) {
			if (count == 33) {
				return img;
			}
			else {
				count = 0;
				i = 0;
			}
		}
	}

	return img;
}

int main() {

	//初始化 DLL
	WSADATA data;
	WORD w = MAKEWORD(2, 0);
	::WSAStartup(w, &data);
	// 创建套接字

	Socket s;
	try
	{
		s.Create(AF_INET, SOCK_STREAM, 0);

		// 构造ip地址
		std::cout << "connecting\n";
		s.Connect(AF_INET, 8888, "127.0.0.1");
	}
	catch (char* e)
	{
		std::cout << "Socket error:" << e << std::endl;
		exit(0);
	}
	
	std::cout << "linked\n";
	while (true) {
		Mat frame = recieveMat(s);
		if (frame.data) imshow("Camera", frame);
		if (waitKey(1) >= 0)break;
	}
	s.Close();
	::WSACleanup();

	return 0;
}
#endif



#include "Sender.hpp"
#include <iostream>

int main()
{
	//初始化 DLL
	WSADATA data;
	WORD w = MAKEWORD(2, 0);
	::WSAStartup(w, &data);


	Sender sender;

	sender.start();

	sender.getpThread()->join();

	WSACleanup();
	return 0;
}



#if 0

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/types_c.h"
#include <Windows.h>
using namespace cv;

HBITMAP	hBmp;
HBITMAP	hOld;

//抓取当前屏幕函数
void Screen() {

	//创建画板
	HDC hScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
	HDC	hCompDC = CreateCompatibleDC(hScreen);
	//取屏幕宽度和高度
	int		nWidth = GetSystemMetrics(SM_CXSCREEN);
	int		nHeight = GetSystemMetrics(SM_CYSCREEN);
	//创建Bitmap对象
	hBmp = CreateCompatibleBitmap(hScreen, nWidth, nHeight);
	hOld = (HBITMAP)SelectObject(hCompDC, hBmp);
	BitBlt(hCompDC, 0, 0, nWidth, nHeight, hScreen, 0, 0, SRCCOPY);
	SelectObject(hCompDC, hOld);
	//释放对象
	DeleteDC(hScreen);
	DeleteDC(hCompDC);
}

//把HBITMAP型转成Mat型
bool HBitmapToMat(HBITMAP& _hBmp, Mat& _mat)

{
	//BITMAP操作
	BITMAP bmp;
	GetObject(_hBmp, sizeof(BITMAP), &bmp);
	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	//mat操作
	Mat v_mat;
	v_mat.create(Size(bmp.bmWidth, bmp.bmHeight), CV_MAKETYPE(CV_8U, nChannels));
	GetBitmapBits(_hBmp, bmp.bmHeight * bmp.bmWidth * nChannels, v_mat.data);
	_mat = v_mat;
	return TRUE;
}


int main()
{
	while (1)
	{
		Mat src;
		Mat dst;
		//屏幕截图
		Screen();

		//类型转换
		HBitmapToMat(hBmp, src);

		//调整大小
		resize(src, dst, Size(1000, 600), 0, 0);

		imshow("dst", dst);
		DeleteObject(hBmp);
		waitKey(30);//这里调节帧数  现在200ms是5帧

	}
	return 0;

}

#endif
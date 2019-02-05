#if 0

#include "opencv2/opencv.hpp"
#include <WinSock2.h>
#include <Windows.h>
#include "WinSocket.hpp"
#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable : 4996)
using namespace cv;

//������ͼ��Ĭ�ϴ�СΪ 640*480�����޸�
#define IMG_WIDTH 1920	// �贫��ͼ��Ŀ�
#define IMG_HEIGHT 1080	// �贫��ͼ��ĸ�
//Ĭ�ϸ�ʽΪCV_8UC3
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

	//��ʼ�� DLL
	WSADATA data;
	WORD w = MAKEWORD(2, 0);
	::WSAStartup(w, &data);
	// �����׽���

	Socket s;
	try
	{
		s.Create(AF_INET, SOCK_STREAM, 0);

		// ����ip��ַ
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
	//��ʼ�� DLL
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

//ץȡ��ǰ��Ļ����
void Screen() {

	//��������
	HDC hScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
	HDC	hCompDC = CreateCompatibleDC(hScreen);
	//ȡ��Ļ��Ⱥ͸߶�
	int		nWidth = GetSystemMetrics(SM_CXSCREEN);
	int		nHeight = GetSystemMetrics(SM_CYSCREEN);
	//����Bitmap����
	hBmp = CreateCompatibleBitmap(hScreen, nWidth, nHeight);
	hOld = (HBITMAP)SelectObject(hCompDC, hBmp);
	BitBlt(hCompDC, 0, 0, nWidth, nHeight, hScreen, 0, 0, SRCCOPY);
	SelectObject(hCompDC, hOld);
	//�ͷŶ���
	DeleteDC(hScreen);
	DeleteDC(hCompDC);
}

//��HBITMAP��ת��Mat��
bool HBitmapToMat(HBITMAP& _hBmp, Mat& _mat)

{
	//BITMAP����
	BITMAP bmp;
	GetObject(_hBmp, sizeof(BITMAP), &bmp);
	int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	//mat����
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
		//��Ļ��ͼ
		Screen();

		//����ת��
		HBitmapToMat(hBmp, src);

		//������С
		resize(src, dst, Size(1000, 600), 0, 0);

		imshow("dst", dst);
		DeleteObject(hBmp);
		waitKey(30);//�������֡��  ����200ms��5֡

	}
	return 0;

}

#endif
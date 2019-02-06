
#if 1

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

#else

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

#endif
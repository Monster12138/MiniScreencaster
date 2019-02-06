#ifndef _SCREEN_HPP_
#define _SCREEN_HPP_

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2\core\types_c.h"
#include <Windows.h>
#include <shellscalingapi.h>

using namespace cv;

HBITMAP	hOld;

void GetWidthAndHeight(int& width, int& height)
{
	//取屏幕宽度和高度
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);
}

//抓取当前屏幕函数
void Screen(HBITMAP& hBmp, const int nWidth, const int nHeight) {

	//创建画板
	HDC hScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
	HDC	hCompDC = CreateCompatibleDC(hScreen);

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


#endif // !_SCREEN_HPP_

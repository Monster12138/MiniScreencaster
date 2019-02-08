#if 0

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/types_c.h"
#include <Windows.h>
#include <shellscalingapi.h>

using namespace cv;

HBITMAP	hBmp;
HBITMAP	hOld;

//抓取当前屏幕函数
void Screen() {

	//创建画板
	HDC hScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
	HDC	hCompDC = CreateCompatibleDC(hScreen);
	
	//取屏幕宽度和高度
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	int		nWidth = GetSystemMetrics(SM_CXSCREEN);
	int		nHeight = GetSystemMetrics(SM_CYSCREEN);

	std::cout << "width:" << nWidth << std::endl;
	std::cout << "height:" << nHeight << std::endl;
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
	//while (1)
	{
		Mat src;
		Mat dst;
		//屏幕截图
		Screen();

		//类型转换
		HBitmapToMat(hBmp, src);

		//调整大小
		resize(src, dst, Size(960, 540), 0, 0);

		imshow("dst", dst);
		DeleteObject(hBmp);
		waitKey(0);//这里调节帧数  现在200ms是5帧

	}
	return 0;

}


#include "Sender.hpp"
#include <iostream>

int main()
{
	//初始化 DLL
	WSADATA data;
	WORD w = MAKEWORD(2, 0);
	::WSAStartup(w, &data);

	//ShowWindow(GetConsoleWindow(), SW_HIDE);
	GetWidthAndHeight(IMG_WIDTH, IMG_HEIGHT);
	BUFFER_SIZE = IMG_WIDTH * IMG_HEIGHT * 4 / 32;

	Sender sender;

	sender.start();

	sender.getpThread()->join();

	WSACleanup();
	return 0;
}

#else
#include "WinSocket.hpp"
#include "Screen.hpp"
#include <iostream>
#include <string.h>
#include <vector>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#pragma warning(disable : 4996)

using namespace cv;
using namespace std;

const int IMG_WIDTH = 1920;
const int IMG_HEIGHT = 1080;
const int BUFFER_SIZE = IMG_WIDTH * IMG_HEIGHT * 4/32;

struct Addr
{
	struct in_addr ip;
	int port;
};

struct sentbuf {
	int flag;
	int size;
	uchar buf[];
	sentbuf() :flag(0), size(sizeof(sentbuf) + BUFFER_SIZE) {}
};

sentbuf *data_ = (sentbuf *)new char[sizeof(sentbuf) + BUFFER_SIZE];

void sendVector(const vector<uchar>& vec)
{

}

void sendMat(Mat image, Socket socket)
{
	data_->size = sizeof(sentbuf) + BUFFER_SIZE;
	for (int k = 0; k < 32; k++)
	{
		int num1 = IMG_HEIGHT / 32 * k;
		for (int i = 0; i < IMG_HEIGHT / 32; i++)
		{
			int num2 = i * IMG_WIDTH * 4;
			uchar* ucdata = image.ptr<uchar>(i + num1);

			for (int j = 0; j < IMG_WIDTH * 4; j++)
			{
				data_->buf[num2 + j] = ucdata[j];
			}
		}

		if (k == 319)
			data_->flag = 2;
		else
			data_->flag = 1;

		int ret;
		ret = socket.Send((char*)data_, data_->size);
		std::cout << "Send " << ret  << " Byte data\n";
		//send(sockClient, (char*)(&data), sizeof(data), 0);
	}
}

//利用.ptr和数组下标进行图像像素遍历,位操作运算用于减少图像颜色总数
void colorReduce3(cv::Mat &image, int div = 64)
{
	int nl = image.rows;
	int nc = image.cols * image.channels();

	int n = static_cast<int>(log(static_cast<double>(div)) / log(2.0));   //div=64, n=6
	uchar mask = 0xFF << n;                                            //e.g. div=64, mask=0xC0

	   //遍历图像的每个像素
	for (int j = 0; j < nl; ++j)
	{
		uchar *data = image.ptr<uchar>(j);
		for (int i = 0; i < nc; ++i)
		{
			*data++ = *data&mask + div / 2;
		}
	}
}

int main()
{
	//初始化 DLL
	WSADATA data;
	WORD w = MAKEWORD(2, 0);
	::WSAStartup(w, &data);

#if 1
	Addr myaddr;

	char c = '1';
	Socket Sender;
	Sender.Create(AF_INET, SOCK_STREAM, 0);
	int contain;
	setsockopt(Sender.getSocket(), SOL_SOCKET, SO_REUSEADDR, (char *)&contain, sizeof(int));

	Sender.Connect(8888, "39.108.227.206");

	Sender.Recv((char *)&myaddr, sizeof(Addr));
	std::cout << "ip:" << inet_ntoa(myaddr.ip) << " port:" << myaddr.port << std::endl;

	int width, height;
	GetWidthAndHeight(width, height);
	HBITMAP hBmp;

	while (1)
	{
		Screen(hBmp, width, height);
		Mat mat;
		Mat mat2;

		HBitmapToMat(hBmp, mat);

		vector<uchar> buff;
		vector<int> param = vector<int>(2);
		param[0] = IMWRITE_JPEG_CHROMA_QUALITY;
		param[1] = 0;

		imencode(".jpeg", mat, buff, param);
		cout << "code file size(jpeg):" << buff.size() << endl;
		Sender.Send((char *)(*buff.begin()), buff.size());
		mat2 = imdecode(Mat(buff), IMREAD_COLOR);

		//imwrite("1.jpeg", mat);
		//mat2 = imread("1.jpeg");
		resize(mat2, mat, Size(IMG_WIDTH * 2 / 3, IMG_HEIGHT * 2 / 3), 0, 0);
		imshow("Sender", mat);
		waitKey(30);
	}
#endif
	//sendMat(mat2, Sender);

	//Sender.Close();



#if 0
	Socket AcptSock;
	AcptSock.Create(AF_INET, SOCK_STREAM, 0);
	if (AcptSock.Bind(myaddr.port))
	{
		std::cout << "port: " << myaddr.port << " Bind success!\n";
	}
	AcptSock.Listen(5);
	std::cout << "Listening...\n";
	Sender.Send(&c, sizeof(char));
	std::cout << "Send success\n";

	Socket SenderSock;
	sockaddr_in recvAddr;
	int len;
	SenderSock.setSocket(AcptSock.Accept((sockaddr*)&recvAddr, len));
	AcptSock.Close();

	int width, height;
	GetWidthAndHeight(width, height);
	HBITMAP hBmp;
	Screen(hBmp, width, height);
	Mat mat;
	HBitmapToMat(hBmp, mat);
	sendMat(mat, SenderSock);

	SenderSock.Close();
#endif

	WSACleanup();
}

#endif
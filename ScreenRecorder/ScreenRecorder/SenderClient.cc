#if 0

#define WIN32_LEAN_AND_MEAN
#include "GlobalData.h"
#include "Sender.hpp"
#include "ThreadBase.hpp"
#include "RWLock.hpp"
#include "Screen.hpp"
#include "Screener.hpp"
#include "Displayer.hpp"
#include <iostream>

#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#pragma warning(disable : 4996)

using namespace std;

int main()
{
	GlobalData data;

	GetWidthAndHeight(data.width_, data.height_);

	Screener screener(&data);
	Displayer displayer(&data);
	Sender sender(&data);
	screener.start();
	displayer.start();
	sender.start();

	return 0;
}

#else

#include "WinSocket.hpp"
#include "Screen.hpp"

#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <thread>
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#pragma warning(disable : 4996)

using namespace cv;
using namespace std;


int main() {
	VideoCapture capture(0);
	if (!capture.isOpened())
		return -1;

#if 0
	unsigned int f = (unsigned)capture.get(cv::CAP_PROP_FOURCC);
	char fourcc[] = {
		(char)f, // First character is lowest bits
		(char)(f >> 8), // Next character is bits 8-15
		(char)(f >> 16), // Next character is bits 16-23
		(char)(f >> 24), // Last character is bits 24-31
		'\0' // and don't forget to terminate
	};
	cout << "FourCC for this video was: " << fourcc << endl;
#endif
	
	double dWidth = capture.get(CAP_PROP_FRAME_WIDTH); //get the width of frames of the video  
	double dHeight = capture.get(CAP_PROP_FRAME_HEIGHT);
	cout << dWidth << "x" << dHeight << endl;

	char *pbuf = new char[10000000];
	VideoWriter writer("VideoTest.mp4", MAKEFOURCC('D', 'I', 'V', 'X'), 15.0, Size(static_cast<int>(dWidth), static_cast<int>(dHeight)), true);
	//VideoWriter writer("VideoTest.avi", MAKEFOURCC('M', 'J', 'P', 'G'), 15.0, Size(dWidth, dHeight), true);
	if (!writer.isOpened())
	{
		cout << "writer open failed!\n";
		return -1;
	}
	Mat frame;
	int frameNum = 100;
	while (frameNum--) {
		capture >> frame;
		imshow("读取视频", frame);
		writer << frame;
		if (waitKey(33) == 27)
			break;//给图像绘制留点时间
	}

	

	writer.release();
	
	capture.release();
	destroyAllWindows();
	
	ifstream video("VideoTest.mp4");
	if (!video.is_open())
	{
		cout << "can' open file\n";
		return -1;
	}
	char *buf = new char[1000000];
	video.get(buf, 1000000, EOF);
	cout << buf << endl;
	video.close();


	return 0;
}

#endif
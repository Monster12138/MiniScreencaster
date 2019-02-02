#include "opencv2/opencv.hpp"
#include <WinSock2.h>
#include <Windows.h>
#include "WinSocket.hpp"
#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable : 4996)
using namespace cv;

//������ͼ��Ĭ�ϴ�СΪ 640*480�����޸�
#define IMG_WIDTH 640	// �贫��ͼ��Ŀ�
#define IMG_HEIGHT 480	// �贫��ͼ��ĸ�
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



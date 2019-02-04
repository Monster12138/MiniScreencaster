#if 0

#include "opencv2/opencv.hpp"
#include "WinSocket.hpp"
#include <Windows.h>
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
sentbuf data;

void sendMat(Socket sockClient, Mat image) {
	for (int k = 0; k < 32; k++) {
		int num1 = IMG_HEIGHT / 32 * k;
		for (int i = 0; i < IMG_HEIGHT / 32; i++) {
			int num2 = i * IMG_WIDTH * 3;
			uchar* ucdata = image.ptr<uchar>(i + num1);
			for (int j = 0; j < IMG_WIDTH * 3; j++) {
				data.buf[num2 + j] = ucdata[j];
			}
		}
		if (k == 31)
			data.flag = 2;
		else
			data.flag = 1;

		sockClient.Send((char*)(&data), sizeof(data));
		//send(sockClient, (char*)(&data), sizeof(data), 0);
	}
}

int main(int, char**)
{
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;
	Mat frame;
	//��ʼ�� DLL
	WSADATA wsaData;
	::WSAStartup(MAKEWORD(2, 0), &wsaData);
	//�����׽���
	Socket servSock;

	try
	{
		servSock.Create(AF_INET, SOCK_STREAM, 0);
		//SOCKET servSock = ::socket(AF_INET, SOCK_STREAM, 0);

		//���׽���
		servSock.Bind(8888);
		//�������״̬
		servSock.Listen(5);
	}
	catch (char* e)
	{
		std::cout << "Socket error:" << e << std::endl;
	}

	//���տͻ�������
	SOCKADDR clntAddr;
	int nSize = sizeof(SOCKADDR);
	Socket clientSock(servSock.Accept((sockaddr*)& clntAddr, nSize));

	//SOCKET clntSock = accept(servSock, (SOCKADDR*)& clntAddr, &nSize);
	std::cout << "linked\n";
	for (;;)
	{
		cap >> frame; // get a new frame from camera

		sendMat(clientSock, frame);
	}
	// the camera will be deinitialized automatically in VideoCapture destructor

	//�ر��׽���
	clientSock.Close();
	servSock.Close();

	//��ֹ DLL ��ʹ��
	WSACleanup();
	return 0;
}


#else

#include "Sender.hpp"
#include "Receiver.hpp"
#include <iostream>

int main()
{
	//��ʼ�� DLL
	WSADATA data;
	WORD w = MAKEWORD(2, 0);
	::WSAStartup(w, &data);

	Receiver receiver;

	receiver.start();

	receiver.getpThread()->join();

	WSACleanup();
	return 0;
}

#endif
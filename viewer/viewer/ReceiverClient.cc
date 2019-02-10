#if 0

#include "Receiver.hpp"
#include <iostream>

int main()
{
	//初始化 DLL
	WSADATA data;
	WORD w = MAKEWORD(2, 0);
	::WSAStartup(w, &data);

	GetWidthAndHeight(IMG_WIDTH, IMG_HEIGHT);
	BUFFER_SIZE = IMG_WIDTH * IMG_HEIGHT * 4 / 32;

	Receiver receiver;

	receiver.start();

	receiver.getpThread()->join();

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

struct Addr
{
	struct in_addr ip;
	int port;
};

uchar recvbuf[1000000];

bool recvScreen(vector<uchar> &vec, Socket socket)
{
	int check = 0;
	int recvBytes = 0;

	int len;
	socket.Recv((char *)&len, sizeof(int));
	cout << "len: " << len << endl;
	vec.resize(len);

	while (recvBytes < len)
	{
		int ret = socket.Recv((char *)&vec[0] + recvBytes, len - recvBytes);
		if (ret <= 0)
			return false;
		recvBytes += ret;
		cout << "Receive " << recvBytes << " Bytes\n";
	}
	cout << "Check: " << check << endl;

	return true;
}

int main()
{
	//初始化 DLL
	WSADATA data;
	WORD w = MAKEWORD(2, 0);
	::WSAStartup(w, &data);

	Addr myaddr;

	char c = '1';
	Socket Recver;
	Recver.Create(AF_INET, SOCK_STREAM, 0);
	int contain;
	setsockopt(Recver.getSocket(), SOL_SOCKET, SO_REUSEADDR, (char *)&contain, sizeof(int));

	Recver.Connect(8888, "39.108.227.206");

	Recver.Recv((char *)&myaddr, sizeof(Addr));
	std::cout << "ip:" << inet_ntoa(myaddr.ip) << " port:" << myaddr.port << std::endl;

	int width, height;
	GetWidthAndHeight(width, height);

	vector<uchar> buff;
	buff.reserve(1000000);

	while (1)
	{
		Mat mat;
		Mat dstMat;
		if (!recvScreen(buff, Recver))
		{
			cout << "网络错误!\n";
			break;
		}
		cout << "======================================\n";
		mat = imdecode(Mat(buff), IMREAD_COLOR);
		buff.clear();
		resize(mat, dstMat, Size(width * 2 / 3, height * 2 / 3), 0, 0);
		imshow("Recver", dstMat);
		waitKey(1);
	}
	WSACleanup();
}

#endif
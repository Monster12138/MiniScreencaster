#if 0

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

	return true;
}

int main()
{
#if 1
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
		cout << "//======================================//\n";
		mat = imdecode(Mat(buff), IMREAD_COLOR);
		buff.clear();
		resize(mat, dstMat, Size(width * 2 / 3, height * 2 / 3), 0, 0);
		imshow("Recver", dstMat);
		if (27 == waitKey(1))
		{
			break;
		}
	}
	WSACleanup();
#endif
}

#endif
#include "protocol.hpp"
#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#pragma warning(disable : 4996)

using namespace std;

int main()
{
	//初始化 DLL
	WSADATA data;
	WORD w = MAKEWORD(2, 0);
	::WSAStartup(w, &data);

#if 0 //UDP接收
	int sockfd = UDP::Create();
	UDP::Bind(sockfd, 8777);
	struct sockaddr_in peer_addr;

	UDP::RecvVideo(sockfd, "Video.mp4", peer_addr);
#endif

	int listen_sock = TCP::Create();
	TCP::Bind(listen_sock, 8777);
	TCP::Listen(listen_sock, 5);
	struct sockaddr_in peer_addr;
	int len = sizeof(peer_addr);
	int work_sock = TCP::Accept(listen_sock, (struct sockaddr*)&peer_addr, len);

	TCP::RecvVideo(work_sock, "Video.mp4");

	WSACleanup();
	return 0;
}
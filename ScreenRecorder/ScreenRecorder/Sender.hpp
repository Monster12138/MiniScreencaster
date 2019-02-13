#ifndef _SENDER_HPP_
#define _SENDER_HPP_

#include "Screen.hpp"
#include "WinSocket.hpp"
#include "ThreadBase.hpp"
#include "GlobalData.h"

//默认格式为CV_8UC4
using namespace cv;
using namespace std;

typedef unsigned char uchar;

struct Addr
{
	struct in_addr ip;
	int port;
};

class Sender :public ThreadBase
{
public:
	Sender(GlobalData *pData):pData_(pData) {}

	Sender(const Sender& rs) = delete;

	Sender& operator=(const Sender& rs) = delete;

	bool sendVector()
	{
		char c;
		int check = 0;
		int sentBytes = 0;
		int ret = sock_.Recv(&c, sizeof(char));
		if (ret <= 0)
			return false;

		while (sentBytes < len_)
		{
			ret = sock_.Send((char *)&vec_[0] + sentBytes, len_ - sentBytes);
			if (ret <= 0)
				return false;
			sentBytes += ret;
			//cout << "Send " << sentByte << " Byte\n";
		}
		//cout << "Check: " << check << endl;
		cout << "sentBytes: " << sentBytes << endl;
		return true;
	}

	virtual void threadMain()override
	{
		cout << "Sender thread start!\n";
		//初始化 DLL
		WSADATA data;
		WORD w = MAKEWORD(2, 0);
		::WSAStartup(w, &data);

		Addr myAddr;
		vector<int> param = vector<int>(2);
		param[0] = IMWRITE_JPEG_CHROMA_QUALITY;
		param[1] = 0;
		vec_.reserve(1000000);

		sock_.Create(AF_INET, SOCK_STREAM, 0);
		int contain;
		setsockopt(sock_.getSocket(), SOL_SOCKET, SO_REUSEADDR, (char *)&contain, sizeof(int));

		sock_.Connect(8888, "39.108.227.206");

		sock_.Recv((char *)&myAddr, sizeof(Addr));
		std::cout << "local ip:" << inet_ntoa(myAddr.ip) << " port:" << myAddr.port << std::endl;

		while (isRunning())
		{
			pData_->RWLock_.ReadLock();
			imencode(".jpeg", pData_->screen_, vec_, param);
			pData_->RWLock_.UnReadLock();

			len_ = vec_.size(); 
			cout << "code file size(jpeg):" << len_ << endl;
			cout << "==============================================\n";
			sock_.Send((char *)&len_, sizeof(int));

			if (!sendVector())
			{
				cout << "网络错误！\n";
				break;
			}
			vec_.clear();
		}

		WSACleanup();
		cout << "Sender thread quit!\n";
	}
private:
	GlobalData *pData_;
	Socket sock_;
	vector<uchar> vec_;
	int len_;
};

#endif // !_SENDER_HPP_
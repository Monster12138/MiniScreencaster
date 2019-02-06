#include "Receiver.hpp"
#include <iostream>

int main()
{
	//³õÊ¼»¯ DLL
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
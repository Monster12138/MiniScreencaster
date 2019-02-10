#if 1

#include "Receiver.hpp"
#include <iostream>

int main()
{
	//³õÊ¼»¯ DLL
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

#endif
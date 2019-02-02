#ifndef _SENDER_HPP_
#define _SENDER_HPP_

#include "ThreadBase.hpp"
#include "WinSocket.hpp"

class Sender :public ThreadBase
{
public:
	Sender():Sender_quit(false) {}

	void start()
	{
		Sender_quit = false;
		ThreadBase::start();
	}

	void quit()
	{

	}
private:
	Socket SendSocket_;
	Socket RecvSocket_;
	bool Sender_quit;
};

#endif // !_SENDER_HPP_

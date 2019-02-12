#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#include <thread>

class ThreadBase
{
public:
	ThreadBase(){}

	virtual ~ThreadBase() 
	{
		if (isRunning_)
		{
			isRunning_ = false;
		}
		if (th_.joinable())
		{
			th_.join();
		}
	}

	virtual void start()
	{
		isRunning_ = true;
		std::thread tmpth(std::bind(&ThreadBase::threadMain, this));
		th_ = std::move(tmpth);
	}

	virtual void quit()
	{
		isRunning_ = false;
	}

	virtual void join()
	{
		th_.join();
	}

	bool isRunning()
	{
		return isRunning_;
	}

	void mSleep(int64_t m)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(m));
	}

	void sleep(int64_t m)
	{
		std::this_thread::sleep_for(std::chrono::seconds(m));
	}
protected:
	virtual void threadMain() = 0;
private:
	bool isRunning_;
	std::thread th_;
};


#endif // !_THREAD_HPP_

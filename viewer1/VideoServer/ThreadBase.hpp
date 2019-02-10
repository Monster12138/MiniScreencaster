#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#include <thread>

class ThreadBase
{
public:
	ThreadBase():m_thread(nullptr){}

	virtual ~ThreadBase() {}

	std::thread* getpThread()
	{
		return m_thread;
	}

	virtual void start() 
	{
		if (nullptr == m_thread)
		{
			std::cout << "new thread is created!\n";
			m_thread = new std::thread{ &ThreadBase::threadMain, this };
		}
	}

	virtual void quit()
	{
		if (nullptr != m_thread)
		{
			delete m_thread;
			m_thread = nullptr;
		}
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
	std::thread* m_thread;
};


#endif // !_THREAD_HPP_

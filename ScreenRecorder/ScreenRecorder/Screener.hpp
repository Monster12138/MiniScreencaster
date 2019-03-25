#ifndef _SCREENER_HPP_
#define _SCREENER_HPP_

#include "Screen.hpp"
#include "WinSocket.hpp"
#include "ThreadBase.hpp"
#include "GlobalData.h"

typedef unsigned char uchar;

class Screener :public ThreadBase
{
public:
	Screener(GlobalData *pData):pData_(pData) {}

	Screener(const Screener& rs) = delete;

	Screener& operator=(const Screener& rs) = delete;

	virtual void threadMain()override
	{
		cout << "Screener thread start!\n";


#if 1
		while (isRunning())
		{
			pData_->RWLock_.WriteLock();
			Screen(pData_->hbmp_, pData_->width_, pData_->height_);
			HBitmapToMat(pData_->hbmp_, pData_->screen_);
			pData_->RWLock_.UnWriteLock();
			mSleep(1);
		}
#else
		VideoCapture cap(0);
		if (!cap.isOpened())
		{
			exit(2);
		}
		while (isRunning())
		{
			pData_->RWLock_.WriteLock();
			cap >> pData_->screen_;
			pData_->RWLock_.UnWriteLock();
			mSleep(1);
		}
#endif
		cout << "Screener thread quit!\n";
		exit(1);
	}
private:
	GlobalData *pData_;
};

#endif // !_SCREENER_HPP_
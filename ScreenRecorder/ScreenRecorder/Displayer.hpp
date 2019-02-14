#ifndef _DISPLAYER_HPP_
#define _DISPLAYER_HPP_

#include "Screen.hpp"
#include "WinSocket.hpp"
#include "ThreadBase.hpp"

typedef unsigned char uchar;

class Displayer :public ThreadBase
{
public:
	Displayer(GlobalData *pData) :pData_(pData) {}

	Displayer(const Screener& rs) = delete;

	Displayer& operator=(const Displayer& rs) = delete;

	virtual void threadMain()override
	{
		cout << "Displayer thread start!\n";

		pData_->RWLock_.ReadLock();
		cout << "Displayer thread get readLock\n";
		showWidth = pData_->width_ * 2 / 3;
		showHeight = pData_->height_ * 2 / 3;
		pData_->RWLock_.UnReadLock();
		cout << "Displayer thread get readLock\n";

		while (isRunning())
		{
			pData_->RWLock_.ReadLock();
			cout << "Displayer thread get readLock\n";
			resize(pData_->screen_, dstMat, Size(showWidth, showHeight), 0, 0);
			pData_->RWLock_.UnReadLock();
			cout << "Displayer thread get readLock\n";

			imshow("Sender", dstMat);
			if (waitKey(10) == 27)
				break;
		}
		cout << "Displayer thread quit!\n";
	}
private:
	GlobalData *pData_;
	cv::Mat dstMat;
	int showWidth;
	int showHeight;
};

#endif // !_DISPLAYER_HPP_
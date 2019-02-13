#ifndef _RWLOCK_HPP_
#define _RWLOCK_HPP_

#include <Windows.h>

class MyRWLock
{
public:
	MyRWLock()
	{
		InitializeSRWLock(&lock_);
	}

	void WriteLock()
	{
		AcquireSRWLockExclusive(&lock_);
	}

	void UnWriteLock()
	{
		ReleaseSRWLockExclusive(&lock_);
	}

	void ReadLock()
	{
		AcquireSRWLockShared(&lock_);
	}

	void UnReadLock()
	{
		ReleaseSRWLockShared(&lock_);
	}

private:
	SRWLOCK lock_;
};

#endif // !_RWLOCK_HPP_


#if 0

#ifndef _RWLockImpl_Header
#define _RWLockImpl_Header

#include <assert.h>
#include <process.h>
#include <Windows.h>

using namespace std;
/*
 读写锁允许当前的多个读用户访问保护资源，但只允许一个写读者访问保护资源
*/

//-----------------------------------------------------------------
class CRWLockImpl
{
protected:
	CRWLockImpl();
	~CRWLockImpl();
	void ReadLockImpl();
	bool TryReadLockImpl();
	void WriteLockImpl();
	bool TryWriteLockImpl();
	void UnlockImpl();

private:
	void AddWriter();
	void RemoveWriter();
	DWORD TryReadLockOnce();

	HANDLE   m_mutex;
	HANDLE   m_readEvent;
	HANDLE   m_writeEvent;
	unsigned m_readers;
	unsigned m_writersWaiting;
	unsigned m_writers;
};

CRWLockImpl::CRWLockImpl() : m_readers(0), m_writersWaiting(0), m_writers(0)
{
	m_mutex = CreateMutex(NULL, FALSE, NULL);
	if (m_mutex == NULL)
		cout << "cannot create reader/writer lock" << endl;

	m_readEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (m_readEvent == NULL)
		cout << "cannot create reader/writer lock" << endl;

	m_writeEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (m_writeEvent == NULL)
		cout << "cannot create reader/writer lock" << endl;
}

CRWLockImpl::~CRWLockImpl()
{
	CloseHandle(m_mutex);
	CloseHandle(m_readEvent);
	CloseHandle(m_writeEvent);
}

inline void CRWLockImpl::AddWriter()
{
	switch (WaitForSingleObject(m_mutex, INFINITE))
	{
	case WAIT_OBJECT_0:
		if (++m_writersWaiting == 1)
			ResetEvent(m_readEvent);
		ReleaseMutex(m_mutex);
		break;
	default:
		cout << "cannot lock reader/writer lock" << endl;
	}
}

inline void CRWLockImpl::RemoveWriter()
{
	switch (WaitForSingleObject(m_mutex, INFINITE))
	{
	case WAIT_OBJECT_0:
		if (--m_writersWaiting == 0 && m_writers == 0)
			SetEvent(m_readEvent);
		ReleaseMutex(m_mutex);
		break;
	default:
		cout << "cannot lock reader/writer lock" << endl;
	}
}

void CRWLockImpl::ReadLockImpl()
{
	HANDLE h[2];
	h[0] = m_mutex;
	h[1] = m_readEvent;
	switch (WaitForMultipleObjects(2, h, TRUE, INFINITE))
	{
	case WAIT_OBJECT_0:
	case WAIT_OBJECT_0 + 1:
		++m_readers;
		ResetEvent(m_writeEvent);
		ReleaseMutex(m_mutex);
		assert(m_writers == 0);
		break;
	default:
		cout << "cannot lock reader/writer lock" << endl;
	}
}

bool CRWLockImpl::TryReadLockImpl()
{
	for (;;)
	{
		if (m_writers != 0 || m_writersWaiting != 0)
			return false;

		DWORD result = TryReadLockOnce();
		switch (result)
		{
		case WAIT_OBJECT_0:
		case WAIT_OBJECT_0 + 1:
			return true;
		case WAIT_TIMEOUT:
			continue;
		default:
			cout << "cannot lock reader/writer lock" << endl;
		}
	}
}

void CRWLockImpl::WriteLockImpl()
{
	AddWriter();
	HANDLE h[2];
	h[0] = m_mutex;
	h[1] = m_writeEvent;
	switch (WaitForMultipleObjects(2, h, TRUE, INFINITE))
	{
	case WAIT_OBJECT_0:
	case WAIT_OBJECT_0 + 1:
		--m_writersWaiting;
		++m_readers;
		++m_writers;
		ResetEvent(m_readEvent);
		ResetEvent(m_writeEvent);
		ReleaseMutex(m_mutex);
		assert(m_writers == 1);
		break;
	default:
		RemoveWriter();
		cout << "cannot lock reader/writer lock" << endl;
	}
}

bool CRWLockImpl::TryWriteLockImpl()
{
	AddWriter();
	HANDLE h[2];
	h[0] = m_mutex;
	h[1] = m_writeEvent;
	switch (WaitForMultipleObjects(2, h, TRUE, 1))
	{
	case WAIT_OBJECT_0:
	case WAIT_OBJECT_0 + 1:
		--m_writersWaiting;
		++m_readers;
		++m_writers;
		ResetEvent(m_readEvent);
		ResetEvent(m_writeEvent);
		ReleaseMutex(m_mutex);
		assert(m_writers == 1);
		return true;
	case WAIT_TIMEOUT:
		RemoveWriter();
	default:
		RemoveWriter();
		cout << "cannot lock reader/writer lock" << endl;
	}
	return false;
}

void CRWLockImpl::UnlockImpl()
{
	switch (WaitForSingleObject(m_mutex, INFINITE))
	{
	case WAIT_OBJECT_0:
		m_writers = 0;
		if (m_writersWaiting == 0) SetEvent(m_readEvent);
		if (--m_readers == 0) SetEvent(m_writeEvent);
		ReleaseMutex(m_mutex);
		break;
	default:
		cout << "cannot unlock reader/writer lock" << endl;
	}
}

DWORD CRWLockImpl::TryReadLockOnce()
{
	HANDLE h[2];
	h[0] = m_mutex;
	h[1] = m_readEvent;
	DWORD result = WaitForMultipleObjects(2, h, TRUE, 1);
	switch (result)
	{
	case WAIT_OBJECT_0:
	case WAIT_OBJECT_0 + 1:
		++m_readers;
		ResetEvent(m_writeEvent);
		ReleaseMutex(m_mutex);
		assert(m_writers == 0);
		return result;
	case WAIT_TIMEOUT:
	default:
		cout << "cannot lock reader/writer lock" << endl;
	}
	return result;
}


//-----------------------------------------------------------------

class CMyRWLock: private CRWLockImpl
{
public:

	//创建读/写锁
	CMyRWLock(){};

	//销毁读/写锁
	~CMyRWLock(){};

	//获取读锁
	//如果其它一个线程占有写锁，则当前线程必须等待写锁被释放，才能对保护资源进行访问
	void ReadLock();

	//尝试获取一个读锁
	//如果获取成功，则立即返回true，否则当另一个线程占有写锁，则返回false
	bool TryReadLock();

	//获取写锁
	//如果一个或更多线程占有读锁，则必须等待所有锁被释放
	//如果相同的一个线程已经占有一个读锁或写锁，则返回结果不确定
	void WriteLock();

	//尝试获取一个写锁
	//如果获取成功，则立即返回true，否则当一个或更多其它线程占有读锁，返回false
	//如果相同的一个线程已经占有一个读锁或写锁，则返回结果不确定
	bool TryWriteLock();

	//释放一个读锁或写锁
	void Unlock();

	CMyRWLock(const CMyRWLock&) = delete;
	CMyRWLock& operator = (const CMyRWLock&) = delete;
};

inline void CMyRWLock::ReadLock()
{
	ReadLockImpl();
}

inline bool CMyRWLock::TryReadLock()
{
	return TryReadLockImpl();
}

inline void CMyRWLock::WriteLock()
{
	WriteLockImpl();
}

inline bool CMyRWLock::TryWriteLock()
{
	return TryWriteLockImpl();
}

inline void CMyRWLock::Unlock()
{
	UnlockImpl();
}


#endif


#endif
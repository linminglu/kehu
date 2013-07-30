#include "stdafx.h"
#include "FCSemaphore.h"

namespace EmrysTool
{


	FCSemaphore::FCSemaphore(int initValue)
	{
		sem_ = ::CreateSemaphore(0, initValue, 1, NULL);
	}

	FCSemaphore::~FCSemaphore()
	{
		::CloseHandle(sem_);
	}

	void FCSemaphore::Wait()
	{
		::WaitForSingleObject(sem_, INFINITE);
	}

	void FCSemaphore::Post()
	{
		::ReleaseSemaphore(sem_, 1, NULL);
	}

	int FCSemaphore::Value()
	{
		long value = -1;
		::ReleaseSemaphore(sem_, 0, &value);
		return int(value);
	}

}

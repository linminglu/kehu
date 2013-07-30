#include "stdafx.h"
#include "FCLockMutex.h"


namespace EmrysTool {

	FCLockMutex::FCLockMutex()
	{
		::InitializeCriticalSectionAndSpinCount(&cs_, 2000);
	}

	FCLockMutex::~FCLockMutex()
	{
		::DeleteCriticalSection(&cs_);
	}

	bool FCLockMutex::tryLockMutex()
	{
		if (::TryEnterCriticalSection(&cs_) != FALSE) {
			return true;
		}
		return false;
	}

	void FCLockMutex::acquireMutex()
	{
		::EnterCriticalSection(&cs_);
	}

	void FCLockMutex::releaseMutex()
	{
		::LeaveCriticalSection(&cs_);
	}

}


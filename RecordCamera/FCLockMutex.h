#pragma once
#include "FCGlobalConfig.h"
namespace EmrysTool {
class FCLockMutex
{
public:
	FCLockMutex(void);

	~FCLockMutex(void);

	bool tryLockMutex();

	void acquireMutex();

	void releaseMutex();
private:

private:
	CRITICAL_SECTION cs_;

	CLASS_COPY_AND_ASSIGN(FCLockMutex);
};
//
// Lock辅助类
// 构造函数-释放锁
// 析构函数-加锁
//
class FCAutoUnlock {
public:
	explicit FCAutoUnlock(FCLockMutex& lock) : lock_(lock) {
		lock_.releaseMutex();
	}

	~FCAutoUnlock() {
		lock_.acquireMutex();
	}

private:
	FCLockMutex& lock_;
	CLASS_COPY_AND_ASSIGN(FCAutoUnlock);
};

} // namespace Base
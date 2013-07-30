#pragma once
#include "FCGlobalConfig.h"
namespace EmrysTool
{
	class FCSemaphore
	{
	public:
		// initValue是信号量初始值
		FCSemaphore(int initValue);

		~FCSemaphore();

		// 等待，直到另一个线程调用释放
		void Wait();

		// 释放信号量
		void Post();

		// 返回当前值
		int Value();

	private:
		HANDLE sem_;
	};
}



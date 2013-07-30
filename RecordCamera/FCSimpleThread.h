#pragma once
#include "FCGlobalConfig.h"
#include "FCLockMutex.h"
#include "FConditionVariable.h"
namespace EmrysTool
{

	class FCSimpleThread
	{
	public:
		FCSimpleThread();

		virtual ~FCSimpleThread();

		// �����߳�, ��֤Start()��������ʱ���߳��Ѿ���ʼִ��
		virtual void Start();

		virtual void Stop();

		virtual bool IsStop();

		// �ȴ��̹߳ر�
		virtual void Join();

		// ������Ҫ���ش˺���
		// Run()�����������½����߳���
		virtual void Run() = 0;

		HANDLE ThreadId() { return thread_id_; }

		void ThreadMain();

	private:
		HANDLE thread_id_;
		bool is_stop_;
		EmrysTool::FConditionVariable cond_;
		EmrysTool::FCLockMutex cond_lock_;
	};


}


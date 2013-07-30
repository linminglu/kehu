#include "stdafx.h"
#include "FCSimpleThread.h"


namespace EmrysTool
{


	DWORD __stdcall ThreadFunc(void* params)
	{
		FCSimpleThread* thread = static_cast<FCSimpleThread*>(params);
		thread->ThreadMain();
		return NULL;
	}

	FCSimpleThread::FCSimpleThread()
		: cond_(&cond_lock_), is_stop_(true)
	{

	}

	FCSimpleThread::~FCSimpleThread()
	{

	}

	void FCSimpleThread::Start()
	{
		thread_id_ = ::CreateThread(NULL, 0, ThreadFunc, this, 0, NULL);
		//cond_.Wait();    
	}

	void FCSimpleThread::Stop()
	{
		is_stop_ = true;
	}

	bool FCSimpleThread::IsStop()
	{
		return is_stop_;
	}

	void FCSimpleThread::Join()
	{
		::WaitForSingleObject(thread_id_, INFINITE);
		::CloseHandle(thread_id_);
	}

	void FCSimpleThread::ThreadMain()
	{
		is_stop_ = false;
		//cond_.Signal();
		Run();
	}

}
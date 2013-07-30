#pragma once
#include "FCGlobalConfig.h"
namespace EmrysTool
{
	class FCSemaphore
	{
	public:
		// initValue���ź�����ʼֵ
		FCSemaphore(int initValue);

		~FCSemaphore();

		// �ȴ���ֱ����һ���̵߳����ͷ�
		void Wait();

		// �ͷ��ź���
		void Post();

		// ���ص�ǰֵ
		int Value();

	private:
		HANDLE sem_;
	};
}



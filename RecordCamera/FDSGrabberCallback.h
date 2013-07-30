#pragma once
#include "qedit.h"
#include "FCLockMutex.h"
class FDSGrabberCallback :
	public ISampleGrabberCB
{
public:
	FDSGrabberCallback();
	~FDSGrabberCallback();
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
	STDMETHODIMP SampleCB(double SampleTime, IMediaSample* pSample);
	STDMETHODIMP BufferCB(double dblSampleTime, BYTE* pBuffer, long lBufferSize);

	bool IsBufferAvailable();

	char* GetBuffer();

	void ReleaseBuffer(char* buf);

	int BufferSize() { return buffer_size_; }

	__int64 Timestamp() { return timestamp_; }

	UINT FPSMeter() { return fps_meter_; }

	void ResetFPSMeter() { fps_meter_ = 0; }

private:
	char* buffer_;
	int buffer_size_;
	__int64 timestamp_;
	bool is_buffer_available_;
	UINT fps_meter_;
	//base::Semaphore semaphore_;
	EmrysTool::FCLockMutex lock_;
};


#include "stdafx.h"
#include "FDSGrabberCallback.h"



FDSGrabberCallback::FDSGrabberCallback()
	: is_buffer_available_(false)
{
	buffer_ = NULL;
	buffer_size_ = 0;
}

FDSGrabberCallback::~FDSGrabberCallback()
{
	if (buffer_)
	{
		delete [] buffer_;
		buffer_=NULL;
	}
}

STDMETHODIMP_(ULONG) FDSGrabberCallback::AddRef()
{
	return 2;
}

STDMETHODIMP_(ULONG) FDSGrabberCallback::Release()
{
	return 1;
}

STDMETHODIMP FDSGrabberCallback::QueryInterface(REFIID riid, void ** ppv)
{
	if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ) 
	{
		*ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
		return NOERROR;
	}    

	return E_NOINTERFACE;
}

STDMETHODIMP FDSGrabberCallback::SampleCB(double SampleTime, IMediaSample* pSample)
{
	return 0;
}

STDMETHODIMP FDSGrabberCallback::BufferCB(double dblSampleTime, BYTE *pBuffer, long lBufferSize)
{
	if (!pBuffer)
	{
		return E_POINTER;
	}

	//semaphore_.Wait();
	EmrysTool::FCAutoLock al(lock_);

	timestamp_ = ::GetTickCount();

	if( buffer_size_ < lBufferSize )
	{
		if (buffer_)
		{
			delete [] buffer_;
			buffer_=NULL;
		}
		buffer_size_ = 0;
	}

	if (!buffer_)
	{
		buffer_ = new char[lBufferSize];
		buffer_size_ = lBufferSize;
	}

	if( !buffer_ )
	{
		buffer_size_ = 0;
		//semaphore_.Post();
		return E_OUTOFMEMORY;
	}

	memcpy( buffer_, pBuffer, lBufferSize );

	//     buffer_ = (char*)pBuffer;
	//     buffer_size_ = lBufferSize;

	is_buffer_available_ = true;

	fps_meter_++;

	//semaphore_.Post();

	return 0;
}

bool FDSGrabberCallback::IsBufferAvailable()
{
	return is_buffer_available_;
}

char* FDSGrabberCallback::GetBuffer()
{
	EmrysTool::FCAutoLock al(lock_);

	char* buf = NULL;
	if (BufferSize())
	{
		buf = (char*)malloc(BufferSize());
		memcpy(buf, buffer_, BufferSize());
	}

	return buf;
}

void FDSGrabberCallback::ReleaseBuffer(char* buf)
{
	is_buffer_available_ = false;
	if (buf) free(buf);
}


#include "stdafx.h"
#include "FDCapture.h"




#include <map>

#include "FDSVideoGraph.h"
#include "FDSVideoCaptureDevice.h"
#include "FVideoEncoderThread.h"
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
FDSCapture::FDSCapture()
{
	// initializing a directshow filtergraph
	//
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	ds_graph_ = new FDSGraph;

	ds_video_graph_ = NULL;
	

	ds_video_cap_device_ = NULL;
	

	
	video_encoder_thread_ = NULL;
}

FDSCapture::~FDSCapture()
{
	Destroy();
	SAFE_DELETE( ds_graph_ );
	CoUninitialize();
}

void FDSCapture::Destroy()
{
	SAFE_DELETE( ds_video_graph_ );
	SAFE_DELETE( ds_video_cap_device_ );
	SAFE_DELETE( video_encoder_thread_ );
}

void FDSCapture::Create(const CString& audioDeviceID, const CString& videoDeviceID,
					   const DSVideoFormat& videoFormat,
					   const CString& audioOutname, const CString& videoOutname)
{
	HRESULT hr;

	// audio

	
	// video

	ds_video_graph_ = new FDSVideoGraph;
	ds_video_cap_device_ = new FDSVideoCaptureDevice;
	video_encoder_thread_ = new FVideoEncoderThread(ds_video_graph_);

	hr = ds_video_cap_device_->Create(videoDeviceID);
	if(FAILED( hr ))
	{
		SAFE_DELETE( ds_video_cap_device_ );
		SAFE_DELETE( ds_video_graph_ );
	}

	if( ds_video_cap_device_ )
	{
		ds_video_cap_device_->SetPreferredVideoWidth(videoFormat.width);
		ds_video_cap_device_->SetPreferredVideoHeight(videoFormat.height);
		ds_video_cap_device_->SetPreferredVideoFPS(videoFormat.fps);

		HRESULT hr = ds_video_graph_->Create( ds_video_cap_device_ );
		if(FAILED(hr))
		{
			return;
		}

		video_encoder_thread_->SetOutputFilename(CStringToString(videoOutname));
	}
}

void FDSCapture::Create(const CString& audioDeviceID, const CString& videoDeviceID)
{
	HRESULT hr;

	// audio

	

	// video

	ds_video_graph_ = new FDSVideoGraph;
	ds_video_cap_device_ = new FDSVideoCaptureDevice;

	hr = ds_video_cap_device_->Create(videoDeviceID);
	if(FAILED( hr ))
	{
		SAFE_DELETE( ds_video_cap_device_ );
		SAFE_DELETE( ds_video_graph_ );
	}

	if( ds_video_cap_device_ )
	{
		HRESULT hr = ds_video_graph_->Create( ds_video_cap_device_ );
		if(FAILED(hr))
		{
		}
	}
}

void FDSCapture::StartVideo()
{
	video_encoder_thread_->Start();
	ds_video_graph_->Start();
}

void FDSCapture::StopVideo()
{
	ds_video_graph_->Stop();
	video_encoder_thread_->Stop();
	video_encoder_thread_->Join();
}

HRESULT FDSCapture::SetVideoFormat( UINT nPreferredVideoWidth, UINT nPreferredVideoHeight, REFERENCE_TIME rtPreferredVideoFPS )
{
	StopVideo();

	ds_video_graph_->Destroy();

	// setting preferred width, height, fps
	// the video capture device's supported video format may not match it, that's
	// why these functions contain "preferred" in their names
	//
	ds_video_cap_device_->SetPreferredVideoWidth( nPreferredVideoWidth );
	ds_video_cap_device_->SetPreferredVideoHeight( nPreferredVideoHeight );
	ds_video_cap_device_->SetPreferredVideoFPS( rtPreferredVideoFPS );

	HRESULT hr = ds_video_graph_->Create( ds_video_cap_device_ );
	if( FAILED( hr ) )
	{
		return hr;
	}

	StartVideo();

	return 0;
}

void FDSCapture::StartAudio()
{
	
}

void FDSCapture::StopAudio()
{
	
}

HRESULT FDSCapture::SetAudioFormat( DWORD dwPreferredSamplesPerSec, WORD wPreferredBitsPerSample, WORD nPreferredChannels )
{
	
	return 0;
}

void FDSCapture::AdjustVideoWindow(OAHWND owner, unsigned int width, unsigned int height)
{
	ds_video_graph_->AdjustVideoWindow(owner, width, height);
}

std::string FDSCapture::CStringToString(const CString& mfcStr)
{
	CT2CA pszConvertedAnsiString(mfcStr);
	return (pszConvertedAnsiString);
}

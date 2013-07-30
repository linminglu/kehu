#pragma once
#include "fdsgraph.h"
#include "FDSVideoCaptureDevice.h"
class FDSVideoGraph :
	public FDSGraph
{
public:
	FDSVideoGraph();

	~FDSVideoGraph();

	HRESULT Create(FDSVideoCaptureDevice* videoCaptureDevice);

	void Destroy();

	void AdjustVideoWindow(OAHWND owner, unsigned int width, unsigned int height);

	UINT Width() { return video_cap_device_->GetVideoWidth(); }

	UINT Height() { return video_cap_device_->GetVideoHeight(); }

	REFERENCE_TIME FPS() { return video_cap_device_->GetVideoFPS(); }

private:
	HRESULT SetVideoFormat(UINT preferredImageWidth, UINT preferredImageHeight, 
		REFERENCE_TIME preferredFPS);

private:
	FDSVideoCaptureDevice* video_cap_device_;
	CComQIPtr<IVideoWindow, &IID_IVideoWindow> active_video_window_;
	CComPtr<IBaseFilter> splitter_;
};



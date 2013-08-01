#pragma once

#include "FDSGraph.h"

class FDSVideoGraph;
class FDSVideoCaptureDevice;
class FVideoEncoderThread;


struct DSVideoFormat
{
	unsigned int width;
	unsigned int height;
	unsigned int fps;
};

class FDSCapture
{
public:
	FDSCapture();

	~FDSCapture();

	void Create(const CString& audioDeviceID, const CString& videoDeviceID,const DSVideoFormat& videoFormat,
		const CString& audioOutname, const CString& videoOutname);

	void Create(const CString& audioDeviceID, const CString& videoDeviceID);

	void Destroy();

	HRESULT	SetVideoFormat(UINT nPreferredVideoWidth, 
		UINT nPreferredVideoHeight, REFERENCE_TIME rtPreferredVideoFPS);

	void StartVideo();

	void StopVideo();

	HRESULT	SetAudioFormat(DWORD dwPreferredSamplesPerSec,
		WORD wPreferredBitsPerSample, WORD nPreferredChannels);

	void StartAudio();

	void StopAudio();

	void AdjustVideoWindow(OAHWND owner, unsigned int width, unsigned int height);

	FDSVideoGraph* GetVideoGraph() { return ds_video_graph_; }



	FDSGraph * DShowGraph() { return ds_graph_; }

private:
	std::string CStringToString(const CString& mfcStr);

private:
	FDSGraph*		ds_graph_;

	FDSVideoCaptureDevice*	ds_video_cap_device_;
	FDSVideoGraph*			ds_video_graph_;
	FVideoEncoderThread* video_encoder_thread_;
};
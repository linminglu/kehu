#pragma once
#include <string>
#include <control.h>
#include <map>
using namespace std;

class FDSGraph;
class FDSVideoGraph;
class FDSVideoCaptureDevice;
class FVideoEncoderThread;
struct FDSVideoFormat
{
	unsigned int width;
	unsigned int height;
	unsigned int fps;
};

class FDSCaptureManager
{
public:
	FDSCaptureManager();

	~FDSCaptureManager();

	void Create(const CString& videoDeviceID,
		const FDSVideoFormat& videoFormat,
		const CString& videoOutname);

	void Create(const CString& videoDeviceID);

	void Destroy();

	HRESULT	SetVideoFormat(UINT nPreferredVideoWidth, 
		UINT nPreferredVideoHeight, REFERENCE_TIME rtPreferredVideoFPS,CString str);

	void StartVideo(CString str);

	void StopVideo(CString str);

	void AdjustVideoWindow(OAHWND owner, unsigned int width, unsigned int height,CString str);

	FDSVideoGraph* GetVideoGraph(CString str) { return m_ds_video_graph_[str]; }
	FDSGraph * DShowGraph() { return ds_graph_; }

private:
	std::string CStringToString(const CString& mfcStr);
	void deleteMapBy(CString str);
private:
	FDSGraph*		ds_graph_;
	map<CString ,FDSVideoCaptureDevice*>m_ds_video_cap_device_;
	map<CString, FDSVideoGraph*	>m_ds_video_graph_;
	map<CString, FVideoEncoderThread*>m_video_encoder_thread_;
};

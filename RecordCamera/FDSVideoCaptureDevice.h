#pragma once
#include "fdscapturedevice.h"
class FDSVideoCaptureDevice : //��ͬ���豸�ж�������
	public FDSCaptureDevice
{
public:
	FDSVideoCaptureDevice();

	~FDSVideoCaptureDevice();

	HRESULT Create(const CString& comObjID);

	// -----------------------------------------------------------------------
	// set��get���ݳ�Ա

	void SetVideoWidth(UINT videoWidth) { video_width_ = videoWidth; }
	const UINT& GetVideoWidth() { return video_width_; }

	void SetVideoHeight(UINT videoHeight) { video_height_ = videoHeight; }
	const UINT& GetVideoHeight() { return video_height_; }

	void SetVideoFPS(REFERENCE_TIME videoFPS) { video_fps_ = videoFPS; }
	const REFERENCE_TIME& GetVideoFPS() { return video_fps_; }

	void SetPreferredVideoWidth(UINT preferredWidth) { preferred_image_width_ = preferredWidth; }
	const UINT& GetPreferredVideoWidth() { return preferred_image_width_; }

	void SetPreferredVideoHeight(UINT preferredHeight) { preferred_image_height_ = preferredHeight; }
	const UINT& GetPreferredVideoHeight() { return preferred_image_height_; }

	void SetPreferredVideoFPS(REFERENCE_TIME preferredFPS) { preferred_fps_ = preferredFPS; }
	const REFERENCE_TIME& GetPreferredVideoFPS() { return preferred_fps_; }

private:
	UINT video_width_;
	UINT video_height_;
	REFERENCE_TIME video_fps_;

	UINT preferred_image_width_;
	UINT preferred_image_height_;
	REFERENCE_TIME preferred_fps_;
};


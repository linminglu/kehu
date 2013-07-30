#include "stdafx.h"
#include "FDSVideoCaptureDevice.h"


FDSVideoCaptureDevice::FDSVideoCaptureDevice()
{
	video_width_ = 0;
	video_height_ = 0;
	video_fps_ = 0;

	preferred_image_width_ = 0;
	preferred_image_height_ = 0;
	preferred_fps_ = 0;
}

FDSVideoCaptureDevice::~FDSVideoCaptureDevice()
{

}

HRESULT FDSVideoCaptureDevice::Create(const CString& comObjID)
{
	return FDSCaptureDevice::Create(comObjID);
}

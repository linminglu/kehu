#pragma once
#include "FCSimpleThread.h"
#include <string>
using namespace std;
class FH264Encode;
class FDSVideoGraph;
class FVideoEncoderThread :
	public EmrysTool::FCSimpleThread
{
public:
	FVideoEncoderThread(FDSVideoGraph* dsVideoGraph);

	~FVideoEncoderThread();
	virtual void Run();

	void SetOutputFilename(const std::string& filename);

private:
	bool RGB2YUV420(LPBYTE RgbBuf,UINT nWidth,UINT nHeight,LPBYTE yuvBuf,unsigned long *len);

private:
	FDSVideoGraph* ds_video_graph_;
	FH264Encode* x264_encoder_;
	std::string filename_264_;
};


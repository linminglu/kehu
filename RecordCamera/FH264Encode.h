#pragma once

#include <cstdint>
extern "C"
{
#include "x264/x264.h"
}

struct TNAL
{
	int size;
	unsigned char* data;
	TNAL(): size(0), data(NULL) {}
};

class FH264Encode
{
public:
	FH264Encode();

	~FH264Encode();

	// 初始化编码器
	int Initialize(int iWidth, int iHeight, int iRateBit = 96, int iFps = 25);

	// 对一帧对象进行编码，返回NAL数组
	int Encode(unsigned char* szYUVFrame, unsigned char* outBuf, int& outLen, bool& isKeyframe);

	// 销毁NAL数组
	void CleanNAL(TNAL* pNALArray, int iNalNum);

	// 销毁编码器
	int Destroy();

private:
	x264_param_t m_param;
	x264_picture_t m_pic;
	x264_t* m_h;
};


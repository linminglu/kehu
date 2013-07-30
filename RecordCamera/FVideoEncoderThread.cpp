#include "stdafx.h"
#include "FVideoEncoderThread.h"

#include "FDSVideoGraph.h"
#include "FH264Encode.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
FVideoEncoderThread::FVideoEncoderThread(FDSVideoGraph* dsVideoGraph)
	: ds_video_graph_(dsVideoGraph), x264_encoder_(new FH264Encode)
{

}

FVideoEncoderThread::~FVideoEncoderThread()
{
	delete x264_encoder_;
}

void FVideoEncoderThread::Run()
{
	// 初始化工作
	x264_encoder_->Initialize(ds_video_graph_->Width(), ds_video_graph_->Height(), 200, 10);
	FILE* fp_264 = 0;
	FILE* fp_yuv = 0;
	FILE* fp_log = 0;
	if (false == filename_264_.empty())
	{
		 fopen_s(&fp_264,filename_264_.c_str(), "wb");
	}

	// 开始循环获取每一帧，并编码
	unsigned long yuvimg_size = ds_video_graph_->Width() * ds_video_graph_->Height() * 3 / 2;
	unsigned char* yuvbuf = (unsigned char*)malloc(yuvimg_size);
	unsigned char* x264buf = (unsigned char*)malloc(yuvimg_size*10);
	int x264buf_len = 0;

	while (false == IsStop())
	{
		char* rgbbuf = ds_video_graph_->GetBuffer();
		unsigned int now_tick = ::GetTickCount();
		unsigned int next_tick = now_tick + 100;
		if (rgbbuf)
		{
			//mark
			RGB2YUV420((LPBYTE)rgbbuf, ds_video_graph_->Width(),  ds_video_graph_->Height(),
				(LPBYTE)yuvbuf,  &yuvimg_size);

			bool is_keyframe = false;
			x264_encoder_->Encode(yuvbuf, x264buf, x264buf_len, is_keyframe);

			if (fp_264 && x264buf_len > 0)
			{
				fwrite(x264buf, x264buf_len, 1, fp_264);
			}
		}
		ds_video_graph_->ReleaseBuffer(rgbbuf);

		//Sleep(1000/ds_video_graph_->FPS());
		now_tick = ::GetTickCount();
		if (next_tick > now_tick)
		{
			Sleep(next_tick-now_tick);
		}
	}
	free(yuvbuf);

	if (fp_264)
	{
		fclose(fp_264);
	}
}

void  FVideoEncoderThread::SetOutputFilename(const std::string& filename)
{
	filename_264_ = filename;
}

bool FVideoEncoderThread::RGB2YUV420(LPBYTE RgbBuf,UINT nWidth,UINT nHeight,LPBYTE yuvBuf,unsigned long *len)
{
	int i, j; 
	unsigned char*bufY, *bufU, *bufV, *bufRGB,*bufYuv; 
	//memset(yuvBuf,0,(unsigned int )*len);
	bufY = yuvBuf; 
	bufV = yuvBuf + nWidth * nHeight; 
	bufU = bufV + (nWidth * nHeight* 1/4); 
	*len = 0; 
	unsigned char y, u, v, r, g, b,testu,testv; 
	unsigned int ylen = nWidth * nHeight;
	unsigned int ulen = (nWidth * nHeight)/4;
	unsigned int vlen = (nWidth * nHeight)/4; 
	for (j = 0; j<nHeight;j++)
	{
		bufRGB = RgbBuf + nWidth * (nHeight - 1 - j) * 3 ; 
		for (i = 0;i<nWidth;i++)
		{
			int pos = nWidth * i + j;
			r = *(bufRGB++);
			g = *(bufRGB++);
			b = *(bufRGB++);

			y = (unsigned char)( ( 66 * r + 129 * g + 25 * b + 128) >> 8) + 16 ;           
			u = (unsigned char)( ( -38 * r - 74 * g + 112 * b + 128) >> 8) + 128 ;           
			v = (unsigned char)( ( 112 * r - 94 * g - 18 * b + 128) >> 8) + 128 ;
			*(bufY++) = max( 0, min(y, 255 ));

			if (j%2==0&&i%2 ==0)
			{
				if (u>255)
				{
					u=255;
				}
				if (u<0)
				{
					u = 0;
				}
				*(bufU++) =u;
				//存u分量
			}
			else
			{
				//存v分量
				if (i%2==0)
				{
					if (v>255)
					{
						v = 255;
					}
					if (v<0)
					{
						v = 0;
					}
					*(bufV++) =v;

				}
			}

		}

	}
	*len = nWidth * nHeight+(nWidth * nHeight)/2;
	return true;
}

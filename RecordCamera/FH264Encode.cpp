#include "stdafx.h"
#include "FH264Encode.h"


void *my_malloc( int i_size )
{
#ifdef SYS_MACOSX
	/* Mac OS X always returns 16 bytes aligned memory */
	return malloc( i_size );
#elif defined( HAVE_MALLOC_H )
	return memalign( 16, i_size );
#else
	uint8_t * buf;
	uint8_t * align_buf;
	buf = (uint8_t *) malloc( i_size + 15 + sizeof( void ** ) +
		sizeof( int ) );
	align_buf = buf + 15 + sizeof( void ** ) + sizeof( int );
	align_buf -= (intptr_t) align_buf & 15;
	*( (void **) ( align_buf - sizeof( void ** ) ) ) = buf;
	*( (int *) ( align_buf - sizeof( void ** ) - sizeof( int ) ) ) = i_size;
	return align_buf;
#endif
}

void my_free( void *p )
{
	if( p )
	{
#if defined( HAVE_MALLOC_H ) || defined( SYS_MACOSX )
		free( p );
#else
		free( *( ( ( void **) p ) - 1 ) );
#endif
		p = NULL;
	}
}

FH264Encode::FH264Encode()
{
	m_h = NULL;
	x264_param_default(&m_param);
}

FH264Encode::~FH264Encode()
{
	Destroy();
}

int FH264Encode::Initialize(int iWidth, int iHeight, int iRateBit, int iFps)
{
	m_param.i_width = iWidth;
	m_param.i_height = iHeight;

	m_param.i_fps_num = iFps;
	m_param.i_fps_den = 1;

	m_param.rc.i_bitrate = iRateBit;
	m_param.rc.i_rc_method = X264_RC_ABR;

	m_param.i_frame_reference = 2; /* 参考帧的最大帧数 */
	//m_param.i_keyint_max = 8;
	//m_param.i_keyint_min = 4;
	m_param.i_frame_total = 0;

	m_param.i_threads = 1;

	/* 根据输入参数param初始化总结构 x264_t *h     */
	if( ( m_h = x264_encoder_open( &m_param ) ) == NULL )
	{
		//fprintf( stderr, "x264 [error]: x264_encoder_open failed\n" );

		return -1;
	}

	x264_picture_alloc( &m_pic, X264_CSP_I420, m_param.i_width, m_param.i_height );
	m_pic.i_type = X264_TYPE_AUTO;
	m_pic.i_qpplus1 = 0;

	return 0;
}

int FH264Encode::Destroy()
{
	x264_picture_clean( &m_pic );

	// TODO: clean m_h
	x264_encoder_close  (m_h); //?????

	return 0;
}

//FILE* ff1 ;
int FH264Encode::Encode(unsigned char* szYUVFrame, unsigned char* outBuf, int& outLen, bool& isKeyframe)
{
	// 可以优化为m_pic中保存一个指针,直接执行szYUVFrame
	memcpy(m_pic.img.plane[0], szYUVFrame, m_param.i_width * m_param.i_height*3 / 2);
	//m_pic.img.plane[0] = szYUVFrame;

	m_param.i_frame_total++;
	m_pic.i_pts = (int64_t)m_param.i_frame_total * m_param.i_fps_den;

	x264_picture_t pic_out;
	x264_nal_t *nal=0;
	int i_nal, i; // nal的个数

	if( x264_encoder_encode( m_h, &nal, &i_nal, &m_pic, &pic_out ) < 0 )
	{
		//fprintf( stderr, "x264 [error]: x264_encoder_encode failed\n" );
		return -1;
	}

	outLen = 0;
	for( i = 0; i < i_nal; i++ )
	{
		int i_size = 0;
		x264_nal_encode(outBuf+outLen, &i_size, 1, &nal[i] );
		outLen += i_size;
	}

	isKeyframe = (pic_out.i_type == X264_TYPE_IDR);

	return 0;
}

void FH264Encode::CleanNAL(TNAL* pNALArray, int iNalNum)
{
	for(int i = 0; i < iNalNum; i++)
	{
		delete []pNALArray[i].data;
		pNALArray[i].data = NULL;
	}
	delete []pNALArray;
	pNALArray = NULL;
}

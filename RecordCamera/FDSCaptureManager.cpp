#include "stdafx.h"
#include "FDSGraph.h"
#include "FDSVideoGraph.h"
#include "FDSCaptureManager.h"
#include "FDSCaptureDevice.h"
#include "FDSVideoCaptureDevice.h"
#include "FVideoEncoderThread.h"

FDSCaptureManager::FDSCaptureManager()
{
	// initializing a directshow filtergraph
	//
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	ds_graph_ = new FDSGraph;
	m_ds_video_graph_.clear();// = NULL;
	m_ds_video_cap_device_.clear();// = NULL;
	m_video_encoder_thread_.clear();// = NULL;
}

FDSCaptureManager::~FDSCaptureManager()
{
	Destroy();
	if( ds_graph_ )
	{
		delete ds_graph_;
		ds_graph_=NULL;
	}
	CoUninitialize();
}

void FDSCaptureManager::Destroy()
{

	for (map<CString,FDSVideoGraph*>::iterator it=m_ds_video_graph_.begin();it!=m_ds_video_graph_.end();it++)
	{
		if((*it).second)
		{

			delete (*it).second;
			(*it).second=NULL;
		}
	}
	m_ds_video_graph_.clear();
	for (map<CString,FDSVideoCaptureDevice*>::iterator it=m_ds_video_cap_device_.begin();it!=m_ds_video_cap_device_.end();it++)
	{
		if((*it).second)
		{

			delete (*it).second;
			(*it).second=NULL;
		}
	}
	m_ds_video_cap_device_.clear();
	for (map<CString,FVideoEncoderThread *>::iterator it=m_video_encoder_thread_.begin();it!=m_video_encoder_thread_.end();it++)
	{
		if((*it).second)
		{

			delete (*it).second;
			(*it).second=NULL;
		}
	}
}
void FDSCaptureManager::deleteMapBy(CString str)
{
	for (map<CString,FDSVideoGraph*>::iterator it=m_ds_video_graph_.begin();it!=m_ds_video_graph_.end();it++)
	{
		if((*it).first==str)
		{

			delete (*it).second;
			(*it).second=NULL;
			m_ds_video_graph_.erase(it);
			break;
		}
	}
	for (map<CString,FDSVideoCaptureDevice*>::iterator it=m_ds_video_cap_device_.begin();it!=m_ds_video_cap_device_.end();it++)
	{
		if((*it).first==str)
		{

			delete (*it).second;
			(*it).second=NULL;
			m_ds_video_cap_device_.erase(it);
			break;
		}
	}
	for (map<CString,FVideoEncoderThread *>::iterator it=m_video_encoder_thread_.begin();it!=m_video_encoder_thread_.end();it++)
	{
		if((*it).first==str)
		{

			delete (*it).second;
			(*it).second=NULL;
			m_video_encoder_thread_.erase(it);
			break;
		}
	}

}
void FDSCaptureManager::Create( const CString& videoDeviceID,
							   const FDSVideoFormat& videoFormat, const CString& videoOutname)
{
	HRESULT hr;

	FDSVideoGraph *tempds_video_graph_ = new FDSVideoGraph;
	FDSVideoCaptureDevice *tempds_video_cap_device_ = new FDSVideoCaptureDevice;
	hr = tempds_video_cap_device_->Create(videoDeviceID);
	if(FAILED( hr ))
	{
		if(tempds_video_graph_)
		{
			delete tempds_video_graph_;
			tempds_video_graph_=NULL;
		}
		if(tempds_video_cap_device_)
		{
			delete tempds_video_cap_device_;
			tempds_video_cap_device_=NULL;

		}
		return;
	}
	FVideoEncoderThread *video_encoder_thread_ = new FVideoEncoderThread(tempds_video_graph_);
	m_ds_video_graph_[videoDeviceID]=tempds_video_graph_;
	m_ds_video_cap_device_[videoDeviceID]=tempds_video_cap_device_;
	m_video_encoder_thread_[videoDeviceID]=video_encoder_thread_;

	if( tempds_video_cap_device_ )
	{
		tempds_video_cap_device_->SetPreferredVideoWidth(videoFormat.width);
		tempds_video_cap_device_->SetPreferredVideoHeight(videoFormat.height);
		tempds_video_cap_device_->SetPreferredVideoFPS(videoFormat.fps);

		HRESULT hr = tempds_video_graph_->Create( tempds_video_cap_device_ );
		if(FAILED(hr))
		{
			return;
		}
		string strFile=CStringToString(videoOutname);
		//strFile+=CStringToString(videoDeviceID);
		if(m_video_encoder_thread_[videoDeviceID])
		{
			m_video_encoder_thread_[videoDeviceID]->SetOutputFilename(strFile);
		}

	}
}

void FDSCaptureManager::Create(const CString& videoDeviceID)
{
	// 	HRESULT hr;
	// 
	// 	// audio
	// 
	// 	ds_audio_graph_ = new DSAudioGraph;
	// 	ds_audio_cap_device_ = new DSAudioCaptureDevice;
	// 
	// 	hr = ds_audio_cap_device_->Create(audioDeviceID);
	// 	if( FAILED( hr ) )
	// 	{
	// 		SAFE_DELETE( ds_audio_cap_device_ );
	// 		SAFE_DELETE( ds_audio_graph_ );
	// 	}
	// 	if( ds_audio_cap_device_ )
	// 	{
	// 		HRESULT hr = ds_audio_graph_->Create( ds_audio_cap_device_ );
	// 		if( FAILED( hr ) )
	// 		{
	// 
	// 		}
	// 	}
	// 
	// 	// video
	// 
	// 	ds_video_graph_ = new DSVideoGraph;
	// 	ds_video_cap_device_ = new DSVideoCaptureDevice;
	// 
	// 	hr = ds_video_cap_device_->Create(videoDeviceID);
	// 	if(FAILED( hr ))
	// 	{
	// 		SAFE_DELETE( ds_video_cap_device_ );
	// 		SAFE_DELETE( ds_video_graph_ );
	// 	}
	// 
	// 	if( ds_video_cap_device_ )
	// 	{
	// 		HRESULT hr = ds_video_graph_->Create( ds_video_cap_device_ );
	// 		if(FAILED(hr))
	// 		{
	// 		}
	// 	}
}

void FDSCaptureManager::StartVideo(CString str)
{
	if(m_video_encoder_thread_[str])
	{
		m_video_encoder_thread_[str]->Start();
	}
	if(m_ds_video_graph_[str])
	{
		m_ds_video_graph_[str]->Start();
	}	
}

void FDSCaptureManager::StopVideo(CString str)
{
	if(m_ds_video_graph_[str])
	{
		m_ds_video_graph_[str]->Stop();
	}
	if(m_video_encoder_thread_[str])
	{
		m_video_encoder_thread_[str]->Stop();
	}
	if(m_video_encoder_thread_[str])
	{
		m_video_encoder_thread_[str]->Join();
	}


}

HRESULT FDSCaptureManager::SetVideoFormat( UINT nPreferredVideoWidth, UINT nPreferredVideoHeight, REFERENCE_TIME rtPreferredVideoFPS ,CString str)
{
	StopVideo(str);
	if(m_ds_video_graph_[str])
	{
		m_ds_video_graph_[str]->Destroy();
	}



	if(m_ds_video_cap_device_[str])
	{
		m_ds_video_cap_device_[str]->SetPreferredVideoWidth( nPreferredVideoWidth );

		m_ds_video_cap_device_[str]->SetPreferredVideoHeight( nPreferredVideoHeight );

		m_ds_video_cap_device_[str]->SetPreferredVideoFPS( rtPreferredVideoFPS );
	}



	HRESULT hr = m_ds_video_graph_[str]->Create( m_ds_video_cap_device_[str] );
	if( FAILED( hr ) )
	{
		return hr;
	}

	StartVideo(str);

	return 0;
}


void FDSCaptureManager::AdjustVideoWindow(OAHWND owner, unsigned int width, unsigned int height,CString str)
{
	if(m_ds_video_graph_[str])
	{
		m_ds_video_graph_[str]->AdjustVideoWindow(owner, width, height);
	}

}

std::string FDSCaptureManager::CStringToString(const CString& mfcStr)
{
	CT2CA pszConvertedAnsiString(mfcStr);
	return (pszConvertedAnsiString);
}

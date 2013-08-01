#include "stdafx.h"
#include "FDSVideoGraph.h"
#include "./DirectshowInterface/mtype.h"
FDSVideoGraph::FDSVideoGraph()
{

}

FDSVideoGraph::~FDSVideoGraph()
{
	Destroy();
}

HRESULT FDSVideoGraph::Create(FDSVideoCaptureDevice* videoCaptureDevice)
{
	HRESULT hr;

	video_cap_device_ = videoCaptureDevice;

	// creating the smart tee splitter
	//
	splitter_.CoCreateInstance( CLSID_SmartTee );

	if( !splitter_ )
	{
		// can't create Smart Tee Splitter
		return -1;
	}
	CComQIPtr< IBaseFilter, &IID_IBaseFilter > splitter_filter_( splitter_ );

	// add the splitter to the graph
	//
	graph_builder_->AddFilter( splitter_filter_, L"Splitter" );


	// creating the sample grabber
	//
	grabber_.CoCreateInstance( CLSID_SampleGrabber );

	if( !grabber_ )
	{
		// can't create ISampleGrabber
		return -2;
	}

	CComQIPtr< IBaseFilter, &IID_IBaseFilter > grabber_filter_( grabber_ );

	// add the grabber to the graph
	//
	graph_builder_->AddFilter( grabber_filter_, L"Grabber" );

	// create the source filter (pSourceFilter)

	source_filter_ = video_cap_device_->GetBaseFilter();
	CComQIPtr< IBaseFilter, &IID_IBaseFilter > cap_filter_( source_filter_ );

	// add video capture source to the graph
	//
	graph_builder_->AddFilter( cap_filter_, L"Source" );
	cap_filter_.Release();

	// tell the grabber to grab 24-bit video
	//
	CMediaType media_type;
	media_type.SetType( &MEDIATYPE_Video );
	media_type.SetSubtype( &MEDIASUBTYPE_RGB24 );
	hr = grabber_->SetMediaType( &media_type );

	// setting video size
	//
	video_cap_device_->SetOutPin( GetOutPin( source_filter_, 0 ) );
	SetVideoFormat( video_cap_device_->GetPreferredVideoWidth(), 
		video_cap_device_->GetPreferredVideoHeight(),
		video_cap_device_->GetPreferredVideoFPS() );

	// connect the webcam out pin to the splitter
	//
	CComPtr< IPin > splitter_in_pin = GetInPin( splitter_filter_, 0 );
	hr = graph_builder_->Connect( video_cap_device_->GetOutPin(), splitter_in_pin );
	if( FAILED( hr ) )
	{
		// can't connect pins in filtergraph
		return -3;
	}
	splitter_in_pin.Release();

	// connect the splitter capture pin to the grabber
	//
	CComPtr< IPin > splitter_out_pin = GetOutPin( splitter_filter_, 0 );
	CComPtr< IPin > grabber_in_pin = GetInPin( grabber_filter_, 0 );
	grabber_filter_.Release();
	hr = graph_builder_->Connect( splitter_out_pin, grabber_in_pin );
	if( FAILED( hr ) )
	{
		// can't connect pins in filtergraph
		return -3;
	}
	grabber_in_pin.Release();
	splitter_out_pin.Release();

	// connect the splitter preview pin to the renderer
	//
	CComPtr< IPin > splitter_preview_out_pin = GetOutPin( splitter_filter_, 1 );
	splitter_filter_.Release();
	hr = graph_builder_->Render( splitter_preview_out_pin );
	splitter_preview_out_pin.Release();

	if( FAILED( hr ) )
	{
		// can't render image from the device
		return -4;
	}

	// don't buffer the samples as they pass through
	//
	grabber_->SetBufferSamples( FALSE );

	// not grabbing just one frame
	//
	grabber_->SetOneShot( FALSE );

	// setting callback
	//
	grabber_->SetCallback((ISampleGrabberCB*)(&grabber_callback_), 1 );

	// set render out to the activemovie window
	//
	active_video_window_ = graph_builder_;

	return 0;
}

void FDSVideoGraph::Destroy()
{
	active_video_window_.Release();
	splitter_.Release();
	grabber_.Release();
}

HRESULT FDSVideoGraph::SetVideoFormat(UINT preferredImageWidth, UINT preferredImageHeight, 
									 REFERENCE_TIME preferredFPS)
{
	// get the nearest, or exact video size the user wants
	//
	IEnumMediaTypes *enum_media_types;
	AM_MEDIA_TYPE *pmt = NULL, *pfnt = NULL;

	HRESULT hr = video_cap_device_->GetOutPin()->EnumMediaTypes( &enum_media_types );

	if( SUCCEEDED( hr ) )
	{
		// going through all available video formats until we find the correct one
		//
		while( enum_media_types->Next( 1, &pmt, 0 ) == S_OK )
		{
			if ( ( pmt->formattype == FORMAT_VideoInfo ) && 
				( pmt->cbFormat == sizeof(VIDEOINFOHEADER) ) )
			{
				VIDEOINFOHEADER *vih = (VIDEOINFOHEADER *)pmt->pbFormat;
				if( (UINT)vih->bmiHeader.biWidth == preferredImageWidth 
					&& (UINT)vih->bmiHeader.biHeight == preferredImageHeight )
				{
					pfnt = pmt;

					// found correct video size
					//
					break;
				}
				DeleteMediaType( pmt );
			}
		}

		if( enum_media_types )
		{
			enum_media_types->Release();
			enum_media_types=NULL;
		}

		if (!pfnt)
		{
			ICaptureGraphBuilder2* cap_graph_builder;
			CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, 
				IID_ICaptureGraphBuilder2, (void**)&cap_graph_builder);

			IAMStreamConfig* capcfg;
			cap_graph_builder->FindInterface(&PIN_CATEGORY_CAPTURE,
				&MEDIATYPE_Video, source_filter_,
				IID_IAMStreamConfig, (void **)&capcfg);
			capcfg->GetFormat(&pfnt);
		}
	}

	// set the video size
	//
	CComPtr<IAMStreamConfig> stream_cfg;
	hr = video_cap_device_->GetOutPin()->QueryInterface( IID_IAMStreamConfig, (void **) &stream_cfg );
	if( SUCCEEDED( hr ) )
	{
		if( pfnt != NULL )
		{
			VIDEOINFOHEADER *vih = (VIDEOINFOHEADER *)pfnt->pbFormat;

			// setting fps
			//
			vih->AvgTimePerFrame = 10000000/preferredFPS;
			hr = stream_cfg->SetFormat( pfnt );

			DeleteMediaType( pfnt );
		}
		else
		{
			return -1;
		}

		// getting current video format, it may differ from the format we wanted
		// so that's way we ask for it again
		//
		hr = stream_cfg->GetFormat( &pfnt );

		if( SUCCEEDED( hr ) )
		{
			video_cap_device_->SetVideoWidth( ((VIDEOINFOHEADER *)pfnt->pbFormat)->bmiHeader.biWidth );
			video_cap_device_->SetVideoHeight( ((VIDEOINFOHEADER *)pfnt->pbFormat)->bmiHeader.biHeight );
			video_cap_device_->SetVideoFPS( 10000000/((VIDEOINFOHEADER *)pfnt->pbFormat)->AvgTimePerFrame );

			DeleteMediaType( pfnt );
		}
	}
	return 0;
}

void FDSVideoGraph::AdjustVideoWindow(OAHWND owner, unsigned int width, unsigned int height)
{
	active_video_window_->put_Visible(OAFALSE);
	active_video_window_->put_Owner(NULL);

	active_video_window_->put_Owner(owner);
	active_video_window_->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	active_video_window_->put_Visible(OATRUE);
	active_video_window_->SetWindowPosition(0, 0, width, height);
}
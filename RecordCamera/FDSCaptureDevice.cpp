#include "stdafx.h"
#include "FDSCaptureDevice.h"

#define  SAFE_RELEASE(__PTR__) if(__PTR__) {__PTR__->Release();__PTR__=NULL;}

FDSCaptureDevice::FDSCaptureDevice()
{

} 

FDSCaptureDevice::~FDSCaptureDevice()
{
	base_filter_.Release();
	out_pin_.Release();
}

HRESULT FDSCaptureDevice::Create(const CString& comObjID)
{
	HRESULT hr;

	CComPtr<ICreateDevEnum> create_dev_enum;
	create_dev_enum.CoCreateInstance(CLSID_SystemDeviceEnum);

	CComPtr<IEnumMoniker> enum_moniker;
	create_dev_enum->CreateClassEnumerator  (CLSID_VideoInputDeviceCategory, &enum_moniker, 0);

	if (!enum_moniker)
	{
		create_dev_enum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &enum_moniker, 0);

		if (!enum_moniker)
		{
			return -1;
		}
	}

	enum_moniker->Reset();

	IBindCtx* bind_ctx;
	hr = ::CreateBindCtx( 0, &bind_ctx );
	if( hr != S_OK )
	{
		return -2;
	}

	ULONG ulFetched;
	CComPtr<IMoniker> moniker;

	//
	hr = ::MkParseDisplayName( bind_ctx, comObjID, &ulFetched, &moniker );
	SAFE_RELEASE( bind_ctx );

	if( hr != S_OK )
	{
		return -2;
	}

	hr = moniker->BindToObject(0,0,IID_IBaseFilter, (void **)&base_filter_);
	if( hr != S_OK )
	{
		return -2;
	}
	CComPtr< IPropertyBag > pBag;
	hr = moniker->BindToStorage( 0, 0, IID_IPropertyBag, (void**) &pBag );
	if( hr != S_OK )
	{
		return -2;
	}

	CComVariant var;
	var.vt = VT_BSTR;
	pBag->Read( L"FriendlyName", &var, NULL );

	LPOLESTR szDeviceID;
	moniker->GetDisplayName( 0, NULL, &szDeviceID );

	com_obj_id_ = comObjID;
	device_name_ = var.bstrVal;

	return 0;
}

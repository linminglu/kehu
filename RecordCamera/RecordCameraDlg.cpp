
// RecordCameraDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RecordCamera.h"
#include "RecordCameraDlg.h"
#include "afxdialogex.h"
#include "FDSGraph.h"
#include <iostream>
#include "FDCapture.h"
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
std::string gCStringToString(const CString& mfcStr)
{
	CT2CA pszConvertedAnsiString(mfcStr);
	return (pszConvertedAnsiString);
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRecordCameraDlg 对话框



CRecordCameraDlg::CRecordCameraDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRecordCameraDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ds_capture_ = new FDSCapture();
}
CRecordCameraDlg::~CRecordCameraDlg()
{
	if(ds_capture_)
	{

		delete ds_capture_;
		ds_capture_=NULL;
	}
	FreeConsole();
}

void CRecordCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRecordCameraDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_STARTRECORDONE, &CRecordCameraDlg::OnBnClickedStartrecordone)
	ON_BN_CLICKED(IDC_STOPRECORDONE, &CRecordCameraDlg::OnBnClickedStoprecordone)
	ON_BN_CLICKED(IDC_STARTRECORDTWO, &CRecordCameraDlg::OnBnClickedStartrecordtwo)
	ON_BN_CLICKED(IDC_STOPRECORDTWO, &CRecordCameraDlg::OnBnClickedStoprecordtwo)
END_MESSAGE_MAP()


// CRecordCameraDlg 消息处理程序

BOOL CRecordCameraDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	::AllocConsole();    // 打开控件台资源
	FILE *pfile;
	freopen_s(&pfile,"CONOUT$", "w+t", stdout);    // 申请写
	// TODO: 在此添加额外的初始化代码

	// 音视频设备

	std::map<CString, CString> a_devices = ds_capture_->DShowGraph()->AudioCapDevices();
	std::map<CString, CString> v_devices = ds_capture_->DShowGraph()->VideoCapDevices();

	for (std::map<CString, CString>::iterator it = v_devices.begin();
		it != v_devices.end(); ++it)
	{
		video_device_index_.push_back(it->first);
	}
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRecordCameraDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRecordCameraDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRecordCameraDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRecordCameraDlg::OnBnClickedStartrecordone()
{


CString video_device_id = video_device_index_[0];

	DSVideoFormat video_fmt;
	video_fmt.width = 640;
	video_fmt.height = 480;
	video_fmt.fps = 10;

	CWnd *pWnd = GetDlgItem(IDC_STATICONE);
	RECT rc;
	pWnd->GetClientRect(&rc);

	CString video_outname(_T("11.h264"));
	

	ds_capture_->Create(_T(""), video_device_id, video_fmt, _T(""), video_outname);
	ds_capture_->AdjustVideoWindow((OAHWND)pWnd->GetSafeHwnd(), rc.right, rc.bottom);
	ds_capture_->StartAudio();
	ds_capture_->StartVideo();


}


void CRecordCameraDlg::OnBnClickedStoprecordone()
{
	// TODO: 在此添加控件通知处理程序代码

		ds_capture_->StopVideo();
		(CButton *)GetDlgItem(IDC_STARTRECORDONE)->EnableWindow(TRUE);
}


void CRecordCameraDlg::OnBnClickedStartrecordtwo()
{
	/*
	// TODO: 在此添加控件通知处理程序代码
	if(m_vCamera.size()>0)
	{
		CString video_device_id = this->m_vCameraid[1];
		FDSVideoFormat video_fmt;
		video_fmt.width = 640;
		video_fmt.height = 480;
		video_fmt.fps = 10;

		CWnd *pWnd = GetDlgItem(IDC_STATICTWO);
		RECT rc;
		pWnd->GetClientRect(&rc);
		CString strDate;
		CTime ttime = CTime::GetCurrentTime();
		strDate.Format(_T("2-%d-%d-%d-%d-%d-%d"),ttime.GetYear(),ttime.GetMonth(),ttime.GetDay(),ttime.GetMinute(),ttime.GetMinute(),ttime.GetSecond());
		CString  video_outname;
		video_outname=strDate;

		cout<<"开始捕获"<<gCStringToString(video_device_id)<<endl;

		ds_capture_->Create(m_vCameraid[1], video_fmt, video_outname);
		ds_capture_->AdjustVideoWindow((OAHWND)pWnd->GetSafeHwnd(), rc.right, rc.bottom,video_device_id);
		
		ds_capture_->StartVideo(video_device_id);
		
		(CButton *)GetDlgItem(IDC_STARTRECORDONE)->EnableWindow(FALSE);
	}
	*/
}


void CRecordCameraDlg::OnBnClickedStoprecordtwo()
{
	// TODO: 在此添加控件通知处理程序代码
// 	if(m_vCamera.size()>1)
// 	{
// 		ds_capture_->StopVideo(m_vCameraid[1]);
// 		(CButton *)GetDlgItem(IDC_STARTRECORDONE)->EnableWindow(TRUE);
// 	}
}

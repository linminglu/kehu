
// RecordCameraDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CRecordCameraDlg �Ի���



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


// CRecordCameraDlg ��Ϣ�������

BOOL CRecordCameraDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	::AllocConsole();    // �򿪿ؼ�̨��Դ
	FILE *pfile;
	freopen_s(&pfile,"CONOUT$", "w+t", stdout);    // ����д
	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	// ����Ƶ�豸

	std::map<CString, CString> a_devices = ds_capture_->DShowGraph()->AudioCapDevices();
	std::map<CString, CString> v_devices = ds_capture_->DShowGraph()->VideoCapDevices();

	for (std::map<CString, CString>::iterator it = v_devices.begin();
		it != v_devices.end(); ++it)
	{
		video_device_index_.push_back(it->first);
	}
	

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRecordCameraDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

		ds_capture_->StopVideo();
		(CButton *)GetDlgItem(IDC_STARTRECORDONE)->EnableWindow(TRUE);
}


void CRecordCameraDlg::OnBnClickedStartrecordtwo()
{
	/*
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

		cout<<"��ʼ����"<<gCStringToString(video_device_id)<<endl;

		ds_capture_->Create(m_vCameraid[1], video_fmt, video_outname);
		ds_capture_->AdjustVideoWindow((OAHWND)pWnd->GetSafeHwnd(), rc.right, rc.bottom,video_device_id);
		
		ds_capture_->StartVideo(video_device_id);
		
		(CButton *)GetDlgItem(IDC_STARTRECORDONE)->EnableWindow(FALSE);
	}
	*/
}


void CRecordCameraDlg::OnBnClickedStoprecordtwo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
// 	if(m_vCamera.size()>1)
// 	{
// 		ds_capture_->StopVideo(m_vCameraid[1]);
// 		(CButton *)GetDlgItem(IDC_STARTRECORDONE)->EnableWindow(TRUE);
// 	}
}

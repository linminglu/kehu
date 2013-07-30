
// RecordCameraDlg.h : ͷ�ļ�
//

#pragma once

#include "FDSCaptureManager.h"
#include <vector>
using namespace std;
// CRecordCameraDlg �Ի���
class CRecordCameraDlg : public CDialogEx
{
// ����
public:
	CRecordCameraDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CRecordCameraDlg();
// �Ի�������
	enum { IDD = IDD_RECORDCAMERA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	 FDSCaptureManager* ds_capture_;
	 vector<CString>m_vCamera;
	 vector<CString>m_vCameraid;
public:
	afx_msg void OnBnClickedStartrecordone();
	afx_msg void OnBnClickedStoprecordone();
	afx_msg void OnBnClickedStartrecordtwo();
	afx_msg void OnBnClickedStoprecordtwo();
};

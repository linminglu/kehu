
// RecordCameraDlg.h : 头文件
//

#pragma once

#include "FDSCaptureManager.h"
#include <vector>
using namespace std;
// CRecordCameraDlg 对话框
class CRecordCameraDlg : public CDialogEx
{
// 构造
public:
	CRecordCameraDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CRecordCameraDlg();
// 对话框数据
	enum { IDD = IDD_RECORDCAMERA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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

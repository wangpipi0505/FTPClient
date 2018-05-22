#pragma once
#include "afxwin.h"


// CAutoUpDownLoad 对话框

class CAutoUpDownLoad : public CDialogEx
{
	DECLARE_DYNAMIC(CAutoUpDownLoad)

public:
	CAutoUpDownLoad(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAutoUpDownLoad();

// 对话框数据
	enum { IDD = IDD_DIALOG_AUTO_UPDOWN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonUpload();
	//CEdit m_up_settime;
	afx_msg void OnBnClickedOk();
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
	CEdit m_down_settime;
//	afx_msg void OnBnClickedButtonDownload();
	CEdit m_edit_downpath;
	CEdit m_edit_des_path;
	afx_msg void OnBnClickedButtonDownload();
	/*CEdit m_edit_up_hour;
	CEdit m_edit_up_minute;
	CEdit m_edit_up_second;*/
	virtual BOOL OnInitDialog();
};

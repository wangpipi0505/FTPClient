#pragma once


// CSettingServer ???��?��

class CSettingServer : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingServer)

public:
	CSettingServer(CWnd* pParent = NULL);   // ������?11?��o����y
	virtual ~CSettingServer();

// ???��?����y?Y
	enum { IDD = IDD_SETTINGSERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ?��3?

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

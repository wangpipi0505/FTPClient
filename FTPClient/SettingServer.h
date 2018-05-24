#pragma once


// CSettingServer ???¡ã?¨°

class CSettingServer : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingServer)

public:
	CSettingServer(CWnd* pParent = NULL);   // ¡À¨º¡Á?11?¨¬o¡¥¨ºy
	virtual ~CSettingServer();

// ???¡ã?¨°¨ºy?Y
	enum { IDD = IDD_SETTINGSERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ?¡ì3?

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

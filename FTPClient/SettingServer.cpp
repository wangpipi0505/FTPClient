// SettingServer.cpp : êμ?????t
//

#include "stdafx.h"
#include "FTPClient.h"
#include "SettingServer.h"
#include "afxdialogex.h"


// CSettingServer ???°?ò

IMPLEMENT_DYNAMIC(CSettingServer, CDialogEx)

CSettingServer::CSettingServer(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSettingServer::IDD, pParent)
{

}

CSettingServer::~CSettingServer()
{
}

void CSettingServer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSettingServer, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSettingServer::OnBnClickedOk)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSettingServer ???￠′|àí3ìDò


void CSettingServer::OnBnClickedOk()
{
	// TODO: ?ú′?ìí?ó???tí¨?a′|àí3ìDò′ú??
	FILE *SettingFile;
	CString Server_IP,Server_Name,Server_Passwd,Server_Port;
	TCHAR lpPath[MAX_PATH];
	TCHAR FPath[MAX_PATH];
	CWnd *ServerIP = GetDlgItem(IDC_IPADDRESS_SERVER);
	CWnd *ServerName = GetDlgItem(IDC_EDIT_USERNAME); 
	CWnd *ServerPasswd = GetDlgItem(IDC_EDIT_PASSWD); 
	CWnd *ServerPort = GetDlgItem(IDC_EDIT_PORT); 

	//CWnd *UploadTime = GetDlgItem(IDC_EDIT_UP_TIME);
	ServerIP->GetWindowText(Server_IP);
	ServerName->GetWindowText(Server_Name);
	ServerPasswd->GetWindowText(Server_Passwd);
	ServerPort->GetWindowText(Server_Port);
	
	/*::GetCurrentDirectory(MAX_PATH,lpPath);
	StrCatBuffW(lpPath,_T("\\ServerConfig.ini"),sizeof(lpPath));*/
	CString szIniPath;
	::GetModuleFileName(NULL,lpPath,MAX_PATH);
	TCHAR *pfind = _tcsrchr(lpPath,'\\');
	if (pfind != NULL)
	{
		*pfind = '\0';
		szIniPath = lpPath;
		szIniPath += "\\";
		szIniPath += _T("ServerConfig.ini");
	}
	/*::GetCurrentDirectory(MAX_PATH,FPath);
	StrCatBuffW(FPath,_T("\\ServerConfig.ini"),sizeof(FPath));*/
	/*strcpy(lpPath,"C:\\FTP\\FTPClient_0514济南\\FTPClient\\ServerConfig.ini";*/
	CFile ConfigFile;
	if(!ConfigFile.Open(szIniPath,CFile::modeRead,NULL))
	{
		AfxMessageBox(_T("打开ServerConfig.ini配置文件失败！"));
		return;
	}
	ConfigFile.Close();
	WritePrivateProfileString(_T("ServerConfig"),_T("IpAddress"),Server_IP,szIniPath);
	WritePrivateProfileString(_T("ServerConfig"),_T("UserName"),Server_Name,szIniPath);
	WritePrivateProfileString(_T("ServerConfig"),_T("PassWord"),Server_Passwd,szIniPath);
	WritePrivateProfileString(_T("ServerConfig"),_T("Port"),Server_Port,szIniPath);
	
	CDialogEx::OnOK();
}


void CSettingServer::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	 /* CWnd* pWnd=GetDlgItem(IDC_STATIC_CONNECT);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.01*cx,0.01*cy,(int)(cx*0.98),cy*0.09);*/
	CWnd* pWnd = GetDlgItem(IDC_STATIC);
	if (pWnd->GetSafeHwnd())
		pWnd->MoveWindow(0.03*cx,0.045*cy,(int)(cx*0.07),cy*0.04);
	pWnd = GetDlgItem(IDC_IPADDRESS_SERVER);
	if (pWnd->GetSafeHwnd())
		pWnd->MoveWindow(0.09*cx,0.04*cy,(int)(cx*0.1),cy*0.04);

}

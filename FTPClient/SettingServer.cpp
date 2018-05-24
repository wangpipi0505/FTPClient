// SettingServer.cpp : ¨º¦Ì?????t
//

#include "stdafx.h"
#include "FTPClient.h"
#include "SettingServer.h"
#include "afxdialogex.h"


// CSettingServer ???¡ã?¨°

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


// CSettingServer ???¡é¡ä|¨¤¨ª3¨¬D¨°


void CSettingServer::OnBnClickedOk()
{
	// TODO: ?¨²¡ä?¨¬¨ª?¨®???t¨ª¡§?a¡ä|¨¤¨ª3¨¬D¨°¡ä¨²??
	FILE *SettingFile;
	CString Server_IP,Server_Name,Server_Passwd,Server_Port;
	
	SettingFile = fopen("settingServer.txt","w+");
	CWnd *ServerIP = GetDlgItem(IDC_IPADDRESS_SERVER);
	CWnd *ServerName = GetDlgItem(IDC_EDIT_USERNAME); 
	CWnd *ServerPasswd = GetDlgItem(IDC_EDIT_PASSWD); 
	CWnd *ServerPort = GetDlgItem(IDC_EDIT_PORT); 

	//CWnd *UploadTime = GetDlgItem(IDC_EDIT_UP_TIME);
	ServerIP->GetWindowText(Server_IP);
	ServerName->GetWindowText(Server_Name);
	ServerPasswd->GetWindowText(Server_Passwd);
	ServerPort->GetWindowText(Server_Port);
	DWORD dwNum_server_ip = WideCharToMultiByte(CP_OEMCP,NULL,Server_IP,-1,NULL,NULL,0,NULL);
	char *server_ip_after = new char[dwNum_server_ip];
	WideCharToMultiByte(CP_OEMCP,NULL,Server_IP,-1,server_ip_after,dwNum_server_ip,0,NULL);
	//sprintf(server_ip_after,"%s\n",server_ip_after);
	//fprintf(SettingFile,"\n");
	fwrite(server_ip_after,strlen(server_ip_after),1,SettingFile);

	DWORD dwNum_server_name = WideCharToMultiByte(CP_OEMCP,NULL,Server_Name,-1,NULL,NULL,0,NULL);
	char *server_name_after = new char[dwNum_server_name];
	WideCharToMultiByte(CP_OEMCP,NULL,Server_Name,-1,server_name_after,dwNum_server_name,0,NULL);
	//sprintf(server_name_after,"%s\n",server_name_after);
	fprintf(SettingFile,"\n");
	fwrite(server_name_after,sizeof(server_name_after),1,SettingFile);

	DWORD dwNum_server_pass = WideCharToMultiByte(CP_OEMCP,NULL,Server_Passwd,-1,NULL,NULL,0,NULL);
	char *server_pass_after = new char[dwNum_server_pass];
	WideCharToMultiByte(CP_OEMCP,NULL,Server_Passwd,-1,server_pass_after,dwNum_server_pass,0,NULL);
	//sprintf(server_pass_after,"%s\n",server_pass_after);
	fprintf(SettingFile,"\n");
	fwrite(server_pass_after,sizeof(server_pass_after),1,SettingFile);
	
	DWORD dwNum_server_port = WideCharToMultiByte(CP_OEMCP,NULL,Server_Port,-1,NULL,NULL,0,NULL);
	char *server_port_after = new char[dwNum_server_port];
	WideCharToMultiByte(CP_OEMCP,NULL,Server_Port,-1,server_port_after,dwNum_server_port,0,NULL);
	//sprintf(server_port_after,"%s\n",server_port_after);
	fprintf(SettingFile,"\n");
	fwrite(server_port_after,sizeof(server_port_after),1,SettingFile);

	//fwrite("test",5,1,SettingFile);
	fclose(SettingFile);
	CDialogEx::OnOK();
}


void CSettingServer::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: ?¨²¡ä?¡ä|¨¬¨ª?¨®???¡é¡ä|¨¤¨ª3¨¬D¨°¡ä¨²??
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

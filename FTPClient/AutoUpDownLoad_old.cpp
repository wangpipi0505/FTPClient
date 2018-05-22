// AutoUpDownLoad.cpp : 实现文件
//

#include "stdafx.h"
#include "FTPClient.h"
#include "AutoUpDownLoad.h"
#include "afxdialogex.h"
#include "ccurl.h"
#include "outctrl.h"
#include "FTPClientDlg.h"

// CAutoUpDownLoad 对话框
extern char *user_local;//用户名
extern char *pass_local;//密码
extern char *host_local;//端口
int time_up,time_down;//定时上传下载时间
//extern CListBox m_list_log;//quanjuLIST
IMPLEMENT_DYNAMIC(CAutoUpDownLoad, CDialogEx)
	//CArray<CString, CString> TmpUploadFolderPath;//定时上传文件夹路径保存
CString TmpUploadFolderPath;//定时上传文件夹路径保存
CString TmpDownloadFolderPath;//定时下载
CString Upload_forder_path[MAX_PATH];//上传
CString Download_forder_path[MAX_PATH];//下载
CString UploadSetTime;//定时上传时间设置
CString DownloadSetTime;//定是下载时间设置
CString DownloadFolderPath;//定时下载文件夹路径
CString up_hour,up_minute,up_second;//上传开启时间
CString down_hour,down_minute,down_second;//下载开启时间

CAutoUpDownLoad::CAutoUpDownLoad(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAutoUpDownLoad::IDD, pParent)
{

}

CAutoUpDownLoad::~CAutoUpDownLoad()
{
}

void CAutoUpDownLoad::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_UP_TIME, m_up_settime);
	DDX_Control(pDX, IDC_EDIT_DOWN_TIME, m_down_settime);
	DDX_Control(pDX, IDC_EDIT_DOWN_PATH, m_edit_downpath);
	DDX_Control(pDX, IDC_EDIT_DES_PATH, m_edit_des_path);
	DDX_Control(pDX, IDC_EDIT_UP_HOUR, m_edit_up_hour);
	DDX_Control(pDX, IDC_EDIT_UP_MINUTE, m_edit_up_minute);
	DDX_Control(pDX, IDC_EDIT_UP_SECOND, m_edit_up_second);
}


BEGIN_MESSAGE_MAP(CAutoUpDownLoad, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD, &CAutoUpDownLoad::OnBnClickedButtonUpload)
	ON_BN_CLICKED(IDOK, &CAutoUpDownLoad::OnBnClickedOk)
	ON_WM_TIMER()
//	ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CAutoUpDownLoad::OnBnClickedButtonDownload)
ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CAutoUpDownLoad::OnBnClickedButtonDownload)
END_MESSAGE_MAP()


// CAutoUpDownLoad 消息处理程序


void CAutoUpDownLoad::OnBnClickedButtonUpload()
{
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO bi;   
	bi.hwndOwner = m_hWnd;   
	bi.pidlRoot = NULL;   
	bi.pszDisplayName = (LPWSTR)Upload_forder_path;   
	bi.lpszTitle = _T("请选择需要打包的目录：");   
	bi.ulFlags = 0;   
	bi.lpfn = NULL;   
	bi.lParam = 0;   
	bi.iImage = 0;   
	//弹出选择目录对话框
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);  
	if(lp && SHGetPathFromIDList(lp, (LPWSTR)Upload_forder_path))   
	{
		TmpUploadFolderPath.Format(_T("%s"), Upload_forder_path);
		//AfxMessageBox(str);
	}
	else   
		AfxMessageBox(_T("无效的目录，请重新选择"));  

	CWnd *UploadFolderPath = GetDlgItem(IDC_EDIT_UP_PATH); 
	CWnd *UploadTime = GetDlgItem(IDC_EDIT_UP_TIME);
	UploadFolderPath->SetWindowTextW(TmpUploadFolderPath);
	m_up_settime.GetWindowText(UploadSetTime);
	//AfxMessageBox(UploadSetTime);

}

void CAutoUpDownLoad::OnBnClickedButtonDownload()
{
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO bi;   
	bi.hwndOwner = m_hWnd;   
	bi.pidlRoot = NULL;   
	bi.pszDisplayName = (LPWSTR)Download_forder_path;   
	bi.lpszTitle = _T("请选择需要打包的目录：");   
	bi.ulFlags = 0;   
	bi.lpfn = NULL;   
	bi.lParam = 0;   
	bi.iImage = 0;   
	//弹出选择目录对话框
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);  
	if(lp && SHGetPathFromIDList(lp, (LPWSTR)Download_forder_path))   
	{
		TmpDownloadFolderPath.Format(_T("%s"), Download_forder_path);
		//AfxMessageBox(str);
	}
	else   
		AfxMessageBox(_T("无效的目录，请重新选择"));  

	CWnd *DownloadDestPath = GetDlgItem(IDC_EDIT_DES_PATH); 
	//CWnd *UploadTime = GetDlgItem(IDC_EDIT_UP_TIME);
	DownloadDestPath->SetWindowTextW(TmpDownloadFolderPath);
	m_down_settime.GetWindowText(DownloadSetTime);
}

void CAutoUpDownLoad::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	HANDLE Thread_RecvUdp; 

	//CFTPClientDlg ClientDlg;
	if((TmpUploadFolderPath.GetLength()!= 0)&&(UploadSetTime.GetLength()!=0))
	{
		time_up = _ttoi(UploadSetTime);
	}
	//m_down_settime.GetWindowText(DownloadSetTime);
	m_edit_downpath.GetWindowText(DownloadFolderPath);
	if((DownloadSetTime.GetLength()!=0)&&(DownloadFolderPath.GetLength()!=0)&&(TmpDownloadFolderPath.GetLength()!=0))
	{
		time_down = _ttoi(DownloadSetTime);
	}
	m_edit_up_hour.GetWindowText(up_hour);
	m_edit_up_minute.GetWindowText(up_minute);
	m_edit_up_second.GetWindowText(up_second);
	CDialogEx::OnOK();
}


//void CAutoUpDownLoad::AutoUpload()//定时上传
//{
//	CCurl * pCurl = CCurl::Instance();
//	CString strSetUploadFilePath;//文件名加路径
//	CString str = TmpUploadFolderPath; //上传路径
//	CString strfilename;
//	CWnd *List = GetDlgItem(IDC_LIST_LOG);//插入日志
//	if(str.Right(1) != '\\')  
//		str += '\\';  
//	str += "*.*";  
//	CFileFind file;  
//	BOOL bContinue = file.FindFile(str);  //find 子文件
//	while(bContinue)  
//	{  
//		bContinue = file.FindNextFile();  
//		if(!file.IsDirectory() && !file.IsDots())  
//		{  
//			SHFILEINFO info;  
//			CString temp = str; //路径 
//			int index = temp.Find(L"*.*");  
//			temp.Delete(index,3);  
//			strfilename = file.GetFileName();
//			CString strSetUploadFilePath = temp + strfilename;//获取的文件名加路径
//		}
//		pCurl->setHostUserPwd(user_local, pass_local, host_local);
//
//		DWORD dwNum_path = WideCharToMultiByte(CP_ACP,NULL,strSetUploadFilePath,-1,NULL,NULL,0,NULL);
//		char *local_path_after = new char[dwNum_path];
//		WideCharToMultiByte(CP_OEMCP,NULL,strSetUploadFilePath,-1,local_path_after,dwNum_path,0,NULL);
//
//		DWORD dwNum_file_name = WideCharToMultiByte(CP_ACP,NULL,strfilename,-1,NULL,NULL,0,NULL);
//		char *local_file_name_after = new char[dwNum_file_name];
//		WideCharToMultiByte(CP_OEMCP,NULL,strfilename,-1,local_file_name_after,dwNum_file_name,0,NULL);
//
//		if (pCurl->upload(local_path_after,local_file_name_after))
//		{
//			//CLOG("upload ok");
//
//			//m_list_log.InsertString(0,_T("上传成功"));
//			MessageBox(_T("upload ok"),_T("状态"),MB_OK);
//
//		}
//		else 
//		{
//			//m_list_log.InsertString(0,_T("上传失败"));
//			MessageBox(_T("host"),_T("状态"),MB_OK);
//		}
//	}
//}





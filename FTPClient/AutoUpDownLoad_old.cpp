// AutoUpDownLoad.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FTPClient.h"
#include "AutoUpDownLoad.h"
#include "afxdialogex.h"
#include "ccurl.h"
#include "outctrl.h"
#include "FTPClientDlg.h"

// CAutoUpDownLoad �Ի���
extern char *user_local;//�û���
extern char *pass_local;//����
extern char *host_local;//�˿�
int time_up,time_down;//��ʱ�ϴ�����ʱ��
//extern CListBox m_list_log;//quanjuLIST
IMPLEMENT_DYNAMIC(CAutoUpDownLoad, CDialogEx)
	//CArray<CString, CString> TmpUploadFolderPath;//��ʱ�ϴ��ļ���·������
CString TmpUploadFolderPath;//��ʱ�ϴ��ļ���·������
CString TmpDownloadFolderPath;//��ʱ����
CString Upload_forder_path[MAX_PATH];//�ϴ�
CString Download_forder_path[MAX_PATH];//����
CString UploadSetTime;//��ʱ�ϴ�ʱ������
CString DownloadSetTime;//��������ʱ������
CString DownloadFolderPath;//��ʱ�����ļ���·��
CString up_hour,up_minute,up_second;//�ϴ�����ʱ��
CString down_hour,down_minute,down_second;//���ؿ���ʱ��

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


// CAutoUpDownLoad ��Ϣ�������


void CAutoUpDownLoad::OnBnClickedButtonUpload()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO bi;   
	bi.hwndOwner = m_hWnd;   
	bi.pidlRoot = NULL;   
	bi.pszDisplayName = (LPWSTR)Upload_forder_path;   
	bi.lpszTitle = _T("��ѡ����Ҫ�����Ŀ¼��");   
	bi.ulFlags = 0;   
	bi.lpfn = NULL;   
	bi.lParam = 0;   
	bi.iImage = 0;   
	//����ѡ��Ŀ¼�Ի���
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);  
	if(lp && SHGetPathFromIDList(lp, (LPWSTR)Upload_forder_path))   
	{
		TmpUploadFolderPath.Format(_T("%s"), Upload_forder_path);
		//AfxMessageBox(str);
	}
	else   
		AfxMessageBox(_T("��Ч��Ŀ¼��������ѡ��"));  

	CWnd *UploadFolderPath = GetDlgItem(IDC_EDIT_UP_PATH); 
	CWnd *UploadTime = GetDlgItem(IDC_EDIT_UP_TIME);
	UploadFolderPath->SetWindowTextW(TmpUploadFolderPath);
	m_up_settime.GetWindowText(UploadSetTime);
	//AfxMessageBox(UploadSetTime);

}

void CAutoUpDownLoad::OnBnClickedButtonDownload()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO bi;   
	bi.hwndOwner = m_hWnd;   
	bi.pidlRoot = NULL;   
	bi.pszDisplayName = (LPWSTR)Download_forder_path;   
	bi.lpszTitle = _T("��ѡ����Ҫ�����Ŀ¼��");   
	bi.ulFlags = 0;   
	bi.lpfn = NULL;   
	bi.lParam = 0;   
	bi.iImage = 0;   
	//����ѡ��Ŀ¼�Ի���
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);  
	if(lp && SHGetPathFromIDList(lp, (LPWSTR)Download_forder_path))   
	{
		TmpDownloadFolderPath.Format(_T("%s"), Download_forder_path);
		//AfxMessageBox(str);
	}
	else   
		AfxMessageBox(_T("��Ч��Ŀ¼��������ѡ��"));  

	CWnd *DownloadDestPath = GetDlgItem(IDC_EDIT_DES_PATH); 
	//CWnd *UploadTime = GetDlgItem(IDC_EDIT_UP_TIME);
	DownloadDestPath->SetWindowTextW(TmpDownloadFolderPath);
	m_down_settime.GetWindowText(DownloadSetTime);
}

void CAutoUpDownLoad::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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


//void CAutoUpDownLoad::AutoUpload()//��ʱ�ϴ�
//{
//	CCurl * pCurl = CCurl::Instance();
//	CString strSetUploadFilePath;//�ļ�����·��
//	CString str = TmpUploadFolderPath; //�ϴ�·��
//	CString strfilename;
//	CWnd *List = GetDlgItem(IDC_LIST_LOG);//������־
//	if(str.Right(1) != '\\')  
//		str += '\\';  
//	str += "*.*";  
//	CFileFind file;  
//	BOOL bContinue = file.FindFile(str);  //find ���ļ�
//	while(bContinue)  
//	{  
//		bContinue = file.FindNextFile();  
//		if(!file.IsDirectory() && !file.IsDots())  
//		{  
//			SHFILEINFO info;  
//			CString temp = str; //·�� 
//			int index = temp.Find(L"*.*");  
//			temp.Delete(index,3);  
//			strfilename = file.GetFileName();
//			CString strSetUploadFilePath = temp + strfilename;//��ȡ���ļ�����·��
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
//			//m_list_log.InsertString(0,_T("�ϴ��ɹ�"));
//			MessageBox(_T("upload ok"),_T("״̬"),MB_OK);
//
//		}
//		else 
//		{
//			//m_list_log.InsertString(0,_T("�ϴ�ʧ��"));
//			MessageBox(_T("host"),_T("״̬"),MB_OK);
//		}
//	}
//}





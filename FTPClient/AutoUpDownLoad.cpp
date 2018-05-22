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
	//DDX_Control(pDX, IDC_EDIT_UP_TIME, m_up_settime);
	//DDX_Control(pDX, IDC_EDIT_DOWN_TIME, m_down_settime);
	DDX_Control(pDX, IDC_EDIT_DOWN_PATH, m_edit_downpath);
	DDX_Control(pDX, IDC_EDIT_DES_PATH, m_edit_des_path);
	//DDX_Control(pDX, IDC_EDIT_UP_HOUR, m_edit_up_hour);
	//DDX_Control(pDX, IDC_EDIT_UP_MINUTE, m_edit_up_minute);
	//DDX_Control(pDX, IDC_EDIT_UP_SECOND, m_edit_up_second);
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
	//CWnd *UploadTime = GetDlgItem(IDC_EDIT_UP_TIME);
	UploadFolderPath->SetWindowTextW(TmpUploadFolderPath);
	//m_up_settime.GetWindowText(UploadSetTime);
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
	//m_down_settime.GetWindowText(DownloadSetTime);
}

void CAutoUpDownLoad::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	HANDLE Thread_RecvUdp; 

	//CFTPClientDlg ClientDlg;
	if((TmpUploadFolderPath.GetLength()!= 0)/*&&(UploadSetTime.GetLength()!=0)*/)
	{
		/*time_up = _ttoi(UploadSetTime);*/
	}
	//m_down_settime.GetWindowText(DownloadSetTime);
	m_edit_downpath.GetWindowText(DownloadFolderPath);
	if(/*(DownloadSetTime.GetLength()!=0)&&*/(DownloadFolderPath.GetLength()!=0)&&(TmpDownloadFolderPath.GetLength()!=0))
	{
		//time_down = _ttoi(DownloadSetTime);
	}
	//m_edit_up_hour.GetWindowText(up_hour);
	//m_edit_up_minute.GetWindowText(up_minute);
	//m_edit_up_second.GetWindowText(up_second);
	CDialogEx::OnOK();
}


BOOL CAutoUpDownLoad::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CWnd *UploadFolderPath = GetDlgItem(IDC_EDIT_UP_PATH); 
	UploadFolderPath->SetWindowTextW(TmpUploadFolderPath);

	CWnd *DownloadDestPath = GetDlgItem(IDC_EDIT_DES_PATH); 
	DownloadDestPath->SetWindowTextW(TmpDownloadFolderPath);

	CWnd *DownloadPath = GetDlgItem(IDC_EDIT_DOWN_PATH); 
	DownloadPath->SetWindowTextW(DownloadFolderPath);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

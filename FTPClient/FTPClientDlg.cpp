
// FTPClientDlg.cpp : ??????
//

#include "stdafx.h"
#include "FTPClient.h"
#include "FTPClientDlg.h"
#include "afxdialogex.h"
#include "ccurl.h"
#include "outctrl.h"
#include "AutoUpDownLoad.h"
#include <stdlib.h>
#include "io.h"
#include "direct.h"
#include <string.h>
#include<atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;

//char *user_local;//?????
//char *pass_local;//????
//char *host_local;//???
extern int time_up,time_down;//?????????????
extern CString TmpUploadFolderPath;//???????????��??????
extern CString TmpDownloadFolderPath;//???????
extern CString DownloadFolderPath;//????????????��??
extern CString up_hour,up_minute,up_second;//??????????
extern CString down_hour,down_minute,down_second;//??????????

int flag;		//??????????????0????��?1??????2?????

vec_upload vUpload;
up_info uploadfile_info;

vec_download vDownload;
download_info downloadfile_info;

vec_auto_upload vAutoUpload;
auto_up autoUpload_info;
// ??????��????????????? CAboutDlg ?????

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// ?????????
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ???

	// ???
protected:
	DECLARE_MESSAGE_MAP()
public:
	//	afx_msg void On32785();
	//	afx_msg void On32786();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

	//	ON_COMMAND(ID_32785, &CAboutDlg::On32785)
	//	ON_COMMAND(ID_32786, &CAboutDlg::On32786)
	//ON_WM_TIMER()
END_MESSAGE_MAP()




// CFTPClientDlg ?????



CFTPClientDlg::CFTPClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFTPClientDlg::IDD, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	flag = 0;		//??��??????????????
	m_pCurl = CCurl::Instance();

}

void CFTPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_LOCAL, m_tree_local);
	DDX_Control(pDX, IDC_TREE_SERVER, m_tree_server);
	/*DDX_Control(pDX, IDC_LIST_LOCAL_FILE, m_list_local);
	DDX_Control(pDX, IDC_LIST_SERVER_FILE, m_list_server);*/
	DDX_Control(pDX, IDC_LIST_LOCAL, m_list_local);
	DDX_Control(pDX, IDC_LIST_SERVER, m_list_server);
	DDX_Control(pDX, IDC_EDIT_LOCAL_PATH, m_edit_local);
	DDX_Control(pDX, IDC_EDIT_HOST, m_edit_host);
	DDX_Control(pDX, IDC_EDIT_USER, m_edit_user);
	DDX_Control(pDX, IDC_EDIT_PASS, m_edit_pass);
	DDX_Control(pDX, IDC_EDIT_PORT, m_edit_port);
	DDX_Control(pDX, IDC_LIST_LOG, m_list_log);
	DDX_Control(pDX, IDC_EDIT_SERVER_PATH, m_edit_server);
}

BEGIN_MESSAGE_MAP(CFTPClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_LOCAL, &CFTPClientDlg::OnTvnItemexpandedTreeLocal)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_LOCAL, &CFTPClientDlg::OnTvnSelchangedTreeLocal)
	ON_NOTIFY(NM_CLICK, IDC_TREE_LOCAL, &CFTPClientDlg::OnNMClickTreeLocal)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_LOCAL, &CFTPClientDlg::OnNMRClickListLocal)
	ON_BN_CLICKED(IDC_BUTTON_CONNETCT, &CFTPClientDlg::OnBnClickedButtonConnetct)
	ON_COMMAND(ID_32782, &CFTPClientDlg::OnUpload)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LOCAL, &CFTPClientDlg::OnNMClickListLocal)
	ON_NOTIFY(NM_CLICK, IDC_TREE_SERVER, &CFTPClientDlg::OnNMClickTreeServer)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_SERVER, &CFTPClientDlg::OnTvnItemexpandedTreeServer)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SERVER, &CFTPClientDlg::OnTvnSelchangedTreeServer)
	//ON_COMMAND(ID_32783, &CFTPClientDlg::OnDownload)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SERVER, &CFTPClientDlg::OnNMRClickListServer)

	ON_COMMAND(ID_32784, &CFTPClientDlg::On32784)
	ON_WM_TIMER()
	ON_COMMAND(ID_32786, &CFTPClientDlg::OnDownload)
	ON_MESSAGE(WM_SHOWTASK,&CFTPClientDlg::OnShowTask)
END_MESSAGE_MAP()



CFTPClientDlg::~CFTPClientDlg()
{
	CoUninitialize(); // ????????
}
// CFTPClientDlg ??????????

BOOL CFTPClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CoInitialize(NULL); // ???????
	// ????????...?????????????????��?

	// IDM_ABOUTBOX ????????????��???
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ?????????????????��??????????????????????????
	//  ??��????
	SetIcon(m_hIcon, TRUE);			// ????????
	SetIcon(m_hIcon, FALSE);		// ????��???

	// TODO: ??????????????????
	CRect temprect(0,0,905,765);
	CWnd::SetWindowPos(NULL,0,0,temprect.Width(),temprect.Height(),SWP_NOZORDER|SWP_NOMOVE);
	m_ImageList.Create(32,32,ILC_COLOR32,10,30);     //???????????CImageList????   
	m_list_local.SetImageList(&m_ImageList,LVSIL_NORMAL);  //????��?????????????     
	m_tree_local.ModifyStyle(NULL,TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS);  
	m_hRoot = m_tree_local.InsertItem(_T("??????"));         //????????  
	GetLogicalDrives(m_hRoot);                      //????��?? ???????  
	GetDriveDir(m_hRoot);                           //????��?? ???????????  
	m_tree_local.Expand(m_hRoot,TVE_EXPAND);              //?????????????��? TVE_EXPAND????��?  
	initLocalList();//?????????????��?
	initServerList();//???????????????��?

	HANDLE Thread_Upload_File; 
	HANDLE Thread_Download_File;
	Thread_Upload_File = CreateThread(NULL,0,Thread_UploadFile,this,0,NULL );//??????
	Thread_Download_File = CreateThread(NULL,0,Thread_DownloadFile,this,0,NULL);//???????

	//set default upload/download dir
	TmpUploadFolderPath = _T("D:\\RadarTaskFile");
	DownloadFolderPath = _T("FtpAutoDownloadFile");
	TmpDownloadFolderPath = _T("D:\\FtpAutoDownloadFile");

	if(_access("D:\\RadarTaskFile",0) == -1)
	{
		if(_mkdir("D:\\RadarTaskFile") == -1)
		{
			int err = GetLastError();
		}
	}
	if(_access("D:\\FtpAutoDownloadFile",0) == -1)
	{
		if(_mkdir("D:\\FtpAutoDownloadFile") == -1)
		{
			int err = GetLastError();
		}
	}
	//host="localhost";
	host="25.95.234.232";
	user="11";
	pass="11";
	port="2121";

	m_edit_user.SetWindowText(_T("11"));
	m_edit_pass.SetWindowText(_T("11"));
	m_edit_port.SetWindowText(_T("2121"));
	m_edit_host.SetWindowText(_T("25.95.234.232"));
	m_pCurl->setHostUserPwd("11","11","25.95.234.232",2121);

	ConnectFtp(_T("25.95.234.232"),_T("11"),_T("11"),_T("2121"));

	//m_pCurl->setHostUserPwd("11","11","localhost",2121);
	//ConnectFtp(_T("localhost"),_T("11"),_T("11"),_T("2121"));

	if(bconnect)
	{
		m_list_log.InsertString(0,_T("??????"));
		m_serverRoot=m_tree_server.InsertItem(_T("??????"));
		/*m_tree_server.ModifyStyle(NULL,TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS);  */
		UpdateDir(m_serverRoot);
		GetFTPDriveDir(m_serverRoot);
	}

	SetTimer(10,1000,NULL);		//test ??????autodownload and autoupload

	return TRUE;  // ???????????????????????? TRUE
}

void CFTPClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}
void CFTPClientDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if(nType == SIZE_MINIMIZED)
	{
		/*??��????????*/
		m_nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
		m_nid.hWnd = this->m_hWnd;
		m_nid.uID = IDI_ICON1;
		m_nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
		m_nid.uCallbackMessage = WM_SHOWTASK;
		m_nid.hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1));
		wcscpy(m_nid.szTip, L"FTP?????");
		Shell_NotifyIcon(NIM_ADD,&m_nid);// ??????????????
		ShowWindow(SW_HIDE);//??????��???????????????
	}
	// TODO: ????????????????????
	int m_interval = (int)(cx*0.005);
	//CONNECT INPUT AREA
	CWnd* pWnd=GetDlgItem(IDC_STATIC_CONNECT);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.01*cx,0.01*cy,(int)(cx*0.98),cy*0.09);
	pWnd=GetDlgItem(IDC_STATIC_HOST);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.03*cx,0.045*cy,(int)(cx*0.07),cy*0.04);

	pWnd=GetDlgItem(IDC_EDIT_HOST);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.09*cx,0.04*cy,(int)(cx*0.1),cy*0.04);

	pWnd=GetDlgItem(IDC_STATIC_USER);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.23*cx,0.045*cy,(int)(cx*0.07),cy*0.04);

	pWnd=GetDlgItem(IDC_EDIT_USER);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.31*cx,0.04*cy,(int)(cx*0.1),cy*0.04);

	pWnd=GetDlgItem(IDC_STATIC_PASS);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.43*cx,0.045*cy,(int)(cx*0.07),cy*0.04);

	pWnd=GetDlgItem(IDC_EDIT_PASS);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.49*cx,0.04*cy,(int)(cx*0.1),cy*0.04);

	pWnd=GetDlgItem(IDC_STATIC_PORT);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.63*cx,0.045*cy,(int)(cx*0.07),cy*0.04);

	pWnd=GetDlgItem(IDC_EDIT_PORT);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.69*cx,0.04*cy,(int)(cx*0.1),cy*0.04);

	pWnd=GetDlgItem(IDC_BUTTON_CONNETCT);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.84*cx,0.04*cy,(int)(cx*0.1),cy*0.04);

	//LOCAL FILE PATH
	pWnd=GetDlgItem(IDC_STATIC_LOCALPATH);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.01*cx,0.1*cy,(int)(cx*0.48),cy*0.37);

	pWnd=GetDlgItem(IDC_STATIC_LOCAL);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.03*cx,0.15*cy,(int)(cx*0.1),cy*0.04);

	pWnd=GetDlgItem(IDC_EDIT_LOCAL_PATH);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.14*cx,0.15*cy,(int)(cx*0.32),cy*0.04);

	pWnd=GetDlgItem(IDC_TREE_LOCAL);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.02*cx,0.2*cy,(int)(cx*0.46),cy*0.25);


	////SERVER FILE PATH

	pWnd=GetDlgItem(IDC_STATIC_SERVERPATH);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.51*cx,0.1*cy,(int)(cx*0.48),cy*0.37);

	pWnd=GetDlgItem(IDC_STATIC_SERVER);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.53*cx,0.15*cy,(int)(cx*0.12),cy*0.04);

	pWnd=GetDlgItem(IDC_EDIT_SERVER_PATH);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.66*cx,0.15*cy,(int)(cx*0.3),cy*0.04);

	pWnd=GetDlgItem(IDC_TREE_SERVER);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.52*cx,0.2*cy,(int)(cx*0.46),cy*0.25);

	////LOCAL FILE LIST
	pWnd=GetDlgItem(IDC_LIST_LOCAL);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.02*cx,0.49*cy,(int)(cx*0.46),cy*0.27);

	////SERVER FILE LIST
	pWnd=GetDlgItem(IDC_LIST_SERVER);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.52*cx,0.49*cy,(int)(cx*0.46),cy*0.27);

	////LIST LOG
	pWnd=GetDlgItem(IDC_LIST_LOG);
	if(pWnd->GetSafeHwnd())   
		pWnd->MoveWindow(0.02*cx,0.77*cy,(int)(cx*0.96),cy*0.20);

}

// ??????????????��????????????????????
//  ????????????????????/??????? MFC ??��???
//  ???????????��?

void CFTPClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ?????????��??????

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ????????????????��???
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ???????
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//??????????��???????????????????��??
//?????
HCURSOR CFTPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFTPClientDlg::GetLogicalDrives(HTREEITEM hParent)
{
	//???????????????????????  
	//size_t szAllDriveStrings = GetLogicalDriveStrings(0,NULL);           //???????????  
	//char *pDriveStrings = new char[szAllDriveStrings + sizeof(_T(""))];  //????????  

	//GetLogicalDriveStrings(szAllDriveStrings,(LPWSTR)pDriveStrings);

	////GetLogicalDriveStrings(szAllDriveStrings,);
	//size_t szDriveString = strlen(pDriveStrings);                        //??????��  
	//while(szDriveString > 0)  
	//{   
	//	m_tree_local.InsertItem((LPWSTR)pDriveStrings,hParent);       //??????hParent????????  
	//	//pDriveStrings += szDriveString + 1;             //pDriveStrings??C:\ D:\ E:\??  
	//	pDriveStrings += szDriveString+1;
	//	szDriveString = strlen(pDriveStrings);  
	//} 
	TCHAR szTemp[512];
	szTemp[0] = '\0';

	if (GetLogicalDriveStrings(512-1, szTemp)) 
	{
		TCHAR szName[MAX_PATH];
		TCHAR szDrive[3] = TEXT(" :");
		BOOL bFound = FALSE;
		TCHAR* p = szTemp;

		while(*p)  
		{   
			m_tree_local.InsertItem(p,hParent);       //??????hParent????????  
			//wszResult += wszResult + 1;             //pDriveStrings??C:\ D:\ E:\??  
			//szDriveString = strlen(pDriveStrings);  

			while(*p++)
				;

			//szDriveString = strlen(CStr); 
		} 
	}
}
void CFTPClientDlg::GetDriveDir(HTREEITEM hParent)
{
	HTREEITEM hChild = m_tree_local.GetChildItem(hParent);   //??????��???��?????  
	while(hChild)                                        
	{  
		CString strText = m_tree_local.GetItemText(hChild);  //?????��??????????  
		if(strText.Right(1) != '\\')                   //?????1??????????????nCount?????  
			strText += _T('\\');  
		strText += "*.*";  
		//????????????????InsertItem??????  
		CFileFind file;                                       //???��?????????  
		BOOL bContinue = file.FindFile(strText);              //?????????????????  
		while(bContinue)  
		{  
			bContinue = file.FindNextFile();                  //????????????  
			if(file.IsDirectory() && !file.IsDots())          //?????????????????"."  
				m_tree_local.InsertItem(file.GetFileName(),hChild); //??????��????????????  
		}  
		GetDriveDir(hChild);                                  //??????  
		hChild = m_tree_local.GetNextItem(hChild,TVGN_NEXT);        //??????��??TVGN_NEXT???????  
	}  
}

void CFTPClientDlg::AddSubDir(HTREEITEM hParent)
{
	CString strPath = GetFullPath(hParent);     //????��??  
	if(strPath.Right(1) != '\\')  
		strPath += '\\';  
	strPath += "*.*";  
	CFileFind file;  
	BOOL bContinue = file.FindFile(strPath);    //?????????????????  
	while(bContinue)  
	{  
		bContinue = file.FindNextFile();        //????????????  
		if(file.IsDirectory() && !file.IsDots())  
			m_tree_local.InsertItem(file.GetFileName(),hParent);  
	}  
}
CString CFTPClientDlg::GetFullPath(HTREEITEM hCurrent)
{
	CString strTemp;  
	CString strReturn = L"";  
	while(hCurrent != m_hRoot)  
	{  
		strTemp = m_tree_local.GetItemText(hCurrent);    //?????��??????????  
		if(strTemp.Right(1) != '\\')  
			strTemp += '\\';  
		strReturn = strTemp  + strReturn;          
		hCurrent = m_tree_local.GetParentItem(hCurrent); //???????????  
	}  
	return strReturn;  
}

void CFTPClientDlg::OnTvnItemexpandedTreeLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: ????????????????????
	TVITEM item = pNMTreeView->itemNew;                  //????\????????????????????  
	if(item.hItem == m_hRoot)  
		return;  
	HTREEITEM hChild = m_tree_local.GetChildItem(item.hItem);  //??????��???��?????  
	while(hChild)  
	{  
		AddSubDir(hChild);                               //???????  
		hChild = m_tree_local.GetNextItem(hChild,TVGN_NEXT);   //??????��??TVGN_NEXT???????  
	}  
	*pResult = 0;
}


void CFTPClientDlg::OnTvnSelchangedTreeLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: ????????????????????
	m_list_local.DeleteAllItems();  
	file_num=0;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;  
	TVITEM item = pNMTreeView->itemNew;  
	if(item.hItem == m_hRoot)  
		return;  
	CString str = GetFullPath(item.hItem); 
	CString strfilename;
	if(str.Right(1) != '\\')  
		str += '\\';  
	str += "*.*";  
	CFileFind file;  
	BOOL bContinue = file.FindFile(str);  //find ?????
	while(bContinue)  
	{  
		bContinue = file.FindNextFile();  
		if(!file.IsDirectory() && !file.IsDots())  
		{  
			SHFILEINFO info;  
			CString temp = str;  
			int index = temp.Find(L"*.*");  
			temp.Delete(index,3);  
			strfilename = file.GetFileName();
			CString strSetIcon = temp + strfilename;

			//?????????????????
			WIN32_FIND_DATA ffd ;
			HANDLE hFind = FindFirstFile(strSetIcon,&ffd);
			SYSTEMTIME stUTC, stLocal;
			FileTimeToSystemTime(&(ffd.ftLastWriteTime), &stUTC);//???????????????
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);//????????
			FileSize.Format(_T("%d"),ffd.nFileSizeLow);
			LastWriteTime.Format(_T("%d/%d/%d %d:%d:%d"),stLocal.wYear,stLocal.wMonth,stLocal.wDay,stLocal.wHour,stLocal.wMinute,stLocal.wSecond);//???????????
			m_list_local.InsertItem(file_num,strfilename);
			m_list_local.SetItemText(file_num,1,FileSize);
			m_list_local.SetItemText(file_num,2,LastWriteTime);
			file_num++;
			//SHGetFileInfo(temp + file.GetFileName(),0,&info,sizeof(&info),SHGFI_DISPLAYNAME | SHGFI_ICON);  
			//file_num = m_ImageList.Add(info.hIcon);  
			//m_list_local.InsertItem(file_num,strfilename);  
			//--get the LastWriteTime of folder/files
			//myTime.Format("%d. %d %d, %d:%d",stLocal.wDay,stLocal.wMonth,stLocal.wYear,stLocal.wHour,stLocal.wMinute);
			//--
			//for(int i=0;i<file_num;i++)
			/*m_list_local.SetItemText(file_num,0,strfilename);
			m_list_local.SetItemText(file_num,1,FileSize);
			m_list_local.SetItemText(file_num,3,LastWriteTime);*/
			//file.GetLastWriteTime()
			//FindClose(hFind);		
		}  
	}  
	*pResult = 0;
}

#define BUFSIZE 4096
void CFTPClientDlg::OnNMClickTreeLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ????????????????????
	CPoint point;
	TCHAR  buffer[BUFSIZE];   
	GetCursorPos(&point);//??????????��??
	m_tree_local.ScreenToClient(&point);//???????????
	UINT uFlags;

	HTREEITEM CurrentItem;
	/*CString local_path;*/
	CurrentItem=m_tree_local.HitTest(point,&uFlags);//????????????ITEM
	local_path = GetFullPath(CurrentItem);
	m_edit_local.SetWindowText(local_path);
	*pResult = 0;
}



void CFTPClientDlg::OnNMRClickListLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ????????????????????
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if(pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point( LOWORD(dwPos), HIWORD(dwPos) );

		CMenu menu;
		menu.LoadMenu( IDR_MENU1 );
		CMenu* popup = menu.GetSubMenu(0);
		ASSERT( popup != NULL );
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this ); 
	}
	//???????????��??
	filefile = local_path;
	local_file_name = m_list_local.GetItemText( pNMListView->iItem, pNMListView->iSubItem ) ;
	filefile =  filefile  + local_file_name;
	//MessageBox(filefile);
	*pResult = 0;
}
void CFTPClientDlg::OnNMRClickListServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ????????????????????
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if(pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point( LOWORD(dwPos), HIWORD(dwPos) );

		CMenu menu;
		menu.LoadMenu( IDR_MENU2 );
		CMenu* popup = menu.GetSubMenu(0);
		ASSERT( popup != NULL );
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this ); 
	}
	server_file_name = m_list_server.GetItemText( pNMListView->iItem, pNMListView->iSubItem ) ;
	//AfxMessageBox(server_file_name);
	*pResult = 0;
}
void CFTPClientDlg::OnNMClickListLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ????????????????????
	//NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	/*CString filefile;*/
	//filefile = local_path;
	//local_file_name = m_list_local.GetItemText( pNMListView->iItem, pNMListView->iSubItem ) ;
	//filefile =  filefile  + local_file_name;
	//local_path = filefile + local_file_name;
	//MessageBox(filefile);
	//str = m_szPath + str;
	//char *fileName = (LPSTR)(LPCTSTR)str;
	//IplImage* img = cvLoadImage(fileName);
	*pResult = 0;
}

void CFTPClientDlg::OnBnClickedButtonConnetct()
{
	// TODO: ????????????????????
	m_edit_host.GetWindowText(host);
	m_edit_user.GetWindowText(user);
	m_edit_pass.GetWindowText(pass);
	m_edit_port.GetWindowText(port);
	ConnectFtp(host,user,pass,port);
	if(bconnect)
	{
		m_list_log.InsertString(0,_T("??????"));
		m_serverRoot=m_tree_server.InsertItem(_T("??????"));
		/*m_tree_server.ModifyStyle(NULL,TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS);  */
		UpdateDir(m_serverRoot);
		GetFTPDriveDir(m_serverRoot);
		AfxMessageBox(_T("??????"));
	}

}

DWORD WINAPI CFTPClientDlg::Thread_UploadFile(LPVOID pParam)//????udp????
{
	int count=0;
	while(1)
	{
		/*vec_upload vUpload;
		up_info up_fileinfo;
		vec_upload vUpload;
		up_info uploadfile_info;
		*/

		//CFTPClientDlg::AutoUpload();
		//int fileNum = vUpload.vec_up.size();
		CCurl * pCurl = CCurl::Instance();
		if(flag != 2)
		{
			if(vUpload.vec_up.size() != 0)	//?????????????
			{
				flag = 1;		//?????��????????????????
				DWORD dwNum_user = WideCharToMultiByte(CP_OEMCP,NULL,user,-1,NULL,NULL,0,NULL);
				char *user_local = new char[dwNum_user+1];
				WideCharToMultiByte(CP_OEMCP,NULL,user,-1,user_local,dwNum_user,0,NULL);
				user_local[dwNum_user] = '\0';

				DWORD dwNum_pass = WideCharToMultiByte(CP_OEMCP,NULL,pass,-1,NULL,NULL,0,NULL);
				char *pass_local = new char[dwNum_pass+1];
				WideCharToMultiByte(CP_OEMCP,NULL,pass,-1,pass_local,dwNum_pass,0,NULL);
				pass_local[dwNum_pass]='\0';

				DWORD dwNum_host = WideCharToMultiByte(CP_ACP,NULL,host,-1,NULL,NULL,0,NULL);
				char *host_local = new char[dwNum_host];
				WideCharToMultiByte(CP_OEMCP,NULL,host,-1,host_local,dwNum_host,0,NULL);
				int port_after = _tstoi(port);
				pCurl->setHostUserPwd(user_local, pass_local, host_local,port_after);
				delete user_local;
				delete pass_local;

				for(int i = 0;i<vUpload.vec_up.size();i++)
				{
					uploadfile_info = vUpload.vec_up.at(0);
					CString list_log;
					list_log = uploadfile_info.upfile_fullpath;
					if (pCurl->upload(uploadfile_info.upfile_fullpath,uploadfile_info.upfile_name))
					{
						list_log += _T("??????");
						//m_list_log.InsertString(0,_T("??????"));
						vUpload.vec_up.erase(vUpload.vec_up.begin());		//?????????????????????????????????
					}
					else 
					{
						count++;
						list_log += _T("??????");
						CString errCountStr;
						errCountStr.Format(_T(",????????%d"),count);
						list_log+= errCountStr;

						Sleep(3000);

						if(count >= 3)
						{
							vUpload.vec_up.erase(vUpload.vec_up.begin());//?????????????????????????????????
							count = 0;
						}
					}

					m_list_log.InsertString(0,list_log);
					//vUpload.vec_up.erase(vUpload.vec_up.begin());//?????????????????????????????????
				}
				/*vUpload.vec_up.clear();*/
			}
			else
			{
				if(flag == 1)		//????????????????????
					flag = 0;
				Sleep(3000);
			}
		}
	}
	return 0;
}
void CFTPClientDlg::OnUpload()
{
	// TODO: ??????????????????
	USES_CONVERSION;

	DWORD dwNum_path = WideCharToMultiByte(CP_ACP,NULL,filefile,-1,NULL,NULL,0,NULL);
	char *local_path_after = new char[dwNum_path];
	WideCharToMultiByte(CP_OEMCP,NULL,filefile,-1,local_path_after,dwNum_path,0,NULL);

	DWORD dwNum_file_name = WideCharToMultiByte(CP_ACP,NULL,local_file_name,-1,NULL,NULL,0,NULL);
	char *local_file_name_after = new char[dwNum_file_name];
	WideCharToMultiByte(CP_OEMCP,NULL,local_file_name,-1,local_file_name_after,dwNum_file_name,0,NULL);

	strcpy(uploadfile_info.upfile_fullpath,local_path_after);
	strcpy(uploadfile_info.upfile_name,local_file_name_after);

	//uploadfile_info.upfile_fullpath = local_path_after;
	//uploadfile_info.upfile_name = local_file_name_after;
	vUpload.vec_up.push_back(uploadfile_info);
}
DWORD WINAPI CFTPClientDlg::Thread_DownloadFile(LPVOID pParam)//??????????
{
	int count=0;
	while(1)
	{
		//CFTPClientDlg::AutoDownload();
		int file_down_num = vDownload.vec_down.size();
		CCurl * pCurl = CCurl::Instance();
		
		if(flag != 1)
		{
			if(vDownload.vec_down.size() != 0)
			{
				flag = 2;//???????????????
				DWORD dwNum_user = WideCharToMultiByte(CP_OEMCP,NULL,user,-1,NULL,NULL,0,NULL);
				char *user_local = new char[dwNum_user];
				WideCharToMultiByte(CP_OEMCP,NULL,user,-1,user_local,dwNum_user,0,NULL);

				DWORD dwNum_pass = WideCharToMultiByte(CP_OEMCP,NULL,pass,-1,NULL,NULL,0,NULL);
				char *pass_local = new char[dwNum_pass];
				WideCharToMultiByte(CP_OEMCP,NULL,pass,-1,pass_local,dwNum_pass,0,NULL);

				DWORD dwNum_host = WideCharToMultiByte(CP_ACP,NULL,host,-1,NULL,NULL,0,NULL);
				char *host_local = new char[dwNum_host];
				WideCharToMultiByte(CP_OEMCP,NULL,host,-1,host_local,dwNum_host,0,NULL);
				int port_after = _tstoi(port);
				pCurl->setHostUserPwd(user_local, pass_local, host_local,port_after);
				CString list_log;
				list_log = downloadfile_info.downfile_name;
				for(int i = 0;i<vDownload.vec_down.size();i++)
				{
					downloadfile_info = vDownload.vec_down.at(0);
					if (pCurl->download(downloadfile_info.downfile_name,downloadfile_info.downfile_fullpath))
					{
						list_log += _T("??????");
						vDownload.vec_down.erase(vDownload.vec_down.begin());		//??????????????????????????????????
					}
					else
					{
						count++;
						list_log += _T("???????");
						CString strCount;
						strCount.Format(_T(",??????%d"),count);
						list_log += strCount;

						Sleep(3000);
						if(count>=3)
						{
							vDownload.vec_down.erase(vDownload.vec_down.begin());		//??????????????????????????????????
							count = 0;
						}
					}
					m_list_log.InsertString(0,list_log);
				}
			}
			else
			{
				if(flag == 2)
					flag = 0;//??????????????????��???????
				Sleep(3000);
			}
		}
	}
	return 0;
}
void CFTPClientDlg::OnDownload()
{
	// TODO: ??????????????????
	CCurl * pCurl = CCurl::Instance();

	DWORD dwNum_server_file_name = WideCharToMultiByte(CP_OEMCP,NULL,server_file_name,-1,NULL,NULL,0,NULL);
	char *server_file_name_after = new char[dwNum_server_file_name+1];
	WideCharToMultiByte(CP_OEMCP,NULL,server_file_name,-1,server_file_name_after,dwNum_server_file_name,0,NULL);
	server_file_name_after[dwNum_server_file_name]='\0';

	DWORD dwNum_server_path = WideCharToMultiByte(CP_OEMCP,NULL,server_path,-1,NULL,NULL,0,NULL);
	char *server_path_after = new char[dwNum_server_path];
	WideCharToMultiByte(CP_OEMCP,NULL,server_path,-1,server_path_after,dwNum_server_path,0,NULL);

	//char szPath[MAX_PATH];     //?????????��?? 
	/*CString server_path[MAX_PATH];*/
	CString str;
	//ZeroMemory(szPath, sizeof(szPath));
	//ZeroMemory(server_path, sizeof(server_path));

	BROWSEINFO bi;   
	bi.hwndOwner = m_hWnd;   
	bi.pidlRoot = NULL;   
	bi.pszDisplayName = (LPWSTR)server_dpath;   
	bi.lpszTitle = _T("?????????????????");   
	bi.ulFlags = 0;   
	bi.lpfn = NULL;   
	bi.lParam = 0;   
	bi.iImage = 0;   
	//??????????????
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);  
	if(lp && SHGetPathFromIDList(lp, (LPWSTR)server_dpath))   
	{
		str.Format(_T("%s"), server_dpath);
		//AfxMessageBox(str);
	}
	else   
		AfxMessageBox(_T("??��???????????????"));  

	if(str.Right(1) != '\\')  
		str += '\\';
	server_file_path = str + server_file_name;

	DWORD dwNum_server_file_path = WideCharToMultiByte(CP_OEMCP,NULL,server_file_path,-1,NULL,NULL,0,NULL);
	char *server_file_path_after = new char[dwNum_server_file_path];
	WideCharToMultiByte(CP_OEMCP,NULL,server_file_path,-1,server_file_path_after,dwNum_server_file_path,0,NULL);
	char *pathpath = strcat(server_path_after,server_file_name_after);

	strcpy(downloadfile_info.downfile_fullpath,server_file_path_after);
	strcpy(downloadfile_info.downfile_name,pathpath);
	vDownload.vec_down.push_back(downloadfile_info);//????????push????????

	delete server_file_name_after;
}


void CFTPClientDlg::ConnectFtp(CString sip,CString strusr,CString strpwd,CString strport){
	BYTE nFild[4];
	UpdateData();
	//ServerIP.GetAddress(nFild[0],nFild[1],nFild[2],nFild[3]);
	//CString sip;
	//sip.Format("%d.%d.%d.%d",nFild[0],nFild[1],nFild[2],nFild[3]);
	if (sip.IsEmpty())
	{
		AfxMessageBox(_T("IP???????"));
		return;
	}
	if (strport.IsEmpty())
	{
		AfxMessageBox(_T("????????"));
		return;
	}
	if (strusr.IsEmpty())
	{
		return;
	}
	//???????Internet??
	pInternetSession= new CInternetSession(_T("MR"),INTERNET_OPEN_TYPE_PRECONFIG);

	DWORD dwNum_port = WideCharToMultiByte(CP_ACP,NULL,strport,-1,NULL,NULL,0,NULL);
	char *port = new char[dwNum_port+1];
	WideCharToMultiByte(CP_OEMCP,NULL,strport,-1,port,dwNum_port,0,NULL);
	port[dwNum_port]='\0';
	//try
	//{
	//????Internet??????pInternetSession?????FTP????
	pFtpConnection=pInternetSession->GetFtpConnection(sip,strusr,strpwd,atoi(port));
	//pThreadFtpConnection = pInternetSession->GetFtpConnection(sip,strusr,strpwd,atoi(port));
	delete port;

	bconnect=true;

	//}
	//catch (CInternetException* pEx)
	//{
	//	TCHAR szErr[1024];
	//	pEx->GetErrorMessage(szErr,1024);
	//	AfxMessageBox(szErr);
	//	pEx->Delete();
	//} 

}
void CFTPClientDlg::UpdateDir(HTREEITEM hParent){
	//m_list_server.ResetContent();
	//m_list_server.DeleteAllItems(); // ??????
	//??��???????��????????????????CFtpFileFind?????	
	//???????????????????��????CFtpFileFind::FindFile???
	//BOOL bfind=ftpfind.FindFile(NULL);
	//m_tree_server.Expand(hParent,TVE_EXPAND);              //?????????????��? TVE_EXPAND????��?  
	//GetFTPDriveDir(hParent); 
	//m_ImageList.Create(32,32,ILC_COLOR32,10,30);     //???????????CImageList????   
	//m_list_server.SetImageList(&m_ImageList,LVSIL_NORMAL);  //????��?????????????   
	m_tree_server.ModifyStyle(NULL,TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS);  
	CFtpFileFind ftpfind(pFtpConnection);
	BOOL bfind=ftpfind.FindFile(NULL);
	file_num = 0;
	while (bfind)
	{
		bfind=ftpfind.FindNextFile();
		CString strpath;
		if(ftpfind.IsDirectory() && !ftpfind.IsDots())          //?????????????????"."  
			m_tree_server.InsertItem(ftpfind.GetFileName(),/*hChild*/hParent); //??????��????????????  
		if(!ftpfind.IsDirectory() && !ftpfind.IsDots())
		{
			SHFILEINFO info;   
			CString strfilename = ftpfind.GetFileName();//????????

			FileSize.Format(_T("%I64d"),ftpfind.GetLength());//?????????????��
			CTime time;//????????????
			ftpfind.GetLastWriteTime(time);//???????????
			LastWriteTime.Format(_T("%d/%d/%d %d:%d:%d"),time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());//???????????

			//????????list???

			m_list_server.InsertItem(file_num,strfilename);
			m_list_server.SetItemText(file_num,1,FileSize);
			m_list_server.SetItemText(file_num,2,LastWriteTime);

			file_num++;
		}


	}

	m_tree_server.Expand(hParent,TVE_EXPAND);              //?????????????��? TVE_EXPAND????��?  
}

void CFTPClientDlg::GetFTPDriveDir(HTREEITEM hParent)
{
	HTREEITEM hChild = m_tree_server.GetChildItem(hParent);   //??????��???��?????  
	while(hChild)                                        
	{ 
		//????????????????InsertItem??????                                
		CString strText = m_tree_server.GetItemText(hChild);  //?????��??????????  
		if(strText.Right(1) != '\\')                   //?????1??????????????nCount?????  
			strText += _T('\\');  
		strText += "*.*";  
		CFtpFileFind ftpfind(pFtpConnection);//????FTP????
		//BOOL bContinue = ftpfind.FindFile(strText);              //?????????????????  
		BOOL bContinue = ftpfind.FindFile(strText);              //????????????????? 
		while(bContinue)  
		{  
			bContinue = ftpfind.FindNextFile();                  //????????????  
			if(ftpfind.IsDirectory() && !ftpfind.IsDots())          //?????????????????"."  
				m_tree_server.InsertItem(ftpfind.GetFileName(),hChild/*hParent*/); //??????��????????????  
		}  
		GetFTPDriveDir(hChild/*hParent*/);                                  //??????  
		hChild = m_tree_server.GetNextItem(hChild,TVGN_NEXT);        //??????��??TVGN_NEXT???????  
	}  
	//m_tree_server.Expand(hParent,TVE_EXPAND);              //?????????????��? TVE_EXPAND????��? 
}
CString CFTPClientDlg::GetFTPFullPath(HTREEITEM hCurrent)
{
	CString strTemp;  
	CString strReturn = L"";  
	while(hCurrent != m_serverRoot)  
	{  
		strTemp = m_tree_server.GetItemText(hCurrent);    //?????��??????????  
		if(strTemp.Right(1) != '\\')  
			strTemp += '\\';  
		strReturn = strTemp  + strReturn;          
		hCurrent = m_tree_server.GetParentItem(hCurrent); //???????????  
	}  
	return strReturn;  
}
void CFTPClientDlg::AddFTPSubDir(HTREEITEM hParent)
{
	CString strPath = GetFTPFullPath(hParent);     //????��??  
	if(strPath.Right(1) != '\\')  
		strPath += '\\';  
	strPath += "*.*";  
	//CFileFind file;  
	CFtpFileFind ftpfind(pFtpConnection);//????FTP????
	BOOL bContinue = ftpfind.FindFile(strPath);    //?????????????????  
	while(bContinue)  
	{  
		bContinue = ftpfind.FindNextFile();        //????????????  
		if(ftpfind.IsDirectory() && !ftpfind.IsDots())  
			m_tree_server.InsertItem(ftpfind.GetFileName(),hParent);  
	}  
}



void CFTPClientDlg::OnTvnItemexpandedTreeServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: ????????????????????
	TVITEM item = pNMTreeView->itemNew;                  //????\????????????????????  
	if(item.hItem == m_serverRoot)  
		return;  
	HTREEITEM hChild = m_tree_server.GetChildItem(item.hItem);  //??????��???��?????  
	while(hChild)  
	{  
		AddFTPSubDir(hChild);                               //???????  
		hChild = m_tree_server.GetNextItem(hChild,TVGN_NEXT);   //??????��??TVGN_NEXT???????  
	} 
	*pResult = 0;
}
void CFTPClientDlg::OnNMClickTreeServer(NMHDR *pNMHDR, LRESULT *pResult)//???????????��??
{
	// TODO: ????????????????????
	CPoint point;
	TCHAR  buffer[BUFSIZE];   
	GetCursorPos(&point);//??????????��??
	m_tree_server.ScreenToClient(&point);//???????????
	UINT uFlags;

	HTREEITEM CurrentItem;
	//CString server_path;//??????��??
	CurrentItem=m_tree_server.HitTest(point,&uFlags);//????????????ITEM
	server_path = GetFTPFullPath(CurrentItem);

	m_edit_server.SetWindowText(server_path);
	*pResult = 0;
}

void CFTPClientDlg::OnTvnSelchangedTreeServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: ????????????????????
	m_list_server.DeleteAllItems();  
	file_num=0;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;  
	TVITEM item = pNMTreeView->itemNew;  
	if(item.hItem == m_serverRoot)  
		return;  
	CString str = GetFTPFullPath(item.hItem);  
	if(str.Right(1) != '\\')  
		str += '\\';  
	str += "*.*";  
	/*CFileFind file;  */
	CFtpFileFind ftpfind(pFtpConnection);//????FTP????
	BOOL bContinue = ftpfind.FindFile(str);  
	while(bContinue)  
	{  
		bContinue = ftpfind.FindNextFile();  
		if(!ftpfind.IsDirectory() && !ftpfind.IsDots())  
		{  
			SHFILEINFO info;  
			CString temp = str;  
			//CString file_type;
			int index = temp.Find(L"*.*");  
			temp.Delete(index,3);  
			CString strfilename = ftpfind.GetFileName();//????????
			CString strSetIcon = temp + strfilename;

			FileSize.Format(_T("%I64d"),ftpfind.GetLength());//?????????????��
			CTime time;//????????????
			ftpfind.GetLastWriteTime(time);//???????????
			LastWriteTime.Format(_T("%d/%d/%d %d:%d:%d"),time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());//???????????

			//????????list???
			//strfilename = GBKToUTF8(strfilename);
			/*setlocale(LC_ALL,"chs");
			char* p = new char[1024];
			wcstombs( p ,strfilename,strfilename.GetLength() );
			std::string out = p;*/
			//CString namename = GBKToUTF8(strfilename);
			/*CString ser_name;
			ser_name.Format(_T("%s"), namename.c_str());*/
			m_list_server.InsertItem(file_num,strfilename);
			m_list_server.SetItemText(file_num,1,FileSize);
			m_list_server.SetItemText(file_num,2,LastWriteTime);

			//ftpfind.Close();
			file_num++;


			//AfxMessageBox(strSetIcon);
			/*	DWORD dw = SHGetFileInfo(strSetIcon,0,&info,sizeof(&info),SHGFI_DISPLAYNAME | SHGFI_ICON);  
			int i = m_ImageList.Add(info.hIcon);  */
			//?????????????????
			//WIN32_FIND_DATA ffd ;
			//HANDLE hFind = FtpFindFirstFile(pFtpConnection,strSetIcon,&ffd,2,0);
			//SYSTEMTIME stUTC, stLocal;
			//FileTimeToSystemTime(&(ffd.ftLastWriteTime), &stUTC);//???????????????
			//SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);//????????
			//FileSize.Format(_T("%d"),ffd.nFileSizeLow);

			//LastWriteTime.Format(_T("%d/%d/%d %d:%d:%d"),stLocal.wYear,stLocal.wMonth,stLocal.wDay,stLocal.wHour,stLocal.wMinute,stLocal.wSecond);

			//FindClose(hFind);

		}  
	} 
	*pResult = 0;
}
void CFTPClientDlg::initLocalList()
{
	CRect rect;
	CDC *pDC = GetDlgItem(IDC_LIST_LOCAL)->GetDC();
	GetDlgItem(IDC_LIST_LOCAL)->GetClientRect(rect);
	AexX = (int)(rect.Width()/4.0);
	//AexY = rect.Height();
	m_list_local.InsertColumn(0,_T("?????"));//?????
	m_list_local.InsertColumn(1,_T("?????��(???)"));
	//m_list_local.InsertColumn(2,_T("???????"));//?????
	m_list_local.InsertColumn(2,_T("??????"));
	//index1 = m_list_local.InsertColumn(1,_T("????��??"));
	// m_list_local.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES |LVS_EX_CHECKBOXES);//???? 
	//m_list_local.SetExtendedStyle(m_list_local.GetExtendedStyle()|LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP); 
	//m_list_local.ModifyStyle(1,LVS_REPORT|LVS_EX_CHECKBOXES|LVS_SHOWSELALWAYS);
	//m_list_local.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES/*|LVS_EX_CHECKBOXES*/|LVS_SHOWSELALWAYS|LVS_EX_HEADERDRAGDROP);
	m_list_local.SetColumnWidth(0, (int)(1.3*AexX));//?????��?
	m_list_local.SetColumnWidth(1, (int)(1.2*AexX));
	m_list_local.SetColumnWidth(2, (int)(1.5*AexX));

	m_list_local.SetRedraw(TRUE); 
}
void CFTPClientDlg::initServerList()
{
	CRect rect;
	CDC *pDC = GetDlgItem(IDC_LIST_SERVER)->GetDC();
	GetDlgItem(IDC_LIST_SERVER)->GetClientRect(rect);
	AexX = (int)(rect.Width()/4.0);
	//AexY = rect.Height();
	m_list_server.InsertColumn(0,_T("?????"));//?????
	m_list_server.InsertColumn(1,_T("?????��(???)"));
	//m_list_server.InsertColumn(2,_T("???????"));//?????
	m_list_server.InsertColumn(2,_T("??????"));
	//index1 = m_list_server.InsertColumn(1,_T("????��??"));
	// m_list_server.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES |LVS_EX_CHECKBOXES);//???? 
	//m_list_server.SetExtendedStyle(m_list_server.GetExtendedStyle()|LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP); 
	//m_list_server.ModifyStyle(1,LVS_REPORT|LVS_EX_CHECKBOXES|LVS_SHOWSELALWAYS);
	//m_list_server.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES/*|LVS_EX_CHECKBOXES*/|LVS_SHOWSELALWAYS|LVS_EX_HEADERDRAGDROP);

	m_list_server.SetColumnWidth(0, (int)(1.3*AexX));//?????��?
	m_list_server.SetColumnWidth(1, (int)(1.2*AexX));
	m_list_server.SetColumnWidth(2, (int)(1.5*AexX));

	m_list_server.SetRedraw(TRUE); 
}
// ????????????????CListCtrl???��?  
void CFTPClientDlg::AutoAdjustColumnWidth(CListCtrl pListCtrl)  
{  
	pListCtrl.SetRedraw(FALSE);  
	CHeaderCtrl *pHeader = pListCtrl.GetHeaderCtrl();  
	int nColumnCount = pHeader->GetItemCount();  

	for(int i = 0; i < nColumnCount; i++)  
	{  
		pListCtrl.SetColumnWidth(i, LVSCW_AUTOSIZE);  
		int nColumnWidth = pListCtrl.GetColumnWidth(i);  
		pListCtrl.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);  
		int nHeaderWidth = pListCtrl.GetColumnWidth(i);  

		pListCtrl.SetColumnWidth(i, max(nColumnWidth, nHeaderWidth));  
	}  
	pListCtrl.SetRedraw(TRUE);  
} 
CString CFTPClientDlg::GBKToUTF8(CString strGBK)
{
	setlocale(LC_ALL,"chs");
	char* p = new char[strGBK.GetLength()];
	wcstombs( p ,strGBK,strGBK.GetLength() );
	std::string out = p;
	int nLen = MultiByteToWideChar(CP_ACP, 0, out.c_str(), -1, NULL, 0);
	WCHAR * wszUTF8 = new WCHAR[nLen];
	MultiByteToWideChar(CP_ACP, 0, out.c_str(), -1, wszUTF8, nLen);

	nLen = WideCharToMultiByte(CP_UTF8, 0, wszUTF8, -1, NULL, 0, NULL, NULL);
	char * szUTF8 = new char[nLen];
	WideCharToMultiByte(CP_UTF8, 0, wszUTF8, -1, szUTF8, nLen, NULL, NULL);

	std::string strTemp(szUTF8);
	CString strstr;
	strstr.Format(_T("%s"),strTemp.c_str());
	delete[]wszUTF8;
	delete[]szUTF8;
	return strstr;
}
void CFTPClientDlg::On32784()			//????????????????????��?
{
	// TODO: ??????????????????
	CAutoUpDownLoad autoUpDown;
	autoUpDown.DoModal();
	SetTimer(1,1000,NULL);
	//t = CTime::GetCurrentTime(); //?????????
}

void CFTPClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ????????????????????/?????????
	//switch (nIDEvent)
	//	{
	//	case 1:
	//AfxMessageBox(_T("123123"));

	//	break;
	//case 2:
	//	//	AutoDownload();
	//	break;
	//case 3:
	//	//	SetTimeUpDown();
	//	break;
	//	default:
	//		break;
	//	}


	switch (nIDEvent)
	{
	case 10:
		//AutoDownload();
		AutoUpload();

		//AfxMessageBox(_T("time out of 10"));
		break;
	}


	CDialogEx::OnTimer(nIDEvent);
}
BOOL CFTPClientDlg::SetTimeUpDown()//?��??????
{
	t = CTime::GetCurrentTime(); //?????????
	int h=t.GetHour(); //??????????
	int mm=t.GetMinute(); //???????
	int s=t.GetSecond(); //?????

	CString temp_uphour,temp_upminute,temp_upsecond;
	CString temp_downhour,temp_downminute,temp_downsecond;
	temp_uphour.Format(_T("%d"),h);
	temp_upminute.Format(_T("%d"),mm);
	temp_upsecond.Format(_T("%d"),s);

	temp_downhour.Format(_T("%d"),h);
	temp_downminute.Format(_T("%d"),mm);
	temp_downsecond.Format(_T("%d"),s);

	if ((temp_uphour == up_hour)&&(temp_upminute == up_minute))
	{
		isCorrect=TRUE;
		return isCorrect;

	}
	if ((temp_downhour == down_hour)&&(temp_downminute == down_minute))
	{
		isCorrect=TRUE;
		return isCorrect;

	}
}
void CFTPClientDlg::SetUpDownMode()//?????????????
{
	//if (isCorrect)
	//{
	//	if (time_up != 0)
	//	{
	//		SetTimer(1,time_up,NULL);
	//	}
	//	if (time_down != 0)
	//	{
	//		SetTimer(2,time_down,NULL);
	//	}
	//}
}
void CFTPClientDlg::AutoDownload()//???????
{
	CCurl * pCurl = m_pCurl;  //CCurl::Instance();
	CString pathDes = TmpDownloadFolderPath;//?????????
	CString str = DownloadFolderPath; //????????
	CString strDownloadFile;
	CString strfilename;//?????
	if(str.Right(1) != '\\')  
		str += '\\';  
	str += "*.*";  
	/*CFileFind file;  */
	CFtpFileFind ftpfind(pFtpConnection);//????FTP????
	//CFtpFileFind ftpfind(pThreadFtpConnection);//????FTP????pThreadFtpConnection
	BOOL bContinue = ftpfind.FindFile(str);  
	while(bContinue)  
	{  
		bContinue = ftpfind.FindNextFile();  
		if(!ftpfind.IsDirectory() && !ftpfind.IsDots())  
		{  
			SHFILEINFO info;  
			CString temp = str; //

			//CString file_type;
			int index = temp.Find(L"*.*");  
			temp.Delete(index,3);  
			strfilename = ftpfind.GetFileName();//????????
			strDownloadFile = temp + strfilename;//?
			pathDes = pathDes + strfilename;//???
		}
		if (strfilename.GetLength()!=0)
		{
			DWORD dwNum_source_file_path = WideCharToMultiByte(CP_OEMCP,NULL,strDownloadFile,-1,NULL,NULL,0,NULL);
			char *source_file_path_after = new char[dwNum_source_file_path+1];
			WideCharToMultiByte(CP_OEMCP,NULL,strDownloadFile,-1,source_file_path_after,dwNum_source_file_path,0,NULL);//?��??
			source_file_path_after[dwNum_source_file_path]='\0';

			DWORD dwNum_dest_file_path = WideCharToMultiByte(CP_OEMCP,NULL,pathDes,-1,NULL,NULL,0,NULL);
			char *dest_file_path_after = new char[dwNum_dest_file_path+1];
			WideCharToMultiByte(CP_OEMCP,NULL,pathDes,-1,dest_file_path_after,dwNum_dest_file_path,0,NULL);//?��??
			dest_file_path_after[dwNum_dest_file_path]='\0';
			//char *pathpath = strcat(server_path_after,server_file_name_after);
			strcpy(downloadfile_info.downfile_fullpath,dest_file_path_after);
			strcpy(downloadfile_info.downfile_name,source_file_path_after);

			vDownload.vec_down.push_back(downloadfile_info);
			delete source_file_path_after;
			delete dest_file_path_after;
		}
	}
}
void CFTPClientDlg::AutoUpload()//??????
{
	CCurl * pCurl = CCurl::Instance();

	CString strSetUploadFilePath;//???????��??
	CString str = TmpUploadFolderPath; //???��??
	CString strfilename;
	CString fileNameSingle;//????��??
	CString fileNameFinal;//?????
	CString tmp_path;
	//CWnd *List = GetDlgItem(IDC_LIST_LOG);//???????
	if(str.Right(1) != '\\')  
		str += '\\';  
	str += "*.*";  
	CFileFind file;  
	BOOL bContinue = file.FindFile(str);  //find ?????
	while(bContinue)  
	{  
		bContinue = file.FindNextFile();  
		char temp_path[1024];//????????txt????��??????��??
		if(!file.IsDirectory() && !file.IsDots())  
		{  
			SHFILEINFO info;  
			CString temp = str; //��?? 
			int index = temp.Find(L"*.*");  
			temp.Delete(index,3);  
			strfilename = file.GetFileName();
			strSetUploadFilePath = temp + strfilename;//????????????��??
			DWORD dwNum_file_name = WideCharToMultiByte(CP_ACP,NULL,strSetUploadFilePath,-1,NULL,NULL,0,NULL);
			char *temp_filename = new char[dwNum_file_name];
			WideCharToMultiByte(CP_OEMCP,NULL,strSetUploadFilePath,-1,temp_filename,dwNum_file_name,0,NULL);
			if(strstr(temp_filename,".path.txt"))//???txt????��?????		//?????????.path.txt?????????????????
			{
				FILE *temp_pathFile;
				temp_pathFile = fopen(temp_filename,"r+");
				if(temp_pathFile != NULL)
					fgets(temp_path,1024,temp_pathFile);
				temp_path[strlen(temp_path)-1] = 0;
				/*?????????????��???��??????*/
				//fileNameSingle.Format(_T("%s"),temp_path);
				fileNameSingle=temp_path;
				int pos_temp = fileNameSingle.ReverseFind('\\');
				fileNameFinal = fileNameSingle.Mid(pos_temp+1);

				/*?????????????��??????????push??vector??*/
				DWORD dwNum_fileNameFinal = WideCharToMultiByte(CP_ACP,NULL,fileNameFinal,-1,NULL,NULL,0,NULL);
				char *fileName_needUp = new char[dwNum_fileNameFinal];
				WideCharToMultiByte(CP_OEMCP,NULL,fileNameFinal,-1,fileName_needUp,dwNum_fileNameFinal,0,NULL);

				struct stat file_info;
				FileInfo psrc_local;
				//if(stat(temp_path,&file_info)==0)
				FILE *getfile_size = fopen(temp_path,"r+");
				if (getfile_size == nullptr)
				{
					int err = GetLastError();
					CLOG("?????%s???,err= %d",temp_path,err);
					//return 0;
				}
				else
				{
					_fseeki64(getfile_size, 0L, SEEK_END);
					long long  file_size = _ftelli64(getfile_size);
					fclose(getfile_size);

					if(file_size != 0)
					{
						//psrc_local.filesize = file_info.st_size;	
						//CString msg;
						//msg.Format(_T("%d"),file_info.st_size);//intToCString
						//msg.Format(_T("%l64d"),file_size);//intToCString
						//DWORD dwNum_msg = WideCharToMultiByte(CP_OEMCP,NULL,msg,-1,NULL,NULL,0,NULL);
						//WideCharToMultiByte(CP_OEMCP,NULL,msg,-1,msg_after,dwNum_msg,0,NULL);//CString To Char*
						psrc_local.filesize = file_size;
						char temp_file[256];//=new char[100];//???????????��????????
						sprintf(temp_file,"%s.txt",temp_path);
						FILE *temp ;
						temp = fopen(temp_file,"w+"); 
						if(temp !=NULL)
						{
							//fprintf(temp,"%s\n",msg_after);
							fprintf(temp,"%lld\n",file_size);
							char temp_content[1024];
							while(!feof(temp_pathFile))
							{
								memset(temp_content,0,sizeof(temp_content));
								fgets(temp_content,1024,temp_pathFile);
								fprintf(temp,"%s",temp_content);
							}
							fclose(temp);
							/*???txt???*/
							//tmp_path.Format(_T("%s"),temp_filename);
							tmp_path = temp_filename;
							fclose(temp_pathFile);
							if(!DeleteFile(tmp_path))		//?????????txt???
							{
								int err = GetLastError();
								CLOG("ERROR err = %d",err);
							}
						}

						CString size_filename;
						//fileNameSingle.Format(_T("%s"),temp_path);
						CString size_path;
						size_path = temp_file;
						int pos_temp = size_path.ReverseFind('\\');
						size_filename = size_path.Mid(pos_temp+1);

						DWORD dwNum_size_file = WideCharToMultiByte(CP_ACP,NULL,size_filename,-1,NULL,NULL,0,NULL);
						char *sizefile = new char[dwNum_size_file+1];
						WideCharToMultiByte(CP_OEMCP,NULL,size_filename,-1,sizefile,dwNum_size_file,0,NULL);
						sizefile[dwNum_size_file]='\0';

						strcpy(uploadfile_info.upfile_fullpath,temp_file);
						strcpy(uploadfile_info.upfile_name,sizefile);
						delete sizefile;
						vUpload.vec_up.push_back(uploadfile_info);		//????txt??????

						strcpy(uploadfile_info.upfile_fullpath,temp_path);
						strcpy(uploadfile_info.upfile_name,fileName_needUp);
						vUpload.vec_up.push_back(uploadfile_info);		//?????????????��?
					}
				}

			}
		}
	}
}
long long CFTPClientDlg::get_file_size(FILE *file)
{
	if (file == nullptr)
	{
		return 0;
	}

	_fseeki64(file, 0L, SEEK_END);
	long long  nFileSize = _ftelli64(file);
	_fseeki64(file, 0L, SEEK_SET);

	return nFileSize;
}


//??��???1.?????char *p?? 2.????????? 3.???char*p 4.delete p??????
void CFTPClientDlg::CString2char(char* &p,CString cstr)  
{
	DWORD len = WideCharToMultiByte(CP_ACP,NULL,cstr,-1,NULL,NULL,0,NULL);
	p = new char[len+1];
	p[len]='\0';
	WideCharToMultiByte(CP_OEMCP,NULL,cstr,-1,p,len,0,NULL);
}

//void CAboutDlg::On32785()
//{
//	// TODO: ??????????????????
//	KillTimer(1);
//	KillTimer(2);
//}

LRESULT CFTPClientDlg::OnShowTask(WPARAM wParam, LPARAM lParam)
{
	if(wParam != IDI_ICON1) 
		return 1;
	switch(lParam)
	{
	case WM_RBUTTONUP://??????????????
		{
			LPPOINT lpoint = new tagPOINT;
			::GetCursorPos(lpoint);//??????��??
			CMenu menu;
			menu.CreatePopupMenu();//??????????????
			//menu.AppendMenu(MF_STRING, WM_DESTROY, "???");//Appends a new item to the end ofthis menu
			//menu.AppendMenu(MF_STRING, WM_DESTROY,_T("???"));
			menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x ,lpoint->y,this);
			HMENU hmenu = menu.Detach();
			menu.DestroyMenu();
			delete lpoint;
			break;
		}
	case WM_LBUTTONDBLCLK://???????????
		this->ShowWindow(SW_SHOWNORMAL);//?????????
	}
	return 0;
}





// FTPClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FTPClient.h"
#include "FTPClientDlg.h"
#include "afxdialogex.h"
#include "ccurl.h"
#include "outctrl.h"
#include "AutoUpDownLoad.h"
#include <stdlib.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;

//char *user_local;//用户名
//char *pass_local;//密码
//char *host_local;//端口
extern int time_up,time_down;//定时上传下载时间
extern CString TmpUploadFolderPath;//定时上传文件夹路径保存
extern CString TmpDownloadFolderPath;//定时下载
extern CString DownloadFolderPath;//定时下载文件夹路径
extern CString up_hour,up_minute,up_second;//上传开启时间
extern CString down_hour,down_minute,down_second;//下载开启时间

vec_upload vUpload;
up_info uploadfile_info;

vec_download vDownload;
download_info downloadfile_info;

vec_auto_upload vAutoUpload;
auto_up autoUpload_info;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
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
END_MESSAGE_MAP()


// CFTPClientDlg 对话框



CFTPClientDlg::CFTPClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFTPClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
	//ON_WM_TIMER()
	ON_COMMAND(ID_32786, &CFTPClientDlg::OnDownload)
END_MESSAGE_MAP()



CFTPClientDlg::~CFTPClientDlg()
{
	CoUninitialize(); // 程序结束时
}
// CFTPClientDlg 消息处理程序

BOOL CFTPClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CoInitialize(NULL); // 程序启动时
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CRect temprect(0,0,905,765);
	CWnd::SetWindowPos(NULL,0,0,temprect.Width(),temprect.Height(),SWP_NOZORDER|SWP_NOMOVE);
	m_ImageList.Create(32,32,ILC_COLOR32,10,30);     //创建图像序列CImageList对象   
	m_list_local.SetImageList(&m_ImageList,LVSIL_NORMAL);  //为树形控件设置图像序列     
	m_tree_local.ModifyStyle(NULL,TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS);  
	m_hRoot = m_tree_local.InsertItem(_T("我的电脑"));         //插入根节点  
	GetLogicalDrives(m_hRoot);                      //自定义函数 获取驱动  
	GetDriveDir(m_hRoot);                           //自定义函数 获取驱动子项  
	m_tree_local.Expand(m_hRoot,TVE_EXPAND);              //展开或折叠子项列表 TVE_EXPAND展开列表  
	initLocalList();//初始化本地文件列表
	initServerList();//初始化服务器文件列表

	HANDLE Thread_Upload_File; 
	Thread_Upload_File = CreateThread(NULL,0,Thread_UploadFile,this,0,NULL );//线程上传

	//CCurl * pCurl = CCurl::Instance();
	//pCurl->setHostUserPwd("1", "1", "localhost");
	//if (pCurl->upload("E:\vcredist.bmp","vcredist.bmp"))
	//{
	//	//CLOG("upload ok");
	//	m_list_log.InsertString(0,_T("上传成功"));
	//	MessageBox(_T("upload ok"),_T("状态"),MB_OK);
	//	
	//}
	//else 
	//{
	//	m_list_log.InsertString(0,_T("上传失败"));
	//	MessageBox(_T("upload error"),_T("状态"),MB_OK);
	//}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

	// TODO: 在此处添加消息处理程序代码
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFTPClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFTPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFTPClientDlg::GetLogicalDrives(HTREEITEM hParent)
{
	//获取系统分区驱动器字符串信息  
	//size_t szAllDriveStrings = GetLogicalDriveStrings(0,NULL);           //驱动器总长度  
	//char *pDriveStrings = new char[szAllDriveStrings + sizeof(_T(""))];  //建立数组  

	//GetLogicalDriveStrings(szAllDriveStrings,(LPWSTR)pDriveStrings);

	////GetLogicalDriveStrings(szAllDriveStrings,);
	//size_t szDriveString = strlen(pDriveStrings);                        //驱动大小  
	//while(szDriveString > 0)  
	//{   
	//	m_tree_local.InsertItem((LPWSTR)pDriveStrings,hParent);       //在父节点hParent下添加盘符  
	//	//pDriveStrings += szDriveString + 1;             //pDriveStrings即C:\ D:\ E:\盘  
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
			m_tree_local.InsertItem(p,hParent);       //在父节点hParent下添加盘符  
			//wszResult += wszResult + 1;             //pDriveStrings即C:\ D:\ E:\盘  
			//szDriveString = strlen(pDriveStrings);  

			while(*p++)
				;

			//szDriveString = strlen(CStr); 
		} 
	}
}
void CFTPClientDlg::GetDriveDir(HTREEITEM hParent)
{
	HTREEITEM hChild = m_tree_local.GetChildItem(hParent);   //获取指定位置中的子项  
	while(hChild)                                        
	{  
		CString strText = m_tree_local.GetItemText(hChild);  //检索列表中项目文字  
		if(strText.Right(1) != '\\')                   //从右边1开始获取从右向左nCount个字符  
			strText += _T('\\');  
		strText += "*.*";  
		//将当前目录下文件枚举并InsertItem树状显示  
		CFileFind file;                                       //定义本地文件查找  
		BOOL bContinue = file.FindFile(strText);              //查找包含字符串的文件  
		while(bContinue)  
		{  
			bContinue = file.FindNextFile();                  //查找下一个文件  
			if(file.IsDirectory() && !file.IsDots())          //找到文件为内容且不为点"."  
				m_tree_local.InsertItem(file.GetFileName(),hChild); //添加盘符路径下树状文件夹  
		}  
		GetDriveDir(hChild);                                  //递归调用  
		hChild = m_tree_local.GetNextItem(hChild,TVGN_NEXT);        //获取树形控件TVGN_NEXT下兄弟项  
	}  
}

void CFTPClientDlg::AddSubDir(HTREEITEM hParent)
{
	CString strPath = GetFullPath(hParent);     //获取全路径  
	if(strPath.Right(1) != '\\')  
		strPath += '\\';  
	strPath += "*.*";  
	CFileFind file;  
	BOOL bContinue = file.FindFile(strPath);    //查找包含字符串的文件  
	while(bContinue)  
	{  
		bContinue = file.FindNextFile();        //查找下一个文件  
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
		strTemp = m_tree_local.GetItemText(hCurrent);    //检索列表中项目文字  
		if(strTemp.Right(1) != '\\')  
			strTemp += '\\';  
		strReturn = strTemp  + strReturn;          
		hCurrent = m_tree_local.GetParentItem(hCurrent); //返回父项目句柄  
	}  
	return strReturn;  
}

void CFTPClientDlg::OnTvnItemexpandedTreeLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	TVITEM item = pNMTreeView->itemNew;                  //发送\接受关于树形视图项目信息  
	if(item.hItem == m_hRoot)  
		return;  
	HTREEITEM hChild = m_tree_local.GetChildItem(item.hItem);  //获取指定位置中的子项  
	while(hChild)  
	{  
		AddSubDir(hChild);                               //添加子目录  
		hChild = m_tree_local.GetNextItem(hChild,TVGN_NEXT);   //获取树形控件TVGN_NEXT下兄弟项  
	}  
	*pResult = 0;
}


void CFTPClientDlg::OnTvnSelchangedTreeLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
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
	BOOL bContinue = file.FindFile(str);  //find 子文件
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

			//获取本地文件各类信息
			WIN32_FIND_DATA ffd ;
			HANDLE hFind = FindFirstFile(strSetIcon,&ffd);
			SYSTEMTIME stUTC, stLocal;
			FileTimeToSystemTime(&(ffd.ftLastWriteTime), &stUTC);//获取文件最后修改时间
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);//系统时间转换
			FileSize.Format(_T("%d"),ffd.nFileSizeLow);
			LastWriteTime.Format(_T("%d/%d/%d %d:%d:%d"),stLocal.wYear,stLocal.wMonth,stLocal.wDay,stLocal.wHour,stLocal.wMinute,stLocal.wSecond);//格式化时间显示
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
	// TODO: 在此添加控件通知处理程序代码
	CPoint point;
	TCHAR  buffer[BUFSIZE];   
	GetCursorPos(&point);//获得鼠标点击的位置
	m_tree_local.ScreenToClient(&point);//转化为客户坐标
	UINT uFlags;

	HTREEITEM CurrentItem;
	/*CString local_path;*/
	CurrentItem=m_tree_local.HitTest(point,&uFlags);//获得当前点击节点的ITEM
	local_path = GetFullPath(CurrentItem);
	m_edit_local.SetWindowText(local_path);
	*pResult = 0;
}



void CFTPClientDlg::OnNMRClickListLocal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
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
	//获取所选择文件路径
	filefile = local_path;
	local_file_name = m_list_local.GetItemText( pNMListView->iItem, pNMListView->iSubItem ) ;
	filefile =  filefile  + local_file_name;
	//MessageBox(filefile);
	*pResult = 0;
}
void CFTPClientDlg::OnNMRClickListServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
	m_edit_host.GetWindowText(host);
	m_edit_user.GetWindowText(user);
	m_edit_pass.GetWindowText(pass);
	m_edit_port.GetWindowText(port);
	ConnectFtp(host,user,pass,port);
	if(bconnect)
	{
		m_list_log.InsertString(0,_T("连接成功"));
		m_serverRoot=m_tree_server.InsertItem(_T("服务器"));
		/*m_tree_server.ModifyStyle(NULL,TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS);  */
		UpdateDir(m_serverRoot);
		GetFTPDriveDir(m_serverRoot);
		AfxMessageBox(_T("连接成功"));
	}

}

DWORD WINAPI CFTPClientDlg::Thread_UploadFile(LPVOID pParam)//接收udp数据
{
	while(1)
	{
		/*vec_upload vUpload;
		up_info up_fileinfo;
		vec_upload vUpload;
		up_info uploadfile_info;
		*/
		CFTPClientDlg::AutoUpload();

		int fileNum = vUpload.vec_up.size();
		CCurl * pCurl = CCurl::Instance();
		if(fileNum != 0)
		{
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

			for(int i = 0;i<fileNum;i++)
			{
				uploadfile_info = vUpload.vec_up.at(0);
				if (pCurl->upload(uploadfile_info.upfile_fullpath,uploadfile_info.upfile_name))
				{
					//CLOG("upload ok");
					m_list_log.InsertString(0,_T("上传成功"));
					vUpload.vec_up.erase(vUpload.vec_up.begin()+0);
					//vUpload.vec_up.Remo(i);
					//MessageBox(_T("upload ok"),_T("状态"),MB_OK);

				}
				else 
				{
					m_list_log.InsertString(0,_T("上传失败"));
					Sleep(3000);
					//m_list_log.InsertString(0,_T("上传失败"));
					//MessageBox(host,_T("状态"),MB_OK);
				}
			}
			/*vUpload.vec_up.clear();*/

		}
		else
		{
			Sleep(1000);
		}
	}
	return 0;
}
void CFTPClientDlg::OnUpload()
{
	// TODO: 在此添加命令处理程序代码
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
DWORD WINAPI CFTPClientDlg::Thread_DownloadFile(LPVOID pParam)//线程下载文件
{
	int file_down_num = vDownload.vec_down.size();
	CCurl * pCurl = CCurl::Instance();

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

	for(int i = 0;i<file_down_num;i++)
	{
		downloadfile_info = vDownload.vec_down.at(i);
		if (pCurl->download(downloadfile_info.downfile_name,downloadfile_info.downfile_fullpath))
		{
			//CLOG("down ok 222");
			CString CG;
			//CG.Format(_T("下载文件：%s 成功"),downloadfile_info.downfile_name);
			//m_list_log.InsertString(0,CG);
			m_list_log.InsertString(0,_T("下载成功"));
			//MessageBox(_T("download ok"),_T("状态"),MB_OK);
		}
		else
		{
			m_list_log.InsertString(0,_T("下载失败"));
			//MessageBox(_T("download fail"),_T("状态"),MB_OK);
		}
	}
	vDownload.vec_down.clear();
	return 0;
}
void CFTPClientDlg::OnDownload()
{
	// TODO: 在此添加命令处理程序代码
	CCurl * pCurl = CCurl::Instance();

	DWORD dwNum_server_file_name = WideCharToMultiByte(CP_OEMCP,NULL,server_file_name,-1,NULL,NULL,0,NULL);
	char *server_file_name_after = new char[dwNum_server_file_name];
	WideCharToMultiByte(CP_OEMCP,NULL,server_file_name,-1,server_file_name_after,dwNum_server_file_name,0,NULL);

	DWORD dwNum_server_path = WideCharToMultiByte(CP_OEMCP,NULL,server_path,-1,NULL,NULL,0,NULL);
	char *server_path_after = new char[dwNum_server_path];
	WideCharToMultiByte(CP_OEMCP,NULL,server_path,-1,server_path_after,dwNum_server_path,0,NULL);

	//char szPath[MAX_PATH];     //存放选择的目录路径 
	/*CString server_path[MAX_PATH];*/
	CString str;
	//ZeroMemory(szPath, sizeof(szPath));
	//ZeroMemory(server_path, sizeof(server_path));

	BROWSEINFO bi;   
	bi.hwndOwner = m_hWnd;   
	bi.pidlRoot = NULL;   
	bi.pszDisplayName = (LPWSTR)server_dpath;   
	bi.lpszTitle = _T("请选择需要打包的目录：");   
	bi.ulFlags = 0;   
	bi.lpfn = NULL;   
	bi.lParam = 0;   
	bi.iImage = 0;   
	//弹出选择目录对话框
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);  
	if(lp && SHGetPathFromIDList(lp, (LPWSTR)server_dpath))   
	{
		str.Format(_T("%s"), server_dpath);
		AfxMessageBox(str);
	}
	else   
		AfxMessageBox(_T("无效的目录，请重新选择"));  

	if(str.Right(1) != '\\')  
		str += '\\';
	server_file_path = str + server_file_name;

	DWORD dwNum_server_file_path = WideCharToMultiByte(CP_OEMCP,NULL,server_file_path,-1,NULL,NULL,0,NULL);
	char *server_file_path_after = new char[dwNum_server_file_path];
	WideCharToMultiByte(CP_OEMCP,NULL,server_file_path,-1,server_file_path_after,dwNum_server_file_path,0,NULL);

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
	char *pathpath = strcat(server_path_after,server_file_name_after);

	downloadfile_info.downfile_fullpath = server_file_path_after;//下载路径
	downloadfile_info.downfile_name = pathpath;//下载文件名
	vDownload.vec_down.push_back(downloadfile_info);//将文件信息push到容器中

	HANDLE Thread_Download_File; 
	Thread_Download_File = CreateThread(NULL,0,Thread_DownloadFile,this,0,NULL );//线程上传

	//if (pCurl->download(pathpath,server_file_path_after))
	//{
	//	//CLOG("down ok 222");
	//	CString CG;
	//	CG.Format(_T("下载文件：%s 成功"),server_file_name);
	//	m_list_log.InsertString(0,CG);
	//	//MessageBox(_T("download ok"),_T("状态"),MB_OK);
	//}
	//else
	//{
	//	m_list_log.InsertString(0,_T("下载失败"));
	//	//MessageBox(_T("download fail"),_T("状态"),MB_OK);
	//}
}


void CFTPClientDlg::ConnectFtp(CString sip,CString strusr,CString strpwd,CString strport){
	BYTE nFild[4];
	UpdateData();
	//ServerIP.GetAddress(nFild[0],nFild[1],nFild[2],nFild[3]);
	//CString sip;
	//sip.Format("%d.%d.%d.%d",nFild[0],nFild[1],nFild[2],nFild[3]);
	if (sip.IsEmpty())
	{
		AfxMessageBox(_T("IP地址为空！"));
		return;
	}
	if (strport.IsEmpty())
	{
		AfxMessageBox(_T("端口号为空！"));
		return;
	}
	if (strusr.IsEmpty())
	{
		return;
	}
	//建立一个Internet会话
	pInternetSession= new CInternetSession(_T("MR"),INTERNET_OPEN_TYPE_PRECONFIG);

	DWORD dwNum_port = WideCharToMultiByte(CP_ACP,NULL,strport,-1,NULL,NULL,0,NULL);
	char *port = new char[dwNum_port];
	WideCharToMultiByte(CP_OEMCP,NULL,strport,-1,port,dwNum_port,0,NULL);
	//try
	//{
	//利用Internet会话对象pInternetSession打开一个FTP连接
	pFtpConnection=pInternetSession->GetFtpConnection(sip,strusr,strpwd,atoi(port));
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
	//m_list_server.DeleteAllItems(); // 全部清空
	//读写服务器中的数据，需要创建一个CFtpFileFind的实例	
	//找到第一个文件或者文件夹，通过CFtpFileFind::FindFile实现
	//BOOL bfind=ftpfind.FindFile(NULL);
	//m_tree_server.Expand(hParent,TVE_EXPAND);              //展开或折叠子项列表 TVE_EXPAND展开列表  
	//GetFTPDriveDir(hParent); 
	//m_ImageList.Create(32,32,ILC_COLOR32,10,30);     //创建图像序列CImageList对象   
	//m_list_server.SetImageList(&m_ImageList,LVSIL_NORMAL);  //为树形控件设置图像序列   
	m_tree_server.ModifyStyle(NULL,TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_EDITLABELS);  
	CFtpFileFind ftpfind(pFtpConnection);
	BOOL bfind=ftpfind.FindFile(NULL);
	while (bfind)
	{
		bfind=ftpfind.FindNextFile();
		CString strpath;
		if(ftpfind.IsDirectory() && !ftpfind.IsDots())          //找到文件为内容且不为点"."  
			m_tree_server.InsertItem(ftpfind.GetFileName(),/*hChild*/hParent); //添加盘符路径下树状文件夹  
	}

	m_tree_server.Expand(hParent,TVE_EXPAND);              //展开或折叠子项列表 TVE_EXPAND展开列表  
}

void CFTPClientDlg::GetFTPDriveDir(HTREEITEM hParent)
{
	HTREEITEM hChild = m_tree_server.GetChildItem(hParent);   //获取指定位置中的子项  
	while(hChild)                                        
	{ 
		//将当前目录下文件枚举并InsertItem树状显示                                
		CString strText = m_tree_server.GetItemText(hChild);  //检索列表中项目文字  
		if(strText.Right(1) != '\\')                   //从右边1开始获取从右向左nCount个字符  
			strText += _T('\\');  
		strText += "*.*";  
		CFtpFileFind ftpfind(pFtpConnection);//定义FTP查找
		//BOOL bContinue = ftpfind.FindFile(strText);              //查找包含字符串的文件  
		BOOL bContinue = ftpfind.FindFile(strText);              //查找包含字符串的文件 
		while(bContinue)  
		{  
			bContinue = ftpfind.FindNextFile();                  //查找下一个文件  
			if(ftpfind.IsDirectory() && !ftpfind.IsDots())          //找到文件为内容且不为点"."  
				m_tree_server.InsertItem(ftpfind.GetFileName(),hChild/*hParent*/); //添加盘符路径下树状文件夹  
		}  
		GetFTPDriveDir(hChild/*hParent*/);                                  //递归调用  
		hChild = m_tree_server.GetNextItem(hChild,TVGN_NEXT);        //获取树形控件TVGN_NEXT下兄弟项  
	}  
	//m_tree_server.Expand(hParent,TVE_EXPAND);              //展开或折叠子项列表 TVE_EXPAND展开列表 
}
CString CFTPClientDlg::GetFTPFullPath(HTREEITEM hCurrent)
{
	CString strTemp;  
	CString strReturn = L"";  
	while(hCurrent != m_serverRoot)  
	{  
		strTemp = m_tree_server.GetItemText(hCurrent);    //检索列表中项目文字  
		if(strTemp.Right(1) != '\\')  
			strTemp += '\\';  
		strReturn = strTemp  + strReturn;          
		hCurrent = m_tree_server.GetParentItem(hCurrent); //返回父项目句柄  
	}  
	return strReturn;  
}
void CFTPClientDlg::AddFTPSubDir(HTREEITEM hParent)
{
	CString strPath = GetFTPFullPath(hParent);     //获取全路径  
	if(strPath.Right(1) != '\\')  
		strPath += '\\';  
	strPath += "*.*";  
	//CFileFind file;  
	CFtpFileFind ftpfind(pFtpConnection);//定义FTP查找
	BOOL bContinue = ftpfind.FindFile(strPath);    //查找包含字符串的文件  
	while(bContinue)  
	{  
		bContinue = ftpfind.FindNextFile();        //查找下一个文件  
		if(ftpfind.IsDirectory() && !ftpfind.IsDots())  
			m_tree_server.InsertItem(ftpfind.GetFileName(),hParent);  
	}  
}



void CFTPClientDlg::OnTvnItemexpandedTreeServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	TVITEM item = pNMTreeView->itemNew;                  //发送\接受关于树形视图项目信息  
	if(item.hItem == m_serverRoot)  
		return;  
	HTREEITEM hChild = m_tree_server.GetChildItem(item.hItem);  //获取指定位置中的子项  
	while(hChild)  
	{  
		AddFTPSubDir(hChild);                               //添加子目录  
		hChild = m_tree_server.GetNextItem(hChild,TVGN_NEXT);   //获取树形控件TVGN_NEXT下兄弟项  
	} 
	*pResult = 0;
}
void CFTPClientDlg::OnNMClickTreeServer(NMHDR *pNMHDR, LRESULT *pResult)//点击服务端树形控件
{
	// TODO: 在此添加控件通知处理程序代码
	CPoint point;
	TCHAR  buffer[BUFSIZE];   
	GetCursorPos(&point);//获得鼠标点击的位置
	m_tree_server.ScreenToClient(&point);//转化为客户坐标
	UINT uFlags;

	HTREEITEM CurrentItem;
	//CString server_path;//服务器路径
	CurrentItem=m_tree_server.HitTest(point,&uFlags);//获得当前点击节点的ITEM
	server_path = GetFTPFullPath(CurrentItem);

	m_edit_server.SetWindowText(server_path);
	*pResult = 0;
}

void CFTPClientDlg::OnTvnSelchangedTreeServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
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
	CFtpFileFind ftpfind(pFtpConnection);//定义FTP查找
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
			CString strfilename = ftpfind.GetFileName();//获取文件名
			CString strSetIcon = temp + strfilename;

			FileSize.Format(_T("%I64d"),ftpfind.GetLength());//获取服务端文件大小
			CTime time;//服务器时间变量
			ftpfind.GetLastWriteTime(time);//获取服务端时间
			LastWriteTime.Format(_T("%d/%d/%d %d:%d:%d"),time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());//格式化显示时间

			//插入服务端list表格
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
			//获取本地文件各类信息
			//WIN32_FIND_DATA ffd ;
			//HANDLE hFind = FtpFindFirstFile(pFtpConnection,strSetIcon,&ffd,2,0);
			//SYSTEMTIME stUTC, stLocal;
			//FileTimeToSystemTime(&(ffd.ftLastWriteTime), &stUTC);//获取文件最后修改时间
			//SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);//系统时间转换
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
	m_list_local.InsertColumn(0,_T("文件名"));//添加列
	m_list_local.InsertColumn(1,_T("文件大小(字节)"));
	//m_list_local.InsertColumn(2,_T("文件类型"));//添加列
	m_list_local.InsertColumn(2,_T("最近修改"));
	//index1 = m_list_local.InsertColumn(1,_T("所在路径"));
	// m_list_local.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES |LVS_EX_CHECKBOXES);//属性 
	//m_list_local.SetExtendedStyle(m_list_local.GetExtendedStyle()|LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP); 
	//m_list_local.ModifyStyle(1,LVS_REPORT|LVS_EX_CHECKBOXES|LVS_SHOWSELALWAYS);
	//m_list_local.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES/*|LVS_EX_CHECKBOXES*/|LVS_SHOWSELALWAYS|LVS_EX_HEADERDRAGDROP);
	m_list_local.SetColumnWidth(0, (int)(1.3*AexX));//设置列宽
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
	m_list_server.InsertColumn(0,_T("文件名"));//添加列
	m_list_server.InsertColumn(1,_T("文件大小(字节)"));
	//m_list_server.InsertColumn(2,_T("文件类型"));//添加列
	m_list_server.InsertColumn(2,_T("最近修改"));
	//index1 = m_list_server.InsertColumn(1,_T("所在路径"));
	// m_list_server.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES |LVS_EX_CHECKBOXES);//属性 
	//m_list_server.SetExtendedStyle(m_list_server.GetExtendedStyle()|LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP); 
	//m_list_server.ModifyStyle(1,LVS_REPORT|LVS_EX_CHECKBOXES|LVS_SHOWSELALWAYS);
	//m_list_server.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES/*|LVS_EX_CHECKBOXES*/|LVS_SHOWSELALWAYS|LVS_EX_HEADERDRAGDROP);

	m_list_server.SetColumnWidth(0, (int)(1.3*AexX));//设置列宽
	m_list_server.SetColumnWidth(1, (int)(1.2*AexX));
	m_list_server.SetColumnWidth(2, (int)(1.5*AexX));

	m_list_server.SetRedraw(TRUE); 
}
// 根据内容，自动调整CListCtrl的列宽  
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
void CFTPClientDlg::On32784()
{
	// TODO: 在此添加命令处理程序代码
	CAutoUpDownLoad autoUpDown;
	autoUpDown.DoModal();
	SetTimer(1,1000,NULL);
	//t = CTime::GetCurrentTime(); //获取系统日期
}

void CFTPClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//switch (nIDEvent)
	//	{
	//	case 1:
	AfxMessageBox(_T("123123"));

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

	CDialogEx::OnTimer(nIDEvent);
}
BOOL CFTPClientDlg::SetTimeUpDown()//判断当前时间
{
	t = CTime::GetCurrentTime(); //获取系统日期
	int h=t.GetHour(); //获取当前为几时
	int mm=t.GetMinute(); //获取分钟
	int s=t.GetSecond(); //获取秒

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
void CFTPClientDlg::SetUpDownMode()//设置上传下载模式
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
void CFTPClientDlg::AutoDownload()//定时下载
{
	CCurl * pCurl = CCurl::Instance();
	CString pathDes = TmpDownloadFolderPath;//下载目的地址
	CString str = DownloadFolderPath; //下载源地址
	CString strDownloadFile;
	CString strfilename;//文件名
	if(str.Right(1) != '\\')  
		str += '\\';  
	str += "*.*";  
	/*CFileFind file;  */
	CFtpFileFind ftpfind(pFtpConnection);//定义FTP查找
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
			strfilename = ftpfind.GetFileName();//获取文件名
			strDownloadFile = temp + strfilename;//源
			pathDes = pathDes + strfilename;//目的
		}
		if (strfilename.GetLength()!=0)
		{
			DWORD dwNum_source_file_path = WideCharToMultiByte(CP_OEMCP,NULL,strDownloadFile,-1,NULL,NULL,0,NULL);
			char *source_file_path_after = new char[dwNum_source_file_path];
			WideCharToMultiByte(CP_OEMCP,NULL,server_file_path,-1,source_file_path_after,dwNum_source_file_path,0,NULL);//源路径

			DWORD dwNum_dest_file_path = WideCharToMultiByte(CP_OEMCP,NULL,pathDes,-1,NULL,NULL,0,NULL);
			char *dest_file_path_after = new char[dwNum_dest_file_path];
			WideCharToMultiByte(CP_OEMCP,NULL,pathDes,-1,dest_file_path_after,dwNum_dest_file_path,0,NULL);//源路径


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
			//char *pathpath = strcat(server_path_after,server_file_name_after);
			if (pCurl->download(source_file_path_after,dest_file_path_after))
			{
				//CLOG("down ok 222");
				CString CG;
				CG.Format(_T("下载文件：%s 成功"),strfilename);
				m_list_log.InsertString(0,CG);
				//MessageBox(_T("download ok"),_T("状态"),MB_OK);
			}
			else
			{
				m_list_log.InsertString(0,_T("下载失败"));
				//MessageBox(_T("download fail"),_T("状态"),MB_OK);
			}
		}
	}
}
void CFTPClientDlg::AutoUpload()//定时上传
{
	CCurl * pCurl = CCurl::Instance();

	CString strSetUploadFilePath;//文件名加路径
	CString str = TmpUploadFolderPath; //上传路径
	CString strfilename;
	CString fileNameSingle;//文件全路径
	CString fileNameFinal;//文件名
	CString tmp_path;
	//CWnd *List = GetDlgItem(IDC_LIST_LOG);//插入日志
	if(str.Right(1) != '\\')  
		str += '\\';  
	str += "*.*";  
	CFileFind file;  
	BOOL bContinue = file.FindFile(str);  //find 子文件
	while(bContinue)  
	{  
		bContinue = file.FindNextFile();  
		char temp_path[1024];//读取并存放txt文件中存放的文件路径
		if(!file.IsDirectory() && !file.IsDots())  
		{  
			SHFILEINFO info;  
			CString temp = str; //路径 
			int index = temp.Find(L"*.*");  
			temp.Delete(index,3);  
			strfilename = file.GetFileName();
			strSetUploadFilePath = temp + strfilename;//获取的文件名加路径
			DWORD dwNum_file_name = WideCharToMultiByte(CP_ACP,NULL,strSetUploadFilePath,-1,NULL,NULL,0,NULL);
			char *temp_filename = new char[dwNum_file_name];
			WideCharToMultiByte(CP_OEMCP,NULL,strSetUploadFilePath,-1,temp_filename,dwNum_file_name,0,NULL);
			if(strstr(temp_filename,".path.txt"))//读取txt文件中的内容
			{
				FILE *temp_pathFile;
				temp_pathFile = fopen(temp_filename,"r+");
				if(temp_pathFile != NULL)
					fgets(temp_path,1024,temp_pathFile);
				temp_path[strlen(temp_path)-1] = 0;
				/*将文件名从文件全路径中截取出来*/
				//fileNameSingle.Format(_T("%s"),temp_path);
				fileNameSingle=temp_path;
				int pos_temp = fileNameSingle.ReverseFind('\\');
				fileNameFinal = fileNameSingle.Mid(pos_temp+1);

				/*将获取到的文件全路径以及文件名push到vector中*/
				DWORD dwNum_fileNameFinal = WideCharToMultiByte(CP_ACP,NULL,fileNameFinal,-1,NULL,NULL,0,NULL);
				char *fileName_needUp = new char[dwNum_fileNameFinal];
				WideCharToMultiByte(CP_OEMCP,NULL,fileNameFinal,-1,fileName_needUp,dwNum_fileNameFinal,0,NULL);
				//uploadfile_info.upfile_name = fileName_needUp;
				//uploadfile_info.upfile_fullpath = temp_path;
				/*strcpy(uploadfile_info.upfile_fullpath,temp_path);
				strcpy(uploadfile_info.upfile_name,fileName_needUp);
				vUpload.vec_up.push_back(uploadfile_info);*/

				struct stat file_info;
				FileInfo psrc_local;
				//if(stat(temp_path,&file_info)==0)
				FILE *getfile_size = fopen(temp_path,"r+");
				long long file_size = get_file_size(getfile_size);
				if(file_size != 0)
				{
					//psrc_local.filesize = file_info.st_size;	
					//CString msg;
					//msg.Format(_T("%d"),file_info.st_size);//intToCString
					//msg.Format(_T("%l64d"),file_size);//intToCString
					//DWORD dwNum_msg = WideCharToMultiByte(CP_OEMCP,NULL,msg,-1,NULL,NULL,0,NULL);
					//WideCharToMultiByte(CP_OEMCP,NULL,msg,-1,msg_after,dwNum_msg,0,NULL);//CString To Char*
					psrc_local.filesize = file_size;
					char temp_file[256];//=new char[100];//存放上传文件大小的临时文件
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
						fclose(temp_pathFile);
						/*删除txt文件*/
						//tmp_path.Format(_T("%s"),temp_filename);
						tmp_path = temp_filename;
						DeleteFile(tmp_path);		
					}
					CString size_filename;
					//fileNameSingle.Format(_T("%s"),temp_path);
					CString size_path;
					size_path = temp_file;
					int pos_temp = size_path.ReverseFind('\\');
					size_filename = size_path.Mid(pos_temp+1);

					DWORD dwNum_size_file = WideCharToMultiByte(CP_ACP,NULL,size_filename,-1,NULL,NULL,0,NULL);
					char *sizefile = new char[dwNum_size_file];
					WideCharToMultiByte(CP_OEMCP,NULL,size_filename,-1,sizefile,dwNum_size_file,0,NULL);

					strcpy(uploadfile_info.upfile_fullpath,temp_file);
					strcpy(uploadfile_info.upfile_name,sizefile);
					vUpload.vec_up.push_back(uploadfile_info);
				}
				strcpy(uploadfile_info.upfile_fullpath,temp_path);
				strcpy(uploadfile_info.upfile_name,fileName_needUp);
				vUpload.vec_up.push_back(uploadfile_info);
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


//void CAboutDlg::On32785()
//{
//	// TODO: 在此添加命令处理程序代码
//	KillTimer(1);
//	KillTimer(2);
//}







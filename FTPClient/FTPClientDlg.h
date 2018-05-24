
// FTPClientDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxinet.h"
#include "ccurl.h"
#include <vector>
using namespace std;
// CFTPClientDlg 对话框
//CListBox m_list_log;//quanjuLIST
typedef struct upload_info
{
	char upfile_fullpath[256];//上传文件全路径
	char upfile_name[256];//上传文件名
}up_info/**lp_up_info*/;

typedef struct vector_upload
{
	vector <up_info> vec_up;
	//vector<lp_up_info> vec_up;
}vec_upload;

typedef struct download_info
{
	char downfile_fullpath[256];//下载文件全路径
	char downfile_name[256];//下载文件名
}down_info;

typedef struct vector_download
{
	vector <download_info> vec_down;
}vec_download;

typedef struct auto_upload_info//自动上传结构体
{
	char auto_up_path[256];//自动上传路径
	char auto_up_filename[256];//自动上传文件名
}auto_up;
typedef struct vector_autoup//自动上传vector
{
	vector <auto_upload_info> vec_autoup;
}vec_auto_upload;

static CString host,user,pass,port;//用户登录
static CListBox m_list_log;
#define WM_SHOWTASK WM_USER+1
//CFtpConnection *pThreadFtpConnection;
//CFtpConnection *pFtpConnection;
//vector<up_info> vec_up;
class CFTPClientDlg : public CDialogEx
{
	// 构造

public:
	CFTPClientDlg(CWnd* pParent = NULL);	// 标准构造函数

	~CFTPClientDlg();
	// 对话框数据
	enum { IDD = IDD_FTPCLIENT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	CImageList m_ImageList;  //图像列表(相同大小图像集合)   
	HTREEITEM m_hRoot;       //句柄 CTreeCtrl的根结点"我的电脑"
	HTREEITEM m_serverRoot;	 //服务器CTreeCtrl的根节点；

public:
	CTreeCtrl m_tree_local;
	CTreeCtrl m_tree_server;
	CEdit m_edit_host;
	CEdit m_edit_local;
	CEdit m_edit_server;
	CListCtrl m_list_local;
	CListCtrl m_list_server;
	CEdit m_edit_user;
	CEdit m_edit_pass;
	CEdit m_edit_port;
	/*CListBox m_list_log;*/
	CInternetSession *pInternetSession;
	CFtpConnection *pFtpConnection;
	//CFtpConnection *pThreadFtpConnection;
	/*CListBox m_list_local;
	CListBox m_list_server;*/
	//CString local_path[1024];//本地路径
	CString local_file_name;//本地文件名
	CString server_file_name;//服务器文件名
	CString filefile;
	CString local_path;//本地路径
	CString server_dpath[MAX_PATH];//服务器下载路径
	CString server_path;//服务器文件路径
	CString server_file_path;//下载用文件名加路径
	//CString host,user,pass,port;//用户登录

	//int flag;		//文件上传下载标志，0为空闲，1为上传，为下载
	int AexX,AexY;//记录list控件的宽度，高度
	int file_num;//该文件夹下文件个数
	CString LastWriteTime;//文件最后修改时间
	CString FileSize;//文件大小
	BOOL bconnect;
	BOOL isCorrect;//
	CTime t;//当前时间
	CString GBKToUTF8(CString strGBK);
	void GetLogicalDrives(HTREEITEM hParent);
	void GetDriveDir(HTREEITEM hParent);
	void GetFTPDriveDir(HTREEITEM hParent);
	void AddSubDir(HTREEITEM hParent);
	void AddFTPSubDir(HTREEITEM hParent);
	void ConnectFtp(CString sip,CString strusr,CString strpwd,CString strport);
	void UpdateDir(HTREEITEM hParent);
	void initLocalList();
	void initServerList();
	void AutoAdjustColumnWidth(CListCtrl pListCtrl);
	void OnStartTimer();//定时上传下载定时器
	static void AutoUpload();//实现定时上传
	void AutoDownload();//实现定时下载
	BOOL SetTimeUpDown();//设置时间
	void SetUpDownMode();//设置上传下载模式
	void deleteEnter(char* buf);
	CString GetFullPath(HTREEITEM hCurrent);
	CString GetFTPFullPath(HTREEITEM hCurrent);
	afx_msg void OnTvnItemexpandedTreeLocal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeLocal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTreeLocal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListLocal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonConnetct();
	afx_msg void OnUpload();
	afx_msg void OnNMClickListLocal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTreeServer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemexpandedTreeServer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeServer(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnDownload();
	afx_msg void OnNMRClickListServer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void On32784();

	static DWORD WINAPI Thread_UploadFile(LPVOID pParam);//线程上传文件数据
	static DWORD WINAPI Thread_DownloadFile(LPVOID pParam);//线程下载文件
	static DWORD WINAPI Thread_Auto_UploadFile(LPVOID pParam);//线程自动上传文件
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDownload();
	afx_msg LRESULT OnShowTask(WPARAM wParam,LPARAM IParam);

	CCurl  *m_pCurl;
	NOTIFYICONDATA m_nid;//NOTIFYICONDATA作用是用来向任务栏托盘区与发送消息

	static long long get_file_size(FILE *file);						 // 获取上传文件大小
	static void CString2char(char* &p,CString cstr);
	afx_msg void OnSettingserver();
	afx_msg void OnLogout();
};

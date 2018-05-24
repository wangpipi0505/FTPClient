
// FTPClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxinet.h"
#include "ccurl.h"
#include <vector>
using namespace std;
// CFTPClientDlg �Ի���
//CListBox m_list_log;//quanjuLIST
typedef struct upload_info
{
	char upfile_fullpath[256];//�ϴ��ļ�ȫ·��
	char upfile_name[256];//�ϴ��ļ���
}up_info/**lp_up_info*/;

typedef struct vector_upload
{
	vector <up_info> vec_up;
	//vector<lp_up_info> vec_up;
}vec_upload;

typedef struct download_info
{
	char downfile_fullpath[256];//�����ļ�ȫ·��
	char downfile_name[256];//�����ļ���
}down_info;

typedef struct vector_download
{
	vector <download_info> vec_down;
}vec_download;

typedef struct auto_upload_info//�Զ��ϴ��ṹ��
{
	char auto_up_path[256];//�Զ��ϴ�·��
	char auto_up_filename[256];//�Զ��ϴ��ļ���
}auto_up;
typedef struct vector_autoup//�Զ��ϴ�vector
{
	vector <auto_upload_info> vec_autoup;
}vec_auto_upload;

static CString host,user,pass,port;//�û���¼
static CListBox m_list_log;
#define WM_SHOWTASK WM_USER+1
//CFtpConnection *pThreadFtpConnection;
//CFtpConnection *pFtpConnection;
//vector<up_info> vec_up;
class CFTPClientDlg : public CDialogEx
{
	// ����

public:
	CFTPClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

	~CFTPClientDlg();
	// �Ի�������
	enum { IDD = IDD_FTPCLIENT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


	// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	CImageList m_ImageList;  //ͼ���б�(��ͬ��Сͼ�񼯺�)   
	HTREEITEM m_hRoot;       //��� CTreeCtrl�ĸ����"�ҵĵ���"
	HTREEITEM m_serverRoot;	 //������CTreeCtrl�ĸ��ڵ㣻

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
	//CString local_path[1024];//����·��
	CString local_file_name;//�����ļ���
	CString server_file_name;//�������ļ���
	CString filefile;
	CString local_path;//����·��
	CString server_dpath[MAX_PATH];//����������·��
	CString server_path;//�������ļ�·��
	CString server_file_path;//�������ļ�����·��
	//CString host,user,pass,port;//�û���¼

	//int flag;		//�ļ��ϴ����ر�־��0Ϊ���У�1Ϊ�ϴ���Ϊ����
	int AexX,AexY;//��¼list�ؼ��Ŀ�ȣ��߶�
	int file_num;//���ļ������ļ�����
	CString LastWriteTime;//�ļ�����޸�ʱ��
	CString FileSize;//�ļ���С
	BOOL bconnect;
	BOOL isCorrect;//
	CTime t;//��ǰʱ��
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
	void OnStartTimer();//��ʱ�ϴ����ض�ʱ��
	static void AutoUpload();//ʵ�ֶ�ʱ�ϴ�
	void AutoDownload();//ʵ�ֶ�ʱ����
	BOOL SetTimeUpDown();//����ʱ��
	void SetUpDownMode();//�����ϴ�����ģʽ
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

	static DWORD WINAPI Thread_UploadFile(LPVOID pParam);//�߳��ϴ��ļ�����
	static DWORD WINAPI Thread_DownloadFile(LPVOID pParam);//�߳������ļ�
	static DWORD WINAPI Thread_Auto_UploadFile(LPVOID pParam);//�߳��Զ��ϴ��ļ�
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDownload();
	afx_msg LRESULT OnShowTask(WPARAM wParam,LPARAM IParam);

	CCurl  *m_pCurl;
	NOTIFYICONDATA m_nid;//NOTIFYICONDATA�������������������������뷢����Ϣ

	static long long get_file_size(FILE *file);						 // ��ȡ�ϴ��ļ���С
	static void CString2char(char* &p,CString cstr);
	afx_msg void OnSettingserver();
	afx_msg void OnLogout();
};

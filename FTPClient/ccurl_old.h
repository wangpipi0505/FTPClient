/**
 * FileName:	ccurl.h
 * Author:		chsMiao
 * Description: �򵥷�װLibcurl��easy interface,
 *				����ʵ��Ftp�ϴ������ؼ���Ӧ������ʾ����
 *
 * ps :	
 *   1. curl_global_init��curl_global_cleanup ֻ�ܱ�����һ��
 *		���ڴ�ԭ��, �����õ�������з�װ
 *
 *	 2. ����˽����Ƕ����е��������������
 *	    ��ԭ�������ڽ�����ʱ��
 *      ϵͳ���Զ��������е�ȫ�ֱ�����������ľ�̬��Ա����
 *
 *	 3.	����CLOG��־���������μ�outctrl.h
 */

#ifndef __CCURL_H__ 
#define __CCURL_H__ 

#include <curl/curl.h>
#include <string>

#include "stdlib.h"
#include <sys/stat.h>
#include "stdio.h"
//#include <afx.h>

using std::string;


struct FileInfo{
	string filename;
	FILE *file;
	string mtimeStr;
	time_t mtime; /* modification time */
	curl_off_t filesize;
	curl_off_t remotefilesize;	//FTP�������϶�Ӧ�ļ���С
	bool isFile;

	bool use_resume;

	FileInfo(){
		file=nullptr;
		filesize=0;
		remotefilesize = -1;
		use_resume=false;
	}

	~FileInfo(){
		if(file)
			fclose(file);
	}
};

class CCurl
{
public:
	static CCurl * Instance();

	void setHostInfo(char *ip, int /*port = 21*/);			// ����ftp��������IP�Ͷ˿ں�
	void setUserPwd(char *name, char *pwd);				// ����ftp������û���������
	//void setHostInfo(CString ip, int port = 21);			// ����ftp��������IP�Ͷ˿ں�
	//void setUserPwd(CString name, CString pwd);				// ����ftp������û���������
	void setHostUserPwd(char *name, char *pwd,char *ip, int port/* = 21*/);			// ��Ч setHostInfo + setUserPwd
	/*void setHostUserPwd(CString name, CString pwd,CString ip, int port = 21);*/
	bool upload(char *localFullName, 
				char *remoteFileName = nullptr);		// �ϴ��ļ�
	//bool resumeupload(char *localFullName, 
	//			char *remoteFileName = nullptr);		// �ϴ��ļ�

	bool download(char *remoteFileName, 
				  char *localFullName = nullptr);		// �����ļ�

private:
	CCurl();
	static CCurl *m_pInstance;

	/*
	 * ����˽����Ƕ�༰�侲̬��Ա����,ʵ�ֵ�����CCurl����Դ�ͷ�
	 */
	class CGarbo
	{
	public:
		~CGarbo();
	};

	static CGarbo m_Garbo;

private:

	/*
	 * callback functions
	 */
	static size_t readCallback(char *buffer, size_t size, 
							   size_t nitems, void *instream);
	static size_t writeCallback(char *buffer, size_t size, 
								size_t nitems, void *outstream);
	static int uploadProgressCallback(void *clientp, 
									  curl_off_t dltotal, 
									  curl_off_t dlnow, 
									  curl_off_t ultotal, 
									  curl_off_t ulnow);
	static int downloadProgressCallback(void *clientp, 
										curl_off_t dltotal, 
										curl_off_t dlnow, 
										curl_off_t ultotal, 
										curl_off_t ulnow);


	static size_t getcontentlengthfunc(void *ptr,
										size_t size, 
										size_t nmemb,
										void *stream);	// ��httpͷ����ȡ�ļ�size
	void init();										 // curl��ʼ��
	static string speedConvert(double &speed);			 // ת���ϴ�/�����ٶȵĵ�λ
	long long  get_file_size(FILE *file);						 // ��ȡ�ϴ��ļ���С
	void setUploadOpt(FileInfo *fileinfo, const char* url);		 // �ϴ�ʱcurl_easy_setopt�ĸ�������
	void setDownLoadOpt(FileInfo *fileinfo, const char* url);	 // ����ʱcurl_easy_setopt�ĸ�������
	string getUploadFullName(char *localFullName,		 // �����ϴ�ʱ������ftp�������ϵ��ļ�ȫ��
						char *remoteFileName);
	void getRemoteFileSize(FileInfo *fileinfo, const char* url);
	//bool GetRemoteFileList(string dir);


	string m_strServerUrl;								 // ftp��Ŀ¼��ַ
	string m_strUserPwd;								 // �û���:����
	static CURL *curl;									 // CURL easy interface �ľ��
	static bool m_bGlobalInit;
	static curl_off_t m_nUploaded;
	static curl_off_t m_nDownloaded;

};

#endif
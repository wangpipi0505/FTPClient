/**
 * FileName:	ccurl.h
 * Author:		chsMiao
 * Description: 简单封装Libcurl的easy interface,
 *				用来实现Ftp上传、下载及相应进度显示功能
 *
 * ps :	
 *   1. curl_global_init和curl_global_cleanup 只能被调用一次
 *		基于此原因, 考虑用单例类进行封装
 *
 *	 2. 采用私有内嵌类进行单例类的析构处理
 *	    其原理：程序在结束的时候，
 *      系统会自动析构所有的全局变量和所有类的静态成员变量
 *
 *	 3.	采用CLOG日志输出，具体参见outctrl.h
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
	curl_off_t remotefilesize;	//FTP服务器上对应文件大小
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

	void setHostInfo(char *ip, int /*port = 21*/);			// 设置ftp服务器的IP和端口号
	void setUserPwd(char *name, char *pwd);				// 设置ftp服务的用户名和密码
	//void setHostInfo(CString ip, int port = 21);			// 设置ftp服务器的IP和端口号
	//void setUserPwd(CString name, CString pwd);				// 设置ftp服务的用户名和密码
	void setHostUserPwd(char *name, char *pwd,char *ip, int port/* = 21*/);			// 等效 setHostInfo + setUserPwd
	/*void setHostUserPwd(CString name, CString pwd,CString ip, int port = 21);*/
	bool upload(char *localFullName, 
				char *remoteFileName = nullptr);		// 上传文件
	//bool resumeupload(char *localFullName, 
	//			char *remoteFileName = nullptr);		// 上传文件

	bool download(char *remoteFileName, 
				  char *localFullName = nullptr);		// 下载文件

private:
	CCurl();
	static CCurl *m_pInstance;

	/*
	 * 定义私有内嵌类及其静态成员变量,实现单例类CCurl的资源释放
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
										void *stream);	// 从http头部获取文件size
	void init();										 // curl初始化
	static string speedConvert(double &speed);			 // 转换上传/下载速度的单位
	long long  get_file_size(FILE *file);						 // 获取上传文件大小
	void setUploadOpt(FileInfo *fileinfo, const char* url);		 // 上传时curl_easy_setopt的各项设置
	void setDownLoadOpt(FileInfo *fileinfo, const char* url);	 // 下载时curl_easy_setopt的各项设置
	string getUploadFullName(char *localFullName,		 // 设置上传时保存在ftp服务器上的文件全名
						char *remoteFileName);
	void getRemoteFileSize(FileInfo *fileinfo, const char* url);
	//bool GetRemoteFileList(string dir);


	string m_strServerUrl;								 // ftp根目录地址
	string m_strUserPwd;								 // 用户名:密码
	static CURL *curl;									 // CURL easy interface 的句柄
	static bool m_bGlobalInit;
	static curl_off_t m_nUploaded;
	static curl_off_t m_nDownloaded;

};

#endif
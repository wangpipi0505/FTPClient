#include "stdafx.h"
#include <sstream>
#include "ccurl.h"

#include "stdlib.h"
#include <sys/stat.h>
#include "stdio.h"

#define USE_LOG4CPLUS
#include "outctrl.h"

using std::stringstream;

#ifdef _DEBUG
#pragma comment(lib, "libcurl_a_debug.lib")
#else
#pragma comment(lib, "libcurl_a.lib")
#endif

//#pragma comment(lib, "libcurl_debug.lib")


CCurl *CCurl::m_pInstance = nullptr;  
CCurl::CGarbo CCurl::m_Garbo;

bool CCurl::m_bGlobalInit = false;
CURL *CCurl::curl = nullptr;

curl_off_t CCurl::m_nUploaded = 0;
curl_off_t CCurl::m_nDownloaded = 0;

CCurl::CCurl()
{
	init();
}

CCurl * CCurl::Instance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new CCurl();
	}
	return m_pInstance;
}

CCurl::CGarbo::~CGarbo()
{
	if (curl != nullptr)
	{
		curl_easy_cleanup(curl);
	}

	if (m_bGlobalInit)
	{
		curl_global_cleanup();
	}

	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
	}
}

size_t CCurl::readCallback(char *buffer, size_t size, size_t nitems, void *instream)
{
	return fread(buffer, size, nitems, (FILE *)instream);
}

size_t CCurl::writeCallback(char *buffer, size_t size, size_t nitems, void *outstream)
{
	return fwrite(buffer, size, nitems, (FILE *)outstream);
}

int CCurl::uploadProgressCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
								  curl_off_t ultotal, curl_off_t ulnow)
{
	/* 注意后面四个形参
	* 1. 含义 : 表征传输过程中的文件大小（单位：字节Bytes）
	*    dltotal : 下载文件总大小
	*	  dlnow   : 已下载大小
	*    ultotal : 上传文件总大小
	*    ulnow	  : 已上传大小（单位：字节Bytes）
	* 2. 类型: curl_off_t 
	*    其输出时格式采用 CURL_FORMAT_OFF_T
	*    例如: printf("uplaoding "CURL_FORMAT_OFF_T"/"CURL_FORMAT_OFF_T"...\n", ulnow, ultotal);
	*/ 
	if (ultotal == 0 || ulnow == 0)
	{
		return 0;
	}

	if (m_nUploaded != ulnow)
	{
		// uplaoded percentage
		float nPos = (ulnow * 100.0 / ultotal);
		CLOG("uplaoding %.2f%%...", nPos);
		// uplaoding speed
		double speed;
		curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD, &speed);
		string strSpeed = speedConvert(speed);
		CLOG("uplaoding speed: %s", strSpeed.c_str());


		m_nUploaded = ulnow;
	}

	return 0;
}

int CCurl::downloadProgressCallback(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
									curl_off_t ultotal, curl_off_t ulnow)
{
	if (dltotal == 0 || dlnow == 0)
	{
		return 0;
	}

	if (m_nDownloaded != dlnow)
	{
		// downloaded percentage
		float nPos = (dlnow * 100.0 / dltotal);
		CLOG("downloading %.2f%%...", nPos);

		// downloading speed
		double speed;
		curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &speed);
		string strSpeed = speedConvert(speed);
		CLOG("downloading speed: %s", strSpeed.c_str());

		m_nDownloaded = dlnow;
	}

	return 0;
}

string CCurl::speedConvert(double &speed)
{
	stringstream ss;
	if (speed < 1024.00)
	{
		ss << speed;
		ss << " B/s";
	}
	else if (speed >= 1024.00 && speed < 1024.00 * 1024.00)
	{  
		speed /= 1024.00;  
		ss << speed;
		ss << " KiB/s";
	}
	else if (speed >= 1024.00 * 1024.00 && speed < 1024.00 * 1024.00 * 1024.00)  
	{  
		speed /= (1024.00 * 1024.00);  
		ss << speed;
		ss << " MiB/s";
	}
	else
	{  
		speed /= (1024.00 * 1024.00 * 1024.00);  
		ss << speed;
		ss << " GiB/s";
	}

	return ss.str();
}

void CCurl::init()
{
	CLOG(curl_version());

	// global initialization
	CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (CURLE_OK != res)
	{
		m_bGlobalInit = false;
		CLOG("Global initialization failed");
		exit(res);
	}
	m_bGlobalInit = true;

	// initialization of easy handle
	curl = curl_easy_init();
	if (nullptr == curl)
	{
		CLOG("Init curl failed");
		exit(1);
	}

	CLOG("Init curl successfully!");
}

LONGLONG CCurl::get_file_size(FILE *file)
{
	if (file == nullptr)
	{
		return 0;
	}

	//fseek(file, 0L, SEEK_END);
	//int nFileSize = ftell(file);
	_fseeki64(file,0L,SEEK_END);
	LONGLONG nFileSize = _ftelli64(file);
	_fseeki64(file,0L,SEEK_SET);
	//fseek(file, 0L, SEEK_SET);

	return nFileSize;
}

/* parse headers for Content-Length */ 
size_t CCurl::getcontentlengthfunc(void *ptr, size_t size, size_t nmemb, void *stream) {
	int r;
	curl_off_t len = 0;

	/* _snscanf() is Win32 specific */
	// r = _snscanf(ptr, size * nmemb, "Content-Length: %ld\n", &len);
	r = sscanf((char *)ptr, "Content-Length: %lld\n", &len);
	if (r) /* Microsoft: we don't read the specs */
	{
		*((curl_off_t *) stream) = len;
		printf("filelength = %lld\n",len);

	}

	return size * nmemb;
}


/* discard downloaded data */ 
size_t discardfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
	return size * nmemb;
}

/* read data to upload */ 
size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
	FILE *f = (FILE *)stream;
	size_t n;

	if(ferror(f))
		return CURL_READFUNC_ABORT;

	n = fread(ptr, size, nmemb, f) * size;

	return n;
}


//int resumeupload(CURL *curlhandle, const char *remotepath, const char *localpath, long timeout, long tries)
bool CCurl::resumeupload(char *localFullName, char *remoteFileName/* = nullptr*/)
{
	curl_off_t uploaded_len = 0;
	CURLcode r = CURLE_GOT_NOTHING;

	// check
	if (curl == nullptr)
	{
		CLOG("Curl handle null");
		return false;
	}

	if (m_strServerUrl.empty())
	{
		CLOG("Please set ftp server info");
		return false;
	}

	if (localFullName == nullptr)
	{
		CLOG("Please set valid uploaded file");
		return false;
	}

	// get CURLOPT_URL
	string strRemoteFile = getUploadFullName(localFullName, remoteFileName);

	// open file
	FileInfo srcfile;
	srcfile.file = fopen(localFullName, "rb");
	if (nullptr == srcfile.file)
	{
		CLOG("File open error");
		return false;
	}
	curl_easy_reset(curl);

	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

	curl_easy_setopt(curl, CURLOPT_URL, strRemoteFile.c_str());

	/* if(timeout)
	curl_easy_setopt(curlhandle, CURLOPT_FTP_RESPONSE_TIMEOUT, timeout);*/

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, getcontentlengthfunc);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &uploaded_len);

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discardfunc);

	curl_easy_setopt(curl, CURLOPT_READFUNCTION, readfunc);
	curl_easy_setopt(curl, CURLOPT_READDATA, srcfile.file);

	/* disable passive mode */ 
	//curl_easy_setopt(curl, CURLOPT_FTPPORT, "-");
	curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	bool resume = true;
	// for(count = 0; (r != CURLE_OK) && (count < tries); count++) {
	for(int count = 0; (r != CURLE_OK) && (count < 3); count++) {
		/* are we resuming? */ 
		if(resume) { /* yes */ 
			/* determine the length of the file already written */ 
			/*
			* With NOBODY and NOHEADER, libcurl will issue a SIZE
			* command, but the only way to retrieve the result is
			* to parse the returned Content-Length header. Thus,
			* getcontentlengthfunc(). We need discardfunc() above
			* because HEADER will dump the headers to stdout
			* without it.
			*/ 
			curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
			curl_easy_setopt(curl, CURLOPT_HEADER, 1L);

			r = curl_easy_perform(curl);
			if(r != CURLE_OK)
				continue;

			curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
			curl_easy_setopt(curl, CURLOPT_HEADER, 0L);

			_fseeki64(srcfile.file, uploaded_len, SEEK_SET);

			curl_easy_setopt(curl, CURLOPT_APPEND, 1L);
		}
		else { /* no */ 
			curl_easy_setopt(curl, CURLOPT_APPEND, 0L);
		}

		r = curl_easy_perform(curl);
	}

	fclose(srcfile.file);

	if(r == CURLE_OK)
		return 1;
	else {
		fprintf(stderr, "%s\n", curl_easy_strerror(r));
		return 0;
	}
}

void CCurl::setUploadOpt(FileInfo *fileinfo, const char* url)
{
	if (curl == nullptr || url == nullptr || m_strUserPwd.empty())
	{
		CLOG("null or empty error");
		return;
	}

	int nSize = get_file_size(fileinfo->file);
	if (nSize == 0)
	{
		CLOG("File's size is zero");
		return;
	}

	m_nUploaded = 0;

	// reset all options of curl handle
	curl_easy_reset(curl);

	// specify target URL
	curl_easy_setopt(curl, CURLOPT_URL, url);

	// specify user and password
	curl_easy_setopt(curl, CURLOPT_USERPWD, m_strUserPwd.c_str());

	// enable uploading
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

	// create dir if not existed on ftp server
	curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);

	////设置http 头部处理函数
	//curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, getcontentlengthfunc);
	//curl_easy_setopt(curl, CURLOPT_HEADERDATA, &(fileinfo->filesize));

	// read callback function
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, &CCurl::readCallback);

	// specify which file to upload
	curl_easy_setopt(curl, CURLOPT_READDATA, fileinfo->file);

	//// set the size of the file to upload
	//curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, nSize-fileinfo->filesize);

	// get the process
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, &CCurl::uploadProgressCallback); 
	curl_easy_setopt(curl, CURLOPT_XFERINFODATA, nullptr);



#ifdef _DEBUG
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
}

void CCurl::setDownLoadOpt(FileInfo *fileinfo, const char* url)
{
	if (curl == nullptr || url == nullptr || m_strUserPwd.empty())
	{
		CLOG("null or empty error");
		return;
	}

	m_nDownloaded = 0;

	// reset all options of curl handle
	curl_easy_reset(curl);

	// specify target URL
	curl_easy_setopt(curl, CURLOPT_URL, url);

	// specify user and password
	curl_easy_setopt(curl, CURLOPT_USERPWD, m_strUserPwd.c_str());

	// write callback function
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CCurl::writeCallback);

	// Set a pointer to pass to the callback
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fileinfo->file);

	//set resume tranmit
	curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, fileinfo->use_resume?fileinfo->filesize:0);


	// get the process
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, &CCurl::downloadProgressCallback); 
	curl_easy_setopt(curl, CURLOPT_XFERINFODATA, nullptr);

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

#ifdef _DEBUG
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
}

void CCurl::setUserPwd(char *name, char *pwd)
{
	if (name != nullptr && pwd != nullptr)
	{
		m_strUserPwd = string(name);
		m_strUserPwd = m_strUserPwd + ":" + pwd;
	}
}

void CCurl::setHostInfo(char *ip, int port/* = 21*/)
{
	if (ip != nullptr)
	{
		stringstream ss;
		ss << string("ftp://");
		ss << ip;
		if (port != 21)
		{
			ss << ":";
			ss << port;
		}
		ss << "/";

		m_strServerUrl = ss.str();
	}
}

void CCurl::setHostUserPwd(char *name, char *pwd, char *ip, int port/* = 21*/)
	//void CCurl::setHostUserPwd(CString name, CString pwd,CString ip, int port = 21)
{
	setUserPwd(name, pwd);
	setHostInfo(ip, port);
}

string CCurl::getUploadFullName(char *localFullName, char *remoteFileName)
{
	if (m_strServerUrl.empty() || localFullName == nullptr)
	{
		return "";
	}

	string strRemoteFile;
	if (remoteFileName == nullptr)
	{
		char *filename = strrchr(localFullName, '/') + 1;
		//char *filename = strrchr(localFullName, '/');
		strRemoteFile = m_strServerUrl + filename;
	}
	else
	{
		strRemoteFile = m_strServerUrl + remoteFileName;
	}

	return strRemoteFile;
}



void  CCurl::getFileSize(FileInfo *fileinfo, const char* url)
{
	if (curl == nullptr || url == nullptr || m_strUserPwd.empty())
	{
		CLOG("null or empty error");
		return;
	}

	int nSize = get_file_size(fileinfo->file);
	if (nSize == 0)
	{
		CLOG("File's size is zero");
		return;
	}

	m_nUploaded = 0;

	// reset all options of curl handle
	curl_easy_reset(curl);

	// specify target URL
	curl_easy_setopt(curl, CURLOPT_URL, url);

	// specify user and password
	curl_easy_setopt(curl, CURLOPT_USERPWD, m_strUserPwd.c_str());

	//设置http 头部处理函数
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, getcontentlengthfunc);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &(fileinfo->filesize));

	curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);  
	curl_easy_setopt(curl, CURLOPT_HEADER, 1L);  


#ifdef _DEBUG
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

	CURLcode res = curl_easy_perform(curl);


	if (res != CURLE_OK)
	{
		CLOG("curl_easy_perform() failed: %s", curl_easy_strerror(res));
	}
}

bool CCurl::upload(char *localFullName, char *remoteFileName/* = nullptr*/)
{
	//another example of resume upload:https://curl.haxx.se/libcurl/c/ftpuploadresume.html

	// check
	if (curl == nullptr)
	{
		CLOG("Curl handle null");
		return false;
	}

	if (m_strServerUrl.empty())
	{
		CLOG("Please set ftp server info");
		return false;
	}

	if (localFullName == nullptr)
	{
		CLOG("Please set valid uploaded file");
		return false;
	}

	// get CURLOPT_URL
	string strRemoteFile = getUploadFullName(localFullName, remoteFileName);

	// open file
	FileInfo srcfile;
	//srcfile.file = fopen(localFullName, "rb");
	srcfile.file = fopen(localFullName, "rb");

	if (nullptr == srcfile.file)
	{
		int err=GetLastError();
		CLOG("File open error");
		return false;
	}

	//if(getFileSize(remoteFileName,&srcfile.filesize) == false)

	getFileSize(&srcfile, strRemoteFile.c_str());
	FileInfo tempFileinfo;
	std::string tmpFile;
	tmpFile = strRemoteFile + ".Temp";
	getFileSize(&tempFileinfo, tmpFile.c_str());

	if(srcfile.filesize < tempFileinfo.filesize)
		srcfile.filesize = tempFileinfo.filesize;
	// curl_easy_setopt for uploading
	setUploadOpt(&srcfile, strRemoteFile.c_str());
	if(srcfile.filesize >0)
	{
		_fseeki64(srcfile.file,srcfile.filesize,SEEK_SET);
		if(feof(srcfile.file))		//服务端文件大小和本地文件大小一致，跳过上传
		{
			return true;
		}
		//续传
		curl_easy_setopt(curl, CURLOPT_APPEND, 1L);
		AfxMessageBox(_T("resume upload"));
	}
	// execute
	CURLcode res = curl_easy_perform(curl);

	//// close file
	//if (srcfile.file != nullptr)
	//{
	//	fclose(srcfile.file);
	//}

	if (res != CURLE_OK)
	{
		CLOG("curl_easy_perform() failed: %s", curl_easy_strerror(res));
		return false;
	}

	return true;
}

bool CCurl::download(char *remoteFileName, char *localFullName/* = nullptr*/)
{
	struct stat file_info;
	//curl_off_t local_file_len = -1 ;
	//int use_resume = 0;

	// check
	if (curl == nullptr)
	{
		CLOG("Curl handle null");
		return false;
	}

	if (m_strServerUrl.empty())
	{
		CLOG("Please set ftp server info");
		return false;
	}

	if (remoteFileName == nullptr)
	{
		CLOG("Please set valid downloaded file");
		return false;
	}

	// get CURLOPT_URL
	string strRemoteFile = m_strServerUrl + remoteFileName;

	// open file
	if (localFullName == nullptr)
	{
		localFullName = remoteFileName;
	}

	FileInfo psrc;
	/* 得到本地文件大小 */
	//if(access(localpath,F_OK) ==0)
	if(stat(localFullName, &file_info) == 0) 
	{
		psrc.filesize =  file_info.st_size;
		psrc.use_resume  = 1;
	}

	psrc.file = fopen(localFullName, "ab+");//wb
	if (nullptr == psrc.file)
	{
		CLOG("File open error");
		return false;
	}

	// curl_easy_setopt for downloading
	setDownLoadOpt(&psrc, strRemoteFile.c_str());

	// execute
	CURLcode res = curl_easy_perform(curl);

	//// close file, deconstructor chuli
	//if (psrc.file != nullptr)
	//{
	//	fclose(psrc.file);
	//}

	if (res != CURLE_OK)
	{
		CLOG("curl_easy_perform() failed: %s", curl_easy_strerror(res));
		return false;
	}

	return true;
}
//
////新版本
//CString * SplitString(CString *pStr,CString str, char split, int& iSubStrs)
//{
//    int iPos = 0; //分割符位置
//    int iNums = 0; //分割符的总数
//    CString strTemp = str;
//    CString strRight;
//    CString strLeft;
//	//    //先计算子字符串的数量
//    while (iPos != -1)
//    {
//        iPos = strTemp.Find(split);
//        if (iPos == -1)
//        {
//            break;
//        }
//		while(strTemp.GetAt(++iPos)==split){};
//
//        //左子串
//        strLeft = strTemp.Left(iPos-1);
//        //右子串
//        strRight = strTemp.Mid(iPos);
//        strTemp = strRight;
//        pStr[iNums++] = strLeft;
//    }
//    pStr[iNums] = strTemp;
//	iSubStrs = iNums+1;
//    return pStr;
//}
//
//FileInfo fileinfo;
//std::vector<FileInfo> fileinfolist;
//std::vector<FileInfo> dirlist;
//
//size_t  my_checkdir(void *ptr, size_t size, size_t nmemb, void *stream)
//{
//	bool isDir = true;
//	printf("recv:size=%d,nmemb=%d,data as follows:\n%s\n",size,nmemb,ptr);
//	FileInfo * pFileInfo = (FileInfo *)stream;
//	//FtpParser::ftpparse(pFileInfo,(char*)ptr,nmemb*size);
//
//	CString recvString((char*)ptr);
//	int len = recvString.GetLength();
//	int iPos=-1;   //定义一个游标
//	int linenum=0;
//	CString leftStr;
//	while((iPos=recvString.Find(L"\r\n"))!=-1)
//	{
//		leftStr = recvString.Left(iPos);
//		printf("length of %d linenum is %d\n",linenum,leftStr.GetLength());
//		recvString = recvString.Mid(iPos+2);
//		CString  pStr[4];
//		int iSubStrs;
//
//		//处理list输出
//		/*drwxr-xr-x 1 ftp ftp              0 Nov 29  2016 CentOS-6.4-x86_64-bin-DVD1to2
//		-rw-r--r-- 1 ftp ftp     3420557312 Apr 21  2016 cn_windows_7_ultimate_with_sp1_x64_dvd_u_677408.iso
//		-rw-r--r-- 1 ftp ftp     6294470656 Feb 23 10:53 cn_windows_7_ultimate_x64_dvd_x15-66043_usb3.iso
//		*/
//
//		/*isDir = (leftStr.GetAt(0)=='d');
//		pStr = SplitString(leftStr, ' ', iSubStrs);
//
//		
//
//		pFileInfo->name = (char*)(LPCTSTR)pStr[iSubStrs-1];
//		pFileInfo->size = _ttoi(pStr[iSubStrs-5]);*/
//		//pFileInfo->mtime = 
//
//		//处理 mlsd输出 n
//		/*type=file;modify=20090426154910;size=222;name
//		type=dir;modify=xxx;name
//		*/
//		SplitString(pStr,leftStr, ';', iSubStrs);
//
//		isDir = (pStr[0].Find(_T("dir")) != -1);
//		CString mtime = pStr[1].Mid(pStr[1].Find('=')+1);
//		//pFileInfo->mtime = (char*)(LPCTSTR)pstr[1].Mid(
//		pFileInfo->filesize = pStr[2].Find(_T("size"));
//		if(pFileInfo->filesize == -1)
//			pFileInfo->filename = pStr[2];
//		else
//		{
//			pFileInfo->filesize = _ttoi(pStr[2].Mid(pStr[2].Find('=')+1));
//			pFileInfo->filename = pStr[3];
//		}
//
//
//		if(isDir)
//		{
//			//my_checkdir(ptr,size,nmemb,stream);
//		}
//		else
//		{
//			fileinfolist.push_back(*pFileInfo);
//		}
//		linenum ++;
//		printf("filename = %S, filetime = %ld, filesize = %ld\n",pFileInfo->name,pFileInfo->mtime,pFileInfo->size);
//	}
//	return size*nmemb;
//}
//
//
//bool CCurl::GetRemoteFileList(string dir){
//
//	CURL *easy_handle = curl_easy_init();
//	curl_slist *headerlist=NULL;
//	curl_easy_setopt(easy_handle, CURLOPT_URL, dir); 	
//	//curl_easy_setopt(easy_handle, CURLOPT_URL, "192.168.0.101/test"); 	
//
//
//	headerlist = curl_slist_append (headerlist, "CWD /"); 
//	if(headerlist != NULL){
//		curl_easy_setopt(curl, CURLOPT_QUOTE, headerlist); 
//	}
//
//	curl_slist_append (headerlist, "EPRT |2|::1|31971|");//EPSV,EPRT |2|::1|31971|
//	curl_slist_append (headerlist, "PWD"); 
//	curl_slist_append (headerlist, "TYPE I"); 
//	//curl_slist_append (headerlist, "LIST"); //LIST,MLSD
//	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_checkdir); 
//	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fileinfo); 
//	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
//
//	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "MLSD");
//	 
//	//curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, getcontentlengthfunc);
//	//curl_easy_setopt(curl, CURLOPT_HEADERDATA, &ftpfile);
//
//	curl_easy_perform(curl); 
//	curl_easy_cleanup(easy_handle);
//	curl_slist_free_all (headerlist); 
//
//	return true;
//}

// ftpLibcurl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ccurl.h"
#include <string>
using std::string;

#define USE_LOG4CPLUS
#include "outctrl.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CCurl * pCurl = CCurl::Instance();
	//pCurl->setHostUserPwd("admin", "12345", "192.168.10.58");
	//pCurl->setHostUserPwd("anonymous", "", "localhost");

	//if (pCurl->upload("d:\mysql-installer-community-5.7.11.0.msi", "aa/yes.7z"))
	//if (pCurl->upload("C:/Users/dell/Desktop/up.iso","rcv.iso"))
	if (pCurl->upload("C:/Users/dell/Desktop/ftp.vsd","ftp.vsd"))
	{
		CLOG("upload ok");
	}
	/*if (pCurl->resumeupload("C:/Users/dell/Desktop/ftp.vsd","ftp.vsd"))
	{
		CLOG("upload ok");
	}*/
	//if (pCurl->download("R20161207202314_1.dat.gz", "C:/Users/wwy/Desktop/R20161207202314_1.dat.gz"))
	if (pCurl->download("cn_windows_7_ultimate_x64_dvd_x15-66043_usb3.iso", "C:/Users/dell/Desktop/cn_windows_7_ultimate_x64_dvd_x15-66043_usb3.iso"))
		//if (pCurl->download("/ftp/ftp data/R20160725155412_1.dat", "C:\Users\wwy\Desktop\R20160725155412_1.dat"))
	{
		CLOG("down ok 222");
	}
	//system("pause");
	Sleep(15000);
	return 0;
}



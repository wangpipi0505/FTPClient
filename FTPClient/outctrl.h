/**
 * FileName:	outctrl.h
 * Author:		chsMiao
 * Description: ʵ�ּ򵥵���־�������
 * 
 * Details: 
 * 1. ��������־�����ʽѡ��
 *    a) �Զ���ĸ�ʽ�����Ĭ�ϣ�
 *		 ��� int fprintf ( FILE * stream, const char * format, ... ); 
 *			  ������ǰ��ʱ��� �ͺ��õ��ļ������к�
 *		 �磺 2017-02-13 09:40:54 | hello world! ---[main.cpp:5]
 *    b) log4cplus����־���
 *		 ������þ�̬���ӵķ�ʽ��ע���ַ���ΪUnicode���ɸ������������
 *		 ����Ĭ��ʹ��INFO����ʹ�ýű����ý����������
 *		 ��ʹ�ô��ַ�ʽ��Ҫע��log4cplus��include��libĿ¼������
 * 
 * 2. ���漰���ĺ�
 *    USE_LOG4CPLUS   �����˴˺꣬��ʾ����log4cplus����־���
 *	  MAX_MSG_LENGTH  ����log4cplus����־���ʱ����Ϣ�����ĳ���
 *					  �൱�� ��"%m"�����ԭʼ��Ϣ������󳤶�
 *	  CFG_NAME		  log4cplus��־�������ļ�,Ĭ��"RealUse.cfg"
 *    LOG_SHUTDOWN    �ر���־�������
 *    CLOG			  ʵ�ֵ�����������־����ĺ�	
 *
 *    ע�⣺ǰ�ĸ���Ķ���Ҫ������outctrl.hͷ�ļ�֮ǰ�����ã�
 *         ���Է���VS�������Ե�"Ԥ����������"��
 *
 * 3. ʹ�÷���
 *	  �����ͷ�ļ�outctrl.h��ʹ��CLOG�����������
 *	  �磺
			#define USE_LOG4CPLUS
			#include "outctrl.h"
			int main(int argc, char **argv)
			{
				CLOG("hello world!");
				CLOG("%s  %d %.2f", "hello", 123, 3.1415926);
				return 0;
			}
 *
 */

#ifndef __CTRLOUT_H_
#define __CTRLOUT_H_

#include <string>
#include <sstream>
#include <ctime>
#include <cstdarg>   //�ú������ܿɱ����

#ifdef USE_LOG4CPLUS

#ifdef _DEBUG
#pragma comment(lib, "log4cplusSUD.lib")
#else
#pragma comment(lib, "log4cplusSU.lib")
#endif


//#include "include\log4cplus\logger.h"
//#include "include\log4cplus\configurator.h"
//#include "include\log4cplus\loggingmacros.h"
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/logger.h>
using namespace log4cplus;

#ifndef CFG_NAME
#define CFG_NAME "./RealUse.cfg"
#endif

#ifndef MAX_MSG_LENGTH
#define MAX_MSG_LENGTH	2048
#endif

#endif // USE_LOG4CPLUS

using std::string;
using std::stringstream;


#ifdef LOG_SHUTDOWN
	#define CLOG(fmt, ...)
#else
	#ifndef USE_LOG4CPLUS

	#define CLOG(format, ...)						 \
	do {											 \
		char time_buff[64];							 \
		auto now = time(nullptr);					 \
		strftime(time_buff, sizeof(time_buff),		 \
				"%Y-%m-%d %H:%M:%S | ",				 \
				localtime(&now));					 \
		fprintf(stdout, "%s", time_buff);			 \
		fprintf(stdout, format, ##__VA_ARGS__);      \
		fprintf(stdout, " ---["__FILE__":%d] \n",    \
				__LINE__);							 \
	} while (0)										 \

	#else

	#define CLOG(fmt, ...)							 \
	do{												 \
	char output_buff[MAX_MSG_LENGTH];				 \
	sprintf(output_buff, fmt, ##__VA_ARGS__);		 \
	PropertyConfigurator::doConfigure(				 \
						  LOG4CPLUS_TEXT(CFG_NAME)); \
	LOG4CPLUS_INFO(Logger::getRoot(), output_buff);  \
	} while(0)										 \

	#endif	// USE_LOG4CPLUS
#endif		// LOG_SHUTDOWN

#endif  	// __CTRLOUT_H_
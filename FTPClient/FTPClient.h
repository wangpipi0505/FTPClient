
// FTPClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFTPClientApp:
// �йش����ʵ�֣������ FTPClient.cpp
//

class CFTPClientApp : public CWinApp
{
public:
	CFTPClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFTPClientApp theApp;
//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                     Copyright 2007-2008 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "OlsSample.h"
#include "OlsSampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef RUN_TIME_DYNAMIC_LINKING
#include "../../WinRing0Dll/OlsApiInit.h"
#else // for Load-Time Dynamic Linking
#include "../../WinRing0Dll/OlsApi.h"
#ifdef _M_X64
#pragma comment(lib, "WinRing0x64.lib")
#else if
#pragma comment(lib, "WinRing0.lib")
#endif
#endif

// COlsSampleApp

BEGIN_MESSAGE_MAP(COlsSampleApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// COlsSampleApp construction

COlsSampleApp::COlsSampleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only COlsSampleApp object

COlsSampleApp theApp;


// COlsSampleApp initialization

BOOL COlsSampleApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

#ifdef RUN_TIME_DYNAMIC_LINKING
	HMODULE m_hOpenLibSys = NULL;
	if(! InitOpenLibSys(&m_hOpenLibSys))
	{
		AfxMessageBox(_T("DLL Load Error!!"));
		return FALSE;
	}
#else if
	if(! InitializeOls())
	{
		AfxMessageBox(_T("Error InitializeOls()!!"));
		return FALSE;
	}
#endif

	COlsSampleDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

#ifdef RUN_TIME_DYNAMIC_LINKING
	DeinitOpenLibSys(&m_hOpenLibSys);
#else if
	DeinitializeOls();
#endif

	return FALSE;
}

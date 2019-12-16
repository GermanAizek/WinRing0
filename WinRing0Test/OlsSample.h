//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                          Copyright 2007 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// COlsSampleApp:
// See OlsSample.cpp for the implementation of this class
//

class COlsSampleApp : public CWinApp
{
public:
	COlsSampleApp();

#ifdef RUN_TIME_DYNAMIC_LINKING
	HMODULE m_hOpenLibSys;
#endif

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern COlsSampleApp theApp;
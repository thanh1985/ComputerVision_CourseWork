
// cvHw2.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CcvHw2App:
// See cvHw2.cpp for the implementation of this class
//

class CcvHw2App : public CWinApp
{
public:
	CcvHw2App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CcvHw2App theApp;
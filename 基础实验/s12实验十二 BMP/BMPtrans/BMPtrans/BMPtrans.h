
// BMPtrans.h : main header file for the BMPtrans application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CBMPtransApp:
// See BMPtrans.cpp for the implementation of this class
//

class CBMPtransApp : public CWinApp
{
public:
	CBMPtransApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CBMPtransApp theApp;

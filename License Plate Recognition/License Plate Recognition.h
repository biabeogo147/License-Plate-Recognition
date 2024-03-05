
// License Plate Recognition.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CLicensePlateRecognitionApp:
// See License Plate Recognition.cpp for the implementation of this class
//

class CLicensePlateRecognitionApp : public CWinApp
{
public:
	CLicensePlateRecognitionApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CLicensePlateRecognitionApp theApp;

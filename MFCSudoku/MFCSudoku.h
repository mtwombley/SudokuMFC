
// MFCSudoku.h : main header file for the MFCSudoku application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMFCSudokuApp:
// See MFCSudoku.cpp for the implementation of this class
//

class CMFCSudokuApp : public CWinApp
{
public:
	CMFCSudokuApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCSudokuApp theApp;

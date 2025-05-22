
// ChildView.h : interface of the CChildView class
//

#pragma once
#include "CGridCtrl.h"

// CChildView window

class CChildView : public CWnd
{
// Attributes
public:
  CGridCtrl m_gridCtrl;

// Construction
public:
	CChildView();

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
  afx_msg void OnChangeUIState(UINT nAction, UINT nUIElement);
  afx_msg void OnFileNewrandom();
  afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
  afx_msg void OnSetFocus(CWnd *);
  DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnRButtonUp( UINT nFlags, CPoint point );
};


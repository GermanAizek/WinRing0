//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                          Copyright 2007 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#pragma once
#include "afxwin.h"


// COlsSampleDlg dialog
class COlsSampleDlg : public CDialog
{
// Construction
public:
	COlsSampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_OLSSAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	BOOL m_IsNT;
	void Refresh();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	CEdit m_Edit;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedRefresh();
};

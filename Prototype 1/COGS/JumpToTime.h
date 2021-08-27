#if !defined(AFX_JUMPTOTIME_H__FC25FE2A_8152_4FB9_AB56_BE137729523D__INCLUDED_)
#define AFX_JUMPTOTIME_H__FC25FE2A_8152_4FB9_AB56_BE137729523D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JumpToTime.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJumpToTime dialog

class CJumpToTime : public CDialog
{
// Construction
public:
	CJumpToTime(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CJumpToTime)
	enum { IDD = IDD_DIALOG_JUMP };
	double	m_jumpTime;
	int		m_indexOfPassenger;
	int		m_jumpKind;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJumpToTime)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJumpToTime)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JUMPTOTIME_H__FC25FE2A_8152_4FB9_AB56_BE137729523D__INCLUDED_)

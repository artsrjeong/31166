#if !defined(AFX_DLGSELECTALGORITHM_H__EC6805EF_CE29_4042_9E87_949D2B356F09__INCLUDED_)
#define AFX_DLGSELECTALGORITHM_H__EC6805EF_CE29_4042_9E87_949D2B356F09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelectAlgorithm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectAlgorithm dialog

class CDlgSelectAlgorithm : public CDialog
{
// Construction
public:
	int mSelected;
	int nAlgorithm;
	CString DLLName;
	void SetInfo(char info[13][50]);
	CString AlgorithmName[20];
	CDlgSelectAlgorithm(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSelectAlgorithm)
	enum { IDD = IDD_SELECT_ALGORITHM };
	CListBox	m_ListAlgorithm;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectAlgorithm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelectAlgorithm)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELECTALGORITHM_H__EC6805EF_CE29_4042_9E87_949D2B356F09__INCLUDED_)

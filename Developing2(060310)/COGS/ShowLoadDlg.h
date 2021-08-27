#if !defined(AFX_SHOWLOADDLG_H__5C20E1F9_18D5_47CD_9E91_06F8E1D4B6A6__INCLUDED_)
#define AFX_SHOWLOADDLG_H__5C20E1F9_18D5_47CD_9E91_06F8E1D4B6A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowLoadDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShowLoadDlg dialog

class CShowLoadDlg : public CDialog
{
// Construction
public:
	void SetPassengerNumber(int nPassenger[MAXCAR][MAXFLOOR][MAXDIR], int nCar, int nFloor);
	int m_nPassenger[MAXCAR][MAXFLOOR][MAXDIR];
	int m_nCar;
	int m_nFloor;
	CShowLoadDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShowLoadDlg)
	enum { IDD = IDD_LOAD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowLoadDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShowLoadDlg)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CBrush  brushBk, brushBk1;
	CPen penB2, penDG0;
	BOOL m_bShowFlag;
	CSize m_sCell;
	CSize m_margin;
	CRect rectDlg, rectTable;
	void ShowPassengerNumber();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWLOADDLG_H__5C20E1F9_18D5_47CD_9E91_06F8E1D4B6A6__INCLUDED_)

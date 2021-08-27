//{{AFX_INCLUDES()
#include "msflexgrid.h"
#include "2DSim.h"	// Added by ClassView
//}}AFX_INCLUDES
#if !defined(AFX_DLGSETFLOOR_H__A3AA0C5D_D3A4_42E1_9EEA_F3C92D8A2D12__INCLUDED_)
#define AFX_DLGSETFLOOR_H__A3AA0C5D_D3A4_42E1_9EEA_F3C92D8A2D12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetFloor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetFloor dialog

class CDlgSetFloor : public CDialog
{
// Construction
public:
	void RetrieveAll();
	void CellPaste(CString cstr);
	CString GetClipboardText();
	void SetClipboardText(CString cstr);
	int m_nLogY;
	int m_nLogX;
	long m_lBorderHeight;
	long m_lBorderWidth;
	void RefreshFloorHeight(int fromFloor);
	void RefreshFloorPosition(int fromFloor);
	BUILDING_STRUCTURE Building;
	void SetBuildingParameter(BUILDING_STRUCTURE *b);
	CRect GetSelectedCellRect();
	int floorCount;
	CDlgSetFloor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetFloor)
	enum { IDD = IDD_DLG_FLOOR_CONFIG };
	CEdit	m_edit_tmp;
	CMSFlexGrid	m_Table;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetFloor)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSetFloor)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblClickMsflexgrid1();
	afx_msg void OnKillfocusEditTmp();
	virtual void OnOK();
	afx_msg void OnKeyDownMsflexgrid1(short FAR* KeyCode, short Shift);
	afx_msg void OnKeyPressMsflexgrid1(short FAR* KeyAscii);
	afx_msg void OnScrollMsflexgrid1();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETFLOOR_H__A3AA0C5D_D3A4_42E1_9EEA_F3C92D8A2D12__INCLUDED_)

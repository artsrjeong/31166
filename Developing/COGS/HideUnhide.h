#if !defined(AFX_HIDEUNHIDE_H__21CDD3D9_AF4C_4B08_B861_F26DF20B0DD5__INCLUDED_)
#define AFX_HIDEUNHIDE_H__21CDD3D9_AF4C_4B08_B861_F26DF20B0DD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HideUnhide.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHideUnhide dialog

class CHideUnhide : public CDialog
{
// Construction
public:
	CHideUnhide(CWnd* pParent = NULL, double initiateTime = 0.0);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHideUnhide)
	enum { IDD = IDD_HIDE_UNHIDE };
	int		m_iCar;
	int		m_iDestination;
	double	m_initiateTime;
	double	m_releaseTime;
	BOOL	m_bHoldDoorOpen;
	int		m_IBasis;
	double	m_IDelay;
	int		m_IDirection;
	int		m_IRelativity;
	double	m_IThereshold;
	int		m_RBasis;
	double	m_RDelay;
	int		m_RDirection;
	int		m_RRelativity;
	double	m_RThreshold;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHideUnhide)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHideUnhide)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void test();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HIDEUNHIDE_H__21CDD3D9_AF4C_4B08_B861_F26DF20B0DD5__INCLUDED_)

#if !defined(AFX_USERCONFIGURATION_H__03F2A7BB_6D9C_445B_A924_50F30F63EE0E__INCLUDED_)
#define AFX_USERCONFIGURATION_H__03F2A7BB_6D9C_445B_A924_50F30F63EE0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserConfiguration.h : header file
//
#include "DlgSetfloor.h"
#include "2DSim.h"	// Added by ClassView
/////////////////////////////////////////////////////////////////////////////
// CUserConfiguration dialog

class CUserConfiguration : public CDialog
{
// Construction
public:
	BOOL GetHoistwayManager();
	void SetHoistwayManager(BOOL bHM);
	BOOL GetCOGSSpy();
	void SetCOGSSpy(BOOL bSpy);
	BOOL GetReopen();
	void SetReopen(BOOL bReopen);
	int m_iDwellType;
	void SetDwellType(int iDwellType);
	int GetDwellType();
	/*void SetDLLInfo(BOOL bDispatcherAttached,CString dispatcherDLLFilePath,
		CString dispatcherDLLFileName,int algorithmID,CString algorithmName);
		*/
	BOOL bManual;
	CString m_DLLFileName;
	BOOL bValidDLL;
	CString m_AlgorithmName;
	CString m_DLLFilePath;
	int m_AlgorithmID;
	int SelectDispatchingAlgorithm(char info[][50]);
	BOOL GetMotionKind();
	void SetMotionKind(BOOL bHard);
	int duty;
	int dutyID;
	int CheckValidity(); // Check validity of user input when OK button pressed
	BUILDING_STRUCTURE Building;
	void SetBuidlingParameter(BUILDING_STRUCTURE *b);
	void SetMessageFlag(int flag);
	void SetAdjacentLimit(int limit);
	void SetParameter(PARAMETER *p);
	void SetReassignFlag(BOOL bReassign);
	
	CUserConfiguration(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUserConfiguration)
	enum { IDD = IDD_USER_CONFIGURATION };
	CComboBox	m_ComboDwellType;
	CComboBox	m_ComboDuty;
	double	m_closingTime;
	double	m_openingTime;
	UINT	m_capacity;
	BOOL	m_overhead;
	BOOL	m_pit;
	int		m_fOffset;
	BOOL	m_msgDisable;
	BOOL	m_bReassign;
	int	m_AdjacentLimit;
	UINT	m_hoistways;
	UINT	m_cars;
	UINT	m_floors;
	double	m_acceleration;
	double	m_jerk;
	double	m_startDelay;
	double	m_stopDelay;
	double	m_velocity;
	int		m_lobbyFloor;
	BOOL	m_C;
	BOOL	m_CD;
	BOOL	m_D;
	BOOL	m_H;
	BOOL	m_HC;
	BOOL	m_HCD;
	BOOL	m_HD;
	BOOL	m_NULL;
	BOOL	m_bParamHard;
	BOOL	m_bReopen;
	BOOL	m_bSpy;
	BOOL	m_bHM;
	double	m_minimumSeparationDistance;
	BOOL	m_bDwellTimeExtendACS;
	BOOL	m_bRunDelayACS;
	BOOL	m_bReducedProfile;
	//}}AFX_DATA

	CDlgSetFloor dlgSetFloor;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserConfiguration)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserConfiguration)
	afx_msg void OnButtonFloorConfig();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeEditHoistways();
	afx_msg void OnSelchangeComboDuty();
	afx_msg void OnCheckMotionHard();
	afx_msg void OnButtonAutomaticDispatch();
	afx_msg void OnButtonManualDispatch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERCONFIGURATION_H__03F2A7BB_6D9C_445B_A924_50F30F63EE0E__INCLUDED_)

// 2DSim.h : main header file for the 2DSIM application
//

#if !defined(AFX_2DSIM_H__E9927B84_8DBD_4DB2_984D_90BB7CC4BEB4__INCLUDED_)
#define AFX_2DSIM_H__E9927B84_8DBD_4DB2_984D_90BB7CC4BEB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "../common/2Dsim_defs.h"

//#include "..\HoistwayPlanner\hoistway_plan.h"

/////////////////////////////////////////////////////////////////////////////
// CMy2DSimApp:
// See 2DSim.cpp for the implementation of this class
//

class CMy2DSimApp : public CWinApp
{
public:
	CMy2DSimApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy2DSimApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMy2DSimApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DSIM_H__E9927B84_8DBD_4DB2_984D_90BB7CC4BEB4__INCLUDED_)

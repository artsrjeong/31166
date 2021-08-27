// JumpToTime.cpp : implementation file
//

#include "stdafx.h"
#include "2dsim.h"
#include "JumpToTime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJumpToTime dialog


CJumpToTime::CJumpToTime(CWnd* pParent /*=NULL*/)
	: CDialog(CJumpToTime::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJumpToTime)
	m_jumpTime = 0.0;
	m_indexOfPassenger = 0;
	m_jumpKind = 0;
	//}}AFX_DATA_INIT
}


void CJumpToTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJumpToTime)
	DDX_Text(pDX, IDC_EDIT_TIME, m_jumpTime);
	DDX_Text(pDX, IDC_EDIT_INDEX_PASSENGER, m_indexOfPassenger);
	DDX_Radio(pDX, IDC_RADIO_JUMP_TIME, m_jumpKind);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJumpToTime, CDialog)
	//{{AFX_MSG_MAP(CJumpToTime)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJumpToTime message handlers

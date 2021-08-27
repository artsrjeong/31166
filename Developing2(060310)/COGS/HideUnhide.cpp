// HideUnhide.cpp : implementation file
//

#include "stdafx.h"
#include "2dsim.h"
#include "HideUnhide.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHideUnhide dialog


CHideUnhide::CHideUnhide(CWnd* pParent /*=NULL*/, double initiateTime /*=0.0*/)
	: CDialog(CHideUnhide::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHideUnhide)
	m_iCar = 0;
	m_iDestination = 0;
	m_initiateTime = initiateTime;
	m_releaseTime = initiateTime;
	m_bHoldDoorOpen = FALSE;
	m_IBasis = 0;
	m_IDelay = 0.0;
	m_IDirection = 0;
	m_IRelativity = 0;
	m_IThereshold = 0.0;
	m_RBasis = 0;
	m_RDelay = 0.0;
	m_RDirection = 0;
	m_RRelativity = 0;
	m_RThreshold = 0.0;
	//}}AFX_DATA_INIT
}


void CHideUnhide::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHideUnhide)
	DDX_Text(pDX, IDC_EDIT_CAR, m_iCar);
	DDX_Text(pDX, IDC_EDIT_DESTINATION, m_iDestination);
	DDX_Text(pDX, IDC_EDIT_INITIATE, m_initiateTime);
	DDX_Text(pDX, IDC_EDIT_RELEASE, m_releaseTime);
	DDX_Check(pDX, IDC_CHECK_HOLD_DOOR_OPEN, m_bHoldDoorOpen);
	DDX_Text(pDX, IDC_EDIT_I_BASIS, m_IBasis);
	DDX_Text(pDX, IDC_EDIT_I_DELAY, m_IDelay);
	DDX_Text(pDX, IDC_EDIT_I_DIRECTION, m_IDirection);
	DDX_Text(pDX, IDC_EDIT_I_RELATIVITY, m_IRelativity);
	DDX_Text(pDX, IDC_EDIT_I_THRESHOLD, m_IThereshold);
	DDX_Text(pDX, IDC_EDIT_R_BASIS, m_RBasis);
	DDX_Text(pDX, IDC_EDIT_R_DELAY, m_RDelay);
	DDX_Text(pDX, IDC_EDIT_R_DIRECTION, m_RDirection);
	DDX_Text(pDX, IDC_EDIT_R_RELATIVITY, m_RRelativity);
	DDX_Text(pDX, IDC_EDIT_R_THRESHOLD, m_RThreshold);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHideUnhide, CDialog)
	//{{AFX_MSG_MAP(CHideUnhide)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHideUnhide message handlers

void CHideUnhide::test()
{

}

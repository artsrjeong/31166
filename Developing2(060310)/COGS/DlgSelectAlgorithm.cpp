// DlgSelectAlgorithm.cpp : implementation file
//

#include "stdafx.h"
#include "2dsim.h"
#include "DlgSelectAlgorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectAlgorithm dialog


CDlgSelectAlgorithm::CDlgSelectAlgorithm(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectAlgorithm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelectAlgorithm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSelectAlgorithm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectAlgorithm)
	DDX_Control(pDX, IDC_LIST_ALGORITHM, m_ListAlgorithm);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectAlgorithm, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectAlgorithm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectAlgorithm message handlers

BOOL CDlgSelectAlgorithm::OnInitDialog() 
{
	int i;
	CDialog::OnInitDialog();

	for (i=0;i<nAlgorithm;i++)
	{
		m_ListAlgorithm.AddString(AlgorithmName[i]);
	}
	m_ListAlgorithm.SetCurSel(0);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectAlgorithm::SetInfo(char info[][50])
{
	int i;
	DLLName=info[0];
	nAlgorithm=info[2][0];

	if (nAlgorithm>20)
	{
		nAlgorithm=20;
	}
	for (i=0;i<nAlgorithm;i++)
	{
		AlgorithmName[i]=info[3+i];
	}
}

void CDlgSelectAlgorithm::OnOK() 
{
	// TODO: Add extra validation here
	mSelected=m_ListAlgorithm.GetCurSel();
	CDialog::OnOK();
}

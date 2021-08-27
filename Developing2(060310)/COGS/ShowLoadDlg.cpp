// ShowLoadDlg.cpp : implementation file
//

#include "stdafx.h"
#include "2dsim.h"
#include "ShowLoadDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowLoadDlg dialog


CShowLoadDlg::CShowLoadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowLoadDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShowLoadDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CShowLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowLoadDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShowLoadDlg, CDialog)
	//{{AFX_MSG_MAP(CShowLoadDlg)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowLoadDlg message handlers

void CShowLoadDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if(m_bShowFlag)
		ShowPassengerNumber();
	// Do not call CDialog::OnPaint() for painting messages
}

//Floor labels are hard displayed.
//Without pit or overhead, it has some problems.
//So this function needs spec communition to show floor labels according to spec.
void CShowLoadDlg::ShowPassengerNumber()
{
	int row, column;
	CRect rectCell;
	CString strLoad, strTmp;
	CPen *pOldPen;
	CBrush *pOldBrush;

	CDC* pDC = this->GetDC();
	
	this->GetClientRect(&rectDlg);

	m_sCell.cx = (int)(rectDlg.right - 10)/(m_nCar*2 + 1);
	m_sCell.cy = (int)(rectDlg.bottom - 60)/(m_nFloor + 2);

	m_margin.cx = (int)(rectDlg.right - m_sCell.cx*(m_nCar*2+1))/2;
	m_margin.cy = (int)(rectDlg.bottom - m_sCell.cy*(m_nFloor+2))*3/4;

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_CENTER);

	pOldPen = pDC->SelectObject(&penDG0);
	pOldBrush = pDC->SelectObject(&brushBk);

	rectCell.SetRect(m_margin.cx, m_margin.cy, m_margin.cx+m_sCell.cx, m_margin.cy+m_sCell.cy*2);
	pDC->Rectangle(rectCell);

	rectCell.SetRect(m_margin.cx+m_sCell.cx, m_margin.cy, m_margin.cx+m_sCell.cx*3, m_margin.cy+m_sCell.cy);
	for(column=0; column<m_nCar; column++)
	{
		pDC->Rectangle(rectCell);
		strTmp.Format("Car %d", column);
		//pDC->TextOut(rectCell.left, rectCell.top, strTmp);
		pDC->ExtTextOut(rectCell.CenterPoint().x, rectCell.CenterPoint().y-5, ETO_CLIPPED, rectCell, strTmp, NULL);
		
		rectCell.OffsetRect(m_sCell.cx*2, 0);
	}

	rectCell.SetRect(m_margin.cx+m_sCell.cx, m_margin.cy+m_sCell.cy, m_margin.cx+m_sCell.cx*2, m_margin.cy+m_sCell.cy*2);
	for(column=0; column<m_nCar*2; column++)
	{
		pDC->Rectangle(rectCell);
		if(column % 2)
			pDC->ExtTextOut(rectCell.CenterPoint().x, rectCell.CenterPoint().y-5, ETO_CLIPPED, rectCell, "DN", NULL);
		else
			pDC->ExtTextOut(rectCell.CenterPoint().x, rectCell.CenterPoint().y-5, ETO_CLIPPED, rectCell, "UP", NULL);
		rectCell.OffsetRect(m_sCell.cx, 0);
	}

	rectCell.SetRect(m_margin.cx, m_margin.cy+m_sCell.cy*2, m_margin.cx+m_sCell.cx, m_margin.cy+m_sCell.cy*3);
	for(row=0; row<m_nFloor; row++)
	{
		for(column=0; column<m_nCar*2+1; column++)
		{
			pDC->Rectangle(rectCell);
			if(column == 0)
			{
				if(row == 0)
				{
					pDC->ExtTextOut(rectCell.CenterPoint().x, rectCell.CenterPoint().y-5, ETO_CLIPPED, rectCell, "OH", NULL);
				}
				else if(row == m_nFloor-2)
				{
					pDC->ExtTextOut(rectCell.CenterPoint().x, rectCell.CenterPoint().y-5, ETO_CLIPPED, rectCell, "L", NULL);
				}
				else if(row == m_nFloor-1)
				{
					pDC->ExtTextOut(rectCell.CenterPoint().x, rectCell.CenterPoint().y-5, ETO_CLIPPED, rectCell, "Pit", NULL);
				}
				else
				{
					strTmp.Format("%2d", m_nFloor-row-2);
					pDC->ExtTextOut(rectCell.CenterPoint().x, rectCell.CenterPoint().y-5, ETO_CLIPPED, rectCell, strTmp, NULL);
				}
			}
			else if(column % 2)
			{
				strTmp.Format("%2d", m_nPassenger[column/2][m_nFloor-row-1][UP_INDEX]);
				pDC->ExtTextOut(rectCell.CenterPoint().x, rectCell.CenterPoint().y-5, ETO_CLIPPED, rectCell, strTmp, NULL);
			}
			else
			{
				strTmp.Format("%2d", m_nPassenger[column/2-1][m_nFloor-row-1][DN_INDEX]);
				pDC->ExtTextOut(rectCell.CenterPoint().x, rectCell.CenterPoint().y-5, ETO_CLIPPED, rectCell, strTmp, NULL);
			}
			rectCell.OffsetRect(m_sCell.cx, 0);
		}
		rectCell.OffsetRect(-column*m_sCell.cx, m_sCell.cy);
	}
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	ReleaseDC(pDC);
}

void CShowLoadDlg::SetPassengerNumber(int nPassenger[MAXCAR][MAXFLOOR][MAXDIR], int nCar, int nFloor)
{
	int dir, c, f;//direction, car, floor

	m_nCar = nCar;
	m_nFloor = nFloor;
	for(c=0; c<nCar; c++)
	{
		for(f=0; f<nFloor; f++)
		{
			for(dir=0; dir<MAXDIR; dir++)
			{
				m_nPassenger[c][f][dir] = nPassenger[c][f][dir];
			}
		}
	}
	m_bShowFlag = TRUE;
	Invalidate();
}

BOOL CShowLoadDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bShowFlag = FALSE;
	penB2.CreatePen(PS_SOLID, 2, BLACK);
	penDG0.CreatePen(PS_SOLID, 0, DARKGRAY);
	
	brushBk.CreateSolidBrush(RGB(240,240,240));
	brushBk1.CreateSolidBrush(WHITE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CShowLoadDlg::DestroyWindow() 
{
	penB2.DeleteObject();
	penDG0.DeleteObject();

	brushBk.DeleteObject();
	brushBk1.DeleteObject();

	return CDialog::DestroyWindow();
}

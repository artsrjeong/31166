// DlgSetFloor.cpp : implementation file
//

#include "stdafx.h"
#include "2dsim.h"
#include "DlgSetFloor.h"
#include <Afxctl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetFloor dialog


CDlgSetFloor::CDlgSetFloor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetFloor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetFloor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgSetFloor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetFloor)
	DDX_Control(pDX, IDC_EDIT_TMP, m_edit_tmp);
	DDX_Control(pDX, IDC_MSFLEXGRID1, m_Table);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetFloor, CDialog)
	//{{AFX_MSG_MAP(CDlgSetFloor)
	ON_EN_KILLFOCUS(IDC_EDIT_TMP, OnKillfocusEditTmp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetFloor message handlers

BOOL CDlgSetFloor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int floor,col;
	char floorIndex[20];
	CRect rect;
	// initialize coordinate variable for Grid Control
	m_Table.GetWindowRect(&rect);
	CPoint gridCntlPoint;
	gridCntlPoint=rect.TopLeft();
	ScreenToClient(&gridCntlPoint);
	m_lBorderWidth = gridCntlPoint.x;
	m_lBorderHeight=  gridCntlPoint.y;

	CDC* pDC=m_Table.GetDC();
	m_nLogX = pDC->GetDeviceCaps(LOGPIXELSX);
	m_nLogY	= pDC->GetDeviceCaps(LOGPIXELSY);
	ReleaseDC(pDC);
	//
	SetWindowText("Setup Floor Height");
	floorCount=Building.nTotalFloors;
	m_Table.SetRows(floorCount+1);
	m_Table.SetRowHeight(0,500);
	// table titles
	m_Table.SetColWidth(0,1400);
	m_Table.SetTextMatrix(0,0,"Floor Index");
	m_Table.SetColWidth(1,2200);
	m_Table.SetTextMatrix(0,1,"Floor Label");
	m_Table.SetColWidth(2,1800);
	m_Table.SetTextMatrix(0,2,"Floor Height[m]");
	m_Table.SetColWidth(3,1800);
	m_Table.SetTextMatrix(0,3,"Floor Position[m]");
	// table row index
	for (floor=1;floor<=floorCount;floor++)
	{
		itoa(floorCount-floor,floorIndex,10);
		if (floor==1 && Building.overhead )
		{
			strcat(floorIndex," : OvrHd");
		}
		else if (floor==floorCount && Building.pit)
		{
			strcat(floorIndex," : Pit");
		}
		m_Table.SetTextMatrix(floor,0,floorIndex);
	}
	
	// floor label
	char cInterfloorHeight[10];
	for (floor=1;floor<=floorCount;floor++)
	{
		m_Table.SetTextMatrix(floor,1,Building.floorLabel[floorCount-floor]);
		sprintf(cInterfloorHeight,"%.03lf",Building.interfloorHeight[floorCount-floor]);
		m_Table.SetTextMatrix(floor,2,cInterfloorHeight);
		sprintf(cInterfloorHeight,"%.03lf",Building.floorPosition[floorCount-floor]);
		m_Table.SetTextMatrix(floor,3,cInterfloorHeight);
	}
	for (col=0;col<4;col++)
	{
		m_Table.SetColAlignment(col,4);
	}

	m_Table.SetRow(1);
	m_Table.SetCol(1);
	m_edit_tmp.MoveWindow(GetSelectedCellRect());
	m_edit_tmp.ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// Return selected cell's coordinate in CRect form (left,top,right,bottom) in pixel unit.
CRect CDlgSetFloor::GetSelectedCellRect()
{
	int left_point,right_point,top_point,bottom_point;
	CRect selected;
	
	left_point=(m_Table.GetCellLeft())*m_nLogX/1440 +m_lBorderWidth;
	right_point=left_point+(m_Table.GetCellWidth()*m_nLogX)/1440;
	top_point=((m_Table.GetCellTop())*m_nLogY)/1440+m_lBorderHeight;
	bottom_point=top_point +(m_Table.GetCellHeight()*m_nLogY)/1440; 
	selected.SetRect(left_point,top_point,right_point,bottom_point);

	return selected;
}

BEGIN_EVENTSINK_MAP(CDlgSetFloor, CDialog)
    //{{AFX_EVENTSINK_MAP(CDlgSetFloor)
	ON_EVENT(CDlgSetFloor, IDC_MSFLEXGRID1, -601 /* DblClick */, OnDblClickMsflexgrid1, VTS_NONE)
	ON_EVENT(CDlgSetFloor, IDC_MSFLEXGRID1, -602 /* KeyDown */, OnKeyDownMsflexgrid1, VTS_PI2 VTS_I2)
	ON_EVENT(CDlgSetFloor, IDC_MSFLEXGRID1, -603 /* KeyPress */, OnKeyPressMsflexgrid1, VTS_PI2)
	ON_EVENT(CDlgSetFloor, IDC_MSFLEXGRID1, 73 /* Scroll */, OnScrollMsflexgrid1, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CDlgSetFloor::OnDblClickMsflexgrid1() 
{
	// TODO: Add your control notification handler code here
	short i=13;
	OnKeyPressMsflexgrid1(&i);
}

void CDlgSetFloor::OnKillfocusEditTmp() 
{
	// TODO: Add your control notification handler code here
	int col,row;
	double  ftemp1,ftemp2;
	int floorIndex;
	char cInterfloorHeight[50];
	CString cstr1,cstr2;

	GetDlgItemText(IDC_EDIT_TMP,cstr1);
	col=m_Table.GetCol();
	row=m_Table.GetRow();
	
	if (col==2 && row > 0 ) // floor height is changed
	{
		ftemp1=atof(cstr1);
		cstr2=m_Table.GetTextMatrix(row,col); // floor height
		ftemp2=atof(cstr2);
		
		if (ftemp1!=ftemp2)  //re-calculation of floor position from that floor should occur
		{
			m_Table.SetTextMatrix(row,col,cstr1);
			Building.interfloorHeight[Building.nTotalFloors-row]=ftemp1;
			// recalculation of floorposition from (argument) and update display
			RefreshFloorPosition(Building.nTotalFloors-row); 
		}
	}
	else if (col==3 && row >0) // floor position is changed
	{
		ftemp1=atof(cstr1);
		cstr2=m_Table.GetTextMatrix(row,col); // floor height
		ftemp2=atof(cstr2);
		
		if (ftemp1!=ftemp2)  //re-calculation of floor height at that floor should occur
		{
			floorIndex=Building.nTotalFloors-row;
			Building.floorPosition[floorIndex]=ftemp1;
			m_Table.SetTextMatrix(row,col,cstr1);
			if (floorIndex==0)
			{
				Building.interfloorHeight[floorIndex]=Building.floorPosition[floorIndex+1]
					-Building.floorPosition[floorIndex];
				sprintf(cInterfloorHeight,"%.03lf",Building.interfloorHeight[floorIndex]);
				m_Table.SetTextMatrix(row,2,cInterfloorHeight);
			}
			else if (floorIndex==Building.nTotalFloors-1)
			{
				Building.interfloorHeight[floorIndex-1]=Building.floorPosition[floorIndex]
					-Building.floorPosition[floorIndex-1];
				sprintf(cInterfloorHeight,"%.03lf",Building.interfloorHeight[floorIndex-1]);
				m_Table.SetTextMatrix(row+1,2,cInterfloorHeight);
			}
			else
			{
				Building.interfloorHeight[floorIndex]=Building.floorPosition[floorIndex+1]
					-Building.floorPosition[floorIndex];
				sprintf(cInterfloorHeight,"%.03lf",Building.interfloorHeight[floorIndex]);
				m_Table.SetTextMatrix(row,2,cInterfloorHeight);

				Building.interfloorHeight[floorIndex-1]=Building.floorPosition[floorIndex]
					-Building.floorPosition[floorIndex-1];
				sprintf(cInterfloorHeight,"%.03lf",Building.interfloorHeight[floorIndex-1]);
				m_Table.SetTextMatrix(row+1,2,cInterfloorHeight);
			}
		}
	}
	else
	{
		m_Table.SetTextMatrix(row,col,cstr1);
	}
	
}

// Retrieve Building information from User Interface and Close Dialog
void CDlgSetFloor::OnOK() 
{
	// TODO: Add extra validation here
	int floor;

	CString cstr;
	double floatTmp;

	// retrieve building information from user interface
	for (floor=0;floor<floorCount;floor++)
	{
		cstr=m_Table.GetTextMatrix(Building.nTotalFloors-floor,1); // label
		strcpy(Building.floorLabel[floor],cstr);
		cstr=m_Table.GetTextMatrix(Building.nTotalFloors-floor,2); // floor height
		floatTmp=atof(cstr);
		Building.interfloorHeight[floor]=floatTmp;
		cstr=m_Table.GetTextMatrix(Building.nTotalFloors-floor,3); // floor Position
		floatTmp=atof(cstr);
		Building.floorPosition[floor] =floatTmp;
	}
	if (floor>0) // floor == floorCount
		Building.floorPosition[floor]=Building.floorPopulation[floor-1]+Building.interfloorHeight[floor-1];
	CDialog::OnOK();
}

// Copy Building Parameters for local use in Dialog
void CDlgSetFloor::SetBuildingParameter(BUILDING_STRUCTURE *b)
{
	memcpy(&Building,b,sizeof(BUILDING_STRUCTURE));
}

// Recalculation/Redisplay of Floor position from 'fromFloor' by Building.floorPosition info.
void CDlgSetFloor::RefreshFloorPosition(int fromFloor)
{
	int startfloor,floor;
	char cFloorPosition[30];
	startfloor=fromFloor;

	if (startfloor==0)
	{
		Building.floorPosition[1]=Building.floorPosition[0]+Building.interfloorHeight[0];
		startfloor=1;
	}

	for (floor=startfloor;floor<=Building.nTotalFloors;floor++)
	{
		Building.floorPosition[floor]=Building.floorPosition[floor-1]
			+Building.interfloorHeight[floor-1];
		sprintf(cFloorPosition,"%.03lf",Building.floorPosition[floor]);
		m_Table.SetTextMatrix(floorCount-floor,3,cFloorPosition);
	}
}

// Refresh Floor Height display from fromFloor (No calculation)
void CDlgSetFloor::RefreshFloorHeight(int fromFloor)
{
	char cInterfloorHeight[30];
	int floor;

	for (floor=fromFloor;floor<floorCount;floor++)
	{
		sprintf(cInterfloorHeight,"%.03lf",Building.interfloorHeight[floorCount-floor]);
		m_Table.SetTextMatrix(floor,2,cInterfloorHeight);
	}
}

void CDlgSetFloor::RetrieveAll()
{
	int floor;
	CString cstr;
	char cfloorPosition[50];
	double floatTmp;

	// retrieve building information from user interface
	for (floor=0;floor<floorCount;floor++)
	{
		cstr=m_Table.GetTextMatrix(Building.nTotalFloors-floor,1); // label
		strcpy(Building.floorLabel[floor],cstr);
		cstr=m_Table.GetTextMatrix(Building.nTotalFloors-floor,2); // floor height
		floatTmp=atof(cstr);
		Building.interfloorHeight[floor]=floatTmp;
		if (floor==0)
		{
			cstr=m_Table.GetTextMatrix(Building.nTotalFloors-floor,3); // floor height
			floatTmp=atof(cstr);
			Building.floorPosition[floor] =floatTmp;
		}
		else
		{
			Building.floorPosition[floor]=Building.floorPosition[floor-1]+Building.interfloorHeight[floor-1];
			sprintf(cfloorPosition,"%.03lf",Building.floorPosition[floor]);
			m_Table.SetTextMatrix(Building.nTotalFloors-floor,3,cfloorPosition);
		}
	}	
}
// Deals with Up/Down/Right/Left Arrow
void CDlgSetFloor::OnKeyDownMsflexgrid1(short FAR* KeyCode, short Shift) 
{
	// TODO: Add your control notification handler code here
	long l_tmp;
    CString st;
	
	if(*KeyCode == VK_UP) {  //[UP ARROW] key

		GotoDlgCtrl( GetDlgItem(IDC_MSFLEXGRID1) );
		m_Table.SetFocus();

		l_tmp = m_Table.GetRow();

		if(l_tmp > 1) m_Table.SetRow(l_tmp-1);
		else          m_Table.SetRow(l_tmp);
	}
	else if(*KeyCode == VK_DOWN) {  //[DOWN ARROW] key

		GotoDlgCtrl( GetDlgItem(IDC_MSFLEXGRID1) );
		m_Table.SetFocus();
		
		l_tmp = m_Table.GetRow();

		if(l_tmp + 1 < m_Table.GetRows()) m_Table.SetRow(l_tmp + 1);
		else                              m_Table.SetRow(l_tmp);
	}
	else if(*KeyCode == VK_LEFT) {  //[LEFT ARROW] key

		GotoDlgCtrl( GetDlgItem(IDC_MSFLEXGRID1) );	
		m_Table.SetFocus();

		l_tmp = m_Table.GetCol();
		
		if(l_tmp > 1) m_Table.SetCol(l_tmp - 1);
		else          m_Table.SetCol(l_tmp);
	}
	else if(*KeyCode == VK_RIGHT) {  //[RIGHT ARROW] key

		GotoDlgCtrl( GetDlgItem(IDC_MSFLEXGRID1) );
		m_Table.SetFocus();
		
		l_tmp = m_Table.GetCol();
		
		if(l_tmp + 1 < m_Table.GetCols()) m_Table.SetCol(l_tmp + 1);
		else                              m_Table.SetCol(l_tmp);
	}
	else if ((Shift & CTRL_MASK) !=0)
	{
		if (*KeyCode=='C' || *KeyCode=='c') // Ctrl-C
		{
			SetClipboardText(m_Table.GetClip());
		}
		else if (*KeyCode=='V' || *KeyCode=='v')
		{
			CellPaste(GetClipboardText());
		}
	}
	
}

// Deals with 'Enter' key
BOOL CDlgSetFloor::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN) 
	{
		if(pMsg->wParam == VK_RETURN) 
		{
			CWnd* pFocus = GetFocus();

			if(pFocus == GetDlgItem(IDC_EDIT_TMP)) 
			{
				GotoDlgCtrl(GetDlgItem(IDC_MSFLEXGRID1));
				m_Table.SetFocus();
                return TRUE;
            }
			else if(pFocus == GetDlgItem(IDC_MSFLEXGRID1)) 
			{
				OnDblClickMsflexgrid1();
				GotoDlgCtrl(GetDlgItem(IDC_EDIT_TMP));
                return TRUE;
            }
			else if(pFocus == GetDlgItem(IDOK)) 
			{
				OnOK();
                return TRUE;
			}
		}
		else if( //(pMsg->wParam == VK_LEFT) ||
			     //(pMsg->wParam == VK_RIGHT) ||
				 (pMsg->wParam == VK_UP) ||
				 (pMsg->wParam == VK_DOWN) ) {

			CWnd* pFocus = GetFocus();
			
			if(pFocus==GetDlgItem(IDC_EDIT_TMP)) 
			{
				GotoDlgCtrl(GetDlgItem(IDC_MSFLEXGRID1));
				return TRUE;
			}
		}
/*		else
			return CDialog::PreTranslateMessage(pMsg); */
	}
	return CDialog::PreTranslateMessage(pMsg);
}

// on Key press, set character in a cell edit
void CDlgSetFloor::OnKeyPressMsflexgrid1(short FAR* KeyAscii) 
{
	// TODO: Add your control notification handler code here
	if(GetFocus() != GetDlgItem(IDC_MSFLEXGRID1) ) 
		return;

	if (::GetKeyState(VK_CONTROL)<0) 
		return;
	
	m_edit_tmp.SetWindowText(m_Table.GetText());
	if (*KeyAscii==13) // 13
	{
		m_edit_tmp.SetSel(0,-1);
	}
	else
	{
		char buf[]=" ";
		buf[0]=(char)*KeyAscii;
		m_edit_tmp.SetWindowText(buf);
		m_edit_tmp.SetSel(1,1);
	}
	m_edit_tmp.MoveWindow(GetSelectedCellRect());
	m_edit_tmp.SetFocus();
	m_edit_tmp.ShowWindow(SW_SHOW);
}

void CDlgSetFloor::SetClipboardText(CString cstr)
{
	HANDLE hMem;
	LPVOID lpData;
	if (!OpenClipboard())
	{
		return;
	}
	if (!EmptyClipboard())
	{
		return;
	}
	hMem = ::GlobalAlloc(GHND, cstr.GetLength() + 1);
	lpData=::GlobalLock(hMem);
	strcpy((char*)lpData,(LPCTSTR)cstr);
	SetClipboardData(CF_TEXT,hMem);
	::GlobalUnlock(hMem);
	::CloseClipboard();
}

CString CDlgSetFloor::GetClipboardText()
{
    CString st = "";
	HANDLE hMem;
	LPVOID lpData;

	if(::OpenClipboard(GetSafeHwnd()) == FALSE) return st;

	if(::IsClipboardFormatAvailable(CF_TEXT)) 
	{
		hMem = ::GetClipboardData(CF_TEXT);
		lpData = ::GlobalLock(hMem);
		st = (char*)lpData;
		::GlobalUnlock(hMem);
	}

	::CloseClipboard();
	
	return st;
}

void CDlgSetFloor::CellPaste(CString cstr)
{
	int nRow = m_Table.GetRow();
	int nCol = m_Table.GetCol();

	int nRows = m_Table.GetRows();
	int nCols = m_Table.GetCols();

	int nRowSel = m_Table.GetRowSel();
	int nColSel = m_Table.GetColSel();

	int temp;
//	int row,col;
	int nSelectedRows,nSelectedCols;

	if (cstr.IsEmpty()) 
		return;

	if (nRow>nRowSel)
	{
		temp=nRow;
		nRow=nRowSel;
		nRowSel=temp;
	}

	if (nCol > nColSel)
	{
		temp=nCol;
		nCol=nColSel;
		nColSel=temp;
	}

	nSelectedRows=nRowSel-nRow+1;
	nSelectedCols=nColSel-nCol+1;

	int clipboard_row,clipboard_col;

	char *ch=_strdup((LPCTSTR)cstr);
	char *pch;
	pch=ch;
	clipboard_col=1;clipboard_row=1;
	while (*pch!='\0')
	{
		if (*pch==0x09)
		{
			clipboard_col++;
		}
		else if (*pch==0x0d)
		{
			clipboard_row++;
		}
		pch++;
	}
	free(ch);

	int nPasteRow,nPasteCol,nPasteSelRow,nPasteSelCol;
//	int nPasteRowStart,nPasteColStart;
	int nPasteRowCount,nPasteColCount;
	int i,j;
	if (nSelectedRows==1) 
	{	// paste one time only
		if (nSelectedCols==1)
		{ // one cell is selected
			nPasteRow=nRow;nPasteCol=nCol;
			nPasteSelRow=min(nPasteRow+clipboard_row-1,nRows);
			nPasteSelCol=min(nPasteCol+clipboard_col-1,nCols);
			m_Table.SetRow(nPasteRow);
			m_Table.SetCol(nPasteCol);
			m_Table.SetRowSel(nPasteSelRow);
			m_Table.SetColSel(nPasteSelCol);
			m_Table.SetClip(cstr);
		}
		else if ((nSelectedCols%clipboard_col) ==0)
		{
			nPasteRowCount=1;nPasteColCount=nSelectedCols/clipboard_col;

			for (i=0;i<nPasteColCount;i++)
			{
				if (nCol+(i+1)*clipboard_col-1 >= nCols) break;
				m_Table.SetRow(nRow);
				m_Table.SetCol(nCol+i*clipboard_col);
				m_Table.SetRowSel(nRow);
				m_Table.SetColSel(nCol+(i+1)*clipboard_col-1);
				m_Table.SetClip(cstr);
			}
			m_Table.SetCol(nCol);
		}
		else
			AfxMessageBox("Copy Area and Paste Area is not matched\n");
	}
	else if (nSelectedCols==1)
	{
		if ((nSelectedRows%clipboard_row) ==0)
		{	
			nPasteColCount=1;nPasteRowCount=nSelectedRows/clipboard_row;

			for (i=0;i<nPasteRowCount;i++)
			{
				if (nRow+(i+1)*clipboard_row-1 >= nRows) break;
				m_Table.SetRow(nRow+i*clipboard_row);
				m_Table.SetCol(nCol);
				m_Table.SetRowSel(nRow+(i+1)*clipboard_row-1);
				m_Table.SetColSel(nCol+clipboard_col);
				m_Table.SetClip(cstr);
			}
			m_Table.SetRow(nRow);
		}
		else
			AfxMessageBox("Copy Area and Paste Area is not matched\n");
	}
	else if ( ((nSelectedCols % clipboard_col) ==0) && ((nSelectedRows % clipboard_row) ==0) )
	{
		nPasteColCount=nSelectedCols/clipboard_col;
		nPasteRowCount=nSelectedRows/clipboard_row;
		for (i=0;i<nPasteRowCount;i++)
		{
			if (nRow+(i+1)*clipboard_row-1 > nRows) break;
			m_Table.SetRow(nRow+i*clipboard_row);
			m_Table.SetRowSel(nRow+(i+1)*clipboard_row-1);
			for (j=0;j<nPasteColCount;j++)
			{
				if (nCol+(i+1)*clipboard_col-1 > nCols) 
				{
					i=nPasteRowCount;
					break;
				}
				m_Table.SetCol(nCol+j*clipboard_col);
				m_Table.SetColSel(nCol+(j+1)*clipboard_col-1);
				m_Table.SetClip(cstr);
			}
		}
		m_Table.SetRow(nRow);
	}
	else
	{
		AfxMessageBox("Copy Area and Paste Area is not matched\n");
	}
	RetrieveAll();

}

void CDlgSetFloor::OnScrollMsflexgrid1() 
{
	// TODO: Add your control notification handler code here
	m_edit_tmp.ShowWindow(SW_HIDE);
	m_Table.SetFocus();
}

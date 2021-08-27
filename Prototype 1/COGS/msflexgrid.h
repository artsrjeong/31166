#if !defined(AFX_MSFLEXGRID_H__50842FF1_8D8D_4ADF_B5EC_95E07F81C805__INCLUDED_)
#define AFX_MSFLEXGRID_H__50842FF1_8D8D_4ADF_B5EC_95E07F81C805__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.

/////////////////////////////////////////////////////////////////////////////
// CMSFlexGrid wrapper class

class CMSFlexGrid : public CWnd
{
protected:
	DECLARE_DYNCREATE(CMSFlexGrid)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x6262d3a0, 0x531b, 0x11cf, { 0x91, 0xf6, 0xc2, 0x86, 0x3c, 0x38, 0x5e, 0x30 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd, UINT nID,
		CCreateContext* pContext = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); }

// Attributes
public:

// Operations
public:
	long GetRows();
	void SetRows(long nNewValue);
	long GetCols();
	void SetCols(long nNewValue);
	long GetFixedRows();
	void SetFixedRows(long nNewValue);
	long GetFixedCols();
	void SetFixedCols(long nNewValue);
	short GetVersion();
	CString GetFormatString();
	void SetFormatString(LPCTSTR lpszNewValue);
	long GetTopRow();
	void SetTopRow(long nNewValue);
	long GetLeftCol();
	void SetLeftCol(long nNewValue);
	long GetRow();
	void SetRow(long nNewValue);
	long GetCol();
	void SetCol(long nNewValue);
	long GetRowSel();
	void SetRowSel(long nNewValue);
	long GetColSel();
	void SetColSel(long nNewValue);
	CString GetText();
	void SetText(LPCTSTR lpszNewValue);
	unsigned long GetBackColor();
	void SetBackColor(unsigned long newValue);
	unsigned long GetForeColor();
	void SetForeColor(unsigned long newValue);
	unsigned long GetBackColorFixed();
	void SetBackColorFixed(unsigned long newValue);
	unsigned long GetForeColorFixed();
	void SetForeColorFixed(unsigned long newValue);
	unsigned long GetBackColorSel();
	void SetBackColorSel(unsigned long newValue);
	unsigned long GetForeColorSel();
	void SetForeColorSel(unsigned long newValue);
	unsigned long GetBackColorBkg();
	void SetBackColorBkg(unsigned long newValue);
	BOOL GetWordWrap();
	void SetWordWrap(BOOL bNewValue);
	LPDISPATCH GetFont();
	void SetRefFont(LPDISPATCH newValue);
	float GetFontWidth();
	void SetFontWidth(float newValue);
	CString GetCellFontName();
	void SetCellFontName(LPCTSTR lpszNewValue);
	float GetCellFontSize();
	void SetCellFontSize(float newValue);
	BOOL GetCellFontBold();
	void SetCellFontBold(BOOL bNewValue);
	BOOL GetCellFontItalic();
	void SetCellFontItalic(BOOL bNewValue);
	BOOL GetCellFontUnderline();
	void SetCellFontUnderline(BOOL bNewValue);
	BOOL GetCellFontStrikeThrough();
	void SetCellFontStrikeThrough(BOOL bNewValue);
	float GetCellFontWidth();
	void SetCellFontWidth(float newValue);
	long GetTextStyle();
	void SetTextStyle(long nNewValue);
	long GetTextStyleFixed();
	void SetTextStyleFixed(long nNewValue);
	BOOL GetScrollTrack();
	void SetScrollTrack(BOOL bNewValue);
	long GetFocusRect();
	void SetFocusRect(long nNewValue);
	long GetHighLight();
	void SetHighLight(long nNewValue);
	BOOL GetRedraw();
	void SetRedraw(BOOL bNewValue);
	long GetScrollBars();
	void SetScrollBars(long nNewValue);
	long GetMouseRow();
	long GetMouseCol();
	long GetCellLeft();
	long GetCellTop();
	long GetCellWidth();
	long GetCellHeight();
	long GetRowHeightMin();
	void SetRowHeightMin(long nNewValue);
	long GetFillStyle();
	void SetFillStyle(long nNewValue);
	long GetGridLines();
	void SetGridLines(long nNewValue);
	long GetGridLinesFixed();
	void SetGridLinesFixed(long nNewValue);
	unsigned long GetGridColor();
	void SetGridColor(unsigned long newValue);
	unsigned long GetGridColorFixed();
	void SetGridColorFixed(unsigned long newValue);
	unsigned long GetCellBackColor();
	void SetCellBackColor(unsigned long newValue);
	unsigned long GetCellForeColor();
	void SetCellForeColor(unsigned long newValue);
	short GetCellAlignment();
	void SetCellAlignment(short nNewValue);
	long GetCellTextStyle();
	void SetCellTextStyle(long nNewValue);
	short GetCellPictureAlignment();
	void SetCellPictureAlignment(short nNewValue);
	CString GetClip();
	void SetClip(LPCTSTR lpszNewValue);
	void SetSort(short nNewValue);
	long GetSelectionMode();
	void SetSelectionMode(long nNewValue);
	long GetMergeCells();
	void SetMergeCells(long nNewValue);
	BOOL GetAllowBigSelection();
	void SetAllowBigSelection(BOOL bNewValue);
	long GetAllowUserResizing();
	void SetAllowUserResizing(long nNewValue);
	long GetBorderStyle();
	void SetBorderStyle(long nNewValue);
	long GetHWnd();
	BOOL GetEnabled();
	void SetEnabled(BOOL bNewValue);
	long GetAppearance();
	void SetAppearance(long nNewValue);
	long GetMousePointer();
	void SetMousePointer(long nNewValue);
	LPDISPATCH GetMouseIcon();
	void SetRefMouseIcon(LPDISPATCH newValue);
	long GetPictureType();
	void SetPictureType(long nNewValue);
	LPDISPATCH GetPicture();
	LPDISPATCH GetCellPicture();
	void SetRefCellPicture(LPDISPATCH newValue);
	CString GetTextArray(long index);
	void SetTextArray(long index, LPCTSTR lpszNewValue);
	short GetColAlignment(long index);
	void SetColAlignment(long index, short nNewValue);
	long GetColWidth(long index);
	void SetColWidth(long index, long nNewValue);
	long GetRowHeight(long index);
	void SetRowHeight(long index, long nNewValue);
	BOOL GetMergeRow(long index);
	void SetMergeRow(long index, BOOL bNewValue);
	BOOL GetMergeCol(long index);
	void SetMergeCol(long index, BOOL bNewValue);
	void SetRowPosition(long index, long nNewValue);
	void SetColPosition(long index, long nNewValue);
	long GetRowData(long index);
	void SetRowData(long index, long nNewValue);
	long GetColData(long index);
	void SetColData(long index, long nNewValue);
	CString GetTextMatrix(long Row, long Col);
	void SetTextMatrix(long Row, long Col, LPCTSTR lpszNewValue);
	void AddItem(LPCTSTR Item, const VARIANT& index);
	void RemoveItem(long index);
	void Clear();
	void Refresh();
	LPDISPATCH GetDataSource();
	void SetDataSource(LPDISPATCH newValue);
	BOOL GetRowIsVisible(long index);
	BOOL GetColIsVisible(long index);
	long GetRowPos(long index);
	long GetColPos(long index);
	short GetGridLineWidth();
	void SetGridLineWidth(short nNewValue);
	short GetFixedAlignment(long index);
	void SetFixedAlignment(long index, short nNewValue);
	BOOL GetRightToLeft();
	void SetRightToLeft(BOOL bNewValue);
	long GetOLEDropMode();
	void SetOLEDropMode(long nNewValue);
	void OLEDrag();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSFLEXGRID_H__50842FF1_8D8D_4ADF_B5EC_95E07F81C805__INCLUDED_)

// UserConfiguration.cpp : implementation file
//

#include "stdafx.h"
#include "2dsim.h"
#include "UserConfiguration.h"
#include "DlgSelectAlgorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserConfiguration dialog


CUserConfiguration::CUserConfiguration(CWnd* pParent /*=NULL*/)
	: CDialog(CUserConfiguration::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserConfiguration)
	m_hoistways = 0;
	m_cars = 0;
	m_floors = 0;
	m_acceleration = 0.0;
	m_jerk = 0.0;
	m_startDelay = 0.0;
	m_stopDelay = 0.0;
	m_velocity = 0.0;
	m_lobbyFloor = 0;
	m_C = FALSE;
	m_CD = FALSE;
	m_D = FALSE;
	m_H = FALSE;
	m_HC = FALSE;
	m_HCD = FALSE;
	m_HD = FALSE;
	m_NULL = FALSE;
	m_bParamHard = FALSE;
	m_bReopen = FALSE;
	m_bSpy = FALSE;
	m_bHM = TRUE;
	m_minimumSeparationDistance = 0.0;
	m_bDwellTimeExtendACS = FALSE;
	m_bRunDelayACS = FALSE;
	m_bReducedProfile = FALSE;
	//}}AFX_DATA_INIT
	m_DLLFilePath=_T("");
	m_DLLFileName=_T("");
}


void CUserConfiguration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserConfiguration)
	DDX_Control(pDX, IDC_COMBO_DWELL_TYPE, m_ComboDwellType);
	DDX_Control(pDX, IDC_COMBO_DUTY, m_ComboDuty);
	DDX_Text(pDX, IDC_EDIT_CLOSING_TIME, m_closingTime);
	DDV_MinMaxDouble(pDX, m_closingTime, 0., 10.);
	DDX_Text(pDX, IDC_EDIT_OPENING_TIME, m_openingTime);
	DDV_MinMaxDouble(pDX, m_openingTime, 0., 10.);
	DDX_Text(pDX, IDC_EDIT_CAPACITY, m_capacity);
	DDV_MinMaxUInt(pDX, m_capacity, 0, 30);
	DDX_Check(pDX, IDC_CHECK_OVERHEAD, m_overhead);
	DDX_Check(pDX, IDC_CHECK_PIT, m_pit);
	DDX_Text(pDX, IDC_EDIT_FLOOR_OFFSET, m_fOffset);
	DDX_Check(pDX, IDC_CHECK_MSG_DISABLE, m_msgDisable);
	DDX_Check(pDX, IDC_CHECK_REASSIGN, m_bReassign);
	DDX_Text(pDX, IDC_EDIT_ADJACENT_LIMIT, m_AdjacentLimit);
	DDX_Text(pDX, IDC_EDIT_HOISTWAYS, m_hoistways);
	DDX_Text(pDX, IDC_EDIT_CARS, m_cars);
	DDX_Text(pDX, IDC_EDIT_FLOORCOUNT, m_floors);
	DDX_Text(pDX, IDC_EDIT_ACCELERATION, m_acceleration);
	DDX_Text(pDX, IDC_EDIT_JERK, m_jerk);
	DDX_Text(pDX, IDC_EDIT_START_DELAY, m_startDelay);
	DDX_Text(pDX, IDC_EDIT_STOP_DELAY, m_stopDelay);
	DDX_Text(pDX, IDC_EDIT_VELOCITY, m_velocity);
	DDX_Text(pDX, IDC_EDIT_LOBBY_FLOOR, m_lobbyFloor);
	DDX_Check(pDX, IDC_CHECKC, m_C);
	DDX_Check(pDX, IDC_CHECKCD, m_CD);
	DDX_Check(pDX, IDC_CHECKD, m_D);
	DDX_Check(pDX, IDC_CHECKH, m_H);
	DDX_Check(pDX, IDC_CHECKHC, m_HC);
	DDX_Check(pDX, IDC_CHECKHCD, m_HCD);
	DDX_Check(pDX, IDC_CHECKHD, m_HD);
	DDX_Check(pDX, IDC_CHECKNULL, m_NULL);
	DDX_Check(pDX, IDC_CHECK_MOTION_HARD, m_bParamHard);
	DDX_Check(pDX, IDC_CHECK_REOPEN, m_bReopen);
	DDX_Check(pDX, IDC_CHECK_COGS_SPY, m_bSpy);
	DDX_Check(pDX, IDC_CHECK_HOISTWAYMANAGER, m_bHM);
	DDX_Text(pDX, IDC_EDIT_MINIMUM_DIST, m_minimumSeparationDistance);
	DDV_MinMaxDouble(pDX, m_minimumSeparationDistance, 0., 40.);
	DDX_Check(pDX, IDC_DWELL_TIME_AVOID_CONDITIONAL_STOP, m_bDwellTimeExtendACS);
	DDX_Check(pDX, IDC_RUN_DELAY_AVOID_CONDITIONAL_STOP, m_bRunDelayACS);
	DDX_Check(pDX, IDC_REDUCE_PROFILE, m_bReducedProfile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserConfiguration, CDialog)
	//{{AFX_MSG_MAP(CUserConfiguration)
	ON_BN_CLICKED(IDC_BUTTON_FLOOR_CONFIG, OnButtonFloorConfig)
	ON_EN_CHANGE(IDC_EDIT_HOISTWAYS, OnChangeEditHoistways)
	ON_CBN_SELCHANGE(IDC_COMBO_DUTY, OnSelchangeComboDuty)
	ON_BN_CLICKED(IDC_CHECK_MOTION_HARD, OnCheckMotionHard)
	ON_BN_CLICKED(IDC_BUTTON_AUTOMATIC_DISPATCH, OnButtonAutomaticDispatch)
	ON_BN_CLICKED(IDC_BUTTON_MANUAL_DISPATCH, OnButtonManualDispatch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserConfiguration message handlers

void CUserConfiguration::SetReassignFlag(BOOL bReassign)
{
	m_bReassign = bReassign;
}

void CUserConfiguration::SetParameter(PARAMETER *p)
{
	m_bDwellTimeExtendACS = p->bDwellTimeExtendACS;
	m_bRunDelayACS = p->bRunDelayACS;
	m_bReducedProfile = p->bReducedProfile;
	m_capacity = p->capacity;
	m_openingTime = p->door_open;
	m_closingTime = p->door_close;
	m_AdjacentLimit=p->AdjacentLimit;
	m_fOffset = p->f_offset;
	dutyID=p->dutyID;
	duty=p->duty;
	//shin 040730
	m_jerk = p->MaxJerk;
	m_acceleration = p->MaxAcceleration;
	m_velocity = p->MaxVelocity;
	m_startDelay = p->StartDelay;
	m_stopDelay = p->StopDelay;
	m_minimumSeparationDistance= p->MinSeparationDist;

	// DLL related
	bManual=!p->bAutomaticDispatching;
	m_DLLFilePath=p->dispatcherDLLPath;
	m_DLLFileName=p->dispatcherDLLName;
	m_AlgorithmID=p->algorithmID;
	m_AlgorithmName=p->dispatchingAlgorithmName;
}

void CUserConfiguration::SetAdjacentLimit(int limit)
{
	m_AdjacentLimit = limit;
}

void CUserConfiguration::SetMessageFlag(int flag)
{
	m_msgDisable = flag;
}

void CUserConfiguration::OnButtonFloorConfig() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_floors>MAXFLOOR) 
		m_floors=MAXFLOOR;
	else if (m_floors<2) 
		m_floors=2;

	Building.nUserFloors =m_floors;
	if (m_overhead!=0)
	{
		Building.overhead=1;
	}
	else
		Building.overhead=0;

	if (m_pit!=0)
		Building.pit=1;
	else
		Building.pit=0;
	Building.nTotalFloors=m_floors+Building.overhead+Building.pit;
	
	dlgSetFloor.SetBuildingParameter(&Building);
	if (dlgSetFloor.DoModal()==IDOK)
	{
		SetBuidlingParameter(&(dlgSetFloor.Building));
	}
}
int DutyCapacity[5]={13,15,17,20,24}; //people according to duty id.
int DutyKG[5]={900,1000,1150,1350,1600};

BOOL CUserConfiguration::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_floors=Building.nUserFloors;
	m_cars=Building.nCars;
	m_hoistways=Building.nHoistways;
	m_pit=Building.pit;
	m_overhead=Building.overhead;
	m_lobbyFloor=Building.lobby1Floor;
	
	if (dutyID>=0 && dutyID <5)
	{
		m_ComboDuty.SetCurSel(dutyID);
		m_capacity=DutyCapacity[dutyID];
	}
	else
	{
		m_ComboDuty.SetCurSel(5);
		(CEdit*)GetDlgItem(IDC_EDIT_CAPACITY)->EnableWindow(TRUE);
		m_capacity=duty/65;
	}
	CString DLLDescription;
	if (bManual)
	{
		DLLDescription+= "Manual Dispatch\n";
		DLLDescription+= "Human Intelligence";
	}
	else
	{
		DLLDescription += m_DLLFileName;
		DLLDescription +="\n";
		DLLDescription += m_AlgorithmName;
	}
	SetDlgItemText(IDC_DISPATCH_STATE,DLLDescription);

	m_ComboDwellType.SetCurSel(m_iDwellType); //Default selection is current CAGS behavior

	UpdateData(FALSE);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserConfiguration::SetBuidlingParameter(BUILDING_STRUCTURE *b)
{
	memcpy(&Building,b,sizeof(BUILDING_STRUCTURE));
}

void CUserConfiguration::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	Building.nCars=m_cars;
	Building.nHoistways=m_hoistways;
	Building.nUserFloors=m_floors;
	Building.nTotalFloors=m_floors+m_pit+m_overhead;
	Building.overhead=m_overhead;
	Building.pit=m_pit;
	Building.lobby1Floor=m_lobbyFloor;
	dutyID=m_ComboDuty.GetCurSel();
	if (dutyID>4) 
	{
		dutyID=15;
		duty=65 * m_capacity;
	}
	else if (dutyID >=0)
	{
		duty=DutyKG[dutyID];
	}
	m_iDwellType = m_ComboDwellType.GetCurSel();
	
	//Param
	if (CheckValidity()!=0)	// not valid input
		return;

	CDialog::OnOK();
}

int CUserConfiguration::CheckValidity()
{
	CString cstr,substr;
	int result=0;
	
	cstr.Empty();
	if (m_floors<2)
	{
		result=1;
		cstr+=CString("# of Floors shoud be larger than 1 \n");
	}
	if (m_cars> MAXCAR || m_cars < 1)
	{
		result=2;
		substr.Format("# of cars out of range(1-%d).",MAXCAR);
		cstr+=substr;
	}
	if (m_hoistways>MAXHWY||m_hoistways<1)
	{
		result=2;
		substr.Format("# of hoistways out of range(1-%d)",MAXHWY);
		cstr+=substr;
	}
	if (m_cars<m_hoistways)
	{
		result=2;
		cstr+=CString("# of cars should be larger than # of hoistways\n");
	}
	else if (m_cars>2*m_hoistways)
	{
		result=3;
		cstr+=CString("# of cars should be less than 2 times of # of hoistways, because current simulation don't support 3D\n");
	}
	if (result!=0)
	{
		MessageBox(cstr,"User input error",MB_OK);
	}
	return result;
}

void CUserConfiguration::OnChangeEditHoistways() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	//shin 040817 the number of cars is not twice of the number of hoistway
	// They are independant variables. But they have some relations.
	return;
	// Will be deleted.
	CString cstr;
	GetDlgItemText(IDC_EDIT_HOISTWAYS,cstr);
	m_hoistways=atoi(cstr);
	m_cars=2*m_hoistways;
	cstr.Format("%d",m_cars);
	SetDlgItemText(IDC_EDIT_CARS,cstr);	
}

void CUserConfiguration::OnSelchangeComboDuty() 
{
	// TODO: Add your control notification handler code here
	int sel;
	sel=m_ComboDuty.GetCurSel();
	if (sel==5)
	{
		(CEdit*)GetDlgItem(IDC_EDIT_CAPACITY)->EnableWindow(TRUE);
	}
	else
	{
		(CEdit*)GetDlgItem(IDC_EDIT_CAPACITY)->EnableWindow(FALSE);
		SetDlgItemInt(IDC_EDIT_CAPACITY,DutyCapacity[sel]);
	}
}


void CUserConfiguration::SetMotionKind(BOOL bHard)
{
	m_bParamHard = bHard;
}

BOOL CUserConfiguration::GetMotionKind()
{
	return m_bParamHard;
}

void CUserConfiguration::OnCheckMotionHard() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_bParamHard)
	{
		m_ComboDuty.SetCurSel(5);//custom
		m_capacity = 16;
		m_jerk = 2.0;
		m_acceleration = 1.0;
		m_velocity = 4.0;
		m_openingTime = 1.5;
		m_closingTime = 2.5;
		m_startDelay = 0.0;
		m_stopDelay = 0.0;
		UpdateData(FALSE);
	}
}

void CUserConfiguration::OnButtonAutomaticDispatch() 
{
	// TODO: Add your control notification handler code here
	CFileDialog fdlg(TRUE, "dll",m_DLLFileName,OFN_FILEMUSTEXIST, 
		"Dispatcher DLL(*.dll)|*.dll|All Files (*.*)|*.*||");
	CString errorLog;
	CString stateDescription;
	int strPos;
	CString initialPath;

	HINSTANCE hinstLib;

	DDGetInfo GetDispatcherInfo;
	char info[20][50];
	BOOL bValidDLL=FALSE;
	errorLog.Empty();
	if (!m_DLLFilePath.IsEmpty())
	{	// set dll path as the initial file open path
		strPos=m_DLLFilePath.ReverseFind('\\');

		if (strPos!=-1) // extract only file path
			fdlg.m_ofn.lpstrInitialDir=initialPath=m_DLLFilePath.Left(strPos);
	}
	fdlg.m_ofn.lpstrTitle="Select Dispatcher DLL";

	if (fdlg.DoModal()==IDOK)
	{
		m_DLLFilePath=fdlg.GetPathName(); // get dll file name and path
		m_DLLFileName=fdlg.GetFileName();

		hinstLib=AfxLoadLibrary(m_DLLFilePath); // load dll
		
		if (hinstLib!=NULL) // dll is loaded
		{
			GetDispatcherInfo=(DDGetInfo)GetProcAddress(hinstLib,"GetDispatcherDLLInfo");
			
			if (GetDispatcherInfo!=NULL) // Valid DLL
			{
				GetDispatcherInfo(info);
				// select dialog
				if (SelectDispatchingAlgorithm(info)==1) // SUCCESS
				{
					bValidDLL=TRUE;
					bManual=FALSE;
					m_AlgorithmName=info[3+m_AlgorithmID];
				}
				else
					errorLog+="Algorithm not selected";
			}
			else
				errorLog+="Invalid Dispatcher DLL";

			AfxFreeLibrary(hinstLib); // free library
		}
		else
		{
			errorLog+="Can't Load DLL";
		}
	}
	else
	{
		errorLog += "File Not specified ";
	}

	if (bValidDLL==FALSE)
	{
		MessageBox(errorLog,"Error to load Dispatcher DLL");
		m_AlgorithmID=-1; // no valid algorithm
		stateDescription+="Manual Dispatching";
		stateDescription+="\n";
		stateDescription+="Human Intelligence";
	}
	else
	{
		stateDescription += m_DLLFileName;
		stateDescription +="\n";
		stateDescription += m_AlgorithmName;
	}
	SetDlgItemText(IDC_DISPATCH_STATE,stateDescription);
}

int CUserConfiguration::SelectDispatchingAlgorithm(char info[][50])
{
	CDlgSelectAlgorithm dlgsel;
	
	dlgsel.SetInfo(info);
	if (dlgsel.DoModal()==IDOK)
	{
		m_AlgorithmID=dlgsel.mSelected;
		return 1;
	}
	else
		return -1;
	
}


void CUserConfiguration::OnButtonManualDispatch() 
{
	CString DLLDescription;
	m_AlgorithmID=-1;
	bManual=TRUE;

	DLLDescription+= "Manual Dispatch\n";
	DLLDescription+= "Human Intelligence";

	SetDlgItemText(IDC_DISPATCH_STATE,DLLDescription);
}

/*
void CUserConfiguration::SetDLLInfo(BOOL m_bDispatcherAttached, CString DispatcherDLLFilePath,
			CString DispatcherDLLFileName, int algorithmID,CString algorithmName)
{
	bManual=!m_bDispatcherAttached;
	m_DLLFilePath=DispatcherDLLFilePath;
	m_DLLFileName=DispatcherDLLFileName;
	m_AlgorithmID=algorithmID;
	m_AlgorithmName=algorithmName;
}
*/

int CUserConfiguration::GetDwellType()
{
	return m_iDwellType;
}

void CUserConfiguration::SetDwellType(int iDwellType)
{
	m_iDwellType = iDwellType;
}

void CUserConfiguration::SetReopen(BOOL bReopen)
{
	m_bReopen = bReopen;
}

BOOL CUserConfiguration::GetReopen()
{
	return m_bReopen;
}

void CUserConfiguration::SetCOGSSpy(BOOL bSpy)
{
	m_bSpy = bSpy;
}

BOOL CUserConfiguration::GetCOGSSpy()
{
	return m_bSpy;
}

void CUserConfiguration::SetHoistwayManager(BOOL bHM)
{
	m_bHM=bHM;
}

BOOL CUserConfiguration::GetHoistwayManager()
{
	return m_bHM;
}

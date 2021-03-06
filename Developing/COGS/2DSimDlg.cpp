// 2DSimDlg.cpp : implementation file
//

#include "stdafx.h"
#include "2DSim.h"
#include "2DSimDlg.h"

#include "UserConfiguration.h"
#include "HideUnhide.h"
#include "JumpToTime.h"
#include "ShowLoadDlg.h"
#include "DlgSelectAlgorithm.h"
#include <math.h>
#include "..\HoistwayPlanner\HoistwayPlannerDefs.h"
#include "..\HoistwayPlanner\HoistwayPlanSelection.h"
#include "..\COGS\2DInterfaceData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char DEBUGMsg[500];
#define DEBUGLevel 0
#define DEBUGWrite(x) if ((x) <= DEBUGLevel) MessageBox(DEBUGMsg)

//#define SHOW_COLLISION_WARNING
#define SHOW_REJECTION_WARNING
//#define SHOW_REJECTION_WARNING_DIALOG

// shin050713
CarCommand CarCommandFromHPE[MAXCAR];
BOOL UpdatedCarCommand[MAXCAR];
CHAR AssignmentFromDispatcher;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_AboutDescription;
	CString	m_Version;
	CString	m_VersionInfo;
	CString	m_ExecutableType;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_ExecutableType = _T("");
	//}}AFX_DATA_INIT
	m_Version = _T("2DSim Version ");
	m_Version += ENGINEVERSION;
	m_VersionInfo=VERSION_SUMMARY;
	m_AboutDescription=VERSION_DETAIL;
	m_ExecutableType=EXECUTABLE_TYPE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_ABOUT_DESCRIPTION, m_AboutDescription);
	DDX_Text(pDX, IDC_VERSION, m_Version);
	DDX_Text(pDX, IDC_VERSION_INFO, m_VersionInfo);
	DDX_Text(pDX, IDC_EXECUTABLE_TYPE, m_ExecutableType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMy2DSimDlg dialog

CMy2DSimDlg::CMy2DSimDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMy2DSimDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMy2DSimDlg)
	m_CurrentTime = _T("Current Time: 0.00");
	m_CurrentPassenger = _T("");
	m_0 = _T("");
	m_1 = _T("");
	m_2 = _T("");
	m_3 = _T("");
	m_4 = _T("");
	m_5 = _T("");
	m_6 = _T("");
	m_7 = _T("");
	m_debug = _T("");
	m_strClosestMargin = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	FileMemo.Empty();
	// DLL related initialization
	clearCommunicationContents();
	m_bDispatcherAttached=FALSE;

}

void CMy2DSimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMy2DSimDlg)
	DDX_Control(pDX, IDC_LIST_WARNING, m_ListWarning);
	DDX_Text(pDX, IDC_STATIC_CURRENT_TIME, m_CurrentTime);
	DDX_Text(pDX, IDC_STATIC_PASSENGER_INFO, m_CurrentPassenger);
	DDX_Text(pDX, IDC_EDIT00, m_0);
	DDX_Text(pDX, IDC_EDIT01, m_1);
	DDX_Text(pDX, IDC_EDIT02, m_2);
	DDX_Text(pDX, IDC_EDIT03, m_3);
	DDX_Text(pDX, IDC_EDIT04, m_4);
	DDX_Text(pDX, IDC_EDIT05, m_5);
	DDX_Text(pDX, IDC_EDIT06, m_6);
	DDX_Text(pDX, IDC_EDIT07, m_7);
	DDX_Text(pDX, IDC_STATIC_DEBUG, m_debug);
	DDX_Text(pDX, IDC_STATIC_CLOSEST_MARGIN, m_strClosestMargin);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMy2DSimDlg, CDialog)
	//{{AFX_MSG_MAP(CMy2DSimDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_TRAFFIC, OnButtonOpenTraffic)
	ON_BN_CLICKED(IDC_BUTTON_USER_CONFIGURATION, OnButtonUserConfiguration)
	ON_BN_CLICKED(IDC_BUTTON_NEXT_EVENT, OnButtonNextEvent)
	ON_BN_CLICKED(IDC_BUTTON_NEXT_PASSENGER, OnButtonNextPassenger)
	ON_BN_CLICKED(IDC_BUTTON_ADD_MOVE, OnButtonAddMove)
	ON_BN_CLICKED(IDC_BUTTON_JUMP, OnButtonJump)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSaveTrafficProfile)
	ON_BN_CLICKED(IDC_BUTTON_NEXT_NEW_HCALL, OnButtonNextNewHcall)
	ON_BN_CLICKED(IDC_BUTTON_CLEARWARNING, OnButtonClearwarning)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnButtonLoad)
	ON_COMMAND(IDC_OPEN_BATCH, OnOpenBatch)
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(BN_CLICKED,IDC_BUTTON_CAR0,IDC_BUTTON_CAR0+MAXCAR-1,OnCarAssign)
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////
/*			 CMy2DSimDlg message handlers					*/
//////////////////////////////////////////////////////////////

BOOL CMy2DSimDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	// Load key accelerators
	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR)); 

	// Create DC objects
	PrepareDC();
	// Load bitmap for car call, up call and down call
	LoadBitmap();
	//Initialize variables
	InitVariables();
	InitDrawBuilding();
	//OpenFiles();

	DrawBuilding();

	//Draw Elevator: Create
	DrawElevator();
	OnButtonClearwarning(); // shs
	m_pShowLoadDlg = new CShowLoadDlg(this);
	if(m_pShowLoadDlg != NULL)
	{
		BOOL ret = m_pShowLoadDlg->Create(IDD_LOAD,this);
		if(!ret)   //Create failed.
			AfxMessageBox("Error creating Dialog");
		//m_bIsVisibleShowLoad = m_pShowLoadDlg->ShowWindow(SW_SHOW);
		m_bIsVisibleShowLoad = m_pShowLoadDlg->ShowWindow(SW_HIDE);
	}
	else
		AfxMessageBox("Error Creating Dialog Object");
	
	::GetCurrentDirectory(99,ExecutionDir);
	int str_size;
	str_size=strlen(ExecutionDir);
	if (ExecutionDir[str_size-1]!='\\')
	{
		strcat(ExecutionDir,"\\");
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMy2DSimDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMy2DSimDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		
		UpdateDisplay();
		//DrawHCAssignment(0,0,0);
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMy2DSimDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMy2DSimDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnTimer(nIDEvent);
}

#include "2DFileIO.h"

void CMy2DSimDlg::OnButtonOpenTraffic() 
{
	CString filt = "2D Profile(*.2dp)|*.2dp|pro File (*.pro)|*.pro||";
	CString fileName ;
//	CString strCaption;
	unsigned int p=0, m=0;//Passenger Counter
	int i, iLength;
	int retval; 
	
	CFileDialog *fileDlg;
	C2DFileIO f2d;
	CString file_memo;

	int paxCount,moveCount;

	fileDlg = new CFileDialog( TRUE, "*.2dp", "*.2dp", NULL, filt, this ) ;
	if ( fileDlg != NULL )
	{
		fileDlg->m_ofn.Flags |= OFN_FILEMUSTEXIST ;
		fileDlg->m_ofn.lpstrTitle = "Traffic Profile" ;
		
		if ( fileDlg->DoModal() == IDOK )
		{
			fileName = fileDlg->GetPathName();
			strcpy( profileFileName, fileName );
			iLength = fileName.GetLength();
			fileName.Delete(iLength-4, 4);
			m_strTrafficProfileName = fileName;

			InitPassenger();
			retval = f2d.Load2DPFile(profileFileName,&FileMemo,&Building,&Param,
				Pax, Move,&paxCount,&moveCount);
			if (retval==-1) // 	Only traffic profile data
			{
				MessageBox("Only Traffic Data are read. Ensure Simulation Environment Setting");
				Building.nHoistways=(Building.nCars+1)/2;
			}
			else if (retval==0) // Success
			{
				//MessageBox("File Open Success");
			}
			else if (retval==1) // File open error
			{
				MessageBox("File Open Error");
			}
			else if (retval==3) // File corruption
			{
				MessageBox("File corrupted. Ensure file format is right");
			}

			n_pax=paxCount;
			n_move=moveCount;
			n_hwy=Building.nHoistways;
			n_car=Building.nCars;
			n_floor=Building.nTotalFloors;

			ProfileCarsForCheck=Building.nCars;

			for (i=0;i<MAXCAR;i++)
			{
				Car[i].floor=Param.initial_position[i];
			}

			RedisplayCaption();  // Display Caption

			InitializeEngine();  // initialize simulation engine
			m_iDwellType = 0; //Default selection is current CAGS behavior (0)
			
			m_trafficProfileOpened = TRUE;
			m_bSimulationEnd = FALSE;
			m_bAssigned = FALSE;
			m_bNeedAssign = FALSE;
			m_bReassigned = FALSE;

			InitDrawBuilding();
			UpdateCars(); 
			UpdateDisplay();

			UpdateData(FALSE);
		}
	}
	
	Invalidate();
	OnButtonClearwarning(); 

	delete [] fileDlg ;
}


void CMy2DSimDlg::InitPassenger()
{
	int i;

    for(i=0; i<MAXPAX; i++) {
        Pax[i].t_arrival = 0.0;
        Pax[i].f_origin = 0;
        Pax[i].f_dest = 0;
        Pax[i].assignment = -1;
        Pax[i].t_endwait_CP = 0.0;
		Pax[i].t_endwait_AP = 0.0;
        Pax[i].t_departure = 0.0;
        Pax[i].q_next = 0;
		Pax[i].rejection = 0;
    }

}

void CMy2DSimDlg::OnButtonUserConfiguration() 
{
	CUserConfiguration dlgConfig;
	int h, c, i;
	//int n2D_hwy,iHoistway;
	BOOL bHMChanged = TRUE;//shin051004 Link DLLs again if HM configuration is changed.
	
	dlgConfig.SetReassignFlag(m_bReassign);
	dlgConfig.SetParameter(&Param);
	dlgConfig.SetBuidlingParameter(&Building);
	dlgConfig.SetAdjacentLimit(Param.AdjacentLimit);
	dlgConfig.SetMessageFlag(m_bMsgDisable);
	dlgConfig.SetMotionKind(m_bParamHard);
	dlgConfig.SetDwellType(m_iDwellType);
	dlgConfig.SetReopen(Param.bReopenEnabled);
//	dlgConfig.SetDLLInfo(m_bDispatcherAttached,m_DispatcherDLLFilePath,
//		m_DispatcherDLLFileName,m_AlgorithmID,m_AlgorithmName);
	// Amount of information
	dlgConfig.m_NULL = m_violationHCD & VIOLATION_NULL ? TRUE : FALSE;
	dlgConfig.m_H = m_violationHCD & VIOLATION_H ? TRUE : FALSE;
	dlgConfig.m_C = m_violationHCD & VIOLATION_C ? TRUE : FALSE;
	dlgConfig.m_D = m_violationHCD & VIOLATION_D ? TRUE : FALSE;
	dlgConfig.m_HC = m_violationHCD & VIOLATION_HC ? TRUE : FALSE;
	dlgConfig.m_HD = m_violationHCD & VIOLATION_HD ? TRUE : FALSE;
	dlgConfig.m_CD = m_violationHCD & VIOLATION_CD ? TRUE : FALSE;
	dlgConfig.m_HCD = m_violationHCD & VIOLATION_HCD ? TRUE : FALSE;
	dlgConfig.SetCOGSSpy(m_bSpy);
	dlgConfig.SetHoistwayManager(m_bHM);
	if ( dlgConfig.DoModal() == IDOK )
	{
		Param.bDwellTimeExtendACS = dlgConfig.m_bDwellTimeExtendACS;
		Param.bRunDelayACS = dlgConfig.m_bRunDelayACS;
		Param.bReducedProfile = dlgConfig.m_bReducedProfile;
		Param.capacity=dlgConfig.m_capacity;
		Param.duty=dlgConfig.duty;
		Param.dutyID=dlgConfig.dutyID;
		Param.door_open=dlgConfig.m_openingTime;
		Param.door_close=dlgConfig.m_closingTime;
		//shin 040730
		Param.MaxJerk = dlgConfig.m_jerk;
		Param.MaxAcceleration = dlgConfig.m_acceleration;
		Param.MaxVelocity = dlgConfig.m_velocity;
		Param.StartDelay = dlgConfig.m_startDelay;
		Param.StopDelay = dlgConfig.m_stopDelay;
		Param.MinSeparationDist = dlgConfig.m_minimumSeparationDistance;
		Param.AdjacentLimit=dlgConfig.m_AdjacentLimit;

		Param.f_offset = dlgConfig.m_fOffset;
		m_bMsgDisable = dlgConfig.m_msgDisable;
		
		m_bReassign = dlgConfig.m_bReassign;
		Param.AdjacentLimit = dlgConfig.m_AdjacentLimit;
		SetBuildingParameter(&(dlgConfig.Building));
		// effective to global hoistway/car configuration(display,engine...)
		n_hwy=Building.nHoistways;
		n_car=Building.nCars;
		n_floor=Building.nTotalFloors;

		//shin 040717 Above replaced by below function
		SetHoistway(Building.nCars, Building.nHoistways);
		for (h=0, c=0; h < n_hwy; h++) {
			//Hwy[h].i_car = c;
			for (i = 0; i < Hwy[h].n_car_in_hwy; i++)
			{
				if(i==0)
					Car[c].lowerOrUpper = LOWER_CAR;
				else
					Car[c].lowerOrUpper = UPPER_CAR;
				Car[c++].i_hwy = h;
			}
		}
		// Amount of information
		if(dlgConfig.m_NULL)
			m_violationHCD |= VIOLATION_NULL;
		else
			m_violationHCD &= ~VIOLATION_NULL;
		if(dlgConfig.m_H)
			m_violationHCD |= VIOLATION_H;
		else
			m_violationHCD &= ~VIOLATION_H;
		if(dlgConfig.m_C)
			m_violationHCD |= VIOLATION_C;
		else
			m_violationHCD &= ~VIOLATION_C;
		if(dlgConfig.m_D)
			m_violationHCD |= VIOLATION_D;
		else
			m_violationHCD &= ~VIOLATION_D;
		if(dlgConfig.m_HC)
			m_violationHCD |= VIOLATION_HC;
		else
			m_violationHCD &= ~VIOLATION_HC;
		if(dlgConfig.m_HD)
			m_violationHCD |= VIOLATION_HD;
		else
			m_violationHCD &= ~VIOLATION_HD;
		if(dlgConfig.m_CD)
			m_violationHCD |= VIOLATION_CD;
		else
			m_violationHCD &= ~VIOLATION_CD;
		if(dlgConfig.m_HCD)
			m_violationHCD |= VIOLATION_HCD;
		else
			m_violationHCD &= ~VIOLATION_HCD;
		Param.m_violationHCD = m_violationHCD;
		m_bParamHard = dlgConfig.GetMotionKind();
		m_iDwellType = dlgConfig.GetDwellType();
		Param.bReopenEnabled = dlgConfig.GetReopen();
		m_bSpy = dlgConfig.GetCOGSSpy();
		if(m_bHM != dlgConfig.GetHoistwayManager())
		{
			m_bHM = dlgConfig.GetHoistwayManager();
			bHMChanged = TRUE;
		}

		// Set old dispatcher settings.
		char dispatcherDLLPath[150];
		char dispatcherDLLName[80];
		int bAutomaticDispatching;
		char dispatchingAlgorithmName[80];
		int algorithmID;

		bAutomaticDispatching = Param.bAutomaticDispatching;
		strcpy(dispatcherDLLPath, Param.dispatcherDLLPath);
		strcpy(dispatcherDLLName, Param.dispatcherDLLName);
		algorithmID = Param.algorithmID;
		strcpy(dispatchingAlgorithmName, Param.dispatchingAlgorithmName);
		
		// get dispatcher dll algorithm name,
		if (dlgConfig.bManual==FALSE && dlgConfig.m_AlgorithmID>=0) // load algorithm success
		{
			Param.bAutomaticDispatching=TRUE;
			strcpy(Param.dispatcherDLLPath,dlgConfig.m_DLLFilePath);
			strcpy(Param.dispatcherDLLName,dlgConfig.m_DLLFileName);
			Param.algorithmID=dlgConfig.m_AlgorithmID;
			strcpy(Param.dispatchingAlgorithmName,dlgConfig.m_AlgorithmName);
		}
		else
		{
			Param.bAutomaticDispatching=FALSE;
		}

		if((bAutomaticDispatching != Param.bAutomaticDispatching) ||
			((strcmp(dispatcherDLLPath, Param.dispatcherDLLPath) != 0) ||
			(strcmp(dispatcherDLLName, Param.dispatcherDLLName) != 0) ||
			(algorithmID != Param.algorithmID) ||
			(strcmp(dispatchingAlgorithmName, Param.dispatchingAlgorithmName) != 0))
			//&& (Param.bAutomaticDispatching == TRUE) 
			|| bHMChanged
			)
		{
			LinkDLLs(); // Link Dispatcher, HP and HPE DLLs
			InitializeDLLs();
		}
		RedisplayCaption();

		UpdateCars();
		Invalidate(); 
		ProfileCheckPassed=FALSE;
	}
}

void CMy2DSimDlg::OnButtonNextEvent() 
{
	bEnableCollisionMessageBox=TRUE;
	ProcessNextEvent();
}


//////////////////////////////////////////////////////////////
/*			     Simulator Engine							*/
//////////////////////////////////////////////////////////////

void CMy2DSimDlg::InitVariables()
{
	int h, c;
	time_t ltime;
	int i;
	C2DFileIO fio;

	fio.InitBuilding(&Building);
	fio.InitParameters(&Param);
	n_move = 0;

	//message dialog display disable
	m_bMsgDisable = FALSE;

	//To check the violation for n pieces of information
	Param.numOfInformation = 3;//assignment, direction, upper/low
	
	m_bReassign = FALSE;
	
	m_trafficProfileOpened = FALSE;
	m_bSimulationEnd = FALSE;
	
	n_hwy = Building.nHoistways;
	n_floor = Building.nTotalFloors;
	n_car = Building.nCars;
	// Initialize hoistway structure
	SetHoistway(Building.nCars, n_hwy);
	
	for (h=0, c=0; h < n_hwy; h++) {
		for (i = 0; i < Hwy[h].n_car_in_hwy; i++)
		{
			if(i==0)
				Car[c].lowerOrUpper = LOWER_CAR;
			else
				Car[c].lowerOrUpper = UPPER_CAR;
			Car[c++].i_hwy = h;
		}
	}

	for(c = 0; c < MAXCAR; c++)
	{
		m_bCar[c] = 0;
		Car[c].floor = Param.initial_position[c];
		Car[c].dir = NO_DIR;
		Car[c].oper = IDLE;
		Car[c].t_start_oper=0.0; // shs0826
		Car[c].t_end_oper = 0.0;
	}

	m_bAssigned = FALSE;//Check whether assignment is done or not.
	m_bNeedAssign = FALSE;
	
	//log for some messages and errors
	f_ErrorLog = fopen("Error.log", "a");
	fprintf(f_ErrorLog, "\n\n////////////////////////////////////////////////////\n");//insert date and time
	fprintf(f_ErrorLog, "Engine version: ");
	fprintf(f_ErrorLog, ENGINEVERSION);
	time( &ltime );
	fprintf(f_ErrorLog, ", time: %s\n", ctime( &ltime ) );

	for(c = 0; c < MAXCAR; c++)
	{
		m_nMaxPassenger[c] = 0;
	}
	m_bIsVisibleShowLoad = FALSE;
	m_violationHCD = VIOLATION_H|VIOLATION_HC; //show H and HC violation(1 POI and 2POI)
	Param.m_violationHCD = m_violationHCD;
	m_bParamHard = FALSE;
	m_iDwellType = 0; //Default selection is current CAGS behavior (0)
	
	m_bSpy = FALSE;

	m_bDispatcherAttached = FALSE;
	hinstLib = NULL;
	hinstHPLib=NULL;
	
	// shs0708 Only Hoistway Manager Attached is managed because HP and HE cannot be run separatedly 
 
	m_bHM=TRUE; // try to use HM
	m_bHMAttached=FALSE;  // HM related dll is attached ?

	NumberOfTrafficFile = 0;
	IndexOfTrafficFile=0;
}  // InitVariables


void CMy2DSimDlg::InitializeEngine() {
    int c, h, i, f;
	double margin;

// Initialization of engine state
	Sys.t_nextEvent = 0.0;
	Sys.y_nextEvent = -1;
	Sys.i_nextEvent = -1;
	i_pax = 0;
	i_pax_recent = 0;
	i_move = 0;
	n_carlog = 0;
// Initialize special events
	HandleSpecialEvent(-1);	// initialization call: set n_specialEvent and i_specialEvent
	
	SetHoistway(Building.nCars, Building.nHoistways);

	//shin 040817
	//Set lowerOrUpper(Car Position) and hoistway index for each car
	for (h=0, c=0; h < n_hwy; h++) {
		//Hwy[h].i_car = c;
		for (i = 0; i < Hwy[h].n_car_in_hwy; i++)
		{
			if(Hwy[h].n_car_in_hwy==1) {
				Car[c].lowerOrUpper = LOWER_CAR;
				Car[c].range_bottom = 0;
				Car[c].range_top = n_floor - 1;
			}
			else if(i==0) {
				Car[c].lowerOrUpper = LOWER_CAR;
				Car[c].range_bottom = 0;
				Car[c].range_top = n_floor - 2;
			}
			else {
				Car[c].lowerOrUpper = UPPER_CAR;
				Car[c].range_bottom = 1;
				Car[c].range_top = n_floor - 1;
			}
			Car[c].max_decel_bottom = FloorToPos(Car[c].range_bottom);
			Car[c].max_decel_top = FloorToPos(Car[c].range_top);
			Car[c++].i_hwy = h;
		}
	}
	
	// Initialize Car structure
	for (c = 0; c < MAXCAR; c++) {
		Car[c].oper = IDLE;
		Car[c].t_start_oper = 0.0;
		Car[c].t_end_oper = 0.0;
		Car[c].floor = Param.initial_position[c];
		Car[c].origin = Param.initial_position[c]; // Add for timing
		Car[c].target = Car[c].floor;
		Car[c].NCF = Param.initial_position[c];
		Car[c].currentFloor = Param.initial_position[c];
		Car[c].currentPos = FloorToPos(Car[c].floor);
		Car[c].dir = NO_DIR;
		Car[c].pending_move = 0;
		Car[c].other_car_waiting = 0;
		Car[c].n_deboard = 0;
		Car[c].t_decel = 0.0;
		Car[c].t_actual_position = 0.0;
		Car[c].n_pax_in_car = 0;
		for (f = 0; f < MAXFLOOR; f++) {
			Car[c].n_CCall[f] = 0;
			Car[c].q_CCall[f] = -1;
			Car[c].n_UpCall[f] = 0;
			Car[c].q_UpCall[f] = -1;
			Car[c].z_UpCall[f] = -1;
			Car[c].n_DnCall[f] = 0;
			Car[c].q_DnCall[f] = -1;
			Car[c].z_DnCall[f] = -1;
			Car[c].n_UpStop[f] = 0;
			Car[c].n_DnStop[f] = 0;
		}
		Car[c].mp_n_break = 1;
		Car[c].mp_break[0] = 0.0;
		Car[c].mp_pos[0] = FloorToPos(Param.initial_position[c]);
		Car[c].mp_speed[0] = 0.0;
		Car[c].mp_accel[0] = 0.0;
		Car[c].mp_jerk[0] = 0.0;
		Car[c].extra_dwell = 0;
		Car[c].i_move = -1;
		Car[c].tRunDelay = 0.0;
	}
	
	// Initialize Floor structure
	for (f = 0; f < MAXFLOOR; f++) {
		Floor[f].n_arrival = 0;
		Floor[f].n_departure = 0;
		Floor[f].length = 0;
		Floor[f].max_length = 0;
		Floor[f].t_break = 0.0;
		Floor[f].length1 = 0.0;
		Floor[f].length2 = 0.0;
	}

	for(c = 0; c < MAXCAR; c++)
	{
		m_nMaxPassenger[c] = 0;
	}
	m_bIsVisibleShowLoad = FALSE;
	m_strClosestMargin = _T("");

	m_minMargin = 100000.0;
	m_minMarginTime = 0.0;
	m_car = 0;
	for (c = 0; c < n_car; c++)
		if (Car[c].lowerOrUpper == UPPER_CAR) {  // there are two cars in the hoistway
			margin = FloorToPos(Car[c].floor) - FloorToPos(Car[c-1].floor);
			if (margin < 0.0)
				fprintf(f_ErrorLog, "InitializeEngine: car %d at %lf (%d); lower car at %lf (%d)\n",
					c, FloorToPos(Car[c].floor), Car[c].floor, FloorToPos(Car[c-1].floor),
					Car[c-1].floor);
			if (margin < m_minMargin) {
				//See the log in the warning list of main screen.
				CString str;
				str.Format("(%lf < %lf) (%lf, %d)",margin ,m_minMargin, m_minMarginTime, Car[c].i_hwy);
				DisplayWarningList(str);
				
				m_minMargin = margin;
				m_car = c;
				m_strClosestMargin.Format("%5.3lf (%5.3lf, %d)",m_minMargin, m_minMarginTime, Car[m_car].i_hwy);
				//Write log for minMargin here!!
			}
		}

	m_violationHCD = Param.m_violationHCD;
	AssignmentFromDispatcher = -1;
	ReadSpecification();
	for (c = 0; c < n_car; c++)
	{
		CarCommandFromHPE[c].motionCommand = STANDING;
		CarCommandFromHPE[c].target = -1;
		CarCommandFromHPE[c].runDelay = 0;
		CarCommandFromHPE[c].runProfile = 0;
		CarCommandFromHPE[c].frontDoorCommand = DOOR_NORMAL;
		CarCommandFromHPE[c].frontDoorDwell = 0;
		CarCommandFromHPE[c].rearDoorCommand = DOOR_NORMAL;
		CarCommandFromHPE[c].rearDoorDwell = 0;

		UpdatedCarCommand[c] = FALSE;
	}
		LinkDLLs(); // Link Dispatcher, HP and HPE DLLs
		InitializeDLLs();
}  // InitializeEngine


// Deboarding has already occurred (the number of deboards is in Car[c].n_deboard)
double CMy2DSimDlg::DoorDwell(int c)
{
	int f, n_board, max_board;

	// There are 3 kinds of dwell time logic
	// 0. current CAGS behavior (default)
	// 1. idealized behavior
	// 2. old COGS logic
	if(m_iDwellType == 2)
		return DwellTimeOld(c);

	if (Car[c].oper != DWELL) {
		fprintf(f_ErrorLog, "DoorDwell: car %d in oper %d at t=%lf\n",
			c, Car[c].oper, Sys.t_nextEvent);
		return 0.0;
	}
	
	f = Car[c].floor;
	
	if (Car[c].dir == 1)
		n_board = Car[c].n_UpCall[f];
	else if (Car[c].dir == -1)
		n_board = Car[c].n_DnCall[f];
	else
		n_board = 0;
//sprintf(DEBUGMsg, "c=%d fl=%d nb=%d dir=%d",
//c, f, n_board, Car[c].dir);
//DEBUGWrite(0);

	max_board = Param.capacity - Car[c].n_pax_in_car;
	if (n_board > max_board)
		n_board = max_board;

	if(m_iDwellType == 1)
		return DwellTime(f, n_board, Car[c].n_deboard);// idealized behavior
	else// if(m_iDwellType == 0)
		return DwellTimeCurrent(f, n_board, Car[c].n_deboard);// current CAGS behavior
}  // DoorDwell


// If car doors are opened and there is a corresponding dwell move for the car at this floor
//   during this time period, then hold doors open until dwell_release
void CMy2DSimDlg::AdjustDoorDwell(int c, double end_of_normal_dwell) {
  if (Car[c].oper == DWELL && Car[c].dwell_floor == Car[c].floor &&
	  Car[c].dwell_initiate <= end_of_normal_dwell + EPSILON &&
	  Car[c].dwell_release + EPSILON >= end_of_normal_dwell) {
	Car[c].t_end_oper = Car[c].dwell_release;
	Car[c].extra_dwell = 1;
  }
}  // AdjustDoorDwell


// Return INVALIDFLOOR if the car is not running (hence does not need to display NCF)
int CMy2DSimDlg::DisplayedNCF(int c) {
  int into_decel;

  if (Car[c].oper != RUNNING && Car[c].oper != FORCED_MOVE && Car[c].oper != MOVE_CAR_RANGE)
  {
	  //fprintf(f_ErrorLog, "DisplayedNCF: car(%d) oper=(%d) at t(%lf)\n",
	  //	  c, Car[c].oper, Sys.t_nextEvent);
	  return INVALIDFLOOR;
  }
  else
	return GetNextCommittableFloor(c, &into_decel, 0);
}  // DisplayedNCF


int CMy2DSimDlg::GetDisplayedPosition(int c)
{
	double t_into_run;
	double j;//Jerk
	double a;//Acceleration
	double v;//Velocity
	double pos;//Position

	if(m_bParamHard)//shin 040914 two kinds of parameter
		return DisplayedPosition(c);
	
	if (Car[c].oper != RUNNING && Car[c].oper != FORCED_MOVE && Car[c].oper != MOVE_CAR_RANGE)
		return Car[c].floor;
	else {
		t_into_run = Sys.t_nextEvent - Car[c].t_run_start;
		if (t_into_run > GetRunTime(c, Car[c].floor, Car[c].target) + EPSILON || t_into_run < -EPSILON)
			fprintf(f_ErrorLog, "GetDisplayedPosition 1: car %d, time into run %lf vs %lf at t=%lf\n",
			c, t_into_run, GetRunTime(c, Car[c].floor, Car[c].target), Sys.t_nextEvent);
		
		GetCurrentMotionState(c, &j, &a, &v, &pos);
		
		return PosToFloor(pos);
	}
}  // GetDisplayedPosition


// This is mainly for logging purposes.
//   stage 1: a car cycle begins at every run and move (e.g. 0->4, 0->5)
//   stage 2: a car cycle beginning without a run (e.g. 0->1)
//   stage 3: after doors are closed (only in the case or normal run)
//   stage 4: after release time     (only in the case of hide/unhide)
void CMy2DSimDlg::LogCarCycle(int c, int stage)
{
	if (stage == 1) {  // car cycle begins with a run (either normal or hide)
		Car[c].t_run_start = Sys.t_nextEvent;
		Car[c].origin = Car[c].floor;
		Car[c].n_pax_run = Car[c].n_pax_in_car;
		if (Car[c].oper == MOVE_CAR_RANGE)
			Car[c].changed_target = 2;
		else
			Car[c].changed_target = 0;
		Car[c].door_cycle_in_car_cycle = 0;
	}
	else if (stage == 2) {  // car cycle doesn't require a run
		Car[c].t_run_start = Sys.t_nextEvent;
		Car[c].origin = Car[c].floor;
		Car[c].n_pax_run = Car[c].n_pax_in_car;
//		Car[c].t_decel = Sys.t_nextEvent;            // AH: commented out 041018
		Car[c].t_run_end = Sys.t_nextEvent;
		if (Car[c].oper == MOVE_CAR_RANGE)
			Car[c].changed_target = 2;
		else
			Car[c].changed_target = 0;
		Car[c].door_cycle_in_car_cycle = 0;
	}
	
	else if (stage == 3) {  // end of car cycle as doors close
		// Store data into car log
		CarLog[n_carlog].car = c;
		if (Car[c].changed_target == 2 || Car[c].changed_target == 4)
		  CarLog[n_carlog].type = 'R';  // car Range run
		else
		  CarLog[n_carlog].type = 'N';  // Normal run
		CarLog[n_carlog].t_run_start = Car[c].t_run_start;
		CarLog[n_carlog].t_decel = Car[c].t_decel;
		CarLog[n_carlog].t_actual_position = Car[c].t_actual_position;
		CarLog[n_carlog].t_run_end = Car[c].t_run_end;
		CarLog[n_carlog].origin = Car[c].origin;
		CarLog[n_carlog].dest = Car[c].floor;
		CarLog[n_carlog].n_pax_run = Car[c].n_pax_run;

		CarLog[n_carlog].door_cycle_in_car_cycle = Car[c].door_cycle_in_car_cycle;
		CarLog[n_carlog].t_door_opened = Car[c].t_door_opened;
		CarLog[n_carlog].n_deboard = Car[c].n_deboard;
		CarLog[n_carlog].n_board = Car[c].n_board;
		CarLog[n_carlog].t_door_dwelled = Car[c].t_door_dwelled;
		CarLog[n_carlog].t_door_closed = Sys.t_nextEvent;
		CarLog[n_carlog].extra_dwell = Car[c].extra_dwell;
		CarLog[n_carlog].changed_target = Car[c].changed_target;
		n_carlog++;
		Car[c].extra_dwell = 0;
		
		Car[c].n_deboard = 0;  // required for UpdateText message
	}
	else {  // stage == 4  (end of a move)
		CarLog[n_carlog].car = c;
		// A hide move means that the car is held for some positive period of time (regardless of origin/dest)
		if (Move[Car[c].i_move].t_release > Move[Car[c].i_move].t_initiation + EPSILON)
			CarLog[n_carlog].type = 'H';
		else
			CarLog[n_carlog].type = 'U';
		
		CarLog[n_carlog].t_run_start = Car[c].t_run_start;
		CarLog[n_carlog].t_decel = Car[c].t_decel;
		CarLog[n_carlog].t_actual_position = Car[c].t_actual_position;
		CarLog[n_carlog].t_run_end = Car[c].t_run_end;
		CarLog[n_carlog].origin = Car[c].origin;
		CarLog[n_carlog].dest = Car[c].floor;
		CarLog[n_carlog].n_pax_run = Car[c].n_pax_run;
		
		CarLog[n_carlog].t_released = Sys.t_nextEvent;
		CarLog[n_carlog].extra_dwell = Car[c].extra_dwell;
		CarLog[n_carlog].changed_target = Car[c].changed_target;
		n_carlog++;
		Car[c].extra_dwell = 0;
		if (Car[c].oper == FORCED_PARKING && Move[Car[c].i_move].t_release >= Car[c].t_decel)
			Car[c].t_decel = Move[Car[c].i_move].t_release;
		if (Car[c].oper == FORCED_PARKING && Move[Car[c].i_move].t_release >= Car[c].t_actual_position)
			Car[c].t_actual_position = Move[Car[c].i_move].t_release;
	}
}  // LogCarCycle


// Scan hall calls and car calls for demand, considers car range, and considers motion limits
//   (specified in lo_up, hi_up, hi_dn, lo_dn) to determine new target and new direction
// Calling cases:
//   1a, 1b [standing_f == -1]: running car -- check for updated target within limited demand range
//   2 [standing_f >= 0 && set_predirection == 0]: car is available to run
//   3 [standing_f >= 0 && set_predirection == 1]: car needs predirection
//   4 [any standing_f]: check car range
// Outputs:
//   When standing_f >= 0:
//     *demand_target < 0, *new_target < 0: no further demand, therefore idle
//     *demand_target < 0, *new_target >= 0: target due to car range
//     *demand_target >= 0, *new_target == *demand_target: normal run
//     *demand_target >= 0, *new_target == standing_f: wait due to car range
//     *demand_target >= 0, o.w.: *new_target due to car range
void CMy2DSimDlg::ScanCalls(int c, int lo_up, int hi_up, int hi_dn, int lo_dn, int standing_f,
    int set_predirection, int *n_call, int *new_target, int *predirection, int *demand_target) {
  int cc_pax_earliest, cc_dir_earliest, hc_pax_earliest, hc_dir_earliest;
  int up_direct, up_indirect, dn_direct, dn_indirect, f;
  int range_target, preferred_dir, predirection_f;

  if (lo_up < 0 && hi_up >= 0 || lo_up >= 0 && hi_up < 0 ||
	  lo_dn < 0 && hi_dn >= 0 || lo_dn >= 0 && hi_dn < 0 ||
	  set_predirection && standing_f < 0)
	fprintf(f_ErrorLog, "ScanCalls 1: c=%d t=%lf %d %d %d %d %d %d\n",
		c, Sys.t_nextEvent, lo_up, hi_up, hi_dn, lo_dn, standing_f, set_predirection);

  *n_call = 0;
  cc_pax_earliest = n_pax + 1;  // high index
  hc_pax_earliest = n_pax + 1;
  cc_dir_earliest = NO_DIR;
  hc_dir_earliest = NO_DIR;
  if (set_predirection)
    predirection_f = -1;

// Look upwards for calls
  up_direct = -1;
  up_indirect = -1;
  for (f = lo_up; f <= hi_up && f >= 0; f++)
  {
	if (Car[c].n_CCall[f]) {
	  if (f != standing_f) {
        *n_call += Car[c].n_CCall[f];
	    if (up_direct == -1)
		  up_direct = f;
	    if (Car[c].q_CCall[f] < cc_pax_earliest) {
		  cc_pax_earliest = Car[c].q_CCall[f];
		  cc_dir_earliest = 1;
		}
	  }
	  else if (set_predirection)
		predirection_f = f;
	}

	if (Car[c].n_UpCall[f]) {
      *n_call += Car[c].n_UpCall[f];
  	  if (up_direct == -1 &&                        // only consider hall calls if car has room
		  (Car[c].n_pax_in_car < Param.capacity || Car[c].n_CCall[f] > 0))
		up_direct = f;

	  if (up_direct >= 0 && Car[c].q_UpCall[f] < hc_pax_earliest) {
		hc_pax_earliest = Car[c].q_UpCall[f];
		hc_dir_earliest = 1;
	  }
	}

	if (Car[c].n_DnCall[f] && f != standing_f) {
      *n_call += Car[c].n_DnCall[f];
	  up_indirect = f;
	  if (Car[c].q_DnCall[f] < hc_pax_earliest) {
		hc_pax_earliest = Car[c].q_DnCall[f];
		hc_dir_earliest = 1;
	  }
	}
  }  // for f

// Look downwards for calls
  dn_direct = -1;
  dn_indirect = -1;
  for (f = hi_dn; f >= lo_dn && f >= 0; f--)
  {

	if (Car[c].n_CCall[f] && f != lo_up) {       // don't double-count car call
	  if (f != standing_f) {
	    *n_call += Car[c].n_CCall[f];
	    if (dn_direct == -1)
		  dn_direct = f;
	    if (Car[c].q_CCall[f] < cc_pax_earliest) {
		  cc_pax_earliest = Car[c].q_CCall[f];
		  cc_dir_earliest = 1;
		}
	  }
	  else if (set_predirection)
		predirection_f = f;
	}

	if (Car[c].n_DnCall[f]) {
      *n_call += Car[c].n_DnCall[f];
  	  if (dn_direct == -1 &&                        // only consider hall calls if car has room
		  (Car[c].n_pax_in_car < Param.capacity || Car[c].n_CCall[f] > 0))
		dn_direct = f;
	  if (dn_direct >= 0 && Car[c].q_DnCall[f] < hc_pax_earliest) {
		hc_pax_earliest = Car[c].q_DnCall[f];
		hc_dir_earliest = 1;
	  }
	}

	if (Car[c].n_UpCall[f] && f != standing_f) {
      *n_call += Car[c].n_UpCall[f];
	  dn_indirect = f;
	  if (Car[c].q_UpCall[f] < hc_pax_earliest) {
		hc_pax_earliest = Car[c].q_UpCall[f];
		hc_dir_earliest = 1;
	  }
	}
  }  // for f

// Look at the calls with respect to car direction
  if (Car[c].dir != NO_DIR &&     // preferred predirection is the current direction
	  Car[c].dir != NO_PREDIRECTION)
	preferred_dir = Car[c].dir;
                                  // for car waking up from 0 or 6
  else if (cc_dir_earliest != 0)  // car call direction takes precedence over hall call direction
	preferred_dir = cc_dir_earliest;
  else
	preferred_dir = hc_dir_earliest;

// From scans, determine the demand target
  *demand_target = -1;
  if (preferred_dir == 1) {
	if (up_direct >= 0)
	  *demand_target = up_direct;
	else if (up_indirect >= 0)
	  *demand_target = up_indirect;
	else if (dn_direct >= 0) {
	  *demand_target = dn_direct;
	  preferred_dir = -1;
	}
	else if (dn_indirect >= 0) {
	  *demand_target = dn_indirect;
	  preferred_dir = -1;
	}
  }
  else if (preferred_dir == -1) {
    if (dn_direct >= 0)
	  *demand_target = dn_direct;
	else if (dn_indirect >= 0)
	  *demand_target = dn_indirect;
	else if (up_direct >= 0) {
	  *demand_target = up_direct;
	  preferred_dir = 1;
	}
	else if (up_indirect >= 0) {
	  *demand_target = up_indirect;
	  preferred_dir = 1;

	}
  }
  if (preferred_dir == NO_DIR && *demand_target >= 0)
	fprintf(f_ErrorLog, "ScanCalls 2: c=%d t=%lf %d %d %d %d\n",
		c, Sys.t_nextEvent, preferred_dir, *demand_target, Car[c].floor, Car[c].target);

// Initialize target to "unrestricted" target (not considering car range)
  if (*demand_target >= 0)
	range_target = *demand_target;         // initialize to demand target
  else {
	range_target = Car[c].target;          // if no demand, stick with existing target
	if (Car[c].target > Car[c].floor)
	  preferred_dir = 1;
	else if (Car[c].target < Car[c].floor)
	  preferred_dir = -1;
	else
	  preferred_dir = NO_DIR;
  }

// Now modify range_target based on car range
  if (range_target > Car[c].range_top)                              // closer end of car range
	range_target = Car[c].range_top;
  else if (range_target < Car[c].range_bottom)
	range_target = Car[c].range_bottom;

// Now modify new_target (the real target) given car motion limits (lo_up, etc.)
  if (preferred_dir == 1 && range_target >= lo_up && lo_up >= 0) {  // normal up target
	if (range_target <= hi_up)
	  *new_target = range_target;
	else
	  *new_target = hi_up;
	*predirection = 1;
  }
  else if (preferred_dir == -1 && range_target <= hi_dn) {          // normal down target
	if (range_target >= lo_dn)
	  *new_target = range_target;
	else
	  *new_target = lo_dn;
	*predirection = -1;
  }
  else if (range_target == standing_f && range_target >= 0) {       // stand at current floor
    *new_target = range_target;
	*predirection = preferred_dir;
	if (standing_f != lo_up || standing_f != hi_dn)
	  fprintf(f_ErrorLog, "ScanCalls 3: %d %d %d c=%d %d %d %d t=%lf\n",
		  standing_f, hi_dn, lo_up, c, preferred_dir, *demand_target, range_target, Sys.t_nextEvent);
  }
  else {
	if (lo_up >= 0 && hi_dn >= 0) {                                 // must move: both directions possible
	  if (hi_dn > lo_up)
	    fprintf(f_ErrorLog, "ScanCalls 4: %d %d c=%d %d %d %d t=%lf\n",
		    hi_dn, lo_up, c, preferred_dir, *demand_target, range_target, Sys.t_nextEvent);

	  if (range_target < lo_dn) {
	    *new_target = lo_dn;
		if (preferred_dir == -1)
	      *predirection = -1;
		else
		  *predirection = NO_PREDIRECTION;
	  }
	  else if (range_target <= hi_dn) {
	    *new_target = range_target;
		if (preferred_dir == -1)
	      *predirection = -1;
		else
		  *predirection = NO_PREDIRECTION;
	  }
	  else if (range_target > hi_up) {
	    *new_target = hi_up;
		if (preferred_dir == 1)
	      *predirection = 1;
		else
		  *predirection = NO_PREDIRECTION;
	  }
	  else if (range_target >= lo_up) {
	    *new_target = lo_up;
		if (preferred_dir == 1)
	      *predirection = 1;
		else
		  *predirection = NO_PREDIRECTION;
	  }
	  else if (lo_up - range_target < range_target - hi_dn) {      // lo_up is closer than hi_dn
	    *new_target = lo_up;
	    *predirection = 0;
	  }
	  else {
	    *new_target = hi_dn;
	    *predirection = 0;
	  }
	}
    else if (lo_up >= 0) {                                         // only up direction possible
	  if (range_target > hi_up)
	    *new_target = lo_up;
	  else if (range_target >= lo_up)
	    *new_target = range_target;
	  else
	    *new_target = lo_up;
	  *predirection = 1;
	}
    else if (hi_dn >= 0) {                                         // only down direction possible
	  if (range_target < lo_dn)
	    *new_target = lo_dn;
	  else if (range_target <= hi_dn)
	    *new_target = range_target;
	  else
	    *new_target = hi_dn;
	  *predirection = -1;
	}
  }

  if (set_predirection) {
	*new_target = predirection_f;
	if (*predirection == NO_DIR)
	  *predirection = NO_PREDIRECTION;
  }

  // Search move call
  if(Car[c].i_move >= 0) 
  {
	  if(Move[Car[c].i_move].t_release > Sys.t_nextEvent)
	  {
		  if(Car[c].floor < Move[Car[c].i_move].destination)
		  {
			  *predirection = UP;
		  }
		  else if(Car[c].floor > Move[Car[c].i_move].destination)
		  {
			  *predirection = DN;
		  }
	  }
  }
  
  // shin050713
  // Using HPE command
  if((m_bHMAttached == TRUE) && (UpdatedCarCommand[c] == TRUE))
  {
	  // shin050929 if target is not valid, HPE commands are ignored.
	  if(IsValidTargetCommand(CarCommandFromHPE[c].target, Car[c].NCF, Car[c].dir, Car[c].oper) == TRUE)
	  {
		  *new_target = CarCommandFromHPE[c].target;
		  *predirection = CarCommandFromHPE[c].pre_dir;
		  if(CarCommandFromHPE[c].target != Car[c].currentFloor)
		  {
			  switch(CarCommandFromHPE[c].frontDoorCommand)
			  {
			  case DOOR_NOT_OPEN:
				  //Run to new Target but target is conditional stop or yield stop so don??t open door
				  //*demand_target = -1;
				  break;
			  case DOOR_NORMAL:
				  //Run to new target and Normal Service
				  *demand_target = CarCommandFromHPE[c].target;
				  break;
			  case DOOR_HURRIED:
				  fprintf(f_ErrorLog, "ScanCalls 5: DOOR_HURRIED c=%d %d %d %d t=%lf\n",
					  c, preferred_dir, *demand_target, range_target, Sys.t_nextEvent);
				  break;
			  case DOOR_CLOSE_CUT:
				  fprintf(f_ErrorLog, "ScanCalls 5: DOOR_CLOSE_CUT c=%d %d %d %d t=%lf\n",
					  c, preferred_dir, *demand_target, range_target, Sys.t_nextEvent);
				  break;
			  case DOOR_DELAYED:
				  fprintf(f_ErrorLog, "ScanCalls 5: DOOR_DELAYED c=%d %d %d %d t=%lf\n",
					  c, preferred_dir, *demand_target, range_target, Sys.t_nextEvent);
				  break;
			  default:
				  break;
			  }
		  }
		  // else if Target == currentFloor
		  else
		  {
			  switch(CarCommandFromHPE[c].frontDoorCommand)
			  {
			  case DOOR_NOT_OPEN:
				  //Hold in CurrentFloor due to precedence condition
				  //*demand_target = -1;
				  break;
			  case DOOR_NORMAL:
				  //Door Reopen
				  *demand_target = CarCommandFromHPE[c].target;
				  break;
			  case DOOR_HURRIED:
				  fprintf(f_ErrorLog, "ScanCalls 5: DOOR_HURRIED c=%d %d %d %d t=%lf\n",
					  c, preferred_dir, *demand_target, range_target, Sys.t_nextEvent);
				  break;
			  case DOOR_CLOSE_CUT:
				  // Get the dwell time from command
				  // Managed in the HandleCarOperation function
				  break;
			  case DOOR_DELAYED:
				  fprintf(f_ErrorLog, "ScanCalls 5: DOOR_DELAYED c=%d %d %d %d t=%lf\n",
					  c, preferred_dir, *demand_target, range_target, Sys.t_nextEvent);
				  break;
			  default:
				  break;
			  }
		  }
	  }
  }
//  sprintf(DEBUGMsg, "ScanCalls Z: car %d [%d] (%d,%d %d,%d) Ud=%d Ui=%d Dd=%d Di=%d -> target=%d (%d %d) dir=%d t=%6.4lf",
//	  c, set_predirection, USERFLOOR(lo_up), USERFLOOR(hi_up), USERFLOOR(hi_dn), USERFLOOR(lo_dn),
//		USERFLOOR(up_direct), USERFLOOR(up_indirect), USERFLOOR(dn_direct),
//	  USERFLOOR(dn_indirect), USERFLOOR(*new_target), USERFLOOR(range_target),
//	  USERFLOOR(*demand_target), *predirection, Sys.t_nextEvent);
//DEBUGWrite(0);
}  // ScanCalls


// Given car index (c) and a relativity code (relativity), this routine returns the relativity
// status (status) where the relativity codes are: 1 (other car is strictly above c), 2 (other
// car is at same floor as or above c), 3 (other car is at same floor as or below c), 4 (other
// car is strictly below c); status codes are: 1 (relativity condition is already satisfied),
// 2 (relativity condition will be satisfied sometime during other car's current run), 3 (relativity
// condition will not be satisfied in other car's current run (or other car is not running)).
// If the status is 2 (condition will be satisfied during the run), then time_to_satisfy is set to
// the simulation time when the condition will be satisfied (assuming the current car remains
// stationary and the other car's target is not updated).
void CMy2DSimDlg::MoveRelativity(int c, int relativity, int *status, double *time_to_satisfy) {
  int floor_c, other_c, actual_relativity;
  double pos_c, pos_other_c, dummy_j, dummy_a, dummy_v, traversal_time;

  if (c >= 2*(Building.nCars - Building.nHoistways))
	fprintf(f_ErrorLog, "MoveRelativity 1: invalid car index %d for condition-based move\n", c);
  else if (Car[c].oper == RUNNING || Car[c].oper == FORCED_MOVE || Car[c].oper == MOVE_CAR_RANGE)
	fprintf(f_ErrorLog, "MoveRelativity 2: invalid car oper %d for car %d\n", Car[c].oper, c);

  else {
// Find location of car
	floor_c = Car[c].floor;

// Find location of the other car
	if (c % 2)
	  other_c = c - 1;
	else
	  other_c = c + 1;

	if (Car[other_c].oper != RUNNING &&    // other car is stationary
	    Car[other_c].oper != FORCED_MOVE &&
		Car[other_c].oper != MOVE_CAR_RANGE) {
	  if (Car[other_c].floor > floor_c)
		actual_relativity = 1;             // other car is above
	  else if (Car[other_c].floor == floor_c)
		actual_relativity = 2;             // both cars at same floor
	  else
		actual_relativity = 4;             // other car is below
	}
	else {                                 // other car is moving -- get motion state
	  GetCurrentMotionState(other_c, &dummy_j, &dummy_a, &dummy_v, &pos_other_c);
	  pos_c = Building.floorPosition[floor_c];
	  if (pos_other_c > pos_c)             // other car is above
		actual_relativity = 1;
	  else if (pos_other_c == pos_c)       // both cars at same place -- this should not happen
		actual_relativity = 2;
	  else                                 // other car is below
		actual_relativity = 4;
	}

// See if relativity is already satisfied
	if (actual_relativity == 1 && (relativity == 1 || relativity == 2) ||
		actual_relativity == 2 && (relativity == 2 || relativity == 3) ||
	    actual_relativity == 4 && (relativity == 3 || relativity == 4)) {
	  *status = 1;
	  return;
	}

	*status = 3;
// So, other car does not meet relativity -- now check if its target meets relativity
	if (Car[other_c].oper == RUNNING || Car[other_c].oper == FORCED_MOVE ||
		Car[other_c].oper == MOVE_CAR_RANGE) {
	  if (Car[other_c].target > floor_c)
		actual_relativity = 1;  // other car's target is above
	  else if (Car[other_c].target == floor_c)
		actual_relativity = 2;  // target is the same floor
	  else
		actual_relativity = 4;  // other car's target is below

	  if (actual_relativity == 1 && (relativity == 1 || relativity == 2) ||
		  actual_relativity == 2 && (relativity == 2 || relativity == 3) ||
	      actual_relativity == 4 && (relativity == 3 || relativity == 4))
	    *status = 2;
	}

	if (*status == 3)  // target of other car does not meet condition, so it can't during this run
	  return;  

// Now find the point in time during the run of the other car when the condition will be met
    traversal_time = GetTraversalTime(other_c,
		fabs(pos_c - Building.floorPosition[Car[other_c].origin]));
	*time_to_satisfy = Car[other_c].t_run_start + traversal_time;
	if (traversal_time < 0.0 || *time_to_satisfy + EPSILON < Sys.t_nextEvent)
	  fprintf(f_ErrorLog, "MoveRelativity 3: invalid time to satisfy %lf for car %d (%d %lf) \
with car %d (%d->%d %lf) t=%lf %lf %lf (%d %d)\n",
	      *time_to_satisfy, c, floor_c, pos_c, other_c, Car[other_c].origin, Car[other_c].target,
		  pos_other_c, Sys.t_nextEvent, traversal_time, Car[other_c].t_run_start, relativity,
		  Car[c].i_move);
  }
}  // MoveRelativity


// This is triggered either by a new passenger arrival, a new move, the complete closing
// of doors, or end of parking.  Car operation, target and direction are set.
//   HandleNewPassenger (endoper == 0)
//   HandleCarOperation: end of door closing (endoper == 1)
//   HandleCarOperation: end of parking (endoper == 1)
//   HandleCarOperation: end of run (RUNNING or MOVE_CAR_RANGE) with no demand at floor (endoper == 1)
//   ProcessEvent: new move on an idle car (endoper == 0)
//   ProcessEvent: new move on a parked car (endoper == 0)
//   SetCarRange (endoper == 0)
void CMy2DSimDlg::UpdateTarget(int c, int endoper)
{
	int f, f_commit, into_decel, new_target, predirection, demand_target, n_call, index,
		scan_for_target, relativity_status, run_to_new_target,
		other_c;
	double dist, time_to_satisfy, AP_time;
	
	f = Car[c].floor;
	sprintf(DEBUGMsg, "UpdateTarget 0: car %d op=%d f=%d", c, Car[c].oper, USERFLOOR(f));
	DEBUGWrite(3);
	

	if ((Car[c].oper == RUNNING || Car[c].oper == MOVE_CAR_RANGE) &&   // running case
		endoper == 0)
	{
		f_commit = GetNextCommittableFloor(c, &into_decel, 1);
		
		/*
		if (into_decel)                     // cannot change target
			new_target = Car[c].target;
		else if (Car[c].dir > 0)            // up direction
			ScanCalls(c, f_commit, n_floor-1, -1, -1, -1, 0,
			    &n_call, &new_target, &predirection, &demand_target);  // ScanCalls case 1a
		else                                // down direction
			ScanCalls(c, -1, -1, f_commit, 0, -1, 0,
			    &n_call, &new_target, &predirection, &demand_target);  // ScanCalls case 1b
		*/

		// shin050824tmp
		if (Car[c].dir == UP)            // up direction
			ScanCalls(c, f_commit, n_floor-1, -1, -1, -1, 0,
			    &n_call, &new_target, &predirection, &demand_target);  // ScanCalls case 1a
		else if (Car[c].dir == DN)                               // down direction
			ScanCalls(c, -1, -1, f_commit, 0, -1, 0,
			    &n_call, &new_target, &predirection, &demand_target);  // ScanCalls case 1b
		else
		{
			new_target = Car[c].target;
			if(UpdatedCarCommand[c] == TRUE)
				predirection = CarCommandFromHPE[c].pre_dir;
			else
				predirection = Car[c].dir;
		}
		if (into_decel)                     // cannot change target
			new_target = Car[c].target;
		Car[c].dir = predirection;
		// shin050824tmp

		if (new_target != Car[c].target) {  // new target!
			Car[c].target = new_target;
			if (new_target == demand_target) {
				Car[c].oper = RUNNING;
				if (Car[c].changed_target == 0)
					Car[c].changed_target = 1;  // regular target update
				else if (Car[c].changed_target == 2 || Car[c].changed_target == 4)
					Car[c].changed_target = 3;  // changed from moving car range to running
			}
			else {
				Car[c].oper = MOVE_CAR_RANGE;
				if (Car[c].changed_target == 0 || Car[c].changed_target == 1 ||
					Car[c].changed_target == 3)
				Car[c].changed_target = 4;      // car run changed from running to moving car range
			}

			if(m_bParamHard)
			{
				Car[c].t_decel = Car[c].t_run_start + HardCommitTime(f, new_target);
				Car[c].t_actual_position = Car[c].t_run_start + HardActualPositionTime(f, new_target);
				Car[c].t_end_oper = Car[c].t_run_start + HardRunTime(f, new_target);
				if (Car[c].t_end_oper + EPSILON < Sys.t_nextEvent)
					fprintf(f_ErrorLog, "UpdateTarget 1: %d %d %d end of run %lf at t=%lf\n",
					c, f, new_target, Car[c].t_end_oper, Sys.t_nextEvent);
				if (Car[c].t_decel + EPSILON < Sys.t_nextEvent)
					fprintf(f_ErrorLog, "UpdateTarget 2: %d %d %d commitment %lf (%lf %lf) at t=%lf\n",
						c, f, new_target, Car[c].t_decel, Car[c].t_run_start, Car[c].t_end_oper,
					Sys.t_nextEvent);
			}
			else
			{
				dist = fabs(FloorToPos(Car[c].target) - FloorToPos(Car[c].floor));
				GenerateFullProfile(c, dist);
				Car[c].t_decel = Car[c].t_run_start + GetCommittableTime(c, f, new_target);
				AP_time = GetActualPositionTime(c, f, new_target);
				if (AP_time > EPSILON)
					Car[c].t_actual_position = Car[c].t_run_start + AP_time;
				index = GetBreakPointTableIndex(c, Sys.t_nextEvent);
				if(index < Car[c].mp_n_break-1 && index >= -1)
					Car[c].t_end_oper = Car[c].mp_break[index+1];
				else
				{
					fprintf(f_ErrorLog, "UpdateTarget 3: %d:%d->%d end of run %lf at t=%lf index(%d)\n",
						c, f, new_target, Car[c].t_end_oper, Sys.t_nextEvent, index);
					Car[c].t_end_oper = Car[c].mp_break[1];
				}
				if (Car[c].t_end_oper + EPSILON < Sys.t_nextEvent)
					fprintf(f_ErrorLog, "UpdateTarget 4: %d %d %d end of run %lf at t=%lf\n",
					c, f, new_target, Car[c].t_end_oper, Sys.t_nextEvent);
				if (Car[c].t_decel + EPSILON < Sys.t_nextEvent)
					fprintf(f_ErrorLog, "UpdateTarget 5: %d %d %d commitment %lf (%lf %lf) at t=%lf\n",
					c, f, new_target, Car[c].t_decel, Car[c].t_run_start, Car[c].t_end_oper,
					Sys.t_nextEvent);
			}
		}
	}  // running case
	// standing case
	else if (//Car[c].pending_move != 2 &&
		Car[c].oper == IDLE ||						// idle car ready to go
		Car[c].oper == WAIT_CAR_RANGE ||			// car can't serve demand due to car range
		Car[c].oper == RUNNING ||                   // end of car run with no door operation
		Car[c].oper == MOVE_CAR_RANGE ||            // end of car range move
		(Car[c].oper == FORCED_PARKING ||           // parked car
		Car[c].oper == CLOSING) && endoper == 1)    // end of car cycle
	{

		if (Car[c].pending_move == 2)
			Car[c].pending_move = 0;   // reset so move is picked up first, followed by re-do of car
		// operation (since the end_oper time is still the same)
	
		else {
		// Log data at the end of a car cycle
		    if (Car[c].oper == CLOSING || Car[c].oper == RUNNING || Car[c].oper == MOVE_CAR_RANGE)
			    LogCarCycle(c, 3);
			else if (Car[c].oper == FORCED_PARKING) {
			    LogCarCycle(c, 4);
				//Car[c].dir = NO_DIR;      // assume no preferred direction
			}
		
		    scan_for_target = 1;          // assume that we need to scan list of calls
			if (Car[c].pending_move) {    // must respond to a pending hide/unhide move first

// Check if condition-based move and initiation condition is met
                if (Move[Car[c].i_move].initiation_basis) {
				  MoveRelativity(c, Move[Car[c].i_move].initiation_relativity,
					  &relativity_status, &time_to_satisfy);

				  if (relativity_status == 2 || relativity_status == 3) {
				      if (c % 2)          // index of other car in hoistway
					      other_c = c - 1;
				      else
					      other_c = c + 1;

					  scan_for_target = 0;
					  Car[c].oper = WAIT_INITIATE;
					  Car[other_c].other_car_waiting = 1;
					  if (Car[c].other_car_waiting)
						  fprintf(f_ErrorLog, "UpdateTarget 6: %d %d %d at t=%lf\n",
						      c, other_c, Car[c].i_move, Sys.t_nextEvent);

					  if (relativity_status == 2)
					      Car[c].t_end_oper = time_to_satisfy;
				      else if (relativity_status == 3)
						  Car[c].t_end_oper = 1.0e10;  // wait indefinitely until other car moves
				  }
				}

				scan_for_target = 0;
				if (Move[Car[c].i_move].destination != f) {  // run to parking position
					if(m_bParamHard)
					{
						Car[c].oper = FORCED_MOVE;
						Car[c].target = Move[Car[c].i_move].destination;
						if (Car[c].target > Car[c].floor)
							Car[c].dir = 1;         // set direction case 2a
						else
							Car[c].dir = -1;        // set direction case 2b
						LogCarCycle(c, 1);
						Car[c].t_decel = Car[c].t_run_start + HardCommitTime(f, Car[c].target);
						Car[c].t_actual_position = Car[c].t_run_start +
							HardActualPositionTime(f, Car[c].target);
						Car[c].t_end_oper = Sys.t_nextEvent + HardRunTime(f, Car[c].target);
					}
					else
					{
						Car[c].oper = FORCED_MOVE;
//						Car[c].changed_target = 1;
						Car[c].target = Move[Car[c].i_move].destination;
						if (Car[c].target > Car[c].floor)
							Car[c].dir = 1;         // set direction case 2a
						else
							Car[c].dir = -1;        // set direction case 2b
						LogCarCycle(c, 1);
						dist = fabs(FloorToPos(Car[c].target) - FloorToPos(Car[c].floor));
						GenerateFullProfile(c, dist);
						Car[c].t_decel = Car[c].t_run_start + GetCommittableTime(c, f, Car[c].target);
						AP_time = GetActualPositionTime(c, f, Car[c].target);
						if (AP_time > EPSILON)
						Car[c].t_actual_position = Car[c].t_run_start + AP_time;
						index = GetBreakPointTableIndex(c, Sys.t_nextEvent);
						if(index < Car[c].mp_n_break-1 && index >= -1)
							Car[c].t_end_oper = Car[c].mp_break[index+1];
						else
							Car[c].t_end_oper = Car[c].mp_break[1];
					}
				}
			// hold at parking position
				else if (Move[Car[c].i_move].t_release > Sys.t_nextEvent + EPSILON) {
					Car[c].oper = FORCED_PARKING;
					Car[c].dir = NO_DIR;       // set direction case 2c
					LogCarCycle(c, 2);
					Car[c].t_end_oper = Move[Car[c].i_move].t_release;
				}
				else if (Car[c].oper != IDLE)  // else beyond release time, so scan
					scan_for_target = 1;
			
				Car[c].pending_move = 0;
			}
		
			if (scan_for_target) {                         // find new target from list of calls
				if((Car[c].oper == CLOSING) && (Param.bReopenEnabled != TRUE))
				{
					if(Car[c].dir == UP)
						ScanCalls(c, f+1, n_floor-1, f, 0, f, 0,
						&n_call, &new_target, &predirection, &demand_target);  // ScanCalls case 2
					else if(Car[c].dir == DN)
						ScanCalls(c, f, n_floor-1, f-1, 0, f, 0,
						&n_call, &new_target, &predirection, &demand_target);  // ScanCalls case 2
					else
						ScanCalls(c, f, n_floor-1, f, 0, f, 0,
						&n_call, &new_target, &predirection, &demand_target);  // ScanCalls case 2
				}
				else
					ScanCalls(c, f, n_floor-1, f, 0, f, 0,
					&n_call, &new_target, &predirection, &demand_target);  // ScanCalls case 2
			
				if (Car[c].oper == IDLE && (n_call > 1 || Car[c].dir != 0))
					fprintf(f_ErrorLog, "UpdateTarget 7: idle car %d dir %d now with %d calls at t=%lf\n",
					c, Car[c].dir, n_call, Sys.t_nextEvent);
			
				run_to_new_target = 0;
				if (demand_target < 0 && new_target == f)   // no demand target so go to idle
				{
					Car[c].oper = IDLE;
					Car[c].t_start_oper = Sys.t_nextEvent;
					Car[c].dir = NO_DIR;                     // set direction case 1a
				}
				else if (demand_target == f && new_target == f) {  // open doors at current floor
					//shin050713
					//Use door command from HPE
					if (UpdatedCarCommand[c] == TRUE)
					{
						switch (CarCommandFromHPE[c].frontDoorCommand)
						{
						case DOOR_NOT_OPEN:
							Car[c].oper = RUNNING;
							Car[c].t_start_oper = Sys.t_nextEvent;
							Car[c].dir = predirection;
							run_to_new_target = 1;
							break;
						case DOOR_NORMAL:
						case DOOR_HURRIED:
						case DOOR_CLOSE_CUT:
						case DOOR_DELAYED:
							Car[c].oper = OPENING;
							Car[c].t_start_oper = Sys.t_nextEvent;
							Car[c].dir = predirection;               // set direction to go (case 1b)
							LogCarCycle(c, 2);
							Car[c].n_board=CalculateExpectedBoarder(c);
							Car[c].n_deboard=Car[c].n_CCall[Car[c].floor];
							Car[c].t_end_oper = Sys.t_nextEvent + Param.door_open;
							break;
						default:
							break;
						}
					}
					else
					{
						Car[c].oper = OPENING;	// need consideration
						Car[c].t_start_oper = Sys.t_nextEvent;
						Car[c].n_board= CalculateExpectedBoarder(c);
						Car[c].n_deboard = Car[c].n_CCall[Car[c].floor];
						Car[c].dir = predirection;               // set direction to go (case 1b)
						LogCarCycle(c, 2);
						Car[c].t_end_oper = Sys.t_nextEvent + Param.door_open;
					}
				}
				else if (demand_target >= 0 && new_target == demand_target) {  // run car to call location
					// shin050830 Do not change the oper directly at car's door closing period
					// and update target at the end of closing
					if(!((Car[c].oper == CLOSING) && ((Car[c].t_end_oper-EPSILON) > Sys.t_nextEvent)))
					{
						Car[c].oper = RUNNING;
						Car[c].t_start_oper = Sys.t_nextEvent;
						//					Car[c].changed_target = 1;
						run_to_new_target = 1;
					}
				}
				else if (new_target == f) {                  // different demand target
					Car[c].oper = WAIT_CAR_RANGE;
					//Car[c].dir = NO_DIR;                     // set direction case 1a
					Car[c].dir = predirection; //shin050906 dir is not NO_DIR for moving to yield stop, ?? and WAIT_CAR_RANGE
				}
				else {                                       // different target that differs from demand target
					Car[c].oper = MOVE_CAR_RANGE;
					Car[c].t_start_oper = Sys.t_nextEvent;
					run_to_new_target = 1;
				}

				if (run_to_new_target) {                     // run car to call location
					Car[c].target = new_target;
					Car[c].dir = predirection;			     // set direction case 1c
					LogCarCycle(c, 1);
					if(m_bParamHard)
					{
						Car[c].t_decel = Car[c].t_run_start + HardCommitTime(f, new_target);
						Car[c].t_actual_position = Car[c].t_run_start +
							HardActualPositionTime(f, new_target);
						Car[c].t_end_oper = Car[c].t_run_start + HardRunTime(f, new_target);
					}
					else
					{
						dist = fabs(FloorToPos(Car[c].target) - FloorToPos(Car[c].floor));
						GenerateFullProfile(c, dist);
						Car[c].t_decel = Car[c].t_run_start + GetCommittableTime(c, f, new_target);
						AP_time = GetActualPositionTime(c, f, new_target);
						if (AP_time > EPSILON)
							Car[c].t_actual_position = Car[c].t_run_start + AP_time;
						Car[c].t_end_oper = Car[c].mp_break[1];
					}
				}
			}
		}  // else (pending_move != 2)
	}  // standing case
}  // UpdateTarget


// Determines direction right before doors begin to open for match with boarding passengers
void CMy2DSimDlg::SetPreDirection(int c)
{
  int f, n_call, new_target, predirection, demand_target;

  f = Car[c].floor;
  ScanCalls(c, f, n_floor-1, f, 0, f, 1,
	  &n_call, &new_target, &predirection, &demand_target);  // ScanCalls case 3

  Car[c].dir = predirection;  // set direction case 3
}  // SetPreDirection


void CMy2DSimDlg::UpdatePreviousQueue(int floor, int increment)
{
  Floor[floor].length1 += (double) Floor[floor].length * (Sys.t_nextEvent - Floor[floor].t_break);
  Floor[floor].length2 += (double) Floor[floor].length * Floor[floor].length * (Sys.t_nextEvent - Floor[floor].t_break);
  Floor[floor].length += increment;
  if (Floor[floor].length > Floor[floor].max_length)
	Floor[floor].max_length = Floor[floor].length;

  Floor[floor].t_break = Sys.t_nextEvent;
}  // UpdatePreviousQueue


void CMy2DSimDlg::HandleNewPassenger(int p)
{
	int c, origin, pdir, c_scan;
	//CString str; //shin050908deleted
	double dwellTime, normalDwellTime;
	int other; // the other car in the same hoistway
	
	Pax[p].reregister = 0;  // assume no re-registration unless revised in Board

	if (m_bDispatcherAttached==TRUE)
	{
		EnterNewCall(p); // has side-effect of setting assignment_from_dispatcher
		_RPT1(_CRT_WARN,"Assigned=%d\n",AssignmentFromDispatcher);
		Pax[p].assignment = AssignmentFromDispatcher;
		// note: you still need to check if the dispatcher made an assignment or not 
		//-- theoretically, it can return -1 if all cars are not available (NAV) 
		//or if the traffic is so heavy that the system has reached the MAX_CALL limit 
		//or the MAX_LOAD limit.
		if (Pax[p].assignment < 0) {
			// reject call
			fprintf(f_ErrorLog, "HandleNewPassenger: pax %d was rejected.\n", p);
		}
		else {
			// check if any OCSS commands resulted from HPE as a result of the new plan.
			// Again, this is a side-effect of calling DispatchNewCall
			//...
		}
	}

	if (Pax[p].assignment < 0 || (m_bReassign && !m_bReassigned))
	{
		//printf("Need assignment\n");
		m_bAssigned = FALSE;
		m_bNeedAssign = TRUE;
		return;
	}
	else
	{
		m_bAssigned = TRUE;
	}
	m_bReassigned = FALSE;
	//shin050908deleted//str.Format("%3d %6.f %3d %3d %2d Assigned.\f\n",p, Pax[p].t_arrival,
	//    USERFLOOR(Pax[p].f_origin), USERFLOOR(Pax[p].f_dest), Pax[p].assignment);
	//m_debug += str;
	c = Pax[p].assignment;
	origin = Pax[p].f_origin;
	Floor[origin].n_arrival++;
	// shs0708 HP assign when manual dispatching with HM
	if (m_bHMAttached==TRUE && m_bDispatcherAttached==FALSE)
	{
		Call call;
		call.callType = 0; // normal CALL_TYPE_STANDARD; // TBD
		call.origin=Pax[p].f_origin;
		call.destination=Pax[p].f_dest;
		call.nPassengers=1; // Pax[p].xxx TBD
		TIME_T	currentTime;
		currentTime=MakeIntTime(Sys.t_nextEvent); // 0.1sec unit long integer
		//
		CarState carState[MAX_CAR];
		MakeUpAllCarStatus(carState); 
		//shin050715
		AcceptAssignment(c,call,carState,currentTime); 

	}

	// shin050715
	UpdateTargetFollowingHPE();
	
	// Find passenger direction "pdir"
	if (Pax[p].f_dest > origin)
		pdir = UP;
	else if (Pax[p].f_dest < origin)
		pdir = DN;
	else
		fprintf(f_ErrorLog, "HandleNewPassenger: pax %d has origin=dest=%d\n",
		p, USERFLOOR(origin));
	
	// Count passenger in hall queue
	UpdatePreviousQueue(origin, 1);
	
	if (pdir == UP) {  // up direction
		// Determine if this is a new car-direction registration
		Pax[p].new_registration_car_direction = (Car[c].n_UpCall[origin] == 0);
		// Determine if this is a new direction registration for any car (traditional definition)
        Pax[p].new_registration_direction = 1;
		for (c_scan = 0; c_scan < n_car; c_scan++)
		    if (Car[c_scan].n_UpCall[origin] > 0)
				Pax[p].new_registration_direction = 0;

		// Count passenger into car's hall call queue
		Car[c].n_UpCall[origin]++;
		// Push passenger into car's hall call linked list
		if (Car[c].z_UpCall[origin] < 0)
			Car[c].q_UpCall[origin] = p;
		else
			Pax[Car[c].z_UpCall[origin]].q_next = p;
		Car[c].z_UpCall[origin] = p;
		// Count destination in future car call
		Car[c].n_UpStop[Pax[p].f_dest]++;
	}
	
	else {  // similar for down direction
		Pax[p].new_registration_car_direction = (Car[c].n_DnCall[origin] == 0);
        Pax[p].new_registration_direction = 1;
		for (c_scan = 0; c_scan < n_car; c_scan++)
		    if (Car[c_scan].n_DnCall[origin] > 0)
				Pax[p].new_registration_direction = 0;
		Car[c].n_DnCall[origin]++;
		if (Car[c].z_DnCall[origin] < 0)
			Car[c].q_DnCall[origin] = p;
		else
			Pax[Car[c].z_DnCall[origin]].q_next = p;
		Car[c].z_DnCall[origin] = p;
		Car[c].n_DnStop[Pax[p].f_dest]++;
	}
	Pax[p].q_next = -1;

	// If passenger arrived as doors were open, adjust dwell time
	if (Car[c].oper == DWELL && Car[c].floor == origin  && pdir == Car[c].dir)
	{  
		if(!m_bParamHard) //shin 041025 CAGS dwell time logic
		{//shin050902 Apply Executor's dwell time extension.
			normalDwellTime = DoorDwell(c);
			if((CarCommandFromHPE[c].frontDoorCommand == DOOR_CLOSE_CUT) && (UpdatedCarCommand[c] == TRUE))
			{
				// Get dwell time from HPE
				dwellTime = CarCommandFromHPE[c].frontDoorDwell/1000.0;
				if(dwellTime < normalDwellTime)
					dwellTime = normalDwellTime;
			}
			else
			{
				dwellTime = normalDwellTime;
			}
			//shin050906 Check dwellTime.
			// ManualTestImpact
			if(dwellTime < Sys.t_nextEvent - Car[c].t_door_opened)
			{
				dwellTime = Sys.t_nextEvent - Car[c].t_door_opened;
				fprintf(f_ErrorLog, "HandleNewPassenger: dwellTime is too small for car %d!(%lf) < (%lf - %lf)\n",
				c, dwellTime, Sys.t_nextEvent, Car[c].t_door_opened);
			}

			Car[c].t_end_oper = Car[c].t_door_opened + dwellTime;
		}
		AdjustDoorDwell(c, Car[c].t_end_oper);
	}
	else if (Car[c].oper == DWELL && Car[c].floor == origin &&
		    (Car[c].dir == NO_DIR || Car[c].dir == NO_PREDIRECTION))
	{
		Car[c].dir = pdir;  // set direction case 4
		if(!m_bParamHard) //shin 041025 CAGS dwell time logic
		{//shin050902 Apply Executor's dwell time extension.
			normalDwellTime = DoorDwell(c);
			if((CarCommandFromHPE[c].frontDoorCommand == DOOR_CLOSE_CUT) && (UpdatedCarCommand[c] == TRUE))
			{
				// Get dwell time from HPE
				dwellTime = CarCommandFromHPE[c].frontDoorDwell/1000.0;
				if(dwellTime < normalDwellTime)
					dwellTime = normalDwellTime;
			}
			else
			{
				dwellTime = normalDwellTime;
			}
			//shin050906 Check dwellTime.
			// ManualTestImpact
			if(dwellTime < Sys.t_nextEvent - Car[c].t_door_opened)
			{
				dwellTime = Sys.t_nextEvent - Car[c].t_door_opened;
				fprintf(f_ErrorLog, "HandleNewPassenger: dwellTime is too small for car %d!(%lf) < (%lf - %lf)\n",
				c, dwellTime, Sys.t_nextEvent, Car[c].t_door_opened);
			}

			Car[c].t_end_oper = Car[c].t_door_opened + dwellTime;
		}
		AdjustDoorDwell(c, Car[c].t_end_oper);
	}
	else if (Car[c].oper == OPENING && Car[c].floor == origin &&
		    (Car[c].dir == NO_DIR || Car[c].dir == NO_PREDIRECTION))
		Car[c].dir = pdir;  // set direction case 4a (?)

	//shin050912 direction updating by HPE
	if(!m_bParamHard && (UpdatedCarCommand[c] == TRUE))
		Car[c].dir = CarCommandFromHPE[c].pre_dir;

	//shin050902 Apply Executor's dwell time extension for the other car in the same hoistway.
	//only when hoistway manager is attached.
	if(m_bHMAttached==TRUE)
		other = GetOtherCarIndex(c);
	else
		other = MAXCAR;
	if(other != MAXCAR)
	{
		if (Car[other].oper == DWELL)
		{  
			if(!m_bParamHard)
			{
				normalDwellTime = DoorDwell(other);
				if((CarCommandFromHPE[other].frontDoorCommand == DOOR_CLOSE_CUT) && (UpdatedCarCommand[other] == TRUE))
				{
					dwellTime = CarCommandFromHPE[other].frontDoorDwell/1000.0;
					if(dwellTime < normalDwellTime)
						dwellTime = normalDwellTime;
				}
				else
				{
					dwellTime = normalDwellTime;
				}
				//shin050906 Check dwellTime.
				// ManualTestImpact
				if(dwellTime < Sys.t_nextEvent - Car[other].t_door_opened)
				{
					dwellTime = Sys.t_nextEvent - Car[other].t_door_opened;
					fprintf(f_ErrorLog, "HandleNewPassenger: dwellTime is too small for car %d!(%lf) < (%lf - %lf)\n",
						other, dwellTime, Sys.t_nextEvent, Car[other].t_door_opened);
				}
				
				Car[other].t_end_oper = Car[other].t_door_opened + dwellTime;
				if(UpdatedCarCommand[other] == TRUE)
					Car[other].dir = CarCommandFromHPE[other].pre_dir; //shin050912 other car's direction update according to new assignment.
			}
			AdjustDoorDwell(other, Car[other].t_end_oper);
		}
	}

sprintf(DEBUGMsg, "HandleNewPassenger: p=%d dir=%d c=%d oper=%d ass=%d\n", p, pdir, c,
		Car[c].oper, Pax[p].assignment);
DEBUGWrite(3);

	// CSW050802: if passengers arrive and the dor is closing, reopen the door
	// ManualTestImpact
	if (Car[c].oper == CLOSING && Car[c].floor == origin  && pdir == Car[c].dir) {  
		// reopen the door (with parameter 1
		UpdateTarget(c, 1);
	}
	else if (Car[c].oper == DWELL && Car[c].floor == origin &&
		    (Car[c].dir == NO_DIR || Car[c].dir == NO_PREDIRECTION)) {
		Car[c].dir = pdir;  // set direction case 4
		// reopen the door (with parameter 1
		UpdateTarget(c, 1);
	}
	else // CSW050802: check end
		// Wake up idle car or possibly reset target of car in flight
		UpdateTarget(c, 0);

	// To show updated numbers of passengers when new passenger occurs.
	CalculatePassengerInCars();
	if(m_pShowLoadDlg != NULL)
	{
		m_pShowLoadDlg->SetPassengerNumber(m_nPassenger, n_car, n_floor);
		//m_pShowLoadDlg->ShowWindow(SW_SHOW);
	}
	//Check Assignments for car c and other car in the same hoistway
	//CheckAssignments(c, origin, pdir, p);
}  // HandleNewPassenger


// Deboard all passengers on car "c" at the current floor
void CMy2DSimDlg::Deboard(int c)
{
	int f, p;
	
	f = Car[c].floor;
	Car[c].n_deboard = Car[c].n_CCall[f];  // save for DoorDwell
	
	if(f<0 || f>=n_floor)
	{
		fprintf(f_ErrorLog, "Deboard 1: The boarding car %d is at floor %d at %lf\n",
			c, USERFLOOR(f), Sys.t_nextEvent);
		return;
	}
	// Now unload each passenger, one-by-one
	for (p = Car[c].q_CCall[f]; p >= 0; p = Pax[p].q_next)
	{
		Pax[p].t_departure = Sys.t_nextEvent;
		Car[c].n_CCall[f]--;
		if(Pax[p].f_origin < f)
			Car[c].n_UpStop[f]--;
		else
			Car[c].n_DnStop[f]--;
		Car[c].n_pax_in_car--;
		Floor[f].n_departure++;
	}
	Car[c].q_CCall[f] = -1;
	if (Car[c].n_CCall[f] != 0)
		fprintf(f_ErrorLog, "Deboard 2: car %d at %d, remaining %d\n", c, USERFLOOR(f), Car[c].n_CCall[f]);
}  // Deboard


// Board all passengers (subject to capacity) assigned to car "c" in the
// current direction of the car (which is already set before doors are open)
void CMy2DSimDlg::Board(int c)
{
	int f, p, next_p, dest;
	
	//p = 0;                  // added by CSW 2/17/2005
	p = -1;                  // shin050823
	f = Car[c].floor;
	Car[c].n_board = 0;
	
	if (Car[c].dir == UP)
		p = Car[c].q_UpCall[f];
	else if (Car[c].dir == DN)
		p = Car[c].q_DnCall[f];
	else if (Car[c].dir == NO_PREDIRECTION)
		p = -1;  // no boarding due to impending car range move
	else if (Car[c].q_UpCall[f] >= 0 || Car[c].q_DnCall[f] >= 0)
		fprintf(f_ErrorLog, "Board: direction not set when boarding car %d at floor %d at %lf (%d %d)\n",
		c, USERFLOOR(f), Sys.t_nextEvent, Car[c].q_UpCall[f], Car[c].q_DnCall[f]);
	
	while (p >= 0) {
		if (Car[c].n_pax_in_car >= Param.capacity) {
			Pax[p].reregister = 1;   // passenger re-registers in traditional elevatoring
			Pax[p].t_reregister = Sys.t_nextEvent;
			break;
		}
		else {  // Transfer passenger from floor queue to car queue
			if (Car[c].t_decel < Pax[p].t_arrival)  // passenger arrived after decel point
				Pax[p].t_endwait_CP = Pax[p].t_arrival;
			else
				Pax[p].t_endwait_CP = Car[c].t_decel;  // waiting ends at decel point
			if (Car[c].t_actual_position < Pax[p].t_arrival)
				Pax[p].t_endwait_AP = Pax[p].t_arrival;
			else
				Pax[p].t_endwait_AP = Car[c].t_actual_position;
			if (Pax[p].reregister && Pax[p].t_endwait_CP < Pax[p].t_reregister)
				Pax[p].t_endwait_CP = Pax[p].t_reregister;
			if (Pax[p].reregister && Pax[p].t_endwait_AP < Pax[p].t_reregister)
				Pax[p].t_endwait_AP = Pax[p].t_reregister;
			Pax[p].t_door_opening = Car[c].t_run_end;
			Pax[p].t_door_opened = Car[c].t_door_opened;
			Pax[p].t_door_close = Sys.t_nextEvent;  // warning: there is a problem that this never
													// gets updated if door dwell is extended
			
			next_p = Pax[p].q_next;
			dest = Pax[p].f_dest;
			Pax[p].q_next = Car[c].q_CCall[dest];
			Car[c].q_CCall[dest] = p;
			sprintf(DEBUGMsg, "Boarding c=%d dir=%d p=%d",
				c, Car[c].dir, p);
			DEBUGWrite(3);
			p = next_p;
			
			Car[c].n_board++;
			Car[c].n_CCall[dest]++;
			Car[c].n_pax_in_car ++;    // number in car
		}
	}
	
	// Update counters
	UpdatePreviousQueue(f, -Car[c].n_board);  // number in floor queue
	if (Car[c].dir == UP)                      // number in car's assigned queue
		Car[c].n_UpCall[f] -= Car[c].n_board;
	else if(Car[c].dir == DN)	//shin050823 dir could be 0.
		Car[c].n_DnCall[f] -= Car[c].n_board;
	
	// Update headers to hall call linked list
	if (Car[c].dir == UP) {
		Car[c].q_UpCall[f] = p;
		if (p < 0)
			Car[c].z_UpCall[f] = -1;
	}
	else if (Car[c].dir == DN)	//shin050823 dir could be 0.
	{
		Car[c].q_DnCall[f] = p;
		if (p < 0)
			Car[c].z_DnCall[f] = -1;
	}
	
	sprintf(DEBUGMsg, "End of boarding c=%d f=%d dir=%d num-in-car %d (p=%d nqU=%d nqD=%d)",
		c, USERFLOOR(f), Car[c].dir, Car[c].n_pax_in_car, p, Car[c].n_UpCall[f],Car[c].n_DnCall[f]);
	DEBUGWrite(3);
	
}  // Board


void CMy2DSimDlg::HandleCarOperation(int c)
{
	int index, end_of_run;
	double dwellTime, normalDwellTime;
	int other;

	// shs debug <check this is good place to put this code and mask out other part when HM is attached >
	if (m_bHMAttached) // shs0708 add put car state and issue monitoring
	{
		CarState carState;
		TIME_T currentTime;
		MakeUpCarState(c,&carState);
		PutCarState(c,&carState);
		other = GetOtherCarIndex(c);
		if(other != MAXCAR)
		{
			MakeUpOtherCarState(other, &carState);
			PutCarState(other, &carState);
		}

		currentTime=MakeIntTime(Sys.t_nextEvent);
		MonitorCarState(c,currentTime);
	}
	// shin050715
	UpdateTargetFollowingHPE();

	switch (Car[c].oper)
	{
	case OPENING:         // door opening has ended
		Car[c].door_cycle_in_car_cycle = 1;
		Car[c].t_door_opened = Sys.t_nextEvent;
		Car[c].oper = DWELL;
		Car[c].t_start_oper = Sys.t_nextEvent;
		Deboard(c);
		Car[c].n_board = CalculateExpectedBoarder(c);  // for HP model
		// To show updated numbers of passengers after deboarding.
		CalculatePassengerInCars();
		if(m_pShowLoadDlg != NULL)
		{
			m_pShowLoadDlg->SetPassengerNumber(m_nPassenger, n_car, n_floor);
		}

		normalDwellTime = DoorDwell(c);
		if((CarCommandFromHPE[c].frontDoorCommand == DOOR_CLOSE_CUT) && (UpdatedCarCommand[c] == TRUE))
		{
			// Get dwell time from HPE
			dwellTime = CarCommandFromHPE[c].frontDoorDwell/1000.0;
			// dwell time = max (normal dwell time, HPE dwell time).
			if(dwellTime < normalDwellTime)
				dwellTime = normalDwellTime;
		}
		else
		{
			dwellTime = normalDwellTime;
		}
		//shin050906 Check dwellTime.
		if(dwellTime < Sys.t_nextEvent - Car[c].t_door_opened)
		{
			dwellTime = Sys.t_nextEvent - Car[c].t_door_opened;
			fprintf(f_ErrorLog, "HandleNewPassenger: dwellTime is too small for car %d!(%lf) < (%lf - %lf)\n",
				c, dwellTime, Sys.t_nextEvent, Car[c].t_door_opened);
		}
		
		Car[c].t_end_oper = Car[c].t_door_opened + dwellTime;
		AdjustDoorDwell(c, Car[c].t_end_oper);
		break;
	case DWELL:           // door dwell has ended
		Board(c);
		CheckRejection(c);	//moved from closing timing to dwell timing
		Car[c].t_door_dwelled = Sys.t_nextEvent;
		Car[c].oper = CLOSING;
		Car[c].t_start_oper = Sys.t_nextEvent;
		Car[c].t_end_oper = Sys.t_nextEvent + Param.door_close;
		//Car[c].n_board=0;//shin050915 deleted.
		//Car[c].n_deboard=0;
		break;
	case CLOSING:         // door closing has ended
		UpdateTarget(c, 1);  // side effect of setting operation code to 0, 4 or 5
		//sprintf(DEBUGMsg, "c=%d, (after UpdateTarget) o=%d pm=%d", c, Car[c].oper, Car[c].pending_move);
		//DEBUGWrite(0);
		break;
	case RUNNING:
	case MOVE_CAR_RANGE:
		end_of_run = 1;
		if(!m_bParamHard)
		{
		index = GetBreakPointTableIndex(c, Sys.t_nextEvent);
			if(index < (Car[c].mp_n_break-1) && index >= -1)  // at a breakpoint during car run
		{
			Car[c].t_end_oper = Car[c].mp_break[index+1];
				end_of_run = 0;
			}
			else if(index == (Car[c].mp_n_break-1))
			{
				Car[c].t_end_oper = Car[c].mp_break[index] + Param.StopDelay;
				end_of_run = 0;
		}
		else
		{
			CheckMargin(c);
			}
		}
		
		if (end_of_run) 
		{	 // car run has ended
			Car[c].t_run_end = Sys.t_nextEvent;
			Car[c].floor = Car[c].target;
// Normal case: there is a reason to open the doors
			if (Car[c].q_CCall[Car[c].floor] >= 0||	//There are passengers on the car c to current floor
				Car[c].dir == UP && Car[c].q_UpCall[Car[c].floor] >= 0||	// There are passengers in the hall from current floor for up direction
				Car[c].dir == DN && Car[c].q_DnCall[Car[c].floor] >= 0)	// There are passengers in the hall from current floor for dn direction
			{
				SetPreDirection(c);
				Car[c].oper = OPENING;
				Car[c].n_board= CalculateExpectedBoarder(c);
				Car[c].n_deboard = Car[c].n_CCall[Car[c].floor];
				Car[c].t_start_oper = Sys.t_nextEvent;
				Car[c].t_end_oper = Sys.t_nextEvent + Param.door_open;
			}
// Otherwise doors not opened but let UpdateTarget decide what to do next
			else {
				//delete move
				//set end time for this operation
				// set next operation
				UpdateTarget(c, 1);
			}
		}
		break;
	case FORCED_MOVE:     // forced move (run) has ended
		if(!m_bParamHard)
		{
			index = GetBreakPointTableIndex(c, Sys.t_nextEvent);
			if(index < Car[c].mp_n_break-1 && index >= -1)
			{
				Car[c].t_end_oper = Car[c].mp_break[index+1];
			}
			else if(index == (Car[c].mp_n_break-1))
			{
				Car[c].t_end_oper = Car[c].mp_break[index] + Param.StopDelay;
			}
			else
			{
				CheckMargin(c);
				Car[c].t_run_end = Sys.t_nextEvent;

				if(Car[c].floor < Car[c].target)
					Car[c].dir = UP;
				else if(Car[c].floor < Car[c].target)
					Car[c].dir = DN;
				else
					Car[c].dir = NO_DIR;

				Car[c].floor = Car[c].target;
				Car[c].oper = FORCED_PARKING;
				if(Move[Car[c].i_move].t_release > Sys.t_nextEvent + Param.StopDelay)
					Car[c].t_end_oper = Move[Car[c].i_move].t_release;
				else
					Car[c].t_end_oper = Sys.t_nextEvent + Param.StopDelay;
				if (Sys.t_nextEvent > Car[c].t_end_oper)
					Car[c].t_end_oper = Sys.t_nextEvent;
			}
		}
		else
		{
			Car[c].t_run_end = Sys.t_nextEvent;
			Car[c].floor = Car[c].target;
			Car[c].dir = NO_DIR;	//Keep old logic for dir.
			Car[c].oper = FORCED_PARKING;
			Car[c].t_end_oper = Move[Car[c].i_move].t_release;
			if (Sys.t_nextEvent > Car[c].t_end_oper)
				Car[c].t_end_oper = Sys.t_nextEvent;
		}
		break;
	case FORCED_PARKING:  // forced park has ended
		UpdateTarget(c, 1);  // side effect of setting operation code to 0, 1 or 4
		break;
	case WAIT_INITIATE:
		break;
	case WAIT_RELEASE:
		break;
	default:
		fprintf(f_ErrorLog, "HandleCarOperation: invalid operation %d for car %d at t=%lf\n",
			Car[c].oper, c, Sys.t_nextEvent);
		break;
	}

	//shin050720
	if(Car[c].oper == IDLE)
	{
		if (m_bHMAttached)
		{
			CarState carState;
			TIME_T currentTime;
			MakeUpCarState(c,&carState);
			
			PutCarState(c,&carState);
			currentTime=MakeIntTime(Sys.t_nextEvent);
			MonitorCarState(c,currentTime);
		}
		// shin050715
		UpdateTargetFollowingHPE();
	}
}  // HandleCarOperation


// This function is to schedule actions to occur at predetermined times.  These actions are triggered
// by the simulation's event manager by schedule events that trigger the call to this function.  If
// there are no special events, then the body of this routine should be:
/*
  if (e == -1) {
    n_specialEvent = 0;  // change this to the number of special events
	i_specialEvent = 0;
// Initialize SpecialEventTime values
    ...
  }
// Add logic for each special event
  ...
  */ 
void CMy2DSimDlg::HandleSpecialEvent(int e) {

// BEGIN BODY --------------------------------------------------------------------------
  if (e == -1) {
    n_specialEvent = 0;
	i_specialEvent = 0;
// Initialize SpecialEventTime values
    
  }
// Add logic for each special event
  
// END BODY ---------------------------------------------------------------------------

  else
	fprintf(f_ErrorLog, "HandleSpecialEvent Z: unsupported special event %d (%d %d)\n",
	    e, i_specialEvent, n_specialEvent);
}  // HandleSpecialEvent


void CMy2DSimDlg::ProcessNextEvent() 
{
	int c;
	double t_now;
	int mCar;

	if(!m_trafficProfileOpened)
	{
		MessageBox("Please open the passenger profile!");
		return;
	}

	if (!ProfileCheckPassed)
	{
		if (ProfileCarsForCheck>Building.nCars || ProfileFloorsForCheck>Building.nTotalFloors)
		{ // profile configuration is less than user configuration
			if (MessageBox("Traffic Profile Cars or Floors are larger than user configuration, so simulation can be executed abnormally. Are you OK to Proceed?",
				"Warning of mismatching of configuration",MB_ICONWARNING | MB_YESNO )==IDNO)
				return;
		}
		ProfileCheckPassed=TRUE;
	}

	if(m_bNeedAssign)
	{
		if(i_pax_recent < 0)
		{
			i_pax = 1;
			fprintf(f_ErrorLog, "ProcessNextEvent 0: i_pax_recent %d\n",
			    i_pax_recent);
		}
		
		if((m_bReassign == FALSE) && (Pax[i_pax_recent].assignment >= 0) && (Pax[i_pax_recent].assignment < n_car))
		{
			m_bNeedAssign = FALSE;
			i_pax--;
		}
		
		else
		{
			if(!m_bMsgDisable)
				MessageBox("Please press car button to assign a car!");
			return;
		}
	}

	t_now = Sys.t_nextEvent;
	if (i_pax < n_pax) {          // look at next passenger arrival
		Sys.y_nextEvent = 1;
		Sys.i_nextEvent = i_pax;
		Sys.t_nextEvent = Pax[i_pax].t_arrival;
	}
	else {                        // no more passengers
		Sys.y_nextEvent = 0;
		Sys.t_nextEvent = 1.0e38;
	}
	
	if (i_move < n_move){         // look at next car move
		if (Move[i_move].t_initiation < Sys.t_nextEvent + EPSILON) {           // highest priority
			Sys.y_nextEvent = 2;
			Sys.i_nextEvent = i_move;
			Sys.t_nextEvent = Move[i_move].t_initiation;
		}
	}
	
	if (i_specialEvent < n_specialEvent) {
		if (SpecialEventTime[i_specialEvent] < Sys.t_nextEvent) {
			Sys.y_nextEvent = 4;
			Sys.i_nextEvent = i_specialEvent;
			Sys.t_nextEvent = SpecialEventTime[i_specialEvent];
		}
	}
	
	for (c = 0; c < n_car; c++){  // look at car operations
		if (Car[c].oper != IDLE && Car[c].oper != WAIT_CAR_RANGE &&
			Car[c].t_end_oper + EPSILON < Sys.t_nextEvent) {  // lowest priority
			Sys.y_nextEvent = 3;
			Sys.i_nextEvent = c;
			Sys.t_nextEvent = Car[c].t_end_oper;
		}
	}

	//shin050907 Check time flow.
	if(t_now > Sys.t_nextEvent+EPSILON)
		fprintf(f_ErrorLog, "ProcessNextEvent 0: t_now( %lf ) > Sys.t_nextEvent( %lf )\n",
			    t_now, Sys.t_nextEvent);

//sprintf(DEBUGMsg, "Next event type %d i=%d t=%lf (%d)", Sys.y_nextEvent, Sys.i_nextEvent,
//Sys.t_nextEvent, Car[4].oper);
//DEBUGWrite(0);
	// No more events, so stop simulation
	if (Sys.y_nextEvent == 0)
	{
		// End of simulation wrap-up
		Sys.t_nextEvent = t_now;  // restore time of the last event
		RemakeTrafficProfile();
		RecordData();
		
		CString str;

		str.Format("(%d %d)",IndexOfTrafficFile, NumberOfTrafficFile);
		DisplayWarningList(m_strTrafficProfileName+str);
		
		m_bSimulationEnd = TRUE;
		if((NumberOfTrafficFile == 0) || (IndexOfTrafficFile >= NumberOfTrafficFile-1))
		{
		MessageBox("Simulation Ended!");
		return;
	}
	}
	
	// Otherwise, handle next event
	else if (Sys.y_nextEvent == 1) {       // passenger arrival
		i_pax_recent=i_pax; 
		i_pax++;  
		HandleNewPassenger(Sys.i_nextEvent);
	}
	else if (Sys.y_nextEvent == 2)         // move
	{
		mCar = Move[i_move].car;
	    if (mCar < 0 || mCar >= n_car ||   // invalid dialog parameters
			Move[i_move].destination < 0 || Move[i_move].destination >= n_floor)
			fprintf(f_ErrorLog, "ProcessNextEvent 1: move parameters %d %d %d\n",
			    i_move, mCar, Move[i_move].destination);
		else if (Move[i_move].min_dwell > EPSILON) {  // adjustment of door dwell
			Car[mCar].dwell_floor = Move[i_move].destination;
			Car[mCar].dwell_initiate = Move[i_move].t_initiation;
			Car[mCar].dwell_release = Move[i_move].t_release;

			if (Car[mCar].oper == DWELL && Car[mCar].floor == Move[i_move].destination)
				AdjustDoorDwell(mCar, Car[mCar].t_end_oper);
		}
		else {                             // hide or unhide
			Car[mCar].pending_move = 1;    // indicate (standard) pending move
			Car[mCar].i_move = i_move;

// If car is idle
			if (Car[mCar].oper == IDLE)    // wake up idle car
				UpdateTarget(mCar, 0);
// If car is already in already in forced parking state, then consider adjusting its parameters
			else if (Car[mCar].oper == FORCED_PARKING)
			{
				if (Move[i_move].destination != Car[mCar].floor)  // car is supposed to be elsewhere
					Car[mCar].t_end_oper = Sys.t_nextEvent;  // immediately terminate the parking
				else {                     // otherwise, let new release time supercede old one
					if (Move[i_move].t_release >= Sys.t_nextEvent)
					  Car[mCar].t_end_oper = Move[i_move].t_release;
					else
					  Car[mCar].t_end_oper = Sys.t_nextEvent;
					Car[mCar].pending_move = 0;
				}
			}
// Otherwise, do not interrupt current car operation and let UpdateTarget take care of it
		}

		i_move++;
		// Calling the dispatcher
		ProcessDispatcher(DISPATCH_NO_ASSIGN);
	}
	else if (Sys.y_nextEvent == 3)         // car operation
	{
		HandleCarOperation(Sys.i_nextEvent);
			// Calling the dispatcher
		ProcessDispatcher(DISPATCH_NO_ASSIGN);
	}
	else if (Sys.y_nextEvent == 4) {       // special event
		HandleSpecialEvent(Sys.i_nextEvent);
		i_specialEvent++;
			// Calling the dispatcher
		ProcessDispatcher(DISPATCH_NO_ASSIGN);
	}

	else
	{
		fprintf(f_ErrorLog, "ProcessNextEvent 2: Sys.y_nextEvent = %d but not processed! (Should press Car button or something another)\n", Sys.i_nextEvent);
	}
	UpdateFloorForAllCars();
	for (c = 0; c < n_car; c++)
	{
		UpdatedCarCommand[c] = FALSE;
	}

	CollisionCheck();
	ShowEstimatedArrivalTime();

	if(m_bSpy)
		SpyCOGS();
	DebugFloorShow();
	Invalidate();
}  // ProcessNextEvent


void CMy2DSimDlg::RecordData()
{
	int p, f, z, n_run_normal, n_extra_dwell, n_hide, n_unhide, n_hide_pax, n_unhide_pax,
		m_hwy, n_new_registration_car_direction, n_new_registration_direction, n_reregister;
	FILE *p_log, *c_log, *summary;
	double sum_registration_AP_t, sum_registration_CP_t, sum_waiting_AP_t, sum_waiting_CP_t,
		sum_service_t, max_registration_AP_t, max_registration_CP_t, max_waiting_AP_t,
		max_waiting_CP_t, max_service_t, waiting_AP_t, waiting_CP_t, service_t;
	BYTE overall_rejection;
	int  cntNULL=0, cntH=0, cntC=0, cntD=0, cntHC=0, cntHD=0, cntCD=0;
	CString str;
	time_t ltime;

	//shin050811 generated file name according to 2dp file name.
	if ((p_log = fopen(m_strTrafficProfileName+"-p.csv", "w")) == NULL ||
		(c_log = fopen(m_strTrafficProfileName+"-c.csv", "w")) == NULL ||
		(summary = fopen(m_strTrafficProfileName+"-s.txt", "w")) == NULL)
	{
		MessageBox("Could not open output files");
		fprintf(f_ErrorLog, "RecordData 1: error opening output files\n");
	}
	
	// Passenger log file
	n_new_registration_car_direction = 0;
	n_new_registration_direction = 0;
	n_reregister = 0;
	sum_registration_AP_t = 0.0;
	sum_registration_CP_t = 0.0;
	sum_waiting_AP_t = 0.0;
	sum_waiting_CP_t = 0.0;
	sum_service_t = 0.0;
	max_registration_AP_t = 0.0;
	max_registration_CP_t = 0.0;
	max_waiting_AP_t = 0.0;
	max_waiting_CP_t = 0.0;
	max_service_t = 0.0;
	overall_rejection = 0;
	fprintf(p_log,"Index,T arrival,Origin,Dest,Assignment,T end wait AP,T end wait CP,T door opening,T door opened,T door close,T depart,Reg dir,Reg car-dir,Re-Reg,T Re-Reg,Reg AP T,Reg CP T,Wait AP T,Wait CP T,Srvc T,Rejection\n");
	for (p = 0; p < i_pax; p++)
	{
		fprintf(p_log, "%d,%5.3lf,%d,%d,%d,%5.3lf,%5.3lf,%5.3lf,%5.3lf,%5.3lf,%5.3lf,%d,%d,%d,"
			, p+1, Pax[p].t_arrival,
			USERFLOOR(Pax[p].f_origin), USERFLOOR(Pax[p].f_dest), Pax[p].assignment,
			Pax[p].t_endwait_AP, Pax[p].t_endwait_CP, Pax[p].t_door_opening, Pax[p].t_door_opened,
			Pax[p].t_door_close, Pax[p].t_departure, Pax[p].new_registration_direction,
			Pax[p].new_registration_car_direction, Pax[p].reregister);
		
		if (Pax[p].new_registration_direction)
			n_new_registration_direction++;
		if (Pax[p].reregister) {
			n_reregister++;
			fprintf(p_log, "%5.3lf,", Pax[p].t_reregister);
		}
		else
			fprintf(p_log, ",");
		
		waiting_AP_t = Pax[p].t_endwait_AP - Pax[p].t_arrival;
		waiting_CP_t = Pax[p].t_endwait_CP - Pax[p].t_arrival;
		service_t = Pax[p].t_departure - Pax[p].t_arrival;
		if (Pax[p].new_registration_car_direction) {
			fprintf(p_log, "%5.3lf,%5.3lf,%5.3lf,%5.3lf,%5.3lf,",
				waiting_AP_t, waiting_CP_t, waiting_AP_t, waiting_CP_t, service_t);
			n_new_registration_car_direction++;
			sum_registration_AP_t += waiting_AP_t;
			if (waiting_AP_t > max_registration_AP_t)
				max_registration_AP_t = waiting_AP_t;
			sum_registration_CP_t += waiting_CP_t;
			if (waiting_CP_t > max_registration_CP_t)
				max_registration_CP_t = waiting_CP_t;
		}
		else
			fprintf(p_log, ",,%5.3lf,%5.3lf,%5.3lf,", waiting_AP_t, waiting_CP_t, service_t);

		str = "";
		if(Pax[p].rejection & NULL_MASK)
		{
			cntNULL++;
			str += "NULL ";
		}
		if(Pax[p].rejection & H_MASK)
		{
			cntH++;
			str += "H ";
		}
		if(Pax[p].rejection & C_MASK)
		{
			cntC++;
			str += "C ";
		}
		if(Pax[p].rejection & D_MASK)
		{
			cntD++;
			str += "D ";
		}
		if(Pax[p].rejection & HC_MASK)
		{
			cntHC++;
			str += "HC ";
		}
		if(Pax[p].rejection & HD_MASK)
		{
			cntHD++;
			str += "HD ";
		}
		if(Pax[p].rejection & CD_MASK)
		{
			cntCD++;
			str += "CD ";
		}
		fprintf(p_log, "%s\n", str);

		sum_waiting_AP_t += waiting_AP_t;
		sum_waiting_CP_t += waiting_CP_t;
		sum_service_t += service_t;
		if (waiting_AP_t > max_waiting_AP_t)
			max_waiting_AP_t = waiting_AP_t;
		if (waiting_CP_t > max_waiting_CP_t)
			max_waiting_CP_t = waiting_CP_t;
		if (service_t > max_service_t)
			max_service_t = service_t;
		overall_rejection |= Pax[p].rejection;
	}
	fclose(p_log);
	
	// Car log file
	n_run_normal = 0;
	n_extra_dwell = 0;
	n_hide = 0;
	n_unhide = 0;
	n_hide_pax = 0;
	n_unhide_pax = 0;
	fprintf(c_log, "Car,Type,T begin run,T decel,T reach AP,T end run,Origin,Dest,Pax in run,Changed target,T opened,Deboard,Board,T dwelled,Extra dwell,T closed,T released\n");
	for (z = 0; z < n_carlog; z++)
		if (CarLog[z].type == 'N' || CarLog[z].type == 'R') {
			fprintf(c_log, "%d,%c,%5.3lf,%5.3lf,%5.3lf,%5.3lf,%d,%d,%d,%d",
				CarLog[z].car, CarLog[z].type, CarLog[z].t_run_start, CarLog[z].t_decel,
				CarLog[z].t_actual_position,
				CarLog[z].t_run_end, USERFLOOR(CarLog[z].origin), USERFLOOR(CarLog[z].dest),
				CarLog[z].n_pax_run, CarLog[z].changed_target);
			if (CarLog[z].door_cycle_in_car_cycle)
				fprintf(c_log, ",%5.3lf,%d,%d,%5.3lf,%d,%5.3lf\n",
					CarLog[z].t_door_opened, CarLog[z].n_deboard, CarLog[z].n_board,
					CarLog[z].t_door_dwelled, CarLog[z].extra_dwell, CarLog[z].t_door_closed);
			else
				fprintf(c_log, "\n");

			if (CarLog[z].origin != CarLog[z].dest)
				n_run_normal++;
			if (CarLog[z].extra_dwell)
				n_extra_dwell++;
		}
		else {
			fprintf(c_log, "%d,%c,%5.3lf,%5.3lf,%5.3lf,%5.3lf,%d,%d,%d,%d,,,,,,,%5.3lf\n",
				CarLog[z].car, CarLog[z].type, CarLog[z].t_run_start, CarLog[z].t_decel,
				CarLog[z].t_actual_position,
				CarLog[z].t_run_end, USERFLOOR(CarLog[z].origin), USERFLOOR(CarLog[z].dest),
				CarLog[z].n_pax_run, CarLog[z].changed_target,
				CarLog[z].t_released);
			if (CarLog[z].type == 'H') {
				n_hide++;
				if (CarLog[z].n_pax_run)
					n_hide_pax++;
			}
			else {
				n_unhide++;
				if (CarLog[z].n_pax_run)
					n_unhide_pax++;
			}
		}
	fclose(c_log);
		
		//fclose(h_log);
		
// Summary report
	if (n_new_registration_car_direction > 0) {
		sum_registration_AP_t /= (double)n_new_registration_car_direction;
		sum_registration_CP_t /= (double)n_new_registration_car_direction;
	}
	if (i_pax > 0) {
		sum_waiting_AP_t /= (double)i_pax;
		sum_waiting_CP_t /= (double)i_pax;
		sum_service_t /= (double)i_pax;
	}

	time(&ltime);
	fprintf(summary, "SUMMARY REPORT\n\
                    Profile : %s\n\
             Engine version : %s\n\
             Dispatcher DLL : %s\n\
                  Algorithm : %s\n\
                   Run date : %s\n", profileFileName, ENGINEVERSION, 
				   Param.dispatcherDLLName,Param.dispatchingAlgorithmName,
				   ctime(&ltime));

	fprintf(summary, "\
       Number of passengers : %d", n_pax);
	if (i_pax < n_pax)
		fprintf(summary, " (%d simulated)", i_pax);

	fprintf(summary, "\n\
                     floors : %d (%d + %d pit + %d overhead)\n\
                  hoistways : %d\n\
                       cars : %d\n\n\
             Time simulated : %5.3lf\n\
                Normal runs : %d\n\
    Subset with extra dwell : %d\n\
  Number of specified moves : %d\n\
      Hide moves (with pax) : %d (%d)\n\
    Unhide moves (with pax) : %d (%d)\n\
      Total runs and cycles : %d\n",
      n_floor, Building.nUserFloors, Building.pit, Building.overhead, n_hwy, n_car,
      Sys.t_nextEvent, n_run_normal, n_extra_dwell, n_move, n_hide, n_hide_pax,
	  n_unhide, n_unhide_pax, n_carlog);

	if (! m_bParamHard) {
		m_hwy = Car[m_car].i_hwy;
		fprintf(summary, " Closest margin (time, hwy) : %5.3lf (%5.3lf, %d)\n",
			m_minMargin, m_minMarginTime, m_hwy);
	}
	else
		fprintf(summary, "             Closest margin : not reported due to using CAGS physics\n");

	fprintf(summary, "\n\
  Destination registrations : %d\n\
    Hall call registrations : %d\n\
           Re-registrations : %d\n\
 Avg registration time (AP) : %5.3lf\n\
 Max registration time (AP) : %5.3lf\n\
      Avg waiting time (AP) : %5.3lf\n\
      Max waiting time (AP) : %5.3lf\n\
           Avg service time : %5.3lf\n\
           Max service time : %5.3lf\n\n\
 Avg registration time (CP) : %5.3lf\n\
 Max registration time (CP) : %5.3lf\n\
      Avg waiting time (CP) : %5.3lf\n\
      Max waiting time (CP) : %5.3lf\n\n",
	  n_new_registration_car_direction, n_new_registration_direction, n_reregister,
	  sum_registration_AP_t, max_registration_AP_t, sum_waiting_AP_t, max_waiting_AP_t,
	  sum_service_t, max_service_t, sum_registration_CP_t, max_registration_CP_t,
	  sum_waiting_CP_t, max_waiting_CP_t);

// Floor statistics
	fprintf(summary, "FLOOR SUMMARY STATISTICS:\n\
  Floor  Arrivals  Departures  Avg-Queue  Max-Queue  StdDev-Queue\n");
	
	for (f = n_floor - 1 - Building.overhead; f >= Building.pit; f--) {
		if (Floor[f].length != 0)
			fprintf(f_ErrorLog, "RecordData 2: simulation ends with %d left on floor %d\n",
				Floor[f].length, USERFLOOR(f));
		Floor[f].length1 /= Sys.t_nextEvent;        // mean queue length
		Floor[f].length2 /= Sys.t_nextEvent;
		Floor[f].length2 -= Floor[f].length1*Floor[f].length1;
		Floor[f].length2 = sqrt(Floor[f].length2);  // std dev of queue length
				
		fprintf(summary, "%5d %8d %10d %12.3lf %9d %13.3lf\n", USERFLOOR(f),
			Floor[f].n_arrival,	Floor[f].n_departure, Floor[f].length1,
			Floor[f].max_length, Floor[f].length2);
	}

	// overall rejection status
	fprintf(summary, "\nOverall Rejection Occurrence:\n");
	fprintf(summary, "  Null_Rej(%x), H_Rej(%x), C_Rej(%x), D_Rej(%x), HC_Rej(%x), HD_Rej(%x), CD_Rej(%x)\n",
		overall_rejection&NULL_MASK?1:0,
		overall_rejection&H_MASK?1:0,
		overall_rejection&C_MASK?1:0,
		overall_rejection&D_MASK?1:0,
		overall_rejection&HC_MASK?1:0,
		overall_rejection&HD_MASK?1:0,
		overall_rejection&CD_MASK?1:0);
	fprintf(summary, "\nOverall Rejection Count:\n");
	fprintf(summary, "  NULL_Rej(%d), H_Rej(%d), C_Rej(%d), D_Rej(%d), HC_Rej(%d), HD_Rej(%d), CD_Rej(%d)\n",
		cntNULL, cntH, cntC, cntD, cntHC, cntHD, cntCD);
	fprintf(summary, "Object Function Coefficients: A1S(%ld), A2S(%ld), A1W(%ld), A2W(%ld), V(%ld), W(%ld)\n",
		Param.A1S, Param.A2S, Param.A1W, Param.A2W, Param.V, Param.W);
	fclose(summary);
}  // RecordData


//////////////////////////////////////////////////////////////
/*			  Display Functions       						*/
//////////////////////////////////////////////////////////////

void CMy2DSimDlg::UpdateDisplay()
{
	InitDrawBuilding();
	DrawElevator();
	DrawBuilding();
	DrawLoad();
	if(!m_trafficProfileOpened)
		return;
	DrawQueLength();
	DrawCalls();
	DrawRange();
	DrawNCF();
	DrawTarget();
	DrawState();
	//DrawLabel();
	UpdateText();
}


void CMy2DSimDlg::InitDrawBuilding()
{
	CRect rectList;

	nCols = (n_hwy)*3 + 2; //2 is for floor label and passenger call
	nRows = Building.nUserFloors + 3 + 2;//3: 2 states and label, 2: overhead and pit

	this->GetClientRect(&rectDlg);

	m_ListWarning.GetWindowRect(&rectList);

	rectDlg.bottom-=rectList.Height();

	cellWidth = (int)((rectDlg.right-100)/nCols);
	cellHeight = (int)((rectDlg.bottom-150)/nRows);
	rectTable.top = 80;
	rectTable.left = 50;
	rectTable.bottom = rectTable.top + cellHeight*nRows;
	rectTable.right = rectTable.left + cellWidth*nCols;
}


void CMy2DSimDlg::DrawBuilding()
{
	CString strFloorNum;
	int i,fl;
	CPen* oldPen;
	CBrush* oldBrush;
	CDC* pDC = this->GetDC();

	pDC->SetBkMode(TRANSPARENT);
	oldPen=pDC->SelectObject(&penB0);
	oldBrush=pDC->SelectObject(&brushB);
    
	for(i=0; i<nRows+1; i++)
	{
		pDC->MoveTo(rectTable.left,rectTable.top+i*cellHeight);
		pDC->LineTo(rectTable.right,rectTable.top+i*cellHeight);
	}

	for(i=0; i<nCols+1; i++)
	{
		pDC->MoveTo(rectTable.left+i*cellWidth, rectTable.top);
		pDC->LineTo(rectTable.left+i*cellWidth, rectTable.bottom);
	}

	//Floor Number Display
	// update from label setting - shs
	for(fl=0; fl < Building.nTotalFloors; fl++)
	{
		pDC->TextOut(rectTable.left+1,
			rectTable.top+1+cellHeight*(Building.nTotalFloors-fl+1), Building.floorLabel[fl]);
	}
	pDC->TextOut(rectTable.left+1, rectTable.top+1+cellHeight*0, "UState");
	pDC->TextOut(rectTable.left+1, rectTable.top+1+cellHeight*1, "LState");
	pDC->TextOut(rectTable.left+1, rectTable.top+1+cellHeight*(Building.nUserFloors+4), "MLoad");
	pDC->SelectObject(&oldPen);
	pDC->SelectObject(&oldBrush);
	ReleaseDC(pDC);
}


void CMy2DSimDlg::DrawElevator()
{
	CString strCarNum;
	int iCol;//column index in the building. 
	int xPos, yPos;
	int i;
	int width, height;
	
	width = cellWidth-2;
	height = cellHeight-2;

	//InitFontSize(cellWidth, cellHeight);
	for(i=0; i<n_car; i++)
	{
		//iCol = (i/2*3+1)+2;//2 is for floor label and passenger call
		iCol = (Car[i].i_hwy * 3 + 1) + 2;//2 is for floor label and passenger call
		xPos = (int)(50 + (cellWidth*iCol))+2;
		//yPos = (int)(100 + (cellHeight*(n_floor-Car[i].floor+3)*1))+2;
		yPos = (int)(rectTable.top + (cellHeight*(Building.nUserFloors-GetDisplayedPosition(i)+3)*1))+2;
		if(Car[i].lowerOrUpper == LOWER_CAR)
			strCarNum.Format("%dL",i);
		else if(Car[i].lowerOrUpper == UPPER_CAR)
			strCarNum.Format("%dU",i);
		else
			fprintf(f_ErrorLog, "DrawElevator 1: invalid lowerOrUpper %d for car %d at t=%lf\n"
			, Car[i].lowerOrUpper, i, Sys.t_nextEvent);;
		if(m_bCar[i] == 0)
			m_bCar[i] = m_Car[i].Create(strCarNum, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(10,10,100,29), this, i+1100);
		m_Car[i].SetWindowPos(&wndTop, xPos,yPos,width,height, SWP_SHOWWINDOW);
		//m_Car[i].SetFont(&font);
		
		switch(Car[i].oper)
		{
		case IDLE:
			m_Car[i].SetWindowText(strCarNum+"I");
			break;
		case OPENING:
			m_Car[i].SetWindowText(strCarNum+"<<>>");
			break;
		case DWELL:
			m_Car[i].SetWindowText(strCarNum+"I   I");
			break;
		case CLOSING:
			m_Car[i].SetWindowText(strCarNum+">><<");
			break;
		case RUNNING:
			m_Car[i].SetWindowText(strCarNum);
			break;
		case FORCED_MOVE:
			m_Car[i].SetWindowText(strCarNum+"HIDE");
			break;
		case FORCED_PARKING:
			m_Car[i].SetWindowText(strCarNum+"HOLD");
			break;
		case END_OF_FORCED_PARKING:
			m_Car[i].SetWindowText(strCarNum+"TMP");
			break;
		case WAIT_INITIATE:
			m_Car[i].SetWindowText(strCarNum+"WIn");
			break;
		case WAIT_RELEASE:
			m_Car[i].SetWindowText(strCarNum+"WRl");
			break;
		case WAIT_CAR_RANGE:
			m_Car[i].SetWindowText(strCarNum+"WRg");
			break;
		case MOVE_CAR_RANGE:
			m_Car[i].SetWindowText(strCarNum+"MRg");
			break;
		default:
			fprintf(f_ErrorLog, "DrawElevator 2: invalid operation %d for car %d at t=%lf\n", Car[i].oper, i,
				Sys.t_nextEvent);
			break;
		}
	}
}


void CMy2DSimDlg::DrawLoad()
{
	CString str;
	int car;
	CPen* oldPen;
	int colIndex, rowIndex;
	int posX, posY;//center
	CDC* pDC = this->GetDC();
	pDC->SetBkMode(TRANSPARENT);
	
	for(car=0; car<n_car; car++)
	{
		if(Car[car].lowerOrUpper == LOWER_CAR)
			colIndex = Car[car].i_hwy*3+3;
		else //UPPER_CAR
			colIndex = Car[car].i_hwy*3+5;
		rowIndex = n_floor+2;
		posX = int(rectTable.left+(colIndex - 1)*cellWidth)+2;
		posY = int(rectTable.top+rowIndex*cellHeight)+2;
		
		oldPen=pDC->SelectObject(&penB);
		str.Format("%d", m_nMaxPassenger[car]);
		pDC->TextOut(posX, posY, str);
	}

	pDC->SelectObject(&oldPen);
	ReleaseDC(pDC);
}


void CMy2DSimDlg::DrawQueLength()
{
	CString strFloorNum;
	int fl;
	CPen* oldPen;
	CBrush* oldBrush;
	CDC* pDC = this->GetDC();

	pDC->SetBkMode(TRANSPARENT);
	oldPen=pDC->SelectObject(&penB0);
	oldBrush=pDC->SelectObject(&brushB);
    
	//Floor Queue Length Display
	for(fl=Building.pit; fl <= n_floor - 1 - Building.overhead; fl++)
	{
		strFloorNum.Format("%2d", Floor[fl].length);
		pDC->TextOut(rectTable.left-40,
			rectTable.top+1+cellHeight*(n_floor-Building.overhead-fl+2), strFloorNum);
	}
	pDC->SelectObject(&oldPen);
	pDC->SelectObject(&oldBrush);
	ReleaseDC(pDC);
}


void CMy2DSimDlg::DrawCalls()
{
	DrawRegistration();
	DrawAssignment();
}


void CMy2DSimDlg::DrawRegistration()
{
	int from, to, dir;

	//if(m_bAssigned == TRUE)
	//	return;
	if((i_pax_recent < 0) || (Sys.t_nextEvent < EPSILON))
		return;

	from = Pax[i_pax_recent].f_origin;
	to = Pax[i_pax_recent].f_dest;

	// Exclude the passenger who is not real.
	// From 0 to 0 (From pit to pit)
	if(from <= 0 && to <= 0)
		return;

	if(from < to)
		dir = UP;
	else if(from > to)
		dir = DN;
	else
	{
		dir = NO_DIR;
		fprintf(f_ErrorLog, "DrawRegistration: Origin floor and destination floor are equal (%d) at %f Passenger(%d)\n",
		    USERFLOOR(from), Sys.t_nextEvent,i_pax_recent);
		return;
	}
	if(Pax[i_pax_recent].t_arrival <= Sys.t_nextEvent)
	{
		DrawHCRegistration(dir, from);
		DrawCCRegistration(to);
	}
}


void CMy2DSimDlg::DrawHCRegistration(int dir, int fl)
{
	CPen* oldPen;
	CBrush* oldBrush;
	int colIndex, rowIndex;
	int posX, posY;//center
	CDC* pDC = this->GetDC();
	pDC->SetBkMode(TRANSPARENT);
	long radius,tX, tY;
	CPoint pt[3];

	tX = cellWidth/3;
	tY = cellHeight;
	if(tX > tY)
		radius = tY/2-2;
	else
		radius = tX/2-2;

	colIndex = 2;
	rowIndex = (Building.nUserFloors+4) - fl;
	posX = int(rectTable.left+(2*colIndex - 1)*cellWidth/2);
	posY = int(rectTable.top+(2*rowIndex - 1)*cellHeight/2);

	oldPen=pDC->SelectObject(&penB);
	if(dir == DN)
	{
		posX -= cellWidth/3;
		pt[0] = CPoint(posX, posY+radius);
		pt[1] = CPoint(posX-radius, posY-radius);
		pt[2] = CPoint(posX+radius, posY-radius);
		oldBrush=pDC->SelectObject(&brushR);
	}
	else if(dir == UP)
	{
		pt[0] = CPoint(posX, posY-radius);
		pt[1] = CPoint(posX-radius, posY+radius);
		pt[2] = CPoint(posX+radius, posY+radius);
		oldBrush=pDC->SelectObject(&brushG);
	}

	pDC->SetPolyFillMode(WINDING);
	pDC->Polygon(pt, 3);
	if(Sys.t_nextEvent == Pax[i_pax_recent].t_arrival)
	{
		if(dir == DN)
			pDC->SelectObject(&brushLR);
		else
			pDC->SelectObject(&brushLG);
		pDC->Rectangle(rectTable.left+cellWidth*2,posY-cellHeight/8, rectTable.right, posY+cellHeight/8);
	}

	pDC->SelectObject(&oldPen);
	pDC->SelectObject(&oldBrush);
	ReleaseDC(pDC);
}


void CMy2DSimDlg::DrawCCRegistration(int fl)
{
	CPen* oldPen;
	CBrush* oldBrush;
	int colIndex, rowIndex;
	int posX, posY;//center
	CDC* pDC = this->GetDC();
	pDC->SetBkMode(TRANSPARENT);
	long radius,tX, tY;
	CPoint pt[3];

	tX = cellWidth/3;
	tY = cellHeight;
	if(tX > tY)
		radius = tY/2-2;
	else
		radius = tX/2-2;

	colIndex = 2;
	rowIndex = (Building.nUserFloors+4) - fl;
	posX = int(rectTable.left+(2*colIndex - 1)*cellWidth/2);
	posY = int(rectTable.top+(2*rowIndex - 1)*cellHeight/2);
	posX += cellWidth/3;

	oldPen=pDC->SelectObject(&penB);
	oldBrush=pDC->SelectObject(&brushY);
	
	pDC->Ellipse(posX-radius, posY-radius, posX+radius, posY+radius);

	pDC->SelectObject(&oldPen);
	pDC->SelectObject(&oldBrush);
	ReleaseDC(pDC);
}


void CMy2DSimDlg::DrawAssignment()
{
	int car, fl;

	for(car=0; car<n_car; car++)
	{
		for(fl=1; fl<=Building.nUserFloors; fl++)
		{
			if(Car[car].n_CCall[fl])
				DrawCCAssignment(fl, car);
			else if((Car[car].n_UpStop[fl]) || (Car[car].n_DnStop[fl]))
				DrawCCAssignmentF(fl, car);
			if(Car[car].n_UpCall[fl])
				DrawHCAssignment(UP, fl, car);
			if(Car[car].n_DnCall[fl])
				DrawHCAssignment(DN, fl, car);
		}
	}
}


//Passenger got on the car already.
void CMy2DSimDlg::DrawCCAssignment(int fl, int car)
{
	CPen* oldPen;
	CBrush* oldBrush;
	int colIndex, rowIndex;
	int posX, posY;//center
	CDC* pDC = this->GetDC();
	pDC->SetBkMode(TRANSPARENT);
	long radius,tX, tY;
	CPoint pt[3];
	int offset;

	tX = cellWidth/3;
	tY = cellHeight;
	if(tX > tY)
		radius = tY/2-2;
	else
		radius = tX/2-2;

	//colIndex = int(3*(car+1)/2)+2;
	//shin 20040817 mixed by both 1D and 2D
	if(Car[car].lowerOrUpper == LOWER_CAR) {
		colIndex = 1+3*(Car[car].i_hwy+1)-1;
		offset = -3;
	}
	else if(Car[car].lowerOrUpper == UPPER_CAR) {
		colIndex = 1+3*(Car[car].i_hwy+1)+1;
		offset = 2;
	}
	rowIndex = (Building.nUserFloors+4) - fl;
	posX = int(rectTable.left+(2*colIndex - 1)*cellWidth/2);
	posY = int(rectTable.top+(2*rowIndex - 1)*cellHeight/2);
	posX += cellWidth/3 + offset;

	oldPen=pDC->SelectObject(&penB);
	oldBrush=pDC->SelectObject(&brushY);
	
	pDC->Ellipse(posX-radius, posY-radius, posX+radius, posY+radius);

	pDC->SelectObject(&oldPen);
	pDC->SelectObject(&oldBrush);
	ReleaseDC(pDC);
}


//Passenger does not get on the car yet.
void CMy2DSimDlg::DrawCCAssignmentF(int fl, int car)
{
	CPen* oldPen;
	CBrush* oldBrush;
	int colIndex, rowIndex;
	int posX, posY;//center
	CDC* pDC = this->GetDC();
	pDC->SetBkMode(TRANSPARENT);
	long radius,tX, tY;
	CPoint pt[3];
	int offset;

	tX = cellWidth/3;
	tY = cellHeight;
	if(tX > tY)
		radius = tY/2-2;
	else
		radius = tX/2-2;

	//shin 20040818 mixed by both 1D and 2D
	if(Car[car].lowerOrUpper == LOWER_CAR) {
		colIndex = 1+3*(Car[car].i_hwy+1)-1;
		offset = -3;
	}
	else if(Car[car].lowerOrUpper == UPPER_CAR) {
		colIndex = 1+3*(Car[car].i_hwy+1)+1;
		offset = 2;
	}
	rowIndex = (Building.nUserFloors+4) - fl;
	posX = int(rectTable.left+(2*colIndex - 1)*cellWidth/2);
	posY = int(rectTable.top+(2*rowIndex - 1)*cellHeight/2);
	posX += cellWidth/3 + offset;

	oldPen=pDC->SelectObject(&penB);
	oldBrush=pDC->SelectObject(&brushW);
	
	pDC->Ellipse(posX-radius, posY-radius, posX+radius, posY+radius);

	pDC->SelectObject(&oldPen);
	pDC->SelectObject(&oldBrush);
	ReleaseDC(pDC);
}


void CMy2DSimDlg::DrawHCAssignment(int dir, int fl, int car)
{
	CPen* oldPen;
	CBrush* oldBrush;
	int colIndex, rowIndex;
	int posX, posY;//center
	CDC* pDC = this->GetDC();
	pDC->SetBkMode(TRANSPARENT);
	long radius,tX, tY;
	CPoint pt[3];
	int offset;

	tX = cellWidth/3;
	tY = cellHeight;
	if(tX > tY)
		radius = tY/2-2;
	else
		radius = tX/2-2;

	//colIndex = int(3*(car+1)/2)+2;
	//shin 20040817 mixed by both 1D and 2D
	if(Car[car].lowerOrUpper == LOWER_CAR) {
		colIndex = 1+3*(Car[car].i_hwy+1)-1;
		offset = 0;
	}
	else if(Car[car].lowerOrUpper == UPPER_CAR) {
		colIndex = 1+3*(Car[car].i_hwy+1)+1;
		offset = 5;
	}
	rowIndex = (Building.nUserFloors+4) - fl;
	posX = int(rectTable.left+(2*colIndex - 1)*cellWidth/2) + offset;
	posY = int(rectTable.top+(2*rowIndex - 1)*cellHeight/2);

	oldPen=pDC->SelectObject(&penB);
	
	if(dir == DN)
	{
		posX -= cellWidth/3;
		pt[0] = CPoint(posX, posY+radius);
		pt[1] = CPoint(posX-radius, posY-radius);
		pt[2] = CPoint(posX+radius, posY-radius);
		oldBrush=pDC->SelectObject(&brushR);
	}
	else if(dir == UP)
	{
		pt[0] = CPoint(posX - 2, posY-radius);
		pt[1] = CPoint(posX-radius - 2, posY+radius);
		pt[2] = CPoint(posX+radius - 2, posY+radius);
		oldBrush=pDC->SelectObject(&brushG);
	}
    
	pDC->SetPolyFillMode(WINDING);
	pDC->Polygon(pt, 3);

	pDC->SelectObject(&oldPen);
	pDC->SelectObject(&oldBrush);
	ReleaseDC(pDC);
}


// Draw bottom and top range indicators
void CMy2DSimDlg::DrawRange() {
	CPen* Pen;
	CBrush* Brush;
	CDC* pDC = this->GetDC();
	pDC->SetBkMode(TRANSPARENT);
	CPoint pt[3];

	int c, colIndex, rowIndex;
	int posX, posY, offset;

	for (c = 0; c < n_car; c++) {
		if (Car[c].lowerOrUpper == LOWER_CAR) {
			colIndex = 3*(Car[c].i_hwy+1);
			offset = -1;
		}
		else if (Car[c].lowerOrUpper == UPPER_CAR) {
			colIndex = 3*(Car[c].i_hwy+1) + 1;
			offset = 1;
		}
		posX = int(rectTable.left + colIndex*cellWidth) + offset;

// Bottom of range
		rowIndex = (Building.nUserFloors+4) - Car[c].range_bottom;
		posY = int(rectTable.top + (4*rowIndex - 1)*cellHeight/4);
		pt[0] = CPoint(posX, posY);
		pt[1] = CPoint(posX, posY + 3);
		pt[2] = CPoint(posX + 3*offset, posY);

		Pen = pDC->SelectObject(&penBu);
		Brush=pDC->SelectObject(&brushBu);
		pDC->SetPolyFillMode(WINDING);
		pDC->Polygon(pt, 3);

		pDC->SelectObject(&Pen);
		pDC->SelectObject(&Brush);

// Top of range
		rowIndex = (Building.nUserFloors+4) - Car[c].range_top;
		posY = int(rectTable.top + (4*rowIndex - 3)*cellHeight/4);
		pt[0] = CPoint(posX, posY);
		pt[1] = CPoint(posX, posY - 3);
		pt[2] = CPoint(posX + 3*offset, posY);

		Pen = pDC->SelectObject(&penBu);
		Brush=pDC->SelectObject(&brushBu);
		pDC->SetPolyFillMode(WINDING);
		pDC->Polygon(pt, 3);

		pDC->SelectObject(&Pen);
		pDC->SelectObject(&Brush);

	}  // for c
	ReleaseDC(pDC);
}  // DrawRange


void CMy2DSimDlg::DrawNCF()
{
	int car, fl;

	for(car=0; car<n_car; car++)
	{
		fl = DisplayedNCF(car);
		DrawNCFRect(fl, car);
	}
}


void CMy2DSimDlg::DrawNCFRect(int fl, int car)
{
	CPen* oldPen;
	CBrush* oldBrush;
	int colIndex, rowIndex;
	int posX, posY;//center
	CDC* pDC = this->GetDC();
	pDC->SetBkMode(TRANSPARENT);

	colIndex = Car[car].i_hwy*3+4;
	rowIndex = (Building.nUserFloors+4) - fl;
	posX = int(rectTable.left+(2*colIndex - 1)*cellWidth/2);
	posY = int(rectTable.top+(2*rowIndex - 1)*cellHeight/2);

	oldPen=pDC->SelectObject(&penG);
	oldBrush=pDC->SelectObject(&brushG);
	
	pDC->Rectangle(posX-cellWidth/3, posY-cellHeight/3, posX+cellWidth/3, posY+cellHeight/3);

	pDC->SelectObject(&oldPen);
	pDC->SelectObject(&oldBrush);
	ReleaseDC(pDC);
}


void CMy2DSimDlg::DrawTarget()
{
	int car;

	for(car=0; car<n_car; car++)
	{
		DrawTargetRect(Car[car].target, car);
	}
}


void CMy2DSimDlg::DrawTargetRect(int fl, int car)
{
	CPen* oldPen;
	CBrush* oldBrush;
	int colIndex, rowIndex;
	int posX, posY;//center
	CDC* pDC = this->GetDC();
	pDC->SetBkMode(TRANSPARENT);

	colIndex = Car[car].i_hwy*3 + 4;
	rowIndex = (Building.nUserFloors+4) - fl;
	posX = int(rectTable.left+(2*colIndex - 1)*cellWidth/2);
	posY = int(rectTable.top+(2*rowIndex - 1)*cellHeight/2);

	oldPen=pDC->SelectObject(&penY);
	oldBrush=pDC->SelectObject(&brushY);
	
	pDC->Rectangle(posX-cellWidth/4, posY-cellHeight/4, posX+cellWidth/4, posY+cellHeight/4);

	pDC->SelectObject(&oldPen);
	pDC->SelectObject(&oldBrush);
	ReleaseDC(pDC);
}


void CMy2DSimDlg::DrawState()
{
	CString str;
	int car;
	CPen* oldPen;
	int colIndex, rowIndex;
	int posX, posY;//center
	CDC* pDC = this->GetDC();
	pDC->SetBkMode(TRANSPARENT);
	
	// the number of passengers and car direction display
	for(car=0; car<n_car; car++)
	{
		colIndex = Car[car].i_hwy*3+4;
		rowIndex = car%2;
		posX = int(rectTable.left+(colIndex - 1)*cellWidth)+2;
		posY = int(rectTable.top+(1-rowIndex)*cellHeight)+2;
		
		oldPen=pDC->SelectObject(&penB);
		if(Car[car].dir == UP)
			str.Format("%d, U", Car[car].n_pax_in_car);
		else if(Car[car].dir == DN)
			str.Format("%d, D", Car[car].n_pax_in_car);
		else
			str.Format("%d, S", Car[car].n_pax_in_car);
		pDC->TextOut(posX, posY, str);
	}

	// NCF display
	for(car=0; car<n_car; car++)
	{
		colIndex = Car[car].i_hwy*3+3;
		rowIndex = car%2;
		posX = int(rectTable.left+(colIndex - 1)*cellWidth)+2;
		posY = int(rectTable.top+(1-rowIndex)*cellHeight)+2;
		
		oldPen=pDC->SelectObject(&penB);
		str.Format("%3d", USERFLOOR(Car[car].NCF));
		//str.Format("aa %d", car);
		pDC->TextOut(posX, posY, str);
	}

	// Current position display
	for(car=0; car<n_car; car++)
	{
		colIndex = Car[car].i_hwy*3+5;
		rowIndex = car%2;
		posX = int(rectTable.left+(colIndex - 1)*cellWidth)+2;
		posY = int(rectTable.top+(1-rowIndex)*cellHeight)+2;
		
		oldPen=pDC->SelectObject(&penB);
		str.Format("%4.1f", Car[car].currentPos);
		//str.Format("bb %d", car);
		pDC->TextOut(posX, posY, str);
	}

	pDC->SelectObject(&oldPen);
	ReleaseDC(pDC);
}


void CMy2DSimDlg::UpdateText()
{
	CString str, deboard_board_str;
	int f, n_deboard, n_board, max_board;

	str.Format("Current Time:%6.3f",Sys.t_nextEvent);
	m_CurrentTime = str;
	str.Format("Most Recent Passenger: #%d At %6.3f From %d To %d",
		i_pax_recent + 1,Pax[i_pax_recent].t_arrival, USERFLOOR(Pax[i_pax_recent].f_origin),
		USERFLOOR(Pax[i_pax_recent].f_dest));
	m_CurrentPassenger = str;

	//for debug
	if (Sys.y_nextEvent == 1 && Sys.i_nextEvent >= 0 && Sys.i_nextEvent < n_pax) {
		if (Pax[Sys.i_nextEvent].assignment >= 0)
	        str.Format("t=%5.3lf: new passenger %d from %d to %d on car %d",
	            Sys.t_nextEvent, Sys.i_nextEvent+1, USERFLOOR(Pax[Sys.i_nextEvent].f_origin),
		        USERFLOOR(Pax[Sys.i_nextEvent].f_dest), Pax[Sys.i_nextEvent].assignment);
		else
	        str.Format("t=%5.3lf: new passenger %d from %d to %d",
	            Sys.t_nextEvent, Sys.i_nextEvent+1, USERFLOOR(Pax[Sys.i_nextEvent].f_origin),
		        USERFLOOR(Pax[Sys.i_nextEvent].f_dest));
	}
	else if (Sys.y_nextEvent == 2 && Sys.i_nextEvent >= 0 && Sys.i_nextEvent < n_move)
	    str.Format("t=%5.3lf: hide/unhide request %d for car %d to %d until %5.3lf",
	        Sys.t_nextEvent, Sys.i_nextEvent+1, Move[Sys.i_nextEvent].car,
		    USERFLOOR(Move[Sys.i_nextEvent].destination), Move[Sys.i_nextEvent].t_release);
	else if (Sys.y_nextEvent == 3 && Sys.i_nextEvent >= 0 && Sys.i_nextEvent < n_car) {
		switch (Car[Sys.i_nextEvent].oper) {
		case IDLE:
	        str.Format("t=%5.3lf: car %d to idle", Sys.t_nextEvent, Sys.i_nextEvent);
			break;
		case OPENING:
		case DWELL:
			f = Car[Sys.i_nextEvent].floor;
			n_deboard = Car[Sys.i_nextEvent].n_CCall[f] + Car[Sys.i_nextEvent].n_deboard;
			if (Car[Sys.i_nextEvent].dir == 1)
				n_board = Car[Sys.i_nextEvent].n_UpCall[f];
			else if (Car[Sys.i_nextEvent].dir == -1)
				n_board = Car[Sys.i_nextEvent].n_DnCall[f];
			else
				n_board = 0;
			max_board = Param.capacity - Car[Sys.i_nextEvent].n_pax_in_car + n_deboard;
			if (n_board > max_board)
				n_board = max_board;

			if (n_deboard == 0 && n_board == 0)
			    deboard_board_str.Format("");
			else if (n_deboard == 0)
			    deboard_board_str.Format(" (+%d)", n_board);
			else if (n_board == 0)
			    deboard_board_str.Format(" (-%d)", n_deboard);
			else 
			    deboard_board_str.Format(" (-%d +%d)", n_deboard, n_board);

			if (Car[Sys.i_nextEvent].oper == OPENING)
	            str.Format("t=%5.3lf: car %d opening doors until %5.3lf%s",
				    Sys.t_nextEvent, Sys.i_nextEvent, Car[Sys.i_nextEvent].t_end_oper,
					deboard_board_str);
			else
	            str.Format("t=%5.3lf: car %d doors fully open until %5.3lf%s",
				    Sys.t_nextEvent, Sys.i_nextEvent, Car[Sys.i_nextEvent].t_end_oper,
					deboard_board_str);
			break;
		case CLOSING:
	        str.Format("t=%5.3lf: car %d doors closing until %5.3lf", Sys.t_nextEvent,
				Sys.i_nextEvent, Car[Sys.i_nextEvent].t_end_oper);
			break;
		case RUNNING:
	        str.Format("t=%5.3lf: car %d running to %d until %5.3lf", Sys.t_nextEvent,
				Sys.i_nextEvent, USERFLOOR(Car[Sys.i_nextEvent].target),
				Car[Sys.i_nextEvent].t_end_oper);
			break;
		case FORCED_MOVE:
	        str.Format("t=%5.3lf: car %d hiding to %d until %5.3lf", Sys.t_nextEvent,
				Sys.i_nextEvent, USERFLOOR(Car[Sys.i_nextEvent].target),
				Car[Sys.i_nextEvent].t_end_oper);
			break;
		case FORCED_PARKING:
	        str.Format("t=%5.3lf: car %d holding at %d until %5.3lf", Sys.t_nextEvent,
				Sys.i_nextEvent, USERFLOOR(Car[Sys.i_nextEvent].floor),
				Car[Sys.i_nextEvent].t_end_oper);
			break;
		case WAIT_INITIATE:
	        str.Format("t=%5.3lf: car %d waiting for initiation at %d until %5.3lf", Sys.t_nextEvent,
				Sys.i_nextEvent, USERFLOOR(Car[Sys.i_nextEvent].floor));
			break;
		case WAIT_RELEASE:
	        str.Format("t=%5.3lf: car %d waiting for release at %d until %5.3lf", Sys.t_nextEvent,
				Sys.i_nextEvent, USERFLOOR(Car[Sys.i_nextEvent].floor));
			break;
		case WAIT_CAR_RANGE:
	        str.Format("t=%5.3lf: car %d waiting for car range to change", Sys.t_nextEvent,
				Sys.i_nextEvent);
			break;
		case MOVE_CAR_RANGE:
	        str.Format("t=%5.3lf: car %d run to car range limit at %d until %5.3lf", Sys.t_nextEvent,
				Sys.i_nextEvent, USERFLOOR(Car[Sys.i_nextEvent].target),
				Car[Sys.i_nextEvent].t_end_oper);
			break;

		default:  // not expected to be necessary
	        str.Format("t=%5.3lf: car %d in op mode %d until %5.3lf", Sys.t_nextEvent,
				Sys.i_nextEvent, Car[Sys.i_nextEvent].oper, Car[Sys.i_nextEvent].t_end_oper);
			break;
		}
	}
	else if (Sys.y_nextEvent == -1)
		str.Format("t=%5.3lf: start of simulation", Sys.t_nextEvent);
	else          // not expected to be necessary
	    str.Format("Event Type(%d) Index(%d) Time(%f)", Sys.y_nextEvent,Sys.i_nextEvent,Sys.t_nextEvent);

	m_debug = str;
	UpdateData(FALSE);
}  // UpdateText


void CMy2DSimDlg::DebugFloorShow()
{
	m_0 = DebugStr(0);
	m_1 = DebugStr(1);
	m_2 = DebugStr(2);
	m_3 = DebugStr(3);
	m_4 = DebugStr(4);
	m_5 = DebugStr(5);
	m_6 = DebugStr(6);
	m_7 = DebugStr(7);
}


CString CMy2DSimDlg::DebugStr(int c)
{
	CString str;

	if (Car[c].oper != IDLE)
		str.Format("%5.3f",Car[c].t_end_oper);
	else
		str.Format("-");
	if(m_bAssigned)
		return str;
	else
		return str+m_strEstimatedTime[c];
}


BOOL CMy2DSimDlg::DestroyWindow() 
{
	DeleteDC();
	DeleteBitmap();
	fclose( f_ErrorLog);
	DetachDispatcher();
	delete m_pShowLoadDlg;
	
	return CDialog::DestroyWindow();
}


void CMy2DSimDlg::LoadBitmap()
{
	bmpDNUPCC.LoadBitmap(IDB_BITMAP_DNUPCC);
	bmpDNUP.LoadBitmap(IDB_BITMAP_DNUP);
	bmpDNCC.LoadBitmap(IDB_BITMAP_DNCC);
	bmpDN.LoadBitmap(IDB_BITMAP_DN);
	bmpUPCC.LoadBitmap(IDB_BITMAP_UPCC);
	bmpUP.LoadBitmap(IDB_BITMAP_UP);
	bmpCC.LoadBitmap(IDB_BITMAP_CC);
}


void CMy2DSimDlg::DeleteBitmap()
{
	bmpDNUPCC.DeleteObject();
	bmpDNUP.DeleteObject();
	bmpDNCC.DeleteObject();
	bmpDN.DeleteObject();
	bmpUPCC.DeleteObject();
	bmpUP.DeleteObject();
	bmpCC.DeleteObject();
}


void CMy2DSimDlg::PrepareDC()
{
	//Create CPen Objects  --------------------------------------------------------------------
	penB0.      CreatePen(PS_SOLID, 0,  BLACK);
	penB.      CreatePen(PS_SOLID, 1,  BLACK);
	penBp.     CreatePen(PS_SOLID, 5, BLACK);  //For Printer Device Context
	penW.      CreatePen(PS_SOLID, 1,  WHITE);
	penBW.     CreatePen(PS_SOLID, 1,  BRIGHTWHITE);	
	penB_dash. CreatePen(PS_DASH,  1,  BLACK);
	penB_dot.  CreatePen(PS_DOT,   1,  BLACK);
	penGy.     CreatePen(PS_SOLID, 1,  GRAY);
	penGyp.    CreatePen(PS_SOLID, 5, GRAY);   //For Printer Device Context
	penDGy.    CreatePen(PS_SOLID, 1,  RGB(130,130,130));
	penDGyp.   CreatePen(PS_SOLID, 5, RGB(130,130,130));  //For Printer Device Context
	penLGy.    CreatePen(PS_SOLID, 1,  LIGHTGRAY);
	penDGy_d.  CreatePen(PS_DOT,   1,  DARKGRAY);
	penDGy_d2. CreatePen(PS_DOT,   5,  DARKGRAY);
	penGy_d.   CreatePen(PS_DOT,   1,  GRAY);
	penGy_dp.  CreatePen(PS_DOT,   5,  GRAY);
	penBu.     CreatePen(PS_SOLID, 1,  BLUE);
	penBu2.    CreatePen(PS_SOLID, 2,  BLUE);
	penBup.    CreatePen(PS_SOLID, 5, BLUE);   //For Printer Device Context
	penBu2p.   CreatePen(PS_SOLID, 10, BLUE);   //For Printer Device Context
	penLBu.    CreatePen(PS_SOLID, 1,  LIGHTBLUE);
	penDBu.    CreatePen(PS_SOLID, 1,  DARKBLUE);
	penDBup.   CreatePen(PS_SOLID, 5,  DARKBLUE);  //For Printer Device Context
	penG.      CreatePen(PS_SOLID, 1,  GREEN);
	penDG.     CreatePen(PS_SOLID, 1,  DARKGREEN);
	penDGp.    CreatePen(PS_SOLID, 5, DARKGREEN);
	penR.      CreatePen(PS_SOLID, 1,  RED);
	penLR.     CreatePen(PS_SOLID, 1,  LIGHTRED);
	penDR.     CreatePen(PS_SOLID, 1,  DARKRED);
	penDRp.    CreatePen(PS_SOLID, 5, DARKRED);  //For Printer Device Context
	penY.      CreatePen(PS_SOLID, 1,  YELLOW);
	penLM.     CreatePen(PS_SOLID, 1,  LIGHTMAGENTA);
	penLMp.    CreatePen(PS_SOLID, 5, LIGHTMAGENTA);
	penLY.     CreatePen(PS_SOLID, 1,  LIGHTYELLOW);
	penDGB.    CreatePen(PS_SOLID, 1,  DARKGRAYBLUE);
	penLGB.    CreatePen(PS_SOLID, 1,  LIGHTGRAYBLUE);
	penO.      CreatePen(PS_SOLID, 1,  ORANGE);
    penP2.     CreatePen(PS_SOLID, 1,  PURPLE2);
    penP2p.    CreatePen(PS_SOLID, 5, PURPLE2);
    penMSK.    CreatePen(PS_SOLID, 1,  MASK_COLOR);
	penG_d.    CreatePen(PS_DASH,  1,  GREEN);
	penG_dp.   CreatePen(PS_DASH,  5, GREEN);  //For Printer Device Context
	penR_d.    CreatePen(PS_DOT,   1,  RED);
	penR_dp.   CreatePen(PS_DOT,   5, RED);    //For Printer Device Context
	penC.      CreatePen(PS_SOLID, 1,  BLUE);
	penCp.     CreatePen(PS_SOLID, 5, BLUE);   //For Printer Device Context
	penDG_dot. CreatePen(PS_DOT,   1,  DARKGREEN);
	penDG_dotp.CreatePen(PS_DASH,  5, DARKGREEN);  //For Printer Device Context

	//Create CBrush Objects  ------------------------------------------------------------------
	brushR.  CreateSolidBrush(RED);              //red
	brushLR. CreateSolidBrush(LIGHTRED);         //light red
	brushLLR.CreateSolidBrush(LIGHTLRED);        //more light red
	brushG.  CreateSolidBrush(GREEN);            //green
	brushLG. CreateSolidBrush(LIGHTGREEN);       //light green
	brushBu. CreateSolidBrush(BLUE);             //blue
//	brushBu. CreateSolidBrush(CAFEBLUE);         //blue   -- modified (AH)
	brushLBu.CreateSolidBrush(LIGHTBLUE);        //light blue
	brushB.  CreateSolidBrush(BLACK);            //black
	brushW.  CreateSolidBrush(WHITE);            //white
	brushBW. CreateSolidBrush(BRIGHTWHITE);      //white
	brushLY. CreateSolidBrush(LIGHTYELLOW);      //lightyellow
	brushY.  CreateSolidBrush(YELLOW);           //yellow
	brushGy. CreateSolidBrush(GRAY);             //gray
	brushLV. CreateSolidBrush(LIGHTPURPLE);      //light violet
	brushV.  CreateSolidBrush(PURPLE);           //violet
	brushLM. CreateSolidBrush(LIGHTMAGENTA);     //light magenta
	brushDR. CreateSolidBrush(DARKRED);          //dark red
	brushDBu.CreateSolidBrush(RGB( 60, 60,205)); //dark blue
	brushDGy.CreateSolidBrush(RGB(130,130,130)); //dark gray
	brushLGy.CreateSolidBrush(RGB(210,210,210)); //light gray
	brushDGB.CreateSolidBrush(DARKGRAYBLUE);     //dark gray blue
	brushLGB.CreateSolidBrush(LIGHTGRAYBLUE);    //light gray blue
	brushO.  CreateSolidBrush(ORANGE);           //yellow
	brushP2. CreateSolidBrush(PURPLE2);          //purple
	brushMSK.CreateSolidBrush(MASK_COLOR);       //MASK COLOR
	brushDN. CreateSolidBrush(RGB(255,100,100)); //DOWN COLOR of SUMMARY
	brushLDN.CreateSolidBrush(RGB(255,120,120)); //DOWN COLOR of SUMMARY
	brushWINDOWCOLOR.CreateSolidBrush(WINDOWCOLOR); //WINDOW COLOR
}


void CMy2DSimDlg::DeleteDC()
{
	//Delete CPen Objects  --------------------------------------------------------------------
	penB0.      DeleteObject();
	penB.      DeleteObject();
	penBp.     DeleteObject();  //For Printer Device Context
	penW.      DeleteObject();
	penBW.     DeleteObject();
	penB_dash. DeleteObject();
	penB_dot.  DeleteObject();
	penGy.     DeleteObject();
	penDGy.    DeleteObject();
	penDGyp.   DeleteObject();  //For Printer Device Context
	penLGy.    DeleteObject();
	penDGy_d.  DeleteObject();
	penDGy_d2. DeleteObject();
	penGy_d.   DeleteObject();
	penGy_dp.  DeleteObject();
	penBu.     DeleteObject();
	penBu2.    DeleteObject();
	penBup.    DeleteObject();  //For Printer Device Context
	penBu2p.   DeleteObject();  //For Printer Device Context
	penLBu.    DeleteObject();
	penDBu.    DeleteObject();
	penDBup.   DeleteObject();  //For Printer Device Context
	penG.      DeleteObject();
	penDG.     DeleteObject();
	penDGp.    DeleteObject();
	penR.      DeleteObject();
	penLR.     DeleteObject();
	penDR.     DeleteObject();
	penDRp.    DeleteObject();  //For Printer Device Context
	penY.      DeleteObject();
	penLM.     DeleteObject();
	penLMp.    DeleteObject();
	penLY.     DeleteObject();
	penDGB.    DeleteObject();
	penLGB.    DeleteObject();
	penO.      DeleteObject();
	penP2.     DeleteObject();
	penP2p.    DeleteObject();
	penMSK.    DeleteObject();
	penG_d.    DeleteObject();
	penG_dp.   DeleteObject();  //For Printer Device Context
	penR_d.    DeleteObject();
	penR_dp.   DeleteObject();  //For Printer Device Context
	penC.      DeleteObject();
	penCp.     DeleteObject();  //For Printer Device Context
	penDG_dot. DeleteObject();
	penDG_dotp.DeleteObject();  //For Printer Device Context

	//Delete CBrush Objects  ------------------------------------------------------------------
	brushR.  DeleteObject();
	brushLR. DeleteObject();
	brushLLR.DeleteObject();
	brushG.  DeleteObject();
	brushLG. DeleteObject();
	brushBu. DeleteObject();
	brushLBu.DeleteObject();
	brushB.  DeleteObject();
	brushW.  DeleteObject();
	brushBW. DeleteObject();
	brushLY. DeleteObject();
	brushY.  DeleteObject();
	brushGy. DeleteObject();
	brushLV. DeleteObject();
	brushV.  DeleteObject();
	brushLM. DeleteObject();
	brushDR. DeleteObject();
	brushDBu.DeleteObject();
	brushDGy.DeleteObject();
	brushLGy.DeleteObject();
	brushDGB.DeleteObject();
	brushLGB.DeleteObject();
	brushO.  DeleteObject();
	brushP2. DeleteObject();
	brushMSK.DeleteObject();
	brushWINDOWCOLOR.DeleteObject();

	//Pattern Brush
	brushBu_b. DeleteObject();
	brushBu_bp.DeleteObject();  //For Printer Device Context
	brushUP.   DeleteObject();
	brushUPp.  DeleteObject();  //For Printer Device Context
	brushLUP.  DeleteObject();
	brushLUPp. DeleteObject();  //For Printer Device Context
	brushDN.   DeleteObject();
	brushLDN.  DeleteObject();
}


void CMy2DSimDlg::OnButtonNextPassenger() 
{
	if(!m_trafficProfileOpened)
	{
		MessageBox("Please open the passenger profile!");
		return;
	}

	if (!ProfileCheckPassed)
	{
		if (ProfileCarsForCheck>Building.nCars || ProfileFloorsForCheck>Building.nTotalFloors)
		{ // profile configuration is less than user configuration
			if (MessageBox("Traffic Profile Cars or Floors are larger than user configuration, so simulation can be executed abnormally. Are you OK to Proceed?",
				"Warning of mismatching of configuration",MB_ICONWARNING | MB_YESNO )==IDNO)
				return;
		}
		ProfileCheckPassed=TRUE;
	}
	bEnableCollisionMessageBox=TRUE;

	while(!m_bSimulationEnd)
	{
		ProcessNextEvent(); 
		if(Sys.y_nextEvent == 1)
			return;
	}
}

BOOL CMy2DSimDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (m_hAccel != NULL) 
	{ 
		if (TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) 
			return TRUE; 
	} 
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CMy2DSimDlg::PressNextEventButton()
{
	if(!m_bMsgDisable)
		MessageBox("Please press NextEvent button!");
}

void CMy2DSimDlg::OnButtonAddMove() 
{
	CHideUnhide dlgAddMove(NULL, Sys.t_nextEvent);
	int i,j,dest;
	
	if ( dlgAddMove.DoModal() == IDOK )
	{
		if (dlgAddMove.m_initiateTime + EPSILON < Sys.t_nextEvent)
			MessageBox("Warning: the initiate time is earlier than the current time.");
		if (dlgAddMove.m_releaseTime + EPSILON < dlgAddMove.m_initiateTime)
			dlgAddMove.m_releaseTime = dlgAddMove.m_initiateTime;

		dest = dlgAddMove.m_iDestination - Param.f_offset;
		if (dest < 0) {
			MessageBox("Resetting destination floor (too low).");
			dest = 0;
		}
		else if (dest >= n_floor) {
			MessageBox("Resetting destination floor (too high).");
			dest = n_floor - 1;
		}

// Insert move into Move data structure

		for(i=0; i<n_move; i++)
		{
			if(dlgAddMove.m_initiateTime<Move[i].t_initiation)
				break;
		}
		n_move++;
		for(j=n_move-1; j>i; j--)
		{
			Move[j].car = Move[j-1].car;
			Move[j].destination = Move[j-1].destination;
			Move[j].min_dwell = Move[j-1].min_dwell;
			Move[j].t_initiation = Move[j-1].t_initiation;
			Move[j].initiation_basis = Move[j-1].initiation_basis;
			Move[j].initiation_threshold = Move[j-1].initiation_threshold;
			Move[j].initiation_relativity = Move[j-1].initiation_relativity;
			Move[j].initiation_direction = Move[j-1].initiation_direction;
			Move[j].initiation_delay = Move[j-1].initiation_delay;
			Move[j].t_release = Move[j-1].t_release;
			Move[j].release_basis = Move[j-1].release_basis;
			Move[j].release_threshold = Move[j-1].release_threshold;
			Move[j].release_relativity = Move[j-1].release_relativity;
			Move[j].release_direction = Move[j-1].release_direction;
			Move[j].release_delay = Move[j-1].release_delay;
		}
		Move[i].car = dlgAddMove.m_iCar;
		Move[i].destination = dest;
		Move[i].min_dwell = dlgAddMove.m_bHoldDoorOpen;
		Move[i].t_initiation = dlgAddMove.m_initiateTime;
		Move[i].initiation_basis = dlgAddMove.m_IBasis;
		Move[i].initiation_threshold = dlgAddMove.m_IThereshold;
		Move[i].initiation_relativity = dlgAddMove.m_IRelativity;
		Move[i].initiation_direction = dlgAddMove.m_IDirection;
		Move[i].initiation_delay = dlgAddMove.m_IDelay;
		Move[i].t_release = dlgAddMove.m_releaseTime;
		Move[i].release_basis = dlgAddMove.m_RBasis;
		Move[i].release_threshold = dlgAddMove.m_RThreshold;
		Move[i].release_relativity = dlgAddMove.m_RRelativity;
		Move[i].release_direction = dlgAddMove.m_RDirection;
		Move[i].release_delay = dlgAddMove.m_RDelay;

		SendMove(Move[i],MOVE_ADD);

		if (i < i_move) {  // retroactive move
			MessageBox("Setting initiate time less than current time can be dangerous.  "
			"Consider playback by saving and jumping to current time.");
			i_move = i;
		}

// Special code to disable UpdateTarget until this move is processed first
		if (dlgAddMove.m_initiateTime < Sys.t_nextEvent + EPSILON)
			Car[Move[i].car].pending_move = 2;
	}
}  // OnButtonAddMove

//Remake opened traffic profile with assignments and moves.
void CMy2DSimDlg::RemakeTrafficProfile()
{
	CString strSaveName;
	
	C2DFileIO fsave;
	int retval;

	strSaveName.Format("_%3.3d.2dp",i_pax);

	retval=fsave.Save2DPFile(m_strTrafficProfileName+strSaveName,&FileMemo,&Building,&Param,Pax,Move,n_pax,n_move);
	if (retval==0)
	{
		//MessageBox("File Save Success");
	}
	else
	{
		MessageBox("File Save Error");
	}
}


void CMy2DSimDlg::AssignPostManage()
{
	m_bAssigned = TRUE;
	m_bNeedAssign = FALSE;
	m_bReassigned = TRUE;
	HandleNewPassenger(Sys.i_nextEvent);
	Invalidate(TRUE);
}

BOOL CMy2DSimDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	//cs.style	&= ~WS_MAXIMIZEBOX;
	cs.style	&= ~(WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME);
	
	
	if( !CDialog::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
	//return CDialog::PreCreateWindow(cs);
}
/*
{
	cs.style	&= ~(WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME);
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}
*/

void CMy2DSimDlg::OnButtonJump() 
{
	CJumpToTime dlgJumpTime;
	double jumpTime;
	int indexOfPassenger;
	int jumpKind;
	

	if (!ProfileCheckPassed)
	{
		if (ProfileCarsForCheck>Building.nCars || ProfileFloorsForCheck>Building.nTotalFloors)
		{ // profile configuration is less than user configuration
			if (MessageBox("Traffic Profile Cars or Floors are larger than user configuration, so simulation can be executed abnormally. Are you OK to Proceed?",
				"Warning of mismatching of configuration",MB_ICONWARNING | MB_YESNO )==IDNO)
				return;
		}
		ProfileCheckPassed=TRUE;
	}	

	if ( dlgJumpTime.DoModal() == IDOK )
	{
		jumpTime = dlgJumpTime.m_jumpTime;
		indexOfPassenger = dlgJumpTime.m_indexOfPassenger;
		jumpKind = dlgJumpTime.m_jumpKind;
		
		if(!m_trafficProfileOpened)
		{
			MessageBox("Please open the passenger profile!");
			return;
		}
		bEnableCollisionMessageBox=TRUE; 
		if(jumpKind == 0)
		{
			while(!m_bSimulationEnd && Sys.t_nextEvent<jumpTime)
			{
				ProcessNextEvent(); 
				if(m_bNeedAssign)
					return;
			}
		}
		else if(jumpKind == 1)
		{
			while(!m_bSimulationEnd && i_pax<indexOfPassenger)
			{
				ProcessNextEvent(); 
				if(m_bNeedAssign)
					return;
			}
		}
	}
}

void CMy2DSimDlg::OnButtonSaveTrafficProfile() 
{
	BOOL bRet;

	if(!m_trafficProfileOpened)
	{
		MessageBox("Please open the passenger profile and simulate it!");
		return;
	}
	bRet = MessageBox("Do you want save your assignments?", "Save Traffic Profile", MB_YESNO);
	if(bRet == IDYES)
	{
		RemakeTrafficProfile();
		RecordData();
	}
}

BOOL CMy2DSimDlg::CollisionCheck()
{
	int dummy, UCarFloor, DCarFloor, diff, UCarNCF, DCarNCF;
	int iHoistway; // hoistway index

	for(iHoistway = 0; iHoistway < Building.nHoistways; iHoistway++)
	{
		if(Hwy[iHoistway].n_car_in_hwy < 2)//Check the number of cars in the hoistway
			continue;
		UCarFloor = GetDisplayedPosition(Hwy[iHoistway].i_car+1);
		DCarFloor = GetDisplayedPosition(Hwy[iHoistway].i_car);
		//If one car is at pit or overhead, then there is no message for collision.
		if(((DCarFloor == 0) && Building.pit) || ((UCarFloor == (n_floor-1)) && Building.overhead))
		{
			continue;
		}
		diff = UCarFloor - DCarFloor;
		if(diff < Param.AdjacentLimit) //shin 041006
		{
			CString str;
			str.Format("Collision(Floor): Car(%d, %d) is (%d, %d) at %3.2f!",
				Hwy[iHoistway].i_car, Hwy[iHoistway].i_car+1,USERFLOOR(DCarFloor),USERFLOOR(UCarFloor),Sys.t_nextEvent);
			if (bEnableCollisionMessageBox==TRUE) 
			{
#ifdef SHOW_COLLISION_WARNING
				MessageBox(str);
#endif
				bEnableCollisionMessageBox=FALSE;
			}
			DisplayWarningList(str); 
		}

		UCarNCF = GetNextCommittableFloor(Hwy[iHoistway].i_car+1, &dummy, 0);
		DCarNCF = GetNextCommittableFloor(Hwy[iHoistway].i_car, &dummy, 0);
		diff = UCarNCF - DCarNCF;
		if(diff < Param.AdjacentLimit) //shin 041006
		{
			CString str;
			str.Format("Collision(NCF): Car(%d, %d) is (%d, %d) at %3.2f!",
				Hwy[iHoistway].i_car, Hwy[iHoistway].i_car+1, USERFLOOR(DCarNCF),USERFLOOR(UCarNCF),Sys.t_nextEvent);
			if (bEnableCollisionMessageBox==TRUE) 
			{
#ifdef SHOW_COLLISION_WARNING
				MessageBox(str);
#endif
				bEnableCollisionMessageBox=FALSE;
			}
			DisplayWarningList(str); 
		}

	}
	return TRUE;
}
void CMy2DSimDlg::ShowEstimatedArrivalTime()
{
	int car;
	int targetFl;
	int targetDir;
	double currentTime;
	double estimatedTime[MAXCAR];
	
	if(m_bAssigned)
	{
		for(car=0; car<n_car; car++)
		{
			m_strEstimatedTime[car].Format("");
		}
		return;
	}
	targetFl = Pax[Sys.i_nextEvent].f_origin;
	if(targetFl < Pax[Sys.i_nextEvent].f_dest)
		targetDir = UP;
	else
		targetDir = DN;
	
	currentTime = Sys.t_nextEvent;
	for(car=0; car<n_car; car++)
	{
		estimatedTime[car] = CalculateArrivalTime(car, targetFl, targetDir); //+currentTime
		m_strEstimatedTime[car].Format("\t%6.3f", estimatedTime[car]);
	}
}

// Calculate roughly estimated arrival time for new call with fl and dir
// To get more exact calculation, correct this function.
// Consider blocking condition.
// Should consider current door status
// Minus stopTime for already assigned call
double CMy2DSimDlg::CalculateArrivalTime(int car, int fl, int dir)
{
	int firstDir, scanDir;
	int scanFl, scannedFl, nextCommitFloor;
	double retTime;
	double defaultDwellTime = 2.6;
	double stopTime = 9.4-4; //9.4: flight time for 4 floors, 4; full speed flight time for 4 floors
	//double startDelayTime = 0.75;//by spec
	//double stopDelayTime = 0.65;//by spec
	double minAccelTime = 1.5;//2.0-0.5
	double minDecelTime = 1.5;//2.0-0.5
	double runTime = 0.0;
	double openTime;
	double closeTime;
	double openCloseTime;
	BOOL oneScan = FALSE;
	int dummy;

	nextCommitFloor = GetNextCommittableFloor(car, &dummy, 0);
	scannedFl = nextCommitFloor;
	retTime = 0.0;

	if(Car[car].dir == NO_DIR)
		oneScan = TRUE;
	if(Car[car].dir == UP)
		firstDir = UP;
	else if(Car[car].dir == DN)
		firstDir = DN;
	else if(nextCommitFloor < fl)
		firstDir = UP;
	else 
		firstDir = DN;

	//Car is on the same fl with same dir or NO_DIR
	if(((Car[car].dir == dir) || oneScan) && (nextCommitFloor == fl))
		return retTime;

	openTime = Param.door_open;
	closeTime = Param.door_close;
	openCloseTime = openTime + closeTime;
	if(firstDir == UP)
	{
		scanDir = UP;
		//Calculate current floor
		if(((Car[car].n_UpCall[nextCommitFloor] > 0) || (Car[car].n_UpStop[nextCommitFloor] > 0))
			&& ((Car[car].oper == IDLE) || (Car[car].oper == RUNNING) ||
			(Car[car].oper == MOVE_CAR_RANGE)))
			retTime += Car[car].t_end_oper - Sys.t_nextEvent
			+ DwellTime(nextCommitFloor, Car[car].n_UpCall[nextCommitFloor], Car[car].n_UpStop[nextCommitFloor])
			+ openCloseTime;
		else if(Car[car].oper == OPENING)
			retTime += Car[car].t_end_oper - Sys.t_nextEvent + closeTime + DwellTime(nextCommitFloor, Car[car].n_UpCall[nextCommitFloor], Car[car].n_UpStop[nextCommitFloor]);
		else if(Car[car].oper == DWELL)
			retTime += Car[car].t_end_oper - Sys.t_nextEvent + closeTime;
		else if(Car[car].oper == CLOSING)
			retTime += 0.0;
		
		//check whether nextCommitFloor is top or not
		for(scanFl = nextCommitFloor+1; scanFl<n_floor; scanFl++)
		{
			if(((scanDir == dir) || oneScan) && (scanFl == fl))
				return (retTime + GetCommittableTime(car, scannedFl, scanFl));

			//if the car has hall calls or
			//if the car has future car calls
			if((Car[car].n_UpCall[scanFl] > 0) || (Car[car].n_UpStop[scanFl] > 0))
			{
				runTime = GetRunTime(car, scannedFl, scanFl);
				scannedFl = scanFl;
				retTime += DwellTime(scanFl, Car[car].n_UpCall[scanFl], Car[car].n_UpStop[scanFl]) + openCloseTime + runTime;
			}
		}

		if(Car[car].n_DnCall[scannedFl] > 0)
		{
			retTime += DwellTime(scannedFl, Car[car].n_DnCall[scannedFl], Car[car].n_UpStop[scannedFl])
				- DwellTime(scannedFl, Car[car].n_UpCall[scannedFl], Car[car].n_UpStop[scannedFl]);
		}

		scanDir = DN;
		for(scanFl = scannedFl-1; scanFl>0; scanFl--)
		{
			if(((scanDir == dir) || oneScan) && (scanFl == fl))
				return (retTime + GetCommittableTime(car, scannedFl, scanFl));
			if((Car[car].n_DnCall[scanFl] > 0) || (Car[car].n_DnStop[scanFl] > 0))
			{
				runTime = GetRunTime(car, scannedFl, scanFl);
				scannedFl = scanFl;
				retTime += DwellTime(scanFl, Car[car].n_DnCall[scanFl], Car[car].n_DnStop[scanFl]) + openCloseTime + runTime;
			}
		}

		if(Car[car].n_UpCall[scannedFl] > 0)
		{
			retTime += DwellTime(scannedFl, Car[car].n_UpCall[scannedFl], Car[car].n_UpStop[scannedFl])
				- DwellTime(scannedFl, Car[car].n_DnCall[scannedFl], Car[car].n_UpStop[scannedFl]);
		}

		scanDir = UP;
		for(scanFl = scannedFl+1; scanFl<nextCommitFloor; scanFl++)
		{
			if(((scanDir == dir) || oneScan) && (scanFl == fl))
				return (retTime + GetCommittableTime(car, scannedFl, scanFl));

			//if the car has hall calls or
			//if the car has future car calls
			if((Car[car].n_UpCall[scanFl] > 0) || (Car[car].n_UpStop[scanFl] > 0))
			{
				runTime = GetRunTime(car, scannedFl, scanFl);
				scannedFl = scanFl;
				retTime += DwellTime(scanFl, Car[car].n_UpCall[scanFl], Car[car].n_UpStop[scanFl]) + openCloseTime + runTime;
			}
		}
		
		return retTime;
	}
	else
	{
		scanDir = DN;
			//Calculate current floor
		if(((Car[car].n_DnCall[nextCommitFloor] > 0) || (Car[car].n_DnStop[nextCommitFloor] > 0)) &&
			((Car[car].oper == IDLE) || (Car[car].oper == RUNNING) || (Car[car].oper == MOVE_CAR_RANGE)))
			retTime +=  Car[car].t_end_oper - Sys.t_nextEvent
			+ DwellTime(nextCommitFloor, Car[car].n_DnCall[nextCommitFloor], Car[car].n_DnStop[nextCommitFloor])
			+ openCloseTime;
		else if(Car[car].oper == OPENING)
			retTime += Car[car].t_end_oper - Sys.t_nextEvent + closeTime + defaultDwellTime;
		else if(Car[car].oper == DWELL)
			retTime += Car[car].t_end_oper - Sys.t_nextEvent + closeTime;
		else if(Car[car].oper == CLOSING)
			retTime += 0.0;
		
		//check whether nextCommitFloor is bottom or not
		for(scanFl = nextCommitFloor-1; scanFl>0; scanFl--)
		{
			if(((scanDir == dir) || oneScan) && (scanFl == fl))
				return (retTime + GetCommittableTime(car, scannedFl, scanFl));
			if((Car[car].n_DnCall[scanFl] > 0) || (Car[car].n_DnStop[scanFl] > 0))
			{
				runTime = GetRunTime(car, scannedFl, scanFl);
				scannedFl = scanFl;
				retTime += DwellTime(scanFl, Car[car].n_DnCall[scanFl], Car[car].n_DnStop[scanFl]) + openCloseTime + runTime;
			}
		}
		
		if(Car[car].n_UpCall[scannedFl] > 0)
		{
			retTime += DwellTime(scannedFl, Car[car].n_UpCall[scannedFl], Car[car].n_UpStop[scannedFl])
				- DwellTime(scannedFl, Car[car].n_DnCall[scannedFl], Car[car].n_UpStop[scannedFl]);
		}

		scanDir = UP;
		for(scanFl = scannedFl-1; scanFl<n_floor; scanFl++)
		{
			if(((scanDir == dir) || oneScan) && (scanFl == fl))
				return (retTime + GetCommittableTime(car, scannedFl, scanFl));

			//if the car has hall calls or
			//if the car has future car calls
			if((Car[car].n_UpCall[scanFl] > 0) || (Car[car].n_UpStop[scanFl] > 0))
			{
				runTime = GetRunTime(car, scannedFl, scanFl);
				scannedFl = scanFl;
				retTime += DwellTime(scanFl, Car[car].n_UpCall[scanFl], Car[car].n_UpStop[scanFl]) + openCloseTime + runTime;
			}
		}
		
		if(Car[car].n_DnCall[scannedFl] > 0)
		{
			retTime += DwellTime(scannedFl, Car[car].n_DnCall[scannedFl], Car[car].n_UpStop[scannedFl])
				- DwellTime(scannedFl, Car[car].n_UpCall[scannedFl], Car[car].n_UpStop[scannedFl]);
		}

		scanDir = DN;
		for(scanFl = scannedFl+1; scanFl>nextCommitFloor; scanFl--)
		{
			if(((scanDir == dir) || oneScan) && (scanFl == fl))
				return (retTime + GetCommittableTime(car, scannedFl, scanFl));
			if((Car[car].n_DnCall[scanFl] > 0) || (Car[car].n_DnStop[scanFl] > 0))
			{
				runTime = GetRunTime(car, scannedFl, scanFl);
				scannedFl = scanFl;
				retTime += DwellTime(scanFl, Car[car].n_DnCall[scanFl], Car[car].n_DnStop[scanFl]) + openCloseTime + runTime;
			}
		}

		return retTime;
	}
}

void CMy2DSimDlg::OnButtonNextNewHcall() 
{
	if(!m_trafficProfileOpened)
	{
		MessageBox("Please open the passenger profile!");
		return;
	}
	if (!ProfileCheckPassed)
	{
		if (ProfileCarsForCheck>Building.nCars || ProfileFloorsForCheck>Building.nTotalFloors)
		{ // profile configuration is less than user configuration
			if (MessageBox("Traffic Profile Cars or Floors are larger than user configuration, so simulation can be executed abnormally. Are you OK to Proceed?",
				"Warning of mismatching of configuration",MB_ICONWARNING | MB_YESNO )==IDNO)
				return;
		}
		ProfileCheckPassed=TRUE;
	}
	bEnableCollisionMessageBox=TRUE; 
	while(!m_bSimulationEnd)
	{
		ProcessNextEvent(); 
		if(m_bNeedAssign)
			return;
	}
}

void CMy2DSimDlg::OnButtonClearwarning() 
{
	// TODO: Add your control notification handler code here
	int countList,i;
	countList=m_ListWarning.GetCount();

	for (i=countList;i>=0;i--)
	{
		m_ListWarning.DeleteString(i);
	}
}


void CMy2DSimDlg::DisplayWarningList(CString str)
{
	int listCount;
	m_ListWarning.AddString(str);
	listCount=m_ListWarning.GetCount();
	m_ListWarning.SetCurSel(listCount-1);
}


double CMy2DSimDlg::DwellTime(int fl, int nBoard, int nDeboard)
{
	int total;
	double min_dwell, dwell, lobbyDwell, stopDwell;
	double NorioriTime;
	
	if(nBoard > Param.capacity)
		nBoard = Param.capacity;
	total = nBoard + nDeboard;
	
	if (USERFLOOR(fl) == Building.lobby1Floor || USERFLOOR(fl) == Building.lobby2Floor)
		lobbyDwell = Param.t_lobby_dwell;
	else
		lobbyDwell = 0.0;
	
	if (nBoard == 0)
		stopDwell = Param.t_car_min_dwell;
	else if (nDeboard == 0)
		stopDwell = Param.t_hall_min_dwell;
	else
		stopDwell = Param.t_hall_max_dwell;
	
	min_dwell = max(lobbyDwell, stopDwell);
	
	NorioriTime = 1.0;
	dwell = 2.1 + total*NorioriTime/2.0; //Later, decide first parameter whether it is 2.1 or 2.0.
//sprintf(DEBUGMsg, "fl=%d nb=%d ndb=%d lob=%4.2lf stop=%4.2lf min=%4.2lf dwel=%4.2lf",
//fl, nBoard, nDeboard, lobbyDwell, stopDwell, min_dwell, dwell);
//DEBUGWrite(0);
	
	if (dwell < min_dwell)
		return min_dwell;
	else
		return dwell;
}  // DwellTime


void CMy2DSimDlg::CalculatePassengerInCars()
{
	int c, f, dir;//car, floor, direction
	int ncf;//nextCommitFloor
	int dummy;
	int nPassenger=0;
	
	for(c=0; c<n_car; c++)
	{
		if(Car[c].dir == NO_DIR)
		{
			for(dir=0; dir<MAXDIR; dir++)
			{
				for(f=0; f<n_floor; f++)
				{
					m_nPassenger[c][f][dir] = 0;
				}
			}
		}
		else
		{
			ncf = GetNextCommittableFloor(c, &dummy, 0);
			nPassenger = Car[c].n_pax_in_car;
			if(Car[c].dir == UP)
			{
				for(f=ncf; f<n_floor; f++)
				{
					nPassenger += Car[c].n_UpCall[f] - Car[c].n_UpStop[f];
					m_nPassenger[c][f][UP_INDEX] = nPassenger;
				}
				for(f=n_floor-1; f>=0; f--)
				{
					nPassenger += Car[c].n_DnCall[f] - Car[c].n_DnStop[f];
					m_nPassenger[c][f][DN_INDEX] = nPassenger;
				}
				for(f=0; f<ncf; f++)
				{
					nPassenger += Car[c].n_UpCall[f] - Car[c].n_UpStop[f];
					m_nPassenger[c][f][UP_INDEX] = nPassenger;
				}
			}
			else if(Car[c].dir == DN)
			{
				for(f=ncf; f>=0; f--)
				{
					nPassenger += Car[c].n_DnCall[f] - Car[c].n_DnStop[f];
					m_nPassenger[c][f][DN_INDEX] = nPassenger;
				}
				for(f=0; f<n_floor; f++)
				{
					nPassenger += Car[c].n_UpCall[f] - Car[c].n_UpStop[f];
					m_nPassenger[c][f][UP_INDEX] = nPassenger;
				}
				for(f=n_floor-1; f>ncf; f--)
				{
					nPassenger += Car[c].n_DnCall[f] - Car[c].n_DnStop[f];
					m_nPassenger[c][f][DN_INDEX] = nPassenger;
				}
			}
		}
		m_nMaxPassenger[c] = 0;
		for(dir=0; dir<MAXDIR; dir++)
		{
			for(f=0; f<n_floor; f++)
			{
				if(m_nPassenger[c][f][dir] > m_nMaxPassenger[c])
					m_nMaxPassenger[c] = m_nPassenger[c][f][dir];
			}
		}
		dir = NO_DIR;
	}
}

void CMy2DSimDlg::SetBuildingParameter(BUILDING_STRUCTURE *b)
{
	memcpy(&Building,b,sizeof(BUILDING_STRUCTURE));
}

void CMy2DSimDlg::OnButtonLoad() 
{
	m_bIsVisibleShowLoad = m_pShowLoadDlg->ShowWindow(SW_SHOW);
}

void CMy2DSimDlg::GenerateFullProfile(int c, double dist)
{
	double maxJerk;
	double maxAcceleration;
	double maxVelocity;
	int kindOfProfile;//1:Car reaches full speed ,2: Car reaches full acceleration ,3: Car does not reach full speed or acceleration
	double temp1, temp2;
	BREAK_TABLE b;
	int i;

	if(FALSE) // If dispatcher send breakpoint table
	{
		//b.num = 0; from dispatcher
		//b.time[0] = 0.0; from dispatcher
		//b.jerk[0] = 0.0; from dispatcher
		GenerateProfile(&b);
		Car[c].mp_n_break = b.num;
		for(i=0; i<b.num; i++)
		{
			Car[c].mp_jerk[i] = b.jerk[i];
			Car[c].mp_accel[i] = b.accel[i];
			Car[c].mp_speed[i] = b.velocity[i];
			Car[c].mp_pos[i] = b.position[i];
		}
		return;
	}
	if(dist <= (0.0 + EPSILON))
	{
		// dist is too short or minus. Add this to the error.log.
		// dist is 0.0 when the origin floor and target floor are same.
		return;
	}

	maxJerk = Param.MaxJerk;
	maxAcceleration = Param.MaxAcceleration;
	maxVelocity = Param.MaxVelocity;

	//The condition where full speed is reached before full acceleration
	//(a^2>v*j) is discarded as this would be an illogical design.
	//move this to initialization
	if(pow(maxAcceleration, 2.0) > maxVelocity*maxJerk)
	{
		CString strTemp;
		strTemp.Format("Illogical design! (a^2 > v*j), j=(%3.2f), a=(%3.2f), v=(%3.2f)",maxJerk, maxAcceleration, maxVelocity);
		MessageBox(strTemp);
	}

	//check the motion profile kind
	temp1 = (pow(maxAcceleration, 2.0)*maxVelocity + pow(maxVelocity, 2.0)*maxJerk)/(maxJerk*maxAcceleration);
	temp2 = 2.0*pow(maxAcceleration, 3.0)/pow(maxJerk, 2.0);
	if(dist >= temp1)
		kindOfProfile = 1; // full speed profile
	else if(dist >= temp2)
		kindOfProfile = 2; // full acc profile
	else
		kindOfProfile = 3;
	switch(kindOfProfile)
	{
		//Car reaches full speed
	case 1: MakeBreakPointTableA(c, maxJerk, maxAcceleration, maxVelocity, dist);
		break;
		//Car reaches full acceleration
	case 2: MakeBreakPointTableB(c, maxJerk, maxAcceleration, maxVelocity, dist);;//FA
		break;
		//Car doesn't reache full speed or acceleration
	case 3: MakeBreakPointTableC(c, maxJerk, maxAcceleration, maxVelocity, dist);;//NO
		break;
	default:
		break;
	}
}

void CMy2DSimDlg::MakeBreakPointTableA(int c, double j, double a, double v, double d)
{
	double T1, T2, T3;
	double deltaV1, deltaV2, deltaV3;
	double deltaD1, deltaD2, deltaD3;
	int i;
	double startTime;

	//shin 040819 After start delay time, car start to run.
	startTime = Car[c].t_run_start + Param.StartDelay + Car[c].tRunDelay;

	Car[c].mp_n_break = NUM_BREAK_MAX_VELOCITY;

	//T1, T2, T3, deltaV1, deltaV2, deltaV3, deltaD1, deltaD2 and deltaD3
	//can be calculated in the initialization part to speed up for execution time.
	T1 = a/j;
	T2 = v/a;
	T3 = T1+T2;

	deltaV1 = pow(a, 2.0)/(2.0*j);
	deltaV2 = v - pow(a, 2.0)/j;
	deltaV3 = deltaV1;

	deltaD1 = pow(a, 3.0)/(6.0*pow(j, 2.0));
	deltaD2 = pow(v, 2.0)/(2.0*a) - a*v/(2.0*j);
	deltaD3 = a*v/j - deltaD1;


	Car[c].mp_break[0] = startTime;
	Car[c].mp_break[1] = startTime + T1;
	Car[c].mp_break[2] = startTime + T2;
	Car[c].mp_break[3] = startTime + T3;
	Car[c].mp_break[4] = startTime + d/v;
	Car[c].mp_break[5] = startTime + d/v+T1;
	Car[c].mp_break[6] = startTime + d/v+T2;
	Car[c].mp_break[7] = startTime + d/v+T3;

	if(Car[c].dir == UP)
	{
		Car[c].mp_jerk[0] = j;
		Car[c].mp_jerk[1] = 0.0;
		Car[c].mp_jerk[2] = -j;
		Car[c].mp_jerk[3] = 0.0;
		Car[c].mp_jerk[4] = -j;
		Car[c].mp_jerk[5] = 0.0;
		Car[c].mp_jerk[6] = j;
		Car[c].mp_jerk[7] = 0.0;
	}
	else if(Car[c].dir == DN)
	{
		Car[c].mp_jerk[0] = -j;
		Car[c].mp_jerk[1] = 0.0;
		Car[c].mp_jerk[2] = j;
		Car[c].mp_jerk[3] = 0.0;
		Car[c].mp_jerk[4] = j;
		Car[c].mp_jerk[5] = 0.0;
		Car[c].mp_jerk[6] = -j;
		Car[c].mp_jerk[7] = 0.0;
	}
	else
	{
		for(i=0; i<Car[c].mp_n_break; i++)
		{
			Car[c].mp_jerk[i] = 0.0;
		}
		fprintf(f_ErrorLog, "MakeBreakPointTableA: car %d, dir %d at t=%lf\n",
			c, Car[c].dir, Sys.t_nextEvent);
	}

	//Calculate acceleration, speed and position from break time and jerk
	Car[c].mp_accel[0] = 0.0;
	Car[c].mp_speed[0] = 0.0;
	Car[c].mp_pos[0] = FloorToPos(Car[c].floor);

	for(i=1; i<Car[c].mp_n_break; i++)
	{
		Car[c].mp_accel[i] = Car[c].mp_accel[i-1]
			+ Car[c].mp_jerk[i-1]*(Car[c].mp_break[i]
			- Car[c].mp_break[i-1]);
		Car[c].mp_speed[i] = Car[c].mp_speed[i-1]
			+ Car[c].mp_accel[i-1]*(Car[c].mp_break[i] - Car[c].mp_break[i-1])
			+ Car[c].mp_jerk[i-1]*pow((Car[c].mp_break[i] - Car[c].mp_break[i-1]), 2.0)/2.0;
		Car[c].mp_pos[i] = Car[c].mp_pos[i-1]
			+ Car[c].mp_speed[i-1]*(Car[c].mp_break[i] - Car[c].mp_break[i-1])
			+ Car[c].mp_accel[i-1]*pow((Car[c].mp_break[i] - Car[c].mp_break[i-1]), 2.0)/2.0
			+ Car[c].mp_jerk[i-1]*pow((Car[c].mp_break[i] - Car[c].mp_break[i-1]), 3.0)/6.0;
	}
}

void CMy2DSimDlg::MakeBreakPointTableB(int c, double j, double a, double v, double d)
{
	double T1, T2, T3;
	double tmpT;
	double deltaV1, deltaV2, deltaV3;
	double deltaD1, deltaD2, deltaD3;
	int i;
	double startTime;

	//shin 040819 After start delay time, car start to run.
	startTime = Car[c].t_run_start + Param.StartDelay + Car[c].tRunDelay;

	Car[c].mp_n_break = NUM_BREAK_MAX_ACCELERATION;

	tmpT = sqrt(pow(a, 3.0)+4*d*pow(j, 2.0))/(2*j*sqrt(a));
	T1 = a/j;
	T2 = tmpT - 0.5*T1;
	T3 = tmpT + 0.5*T1;

	deltaV1 = pow(a, 2.0)/(2.0*j);
	deltaV2 = a*(tmpT-1.5*T1);
	deltaV3 = deltaV1;

	deltaD1 = pow(a, 3.0)/(6.0*pow(j, 2.0));
	deltaD2 = 0.5*(tmpT-1.5*T1)*(a*tmpT-deltaV1);
	deltaD3 = 0.5*d-deltaD1-deltaD2;


	Car[c].mp_break[0] = startTime;
	Car[c].mp_break[1] = startTime + T1;
	Car[c].mp_break[2] = startTime + T2;
	Car[c].mp_break[3] = startTime + T3;
	Car[c].mp_break[4] = startTime + T3+T1;
	Car[c].mp_break[5] = startTime + T3+T2;
	Car[c].mp_break[6] = startTime + 2.0*T3;

	if(Car[c].dir == UP)
	{
		Car[c].mp_jerk[0] = j;
		Car[c].mp_jerk[1] = 0.0;
		Car[c].mp_jerk[2] = -j;
		Car[c].mp_jerk[3] = -j;
		Car[c].mp_jerk[4] = 0.0;
		Car[c].mp_jerk[5] = j;
		Car[c].mp_jerk[6] = 0.0;
	}
	else if(Car[c].dir == DN)
	{
		Car[c].mp_jerk[0] = -j;
		Car[c].mp_jerk[1] = 0.0;
		Car[c].mp_jerk[2] = j;
		Car[c].mp_jerk[3] = j;
		Car[c].mp_jerk[4] = 0.0;
		Car[c].mp_jerk[5] = -j;
		Car[c].mp_jerk[6] = 0.0;
	}
	else
	{
		for(i=0; i<Car[c].mp_n_break; i++)
		{
			Car[c].mp_jerk[i] = 0.0;
		}
		fprintf(f_ErrorLog, "MakeBreakPointTableB: car %d, dir %d at t=%lf\n",
			c, Car[c].dir, Sys.t_nextEvent);
	}

	//Calculate acceleration, speed and position from break time and jerk
	Car[c].mp_accel[0] = 0.0;
	Car[c].mp_speed[0] = 0.0;
	Car[c].mp_pos[0] = FloorToPos(Car[c].floor);

	for(i=1; i<Car[c].mp_n_break; i++)
	{
		Car[c].mp_accel[i] = Car[c].mp_accel[i-1]
			+ Car[c].mp_jerk[i-1]*(Car[c].mp_break[i]
			- Car[c].mp_break[i-1]);
		Car[c].mp_speed[i] = Car[c].mp_speed[i-1]
			+ Car[c].mp_accel[i-1]*(Car[c].mp_break[i] - Car[c].mp_break[i-1])
			+ Car[c].mp_jerk[i-1]*pow((Car[c].mp_break[i] - Car[c].mp_break[i-1]), 2.0)/2.0;
		Car[c].mp_pos[i] = Car[c].mp_pos[i-1]
			+ Car[c].mp_speed[i-1]*(Car[c].mp_break[i] - Car[c].mp_break[i-1])
			+ Car[c].mp_accel[i-1]*pow((Car[c].mp_break[i] - Car[c].mp_break[i-1]), 2.0)/2.0
			+ Car[c].mp_jerk[i-1]*pow((Car[c].mp_break[i] - Car[c].mp_break[i-1]), 3.0)/6.0;
	}
}

void CMy2DSimDlg::MakeBreakPointTableC(int c, double j, double a, double v, double d)
{
	double tmpT;
	int i;
	double startTime;

	//shin 040819 After start delay time, car start to run.
	startTime = Car[c].t_run_start + Param.StartDelay + Car[c].tRunDelay;

	Car[c].mp_n_break = NUM_BREAK_NEITHER;

	tmpT = pow(0.5*d/j, 1.0/3.0);

	Car[c].mp_break[0] = startTime;
	Car[c].mp_break[1] = startTime + tmpT;
	Car[c].mp_break[2] = startTime + 2.0*tmpT;
	Car[c].mp_break[3] = startTime + 3.0*tmpT;
	Car[c].mp_break[4] = startTime + 4.0*tmpT;

	if(Car[c].dir == UP)
	{
		Car[c].mp_jerk[0] = j;
		Car[c].mp_jerk[1] = -j;
		Car[c].mp_jerk[2] = -j;
		Car[c].mp_jerk[3] = j;
		Car[c].mp_jerk[4] = 0.0;
	}
	else if(Car[c].dir == DN)
	{
		Car[c].mp_jerk[0] = -j;
		Car[c].mp_jerk[1] = j;
		Car[c].mp_jerk[2] = j;
		Car[c].mp_jerk[3] = -j;
		Car[c].mp_jerk[4] = 0.0;
	}
	else
	{
		for(i=0; i<Car[c].mp_n_break; i++)
		{
			Car[c].mp_jerk[i] = 0.0;
		}
		fprintf(f_ErrorLog, "MakeBreakPointTableC: car %d, dir %d at t=%lf\n",
			c, Car[c].dir, Sys.t_nextEvent);
	}

	//Calculate acceleration, speed and position from break time and jerk
	Car[c].mp_accel[0] = 0.0;
	Car[c].mp_speed[0] = 0.0;
	Car[c].mp_pos[0] = FloorToPos(Car[c].floor);

	for(i=1; i<Car[c].mp_n_break; i++)
	{
		Car[c].mp_accel[i] = Car[c].mp_accel[i-1]
			+ Car[c].mp_jerk[i-1]*(Car[c].mp_break[i]
			- Car[c].mp_break[i-1]);
		Car[c].mp_speed[i] = Car[c].mp_speed[i-1]
			+ Car[c].mp_accel[i-1]*(Car[c].mp_break[i] - Car[c].mp_break[i-1])
			+ Car[c].mp_jerk[i-1]*pow((Car[c].mp_break[i] - Car[c].mp_break[i-1]), 2.0)/2.0;
		Car[c].mp_pos[i] = Car[c].mp_pos[i-1]
			+ Car[c].mp_speed[i-1]*(Car[c].mp_break[i] - Car[c].mp_break[i-1])
			+ Car[c].mp_accel[i-1]*pow((Car[c].mp_break[i] - Car[c].mp_break[i-1]), 2.0)/2.0
			+ Car[c].mp_jerk[i-1]*pow((Car[c].mp_break[i] - Car[c].mp_break[i-1]), 3.0)/6.0;
	}
}

BOOL CMy2DSimDlg::ComputeMotionState(int c, double t, double *jerk, double *accel, double *speed, double *pos)
{
	int index;//index of break point table (motion profile)
	double deltaT;
	double j;//jerk

	index = GetBreakPointTableIndex(c, t);
	if( index > -1)
		deltaT = t - Car[c].mp_break[index];
	else
	{
		*jerk = *accel = *speed = 0.0;//shin 040812
		*pos = FloorToPos(Car[c].floor);//shin 040812
		if (index == START_DELAY_PERIOD )
		{
			return TRUE;//No change in the motion state
		}
		if (index == STOP_DELAY_PERIOD)
		{
			*pos=FloorToPos(Car[c].target);//Change only position
			return TRUE;
		}
		else
		{
			return FALSE;//add err log
		}
	}
	if(deltaT < 0.0)
		MessageBox("deltaT is less than 0!");//add err log

	j = Car[c].mp_jerk[index];
	*jerk = j;
	*accel = Car[c].mp_accel[index] + j * deltaT;
	*speed = Car[c].mp_speed[index]
		+ Car[c].mp_accel[index] * deltaT
		+ j * pow(deltaT, 2.0)/2.0;
	*pos = Car[c].mp_pos[index]
		+ Car[c].mp_speed[index]*deltaT
		+ 0.5 * Car[c].mp_accel[index] * pow(deltaT, 2.0)
		+ j * pow(deltaT, 3.0)/6.0;
	
	return TRUE;
}

int CMy2DSimDlg::GetBreakPointTableIndex(int c, double t)
{
	int i;
	int num;

	num = Car[c].mp_n_break;
	if(num < 1)
		return -4;//no break table

	// check whether t is out of range in the break point table including delay time.
	if((t < Car[c].mp_break[0]-(Param.StartDelay + Car[c].tRunDelay)) || (Car[c].mp_break[num-1]+Param.StopDelay < t))
		return -3;//add err log

	if (t < Car[c].mp_break[0] )
	{
		return START_DELAY_PERIOD;
	}
	
	if (t > Car[c].mp_break[num-1])
	{
		return STOP_DELAY_PERIOD;
	}
	if (Param.StopDelay == 0 && t == Car[c].mp_break[num-1])
		return STOP_DELAY_PERIOD;
	
	//search index
	for(i=num-1; i>=0 ; i--)  // IN TABLE
	{
		if(Car[c].mp_break[i] <= t)
			return i;
	}

	return -5;//fail, add err log
}

// Get time after running
double CMy2DSimDlg::GetRunTime(int c)
{
	//return Car[c].mp_break[Car[c].mp_n_break-1];
	//shin 040923 Run time included StopDelay time.
	return Car[c].mp_break[Car[c].mp_n_break-1] + Param.StopDelay;
}

//Get the current motion state(jerk, accel, speed and pos)
void CMy2DSimDlg::GetCurrentMotionState(int c, double *jerk, double *accel, double *speed, double *pos)
{
	double currentTime;
	
	currentTime = Sys.t_nextEvent;
	ComputeMotionState(c, currentTime, jerk, accel, speed, pos);
}

//Get the run time from an origin floor position to a destination floor position.
double CMy2DSimDlg::GetRunTime(int c, int origin, int dest)
{
	double originPos, destPos;
	double dist;
	double j;//maxJerk
	double a;//maxAcceleration
	double v;//maxVelocity
	int kindOfProfile;//1:Car reaches full speed ,2: Car reaches full acceleration ,3: Car does not reach full speed or acceleration
	double temp1, temp2;
	double time;

	if(m_bParamHard)//shin 040914 two kinds of parameter
		return HardRunTime(origin, dest);
	
	j = Param.MaxJerk;
	a = Param.MaxAcceleration;
	v = Param.MaxVelocity;

	originPos = FloorToPos(origin);
	destPos = FloorToPos(dest);
	dist = fabs(originPos - destPos);

	// If distance is zero, than delay time is excluded.
	// So it takes 0 second.
	if(dist < EPSILON)
		return 0.0;

	//check the motion profile kind
	temp1 = (pow(a, 2.0)*v + pow(v, 2.0)*j)/(j*a);
	temp2 = 2.0*pow(a, 3.0)/pow(j, 2.0);
	if(dist >= temp1)
		kindOfProfile = 1;
	else if(dist >= temp2)
		kindOfProfile = 2;
	else
		kindOfProfile = 3;

	switch (kindOfProfile)
	{
	case 1:
		time = (dist/v+a/j+v/a);
		break;
	case 2:
		time = (2.0*(sqrt(pow(a, 3.0)+4*dist*pow(j, 2.0))/(2*j*sqrt(a)) + 0.5*a/j));
		break;
	case 3:
		time = (4.0*(pow(0.5*dist/j, 1.0/3.0)));
		break;
	default:
		time = 0.0;
		break;
	}
	time += Param.StartDelay + Param.StopDelay + Car[c].tRunDelay;
	return time;
}

//Should be related floor and position table
double CMy2DSimDlg::FloorToPos(int floor)
{
	return Building.floorPosition[floor];//Change this according to spec table
}

//Should be related floor and position table
int CMy2DSimDlg::PosToFloor(double pos)
{
	int f;
	for(f=0; f<n_floor; f++)
	{
		if(pos < (Building.floorPosition[f] + Building.floorPosition[f+1])/2.0)
			return f;
	}
	return -1;//Error
}

#if FALSE
int CMy2DSimDlg::GetNextCommittableFloor(int c, int *into_decel, int should_be_in_flight)
{
	double t_into_run;
	double dist, committablePos;
	double posOrg, posDest;//Position
	double maxJ;
	double maxA;
	double maxV;
	double minCommitDist;//miniml committable distance is inter-floor height
	double totalDist, temp1, temp2, temp3;

	if(m_bParamHard)//shin 040914 two kinds of parameter
		return NextCommittableFloor(c, into_decel, should_be_in_flight);
	
	maxJ = Param.MaxJerk;
	maxA = Param.MaxAcceleration;
	maxV = Param.MaxVelocity;

	*into_decel = 0;  // indicates if car is already decelerating into target
	if (Car[c].oper != RUNNING && Car[c].oper != FORCED_MOVE && Car[c].oper != MOVE_CAR_RANGE)
	{
		if (should_be_in_flight)
			fprintf(f_ErrorLog, "GetNextCommittableFloor 1: car %d in operation %d at t=%lf\n",
			c, Car[c].oper, Sys.t_nextEvent);
		return Car[c].floor;
	}
	else if (Car[c].target == Car[c].floor)
	{
		fprintf(f_ErrorLog, "GetNextCommittableFloor 2: car %d at %d at t=%lf\n",
			c, USERFLOOR(Car[c].target), Sys.t_nextEvent);
		return Car[c].floor;
	}
	else
	{
		//t_into_run = Sys.t_nextEvent - Car[c].t_run_start;
		t_into_run = Sys.t_nextEvent - Car[c].t_run_start-Param.StartDelay;
		if(t_into_run < 0.0)//Consider StartDelay
			t_into_run = 0.0;
		posOrg = FloorToPos(Car[c].floor);
		posDest = FloorToPos(Car[c].target);
		totalDist = fabs(posOrg - posDest);
		temp1 = (pow(maxA, 2.0)*maxV + pow(maxV, 2.0)*maxJ)/(maxJ*maxA);
		temp2 = 2.0*pow(maxA, 3.0)/pow(maxJ, 2.0);
		if(totalDist >= temp1) // full speed profile
		{
			if((t_into_run >= totalDist/maxV)/* && (t_into_run <= totalDist/maxV+maxA/maxJ+maxV/maxA)*/)
			{
				dist = totalDist;
				*into_decel = 1;
			}
			else if(t_into_run >= maxA/maxJ + maxV/maxA)
				dist = totalDist - maxV*(GetCommittableTime(Car[c].floor, Car[c].target)-Param.StartDelay-t_into_run);
			else if(t_into_run >= maxV/maxA)
				dist = pow(maxV, 2.0)/maxA + maxA*maxV/maxJ;
			else if(t_into_run >= maxA/maxJ)
				dist = pow(t_into_run + maxA/(2.0*maxJ), 2.0)*maxA - pow(maxA, 3.0)/(4.0*pow(maxJ, 2.0));
			else if(t_into_run >= 0.0)
				dist = 2.0*pow(t_into_run, 3.0)*maxJ;
			else
				fprintf(f_ErrorLog, "GetNextCommittableFloor 3: car(%d) t_into_run=(%lf) at t(%lf)\n",
				c, t_into_run, Sys.t_nextEvent);// out of range
		}
		else if(totalDist >= temp2) // full acc profile
		{
			temp3 = pow(pow(maxA, 3.0) + 4.0*totalDist*pow(maxJ, 2.0), 0.5)/(2.0*maxJ*pow(maxA, 0.5));
			if((t_into_run >= -maxA/(2.0*maxJ) + temp3)/* && (t_into_run <= maxA/maxJ + 2.0*temp3)*/)
			{
				dist = totalDist;
				*into_decel = 1;
			}
			else if(t_into_run >= maxA/maxJ)
				dist = pow(t_into_run + maxA/(2.0*maxJ), 2.0)*maxA - pow(maxA, 3.0)/(4.0*pow(maxJ, 2.0));
			else if(t_into_run >= 0.0)
				dist = 2.0*pow(t_into_run, 3.0)*maxJ;
			else
				fprintf(f_ErrorLog, "GetNextCommittableFloor 4: car(%d) t_into_run=(%lf) at t(%lf)\n",
				c, t_into_run, Sys.t_nextEvent);// out of range
		}
		else
		{
			temp3 = pow(0.5*totalDist/maxJ, 1.0/3.0);
			if((t_into_run >= temp3)/* && (t_into_run <= 4*temp3)*/)
			{
				dist = totalDist;
				*into_decel = 1;
			}
			else if(t_into_run >= 0.0)
				dist = 2.0*pow(t_into_run, 3.0)*maxJ;
			else
				fprintf(f_ErrorLog, "GetNextCommittableFloor 5: car(%d) t_into_run=(%lf) at t(%lf)\n",
				c, t_into_run, Sys.t_nextEvent);// out of range
		}

		//shin 040730
		//correction by minCommitDist
		if(Car[c].dir == UP)
			minCommitDist = FloorToPos(Car[c].floor+1) - posOrg;
		else if(Car[c].dir == DN)
			minCommitDist = posOrg - FloorToPos(Car[c].floor-1);
		else
			minCommitDist = 0;

		if(dist < minCommitDist)
			dist = minCommitDist;

		if(Car[c].dir == UP)
			committablePos = posOrg + dist;
		else if(Car[c].dir == DN)
			committablePos = posOrg - dist;
		else
		{
			committablePos = posOrg;
			fprintf(f_ErrorLog, "GetNextCommittableFloor 6: car %d, time into run %lf dir=%d at t=%lf\n",
			c, t_into_run, Car[c].dir, Sys.t_nextEvent);
		}
		if (should_be_in_flight &&
			(t_into_run > GetRunTime(Car[c].floor, Car[c].target) + EPSILON || t_into_run < -EPSILON))
			fprintf(f_ErrorLog, "GetNextCommittableFloor 7: car %d, time into run %lf vs %lf at t=%lf\n",
			c, t_into_run, GetRunTime(Car[c].floor, Car[c].target), Sys.t_nextEvent);

		if (dist > fabs(posOrg - posDest))
		{
			*into_decel = 1;
			return Car[c].target;
		}
		else
			return PosToFloor(committablePos);
	}
}
#endif

int CMy2DSimDlg::GetNextCommittableFloor(int c, int *into_decel, int should_be_in_flight)
{
	double t_into_run;
	double dist, committablePos;
	int committableFloor;
	double posOrg, posDest;//Position
	double maxJ, maxA, maxV;
	double minCommitDist;//miniml committable distance is inter-floor height
	double j, a, v, p;
	double vb; //velocity at the latest break point before current time
	int indexCT;// index of break point table at current time
	double tStep; // time at max accelerlation point from starting to run
	double tStep1;
	double vStep; // velocity at max accelerlation point from starting to run
	BREAK_TABLE b; // break table for getting next committable floor
	int i, ib;
	int carMovingDirection;

	posDest = 0;  // Added by CSW 2/7/2005, initialize the value of posDest
	dist = 0;     // Added by CSW 2/7/2005, initialize the value of posDest

	if(m_bParamHard)//shin 040914 two kinds of parameter
		return NextCommittableFloor(c, into_decel, should_be_in_flight);
	
	maxJ = Param.MaxJerk;
	maxA = Param.MaxAcceleration;
	maxV = Param.MaxVelocity;

	*into_decel = 0;  // indicates if car is already decelerating into target
	if (Car[c].oper != RUNNING && Car[c].oper != FORCED_MOVE && Car[c].oper != MOVE_CAR_RANGE)
	{
		if (should_be_in_flight)
			fprintf(f_ErrorLog, "GetNextCommittableFloor 1: car %d in operation %d at t=%lf\n",
			c, Car[c].oper, Sys.t_nextEvent);
		return Car[c].floor;
	}
	else if (Car[c].target == Car[c].floor)
	{
		fprintf(f_ErrorLog, "GetNextCommittableFloor 2: car %d at %d at t=%lf\n",
			c, USERFLOOR(Car[c].target), Sys.t_nextEvent);
		return Car[c].floor;
	}
	else
	{
		if(Sys.t_nextEvent - Car[c].t_run_start > Param.StartDelay + Car[c].tRunDelay)
			t_into_run = Sys.t_nextEvent - Car[c].t_run_start - (Param.StartDelay + Car[c].tRunDelay);
		else if(Sys.t_nextEvent > Car[c].t_run_start)
			t_into_run = Sys.t_nextEvent - Car[c].t_run_start;
		else
			t_into_run = 0.0;

		tStep = maxA/maxJ;
		vStep = tStep * maxA / 2.0;
		indexCT = GetBreakPointTableIndex(c, Sys.t_nextEvent);
		posOrg = FloorToPos(Car[c].floor);
		posDest = FloorToPos(Car[c].target);

		//shin050822 Use moving direction to get next committable floor
		if(Car[c].floor < Car[c].target)
			carMovingDirection = UP;
		else if(Car[c].floor > Car[c].target)
			carMovingDirection = DN;
		//else
		//	carMovingDirection = NO_DIR;
		if(indexCT < Car[c].mp_n_break && indexCT >= 0)//shin 041006
		{
			//shin050822
			//Current time is the time which motion ends.
			if(indexCT == Car[c].mp_n_break - 1)
			{
				*into_decel = 1;
				return Car[c].target;
			}
			b.accel[0] = Car[c].mp_accel[0];
			b.velocity[0] = Car[c].mp_speed[0];
			b.position[0] = Car[c].mp_pos[0];
			for(i=0; i<=indexCT; i++)
			{
				b.time[i] = Car[c].mp_break[i];
				b.jerk[i] = Car[c].mp_jerk[i];
			}
			ComputeMotionState(c, Car[c].mp_break[indexCT], &j, &a, &vb, &p);
			ComputeMotionState(c, Sys.t_nextEvent, &j, &a, &v, &p);
			if(carMovingDirection == UP)
			{
				if(a > maxA-PRECISION)
				{
					//case 2
					ib = indexCT+1;
					b.time[ib] = Sys.t_nextEvent;
					b.jerk[ib++] = -maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = -maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = 0.0;
					
					b.time[ib] = b.time[ib-1] + v/maxA - tStep/2.0;
					b.jerk[ib++] = maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = 0.0;
					
					b.num = ib;
				}
				else if(a > PRECISION)
				{
					//case 1, 13, 3, 12
					if(j > maxJ-PRECISION)
					{
						if(v < vStep)
						{
							// 1
							tStep1 = Sys.t_nextEvent - b.time[0];
							ib = indexCT+1;
							b.time[ib] = Sys.t_nextEvent;
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1;
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1;
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
						else
						{
							// 13
							tStep1 = Sys.t_nextEvent - b.time[indexCT];
							ib = indexCT+1;
							b.time[ib] = Sys.t_nextEvent;
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1 - (Sys.t_nextEvent-b.time[indexCT]);
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep/2.0;
							b.jerk[ib++] = 0.0;
							
							b.time[ib] = b.time[ib-1] + v/maxA - tStep/2.0;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep/2.0;
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
					}
					else if (j < -maxJ+PRECISION)
					{
						//3, 12
						ib = indexCT+1;
						b.time[ib] = Sys.t_nextEvent;
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep - (Sys.t_nextEvent-b.time[indexCT]);
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] + vb/maxA - tStep/2.0;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
					else
					{
						//Error
					}
				}
				else if(a > -PRECISION)
				{
					//case 4, 8
					if(v > maxV - PRECISION)
					{
						// 4
						ib = indexCT+1;
						b.time[ib] = Sys.t_nextEvent;
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] + v/maxA - tStep;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
					else
					{
						// 8
						ib = indexCT+1;
						b.time[ib] = Sys.t_nextEvent;
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] + v/maxA - tStep;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
				}
				else if(a > -maxA+PRECISION)
				{
					//case 5, 9, 11, 7
					if(j > maxJ-PRECISION)
					{
						if(v < vStep)
						{
							// 11
							ib = indexCT+1;
							b.time[ib] = Sys.t_nextEvent;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = Car[c].mp_break[ib-1];
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
						else
						{
							// 7
							ib = indexCT+1;
							b.time[ib] = Sys.t_nextEvent;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep - (Sys.t_nextEvent-b.time[indexCT]);
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep;
							b.jerk[ib++] = 0.0;
							
							b.time[ib] = b.time[ib-1] + vb/maxA - 3*tStep/2.0;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep;
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
					}
					else if (j < -maxJ+PRECISION)
					{
						//5, 9
						ib = indexCT+1;
						b.time[ib] = Sys.t_nextEvent;
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep - (Sys.t_nextEvent-b.time[indexCT]);
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] + vb/maxA - tStep;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
					else
					{
						//Error
					}
				}
				else //a < -maxA+PRECISION
				{
					//case 6, 10
					ib = indexCT+1;
					b.time[ib] = Sys.t_nextEvent;
					b.jerk[ib++] = 0.0;
					
					//b.time[ib] = b.time[ib-1] + v/maxA - tStep/2.0;
					b.time[ib] = b.time[ib-2] + vb/maxA - tStep/2.0;//shin050822
					b.jerk[ib++] = maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = 0.0;
					
					b.num = ib;
				}
			}
			else if(carMovingDirection == DN)//Car[c].dir == DN
			{
				if(-a > maxA-PRECISION)
				{
					//case 2
					ib = indexCT+1;
					b.time[ib] = Sys.t_nextEvent;
					b.jerk[ib++] = maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = 0.0;
					
					b.time[ib] = b.time[ib-1] + -v/maxA - tStep/2.0;//shin051027
					b.jerk[ib++] = -maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = 0.0;
					
					b.num = ib;
				}
				else if(-a > PRECISION)
				{
					//case 1, 13, 3, 12
					if(-j > maxJ-PRECISION)
					{
						if(-v < vStep)
						{
							// 1
							tStep1 = Sys.t_nextEvent - b.time[0];
							ib = indexCT+1;
							b.time[ib] = Sys.t_nextEvent;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1;
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1;
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
						else
						{
							// 13
							tStep1 = Sys.t_nextEvent - b.time[indexCT];
							ib = indexCT+1;
							b.time[ib] = Sys.t_nextEvent;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1 - (Sys.t_nextEvent-b.time[indexCT]);
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep/2.0;
							b.jerk[ib++] = 0.0;
							
							b.time[ib] = b.time[ib-1] + -v/maxA - tStep/2.0;//shin051027
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep/2.0;
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
					}
					else if (-j < -maxJ+PRECISION)
					{
						//3, 12
						ib = indexCT+1;
						b.time[ib] = Sys.t_nextEvent;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep - (Sys.t_nextEvent-b.time[indexCT]);
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] - vb/maxA - tStep/2.0;//shin051027
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
					else
					{
						//Error
					}
				}
				else if(-a > -PRECISION)
				{
					//case 4, 8
					if(-v > maxV - PRECISION)
					{
						// 4
						ib = indexCT+1;
						b.time[ib] = Sys.t_nextEvent;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] + -v/maxA - tStep;//shin051027
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
					else
					{
						// 8
						ib = indexCT+1;
						b.time[ib] = Sys.t_nextEvent;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] + -v/maxA - tStep;//shin051027
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
				}
				else if(-a > -maxA+PRECISION)
				{
					//case 5, 9, 11, 7
					if(-j > maxJ-PRECISION)
					{
						if(-v < vStep)
						{
							// 11
							ib = indexCT+1;
							b.time[ib] = Sys.t_nextEvent;
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = Car[c].mp_break[ib-1];
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
						else
						{
							// 7
							ib = indexCT+1;
							b.time[ib] = Sys.t_nextEvent;
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep - (Sys.t_nextEvent-b.time[indexCT]);
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep;
							b.jerk[ib++] = 0.0;
							
							b.time[ib] = b.time[ib-1] - vb/maxA - 3*tStep/2.0;//shin051027
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep;
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
					}
					else if (-j < -maxJ+PRECISION)
					{
						//5, 9
						ib = indexCT+1;
						b.time[ib] = Sys.t_nextEvent;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep - (Sys.t_nextEvent-b.time[indexCT]);
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] - vb/maxA - tStep;//shin051027
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
					else
					{
						//Error
					}
				}
				else //-a < -maxA+PRECISION
				{
					//case 6, 10
					ib = indexCT+1;
					b.time[ib] = Sys.t_nextEvent;
					b.jerk[ib++] = 0.0;
					
					//b.time[ib] = b.time[ib-1] + -v/maxA + tStep/2.0;
					b.time[ib] = b.time[ib-2] + -vb/maxA - tStep/2.0;//shin050822
					b.jerk[ib++] = -maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = 0.0;
					
					b.num = ib;
				}
			}
			GenerateProfile(&b);
			dist = fabs(b.position[b.num-1] - posOrg);
			if((b.velocity[b.num-1] > PRECISION) || (b.velocity[b.num-1] < -PRECISION))
				fprintf(f_ErrorLog, "GetNextCommittableFloor: [velocity = %f] car %d, time into run %lf dir=%d at t=%lf\n",
				b.velocity[b.num-1], c, t_into_run, carMovingDirection, Sys.t_nextEvent);
		}
		else if(indexCT == START_DELAY_PERIOD)
		{
			dist = 0.0;
		}
		else if(indexCT == STOP_DELAY_PERIOD)
		{
			*into_decel = 1;
			return Car[c].target;
		}

		//shin 040730
		//correction by minCommitDist
		if(carMovingDirection == UP)
			minCommitDist = FloorToPos(Car[c].floor+1) - posOrg;
		else if(carMovingDirection == DN)
			minCommitDist = posOrg - FloorToPos(Car[c].floor-1);
		else
			minCommitDist = 0.0;

		if(dist < minCommitDist)			       // CSW 2/7/2005, dist cause run time error: being used without being defined
			dist = minCommitDist;

		if(carMovingDirection == UP)
			committablePos = posOrg + dist;
		else if(carMovingDirection == DN)
			committablePos = posOrg - dist;
		else
		{
			committablePos = posOrg;
			fprintf(f_ErrorLog, "GetNextCommittableFloor 3: car %d, time into run %lf dir=%d at t=%lf\n",
			c, t_into_run, carMovingDirection, Sys.t_nextEvent);
		}
		if (should_be_in_flight &&
			(t_into_run > GetRunTime(c, Car[c].floor, Car[c].target) + EPSILON || t_into_run < -EPSILON))
			fprintf(f_ErrorLog, "GetNextCommittableFloor 4: car %d, time into run %lf vs %lf at t=%lf\n",
			c, t_into_run, GetRunTime(c, Car[c].floor, Car[c].target), Sys.t_nextEvent);

		if (dist >= fabs(posOrg - posDest))  // CSW 2/7/2005, posDest causing runtime error: being used without being defined
		{
			// shin051005 check decel started.
			// ManualTestImpact
			if(Car[c].t_decel <= Sys.t_nextEvent)
				*into_decel = 1;
			return Car[c].target;
		}
		else
		{
			committableFloor = CommittablePosToNCF(c, committablePos);// ManualTestImpact
			// ManualTestImpact
			if((committableFloor == Car[c].target) 
				&& (Car[c].t_decel <= Sys.t_nextEvent) //shin050930 check decel started. This makes manual result be different from old manual result.
				)
				*into_decel = 1;
			return committableFloor;
		}
	}
}

//Get and log minimum margin between upper car and lower car
// in the same hoistway for each single trip
// check CollisionCheck ftn
void CMy2DSimDlg::CheckMargin(int c)
{
	int other;//other car in the same hoistway
	int i,j,k;
	BREAK_TABLE bTable;//superimposed break table
	double minMargin;//minimum margin between upper car and lower car in the same hoistway
	double tmpMinMargin;
	double timeMinMargin;// the time for two cars having minimum margin
	double timeAtMinimum;//the time for the local minimum margin
	double LeftPos; // the position at the end of other car's run during the running period
	double tmpJ;//temporary jerk
	double tmpA;//temporary acceleration
	double tmpV;//temporary velocity
	double tmpP;//temporary position
	int thisNum;//the number of lows in the break table (this car)
	int otherNum;//the number of lows in the break table (other car)
	int iHoistway; //hoistway index

	if(m_bParamHard)//shin 040914 two kinds of parameter
		return;

	iHoistway = Car[c].i_hwy;
	if(Hwy[iHoistway].n_car_in_hwy < 2) //Check the number of cars in the hoistway
		return;//There is only one car in the hoistway
	
	// Always 2 cars are supposed for one hoistway.
	if(Car[c].lowerOrUpper == LOWER_CAR)
	{
		if(Car[c+1].lowerOrUpper == UPPER_CAR)
			other = c+1;
		else
			other = MAXCAR;
	}
	else
	{
		other = c-1;
	}

	//Here, consider whether thisNum and otherNum is less than 1.
	thisNum = Car[c].mp_n_break;
	if (fabs(Car[c].mp_break[thisNum - 1] - Sys.t_nextEvent) > (Param.StopDelay + EPSILON))
		fprintf(f_ErrorLog, "CheckMargin 1: car %d oper %d t=%5.3lf %d %5.3lf\n",
			c, Car[c].oper, Sys.t_nextEvent, thisNum, Car[c].mp_break[thisNum - 1]);

	otherNum = Car[other].mp_n_break;
	bTable.num = 0;//initialize
	minMargin = 100000.0;
	timeMinMargin = Car[c].mp_break[0];
	//check whether breakpoints are overlapped for two car.
	//make superimposed table.(sort by time)
	if(Car[c].mp_break[0] >= Car[other].mp_break[otherNum-1])  // other car is standing throughout run
	{
		if(Car[c].lowerOrUpper == UPPER_CAR)
		{
			if(Car[c].mp_pos[0] < Car[c].mp_pos[thisNum-1])   // this car is going up
			{
				minMargin = Car[c].mp_pos[0] - Car[other].mp_pos[otherNum-1];
				timeMinMargin = Car[c].mp_break[0];
			}
			else
			{
				minMargin = Car[c].mp_pos[thisNum-1] - Car[other].mp_pos[otherNum-1];
				timeMinMargin = Car[c].mp_break[thisNum-1];
			}
		}
		else
		{
			if(Car[c].mp_pos[0] < Car[c].mp_pos[thisNum-1])   // this car is going up
			{
				minMargin = Car[other].mp_pos[otherNum-1] - Car[c].mp_pos[thisNum-1];
				timeMinMargin = Car[c].mp_break[thisNum-1];
			}
			else
			{
				minMargin = Car[other].mp_pos[otherNum-1] - Car[c].mp_pos[0];
				timeMinMargin = Car[c].mp_break[0];
			}
		}
	}
	else if(Car[c].mp_break[thisNum-1] <= Car[other].mp_break[0])
	{
		if(Car[c].lowerOrUpper == UPPER_CAR)
		{
			minMargin = Car[c].mp_pos[thisNum-1] - Car[other].mp_pos[0];
			timeMinMargin = Car[c].mp_break[thisNum-1];
		}
		else
		{
			minMargin = Car[other].mp_pos[0] - Car[c].mp_pos[thisNum-1];
			timeMinMargin = Car[c].mp_break[0];
		}
	}
	else if(Car[c].mp_break[thisNum-1] < Car[other].mp_break[otherNum-1])  // other car is moving
	{
		if(Car[other].mp_break[0] < Car[c].mp_break[0])
		{
			for(k=0; k<otherNum; k++)
			{
				if(Car[other].mp_break[k] > Car[c].mp_break[0])
					break;
			}
			j=0;
		}
		else
		{
			for(j=0; j<thisNum; j++)
			{
				if(Car[other].mp_break[0] < Car[c].mp_break[j])
					break;
			}
			k=0;
		}
		if(Car[c].lowerOrUpper == UPPER_CAR)
		{
			if(Car[c].mp_break[j] < Car[other].mp_break[k])
			{
				bTable.time[0] = Car[c].mp_break[j];
				ComputeMotionState(other, bTable.time[0], &tmpJ, &tmpA, &tmpV, &tmpP);
				bTable.jerk[0] = Car[c].mp_jerk[j] - tmpJ;
				bTable.accel[0] = Car[c].mp_accel[j] - tmpA;
				bTable.velocity[0] = Car[c].mp_speed[j] - tmpV;
				bTable.position[0] = Car[c].mp_pos[j] - tmpP;
				if(j<(thisNum-1))
					j++;
				else if(k<(otherNum-1))
					k++;
			}
			else
			{
				bTable.time[0] = Car[other].mp_break[k];
				ComputeMotionState(c, bTable.time[0], &tmpJ, &tmpA, &tmpV, &tmpP);
				bTable.jerk[0] = tmpJ - Car[other].mp_jerk[k];
				bTable.accel[0] = tmpA - Car[other].mp_accel[k];
				bTable.velocity[0] = tmpV - Car[other].mp_speed[k];
				bTable.position[0] = tmpP - Car[other].mp_pos[k];
				if(k<(otherNum-1))
					k++;
				else if(j<(thisNum-1))
					j++;
			}
			bTable.num++;//shin051006

			for(i=1; i<(thisNum+otherNum); i++)
			{
				if(Car[c].mp_break[j] < Car[other].mp_break[k])
				{
					bTable.time[i] = Car[c].mp_break[j];
					if(k > 0)
						bTable.jerk[i] = Car[c].mp_jerk[j] - Car[other].mp_jerk[k-1];
					else
						bTable.jerk[i] = Car[c].mp_jerk[j];
					if(j<(thisNum-1))
						j++;
					else if(k<(otherNum-1))
						k++;
				}
				else if(Car[c].mp_break[j] > Car[other].mp_break[k])
				{
					bTable.time[i] = Car[other].mp_break[k];
					bTable.jerk[i] = Car[c].mp_jerk[j-1] - Car[other].mp_jerk[k];
					if(k<(otherNum-1))
						k++;
					else if(j<(thisNum-1))
						j++;
				}
				else //They have same values
				{
					bTable.time[i] = Car[c].mp_break[j];
					bTable.jerk[i] = Car[c].mp_jerk[j] - Car[other].mp_jerk[k];
					if(j<(thisNum-1))
						j++;
					if(k<(otherNum-1))
						k++;
				}
				bTable.num++;
				if(Car[other].mp_break[k] >= Car[c].mp_break[thisNum-1]) 
				{	//shin051005 one more point at the end of overlapped region
					if((Car[c].mp_break[thisNum-1] > bTable.time[i]))
					{
						bTable.time[i+1] = Car[c].mp_break[thisNum-1];
						bTable.jerk[i+1] = Car[c].mp_jerk[thisNum-1] - Car[other].mp_jerk[k-1];
						bTable.num++;
					}
					break;
				}
			}
		}
		else
		{
			if(Car[c].mp_break[j] < Car[other].mp_break[k])
			{
				bTable.time[0] = Car[c].mp_break[j];
				ComputeMotionState(other, bTable.time[0], &tmpJ, &tmpA, &tmpV, &tmpP);
				bTable.jerk[0] = tmpJ - Car[c].mp_jerk[j];
				bTable.accel[0] = tmpA - Car[c].mp_accel[j];
				bTable.velocity[0] = tmpV - Car[c].mp_speed[j];
				bTable.position[0] = tmpP - Car[c].mp_pos[j];
				if(j<(thisNum-1))
					j++;
				else if(k<(otherNum-1))
					k++;
			}
			else
			{
				bTable.time[0] = Car[other].mp_break[k];
				ComputeMotionState(c, bTable.time[0], &tmpJ, &tmpA, &tmpV, &tmpP);
				bTable.jerk[0] = Car[other].mp_jerk[k] - tmpJ;
				bTable.accel[0] = Car[other].mp_accel[k] - tmpA;
				bTable.velocity[0] = Car[other].mp_speed[k] - tmpV;
				bTable.position[0] = Car[other].mp_pos[k] - tmpP;
				if(k<(otherNum-1))
					k++;
				else if(j<(thisNum-1))
					j++;
			}
			bTable.num++;//shin051006
			for(i=1; i<(thisNum+otherNum); i++)
			{
				if(Car[c].mp_break[j] < Car[other].mp_break[k])
				{
					bTable.time[i] = Car[c].mp_break[j];
					if(k > 0)
						bTable.jerk[i] = Car[other].mp_jerk[k-1] - Car[c].mp_jerk[j];
					else
						bTable.jerk[i] = -Car[c].mp_jerk[j];
					if(j<(thisNum-1))
						j++;
					else if(k<(otherNum-1))
						k++;
				}
				else if(Car[c].mp_break[j] > Car[other].mp_break[k])
				{
					bTable.time[i] = Car[other].mp_break[k];
					bTable.jerk[i] = Car[other].mp_jerk[k] - Car[c].mp_jerk[j-1];
					if(k<(otherNum-1))
						k++;
					else if(j<(thisNum-1))
						j++;
				}
				else //They have same values
				{
					bTable.time[i] = Car[c].mp_break[j];
					bTable.jerk[i] = Car[other].mp_jerk[k] - Car[c].mp_jerk[j];
					if(j<(thisNum-1))
						j++;
					if(k<(otherNum-1))
						k++;
				}
				bTable.num++;
				if(Car[other].mp_break[k] >= Car[c].mp_break[thisNum-1])
				{	//shin051005 one more point at the end of overlapped region
					if((Car[c].mp_break[thisNum-1] > bTable.time[i]))
					{
						bTable.time[i+1] = Car[c].mp_break[thisNum-1];
						bTable.jerk[i+1] = Car[c].mp_jerk[thisNum-1] - Car[other].mp_jerk[k-1];
						bTable.num++;
					}
					break;
				}
			}
		}
		GenerateProfile(&bTable);//from initial conditions, break times and jerks
		//calculate minimum point
		//Calculate minMargin and timeMinMargin
		for(i=0; i<(bTable.num-1); i++)
		{
			// shin051005 check more (only check velocity conditions)
			if( /*(bTable.accel[i] >= 0.0)
				&& (bTable.accel[i+1] >= 0.0)
				&&*/ (bTable.velocity[i] <= EPSILON)
				&& (bTable.velocity[i+1] >= -EPSILON) )
			{
				tmpMinMargin = GetMinMargin(&bTable, i, &timeAtMinimum);
				if((tmpMinMargin > 0.0) && (tmpMinMargin < minMargin))
				{
					minMargin = tmpMinMargin;
					timeMinMargin = timeAtMinimum;
				}
			}
		}
	}

// other car was moving but stopped during this car's run
	else//Car[c].mp_break[0] < Car[other].mp_break[otherNum-1] < Car[c].mp_break[thisNum-1]
	{
		// time for LeftPos = Car[other].mp_break[otherNum-1];
		ComputeMotionState(c, Car[other].mp_break[otherNum-1], &tmpJ, &tmpA, &tmpV, &LeftPos);
		// Get position at above time (LeftPos)
		if(Car[c].lowerOrUpper == UPPER_CAR)
		{
			if(LeftPos < Car[c].mp_pos[thisNum-1])
			{
				minMargin = LeftPos - Car[other].mp_pos[otherNum-1];
				timeMinMargin = Car[other].mp_break[otherNum-1];
			}
			else
			{
				minMargin = Car[c].mp_pos[thisNum-1] - Car[other].mp_pos[otherNum-1];
				timeMinMargin = Car[c].mp_break[thisNum-1];
			}
		}
		else
		{
			if(LeftPos < Car[c].mp_pos[thisNum-1])
			{
				minMargin = Car[other].mp_pos[otherNum-1] - Car[c].mp_pos[thisNum-1];
				timeMinMargin = Car[c].mp_break[thisNum-1];
			}
			else
			{
				minMargin = Car[other].mp_pos[otherNum-1] - LeftPos;
				timeMinMargin = Car[other].mp_break[otherNum-1];
			}
		}
	}
// AH:
//sprintf(DEBUGMsg, "CheckMargin: %d %d [=%4.2lf] t=%6.4lf margin %6.4lf at %6.4lf", c, USERFLOOR(Car[c].target),
//Car[c].mp_pos[thisNum-1], Sys.t_nextEvent, minMargin, timeMinMargin);
//DEBUGWrite(0);

	if((minMargin >= 0.0) && (minMargin < 100000.0) && ((minMargin+EPSILON) < m_minMargin))
	{
		//See the log in the warning list of main screen.
		CString str;
		str.Format("(%lf < %lf) (%lf, %d)",minMargin ,m_minMargin, m_minMarginTime, Car[m_car].i_hwy);
		DisplayWarningList(str);

		m_minMargin = minMargin;
		m_minMarginTime = timeMinMargin;
		m_car = c;
		m_strClosestMargin.Format("%5.3lf (%5.3lf, %d)",m_minMargin, m_minMarginTime, Car[m_car].i_hwy);
		//Write log for minMargin here!!
	}
}

void CMy2DSimDlg::GenerateProfile(BREAK_TABLE *b)
{
	int i;
	double diff; //time difference

	for(i=1; i<b->num; i++)
	{
		diff = b->time[i] - b->time[i-1];
		b->accel[i] = b->accel[i-1] + b->jerk[i-1] * diff;
		b->velocity[i] = b->velocity[i-1]
			+ b->accel[i-1] * diff
			+ b->jerk[i-1] * pow(diff, 2.0) / 2.0;
		b->position[i] = b->position[i-1]
			+ b->velocity[i-1] * diff
			+ b->accel[i-1] * pow(diff, 2.0) / 2
			+ b->jerk[i-1] * pow(diff, 3.0) / 6;
	}
}

double CMy2DSimDlg::GetMinMargin(BREAK_TABLE *b, int index, double *t)
{
	double j,a,v,p,startT,endT;
	double t1,t2;//root
	double discriminant;
	double tmp1, tmp2;
	double returnMargin;

	double margin[4], marginTime[4], minMargin;
	int checkNum, indexMin, i;

	startT = b->time[index];
	endT = b->time[index+1];
	j = b->jerk[index];
	a = b->accel[index];
	v = b->velocity[index];
	p = b->position[index];

	checkNum = 2;
	marginTime[0] = startT;
	margin[0] = p;
	marginTime[1] = endT;
	margin[1] = b->position[index+1];

	if((j < EPSILON) && (j > -EPSILON))// It's 1st order equation.
	{
		checkNum = 3;
		marginTime[2] = -v/a + startT;
		margin[2] = p + v*(marginTime[2]-startT) + a*pow(marginTime[2]-startT, 2.0)/2.0;
	}
	else	//It's 2nd order equation.
	{
		tmp1 = a-j*startT;
		discriminant = pow(tmp1, 2.0) - 2*j*(0.5*j*pow(startT, 2.0)-a*startT+v);
		if(discriminant >= -EPSILON)
		{
			if(discriminant < 0.0)
			{
				checkNum = 3;
				marginTime[2] = -tmp1/j;
				margin[2] = p + v*(marginTime[2]-startT) + a*pow(marginTime[2]-startT, 2.0)/2.0
					+j*pow(marginTime[2]-startT, 3.0)/6;;
			}
			else
			{
				tmp2 = pow(discriminant, 0.5);
				t1 = (-tmp1 + tmp2)/j;
				t2 = (-tmp1 - tmp2)/j;
				if((t1 >= startT) && (t1 <= endT) && (a+j*(t1-startT) >= 0.0))
				{
					marginTime[checkNum] = t1;
					margin[checkNum] = p + v*(t1-startT) + a*pow(t1-startT, 2.0)/2.0
						+j*pow(t1-startT, 3.0)/6;
					checkNum++;
				}
				if((t2 >= startT) && (t2 <= endT) && (a+j*(t2-startT) >= 0.0))
				{
					marginTime[checkNum] = t2;
					margin[checkNum] = p + v*(t2-startT) + a*pow(t2-startT, 2.0)/2.0
						+j*pow(t2-startT, 3.0)/6;
					checkNum++;
				}

				if(checkNum <= 2)
				{
					fprintf(f_ErrorLog, "GetMinMargin 1: t1(%lf) t2(%lf) startT(%lf) endT(%lf)\n", t1,t2,startT,endT);
				}
			}
		}
		else
		{
			fprintf(f_ErrorLog, "GetMinMargin 2: discriminant is %lf\n", discriminant);
		}
	}
	//Search minimum
	indexMin = 0;
	minMargin = margin[0];
	for(i=1; i<checkNum; i++)
	{
		if(margin[i] < minMargin)
			indexMin = i;
	}
	returnMargin = margin[indexMin];
	*t = marginTime[indexMin];

	return returnMargin;
}

// Returns the amount of time that it takes from the beginning of run for a moving car to
// traverse traversalDistance.  It is assumed that the car is in a moving operation.
double CMy2DSimDlg::GetTraversalTime(int c, double traversalDistance) {
  double distance, flight_length, factor, pos, lo, hi, mid, lo_pos, mid_pos, hi_pos,
	  dummy_j, dummy_a, dummy_v;

  if ((Car[c].oper != RUNNING) && (Car[c].oper != FORCED_MOVE) && (Car[c].oper != MOVE_CAR_RANGE)) {
	fprintf(f_ErrorLog, "GetTraversalTime 1: car %d oper %d dist %lf\n",
		c, Car[c].oper, traversalDistance);
	return -EPSILON;
  }

  distance = fabs(traversalDistance);
  flight_length = fabs(Building.floorPosition[Car[c].origin] -
	  Building.floorPosition[Car[c].target]);
  if (traversalDistance > flight_length + EPSILON || Car[c].origin == Car[c].target) {
	fprintf(f_ErrorLog, "GetTraversalTime 2: car %d oper %d (%d->%d) dist %lf %d\n",
		c, Car[c].oper, Car[c].origin, Car[c].target, flight_length, traversalDistance);
	return -EPSILON;
  }

// Set lo and hi search range
  lo = Sys.t_nextEvent;
  hi = Car[c].mp_break[Car[c].mp_n_break-1];
  if (lo > hi + EPSILON) {
	fprintf(f_ErrorLog, "GetTraversalTime 3: car %d oper %d (%d->%d) %lf %lf\n",
		c, Car[c].oper, Car[c].origin, Car[c].target, lo, hi);
	return -EPSILON;
  }

// Check consistency of lo and hi position relative to traversal
  if (Car[c].origin < Car[c].target) {  // car is moving up
	factor = 1.0;
	pos = Building.floorPosition[Car[c].origin] + distance;
  }
  else {                                // car is moving down
	factor = -1.0;
	pos = Building.floorPosition[Car[c].origin] - distance;
  }
  ComputeMotionState(c, lo, &dummy_j, &dummy_a, &dummy_v, &lo_pos);
  ComputeMotionState(c, hi, &dummy_j, &dummy_a, &dummy_v, &hi_pos);
  if (factor*(pos - lo_pos) <= 0.0 || factor*(hi_pos - pos) < -EPSILON) {
	fprintf(f_ErrorLog, "GetTraversalTime 4: car %d oper %d (%d->%d) %lf %lf / %lf %lf [%lf]\n",
		c, Car[c].oper, Car[c].origin, Car[c].target, lo, hi, lo_pos, hi_pos, pos);
	return -EPSILON;
  }

// Perform binary search in the range from lo to hi until precision is guaranteed
  while (hi - lo > PRECISION) {
    mid = 0.5*(lo + hi);
    ComputeMotionState(c, mid, &dummy_j, &dummy_a, &dummy_v, &mid_pos);
    if (factor*(pos - mid_pos) > 0.0) {
	  lo = mid;
	  lo_pos = mid_pos;
	}
	else {
	  hi = mid;
	  hi_pos = mid_pos;
	}
  }
	return (0.5*(lo + hi) - Sys.t_nextEvent);

}  /* GetTraversalTime */


void CMy2DSimDlg::UpdateCars()
{
	int i;
	CString strCarNum;
	for (i=0;i<n_car;i++)
	{
		if (m_bCar[i]==0)
		{
			if(i%2 == 0)
				strCarNum.Format("%dL",i);
			else
				strCarNum.Format("%dU",i);
			m_bCar[i] = m_Car[i].Create(strCarNum, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(10,10,100,29), this, i+1100);
		}
		m_Car[i].EnableWindow(TRUE);
		m_Car[i].ShowWindow(SW_SHOW);
	}
		// DISABLE
	for (;i<MAXCAR;i++)
	{
		if (m_bCar[i]!=0)
		{
			m_Car[i].EnableWindow(FALSE);
			m_Car[i].ShowWindow(SW_HIDE);
		}
	}
}

void CMy2DSimDlg::OnCarAssign(UINT nID)
{
	int assigned_car;
	if (nID>=(IDC_BUTTON_CAR0+MAXCAR) 
		|| nID>=UINT((IDC_BUTTON_CAR0+ n_car)))
		return;

	assigned_car=nID-IDC_BUTTON_CAR0;

	if(m_bAssigned == TRUE)
	{
		PressNextEventButton();
		return;
	}
	else if(Sys.y_nextEvent == 1) //next event is not passenger arrival
	{
		Pax[i_pax_recent].assignment = assigned_car;
		AssignPostManage();
	}
	else
		fprintf(f_ErrorLog, "OnCarAssign: Without passenger arrival, assignment button %d is pressed! (Sys.y_nextEvent = %d)\n", assigned_car,Sys.i_nextEvent);
	
}

//Get committable time from source floor to destination floor
double CMy2DSimDlg::GetCommittableTime(int c, int s, int d)
{
	double dist;
	double time;
	double j,a,v;
	double temp1, temp2;
	
	if(m_bParamHard)//shin 040914 two kinds of parameter
		return HardCommitTime(s, d);

	dist = fabs(FloorToPos(s) - FloorToPos(d));

	j = Param.MaxJerk;
	a = Param.MaxAcceleration;
	v = Param.MaxVelocity;

	temp1 = (pow(a, 2.0) * v + pow(v, 2.0) * j) / (j * a);
	temp2 = 2.0 * pow(a, 3.0) / pow(j, 2.0);
	if(dist >= temp1)
		time = dist/v;
	else if(dist >= temp2)
		time = -0.5*a/j + 0.5*pow(pow(a, 3.0) + 4*dist*pow(j, 2.0), 0.5)/(pow(a, 0.5)*j);
	else
		time = pow(4.0*dist/j, 1.0/3.0);

	time += Param.StartDelay + Car[c].tRunDelay;
	return time;
}


double CMy2DSimDlg::GetActualPositionTime(int c, int origin, int dest)
{
	int before_dest;  // index of the floor right before dest during a run from origin
	double traversalDistance;
	double traversalTime;

	if (origin == dest)
		return 0.0;
	else {
		if (origin > dest)
			before_dest = dest + 1;
		else
			before_dest = dest - 1;

		traversalDistance = fabs(FloorToPos(origin) - 0.5*FloorToPos(before_dest) -
			0.5*FloorToPos(dest));
		traversalTime = GetTraversalTime(c, traversalDistance);

		return (traversalTime);
	}
}  // GetActualPositionTime


double CMy2DSimDlg::HardActualPositionTime(int origin, int dest)
{
	int dist;
	
	dist = abs(origin - dest);
	switch (dist) 
	{
	case 0:
		return 0.0;
	case 1:
		return 2.9;
	case 2:
		return 4.4;
	case 3:
		return 5.6;
	default:
		return 2.7 + (double) dist;
	}
}  // HardActualPositionTime


//shin 20040812
// When one car stops at some floor, this function checks whether other car assigned with same direction.
// This is for hoistway asignment function.
//shin 20040902
// According to the number of information, Check the violation
// Check violation only when one car stops.
void CMy2DSimDlg::CheckOtherAssigned(int c)
{
	int other;
	int flr, dir;
	CString str;
	int iHoistway; //hoistway index

	if(Param.numOfInformation > 2) // 3 pieces of info: assignment, direction, upper/lower
		return;
	else
	{
		iHoistway = Car[c].i_hwy;
		if(Hwy[iHoistway].n_car_in_hwy < 2) //Check the number of cars in the hoistway
			return;//There is only one car in the hoistway
		// Always 2 cars are supposed for one hoistway by above check.
		if(Car[c].lowerOrUpper == LOWER_CAR)
		{
			if(Car[c+1].lowerOrUpper == UPPER_CAR)
				other = c+1;
			else
				other = MAXCAR;
		}
		else
		{
			other = c-1;
		}
		flr = Car[c].floor;
		dir = Car[c].dir;

		if(Param.numOfInformation == 2) // 2 pieces of info: assignment, direction
		{
			//check direction, stop, assignment
			if(((dir == UP) && Car[other].n_UpCall[flr])
				|| ((dir == DN) && Car[other].n_DnCall[flr]))
			{
				fprintf(f_ErrorLog,
					"CheckOtherAssigned 1: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!\n",
					other, flr, dir, Sys.t_nextEvent, Car[c].oper);
				str.Format("TWO INFO: (%d) is assigned at (%d) floor with direction (%d)!", other, flr, dir);
				MessageBox(str);
			}
		}
		else // 1 piece of info: assignment
		{
			if(Car[other].n_UpCall[flr] || Car[other].n_DnCall[flr])
			{
				fprintf(f_ErrorLog,
					"CheckOtherAssigned 2: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!\n",
					other, flr, dir, Sys.t_nextEvent, Car[c].oper);
				str.Format("ONE INFO: (%d) is assigned at (%d) floor with direction (%d)!", other, flr, dir);
				MessageBox(str);
			}
		}
	}
}

//shin 040817
// Set no of hoistway and index to first car in hoistway for each hoistway
void CMy2DSimDlg::SetHoistway(int c, int hwy)
{
	int n2D_hwy,iHoistway;

	n2D_hwy=c-hwy;
	if (n2D_hwy<=0)
		n2D_hwy=0;
	for (iHoistway=0;iHoistway<n2D_hwy;iHoistway++)
	{
		Hwy[iHoistway].n_car_in_hwy = 2;
		Hwy[iHoistway].i_car = iHoistway * 2;
	}
	for (;iHoistway<hwy;iHoistway++)
	{
		Hwy[iHoistway].n_car_in_hwy= 1;
		Hwy[iHoistway].i_car = n2D_hwy + iHoistway;
	}
}

// Check assignment of assigned car and the other car in the same hoistway
// at floor fl with direction dir.
void CMy2DSimDlg::CheckAssignments(int c, int flr, int dir, int p)
{
	int other;
	CString str;
	int i;
	BOOL bCond;
	
	if(Car[c].lowerOrUpper == LOWER_CAR)
	{
		if(Car[c+1].lowerOrUpper == UPPER_CAR)
			other = c+1;
		else
			other = MAXCAR;
	}
	else
	{
		other = c-1;
	}
	
	Pax[p].violation = NULL;
	// NULL
	for(i=0, bCond=FALSE; i<n_car; i++)
	{
		if( i == c )
			continue;
		if(Car[i].n_UpCall[flr] || Car[i].n_DnCall[flr] ||
			(((Car[i].oper == 1) || (Car[i].oper == 2) || (Car[i].oper == 3)) && (flr == Car[i].floor)))
		{
			bCond = TRUE;
			break;
			
		}
	}
	if((bCond) ||
		((Car[c].n_UpCall[flr] && (dir == DN)) || (Car[c].n_DnCall[flr] && (dir == UP))) ||
		(((Car[c].oper == 1) || (Car[c].oper == 2) || (Car[c].oper == 3)) && (flr == Car[c].floor) && (dir == -Car[c].dir)))
	{
		Pax[p].violation |= VIOLATION_NULL;
		fprintf(f_ErrorLog,
			"CheckAssignments:\nNULL - Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!\n",
			c, flr, dir, Sys.t_nextEvent, Car[c].oper);
		str.Format("NULL: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!",
			c, flr, dir, Sys.t_nextEvent, Car[c].oper);
		if(m_violationHCD & VIOLATION_NULL)
			DisplayWarningList(str);
	}
	
	// H: ???? ???? ???? ?????????????? ???? ???? ???? ???????? ???????
	if((Car[other].n_UpCall[flr] || Car[other].n_DnCall[flr]) ||
		(((Car[other].oper == 1) || (Car[other].oper == 2) || (Car[other].oper == 3)) && (flr == Car[other].floor)) ||
		((Car[c].n_UpCall[flr] && (dir == DN)) || (Car[c].n_DnCall[flr] && (dir == UP))) ||
		(((Car[c].oper == 1) || (Car[c].oper == 2) || (Car[c].oper == 3)) && (dir == -Car[c].dir) && Car[c].floor == flr))
	{
		Pax[p].violation |= VIOLATION_H;
		fprintf(f_ErrorLog,
			"H: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!\n",
			c, flr, dir, Sys.t_nextEvent, Car[c].oper);
		str.Format("H: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!",
			c, flr, dir, Sys.t_nextEvent, Car[c].oper);
		if(m_violationHCD & VIOLATION_H)
			DisplayWarningList(str);
		// HC: ???? ???? ???? ?????????????? ???????? ???? ???????? ???? ???????? ???????
		if(((Car[c].n_UpCall[flr] && (dir == DN)) || (Car[c].n_DnCall[flr] && (dir == UP))) ||
			(((Car[c].oper == 1) || (Car[c].oper == 2) || (Car[c].oper == 3)) && (flr == Car[c].floor) && (dir == -Car[c].dir)))
		{
			Pax[p].violation |= VIOLATION_HC;
			fprintf(f_ErrorLog,
				"HC: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!\n",
				c, flr, dir, Sys.t_nextEvent, Car[c].oper);
			str.Format("HC: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!",
				c, flr, dir, Sys.t_nextEvent, Car[c].oper);
			if(m_violationHCD & VIOLATION_HC)
				DisplayWarningList(str);
		}
		// HD: ???? ???? ???? ?????????????? ???? ???? ???? ???? ???????? ???????? ???????
		if((Car[other].n_UpCall[flr] && (dir == UP)) || (Car[other].n_DnCall[flr] && (dir == DN)) ||
			(((Car[other].oper == 1) || (Car[other].oper == 2) || (Car[other].oper == 3)) && (flr == Car[other].floor) && (Car[other].dir == dir)))
		{
			Pax[p].violation |= VIOLATION_HD;
			fprintf(f_ErrorLog,
				"HD: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!\n",
				c, flr, dir, Sys.t_nextEvent, Car[c].oper);
			str.Format("HD: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!",
				c, flr, dir, Sys.t_nextEvent, Car[c].oper);
			if(m_violationHCD & VIOLATION_HD)
				DisplayWarningList(str);
		}
	}
	// C: ???? ???? ???? ?????????????? ???? ???? ???? ???????? ?????? ???? ?????????????? ???? ???? ???? ???????? ???? ???????? ???????
	for(i=0, bCond=FALSE; i<n_car; i++)
	{
		if( i == c )
			continue;
		if((Car[i].lowerOrUpper == Car[c].lowerOrUpper) &&
			((Car[i].n_UpCall[flr] || Car[i].n_DnCall[flr]) ||
			(((Car[i].oper == 1) || (Car[i].oper == 2) || (Car[i].oper == 3)) && (flr == Car[i].floor))))
		{
			bCond = TRUE;
			break;
		}
	}
	if((bCond) ||
		((Car[c].n_UpCall[flr] && (dir == DN)) || (Car[c].n_DnCall[flr] && (dir == UP))) ||
		(((Car[c].oper == 1) || (Car[c].oper == 2) || (Car[c].oper == 3)) && (flr == Car[c].floor) && (dir == -Car[c].dir)))
	{
		Pax[p].violation |= VIOLATION_C;
		fprintf(f_ErrorLog,
			"C: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!\n",
			c, flr, dir, Sys.t_nextEvent, Car[c].oper);
		str.Format("C: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!",
			c, flr, dir, Sys.t_nextEvent, Car[c].oper);
		if(m_violationHCD & VIOLATION_C)
			DisplayWarningList(str);
		// CD: ???? ???? ???? ?????????????? ???? ???? ???? ???????? ???? ???????? ???????
		for(i=0; i<n_car; i++)
		{
			if( i == c )
				continue;
			if((Car[i].lowerOrUpper == Car[c].lowerOrUpper) &&
				((Car[i].n_UpCall[flr] && dir == UP) || (Car[i].n_DnCall[flr]) && dir == DN) ||
				(((Car[i].oper == 1) || (Car[i].oper == 2) || (Car[i].oper == 3)) && (flr == Car[i].floor) && (Car[i].dir == dir)))
			{
				Pax[p].violation |= VIOLATION_CD;
				fprintf(f_ErrorLog,
					"CD: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!\n",
					c, flr, dir, Sys.t_nextEvent, Car[c].oper);
				str.Format("CD: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!",
					c, flr, dir, Sys.t_nextEvent, Car[c].oper);
				if(m_violationHCD & VIOLATION_CD)
					DisplayWarningList(str);
				break;
				
			}
		}
	}
	// D: ???? ???? ???? ?????????????? ?? ???? ???? ?????????????? ???? ???? ???? ???????? ???? ???????? ???????
	for(i=0; i<n_car; i++)
	{
		if( i == c )
			continue;
		if(((Car[i].n_UpCall[flr] && dir == UP) || (Car[i].n_DnCall[flr]) && dir == DN) ||
			(((Car[i].oper == 1) || (Car[i].oper == 2) || (Car[i].oper == 3)) && (flr == Car[i].floor) && (Car[i].dir == dir)))
		{
			Pax[p].violation |= VIOLATION_D;
			fprintf(f_ErrorLog,
				"D: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!\n",
				c, flr, dir, Sys.t_nextEvent, Car[c].oper);
			str.Format("D: Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!",
				c, flr, dir, Sys.t_nextEvent, Car[c].oper);
			if(m_violationHCD & VIOLATION_D)
				DisplayWarningList(str);
			break;
			
		}
	}
	
	// HCD: This may be occur in very heavy traffic situation.
	// Dispatcher system will decide the squence of service for the same floor and same direction and same car.
	// Later, this will be implemented when it is needed.
	
	if(Pax[p].violation)
	{
		fprintf(f_ErrorLog,
			"VIOLATION(%x): Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!\n",
			Pax[p].violation, c, flr, dir, Sys.t_nextEvent, Car[c].oper);
		str.Format("VIOLATION(%x): Car(%d) Floor(%d) Dir(%d) Time(%f) OPER(%d)!",
			Pax[p].violation, c, flr, dir, Sys.t_nextEvent, Car[c].oper);
		if(m_violationHCD)
			DisplayWarningList(str);
	}
	//check
	if(Pax[p].violation & VIOLATION_HCD)
	{
		if(!(Pax[p].violation & VIOLATION_HC))
		{
			str.Format("HCD but not HC!");
			DisplayWarningList(str);
		}
		if(!(Pax[p].violation & VIOLATION_HD))
		{
			str.Format("HCD but not HD!");
			DisplayWarningList(str);
		}
		if(!(Pax[p].violation & VIOLATION_CD))
		{
			str.Format("HCD but not CD!");
			DisplayWarningList(str);
		}
		if(!(Pax[p].violation & VIOLATION_H))
		{
			str.Format("HCD but not H!");
			DisplayWarningList(str);
		}
		if(!(Pax[p].violation & VIOLATION_C))
		{
			str.Format("HCD but not C!");
			DisplayWarningList(str);
		}
		if(!(Pax[p].violation & VIOLATION_D))
		{
			str.Format("HCD but not D!");
			DisplayWarningList(str);
		}
		if(!(Pax[p].violation & VIOLATION_NULL))
		{
			str.Format("HCD but not NULL!");
			DisplayWarningList(str);
		}
	}
	if(Pax[p].violation & VIOLATION_HC)
	{
		if(!(Pax[p].violation & VIOLATION_H))
		{
			str.Format("HC but not H!");
			DisplayWarningList(str);
		}
		if(!(Pax[p].violation & VIOLATION_C))
		{
			str.Format("HC but not C!");
			DisplayWarningList(str);
		}
		if(!(Pax[p].violation & VIOLATION_NULL))
		{
			str.Format("HC but not NULL!");
			DisplayWarningList(str);
		}
	}
	if(Pax[p].violation & VIOLATION_HD)
	{
		if(!(Pax[p].violation & VIOLATION_H))
		{
			str.Format("HD but not H!");
			DisplayWarningList(str);
		}
		if(!(Pax[p].violation & VIOLATION_D))
		{
			str.Format("HD but not D!");
			DisplayWarningList(str);
		}
		if(!(Pax[p].violation & VIOLATION_NULL))
		{
			str.Format("HD but not NULL!");
			DisplayWarningList(str);
		}
	}
	if(Pax[p].violation & VIOLATION_CD)
	{
		if(!(Pax[p].violation & VIOLATION_C))
		{
			str.Format("CD but not C!");
			DisplayWarningList(str);
		}
		if(!(Pax[p].violation & VIOLATION_D))
		{
			str.Format("CD but not D!");
			DisplayWarningList(str);
		}
		if(!(Pax[p].violation & VIOLATION_NULL))
		{
			str.Format("CD but not NULL!");
			DisplayWarningList(str);
		}
	}
	if(Pax[p].violation & VIOLATION_H)
	{
		if(!(Pax[p].violation & VIOLATION_NULL))
		{
			str.Format("H but not NULL!");
			DisplayWarningList(str);
		}
	}
	if(Pax[p].violation & VIOLATION_C)
	{
		if(!(Pax[p].violation & VIOLATION_NULL))
		{
			str.Format("C but not NULL!");
			DisplayWarningList(str);
		}
	}
	if(Pax[p].violation & VIOLATION_D)
	{
		if(!(Pax[p].violation & VIOLATION_NULL))
		{
			str.Format("D but not NULL!");
			DisplayWarningList(str);
		}
	}
}

void CMy2DSimDlg::CheckRejection(int c)
{
	int flr;
	int dir;
	int i, other;
	int hwy;
	
	flr = Car[c].floor;
	dir = Car[c].dir;
	hwy = Car[c].i_hwy;
	if (Car[c].n_pax_in_car >= Param.capacity)
		return;
	
	if(Car[c].lowerOrUpper == LOWER_CAR)
	{
		if(Car[c+1].lowerOrUpper == UPPER_CAR)
			other = c+1;
		else
			other = MAXCAR;
	}
	else
	{
		other = c-1;
	}
	
	// NULL_MASK
	for(i=0; i<n_car; i++)
	{
		if(( i == c ) || (Car[i].lowerOrUpper == Car[c].lowerOrUpper))
			continue;
		if(dir == UP)
		{
			if(Car[i].q_DnCall[flr] >= 0)
			{
				SetRejection(c, flr, NULL_MASK, Car[i].q_DnCall[flr]);
			}
		}
		else
		{
			if(Car[i].q_UpCall[flr] >= 0)
			{
				SetRejection(c, flr, NULL_MASK, Car[i].q_UpCall[flr]);
			}
		}
	}
	// H_MASK
	if(Car[other].q_UpCall[flr] >= 0)
		SetRejection(c, flr, H_MASK, Car[other].q_UpCall[flr]);
	if(Car[other].q_DnCall[flr] >= 0)
		SetRejection(c, flr, H_MASK, Car[other].q_DnCall[flr]);
	
	// C_MASK
	for(i=0; i<n_car; i++)
	{
		if(( i == c ) || (Car[i].lowerOrUpper != Car[c].lowerOrUpper))
			continue;
		if(Car[i].q_UpCall[flr] >= 0)
			SetRejection(c, flr, C_MASK, Car[i].q_UpCall[flr]);
		if(Car[i].q_DnCall[flr] >= 0)
			SetRejection(c, flr, C_MASK, Car[i].q_DnCall[flr]);
	}
	
	// D_MASK
	for(i=0; i<n_car; i++)
	{
		if(i == c)
			continue;
		if(dir == UP)
		{
			if(Car[i].q_UpCall[flr] >= 0)
			{
				SetRejection(c, flr, D_MASK, Car[i].q_UpCall[flr]);
			}
		}
		else
		{
			if(Car[i].q_DnCall[flr] >= 0)
			{
				SetRejection(c, flr, D_MASK, Car[i].q_DnCall[flr]);
			}
		}
	}

	//HC_MASK
	if((dir == UP) && (Car[c].q_DnCall[flr] >= 0))
		SetRejection(c, flr, HC_MASK, Car[c].q_DnCall[flr]);
	if((dir == DN) && (Car[c].q_UpCall[flr] >= 0))
		SetRejection(c, flr, HC_MASK, Car[c].q_UpCall[flr]);
	//HD_MASK
	if((dir == UP) && (Car[other].q_UpCall[flr] >= 0))
		SetRejection(c, flr, HD_MASK, Car[other].q_UpCall[flr]);
	if((dir == DN) && (Car[other].q_DnCall[flr] >= 0))
		SetRejection(c, flr, HD_MASK, Car[other].q_DnCall[flr]);
	//CD_MASK
	for(i=0; i<n_car; i++)
	{
		if(( i == c ) || (Car[i].lowerOrUpper != Car[c].lowerOrUpper))
			continue;
		if((dir == UP) && (Car[i].q_UpCall[flr] >= 0))
			SetRejection(c, flr, CD_MASK, Car[i].q_UpCall[flr]);
		if((dir == DN) && (Car[i].q_DnCall[flr] >= 0))
			SetRejection(c, flr, CD_MASK, Car[i].q_DnCall[flr]);
	}
}

void CMy2DSimDlg::SetRejection(int c, int flr, BYTE mask, int iPax)
{
	int p;
	CString str;
	CString strMask;

	for(p=iPax; p>=0; p=Pax[p].q_next)
	{
		Pax[p].rejection |= mask;
#ifdef SHOW_REJECTION_WARNING
		strMask = "";
		if((mask & NULL_MASK) && (m_violationHCD & VIOLATION_NULL))
			strMask += "N ";
		if((mask & H_MASK) && (m_violationHCD & VIOLATION_H))
			strMask += "H ";
		if((mask & C_MASK) && (m_violationHCD & VIOLATION_C))
			strMask += "C ";
		if((mask & D_MASK) && (m_violationHCD & VIOLATION_D))
			strMask += "D ";
		if((mask & HC_MASK) && (m_violationHCD & VIOLATION_HC))
			strMask += "HC ";
		if((mask & HD_MASK) && (m_violationHCD & VIOLATION_HD))
			strMask += "HD ";
		if((mask & CD_MASK) && (m_violationHCD & VIOLATION_CD))
			strMask += "CD ";
		if(strMask != "")
		{
			str.Format("Rejection: When car(%d) serves fl(%d), p(%d) from %d to %d A(%d) by mask(%s)",
				c, flr-1, p, Pax[p].f_origin-1, Pax[p].f_dest-1, Pax[p].assignment, strMask);
			DisplayWarningList(str);
#ifdef SHOW_REJECTION_WARNING_DIALOG
			MessageBox(str);
#endif
		}
#endif
	}
}



double CMy2DSimDlg::HardRunTime(int origin, int dest)
{
	int dist;
	
	dist = abs(origin - dest);
	switch (dist) {
	case 0:
		return 0.0;
	case 1:
		return 5.4;
	case 2:
		return 7.2;
	case 3:
		return 7.8;
	default:
		return 5.4 + (double) dist;
	}
}

int CMy2DSimDlg::NextCommittableFloor(int c, int *into_decel, int should_be_in_flight)
{
	double t_into_run;
	int dist;
	
	*into_decel = 0;  // indicates if car is already decelerating into target
	if (Car[c].oper != RUNNING && Car[c].oper != FORCED_MOVE && Car[c].oper != MOVE_CAR_RANGE) {
		if (should_be_in_flight)
			fprintf(f_ErrorLog, "NextCommittableFloor 1: car %d in operation %d at t=%lf\n",
			c, Car[c].oper, Sys.t_nextEvent);
		return Car[c].floor;
	}
	else if (Car[c].target == Car[c].floor) {
		fprintf(f_ErrorLog, "NextCommittableFloor 2: car %d at %d at t=%lf\n",
			c, USERFLOOR(Car[c].target), Sys.t_nextEvent);
		return Car[c].floor;
	}
	else {
		t_into_run = Sys.t_nextEvent - Car[c].t_run_start;
		if (should_be_in_flight &&
			(t_into_run > HardRunTime(Car[c].floor, Car[c].target) + EPSILON || t_into_run < -EPSILON))
			fprintf(f_ErrorLog, "NextCommittableFloor 3: car %d, time into run %lf vs %lf at t=%lf\n",
			c, t_into_run, HardRunTime(Car[c].floor, Car[c].target), Sys.t_nextEvent);
		if (t_into_run < 2.75)
			dist = 1;
		else if (t_into_run < 3.65)
			dist = 2;
		else if (t_into_run < 3.95)
			dist = 3;
		else
			dist = (int) (t_into_run + 0.249999);
		
		if (dist > abs(Car[c].floor - Car[c].target)) {
			*into_decel = 1;
			return Car[c].target;
		}
		else if (Car[c].target > Car[c].floor)
			return Car[c].floor + dist;
		else
			return Car[c].floor - dist;
	}
}

int CMy2DSimDlg::DisplayedPosition(int c)
{
	double t_into_run;
	int run_dist, dir, dist;
	
	if ((Car[c].oper != RUNNING) && (Car[c].oper != FORCED_MOVE) && (Car[c].oper != MOVE_CAR_RANGE))
		return Car[c].floor;
	else {
		t_into_run = Sys.t_nextEvent - Car[c].t_run_start;
		if (t_into_run > HardRunTime(Car[c].floor, Car[c].target) + EPSILON || t_into_run < -EPSILON)
			fprintf(f_ErrorLog, "DisplayedPosition 1: car %d, time into run %lf vs %lf at t=%lf\n",
			c, t_into_run, HardRunTime(Car[c].floor, Car[c].target), Sys.t_nextEvent);
		
		if (Car[c].floor <= Car[c].target) {
			run_dist = Car[c].target - Car[c].floor;
			dir = 1;
		}
		else {
			run_dist = Car[c].floor - Car[c].target;
			dir = -1;
		}
		
		if (t_into_run < 2.7)
			dist = 0;
		else if (run_dist == 2) {
			if (t_into_run < 4.3)
				dist = 1;
			else
				dist = 2;
		}
		else if (t_into_run < 4.2)
			dist = 1;
		else if (run_dist == 3) {
			if (t_into_run < 5.5)
				dist = 2;
			else
				dist = 3;
		}
		else {
			dist = 2 + (int) (t_into_run - 4.2);
			if (dist < 0 || dist > run_dist + 3)
				fprintf(f_ErrorLog, "DisplayedPosition 2: car %d, time into run %lf %d %d at t=%lf\n",
				c, t_into_run, dist, run_dist, Sys.t_nextEvent);
			if (dist <= run_dist - 1)
				;
			else if (t_into_run < (double) (run_dist) + 2.6)
				dist = run_dist - 1;
			else
				dist = run_dist;
		}
		return Car[c].floor + dir*dist;
	}
}

double CMy2DSimDlg::HardCommitTime(int origin, int dest)
{
	int dist;
	
	dist = abs(origin - dest);
	switch (dist) 
	{
		/*
	case 0:
		return 0.0;
	case 1:
		return 2.75;
	case 2:
		return 3.65;
	case 3:
		return 3.95;
	default:
		return 0.75 + (double) dist;
		*/
	case 0:
		return 0.0;
	case 1:
		return 2.9;
	case 2:
		return 3.7;
	case 3:
		return 4.4;
	default:
		return 0.9 + (double) dist;
	}
}


// This function sets the car range for a given car.  The target of the car may be revised
// according to the range.  Currently, the functionality of "door_position" is not implemented.
void CMy2DSimDlg::SetCarRange(int c, int bottom, int top, int door_position) {
  if (c < 0 || c >= n_car)
	fprintf(f_ErrorLog, "SetCarRange 1: c=%c t=%5.3lf\n", c, Sys.t_nextEvent);
  else {
	Car[c].range_bottom = bottom;
	Car[c].range_top = top;
// Wake up car waiting for range adjustment or possibly reset target of car in flight
    UpdateTarget(c, 0);
  }
}  // SetCarRange


void CMy2DSimDlg::ProcessDispatcher(int mode)
{
		// shs0708 This code is obsolete or used without HM?
	/*
	int dispatchResult;
	if(m_bDispatcherAttached)
	{
		//UpdateFloorForAllCars();
		dispatchResult=PreprocessDispatcher(mode);	// send status to dispatcher
		PostprocessDispatcher(dispatchResult); // receive data from dispathcer & process
		clearCommunicationContents();
	}
	*/
}

// mode : DISPATCH_NEW_ASSIGN - preprocess and call dispatcher at new passenger request
//			DISPATCH_NO_ASSIGN - preprocessing and call dispatcher at no new passenger request
UINT CMy2DSimDlg::PreprocessDispatcher(int mode)
{
		// shs0708 This code is obsolete or used without HM?
	/*
	UINT retval;
	CAR_STATE d_Car[MAXCAR];
	double currentTime;
	int i;

	currentTime=Sys.t_nextEvent;
	for (i=0;i<n_car;i++)
	{
		memcpy(&d_Car[i],&Car[i],sizeof(CAR_STATE));
	}
	*/
/*
	if (mode==DISPATCH_NEW_ASSIGN)
	{	
		d_NoOfNewPassengers=1; 
		d_passengers.nCurrentCalls = d_NoOfNewPassengers;
		for (int i=0;i<d_NoOfNewPassengers;i++)
		{
			d_passengers.passengerCall[i].originFloor=Pax[i_pax_recent+i].f_origin;
			d_passengers.passengerCall[i].destFloor=Pax[i_pax_recent+i].f_dest;
			d_passengers.passengerCall[i].passengerID=i_pax_recent+i;
			d_passengers.passengerCall[i].nPassengers=1;// currently 1.
		}
	}
	else if (mode==DISPATCH_NO_ASSIGN)
	{
		d_passengers.nCurrentCalls=-1;  // specify it is not new assignement getting call for dispatcher
	}
*/
	// Move Updates
	// shs0708 obsolete function to call dispatcher
	/*
	d_moveCommands.iMove=i_move;

	retval=(GetDispatcherDecision)(currentTime,d_Car,&d_passengers,
			&d_moveCommands,&d_motionCommands,&d_extraMsgs);
	*/
	//return retval;
	return 0;
	
}

// shs : should return error code - change it afterwards
void CMy2DSimDlg::PostprocessDispatcher(UINT dispatchResult)
{
		// shs0708 obsolete function to precess dispatcher decision(old)
/*
	if (dispatchResult & 0x01) // new assignments
	{
		ProcessPassenger(&d_passengers);
	}
	if (dispatchResult & 0x02) // move specified
	{
		ProcessMoveCommands(&d_moveCommands);
	}
	if (dispatchResult & 0x04) // motion profile generated
	{}
	if (dispatchResult & 0x08) // extra msgs
	{
		DecodingMsgs(&d_extraMsgs);
	}
	*/
}

int CMy2DSimDlg::SelectDispatchingAlgorithm(char info[][50])
{
	CDlgSelectAlgorithm dlgsel;
	
	dlgsel.SetInfo(info);
	if (dlgsel.DoModal()==IDOK)
	{
		Param.algorithmID=dlgsel.mSelected;
		return 1;
	}
	else
		return -1;
	
}

// shs0708 Update Function
// Link Dispatcher DLL and Get procedure name
int CMy2DSimDlg::LinkDispatcherDLL(CString DLLFileName, int AlgorithmID)
{
	CString cstr;

	m_bDispatcherAttached = FALSE;
	if (hinstLib != NULL)
		AfxFreeLibrary(hinstLib);

	hinstLib=AfxLoadLibrary(DLLFileName);
	if (hinstLib != NULL)
	{
		GDInitialize=(GDInitialize_T) GetProcAddress(hinstLib,"GDInitialize");
		GDAlgorithmInitialize=(GDAlgorithmInitialize_T)
			GetProcAddress(hinstLib,"GDAlgorithmInitialize");
		DispatchNewCall=(DispatchNewCall_T) GetProcAddress(hinstLib,"DispatchNewCall");

		if (GDInitialize != NULL && DispatchNewCall != NULL && GDAlgorithmInitialize != NULL)
		{
			m_bDispatcherAttached = TRUE;
		}
	}

	if (m_bDispatcherAttached==FALSE)
	{
		if (hinstLib!=NULL)
			FreeLibrary(hinstLib);
		cstr.Format("Fail to Load DLL (%s)",DLLFileName);
		MessageBox(cstr);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CMy2DSimDlg::RedisplayCaption()
{
	CString strCaption;

	strCaption = "2-D Simulation ";
	
	if (!m_strTrafficProfileName.IsEmpty())
	{
		strCaption +=m_strTrafficProfileName;
	}

	if (Param.bAutomaticDispatching==TRUE)
	{
		strCaption +="(";
		strCaption +=Param.dispatchingAlgorithmName;
		strCaption +=")";
	}
	else
	{
		strCaption +="(";
		strCaption += "Manual Dispatching";
		strCaption += ")";
	}
	SetWindowText(strCaption);
}

void CMy2DSimDlg::DetachDispatcher()
{
	if (hinstLib!=NULL)
	{
		AfxFreeLibrary(hinstLib);
		hinstLib=NULL;
	}
	m_bDispatcherAttached = FALSE;
}

// Update Move structure by move
// if there is no move specified in Move then Insert Move
// If there is collision or match, then update Move
void CMy2DSimDlg::UpdateMove(MOVE_PARAMETER move)
{
	/*
	int carno, moveindex,index;
	int flag; //0, non-zero-processed
	int i;
	flag=0;
	// Check Current Move in Car - Update or delete
	carno=move.car;
	moveindex=Car[carno].i_move;

	if (moveindex>=0 && moveindex<n_move)
	{  // Valid move index ?
		if (IsValidMoveOperation(Car[carno].oper)) // current car in move?
		{	// ONLY CONSIDER FORCED MOVE
			if (move.t_initiation >= Move[moveindex].t_initiation)
			{   // valid move operation to compare in car
				if (move.t_initiation <= Sys.t_nextEvent+EPSILON)
				{	// current move command update needed
					if (Move[moveindex].destination==move.destination)
					{ // same destination
						Move[moveindex].t_release=move.t_release;
					}
					else
					{ // destination updated
						Move[moveindex].destination=move.destination;
						Move[moveindex].t_release=move.t_release;
					}
					//UpdateTarget(carno,Car[carno].oper); // shs move?
					flag=1;
				}
			}
		}
	}

	// future move and not processed - replace current
	if (move.t_initiation >= Sys.t_nextEvent && !flag)
	{	// Overwrite move in move from i_move
		int index2;
		for(index=0; index<n_move; index++)
		{
			if (move.t_initiation < Move[index].t_initiation)
				break;  // timing is less than Move[index]
		}
		for (index2=n_move-1;index2>index;index2--)
		{
			if (move.t_initiation > Move[index2].t_initiation)
				break;
		}
		
		if (i_move>0) // check same value?-->Update
		{
			for (i=index;i<=index2;i++)
			{
				if (move.car == Move[i].car) // for same car?
				{
					if (move.t_initiation >= Sys.t_nextEvent-EPSILON &&
						move.t_initiation <= Sys.t_nextEvent+EPSILON) // current time
					{
						memcpy(&Move[i],&move,sizeof(MOVE_PARAMETER)); // update
						flag=2;
						break;
					}
					
				}
			}
		}

		if (!flag) // Add Move
		{
			n_move++;
			if (n_move>=33)
				n_move=n_move;
			if (n_move>=MAXMOVE) 
				return;  // shs break points
			for(int j=n_move-1; j>index; j--)
			{
				Move[j].car = Move[j-1].car;
				Move[j].destination = Move[j-1].destination;
				Move[j].min_dwell = Move[j-1].min_dwell;
				Move[j].t_initiation = Move[j-1].t_initiation;
				Move[j].initiation_basis = Move[j-1].initiation_basis;
				Move[j].initiation_threshold = Move[j-1].initiation_threshold;
				Move[j].initiation_relativity = Move[j-1].initiation_relativity;
				Move[j].initiation_direction = Move[j-1].initiation_direction;
				Move[j].initiation_delay = Move[j-1].initiation_delay;
				Move[j].t_release = Move[j-1].t_release;
				Move[j].release_basis = Move[j-1].release_basis;
				Move[j].release_threshold = Move[j-1].release_threshold;
				Move[j].release_relativity = Move[j-1].release_relativity;
				Move[j].release_direction = Move[j-1].release_direction;
				Move[j].release_delay = Move[j-1].release_delay;
			}
			Move[index].car = move.car;
			Move[index].destination = move.destination;
			Move[index].min_dwell = move.min_dwell;
			Move[index].t_initiation = move.t_initiation;
			Move[index].initiation_basis = move.initiation_basis;
			Move[index].initiation_threshold = move.initiation_threshold;
			Move[index].initiation_relativity = move.initiation_relativity;
			Move[index].initiation_direction = move.initiation_direction;
			Move[index].initiation_delay = move.initiation_delay;
			Move[index].t_release = move.t_release;
			Move[index].release_basis = move.release_basis;
			Move[index].release_threshold = move.release_threshold;
			Move[index].release_relativity = move.release_relativity;
			Move[index].release_direction = move.release_direction;
			Move[index].release_delay = move.release_delay;
		}
	}
	*/
}



BOOL CMy2DSimDlg::IsValidMoveOperation(int caroperation)
{
	switch (caroperation)
	{
	case FORCED_MOVE:
	case FORCED_PARKING :
	case END_OF_FORCED_PARKING :
	case WAIT_INITIATE :
	case WAIT_RELEASE :
		return TRUE;
	default:
		return FALSE;
	}
}

void CMy2DSimDlg::ClearMoveFromN(int fromIndex)
{
	n_move=i_move;
}

void CMy2DSimDlg::ProcessMoveCommands(MOVE_COMMANDS *movecmds)
{
	int move_size;
	int move_no;
	int retval;
	move_size=movecmds->nMoves;

	for (move_no=0;move_no<move_size;move_no++)
	{
		switch(movecmds->Move[move_no].cmd)
		{	
		case MOVE_ADD: // Add move
			retval=AddMove(movecmds->Move[move_no]);
			break;
		case MOVE_DELETE: // Delete Move
			DeleteMove(movecmds->Move[move_no]);
			break;
		}

	}
}
// return 0 : success
// 1 : buffer full
// 2 : duplicated
// -1 : move not good (car> no of car),etc
int CMy2DSimDlg::AddMove(MOVE_PARAMETER move)
{
	int index1,index2,i;

	if (n_move >= MAXMOVE)
	{
		return 1;
	}
	if (move.car > n_car || move.destination > n_floor || move.destination < 0)
	{  // invalid move
		return -1;
	}

	if (n_move>0) 
	{	// if number of move is more than 0, then search duplicated one and not add
		// search index Move's time is later than the move 
		for(index1=0; index1 < n_move; index1++)
		{
			if(move.t_initiation < Move[index1].t_initiation)
			{
				break;
			}
		}
		// search index of Move's time is earlier than move
		for (index2=index1-1;index2>=0;index2--)
		{
			if (move.t_initiation > Move[index2].t_initiation)
			{
				break;
			}
		}
		// index1<n_move,index1==n_move==>no match larger than move
		// index2 <0   no match less than move
		if (index1<=n_move || index2>=0)
		{	// remove duplication
			for (i=index2+1;i<index1;i++)
			{
				if (CompareMove(move,Move[i])==0) // same
					return 2;  // duplicated - No Add
			}
		}
	}
	else
	{
		index1=0;
	}
	n_move++;
	for (i=n_move-1;i>index1;i--)
	{
		memcpy(&(Move[i]),&(Move[i-1]),sizeof(MOVE_PARAMETER));
	}
	memcpy(&(Move[index1]),&move,sizeof(MOVE_PARAMETER));
	
	return 0;
}

// return 0 : success
// return 1 : no move to delete
int CMy2DSimDlg::DeleteMove(MOVE_PARAMETER move)
{
	int i,j,matchFlag;
	i=0;j=0,matchFlag=0;
	for (i=i_move;i<n_move;i++)  // delete only not processed
	{
		if (CompareMove(move,Move[i])==0) // match
		{  // delete move
			for (j=i;j<n_move-1;j++)
			{
				memcpy(&(Move[j]),&(Move[j+1]),sizeof(MOVE_PARAMETER));
			}
			n_move--; // delete one
			matchFlag=1;
		}
	}
	if (matchFlag==0)
	{	// no match to delete
		return 1;
	}
	else
	{	// delete
		return 0;
	}
}
// return 0 : if move1==move2
// return 1 : if move1 != move2
int CMy2DSimDlg::CompareMove(MOVE_PARAMETER move1, MOVE_PARAMETER move2)
{
	if (move1.car==move2.car)
	{
		if (move1.destination==move2.destination)
		{
			if (move1.min_dwell==move2.min_dwell)
			{
				if (move1.initiation_basis==move2.initiation_basis
					&& move1.release_basis == move2.release_basis)
				{
					if (move1.t_initiation == move2.t_initiation &&
						move1.t_release == move2.t_release)
					{
						if (move1.initiation_threshold == move2.initiation_threshold &&
							move1.release_threshold==move2.release_threshold &&
							move1.initiation_direction == move2.initiation_direction &&
							move1.release_direction == move2.release_direction &&
							move1.initiation_relativity == move2.initiation_relativity &&
							move1.release_relativity == move2.release_relativity &&
							move1.initiation_delay == move2.initiation_delay &&
							move1.release_delay == move2.release_delay)
						{
							return 0;
						}
					}
				}
			}
		}
	}
	return 1;
}

// when passenger arrival, passenger enter his destination and number of passenger
//  and DES send destination entry item to get assignments from Dispatcher
void CMy2DSimDlg::EnterDestinationEntry(int passId, int origin, int dest, int ncount,int callType)
{
	int nQueue;
	if (Param.bAutomaticDispatching==TRUE)
	{
		nQueue=d_passengers.nCurrentCalls;
		if (passId<MAXPAX && passId>=0)
		{
			d_passengers.passengerCall[nQueue].originFloor=origin;
			d_passengers.passengerCall[nQueue].destFloor=dest;
			d_passengers.passengerCall[nQueue].assignment=-1;
			d_passengers.passengerCall[nQueue].callType=callType;
			d_passengers.passengerCall[nQueue].nPassengers=ncount;
			d_passengers.passengerCall[nQueue].passengerID=passId;
			d_passengers.passengerCall[nQueue].cmd=PAX_DE_ENTERED;	
		}
		d_passengers.nCurrentCalls++;
	}
}

// Dispatcher assign car to destination entry item or command to delete or to do something
int CMy2DSimDlg::AssignDestinationEntry(int carID, int passID,int cmd)
{	// temporary support only assign
	switch (cmd)
	{
	case 1:	// assign
		if (passID>=0 && passID <=i_pax)
			Pax[passID].assignment=carID;
		break;

	case 2: // deassign
	case 3: // reassign
		break;
	}
	return 3; // accept
}

// send move command with acceptance status or adding/delete commands for dispatcher
void CMy2DSimDlg::SendMove(MOVE_PARAMETER move,int cmd)
{
	int n;
	if (Param.bAutomaticDispatching==TRUE)
	{
		n=d_moveCommands.nMoves;
		if (n<MAXMOVE)
		{
			memcpy(&d_moveCommands.Move[n],&move,sizeof(MOVE_PARAMETER));
			d_moveCommands.Move[n].cmd=cmd;
			d_moveCommands.nMoves++;
		}
	}
}

// send extra message or status for dispatcher
void CMy2DSimDlg::SendExtraMsg(int id, int len, char *data)
{
	if (Param.bAutomaticDispatching==TRUE)
	{

	}
}

// Process extra message from dispatcher
void CMy2DSimDlg::ProcessExtraMsg(char msg[])
{

}

void CMy2DSimDlg::clearCommunicationContents()
{
	d_passengers.nCurrentCalls=0;
	d_motionCommands.nCmds=0;
	d_moveCommands.nMoves=0;
	d_extraMsgs.nBytes=0;
}

void CMy2DSimDlg::ProcessPassenger(PASSENGER_STATUS *passengers)
{
	int passengerSize=passengers->nCurrentCalls;
	int passID;
	int i;

	for (i=0;i<passengerSize;i++)
	{	// instant call assignments
		if (passengers->passengerCall[i].cmd==PAX_ASSIGN)
		{
			passID=passengers->passengerCall[i].passengerID;
			if (passID>=0 && passID<MAXPAX)
			{
				Pax[passID].assignment=	passengers->passengerCall[i].assignment;
			}
		}
	}

}

void CMy2DSimDlg::DecodingMsgs(EXTRA_MSGS *msg)
{
	int size=msg->nBytes;
	int count=0;
	unsigned int uintTemp;
	double doubleTemp;
	unsigned char byteTemp[5];
	char *ptrByte=msg->cmdMsg;
	if (size==0) return;
	while (count < size)
	{	
		count++; // id check
		switch (*ptrByte++)
		{
			case	MSGID_SPECIALTIME:	// Special Time setting
				byteTemp[0]=*ptrByte++; count++;	// length
				byteTemp[1]=*ptrByte++;	count++;	// high byte of time * 10
				byteTemp[2]=*ptrByte++; count++;	// low byte of time * 10
				uintTemp=((byteTemp[1])<<8) | (byteTemp[2]); // compose time *10
				doubleTemp=uintTemp/10.0;	// time is divide by 10. 100ms resolution
				SetSpecialTime(doubleTemp);
				break;
			case MSGID_CARRANGE:	// Car Range Setting
				byteTemp[0]=*ptrByte++; count++;
				byteTemp[1]=*ptrByte++;	count++;
				byteTemp[2]=*ptrByte++; count++;
				byteTemp[3]=*ptrByte++; count++;
				SetCarRange((byteTemp[1]&0xf0) >> 4, // car id
					byteTemp[2],byteTemp[3],byteTemp[1]&0x0f); // bottom,top,door
					break;
			default:
				count=size;
				break;
		}
	}
}

int CMy2DSimDlg::SetSpecialTime(double sTime)
{
	if (n_specialEvent >= MAXSPECIALEVENT)
		return -1;
	else
		SpecialEventTime[n_specialEvent++]=sTime;
	return TRUE;
}

void CMy2DSimDlg::UpdateFloorForAllCars()
{
	int c;
	double j,a,v,pos;
	int into_decel; //jsr
	for(c=0; c<n_car; c++)
	{
		GetCurrentMotionState(c, &j, &a, &v, &pos);
		//jsr
		Car[c].currentPos=pos;
		Car[c].NCF=GetNextCommittableFloor(c, &into_decel, 0);
		Car[c].currentFloor = PosToFloor(pos);
	}
	//shin051007 check positions of upper and lower car
	for (int h=0; h < n_hwy; h++)
	{
		if(Hwy[h].n_car_in_hwy >= 2)
		{
			c = Hwy[h].i_car;
			if(Car[c].currentPos>Car[c+1].currentPos)
				fprintf(f_ErrorLog, "UpdateFloorForAllCars: currentPos (%lf > %lf) for hoistway (%d) at t=%lf\n",
				Car[c].currentPos, Car[c+1].currentPos, h, Sys.t_nextEvent);
			if(Car[c].NCF>Car[c+1].NCF)
				fprintf(f_ErrorLog, "UpdateFloorForAllCars: NCF (%d > %d) for hoistway (%d) at t=%lf\n",
				Car[c].NCF, Car[c+1].NCF, h, Sys.t_nextEvent);
			if(Car[c].currentFloor>Car[c+1].currentFloor)
				fprintf(f_ErrorLog, "UpdateFloorForAllCars: currentFloor (%d > %d) for hoistway (%d) at t=%lf\n",
				Car[c].currentFloor, Car[c+1].currentFloor, h, Sys.t_nextEvent);
		}
	}
}

double CMy2DSimDlg::DwellTimeOld(int c)
{
  int f, n_board, max_board, total;
  double min_dwell, dwell;

  if (Car[c].oper != DWELL) {
      fprintf(f_ErrorLog, "DwellTimeOld: car %d in oper %d at t=%lf\n",
	      c, Car[c].oper, Sys.t_nextEvent);
	  return 0.0;
  }

  f = Car[c].floor;
  if (USERFLOOR(f) == Building.lobby1Floor)
  {
	  min_dwell = Param.t_lobby_dwell;
  }
  else
	min_dwell = 2.6;

  if (Car[c].dir == 1)
	n_board = Car[c].n_UpCall[f];
  else if (Car[c].dir == -1)
	n_board = Car[c].n_DnCall[f];
  else
	n_board = 0;
  max_board = Param.capacity - Car[c].n_pax_in_car;
  if (n_board > max_board)
	n_board = max_board;
  total = n_board + Car[c].n_deboard;

  if (total == 0)
	return 0.0;
  else if (total >= 15)
	dwell = 0.5*total + 2.0;
  else if (total >= 9)
	dwell = 0.5*total + 2.1;
  else if (n_board == 0)
      dwell = 0.5*Car[c].n_deboard + 2.1;
  else if (Car[c].n_deboard == 0) {
	if (n_board <= 5)
	  dwell = 4.6;
	else
	  dwell = 0.5*n_board + 2.1;
  }
  else
	dwell = Param.t_hall_max_dwell;

  if (dwell < min_dwell)
	return min_dwell;
  else
	return dwell;
}

double CMy2DSimDlg::DwellTimeCurrent(int fl, int nBoard, int nDeboard)
{
	int total;
	double min_dwell, dwell, lobbyDwell, stopDwell;
	double NorioriTime;
	
	if(nBoard > Param.capacity)
		nBoard = Param.capacity;
	total = nBoard + nDeboard;
	
	if (USERFLOOR(fl) == Building.lobby1Floor || USERFLOOR(fl) == Building.lobby2Floor)
	{
		if((nBoard != 0) && (nDeboard != 0))
			lobbyDwell = Param.t_hall_max_dwell;
		else
			lobbyDwell = Param.t_lobby_dwell;
	}
	else
		lobbyDwell = 0.0;
	
	if (nBoard == 0)
		stopDwell = Param.t_car_min_dwell;
	else if (nDeboard == 0)
		stopDwell = Param.t_hall_min_dwell;
	else
		stopDwell = Param.t_hall_max_dwell;
	
	min_dwell = max(lobbyDwell, stopDwell);
	
	NorioriTime = 1.0;
	dwell = 2.1 + total*NorioriTime/2.0;
	if(dwell > 9.2)
		dwell -= 0.1;

//sprintf(DEBUGMsg, "fl=%d nb=%d ndb=%d lob=%4.2lf stop=%4.2lf min=%4.2lf dwel=%4.2lf",
//fl, nBoard, nDeboard, lobbyDwell, stopDwell, min_dwell, dwell);
//DEBUGWrite(0);
	
	if (dwell < min_dwell)
		return min_dwell;
	else
		return dwell;
}

int CMy2DSimDlg::CommittablePosToNCF(int car, double pos)
{
	int f;
	int carMovingDirection;
	
	//shin050822 Use moving direction to get next committable floor
	if(Car[car].floor < Car[car].target)
		carMovingDirection = UP;
	else if(Car[car].floor > Car[car].target)
		carMovingDirection = DN;

	if(carMovingDirection == UP)
	{
		for(f=0; f<n_floor; f++)
		{
			if(pos <= Building.floorPosition[f])
				return f;
		}
	}
	else if(carMovingDirection == DN)
	{
		for(f=0; f<n_floor; f++)
		{
			if(pos < Building.floorPosition[f])
				return (f-1);
		}
	}
	else
	{
		for(f=0; f<n_floor; f++)
		{
			if(pos < (Building.floorPosition[f] + Building.floorPosition[f+1])/2.0)
				return f;
		}
	}
	return -1;//Error
}


void CMy2DSimDlg::SpyCOGS()
{
	int car;
	try
	{
		FILE *fpos = fopen("c:/dPos.csv","w");
		if(fpos!=NULL)
		{
			fprintf(fpos,"%f,",Sys.t_nextEvent);
			for(car=0;car<MAXCAR;car++)
			{
				fprintf(fpos,"%f,",Car[car].currentPos);
			}	
			fprintf(fpos,"\n");
			fclose(fpos);
		}
	}
	catch(char *str)
	{
		MessageBox(str);
	}

	try
	{
		FILE *fncf = fopen("c:/dNCF.csv","w");
		if(fncf!=NULL)
		{
			fprintf(fncf,"%f,",Sys.t_nextEvent);
			for(car=0;car<MAXCAR;car++)
			{
				int value=Car[car].NCF;
				int state;
				if(Car[car].oper==RUNNING || Car[car].oper==MOVE_CAR_RANGE)
				{
					if(Car[car].t_decel<=Sys.t_nextEvent)
						state=1;
					else
						state=2;
				}
				else
					state=0;
				value+=state<<8;
				fprintf(fncf,"%d,",value);
			}	

			fprintf(fncf,"\n");
			fclose(fncf);
		}
	}
	catch(char *str)
	{
		MessageBox(str);
	}

	try
	{
		FILE *fHandle=fopen("c:/handle.txt","r");
		if(fHandle==NULL)
			return;
		int handle;
		fscanf(fHandle,"%d",&handle);
		fclose(fHandle);
		::SendMessage((HWND)handle,0x401,0,0);
	}
	catch(char *str)
	{
		MessageBox(str);
	}
}
// shs0708 change name and update the function to conform our interface definition
/********************************************************************
* Link Hoistway Plan Executor DLL with name DLLFileName
********************************************************************/
int CMy2DSimDlg::LinkHoistwayPlanExecutor(CString DLLFileName)
{
	BOOL	m_bExecutorAttached=FALSE;
	CString cstr;

	if (hinstExec != NULL)
		AfxFreeLibrary(hinstExec);
	hinstExec=AfxLoadLibrary(DLLFileName);

	if (hinstExec!=NULL)
	{
		HPEInitialize=(HPInitialize_T) GetProcAddress(hinstExec,"HPEInitialize");
		PutCarState=(PutCarState_T) GetProcAddress(hinstExec,"PutCarState");
		MonitorCarState=(MonitorCarState_T) GetProcAddress(hinstExec,"MonitorCarState");

		if (HPEInitialize!=NULL && PutCarState!=NULL && MonitorCarState!=NULL)
		{
			m_bExecutorAttached=TRUE;
		}
	}

	if (m_bExecutorAttached==FALSE)
	{
		if (hinstExec!=NULL)
			FreeLibrary(hinstExec);
		cstr.Format("Fail to Load Executor dll");
		MessageBox(cstr);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

// shs0708 Update Function to conform our interface definition
/*	Link hoistwayplanner dll with name hoistwayPlannerDLL
 *  Function HPInitialize, AcceptAssignment */
int CMy2DSimDlg::LinkHoistwayPlanner(CString hoistwayPlannerDLL)
{
	BOOL m_bHPAttached = FALSE;	// new addition of variable m_bxxxAttached (shs)
	CString cstr;

	if (hinstHPLib != NULL)	// new addition of hinstHPLib <-hinstLib (shs)
		AfxFreeLibrary(hinstHPLib);
	hinstHPLib=AfxLoadLibrary(hoistwayPlannerDLL);

	if (hinstHPLib!=NULL)
	{
		
		HPInitialize=(HPInitialize_T) GetProcAddress(hinstHPLib,"HPInitialize");
		// for manual dispatching to HP (not pass though dispatcher)
		AcceptAssignment=(AcceptAssignment_T) GetProcAddress(hinstHPLib,"AcceptAssignment");

		if (HPInitialize != NULL && AcceptAssignment!=NULL)
		{
			m_bHPAttached=TRUE;
		}
	}

	if (m_bHPAttached==FALSE)
	{
		if (hinstHPLib!=NULL)
			AfxFreeLibrary(hinstHPLib);
		cstr.Format("Fail to Load Planner DLL");
		MessageBox(cstr);
		return FALSE;
	}
	else
	{
		return TRUE; // success
	}	
}

void CMy2DSimDlg::DetachHoistwayPlanner()
{
	if (hinstHPLib!=NULL)
	{
		AfxFreeLibrary(hinstHPLib);
		hinstHPLib=NULL;
	}
}

/*******************************************************************************************
 * A function call from hoistway plan execution to COGS (OCSS)
 * Inform it about the next operation
 ********************************************************************************************/
extern "C"
{
__declspec(dllexport) void OCSSCommand(UCHAR carIndex,
									   CarCommand *oper)
{
	CarCommandFromHPE[carIndex].motionCommand = oper->motionCommand;
	CarCommandFromHPE[carIndex].target = oper->target;
	CarCommandFromHPE[carIndex].runDelay = oper->runDelay;
	CarCommandFromHPE[carIndex].runProfile = oper->runProfile;
	CarCommandFromHPE[carIndex].frontDoorCommand = oper->frontDoorCommand;
	CarCommandFromHPE[carIndex].frontDoorDwell = oper->frontDoorDwell;
	CarCommandFromHPE[carIndex].rearDoorCommand = oper->rearDoorCommand;
	CarCommandFromHPE[carIndex].rearDoorDwell = oper->rearDoorDwell;
	CarCommandFromHPE[carIndex].pre_dir = oper->pre_dir;

	// shin050713
	UpdatedCarCommand[carIndex] = TRUE;
	//return OK;
}

__declspec(dllexport) void ReturnAssignmentToPassenger(USHORT sequenceNumber,
									   CHAR carIndex)
{
	AssignmentFromDispatcher = carIndex;
}
}


// Make up all car state from CAR_STATE in COGS to send HP Or HE
void CMy2DSimDlg::MakeUpAllCarStatus(CarState *carState)
{
	int nCar,car;
	nCar=Building.nCars;
	TIME_T currentTime;

	int n_board,n_deboard;

	n_board=0,n_deboard=0;

	UpdateFloorForAllCars();

	for (car=0;car<nCar;car++)
	{
		currentTime=MakeIntTime(Sys.t_nextEvent);
		carState[car].ctCurrent=currentTime;
		carState[car].currentFloor=Car[car].currentFloor;
		carState[car].currentNCF=Car[car].NCF;
		carState[car].rearDoorState=DOORS_CLOSED;	// TBD
		carState[car].preDirection = Car[car].dir;
		if((Car[car].t_decel <= Sys.t_nextEvent) &&
			((Car[car].oper == RUNNING) || (Car[car].oper == MOVE_CAR_RANGE)))
			carState[car].decelDecided = TRUE;
		else
			carState[car].decelDecided = FALSE;
		switch(Car[car].oper)
		{
		case IDLE:
			carState[car].frontDoorState=DOORS_CLOSED;
			carState[car].motionState=STANDING;
			carState[car].movingDirection=NO_DIR;
			break;
		case OPENING:
			carState[car].frontDoorState=DOORS_OPENING;
			carState[car].motionState=STANDING;
			carState[car].movingDirection=NO_DIR;
			n_board=Car[car].n_board;
			n_deboard=Car[car].n_deboard;
			break;
		case DWELL:
			carState[car].frontDoorState=DOORS_OPENED;
			carState[car].motionState=STANDING;
			carState[car].movingDirection=NO_DIR;
			n_board=Car[car].n_board;
			n_deboard=Car[car].n_deboard;
			break;
		case CLOSING:
			carState[car].frontDoorState=DOORS_CLOSING;
			carState[car].motionState=STANDING;
			carState[car].movingDirection=NO_DIR;
			break;
		case RUNNING:
			carState[car].frontDoorState=DOORS_CLOSED;
			carState[car].motionState=MOVING;
			carState[car].movingDirection = GetDir(Car[car].origin,Car[car].target);
			break;
			//shin050715 Consider another car operations
		case MOVE_CAR_RANGE:
			carState[car].frontDoorState=DOORS_CLOSED;
			carState[car].motionState=MOVING;
			carState[car].movingDirection = GetDir(Car[car].origin,Car[car].target);
			break;
		case WAIT_CAR_RANGE:
			carState[car].frontDoorState=DOORS_CLOSED;
			carState[car].motionState=STANDING;
			carState[car].movingDirection = NO_DIR;
			break;
		default:
			carState[car].frontDoorState=DOORS_CLOSED;
			carState[car].motionState=STANDING;
			carState[car].movingDirection=NO_DIR;
			fprintf(f_ErrorLog, "MakeUpAllCarStatus: Car[%d].oper = %d at t=%lf\n",
				car, Car[car].oper, Sys.t_nextEvent);
			break;
		}
		carState[car].nPassengers=Car[car].n_pax_in_car;
		//carState[car].OPMode=NOR; //TBDTODAY
		carState[car].target=Car[car].target;
		carState[car].tStartOper=MakeIntTime(Car[car].t_start_oper); 
		carState[car].nBoard=n_board;
		carState[car].nDeboard=n_deboard;
		carState[car].origin=Car[car].origin;
	}

}

// Make up BuildingInfo from data in COGS to use in initialize
void CMy2DSimDlg::MakeUpBuildingInfo(BuildingInfo *bdInfo)
{
	bdInfo->nLandings=Building.nTotalFloors; // number of landings, including pit, overhead, virtual stops in express zone
	bdInfo->nCars=Building.nCars; // total number of cars in a group

	bdInfo->nHoistways=Building.nHoistways;			// total number of hoistway in a group
	bdInfo->lobby1Floor=Building.lobby1Floor;		// floor index of lobby 1
	bdInfo->lobby2Floor=Building.lobby2Floor;		// floor index of lobby 2
	bdInfo->lobby3Floor=Building.lobby3Floor;		// floor index of lobby 3
	bdInfo->rest1Floor=Building.rest1Floor;			// floor index of restaurant 1
	bdInfo->rest2Floor=Building.rest2Floor;			// floor index of restaurant 2
	bdInfo->EZ1Top=Building.expressZoneTop;				// floor index of express zone 1 top
	bdInfo->EZ1Bottom=Building.expressZoneBottom;			// floor index of express zone 1 bottom
	// TBD make up
	bdInfo->EZ2Top=Building.expressZoneTop;				// floor index of express zone 2 top
	bdInfo->EZ2Bottom=Building.expressZoneBottom;			// floor index of express zone 2 bottom
	bdInfo->EZ3Top=Building.expressZoneTop;				// floor index of express zone 3 top
	bdInfo->EZ3Bottom=Building.expressZoneBottom;			// floor index of express zone 3 bottom
	for (int floor=0;floor<MAX_FLOOR;floor++)
		bdInfo->landingPosition[floor]=MakeLongPosition(Building.floorPosition[floor]);	// floor position for each floor, in millimeters

	if (Building.pit>0)
		bdInfo->f_offset=-1;
	else
		bdInfo->f_offset=0;
}

// Make up HoistwayInfo from data in COGS to use in initialize
void CMy2DSimDlg::MakeUpHoistwayInfo(int hwyNo,HoistwayInfo *hInfo)
{
	int n_car,n_totalfloor,n_userfloor;
	
	hInfo->idHwy=hwyNo;
	hInfo->bOverhead=Building.overhead; // TBD
	hInfo->bPit=Building.pit;			// TBD
	hInfo->idLowerCar=Hwy[hwyNo].i_car;
	if (Hwy[hwyNo].n_car_in_hwy >1)
	{
		hInfo->idUpperCar=Hwy[hwyNo].i_car+1;
	}
	else
	{
		// hInfo->idUpperCar=Hwy[hwyNo].i_car;
		//CSW050810: use -1 as upper car index
		hInfo->idUpperCar=-1;
	}

	hInfo->nCars=Hwy[hwyNo].n_car_in_hwy;
	n_car=hInfo->nCars;
	n_totalfloor=Building.nTotalFloors;
	int spareFloor=0;
	if (hInfo->bOverhead)
		spareFloor++;
	if (hInfo->bPit)
		spareFloor++;
	n_userfloor=n_totalfloor-spareFloor;

}

/**************************************************
* Make integer time from double data by multiplying scale macro
**************************************************/
LONG CMy2DSimDlg::MakeIntTime(double time)
{
	return (LONG)(time*INT_TIME_SCALE);
}
/**************************************************
* Make integer speed from double data by multiplying scale macro
**************************************************/
SHORT CMy2DSimDlg::MakeIntSpeed(double val)
{
	return (SHORT)(val*INT_SPEED_SCALE);
}

/**************************************************
 * Make Long speed form double data by multiplaying scale
 **************************************************/
LONG CMy2DSimDlg::MakeLongPosition(double pos)
{
	return (LONG)(pos*INT_SPEED_SCALE);
}
/*******************************************************
 * Make carinfo from the member variable Param,Building in class CMy2DSimDlg
 * Needs updating more for car status in COGS and then update this
 *******************************************************/
void CMy2DSimDlg::MakeUpCarInfo(int carNo, CarInfo *carInfo)
{
	double	DefAfd=3.0;  // default forced deceleration for ESP

	carInfo->Afd = MakeIntSpeed(DefAfd);	// forced deceleration for car in mm/s/s
	carInfo->maxJerk=MakeIntSpeed(Param.MaxJerk);  // 		// maximum jerk value in mm/s/s/s
	carInfo->maxAcceleration=MakeIntSpeed(Param.MaxAcceleration);		// maximum acceleration rate in mm/s/s
	carInfo->maxVelocity=MakeIntSpeed(Param.MaxVelocity);			// maximum speed of the car in mm/s
	carInfo->tDoorOpening=MakeIntTime(Param.door_open);			// door opening time in milliseconds
	carInfo->tDoorClosing=MakeIntTime(Param.door_close);			// door closing time in milliseconds
	carInfo->boardingRatio=(USHORT)(Param.boardingRatio);			// percentage
	carInfo->capacity=(USHORT)(Param.capacity);				// max number of passengers the car can carry
	carInfo->nDoors=(UCHAR)Building.nDoors;					// number of doors
	carInfo->doorWidth=(USHORT)Building.doorWidth;				// door width in mm
	carInfo->hoistwayID=Car[carNo].i_hwy;				// index of hoistway this car belongs
	int floor;
	for (floor=0;floor<MAX_FLOOR;floor++)
		carInfo->callMask[floor]=Param.AllowMask[carNo][floor];	// floor mask per floor for each car
	carInfo->tStartDelay=MakeIntTime(Param.StartDelay);
	carInfo->tStopDelay=MakeIntTime(Param.StopDelay);

}
/*************************************************************************
* Make up parameter information to use as a spec.
*************************************************************************/
void CMy2DSimDlg::MakeUpParameter(Parameter *parameter)
{
	parameter->tCarMinDwell=MakeIntTime(Param.t_car_min_dwell);
	parameter->tHallMaxDwell=MakeIntTime(Param.t_hall_max_dwell);
	parameter->tHallMinDwell=MakeIntTime(Param.t_hall_min_dwell);
	parameter->tLobbyDwell=MakeIntTime(Param.t_lobby_dwell);
	parameter->bReopenEnabled=Param.bReopenEnabled;
	parameter->tBoardingDeboardingTime=MakeIntTime(Param.boardingDeboardingTime);
	/* 051116 test
	parameter->A1S=1000;  //    Default (1.0)   Test Procedure (1.2)
	parameter->A2S=10;	  //	default (0.01)  Test Procedure (0.02)
	parameter->A1W=1000;  //    default (1.0)   Test Procedure (2.1)
	parameter->A2W=10;  // 		default (0.01)  Test Procedure (0.05)
	parameter->V=1000;	// 		default (1.0)   Test Procedure (0.9)
	parameter->W=1000;	// 		default (1.0)   Test Procedure (1.1)
	*/
	/*
	parameter->A1S=100;  //    Default (1.0)   Test Procedure (1.2)
	parameter->A2S=5;	  //	default (0.01)  Test Procedure (0.02)
	parameter->A1W=1000;  //    default (1.0)   Test Procedure (2.1)
	parameter->A2W=50;  // 		default (0.01)  Test Procedure (0.05)
	parameter->V=1000;	// 		default (1.0)   Test Procedure (0.9)
	parameter->W=1000;	// 		default (1.0)   Test Procedure (1.1)
	*/
	/*
	parameter->A1S=388;  //    Default (1.0)   Test Procedure (1.2)
	parameter->A2S=3;	  //	default (0.01)  Test Procedure (0.02)
	parameter->A1W=10000;  //    default (1.0)   Test Procedure (2.1)
	parameter->A2W=160;  // 		default (0.01)  Test Procedure (0.05)
	parameter->V=1000;	// 		default (1.0)   Test Procedure (0.9)
	parameter->W=1000;	// 		default (1.0)   Test Procedure (1.1)
	*/
	//shin051121
	parameter->A1S=Param.A1S;
	parameter->A2S=Param.A2S;
	parameter->A1W=Param.A1W;
	parameter->A2W=Param.A2W;
	parameter->V=Param.V;
	parameter->W=Param.W;

	// shs0708 ? dispatcher attached should included?
	parameter->GDAlgorithmID = Param.algorithmID;
	parameter->MinSeparationDist=(ULONG)(Param.MinSeparationDist*INT_DIST_SCALE);
	parameter->bDwellTimeExtendACS = Param.bDwellTimeExtendACS;
	parameter->bRunDelayACS = Param.bRunDelayACS;
	parameter->bReducedProfile = Param.bReducedProfile;
}

// Make new call and request dispatch from Dispatcher
void CMy2DSimDlg::EnterNewCall(int p)
{
	_RPT1(_CRT_WARN,"EnterNewCall(%d)\n",p);
	if (Param.bAutomaticDispatching == TRUE) {
		Call call;
		TIME_T	currentTime;
		USHORT sequenceNumber;
		CHAR manualAssignment;

		call.callType=0; // standard
		call.origin=Pax[p].f_origin;
		call.destination=Pax[p].f_dest;
		call.nPassengers=1;
	// shin050714 Currently sequenceNumber is not used.
	// Use passenger index for the sequence number temporally.
		sequenceNumber = (USHORT)p;
		currentTime=MakeIntTime(Sys.t_nextEvent);
		CarState carState[MAX_CAR];
		MakeUpAllCarStatus(carState);
		manualAssignment = (CHAR)Pax[p].assignment;
		AssignmentFromDispatcher = -1;
		DispatchNewCall(call,sequenceNumber,carState,currentTime,manualAssignment);
	}
	else
		AssignmentFromDispatcher = (CHAR)Pax[p].assignment;
}

// Call it when the 'car' state is updaated. Don't use for the other car
// Make up CarState of car from CAR_STATE variable in this class
void CMy2DSimDlg::MakeUpCarState(int car, CarState *carState)
{
	TIME_T currentTime;
	double j,a,v,pos;
	int into_decel;

	currentTime=MakeIntTime(Sys.t_nextEvent);
	carState->ctCurrent=currentTime;

	GetCurrentMotionState(car, &j, &a, &v, &pos);

	carState->currentFloor=PosToFloor(pos);
	carState->currentNCF=GetNextCommittableFloor(car, &into_decel, 0);
	carState->rearDoorState=DOORS_CLOSED;	// TBD
	carState->preDirection = Car[car].dir;
	carState->target=Car[car].target;
	if((Car[car].t_decel <= Sys.t_nextEvent) &&
		((Car[car].oper == RUNNING) || (Car[car].oper == MOVE_CAR_RANGE)))
		carState->decelDecided = TRUE;
	else
		carState->decelDecided = FALSE;

	// tStartOper will be updated as the status expectation
	carState->tStartOper=MakeIntTime(Car[car].t_start_oper); 
	
	switch(Car[car].oper)
	{
	case IDLE:
		carState->frontDoorState=DOORS_CLOSED;
		carState->motionState=STANDING;
		carState->movingDirection=NO_DIR;
		break;
	case OPENING:
		carState->frontDoorState=DOORS_OPENED;
		carState->motionState=STANDING;
		carState->movingDirection=NO_DIR;
		carState->tStartOper=carState->ctCurrent; // update correct time when the door state is changed to doors opened
		break;
	case DWELL:
		carState->frontDoorState=DOORS_CLOSING;
		carState->motionState=STANDING;
		carState->movingDirection=NO_DIR;
		carState->tStartOper=carState->ctCurrent; // update correct time when the door state is changed to doors start closing
		break;
	case CLOSING:
		carState->frontDoorState=DOORS_CLOSED;
		carState->tStartOper=carState->ctCurrent; // update correct time when the door state is changed to doors closed
		//shin050929 predict motionState, movingDirection and target by HPE target.
		if(Car[car].currentFloor == CarCommandFromHPE[car].target)
		{
			carState->motionState=STANDING;
			carState->movingDirection=NO_DIR;
			carState->tStartOper=carState->ctCurrent; // update correct time when the door state is changed to doors closed
		}
		else if(Car[car].currentFloor < CarCommandFromHPE[car].target)
		{
			carState->motionState=MOVING;
			carState->movingDirection=UP;
			carState->target=CarCommandFromHPE[car].target; //shin051004 Overwrite estimated target with CarCommandFromHPE[car].target
		}
		else
		{
			carState->motionState=MOVING;
			carState->movingDirection=DN;
			carState->target=CarCommandFromHPE[car].target; //shin051004 Overwrite estimated target with CarCommandFromHPE[car].target
		}
		
		break;
	case RUNNING:
		if(Car[car].mp_break[Car[car].mp_n_break-1]+EPSILON < Sys.t_nextEvent)
		{
			// update for expectation for next event, if not updated tStartOper is running start time but status is arrival
			carState->tStartOper=currentTime; // arrival time

			if((Car[car].n_CCall[Car[car].currentFloor] > 0) ||
				((Car[car].n_UpCall[Car[car].currentFloor] > 0) && (Car[car].dir == UP)) ||
				((Car[car].n_DnCall[Car[car].currentFloor] > 0) && (Car[car].dir == DN)))
			{
				carState->frontDoorState=DOORS_OPENING;
				carState->motionState=STANDING;
				carState->movingDirection = NO_DIR;
			}
			else
			{
				carState->frontDoorState=DOORS_CLOSED;
				carState->motionState=STANDING;
				carState->movingDirection = NO_DIR;
			}
		}
		else
		{
			carState->frontDoorState=DOORS_CLOSED;
			carState->motionState=MOVING;
			carState->movingDirection = GetDir(Car[car].origin,Car[car].target);
		}
		break;
		//shin050715 Consider another car operations
	case MOVE_CAR_RANGE:
		if(Car[car].mp_break[Car[car].mp_n_break-1]+EPSILON < Sys.t_nextEvent)
		{
			// update for expectation for next event, if not updated tStartOper is running start time but status is arrival
			carState->tStartOper=currentTime;

			carState->frontDoorState=DOORS_CLOSED;
			carState->motionState=STANDING;
			carState->movingDirection = NO_DIR;
		}
		else
		{
			carState->frontDoorState=DOORS_CLOSED;
			carState->motionState=MOVING;
			carState->movingDirection = GetDir(Car[car].origin,Car[car].target);
		}
		break;
	case WAIT_CAR_RANGE:
		carState->frontDoorState=DOORS_CLOSED;
		carState->motionState=STANDING;
		carState->movingDirection = NO_DIR;
		break;
	default:
		carState->frontDoorState=DOORS_CLOSED;
		carState->motionState=STANDING;
		carState->movingDirection=NO_DIR;
		fprintf(f_ErrorLog, "MakeUpCarState: Car[%d].oper = %d at t=%lf\n",
			car, Car[car].oper, Sys.t_nextEvent);
		break;
	}
	carState->nPassengers=Car[car].n_pax_in_car;
	//carState->OPMode=NOR; //TBDTODAY
	carState->nBoard=Car[car].n_board;
	carState->nDeboard=Car[car].n_deboard;
	carState->origin=Car[car].origin;
}

// shin350715
// Update target using UpdateTarget ftn according to HPE CarCommand which HPE sends.
void CMy2DSimDlg::UpdateTargetFollowingHPE(void)
{
	int c;
	// shin050713
	for (c = 0; c < n_car; c++)
	{  
		if (UpdatedCarCommand[c] == TRUE)
		{
			Car[c].tRunDelay = CarCommandFromHPE[c].runDelay/1000.0;
			// CSW050802 : check whether there is a need to update car direction
			if ( (Car[c].oper == DWELL) || (Car[c].oper == CLOSING))
				Car[c].dir = CarCommandFromHPE[c].pre_dir;	// CSW050802 : set car direction to the next command direction
			// CSW050802 : end
			// shin050812 Car direction is set in the below function UpdateTarget using the predirection of HPE
			UpdateTarget(c, 0);
		}
	}
}

void CMy2DSimDlg::LinkDLLs()
{
	if(Param.bAutomaticDispatching==TRUE) // link dispatcher dll
	{
		if(LinkDispatcherDLL(Param.dispatcherDLLPath,Param.algorithmID)!=TRUE)
		{ // fail to load dll
			CString cstr;
			cstr.Format("Fail to load DLL:%s\\%s. Use user configuration to use dispatcher",
				Param.dispatcherDLLPath,Param.dispatchingAlgorithmName);
			MessageBox(cstr);
			Param.bAutomaticDispatching=FALSE;
			DetachDispatcher();
			strcpy(Param.dispatcherDLLName,"Manual");
			strcpy(Param.dispatchingAlgorithmName,"Human Intelligence");
		}
	}
	else // manual dispatcher
	{
		DetachDispatcher();
		strcpy(Param.dispatcherDLLName,"Manual");
		strcpy(Param.dispatchingAlgorithmName,"Human Intelligence");
	}
	
	if (m_bHM==TRUE)  // attach HM
	{
		if (m_bHMAttached==FALSE)
		{
			if (LinkHoistwayPlanner(HOISTWAY_PLANNER_FILE)==TRUE)
			{
				if (LinkHoistwayPlanExecutor(HOISTWAY_EXECUTOR_FILE)==TRUE)
					m_bHMAttached=TRUE;
				else 
					AfxFreeLibrary(hinstHPLib); // detach HP
			}
		}
	}
	else if (m_bHMAttached==TRUE)  // detach HM
	{
		AfxFreeLibrary(hinstHPLib);
		AfxFreeLibrary(hinstExec);
		m_bHMAttached=FALSE;
	}
}

void CMy2DSimDlg::InitializeDLLs()
{
	BuildingInfo bdInfo;
	HoistwayInfo hwyInfo[MAX_HOISTWAY];
	CarInfo		 carInfo[MAX_CAR];
	Parameter	parameter;
	int hwyNo;
	int car;
	char *dispatcherName;
	
	MakeUpBuildingInfo(&bdInfo);
	for (hwyNo=0;hwyNo<MAX_HOISTWAY;hwyNo++)
		MakeUpHoistwayInfo(hwyNo,&(hwyInfo[hwyNo]));
	for (car=0;car<MAX_CAR;car++)
		MakeUpCarInfo(car,&(carInfo[car]));
	MakeUpParameter(&parameter);
	
	if (m_bDispatcherAttached==TRUE)
	{
		GDInitialize(&bdInfo, hwyInfo, carInfo, &parameter, versionNameGD);
		dispatcherName = Param.dispatcherDLLPath;
	}
	else
		dispatcherName = NO_DISPATCHER;
	
	if (m_bHMAttached==TRUE)
	{
		HPInitialize(&bdInfo, hwyInfo, carInfo, &parameter, versionNameHP);
		strcpy(versionNameHPE,dispatcherName);
		HPEInitialize(&bdInfo, hwyInfo, carInfo, &parameter, versionNameHPE);
		// PutCarState after initializing the status
		CarState carState;
		for (car=0;car<bdInfo.nCars;car++)
		{
			MakeUpCarState(car,&carState);
			PutCarState(car,&carState);
		}
	}

	if (m_bDispatcherAttached)
		GDAlgorithmInitialize();

}

// Calculate expected boarding people of the car at current floor and return
// Only valid from Door Opening before closing
int CMy2DSimDlg::CalculateExpectedBoarder(int car)
{
	int f;
	int c=car;
	int n_board;

	f = Car[c].floor;
	
	if (Car[c].dir == 1)
		n_board = Car[c].n_UpCall[f];
	else if (Car[c].dir == -1)
		n_board = Car[c].n_DnCall[f];
	else
		n_board = 0;

	return n_board;
}

// If there is no other car in the same hoistway, it returns MAXCAR.
// Otherwises, it returns the index of other car in the same hoistway.
int CMy2DSimDlg::GetOtherCarIndex(int c)
{
	int iHoistway; //hoistway index
	int other = MAXCAR;

	iHoistway = Car[c].i_hwy;
	if(Hwy[iHoistway].n_car_in_hwy < 2) //Check the number of cars in the hoistway
		return MAXCAR;//There is only one car in the hoistway

	if(Car[c].lowerOrUpper == LOWER_CAR)
	{
		if(Car[c+1].lowerOrUpper == UPPER_CAR)
			other = c+1;
		else
			other = MAXCAR;
	}
	else // If the car is a upper car.
	{
		if((c-1) >= 0)
		{
			if(Car[c-1].lowerOrUpper == LOWER_CAR)
				other = c-1;
			else
				other = MAXCAR;
		}
		else
			other = MAXCAR;
	}

	return other;
}

void CMy2DSimDlg::MakeUpOtherCarState(int car, CarState *carState)
{
	TIME_T currentTime;
	double j,a,v,pos;
	int into_decel;

	currentTime=MakeIntTime(Sys.t_nextEvent);
	carState->ctCurrent=currentTime;

	GetCurrentMotionState(car, &j, &a, &v, &pos);

	carState->currentFloor=PosToFloor(pos);
	carState->currentNCF=GetNextCommittableFloor(car, &into_decel, 0);
	carState->rearDoorState=DOORS_CLOSED;	// TBD
	carState->preDirection = Car[car].dir;
	if((Car[car].t_decel <= Sys.t_nextEvent) &&
		((Car[car].oper == RUNNING) || (Car[car].oper == MOVE_CAR_RANGE)))
		carState->decelDecided = TRUE;
	else
		carState->decelDecided = FALSE;
	
	switch(Car[car].oper)
	{
	case IDLE:
		carState->frontDoorState=DOORS_CLOSED;
		carState->motionState=STANDING;
		carState->movingDirection=NO_DIR;
		break;
	case OPENING:
		carState->frontDoorState=DOORS_OPENING;
		carState->motionState=STANDING;
		carState->movingDirection=NO_DIR;
		break;
	case DWELL:
		carState->frontDoorState=DOORS_OPENED;
		carState->motionState=STANDING;
		carState->movingDirection=NO_DIR;
		break;
	case CLOSING:
		carState->frontDoorState=DOORS_CLOSING;
		carState->motionState=STANDING;
		carState->movingDirection=NO_DIR;
		break;
	case RUNNING:
		carState->frontDoorState=DOORS_CLOSED;
		carState->motionState=MOVING;
		carState->movingDirection = GetDir(Car[car].origin,Car[car].target);
		break;
	case MOVE_CAR_RANGE:
		carState->frontDoorState=DOORS_CLOSED;
		carState->motionState=MOVING;
		carState->movingDirection = GetDir(Car[car].origin,Car[car].target);
		break;
	case WAIT_CAR_RANGE:
		carState->frontDoorState=DOORS_CLOSED;
		carState->motionState=STANDING;
		carState->movingDirection = NO_DIR;
		break;
	default:
		carState->frontDoorState=DOORS_CLOSED;
		carState->motionState=STANDING;
		carState->movingDirection=NO_DIR;
		fprintf(f_ErrorLog, "MakeUpOtherCarState: Car[%d].oper = %d at t=%lf\n",
			car, Car[car].oper, Sys.t_nextEvent);
		break;
	}
	carState->nPassengers=Car[car].n_pax_in_car;
	carState->target=Car[car].target;
	carState->tStartOper=MakeIntTime(Car[car].t_start_oper); 
	carState->nBoard=Car[car].n_board;
	carState->nDeboard=Car[car].n_deboard;
	carState->origin=Car[car].origin;
}

//shin050929 Check whether the target of HPE command is valid considering ncf, dir and oper.
BOOL CMy2DSimDlg::IsValidTargetCommand(int target, int ncf, int dir, int oper)
{
	BOOL ret;

	ret = FALSE;
	if((oper == RUNNING) || (oper == FORCED_MOVE) || (oper == MOVE_CAR_RANGE))
	{
		if(dir == UP)
		{
			if((target >= ncf) && (target < n_floor))
				ret = TRUE;
		}
		else if(dir == DN)
		{
			if((target <= ncf) && (target >= 0))
				ret = TRUE;
		}
		else
		{
			fprintf(f_ErrorLog, "IsValidTargetCommand: target(%d) ncf(%d) oper(%d) dir(%d) at t=%lf\n",
				target, ncf, dir, oper, Sys.t_nextEvent);
		}
	}
	else //standing case
	{
		ret = TRUE;
	}

	return ret;
}

void CMy2DSimDlg::OnOpenBatch() 
{
	CString filt = "2D Batch(*.bat)|*.bat||";
	CString fileName ;
	int retval; 
	
	CFileDialog *fileDlg;
	C2DFileIO f2d;

	fileDlg = new CFileDialog( TRUE, "*.bat", "*.bat", NULL, filt, this ) ;
	if ( fileDlg != NULL )
	{
		fileDlg->m_ofn.Flags |= OFN_FILEMUSTEXIST ;
		fileDlg->m_ofn.lpstrTitle = "Batch Traffic Profile" ;
		
		if ( fileDlg->DoModal() == IDOK )
		{
			fileName = fileDlg->GetPathName();
			strcpy( batFileName, fileName );

			retval = f2d.LoadBatFile(batFileName);
			if (retval==-1) // 	Only batch traffic profile data
			{
				MessageBox("Only Batch Traffic Data can be read. Ensure Simulation Environment Setting");
			}
			else if (retval==0) // Success
			{
				NumberOfTrafficFile = f2d.NumberOfTrafficFile;
				if(NumberOfTrafficFile>0 && NumberOfTrafficFile<=MAX_NUMBER_TRAFFIC_FILE)
				{
					for(int i=0; i<NumberOfTrafficFile; i++)
					{
						FileNameOf2DP[i] = f2d.FileNameOf2DP[i];
					}
					//BatchMode
					DoBatchSimulation();
				}
			}
			else if (retval==1) // File open error
			{
				MessageBox("File Open Error");
			}
			else if (retval==3) // File corruption
			{
				MessageBox("File corrupted. Ensure file format is right");
			}
		}
		//DoBatchSimulation();
	}
	delete [] fileDlg ;
}

void CMy2DSimDlg::DoBatchSimulation()
{
	int retval; 
	C2DFileIO f2d;
	int i, iLength;;
	CString fileName;

	int paxCount,moveCount;

	for(IndexOfTrafficFile=0; IndexOfTrafficFile<NumberOfTrafficFile; IndexOfTrafficFile++)
	{
		//Do each simulation
		InitPassenger();
		strcpy( profileFileName, FileNameOf2DP[IndexOfTrafficFile] );
		fileName = FileNameOf2DP[IndexOfTrafficFile];
		iLength = fileName.GetLength();
		fileName.Delete(iLength-4, 4);
		m_strTrafficProfileName = fileName;
		retval = f2d.Load2DPFile(profileFileName,&FileMemo,&Building,&Param,
			Pax, Move,&paxCount,&moveCount);
		if (retval==-1) // 	Only traffic profile data
		{
			MessageBox("Only Traffic Data are read. Ensure Simulation Environment Setting");
			Building.nHoistways=(Building.nCars+1)/2;
		}
		else if (retval==0) // Success
		{
			//MessageBox("File Open Success");
		}
		else if (retval==1) // File open error
		{
			MessageBox("File Open Error");
		}
		else if (retval==3) // File corruption
		{
			MessageBox("File corrupted. Ensure file format is right");
		}
		
		n_pax=paxCount;
		n_move=moveCount;
		n_hwy=Building.nHoistways;
		n_car=Building.nCars;
		n_floor=Building.nTotalFloors;
		
		ProfileCarsForCheck=Building.nCars;
		
		for (i=0;i<MAXCAR;i++)
		{
			Car[i].floor=Param.initial_position[i];
		}
		
		RedisplayCaption();  // Display Caption
		
		InitializeEngine();  // initialize simulation engine
		m_iDwellType = 0; //Default selection is current CAGS behavior (0)
		
		m_trafficProfileOpened = TRUE;
		m_bSimulationEnd = FALSE;
		m_bAssigned = FALSE;
		m_bNeedAssign = FALSE;
		m_bReassigned = FALSE;
		
		InitDrawBuilding();
		UpdateCars(); 
		UpdateDisplay();
		
		UpdateData(FALSE);
		Invalidate();

		OnButtonNextNewHcall();
	}
}



void CMy2DSimDlg::ReadSpecification()
{
	int i;
	// shin051121
	//Read object function coefficients from a file.
	FILE *fParam = fopen("c:/parameter.dat", "r");
	if(fParam != NULL)
	{
		ULONG coefficients[6];
		//Add error handling later.
		for(i=0; i<6; i++)
			fscanf(fParam, "%ld", &coefficients[i]);
		Param.A1S = coefficients[0];
		Param.A2S = coefficients[1];
		Param.A1W = coefficients[2];
		Param.A2W = coefficients[3];
		Param.V = coefficients[4];
		Param.W = coefficients[5];
		fclose(fParam);
	}
	else
	{
		Param.A1S = 1000;
		Param.A2S = 10;
		Param.A1W = 1000;
		Param.A2W = 10;
		Param.V = 1000;
		Param.W = 1000;
	}
	
}

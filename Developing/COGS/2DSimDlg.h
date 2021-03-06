// 2DSimDlg.h : header file
//

#if !defined(AFX_2DSIMDLG_H__2FD0115E_FE28_43BC_BFDD_C40B662CB014__INCLUDED_)
#define AFX_2DSIMDLG_H__2FD0115E_FE28_43BC_BFDD_C40B662CB014__INCLUDED_

#include "ShowLoadDlg.h"	// Added by ClassView
//#include "..\HoistwayPlanner\hoistway_plan.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "..\COGS\2DInterfaceFunctionType.h"
#include "2DInterfaceData.h"	// Added by ClassView

/////////////////////////////////////////////////////////////////////////////
// CMy2DSimDlg dialog

class CMy2DSimDlg : public CDialog
{
// Construction
public:
	int IndexOfTrafficFile; //in batch traffic file
	int NumberOfTrafficFile; //in batch traffic file
	CString FileNameOf2DP[MAX_NUMBER_TRAFFIC_FILE]; //in batch batch file
	void InitializeDLLs();
	void LinkDLLs(void);
	void UpdateTargetFollowingHPE(void);
	char versionNameHPE[100];
	char versionNameHP[100];
	char versionNameGD[100];
	void MakeUpCarState(int car,CarState *carState);
	void EnterNewCall(int p);
	void MakeUpParameter(Parameter *parameter);
	SHORT MakeIntSpeed(double val);
	LONG  MakeLongPosition(double pos);
	void MakeUpCarInfo(int carNo,CarInfo *carInfo);
	LONG MakeIntTime(double time);
	void MakeUpHoistwayInfo(int hwyNo,HoistwayInfo *hInfo);
	void MakeUpBuildingInfo(BuildingInfo *bdInfo);
	void MakeUpAllCarStatus(CarState *carState);
	void DetachHoistwayPlanner(void);
	int LinkHoistwayPlanner(CString hoistwayPlannerDLL);
	BOOL m_bSpy;
	void SpyCOGS();
	int CommittablePosToNCF(int car, double pos);
	double DwellTimeCurrent(int fl, int nBoard, int nDeboard);
	double DwellTimeOld(int c);
	void UpdateFloorForAllCars();
	int SendCallServed(int car,int floor,int callset);
	int SetSpecialTime(double sTime);
	void DecodingMsgs(EXTRA_MSGS *msg);
	void ProcessPassenger(PASSENGER_STATUS *passengers);
	void clearCommunicationContents(void);
	void ProcessExtraMsg(char msg[SIZE_EXTRAMSG]);
	void SendExtraMsg(int id,int len,char *data);
	void SendMove(MOVE_PARAMETER move,int cmd);
	int AssignDestinationEntry(int carID,int passID,int cmd);
	void EnterDestinationEntry(int passId,int origin,int dest,int ncount,int callType);
	int CompareMove(MOVE_PARAMETER move1,MOVE_PARAMETER move2);
	int DeleteMove(MOVE_PARAMETER move);
	int AddMove(MOVE_PARAMETER move);
	void ProcessMoveCommands(MOVE_COMMANDS *movecmds);
	BOOL IsValidMoveOperation(int caroperation);
	void DetachDispatcher(void);
	void RedisplayCaption(void);
	int SelectDispatchingAlgorithm(char info[13][50]);
	void PostprocessDispatcher(UINT dispatchResult);
	UINT PreprocessDispatcher(int mode);
	void ProcessDispatcher(int mode);
	BOOL m_bDispatcherAttached;
	void SetCarRange(int c, int bottom, int top, int door_position);
	double HardCommitTime(int origin, int dest);
	int DisplayedPosition(int c);
	int NextCommittableFloor(int c, int *into_decel, int should_be_in_flight);
	double HardRunTime(int origin, int dest);
	BOOL m_bParamHard;	//shin 040914 two kinds of parameter
	int m_iDwellType;
	void SetRejection(int c, int flr, BYTE mask, int iPax);
	void CheckRejection(int c);
	int m_violationHCD;
	void CheckOtherAssigned(int c);
	BOOL ProfileCheckPassed;
	double GetCommittableTime(int c, int s, int d);
	double GetTraversalTime(int c, double traversalDistance);
	double GetActualPositionTime(int c, int origin, int dest);
	double HardActualPositionTime(int origin, int dest);
	double m_minMarginTime;
	double m_minMargin;
	int m_car;
	void GenerateProfile(BREAK_TABLE *b);
	void CheckMargin(int c);
	int GetNextCommittableFloor(int c, int *into_decel, int should_be_in_flight);
	double GetRunTime(int c, int origin, int dest);
	void GetCurrentMotionState(int c,  double *jerk, double *accel, double *speed, double *pos);
	double GetRunTime(int c);
	int GetBreakPointTableIndex(int c, double t);
	BOOL ComputeMotionState(int c, double t, double *jerk, double *accel, double *speed, double *pos);
	void MakeBreakPointTableC(int c, double j, double a, double v, double d);
	void MakeBreakPointTableB(int c, double j, double a, double v, double d);
	void MakeBreakPointTableA(int c, double j, double a, double v, double d);
	void GenerateFullProfile(int c, double dist);
	CShowLoadDlg *m_pShowLoadDlg;
	void SetBuildingParameter(BUILDING_STRUCTURE *b);
	int m_nPassenger[MAXCAR][MAXFLOOR][MAXDIR];
	int m_nMaxPassenger[MAXCAR];
	void CalculatePassengerInCars();
	double DwellTime(int fl, int nBoard, int nDeboard);
	double CalculateArrivalTime(int car, int fl, int dir);
	void InitPassenger();
	CMy2DSimDlg(CWnd* pParent = NULL);	// standard constructor
	char profileFileName[100];
	char batFileName[100];

// Dialog Data
	//{{AFX_DATA(CMy2DSimDlg)
	enum { IDD = IDD_MY2DSIM_DIALOG };
	CListBox	m_ListWarning;
	CString	m_CurrentTime;
	CString	m_CurrentPassenger;
	CString	m_0;
	CString	m_1;
	CString	m_2;
	CString	m_3;
	CString	m_4;
	CString	m_5;
	CString	m_6;
	CString	m_7;
	CString	m_debug;
	CString	m_strClosestMargin;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy2DSimDlg)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
	HACCEL m_hAccel; // accelerator table
	CButton	m_Car[MAXCAR];
	int m_bCar[MAXCAR];
	char	fName[100], assignedfName[100];
	CFont font;
//	int i_lobby;//lobby floor index  // AH: replaced by Param.lobby
	int cellWidth;
	int cellHeight;
	int nCols;//the number of column
	int nRows;//the number of row
	int carWidth, carHeight;
	CRect rectDlg, rectTable;
	CString FileMemo;

	char	ExecutionDir[100];

	//From Simulator Engine
	SYSTEM	Sys;
	PARAMETER	Param;
	PASSENGER	Pax[MAXPAX];
	MOVE_PARAMETER Move[MAXMOVE];
	HOISTWAY    Hwy[MAXHWY];
	CAR_STATE	Car[MAXCAR+1];
	FLOOR_STATE	Floor[MAXFLOOR];
	CAR_LOG     CarLog[MAXCARLOG];
	BUILDING_STRUCTURE	Building; 
	double SpecialEventTime[MAXSPECIALEVENT];
	int ProfileCarsForCheck,ProfileFloorsForCheck; // for checking with configuration when simulation

	// Function to call Hoistway Plan Executor
	HINSTANCE hinstExec;
	int LinkHoistwayPlanExecutor(CString DLLFileName);
	HPEInitialize_T HPEInitialize;
	PutCarState_T	PutCarState;
	MonitorCarState_T	MonitorCarState;

	// Funtion to call Hoistway Planner DLL
	HINSTANCE	hinstHPLib;
	HPInitialize_T	HPInitialize;
	AcceptAssignment_T	AcceptAssignment;

	//  FunDispatcher DLL
	HINSTANCE hinstLib; 
	int LinkDispatcherDLL(CString FileName,int AlgorithmID);
	GDInitialize_T GDInitialize;
	GDAlgorithmInitialize_T GDAlgorithmInitialize;
	DispatchNewCall_T	DispatchNewCall;
	//DDUpdate	GetDispatcherDecision;
	int n_pax, n_move, n_specialEvent, n_hwy, n_car, n_floor, n_carlog;
	//From main()
	int i_pax;         // index of next passenger
	int i_pax_recent;  // index of most recent passenger
	int i_move;        // index of extraneous car move
	int i_specialEvent;  // index of next special event
	FILE *f_ErrorLog;
#define USERFLOOR(x) (x + Param.f_offset)

	// for dispatcher
	int d_NoOfNewPassengers;// for multiple simultaneous new passengers
	PASSENGER_STATUS d_passengers;
	MOVE_COMMANDS d_moveCommands;
	MOTIONPROFILE_COMMANDS d_motionCommands;
	EXTRA_MSGS	d_extraMsgs;
	
protected:
	afx_msg void OnCarAssign(UINT nID);
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMy2DSimDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonOpenTraffic();
	afx_msg void OnButtonUserConfiguration();
	afx_msg void OnButtonNextEvent();
	afx_msg void OnButtonNextPassenger();
	afx_msg void OnButtonAddMove();
	afx_msg void OnButtonJump();
	afx_msg void OnButtonSaveTrafficProfile();
	afx_msg void OnButtonNextNewHcall();
	afx_msg void OnButtonClearwarning();
	afx_msg void OnButtonLoad();
	afx_msg void OnOpenBatch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL IsValidTargetCommand(int target, int ncf, int dir, int oper);
	BOOL m_bHMAttached;
	BOOL m_bHM;
	void ClearMoveFromN(int fromIndex);
	void UpdateMove(MOVE_PARAMETER move);
	void CheckAssignments(int c, int flr, int dir, int p);
	void SetHoistway(int c, int hwy);
	double GetMinMargin(BREAK_TABLE *b, int index, double *t);
	void UpdateCars();
	int PosToFloor(double pos);
	double FloorToPos(int floor);
	BOOL m_bIsVisibleShowLoad;
	void ProcessNextEvent();
	BOOL bEnableCollisionMessageBox;
	void DisplayWarningList(CString str);
	BOOL m_bReassigned;
	CString m_strEstimatedTime[MAXCAR];
	void ShowEstimatedArrivalTime();
	BOOL CollisionCheck();
	BOOL m_bReassign;
	void AssignPostManage();
	CString m_strTrafficProfileName;
	void RemakeTrafficProfile();
	BOOL m_bSimulationEnd;
	BOOL m_trafficProfileOpened;
	void PressNextEventButton();
	BOOL m_bMsgDisable;
	BOOL m_bNeedAssign;
	//CPen Objects
	CPen penB0,	 penB,   penBp,  penB_dash, penB_dot;
	CPen penW,   penBW;
	CPen penGy,  penGyp, penLGy, penGy_d, penDGy_d,  penDGy_d2,  penDGy, penDGyp, penMSK,  penGy_dp;
	CPen penBu,  penBup, penLBu, penDBu,  penBu2,    penBu2p,    penDBup;
	CPen penR,   penLR,  penDR,  penR_d,  penR_dp,   penDRp;
	CPen penG,   penDG,  penG_d, penG_dp, penDG_dot, penDG_dotp, penDGp;
	CPen penY,   penLY;
	CPen penO,   penP2, penP2p;
	CPen penLM,  penLMp,  penC,   penCp;
	CPen penDGB, penLGB;
	//CBrush Objects
	CBrush  brushB;
	CBrush  brushW,  brushBW;
	CBrush  brushGy, brushLGy, brushDGy, brushMSK;
	CBrush  brushBu, brushLBu, brushDBu;
	CBrush  brushR,  brushLR,  brushDR,  brushLLR;
	CBrush  brushG,  brushLG;
	CBrush  brushY,  brushLY;
	CBrush  brushO;
	CBrush  brushV,  brushLV,  brushP2;
	CBrush  brushLM, brushDGB, brushLGB;
	CBrush brushUP, brushDN, brushLUP, brushLDN, brushUPp, brushLUPp;    //For Draw Graph at SUMMARY
	CBrush  brushWINDOWCOLOR;
	CBrush  brushBu_b, brushBu_bp;

	void InitVariables(void);
	CBitmap bmpDNUPCC, bmpDNUP, bmpDNCC, bmpDN, bmpUPCC, bmpUP, bmpCC;
	BOOL m_bAssigned;

	void UpdateDisplay();
	void InitDrawBuilding();
	void DrawBuilding();
	void DrawElevator();
	void DrawLoad();
	void DrawQueLength();
	void DrawCalls();
	void DrawRegistration();
	void DrawCCRegistration(int fl);
	void DrawHCRegistration(int dir, int fl);
	void DrawAssignment();
	void DrawCCAssignment(int fl, int car);
	void DrawCCAssignmentF(int fl, int car);
	void DrawHCAssignment(int dir, int fl, int car);
	void DrawRange();
	void DrawNCF();
	void DrawNCFRect(int fl, int car);
	void DrawTarget();
	void DrawTargetRect(int fl, int car);
	void DrawState();
	void UpdateText();
	CString DebugStr(int c);
	void DebugFloorShow();
	void LoadBitmap();
	void DeleteBitmap();
	void PrepareDC();
	void DeleteDC();
//	void UpdateElevator();
//	void DrawLabel();
//	void SetRowColor(int row, int cnt, COLORREF cr);
//	void DelFont();
//	void InitFontSize(int w, int h);
// Engine:
	void InitializeEngine();
	double DoorDwell(int c);
	void AdjustDoorDwell(int c, double end_of_normal_dwell);
	int DisplayedNCF(int c);
	int GetDisplayedPosition(int c);
	void LogCarCycle(int c, int stage);
	void ScanCalls(int c, int lo_up, int hi_up, int hi_dn, int lo_dn, int standing_f,
		int set_predirection, int *n_call, int *new_target, int *predirection, int *demand_target);
	void MoveRelativity(int c, int relativity, int *status, double *time_to_satisfy);
	void UpdateTarget(int c, int endoper);
	void SetPreDirection(int c);
	void UpdatePreviousQueue(int floor, int increment);
	void HandleNewPassenger(int p);
	void Deboard(int c);
	void Board(int c);
	void HandleCarOperation(int c);
	void HandleSpecialEvent(int e);
	void RecordData();
public:
	void ReadSpecification(void);
	void DoBatchSimulation();
	void MakeUpOtherCarState(int car,CarState *carState);
	int GetOtherCarIndex(int c);
	// Calculate expected boarding people of the car at current floor and return
	int CalculateExpectedBoarder(int car);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DSIMDLG_H__2FD0115E_FE28_43BC_BFDD_C40B662CB014__INCLUDED_)

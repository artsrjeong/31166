// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CCAR_4307280C032A_INCLUDED
#define _INC_CCAR_4307280C032A_INCLUDED
#include "Common.h"
#include "CMotion.h"

/* The following bit mask positions MUST be the same as the OCSS        */
/* for proper translation of ring message contents.                     */
#define         S_FCC             0x01    /* front up car call          */
#define         S_FUHC            0x02    /* front up hall call		*/
#define         S_FDHC            0x04	  /* front down hall call       */
#define         S_FEHC            0x08    /* front emergency hall call  */
#define         S_RCC             0x10    /* rear car call             */
#define         S_RUHC            0x20	  /* rear up hall call		*/
#define         S_RDHC            0x40	  /* rear down hall call	*/
#define         S_REHC            0x80    /* rear emergency hall call   */

#define         E_FCC             0x0100  /* front up car call          */
#define         E_FUHC            0x0200  /* front up hall call		*/
#define         E_FDHC            0x0400  /* front down hall call	*/
#define         E_FEHC            0x0800  /* front emergency hall call  */
#define         E_RCC             0x1000  /* rear car call              */
#define         E_RUHC            0x2000  /* rear up hall call		*/
#define         E_RDHC            0x4000  /* rear down hall call        */
#define         E_REHC            0x8000  /* rear emergency hall call   */

#define         S_UHC           (S_FUHC + S_RUHC)
#define         S_DHC           (S_FDHC + S_RDHC)
#define         S_HC	        (S_FUHC + S_RUHC + S_FDHC + S_RDHC)
#define         S_CC            (S_FCC  + S_RCC)
#define         S_EHC           (S_FEHC + S_REHC)
#define         DBD_SUCC        0x01
#define         DBD_SDCC        0x02


class CMotion;
//##ModelId=4313F6460003
enum STATE {CIS,BUT,CCS,CRS,CAS,CSS,CDOS,CHOLD,CDELAY,CAS_DECEL,CDCS};

//##ModelId=4307280C032A
class CCar 
{
private:
	//##ModelId=4315492A02B6
	int data;
public:
	int assigned_calls[MAXFLOOR];
	int DbdExpectCalls[MAXFLOOR];
	int nTSADelay;
	//##ModelId=4314004801FB
	CMotion motion;
	//##ModelId=431582F1001D
	int GetMotionValue(double time, int type);

	//##ModelId=4315788803DD
	void SetTSACase(int origin, int target, double delay);

	//##ModelId=4313F6460071
	bool isValidTarget(int inx);
	//##ModelId=4313F6460099
	bool isCDELAYCondition(int inx);
	//##ModelId=4313F64600C1
	bool isCHOLDCondition(int inx);
	//##ModelId=4313F64600F3
	void CCS_EntryAction();
	void CRS_EntryAction(int pos,TIME_T time);
	//##ModelId=4313F64600FD
	bool bDispatcherAttached;
	//##ModelId=4313F646011B
	TIME_T DoorDwellTime(int floor,int board,int deboard,int type);
	//##ModelId=4313F64601A7
	//##ModelId=4313F64601D9
	void Init();
	//##ModelId=4313F64601F8
	Parameter	Param;
	//##ModelId=4313F6460203
	CarInfo carInfo;
	//##ModelId=4313F646020C
	UCHAR nPassengers;
	//##ModelId=4313F646022B
	CallList AssignedCalls;
	//##ModelId=4313F6460234
	void CallProcessing();
	//##ModelId=4313F6460248
	SHORT pre_dir;
	//##ModelId=4313F6460270
	SHORT mov_dir;
	//##ModelId=4313F646028E
	void SendTarget();
	//##ModelId=4313F64602A2
	void UpdateState(TIME_T nTime,CarState* car,bool bSend);
	//##ModelId=4313F6460324
	int carId;
	//##ModelId=4313F646034C
	BOOL m_bExecuterAttached;
	//##ModelId=4313F6460374
	int GetDirExec(int f1, int f2);
	//##ModelId=4313F64603CF
	TIME_T prevEventTime;
	//##ModelId=4313F64603E2
	TIME_T GetRunTime(int origin, int dest);
	//##ModelId=4313F64700E1
	double GetStartTime(int inx);
	//##ModelId=4313F6470127
	double GetDelay();
	double GetConditionalDelay();
	//##ModelId=4313F647013B
	int dir;            // 1=up, -1=down, 0=no dir
	//##ModelId=4313F647016D
	int prevOper;
	//##ModelId=4313F6470195
	int target_pos;
	//##ModelId=4313F64701BD
	//##ModelId=4313F64701EF
	UCHAR prevMotion;
	//##ModelId=4313F6470221
	UCHAR prevDoorState;
	//##ModelId=4313F6470249
	UCHAR motionState;		
	//##ModelId=4313F647027B
	UCHAR frontDoorState;
	//##ModelId=4313F64702AD
	UCHAR rearDoorState;
	//##ModelId=4313F64702DF
	BOOL decelDecided;
	//##ModelId=4313F647031B
	BOOL prevDecelDecided;
	//##ModelId=4313F647034D
	double doorCloseEndTime;
	//##ModelId=4313F647038A
	bool isConditionAlive(int inx);
	//##ModelId=4313F64703DA
	int GetStopType(int inx);
	//##ModelId=4313F6480056
	int GetNextTarget(int inx);
	//##ModelId=4313F64800B0
	//##ModelId=4313F64801E7
	TIME_T currentTime;
	//##ModelId=4313F64801FA
	//##ModelId=4313F6480236
	int oper;           // current operation: 0=idle, 1=door opening, 2=dwell, 3=door closing,
	int NCF,prevNCF;
	//##ModelId=4313F6480273
	//##ModelId=4313F64802AF
	//##ModelId=4313F64803B3
	void UpdateSqc(StopSequence paramSqc);
	//##ModelId=4313F649002F
	bool isEnd(int inx);
	//##ModelId=4313F649009D
	int actual_pos;
	//##ModelId=4313F64900E3
	int prevFloor;
	//##ModelId=4313F649012B
	Stop stop[MAX_STOP];
	//##ModelId=4313F649013E
	int curInx;
	//##ModelId=430D7F880316
	CCar* otherCar;
	//##ModelId=4313F649018F
	BuildingInfo *pBuilding;
	//##ModelId=430D774B02F6
	CCar();
	//##ModelId=4313F64901AC
	double FloorToPos(int floor);
	int GetCommittableTime(int s, int d);
	int GetNCP_T3(int s,int d);
	bool isArrivalTimeValid(int new_target);
	//##ModelId=433BA15402F0
	int prevDoorCloseCut;
	//##ModelId=4313F94C01E6
	virtual ~CCar();
	int GetTSADelay(int orgLead,int tgtLead,int orgTrail,int tgtTrail,int minSep);
};

#endif /* _INC_CCAR_4307280C032A_INCLUDED */

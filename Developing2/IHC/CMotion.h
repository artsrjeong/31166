// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CMOTION_430D33890382_INCLUDED
#define _INC_CMOTION_430D33890382_INCLUDED

#include "Common.h"

//##ModelId=430D33890382
class CMotion 
{
public:
	//##ModelId=433BA15202C5
	int n_floor;
	//##ModelId=433BA152038E
	bool m_bParamHard;

	//##ModelId=43157B9000D5
	void SetOriginTarget(int origin, int target);

	//##ModelId=4315839E03DD
	double GetMotionValue(double time, int type);

	//##ModelId=431566000395
	void SetPos(int floor, double pos);

	//##ModelId=4315597201BF
	void SetMotionParam(int nFloors,double jerk, double acceleration, double velocity, double startDelay, double stopDelay,double Afd);

	//##ModelId=4313E96C0233
	double floorPosition[MAXFLOOR]; // floor position in mm. floorPosition[7] = 28000 (m)

	//##ModelId=43127CD50201
	int dir;

	//##ModelId=43127A1702DE
	double t_run_start;

	//##ModelId=43127A4101C6
	int mp_n_break;
	//##ModelId=43127BB7001F
	double mp_break[MAXBREAKPOINT];	// time of breakpoint
	//##ModelId=43127BB70051
	double mp_jerk[MAXBREAKPOINT];	// jerk value between breakpoint and next one
	//##ModelId=43127BB70079
	double mp_accel[MAXBREAKPOINT];	// acceleration value at breakpoint
	//##ModelId=43127BB700AC
	double mp_speed[MAXBREAKPOINT];	// speed value at breakpoint
	//##ModelId=43127BB700DE
	double mp_pos[MAXBREAKPOINT];		// position value at breakpoint

	//##ModelId=4312762C0083
	void MakeBreakPointTableA(double j, double a, double v, double d);

	//##ModelId=4312768501D6
	void MakeBreakPointTableB(double j, double a, double v, double d);

	//##ModelId=4312768B0026
	void MakeBreakPointTableC(double j, double a, double v, double d);

	//##ModelId=43126C640390
	double FloorToPos(int floor);

	//##ModelId=43126D0400D3
	void GenerateFullProfile(double dist);

	//##ModelId=430ED7250134
	CMotion();

	//##ModelId=433BA153008C
	bool ComputeMotionState(double time, double *jerk,double *accel,double *speed,double *pos);
	
	//##ModelId=433BA15301AE
	int GetBreakPointTableIndex(double t);

	//##ModelId=433BA1530208
	int GetNextCommittableFloor(double time,int *into_decel, int should_be_in_flight);
	//##ModelId=433BA15302C7
	int GetNextCommittablePos(double time,int *into_decel, int should_be_in_flight);
	//##ModelId=433BA1530385
	void GenerateProfile(BREAK_TABLE *b);
	int GetESPPos(double time,int* into_decel,int should_be_in_flight);

	//##ModelId=433BA15303D5
	double GetRunTime(int origin, int dest);

	//##ModelId=433BA1540079
	double HardRunTime(int origin, int dest);
	
	//##ModelId=433BA1540105
	int CommittablePosToNCF(double pos);
	//##ModelId=433BA1540160
	int NextCommittableFloor(int *into_decel, int should_be_in_flight);

	//##ModelId=433BA6F702FF
	double GetCommittableTime(int s, int d);

	double GetNCP_T3(int s,int d);
	//##ModelId=433BA6F7038B
	double HardCommitTime(int s, int d);
	//##ModelId=430ED9750243
	 int origin;

	//##ModelId=430ED9E80036
	 int target;
	//##ModelId=4312728703CB
	 double MaxJerk;			//	mm/s^3	
	//##ModelId=43127288000B
	 double MaxAcceleration;	//	mm/s^2
	//##ModelId=431272880029
	 double MaxVelocity;		//	mm/s
	//##ModelId=43127288005B
	 double StartDelay;		//	s
	//##ModelId=431272880079
	 double StopDelay;			//	s
	 double Afd;                // Forced Deceleration mm/ss
private:

	//##ModelId=430D349900C6
	 double dist;

};

#endif /* _INC_CMOTION_430D33890382_INCLUDED */

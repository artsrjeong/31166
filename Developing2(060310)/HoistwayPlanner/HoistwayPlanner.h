// HoistwayPlanner.h
#ifndef	_HOISTWAY_PLANNER_H_
#define	_HOISTWAY_PLANNER_H_
#include "HoistwayPlannerDefs.h"
#include "..\common\2DSIM_DEFS.H"

#define HPLOG	"HP.LOG"
#ifdef	WITH_DEBUG
void initHPLog(void);
void writeHPLog(char *str);
#endif
//void InitializeCalls(void);
void Boarding(int car,int floor);
void Deboarding(int car,int floor);
//int addNewCalls(int car, AssignedDestinationEntryCall newCall);

extern	BuildingInfo	bdInfo;
extern	HoistwayInfo	hwyInfo[MAX_HOISTWAY];
extern	CarInfo			carInfo[MAX_CAR];
extern	Parameter		param;
extern  ParkingInfo		parkingInfo[MAX_CAR];

EXTERN  AssignedDestinationEntryCalls	assignedCalls[MAX_CAR];
EXTERN  FLOOR_T	previousStopFloor[MAX_CAR];
EXTERN  T_TIME		previousStartTime[MAX_CAR];
EXTERN  CallList		uCallList,lCallList;		// local call list for upper car and lower car
#ifdef	WITH_DEBUG
#include <time.h>

EXTERN int virtualAssign;
EXTERN int d_passengerNo; // passengerNo for debug
#endif

EXTERN  void HPInitialize(BuildingInfo *mBdInfo,HoistwayInfo *mHwyInfo,CarInfo *mCarInfo,Parameter *mParam,char* versionName);
#endif


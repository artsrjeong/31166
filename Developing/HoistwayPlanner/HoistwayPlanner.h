// HoistwayPlanner.h
#ifndef	_HOISTWAY_PLANNER_H_
#define	_HOISTWAY_PLANNER_H_
#include "HoistwayPlannerDefs.h"
#include "..\COGS\2DSIM_DEFS.H"

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
#endif


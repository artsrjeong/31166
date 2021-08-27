#if !defined(GDINTERFACECOGS_H)
#define GDINTERFACECOGS_H

#include "..\common\2DInterfaceFunctionType.h"

EXTERN AcceptAssignmentDLL_T AcceptAssignmentDLL;

EXTERN void GiveAssignment(UCHAR carIndex, Call assignedCall, CarState mCarState[MAX_CAR], TIME_T ctCurrentTime);

EXTERN HPPlanParkingDLL_T	HPPlanParkingDLL;

EXTERN void GiveParking(UCHAR car,FLOOR_T lowParkingRange,FLOOR_T highParkingRange, FLOOR_T parkingLandingIndex, 
						TIME_T delay, SHORT option);

#endif // !defined(GDINTERFACECOGS_H)
#include "stdafx.h"

#define EXTERN
#include "GDInterfaceCOGS.h"
#undef EXTERN

void GiveAssignment(UCHAR carIndex,
						 Call assignedCall,
						 CarState mCarState[MAX_CAR],
						 TIME_T ctCurrentTime)
{
	AcceptAssignmentDLL(carIndex, assignedCall, mCarState, ctCurrentTime);
}


void GiveParking(UCHAR car,
				 FLOOR_T lowParkingRange,
				 FLOOR_T highParkingRange, 
				 FLOOR_T parkingLandingIndex, 
				 TIME_T delay, 
				 SHORT option)
{
	HPPlanParkingDLL(car, lowParkingRange, highParkingRange, parkingLandingIndex, delay, option);
}
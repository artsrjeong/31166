// HExec.cpp : Defines the entry point for the DLL application.
//

#include "..\IHC\stdafx.h"
#include "..\common\2dsim_defs.h"				//CSW051213: changed from COGS to common
#include "..\HoistwayPlanner\hoistway_plan.h"
#include "..\common\2DInterfaceFunctionType.h"			//CSW051213: changed from COGS to common
#include <crtdbg.h>

extern "C" 
{
/*******************************************************************************************
 * A function call Dispatcher to hoistway planner, indicating the new call should 
 * be assigned to carIndex.
 ********************************************************************************************/

__declspec(dllexport) void AcceptAssignmentDLL(UCHAR carIndex,
											Call assignedCall,
											CarState mCarState[MAX_CAR],
											TIME_T ctCurrentTime)
 {
	 AcceptAssignment(carIndex, assignedCall, mCarState, ctCurrentTime);
	 return;
 }

 // CSW051112: Add new function
/*******************************************************************************************
 * A function call from Group Dispatcher (GD) to Hoistway Planner
 * Simplay Store the information into a struct for the car
 * When HP wakes up next time, it will use the parameter to make hoistway plan
 *******************************************************************************************/
__declspec(dllexport) void HPPlanParkingDLL(UCHAR car, 
										 FLOOR_T lowParkingRange, 
										 FLOOR_T highParkingRange, 
										 FLOOR_T parkingLandingIndex, 
										 TIME_T delay, 
										 SHORT option)
{
	HPPlanParking(car, lowParkingRange, highParkingRange, parkingLandingIndex, delay, option);
}


}
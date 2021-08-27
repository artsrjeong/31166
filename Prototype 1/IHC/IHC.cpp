// IHC.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "stdio.h"
#include "..\common\2DInterfaceFunctionType.h"	// CSW051213: Changed from COGS to common


#define	EXTERN extern
#include "..\HoistwayPlanner\HoistwayPlanner.h"
#include "..\HoistwayPlanner\HoistwaySchedule.h"
#include "..\HoistwayPlanner\HoistwayPlanSelection.h"
#include "..\HoistwayPlanner\HoistwayPlanGen.h"
#include <crtdbg.h>
#include "..\common\2DSim_Defs.h"		// CSW051213: Changed from COGS to common
#undef	EXTERN



/*******************************************************************************************
 * A function call Dispatcher to hoistway planner, indicating the new call should 
 * be assigned to carIndex.
 ********************************************************************************************/

void AcceptAssignment(UCHAR carIndex,
											Call assignedCall,
											CarState mCarState[MAX_CAR],
											TIME_T ctCurrentTime)
 {
	 int ucar,lcar;
	 UCHAR hwy;
	 CarState lCarState,uCarState;
	 HoistwayPlan bestPlan;

	 hwy=carInfo[carIndex].hoistwayID;
	 ucar=hwyInfo[hwy].idUpperCar;
	 lcar=hwyInfo[hwy].idLowerCar;

	 assignedCall.ctRegistration=ctCurrentTime; 
	 PutHallCall(carIndex,&assignedCall);  // put calls for assigned car

	 if (ucar >= 0)
	 {
		PutCarState(ucar,&(mCarState[ucar])); // put state for upper car in a hoistway
		GetCalls(ucar,&uCallList);
		GetCarState(ucar,&uCarState);
		_RPT4(_CRT_WARN,"car%d Assign time=%f,MOTION=%d,door=%d\n",ucar,uCarState.ctCurrent/1000.0,uCarState.motionState,uCarState.frontDoorState);
	 }

	if (lcar >= 0)
	{
		PutCarState(lcar,&(mCarState[lcar])); // put state for lower car in a hoistway
		GetCalls(lcar,&lCallList);
		GetCarState(lcar,&lCarState);
		_RPT4(_CRT_WARN,"car%d Assign time=%f,MOTION=%d,door=%d\n",lcar,lCarState.ctCurrent/1000.0,lCarState.motionState,lCarState.frontDoorState);
	}

	 HoistwayPlanGen planGeneration;

	 planGeneration.InitHoistwayPlanGen(bdInfo, hwyInfo[hwy]);

	 T_HPSCORE score= planGeneration.generatePlan(hwy, hwyInfo[hwy], carInfo, uCarState, lCarState, uCallList, lCallList, bestPlan);

	 PutPlan(hwy,&bestPlan);

	 ExecutePlan(hwy);
#ifdef	WITH_DEBUG
	 d_passengerNo++;	// for debugging purpose
#endif
	 return;
 }

/*******************************************************************************************
 * This call asked the hoistway planner to regenerate the hoistway plan for a particular
 * hoistway. 
 ********************************************************************************************/
void RegeneratePlan(UCHAR hoistway_index, 
										  TIME_T ctCurrentTime)
{
	 UCHAR ucar,lcar;
	 UCHAR hwy;
	 CarState lCarState,uCarState;
	 HoistwayPlan bestPlan;

	 hwy=hoistway_index;
	 ucar=hwyInfo[hwy].idUpperCar;
	 lcar=hwyInfo[hwy].idLowerCar;
#ifdef	WITH_DEBUG
	 char str[100];
	 sprintf(str,"RegeneratePlan(hwy=%d,time=%f)\n",hoistway_index,MAKE_FLOAT_TIME(ctCurrentTime));
	 writeHPLog(str);
#endif
	 if (ucar >= 0)
	 {
		GetCalls(ucar,&uCallList);
		GetCarState(ucar,&uCarState);
	 }

	 if (lcar >= 0)
	 {
		GetCalls(lcar,&lCallList);
		GetCarState(lcar,&lCarState);
	 }

	 HoistwayPlanGen planGeneration;

	 planGeneration.InitHoistwayPlanGen(bdInfo, hwyInfo[hwy]);

	 T_HPSCORE score= planGeneration.generatePlan(hwy, hwyInfo[hwy], carInfo, uCarState, lCarState, uCallList, lCallList, bestPlan);

	 PutPlan(hwy,&bestPlan);

	 ExecutePlan(hwy);
#ifdef	WITH_DEBUG
	 writeHPLog("Regenerated\n");
	// Regnerate Plan end, 
#endif	
	return;
}

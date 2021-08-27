// HoistwayPlanner.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "stdio.h"
//#include "afxwin.h"
//#include "HoistwayPlannerDefs.h"
//#include "HoistwayPlannerDLL.h"
#include "..\common\2DInterfaceFunctionType.h"	// CSW051213: Changed from COGS to common

#define	EXTERN
#include "HoistwayPlanner.h"
#undef	EXTERN

#define	EXTERN extern
#include "HoistwaySchedule.h"
#include "HoistwayPlanSelection.h"
#include "HoistwayPlanGen.h"
#include <crtdbg.h>
#include "..\common\2DSim_Defs.h"		// CSW051213: Changed from COGS to common
#undef	EXTERN


// HINSTANCE	hinstExec;			// CSW051213: HPE is now in IHC. No need to link
// CSW051213: These parameters will be defined in either GD or IHC
/*
BuildingInfo	bdInfo;
HoistwayInfo	hwyInfo[MAX_HOISTWAY];
CarInfo			carInfo[MAX_CAR];
Parameter		param;
CarState		carState[MAX_CAR];
*/
ParkingInfo		parkingInfo[MAX_CAR];			// CSW051112: Added to store parking information




//CSW051213: Hoistway Planner will not be a DLL entry 
/*
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
*/

// return TRUE if success
// return FALSE if fail
// CSW051213: No need to link HPE from HP
/*
int LinkHoistwayPlanExecutor(char DLLFileName[100])
{
	BOOL	m_bExecutorAttached=FALSE;

	if (hinstExec != NULL)
		FreeLibrary(hinstExec);
	hinstExec=LoadLibrary(DLLFileName);

	if (hinstExec!=NULL)
	{
		PutCarState=(PutCarState_T) GetProcAddress(hinstExec,"PutCarState");
		PutHallCall=(PutHallCall_T) GetProcAddress(hinstExec,"PutHallCall");
		GetCalls=(GetCalls_T) GetProcAddress(hinstExec,"GetCalls");
		GetCarState=(GetCarState_T) GetProcAddress(hinstExec,"GetCarState");

		PutPlan=(PutPlan_T) GetProcAddress(hinstExec,"PutPlan");
		ExecutePlan=(ExecutePlan_T) GetProcAddress(hinstExec,"ExecutePlan");
		if (PutCarState !=NULL && PutHallCall!=NULL && GetCalls!=NULL && GetCarState!=NULL
			&& PutPlan!=NULL && ExecutePlan !=NULL )
		{
			m_bExecutorAttached=TRUE;
		}
	}

	if (m_bExecutorAttached==FALSE)
	{
		if (hinstExec!=NULL)
			FreeLibrary(hinstExec);
		return FALSE;
	}
	else
	{
		return TRUE;
	}

}
*/

/* COARSE TIMING SO OBSOLETED
void PreserveStopFloor(int car,CarState carState)
{
	previousStopFloor[car]=carState.currentNCF;

	if (carState.motionState==STANDING)
	{
		switch(carState.frontDoorState)
		{
		case DOORS_OPENING:
			previousStartTime[car]=MAKE_FLOAT_TIME(carState.tStartOper);
			break;
		case DOORS_CLOSING:
			previousStartTime[car]=MAKE_FLOAT_TIME(carState.tStartOper);
			break;
		case DOORS_OPENED:
			previousStartTime[car]=MAKE_FLOAT_TIME(carState.tStartOper-carInfo[car].tDoorOpening);
			break;
		case DOORS_CLOSED:
			previousStartTime[car]=MAKE_FLOAT_TIME(carState.ctCurrent);//(carState.tStartOper-carInfo[car].tDoorClosing)/(INT_TIME_SCALE*1.0);
			break;
		}
	}
	else  // MOVING
	{
		previousStartTime[car]=MAKE_FLOAT_TIME(carState.ctCurrent);
	}
}
*/
/*
void PreserveStartTime(int car,CarState carState)
{
	previousStartTime[car]=carState.ctCurrent;
}
*/

/*******************************************************************************************
 * A function call from dispatcher to hoistway planner. The call asked the hoistway planner 
 * to return the scoring of the hoistway plan if the new call is assigned to carIndex.
 ********************************************************************************************/
 HPSCORE_T ReturnHoistwayPlanScore(UCHAR carIndex,
													     Call new_call, 
													     CallList	callList[],
													     CarState	carState[],
														 TIME_T currentTime)
 {
	 int ucar,lcar;
	 UCHAR hwy;
	 CarState lCarState,uCarState;
	 HoistwayPlan bestPlan;

	 // if car index is invalid, return max score to indicate infeasible assignment
	 if (carIndex <0 && carIndex >= bdInfo.nCars)
		return T_HPSCORE_MAX;

	 hwy=carInfo[carIndex].hoistwayID;
	 ucar=hwyInfo[hwy].idUpperCar;
	 lcar=hwyInfo[hwy].idLowerCar;

	if (ucar >= 0)
	{
		memcpy(&uCallList,&(callList[ucar]),sizeof(CallList));
		memcpy(&uCarState,&(carState[ucar]),sizeof(CarState));
//		PreserveStopFloor(ucar,uCarState); // shs0719
	}

	if (lcar >= 0)
	{
		memcpy(&lCallList,&(callList[lcar]),sizeof(CallList));
		memcpy(&lCarState,&(carState[lcar]),sizeof(CarState));
//		PreserveStopFloor(lcar,lCarState); // shs0719
	}

	 

	 CallList *assignedList;			// pointer to the CallList where the call is assigned to 

	 if (ucar == carIndex)
		 assignedList = &uCallList;
	 else
		 assignedList = &lCallList;

	 // if no available space for this call, return max score to indicate the assignment
	 // is not feasible
	 if (assignedList->empty_list == -1)
	 {
		return T_HPSCORE_MAX;
	 }

	 // connect the new entry to the waiting call list
	 CALL_INDEX_T ind, newInd;
	 // CSW: 050808 add the new call only when number of passengers is larger than 0.
	 // 0 passenger represent the new call is null, do not create a new entry
	 if (new_call.nPassengers > 0)
	 {
		newInd = assignedList->empty_list;
		CallListEntry *newEntry = &assignedList->calls[newInd];
		// if first call on the waiting list, 
		if ( assignedList->waitingAtHead[new_call.origin] == -1)
		{
			assignedList->waitingAtHead[new_call.origin] = newInd;
			assignedList->waitingAtTail[new_call.origin] = newInd;
			newEntry->prev = -1;
		}
		else
		{
			ind = assignedList->waitingAtTail[new_call.origin];
			assignedList->calls[ind].next = newInd;
			newEntry->prev = ind;
			assignedList->waitingAtTail[new_call.origin] = newInd;
		}

		assignedList->empty_list = newEntry->next;
		newEntry->next = -1;

		// copy data from the newly assigned call
		newEntry->origin = new_call.origin;
		newEntry->destination = new_call.destination;
		newEntry->callType = new_call.callType;
		newEntry->nPassengers = new_call.nPassengers;
		newEntry->ctRegistration = currentTime;
	 }

#ifdef	WITH_DEBUG
	virtualAssign=carIndex;
#endif
	 HoistwayPlanGen planGeneration;

	 planGeneration.InitHoistwayPlanGen(bdInfo, hwyInfo[hwy]);

	 T_HPSCORE score= planGeneration.generatePlan(hwy, hwyInfo[hwy], carInfo, uCarState, lCarState, uCallList, lCallList, bestPlan);

	 //PutPlan(hwy,&bestPlan);

#ifdef	WITH_DEBUG
	virtualAssign=-1;
#endif
	 //ExecutePlan(hwy);
	 return (HPSCORE_T)score;
 }


// shs0711 add implementation
/*******************************************************************************************
 * A function call to Hoistway Planner (HP) module to initialize its local variables 
 ********************************************************************************************/
// shs0712 function return type should be int to indicate initialization error
//void HPInitialize(BuildingInfo *mBdInfo,
//										HoistwayInfo *mHwyInfo,
//										CarInfo *mCarInfo,
//										Parameter *mParam,
//										char *versionName)


void HPInitialize(BuildingInfo *mBdInfo,HoistwayInfo mHwyInfo[],CarInfo mCarInfo[],Parameter *mParam,char* versionName)
{
	// shs0711
	int car,hwy;
	int n_car,n_hwy;
// 	LinkHoistwayPlanExecutor(HOISTWAY_EXECUTOR_FILE);		// CSW051213: No need to link HPE from HP
	memcpy(&bdInfo,mBdInfo,sizeof(BuildingInfo));
	n_hwy=bdInfo.nHoistways;
	n_car=bdInfo.nCars;
	
	for (hwy=0;hwy<n_hwy;hwy++)
	{
		memcpy(&(hwyInfo[hwy]),&(mHwyInfo[hwy]),sizeof(HoistwayInfo));
	}

	for (car=0;car<n_car;car++)
	{
		memcpy(&(carInfo[car]),&(mCarInfo[car]),sizeof(CarInfo)); // shs0713 debugging
	}

	memcpy(&param,mParam,sizeof(Parameter));

	// CSW051112: Clear parking info structure
	for (car = 0; car < n_car; car++)
	{
		parkingInfo[car].lowParkingRange = MAX_FLOOR;
		parkingInfo[car].highParkingRange = MAX_FLOOR;
		parkingInfo[car].parkingLandingIndex = MAX_FLOOR;
		parkingInfo[car].delay = 0;
		parkingInfo[car].option = 0;
	}

	strcpy(versionName,"2005/7/12A");
	/*
	if (LinkHoistwayPlanExecutor(HOISTWAY_EXECUTOR_FILE)==FALSE)
		return FALSE;
	else
		return TRUE;
	*/

#ifdef	WITH_DEBUG
	d_passengerNo=0;
	initHPLog();
	virtualAssign=-1;

	// Clear older contents and put time stamp for log files for Hoistway Plan
	FILE *fp;
	char FileName[100];
	int lowerCarID,upperCarID;
	time_t ltime;
	time( &ltime );

	for (hwy=0;hwy<n_hwy;hwy++)
	{
		lowerCarID=mHwyInfo[hwy].idLowerCar;
		upperCarID=mHwyInfo[hwy].idUpperCar;
		for (int i=0;i<3;i++)
		{
			sprintf(FileName,"HSchedule-%d-%d",lowerCarID,upperCarID);
			switch (i)
			{
			case 0:  // write schedule-x-y.txt
				break;
			case 1:
				sprintf(FileName,"%sV%d",FileName,lowerCarID);
				break;
			case 2:
				sprintf(FileName,"%sV%d",FileName,upperCarID);
				break;
			}
	
			strcat(FileName,".txt");
			
			fp=fopen(FileName,"w");  // clear older contents
			fprintf(fp, "\n\n////////////////////////////////////////////////////\n");//insert date and time
			fprintf(fp, "%s at time: %s\n", FileName, ctime( &ltime ) );
			fclose(fp);
		}
	}
#endif
}


// CSW051112: Add new function
/*******************************************************************************************
 * A function call from Group Dispatcher (GD) to Hoistway Planner
 * Simplay Store the information into a struct for the car
 * When HP wakes up next time, it will use the parameter to make hoistway plan
 *******************************************************************************************/
void HPPlanParking(UCHAR car, 
										 FLOOR_T lowParkingRange, 
										 FLOOR_T highParkingRange, 
										 FLOOR_T parkingLandingIndex, 
										 TIME_T delay, 
										 SHORT option)
{
	// if the car index is out of range, do nothing
	if (car >= MAX_CAR || car <0 )
		return;

	parkingInfo[car].lowParkingRange = lowParkingRange;
	parkingInfo[car].highParkingRange = highParkingRange;
	parkingInfo[car].parkingLandingIndex = parkingLandingIndex;
	parkingInfo[car].delay = delay;
	parkingInfo[car].option = option;
}



extern "C"
{
/*******************************************************************************************
 * A function call from dispatcher to hoistway planner. The call asked the hoistway planner 
 * to return the scoring of the hoistway plan if the new call is assigned to carIndex.
 ********************************************************************************************/
// CSW051213: this function does not need DLL interface
/*
 __declspec(dllexport) HPSCORE_T ReturnHoistwayPlanScoreExt(UCHAR carIndex,
													     Call new_call, 
													     CallList	callList[],
													     CarState	carState[],
														 TIME_T currentTime)
 {
	 return ReturnHoistwayPlanScore(carIndex, new_call, callList, carState, currentTime);
 }
*/

/*******************************************************************************************
 * This call asked the hoistway planner to regenerate the hoistway plan for a particular
 * hoistway. 
 ********************************************************************************************/
 // CSW051213 : This function does not need DLL interface
 /*
__declspec(dllexport) void RegeneratePlanExt(UCHAR hoistway_index, 
										  TIME_T ctCurrentTime)
{
	RegeneratePlan(hoistway_index, ctCurrentTime); 
	return;
}
*/

// shs0711 add implementation
/*******************************************************************************************
 * A function call to Hoistway Planner (HP) module to initialize its local variables 
 ********************************************************************************************/
// shs0712 function return type should be int to indicate initialization error
__declspec(dllexport) void HPInitializeExt(BuildingInfo *mBdInfo,
										HoistwayInfo mHwyInfo[MAX_HOISTWAY],
										CarInfo mCarInfo[MAX_CAR],
										Parameter *mParam,
										char *versionName)
{
	HPInitialize(mBdInfo, mHwyInfo, mCarInfo, mParam, versionName);
}


}


#ifdef	WITH_DEBUG
#include <time.h>

void initHPLog(void)
{
	time_t ltime;

	FILE *fp=fopen(HPLOG,"a");
	fprintf(fp, "\n\n////////////////////////////////////////////////////\n");//insert date and time
	fprintf(fp, "Engine version: ");
	fprintf(fp, ENGINEVERSION);
	time( &ltime );
	fprintf(fp, ", time: %s\n", ctime( &ltime ) );
	fclose(fp);
}

void writeHPLog(char *str)
{
	FILE *fp=fopen(HPLOG,"a");
	fprintf(fp,str);
	fclose(fp);
}
#endif

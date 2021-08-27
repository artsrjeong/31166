// HExec.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "..\common\2dsim_defs.h"				//CSW051213: changed from COGS to common
#include "..\HoistwayPlanner\hoistway_plan.h"
#include "Car.h"
#include "..\common\2DInterfaceFunctionType.h"			//CSW051213: changed from COGS to common
#include <crtdbg.h>


CCar ocss_info[MAX_CAR];
HoistwayPlan localPlan[MAXHWY]; //local hoistplan for executer
int	LocalSeparationFloorTable[MAXHWY][MAX_CARS_IN_HOISTWAY][MAXFLOOR]; // HWY CAR FLOOR

BuildingInfo	bdInfo;					// CSW051213: changed from BdInfo to bdinfo
HoistwayInfo	hwyInfo[MAXHWY];		// CSW051213: changed from HwyInfo to hwyInfo
CarInfo			carInfo[MAX_CAR];		// CSW051213: changed from GCarInfo to carInfo
Parameter		param;					// CSW051213: Changed from Param to param

CarState localCar[MAX_CAR];
FILE *f_ErrorLog;

//##ModelId=4313F1F602F4
typedef struct
{
	int oper;	//0: arrival	1: departure
	double time;
	int floor;
	UCHAR board;
	UCHAR deboard;
} CarTiming;

CarTiming carTiming[MAX_CAR];

HoistwayPlan planDb[MAXHWY]; //DataBase
CarState carDb[MAX_CAR];

CallList AssignedCallsDb[MAX_CAR];    // calls assigned to each car
//##ModelId=4313F1F602FF
typedef int (*COGS_OCSSCommand)(UCHAR carIndex,CarCommand *oper);
//##ModelId=4313F1F60309
typedef void (*GD_GDCancelHallCalls)(UCHAR carIndex,FLOOR_T origin);
//##ModelId=4313F1F60312
typedef void (*GD_GDAcceptCarCall)(UCHAR carIndex,FLOOR_T destination);
//##ModelId=4313F1F6031C
typedef void (*GD_GDCancelCarCalls)(UCHAR carIndex,FLOOR_T destination);
//##ModelId=4313F1F6031E
//typedef void (*HP_RegeneratePlan)(UCHAR hoistway_index,TIME_T ctCurrentTime);
COGS_OCSSCommand OCSSCommand;
GD_GDCancelHallCalls GDCancelHallCalls;
GD_GDAcceptCarCall GDAcceptCarCall;
GD_GDCancelCarCalls GDCancelCarCalls;
//HP_RegeneratePlan RegeneratePlan;

HINSTANCE hinstCogs=NULL;
HINSTANCE hinstDispatcher=NULL;
// HINSTANCE hinstHP=NULL;					// CSW051213: There should be no hinstHP anymore
BOOL bCogsAttached=FALSE;
//#define COGS_FILE "2DSim.exe"

#ifdef	UNIT_TEST_DB
FILE *f_DBUnitTest;
#endif


void HPInitialize(BuildingInfo *mBdInfo,HoistwayInfo mHwyInfo[],CarInfo mCarInfo[],Parameter *mParam,char* version);

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


void updateFloorCall()
{
	int car;
	int index;
	int floor;

	FILE *ffloor = fopen("c:\dFloor.csv","w");
	for(floor=bdInfo.nLandings-1;floor>=0;floor--)
	{
		for(car=0;car<bdInfo.nHoistways*2;car++)
		{
			ocss_info[car].assigned_calls[floor]=0;
			ocss_info[car].DbdExpectCalls[floor]=0;
		}
	}

	for(car=0;car<bdInfo.nHoistways*2;car++)
	{
		for(floor=bdInfo.nLandings-1;floor>=0;floor--)
		{
			if(AssignedCallsDb[car].waitingAtHead[floor]!=-1)
			{
				index=AssignedCallsDb[car].waitingAtHead[floor];
				while(index>=0)
				{
					if(AssignedCallsDb[car].calls[index].origin<AssignedCallsDb[car].calls[index].destination)
					{
						ocss_info[car].assigned_calls[floor]|=S_UHC;
						ocss_info[car].DbdExpectCalls[AssignedCallsDb[car].calls[index].destination]|=DBD_SUCC;						
					}
					if(AssignedCallsDb[car].calls[index].origin>AssignedCallsDb[car].calls[index].destination)
					{
						ocss_info[car].assigned_calls[floor]|=S_DHC;
						ocss_info[car].DbdExpectCalls[AssignedCallsDb[car].calls[index].destination]|=DBD_SDCC;
					}
					index=AssignedCallsDb[car].calls[index].next;
				}
			}
		}
		index=AssignedCallsDb[car].boardedIndex;
		while(index>=0)
		{
			ocss_info[car].assigned_calls[AssignedCallsDb[car].calls[index].destination]|=S_FCC;
			index=AssignedCallsDb[car].calls[index].next;
		}
	}
	

	for(floor=bdInfo.nLandings-1;floor>=0;floor--)
	{
		fprintf(ffloor,"%d,",floor);
		for(car=0;car<bdInfo.nHoistways*2;car++)
		{
			if((ocss_info[car].assigned_calls[floor] & S_UHC)!=0)
				//fprintf(ffloor,"¡ã");
				fprintf(ffloor,"HU_");
			if((ocss_info[car].assigned_calls[floor] & S_DHC)!=0)
				//fprintf(ffloor,"¡å");
				fprintf(ffloor,"HD_");
			if((ocss_info[car].assigned_calls[floor] & S_FCC)!=0)
				//fprintf(ffloor,"¡Ü");
				fprintf(ffloor,"CC_");
			if((ocss_info[car].DbdExpectCalls[floor] & DBD_SUCC)!=0)
				//fprintf(ffloor,"¡ü");
				fprintf(ffloor,"DU_");
			if((ocss_info[car].DbdExpectCalls[floor] & DBD_SDCC)!=0)
				//fprintf(ffloor,"¡ý");
				fprintf(ffloor,"DD");
			fprintf(ffloor," ,");
		}
		fprintf(ffloor,"\n");
	}
	fclose(ffloor);
	try
	{
		FILE *fHandle=fopen("c:\handle.txt","r");
		if(fHandle==NULL)
			return;
		int handle;
		fscanf(fHandle,"%d",&handle);
		fclose(fHandle);
		SendMessage((HWND)handle,0x404,0,0);
	}
	catch(char *str)
	{

	}
}

/*
// Initialize Assigned calls for all calls to be empty.
*/
void InitializeAssignedCallsDB(void)
{
	int car,floor;
	int index;

#ifdef	UNIT_TEST_DB
	f_DBUnitTest = fopen("DBUnitTest.log", "w");
	fclose(f_DBUnitTest);
#endif

	for (car=0;car<MAX_CAR;car++)
	{   // initilalize index for entry
		AssignedCallsDb[car].boardedIndex=-1;
//		AssignedCallsDb[car].waitingIndex=-1;
		AssignedCallsDb[car].empty_list=0;
		AssignedCallsDb[car].expectedTransferHead=-1;
		for (index=0;index<MAX_CALL-1;index++)
		{	// initialize linked list
			AssignedCallsDb[car].calls[index].next=index+1;
			AssignedCallsDb[car].calls[index].prev=index-1;
		}
		for (floor=0;floor<MAX_FLOOR;floor++)
		{
			AssignedCallsDb[car].waitingAtHead[floor]=-1;
			AssignedCallsDb[car].waitingAtTail[floor]=-1;
		}
		AssignedCallsDb[car].calls[index].next=-1;
		AssignedCallsDb[car].calls[index].prev=index-1;
	}
}

#ifdef UNIT_TEST_DB
// For unit testing of call management functions in DB component
// Save call list of floor
void PrintCallList(UCHAR c, FLOOR_T lo_floor, FLOOR_T hi_floor, FLOOR_T origin) 
{
	int f, nDB, nU, nD;
	CALL_INDEX_T p;
	
	// save current call list summary for each floors
	fprintf(f_DBUnitTest, "   floors: up-calls/ down-calls/ deboards :");
	for (f = lo_floor; f <= hi_floor; f++) 
	{
		nDB = 0;  // number deboarding at f
		for (p = AssignedCallsDb[c].boardedIndex; p >= 0; p = AssignedCallsDb[c].calls[p].next)
		{
			if (AssignedCallsDb[c].calls[p].destination == f)
			{
				nDB++;	// Count number of deboarding at floor f
			}
		}
		nU = 0;   // number of up pax waiting at floor f
		nD = 0;   // number of down pax
		// count number of waiting passengers
		for (p = AssignedCallsDb[c].waitingAtHead[f]; p >= 0; p = AssignedCallsDb[c].calls[p].next)
		{
			if (AssignedCallsDb[c].calls[p].destination > f)
			{
			    nU++;	// count number of up waiting passengers at floor f 
			}
			else
			{
			    nD++;	// count number of down waiting passengers at floor f
			}
		}
		// if any particular data exist, print the information of floor/up waiting/dn waiting/deboard at floor
		if (nDB || nU || nD)
			fprintf(f_DBUnitTest, " %d:%d/%d/%d", f, nU, nD, nDB);
	}

	// Show waiting calls at origin floor (Hall call)
	if (origin >= 0 && origin != 255) 
	{
		fprintf(f_DBUnitTest, "\n   Waiting at floor %d (oldest first) to go:", origin);
        for (p = AssignedCallsDb[c].waitingAtHead[origin]; p >= 0;p = AssignedCallsDb[c].calls[p].next)
		{
			fprintf(f_DBUnitTest, " %d", AssignedCallsDb[c].calls[p].destination);
		}
	}
	// Show boarded passengers (car calls)
	if (AssignedCallsDb[c].boardedIndex >= 0) 
	{
		fprintf(f_DBUnitTest, "\n   Boarded to:");
		for (p = AssignedCallsDb[c].boardedIndex; p >= 0; p = AssignedCallsDb[c].calls[p].next)
		{
			fprintf(f_DBUnitTest, " %d", AssignedCallsDb[c].calls[p].destination);
		}
	}
// Show expected transfer list (expected car calls)
	if (AssignedCallsDb[c].expectedTransferHead >= 0) 
	{
		fprintf(f_DBUnitTest, "\n   Expected transfers:");
		for (p = AssignedCallsDb[c].expectedTransferHead; p >= 0;	p = AssignedCallsDb[c].calls[p].next)
		{
			fprintf(f_DBUnitTest, " %d", AssignedCallsDb[c].calls[p].destination);
		}
	}
	fprintf(f_DBUnitTest, "\n");

}  // PrintCallList
#endif

void MakeUpSeparationFloorTable(ULONG ul_minSepartionDist)
{
	int n_car,n_totalfloor;
	int lfloor,ufloor,floor;
	int nHwy,hwy;
	int spareFloor=0;
	ULONG ulMinSeparationDist;

	nHwy=bdInfo.nHoistways;
	n_totalfloor=bdInfo.nLandings;
	ulMinSeparationDist=ul_minSepartionDist;
	for (hwy=0;hwy<nHwy;hwy++)
	{
		n_car=hwyInfo[hwy].nCars;
		// separation table setup from minimum separation distance
		if (n_car > 1) // 2-D hoistway
		{	
			ufloor=0;lfloor=0;
			// lower car separation table setting for minumum separatoin table
			for (lfloor=0;lfloor<n_totalfloor;lfloor++)
			{
				for (;ufloor<n_totalfloor;ufloor++)
				{
					if (bdInfo.landingPosition[lfloor]+ulMinSeparationDist<=bdInfo.landingPosition[ufloor])
					{
						LocalSeparationFloorTable[hwy][LOWER_CAR][lfloor]=ufloor;
						break;
					}
				}
				if (ufloor>=n_totalfloor)
				{
					for (floor=lfloor;floor<n_totalfloor;floor++)
					{
						LocalSeparationFloorTable[hwy][LOWER_CAR][floor]=INVALIDFLOOR;
					}
					break;
				}
			}
			// upper car separation table setting for minimum separation distance
			for (ufloor=n_totalfloor-1;ufloor>=0;ufloor--)
			{
				for (;lfloor>=0;lfloor--)
				{
					if (bdInfo.landingPosition[lfloor]+ulMinSeparationDist <= bdInfo.landingPosition[ufloor])
					{
						LocalSeparationFloorTable[hwy][UPPER_CAR][ufloor]=lfloor;
						break;
					}
				}
				if (lfloor<0)
				{	// lower zone upper car doesn't have separation floor
					for (;ufloor>=0;ufloor--)
					{
						LocalSeparationFloorTable[hwy][UPPER_CAR][ufloor]=INVALIDFLOOR;
					}
					break;
				}
			}
		}
		else	// 1-D hoistway
		{
			for (floor=0;floor<n_totalfloor;floor++)
			{
				LocalSeparationFloorTable[hwy][LOWER_CAR][floor]=n_totalfloor-1;
				LocalSeparationFloorTable[hwy][UPPER_CAR][floor]=0; 
			}
		}
	// Currently minimum separation distance is not applied for pit and first user floor
	// and overhead and last user floor

#ifdef	SEPARATION_FLOOR_EXCEPTION
		// set separation distance exception for pit and first user floor, overhead and last user floor
		if (hwyInfo[hwy].bPit) // pit exist
		{
			for (floor=1; floor <n_totalfloor && LocalSeparationFloorTable[hwy][UPPER_CAR][floor]==INVALIDFLOOR;floor++)
			{
				LocalSeparationFloorTable[hwy][UPPER_CAR][floor]=0;	// pit
			}
			LocalSeparationFloorTable[hwy][LOWER_CAR][0]=1;
		}
		if (hwyInfo[hwy].bOverhead) // overhead exist
		{
			for (floor=n_totalfloor-2;floor>0 && LocalSeparationFloorTable[hwy][LOWER_CAR][floor]==INVALIDFLOOR;floor--)
			{
				LocalSeparationFloorTable[hwy][LOWER_CAR][floor]=n_totalfloor-1; // overheaad
			}
			LocalSeparationFloorTable[hwy][UPPER_CAR][n_totalfloor-1]=n_totalfloor-2;
		}
#endif
	}	// END OF HOISTWAY LOOP
}
//CSW051213: HP is now in the same DLL, no need to link 
/*
bool LinkHP()
{
	bool bHPAttached=TRUE;
	if (hinstHP != NULL)
		FreeLibrary(hinstHP);
	hinstHP=LoadLibrary("HoistwayPlanner.dll");
	if (hinstHP!=NULL)
	{
		RegeneratePlan=(HP_RegeneratePlan)GetProcAddress(hinstHP,"RegeneratePlan");
		if (RegeneratePlan==NULL)
		{
			bHPAttached=FALSE;
		}
	}
	else
		bHPAttached=FALSE;

	if (bHPAttached==FALSE)
	{
		if (hinstHP!=NULL)
			FreeLibrary(hinstHP);
	}
	return bHPAttached;
}
*/

bool LinkDispatcher(char* filename)
{
	bool bDispatcherAttached=TRUE;

	if (hinstDispatcher != NULL)
		FreeLibrary(hinstDispatcher);
	hinstDispatcher=LoadLibrary(filename);
	if (hinstDispatcher!=NULL)
	{
		GDCancelHallCalls=(GD_GDCancelHallCalls)GetProcAddress(hinstDispatcher,"GDCancelHallCalls");
		if (GDCancelHallCalls==NULL)
		{
			bDispatcherAttached=FALSE;
		}
		GDAcceptCarCall=(GD_GDAcceptCarCall)GetProcAddress(hinstDispatcher,"GDAcceptCarCall");
		if (GDAcceptCarCall==NULL)
		{
			bDispatcherAttached=FALSE;
		}
		GDCancelCarCalls=(GD_GDCancelCarCalls)GetProcAddress(hinstDispatcher,"GDCancelCarCalls");
		if (GDCancelCarCalls==NULL)
		{
			bDispatcherAttached=FALSE;
		}
	}
	else
		bDispatcherAttached=FALSE;

	if (bDispatcherAttached==FALSE)
	{
		if (hinstDispatcher!=NULL)
			FreeLibrary(hinstDispatcher);
	}
	return bDispatcherAttached;
}

void LinkCOGS()
{
	int retval=1;
	bCogsAttached=TRUE;
	if (hinstCogs != NULL)
		FreeLibrary(hinstCogs);
	hinstCogs=LoadLibrary(COGS_FILE);

	if (hinstCogs!=NULL)
	{
		OCSSCommand=(COGS_OCSSCommand)GetProcAddress(hinstCogs,"OCSSCommand");

		if (OCSSCommand==NULL)
		{
			bCogsAttached=FALSE;
		}
	}
	else
		bCogsAttached=FALSE;

	if (bCogsAttached==FALSE)
	{
		if (hinstCogs!=NULL)
			FreeLibrary(hinstCogs);
	}
}

void SpyCar(unsigned char car,int cmd_target,int cmd_door,int cmd_preDir)
{
	try
	{
		char str[100];
		sprintf(str,"c:/carState%d.csv",car);
		FILE *fpos = fopen(str,"w");
		if(fpos!=NULL)
		{
			fprintf(fpos,"curInx,time,state,actual_pos,target_pos,NCF,doorState,motionState,mov_dir,decel,cmd_target,cmd_door,cmd_dwell,otherCarControl\n");
			fprintf(fpos,"%d,",ocss_info[car].curInx);
			fprintf(fpos,"%d,",ocss_info[car].currentTime);
			if(cmd_target>=0)
				fprintf(fpos,"CMD,");
			else
			{
				switch(ocss_info[car].oper)
				{
				case CIS:
					fprintf(fpos,"CIS,");
					break;
				case CCS:
					fprintf(fpos,"CCS,");
					break;
				case CRS:
					fprintf(fpos,"CRS,");
					break;
				case CAS:
					fprintf(fpos,"CAS,");
					break;
				case CAS_DECEL:
					fprintf(fpos,"CAS_DECEL,");
					break;
				case CSS:
					fprintf(fpos,"CSS,");
					break;
				case CDOS:
					fprintf(fpos,"CDOS,");
					break;
				case CDCS:
					fprintf(fpos,"CDCS,");
					break;
				case CHOLD:
					fprintf(fpos,"CHOLD,");
					break;
				case CDELAY:
					fprintf(fpos,"CDELAY,");
					break;
				}
			}
			fprintf(fpos,"%d,",ocss_info[car].actual_pos);
			fprintf(fpos,"%d,",ocss_info[car].target_pos);
			fprintf(fpos,"%d,",ocss_info[car].NCF);
			switch(ocss_info[car].frontDoorState)
			{
			case DOORS_CLOSED:
				fprintf(fpos," | ,");
				break;
			case DOORS_CLOSING:
				fprintf(fpos,">><<,");
				break;
			case DOORS_OPENED:
				fprintf(fpos," |  |,");
				break;
			case DOORS_OPENING:
				fprintf(fpos,"<< >>,");
				break;
			}
			switch(ocss_info[car].motionState)
			{
			case MOVING:
				fprintf(fpos,"MOVE,");
				break;
			case STANDING:
				fprintf(fpos,"STAY,");
				break;
			}
			switch(ocss_info[car].mov_dir)
			{
			case UP:
				fprintf(fpos,"UP,");
				break;
			case DN:
				fprintf(fpos,"DN,");
				break;
			case NO_DIR:
				fprintf(fpos,"NO_DIR,");
				break;
			}
			if(cmd_target<0)
				fprintf(fpos,"%d\n",ocss_info[car].decelDecided);
			else
			{
				fprintf(fpos,"%d,",ocss_info[car].decelDecided);
				fprintf(fpos,"%d,",cmd_target);
				switch(cmd_door)
				{
				case DOOR_NOT_OPEN:
					fprintf(fpos,"NOT_OPEN,");
					break;
				case DOOR_NORMAL:
					fprintf(fpos,"NORMAL,");
					break;
				case DOOR_HURRIED:
					fprintf(fpos,"HURRIED,");
					break;
				case DOOR_CLOSE_CUT:
					fprintf(fpos,"CLOSE_CUT,");
					break;
				case DOOR_DELAYED:
					fprintf(fpos,"DELAYED,");
					break;
				}
				switch(cmd_preDir)
				{
				case UP:
					fprintf(fpos,"UP\n");
					break;
				case DN:
					fprintf(fpos,"DN\n");
					break;
				case NO_DIR:
					fprintf(fpos,"NO_DIR\n");
					break;
				}
			}
			fclose(fpos);
		}
	}
	catch(char* str)
	{
		str="test";
	}
	try
	{
		FILE *fHandle=fopen("c:/handle.txt","r");
		if(fHandle==NULL)
			return;
		int handle;
		fscanf(fHandle,"%d",&handle);
		fclose(fHandle);
		::SendMessage((HWND)handle,0x403,car,0);
	}
	catch(char* str)
	{
		str="test";
	}
}

void SpyPlan(unsigned char HW_index)
{
	int lowerInx=0;
	int upperInx=0;
	int lowerPlanInx=0;
	int upperPlanInx=0;
	char type[100];
	try
	{
		char str[100];
		sprintf(str,"c:/plan%d.csv",HW_index);
		FILE *fpos = fopen(str,"w");
		if(fpos!=NULL)
		{
			fprintf(fpos,"UPPER CAR SEQUENCE\n");
			upperInx=localPlan[HW_index].sequences[1].sequence_start;
			while(upperInx>-1)
			{
				switch(localPlan[HW_index].sequences[1].sequence[upperInx].type)
				{
				case STOP_TYPE_DEMAND:
					sprintf(type,"demand");
					break;
				case STOP_TYPE_CONDITIONAL:
					sprintf(type,"conditional");
					break;
				case STOP_TYPE_YIELD:
					sprintf(type,"yield");
					break;
				case STOP_TYPE_PARKING:
					sprintf(type,"parking");
					break;
				case STOP_TYPE_SERVED:
					sprintf(type,"served");
					break;
				case STOP_TYPE_PIVOT:
					sprintf(type,"pivot");
					break;
				}
				fprintf(fpos,"%d,%d,%s,%d\n",upperInx,localPlan[HW_index].sequences[1].sequence[upperInx].floor,type,localPlan[HW_index].sequences[1].sequence[upperInx].departure_predecessor);
				upperInx=localPlan[HW_index].sequences[1].sequence[upperInx].next;
			}
			fprintf(fpos,"LOWER CAR SEQUENCE\n");
			lowerInx=localPlan[HW_index].sequences[0].sequence_start;
			while(lowerInx>-1)
			{
				switch(localPlan[HW_index].sequences[0].sequence[lowerInx].type)
				{
				case STOP_TYPE_DEMAND:
					sprintf(type,"demand");
					break;
				case STOP_TYPE_CONDITIONAL:
					sprintf(type,"conditional");
					break;
				case STOP_TYPE_YIELD:
					sprintf(type,"yield");
					break;
				case STOP_TYPE_PARKING:
					sprintf(type,"parking");
					break;
				case STOP_TYPE_SERVED:
					sprintf(type,"served");
					break;
				case STOP_TYPE_PIVOT:
					sprintf(type,"pivot");
					break;
				}
				fprintf(fpos,"%d,%d,%s,%d\n",lowerInx,localPlan[HW_index].sequences[0].sequence[lowerInx].floor,type,localPlan[HW_index].sequences[0].sequence[lowerInx].departure_predecessor);
				lowerInx=localPlan[HW_index].sequences[0].sequence[lowerInx].next;
			}

			fclose(fpos);
		}
	}
	catch(char *str)
	{
		str="test";
	}
	try
	{
		FILE *fHandle=fopen("c:/handle.txt","r");
		if(fHandle==NULL)
			return;
		int handle;
		fscanf(fHandle,"%d",&handle);
		fclose(fHandle);
		::SendMessage((HWND)handle,0x402,HW_index,0);
	}
	catch(char *str)
	{
		str="test";
	}
}

/*******************************************************************************************
 * A function call to Database for retrieving destination entry calls
 ********************************************************************************************/
void GetCalls(UCHAR carIndex,
									CallList *mCallList)
{
	memcpy(mCallList,&(AssignedCallsDb[carIndex]),sizeof(CallList));
}

/*******************************************************************************************
 * A function call to hoistway planner, informing it a new plan is ready for execution
 ********************************************************************************************/
/*******************************************************************************************
 * A function call to Database for retrieving Hoistway Plan
 ********************************************************************************************/
void GetPlan(unsigned char HW_index,HoistwayPlan * plan)
{
	for(int i=0;i<2;i++)
	{
		for(int j=0;j<MAX_STOP;j++)
		{
			plan->sequences[i].sequence[j]=planDb[HW_index].sequences[i].sequence[j];
		}
		plan->sequences[i].sequence_start=planDb[HW_index].sequences[i].sequence_start;
	}
}

void ErrorLog(char *err)
{
	fclose(f_ErrorLog);
	f_ErrorLog = fopen("Error_Hexec.log", "a");
	fprintf(f_ErrorLog,err);
	fclose(f_ErrorLog);
}

/*******************************************************************************************
 * A function call to Database for inserting destination entry calls
 ********************************************************************************************/

// shs0714 change the function name to PutHallCall from PutCall
UCHAR PutHallCall(UCHAR carIndex, Call *mCall) 
{
	int index,emptyList,prevIndex,origin;
	
	emptyList=AssignedCallsDb[carIndex].empty_list;
	
	if (emptyList<0) // no place to put
	{
		return ERR_FULL;  
	}
	origin=mCall->origin;
	index=AssignedCallsDb[carIndex].waitingAtTail[origin];

	if (index>=0)  // non-empty waiting list
	{	
		/*
		while (AssignedCallsDb[carIndex].calls[index].next!=-1)
		{	// find last element
			index=AssignedCallsDb[carIndex].calls[index].next;
		} */
		// index points last element
		// add new call into waiting list 
		AssignedCallsDb[carIndex].calls[index].next=emptyList; //add
		prevIndex=index;  // pointing to last entry
		index=emptyList; // pointing to new entry
		emptyList=AssignedCallsDb[carIndex].calls[emptyList].next; // update entry for next item of previous emptyList
		AssignedCallsDb[carIndex].empty_list=emptyList; 
		AssignedCallsDb[carIndex].calls[emptyList].prev=-1; //  empty list first entry's prev pointer = -1

		AssignedCallsDb[carIndex].calls[index].prev=prevIndex; // point to previous entry.
		AssignedCallsDb[carIndex].waitingAtTail[origin]=index; // upudate last element pointer
	}
	else // first entry
	{	
		index=emptyList;
		AssignedCallsDb[carIndex].empty_list=AssignedCallsDb[carIndex].calls[index].next;
		AssignedCallsDb[carIndex].waitingAtHead[origin]=index;
		AssignedCallsDb[carIndex].waitingAtTail[origin]=index;
		// contents updating from argument *mCall
		AssignedCallsDb[carIndex].calls[index].prev=-1; // first empty list so no previous pointer
		emptyList=AssignedCallsDb[carIndex].empty_list;
		AssignedCallsDb[carIndex].calls[emptyList].prev=-1;
	}
	AssignedCallsDb[carIndex].calls[index].callType=mCall->callType;
	AssignedCallsDb[carIndex].calls[index].ctRegistration=mCall->ctRegistration;
	AssignedCallsDb[carIndex].calls[index].destination=mCall->destination;
	AssignedCallsDb[carIndex].calls[index].nPassengers=mCall->nPassengers;
	AssignedCallsDb[carIndex].calls[index].origin=mCall->origin;
	AssignedCallsDb[carIndex].calls[index].next=-1;

#ifdef UNIT_TEST_DB
	f_DBUnitTest = fopen("DBUnitTest.log", "a");
	fprintf(f_DBUnitTest, "\nAfter PutHallCall for car %d from %d->%d at t=%6.3lf:\n",
		carIndex, mCall->origin, mCall->destination, MAKE_FLOAT_TIME(mCall->ctRegistration));
    PrintCallList(carIndex, 0, bdInfo.nLandings, mCall->origin);
	fclose(f_DBUnitTest);
#endif
#ifdef COGS_UNIT_TEST
	updateFloorCall();
#endif

	return OK;
}
/*******************************************************************************************
 * A function call to Database for updating Hoistway Plan
 ********************************************************************************************/
void PutPlan(unsigned char HW_index,HoistwayPlan *plan)
{
	for(int i=0;i<2;i++)
	{
		for(int j=0;j<MAX_STOP;j++)
		{
			planDb[HW_index].sequences[i].sequence[j]=plan->sequences[i].sequence[j];
		}
		planDb[HW_index].sequences[i].sequence_start=plan->sequences[i].sequence_start;	
	}
}

void PutCarState(UCHAR carIndex,CarState *car)
{
	memcpy(&carDb[carIndex],car,sizeof(CarState));
}

/*******************************************************************************************
 * A function call to Database for retrieving car state
 ********************************************************************************************/
void GetCarState(UCHAR carIndex,CarState *carState)
{
	memcpy(carState,&carDb[carIndex],sizeof(CarState));
}

bool CheckValidPlan(unsigned char HW_index,int carInx)
{
	int car=(carInx==0) ? hwyInfo[HW_index].idLowerCar : hwyInfo[HW_index].idUpperCar;

	if(localPlan[HW_index].sequences[carInx].sequence_start<=-1)
	{
	#ifdef	WITH_DEBUG
		char msg[100];
		sprintf(msg,"time:%d     StartIndex is %d\n",localCar[car].ctCurrent,localPlan[HW_index].sequences[carInx].sequence_start);
		ErrorLog(msg);
		_RPT1(_CRT_WARN,"Start index is %d\n", localPlan[HW_index].sequences[carInx].sequence_start);
	#endif
		return false;
	}
	return true;
}

void ExecutePlan(UCHAR HW_index)
{
	int carID[MAX_CARS_IN_HOISTWAY];
	GetPlan(HW_index, &localPlan[HW_index]);
	int car;

	carID[0]=hwyInfo[HW_index].idLowerCar;
	carID[1]=hwyInfo[HW_index].idUpperCar;
#ifdef COGS_UNIT_TEST
	SpyPlan(HW_index);
#endif



	for(int i=0;i<MAX_CARS_IN_HOISTWAY;i++)  // shs0811 change 2 to MAX_CARS_IN_HOISTWAY
	{
		car=carID[i];
		if (car<0) continue;
		bool isValidPlan=CheckValidPlan(HW_index,i);
		if(isValidPlan)
		{
			// shs0811 replace all 'HW_index*2+1' with car
			for(int j=0;j<MAX_STOP;j++)
			{
				ocss_info[car].stop[j]=localPlan[HW_index].sequences[i].sequence[j];
			}
			ocss_info[car].curInx=localPlan[HW_index].sequences[i].sequence_start;
		}
		GetCarState(car,&localCar[car]); 
		ocss_info[car].UpdateState(localCar[car].ctCurrent,&localCar[car],false);// for open file
		// CSW050802: Update pre_dir and dir if necessary
		if (ocss_info[car].motionState == STANDING && isValidPlan)	// if the car is not moving, check direction
		{
			PLAN_INDEX_T s1, s2;
			s1 = localPlan[HW_index].sequences[i].sequence_start;
			if (s1 != -1)
			{
				s2 = localPlan[HW_index].sequences[i][s1].next;
				if (s2 != -1)
				{
					ocss_info[car].pre_dir = GetDir(localPlan[HW_index].sequences[i][s1].floor, localPlan[HW_index].sequences[i][s2].floor);
					ocss_info[car].dir = ocss_info[car].pre_dir;
				}
			}
		}
		// CSW050802: end of change
		if(ocss_info[car].oper==CIS && ocss_info[car].curInx!=-1)
		{
			int nextInx=ocss_info[car].stop[ocss_info[car].curInx].next;
			if(nextInx!=-1)
			{
				ocss_info[car].prevEventTime=localCar[car].ctCurrent;
				ocss_info[car].oper=CCS;
			}
		}
	}

	for(i=0;i<MAX_CARS_IN_HOISTWAY;i++)  
	{
		car=carID[i];
		if (car<0) continue;
		ocss_info[car].SendTarget();
		if(ocss_info[car].motionState==STANDING)
		{
			ocss_info[car].CallProcessing();
			ocss_info[car].stop[ocss_info[car].curInx].type=STOP_TYPE_SERVED;
		}
	}		
}

void HPEInitialize(BuildingInfo *mBdInfo,HoistwayInfo mHwyInfo[],CarInfo mCarInfo[],Parameter *mParam,char* version)
{
	// shs0712 insert specification copy and initialization of Call DB	
	int hwy,car;
	InitializeAssignedCallsDB();  // initialize call db
	bool bDispatcherAttached=false;
	bool bHPAttached=false;
	memcpy(&bdInfo,mBdInfo,sizeof(BuildingInfo));
	if(strcmp(version,NO_DISPATCHER)!=0)
	{
		bDispatcherAttached=LinkDispatcher(version); //use version name parameter for dispatcher name
	}

	// bHPAttached=LinkHP();		//CSW051213: No need to link HP since it should be in the same DLL

	for (hwy=0;hwy<MAXHWY;hwy++)
	{
		memcpy(&(hwyInfo[hwy]),&(mHwyInfo[hwy]),sizeof(HoistwayInfo));
	}
	for (car=0;car<MAX_CAR;car++)
	{
		memcpy(&ocss_info[car].carInfo,&(mCarInfo[car]),sizeof(CarInfo));
		memcpy(&ocss_info[car].Param,mParam,sizeof(Parameter));
		if(bDispatcherAttached)
			ocss_info[car].bDispatcherAttached=true;
		else
			ocss_info[car].bDispatcherAttached=false;
		ocss_info[car].carId=car;
		ocss_info[car].otherCar=(car%2==0) ? &ocss_info[car+1] : &ocss_info[car-1];
		ocss_info[car].pBuilding=&bdInfo;
		ocss_info[car].Init();
	}
	MakeUpSeparationFloorTable(mParam->MinSeparationDist);
	char *kkk="2005/07/14";
	strcpy(version,kkk);
	LinkCOGS();
#ifdef WITH_DEBUG
	f_ErrorLog = fopen("Error_Hexec.log", "wt");
#endif
}


extern "C" 
{
__declspec(dllexport) void HPEInitializeDLL(BuildingInfo *mBdInfo,HoistwayInfo mHwyInfo[],CarInfo mCarInfo[],Parameter *mParam,char* version)
{
	// shs0712 insert specification copy and initialization of Call DB	
	int hwy,car;
	InitializeAssignedCallsDB();  // initialize call db
	bool bDispatcherAttached=false;
	bool bHPAttached=false;
	memcpy(&bdInfo,mBdInfo,sizeof(BuildingInfo));
	if(strcmp(version,NO_DISPATCHER)!=0)
	{
		bDispatcherAttached=LinkDispatcher(version); //use version name parameter for dispatcher name
	}

	// bHPAttached=LinkHP();		//CSW051213: No need to link HP since it should be in the same DLL

	for (hwy=0;hwy<MAXHWY;hwy++)
	{
		memcpy(&(hwyInfo[hwy]),&(mHwyInfo[hwy]),sizeof(HoistwayInfo));
	}
	for (car=0;car<MAX_CAR;car++)
	{
		memcpy(&ocss_info[car].carInfo,&(mCarInfo[car]),sizeof(CarInfo));
		memcpy(&ocss_info[car].Param,mParam,sizeof(Parameter));
		if(bDispatcherAttached)
			ocss_info[car].bDispatcherAttached=true;
		else
			ocss_info[car].bDispatcherAttached=false;
		ocss_info[car].carId=car;
		ocss_info[car].otherCar=(car%2==0) ? &ocss_info[car+1] : &ocss_info[car-1];
		ocss_info[car].pBuilding=&bdInfo;
		ocss_info[car].Init();
	}
	MakeUpSeparationFloorTable(mParam->MinSeparationDist);
	char *kkk="2005/07/14";
	strcpy(version,kkk);
	LinkCOGS();
#ifdef WITH_DEBUG
	f_ErrorLog = fopen("Error_Hexec.log", "wt");
#endif
}


__declspec(dllexport) void SetPos(int car,int floor,int pos)
{
	ocss_info[car].motion.SetPos(floor,pos);
}

__declspec(dllexport) void SetTSACase(int car,int origin,int target,int delay)
{
	ocss_info[car].SetTSACase(origin,target,delay);
}

__declspec(dllexport) void MotionCmd(int car, int origin, int target, int delay, int jerk,int acc, int vel)
{
	if(jerk>0) // check motion parameter is valid
	{
		ocss_info[car].motion.MaxJerk=jerk;
		ocss_info[car].motion.MaxAcceleration=acc;
		ocss_info[car].motion.MaxVelocity=vel;
	}
	ocss_info[car].SetTSACase(origin,target,delay);
}

__declspec(dllexport) int GetCommittableTime(int car,int s, int d)
{
	return (int)ocss_info[car].GetCommittableTime(s,d);
}

__declspec(dllexport) int GetNCP_T3(int car,int s,int d)
{
	return (int)ocss_info[car].GetNCP_T3(s,d);
}

__declspec(dllexport) int GetMotionValue(int car,int time,int type)
{
	return (int)ocss_info[car].GetMotionValue(time,type);
}

__declspec(dllexport) void PutCarStateDLL(UCHAR carIndex,CarState *car)
{
	memcpy(&carDb[carIndex],car,sizeof(CarState));
}


__declspec(dllexport) void IHCInitialize(BuildingInfo *mBdInfo,HoistwayInfo mHwyInfo[],CarInfo mCarInfo[],Parameter *mParam,char* version)
{
	HPEInitialize(mBdInfo,mHwyInfo,mCarInfo,mParam,version);
	HPInitialize(mBdInfo,mHwyInfo,mCarInfo,mParam,version);
	//ABCD(mBdInfo);
}

/*********************************************************************************
 * Convert Hall Calls with 'origin' of car 'carIndex' to Expected Car Call 
 * when arriving at origin floor (called n times for n entry)
 * considring predirection start Using waitingAtHead
 *********************************************************************************/
__declspec(dllexport) void ConvertHallCalls(UCHAR carIndex, FLOOR_T origin, CHAR predirection)
{
	int index,expectedIndex,prev,next;
	int found=FALSE;

	int callPredirection=NO_DIR;
	// Waiting passenger deletion
	index=AssignedCallsDb[carIndex].waitingAtHead[origin];

	//index=AssignedCallsDb[carIndex].waitingIndex;
	while(index>=0 && found==FALSE) // Check till found or end of list
	{
		next=AssignedCallsDb[carIndex].calls[index].next;
		if (AssignedCallsDb[carIndex].calls[index].origin == origin)
		{	
			callPredirection=GetDir(origin,AssignedCallsDb[carIndex].calls[index].destination);
			if (callPredirection==predirection)
			{	// matched -> set to expectedTransferHead
				found=TRUE;
				expectedIndex=AssignedCallsDb[carIndex].expectedTransferHead;
				
				prev=AssignedCallsDb[carIndex].calls[index].prev;

				if (expectedIndex>=0) 
				{	// add to expected Transfer list
					while(AssignedCallsDb[carIndex].calls[expectedIndex].next>=0) 
					{
						expectedIndex=AssignedCallsDb[carIndex].calls[expectedIndex].next;
					}
					// last element is expectedIndex
					AssignedCallsDb[carIndex].calls[expectedIndex].next=index;
					AssignedCallsDb[carIndex].calls[index].next=-1; // end of the item of expectedIndex
					AssignedCallsDb[carIndex].calls[index].prev=expectedIndex; // previous item of last item
				}
				else // first expectedTransferHead
				{
					AssignedCallsDb[carIndex].expectedTransferHead=index;
					AssignedCallsDb[carIndex].calls[index].prev=-1;
					AssignedCallsDb[carIndex].calls[index].next=-1;
				}
				// waiting list update
				if (prev<0) // first entry of waiting Index
				{  
					AssignedCallsDb[carIndex].waitingAtHead[origin]=next; 
					if (next>=0) // next item exist
					{
						AssignedCallsDb[carIndex].calls[next].prev=-1; // no previous item													
					}
					else // next<0 then last item
					{
						AssignedCallsDb[carIndex].waitingAtTail[origin]=next;
					}
				}
				else // not first entry
				{
					AssignedCallsDb[carIndex].calls[prev].next=next;
					if (next>=0)
						AssignedCallsDb[carIndex].calls[next].prev=prev;
					else  // next <0 then last item
						AssignedCallsDb[carIndex].waitingAtTail[origin]=next;
				}
			}
		}
		index=next;
	}
#ifdef UNIT_TEST_DB
	f_DBUnitTest = fopen("DBUnitTest.log", "a");
	fprintf(f_DBUnitTest, "\nAfter ConvertHallCalls for car %d at origin %d, predirection %d:\n", carIndex, origin,predirection);
    PrintCallList(carIndex, 0, bdInfo.nLandings, -1);
	fclose(f_DBUnitTest);
#endif
}

/*********************************************************************************
 * Delete expected car call of car 'carIndex' from the list
 * Actually clean all expected car calls in the expected car call list of car 'carIndex'
 *********************************************************************************/
__declspec(dllexport) void DeleteExpectedCarCalls(UCHAR carIndex)
{
	int index,prev,next,emptyList;
	index=AssignedCallsDb[carIndex].expectedTransferHead;

	while(index>=0)
	{
		// linked list deletion
		prev=AssignedCallsDb[carIndex].calls[index].prev;
		next=AssignedCallsDb[carIndex].calls[index].next;
		if (prev<0) // first entry
		{
			AssignedCallsDb[carIndex].expectedTransferHead=next;
			if (next>=0)
				AssignedCallsDb[carIndex].calls[next].prev=-1;
		}
		else // not first entry
		{
			AssignedCallsDb[carIndex].calls[prev].next=next;
			if (next>=0) // not last entry
			{
				AssignedCallsDb[carIndex].calls[next].prev=prev;
			}
		}

		// Add index item to empty list (delete expected car call)
		emptyList=AssignedCallsDb[carIndex].empty_list;
		AssignedCallsDb[carIndex].calls[index].next=emptyList;
		AssignedCallsDb[carIndex].calls[index].prev=-1;
		AssignedCallsDb[carIndex].calls[emptyList].prev=index; //old empty entry's previous is current empty
		AssignedCallsDb[carIndex].empty_list=index;

		index=next; //next loop
	}
}
/*********************************************************************************
 * Remove Call with 'destination' from the boarded list and make them to entry
 * when arriving at the 'destination' floor (Remove all boarded calls with destinatio)
 *********************************************************************************/
__declspec(dllexport) UCHAR RemoveCarCalls(UCHAR carIndex, FLOOR_T destination)
{
	// Serviced passenger deletion
	int index,prev,next,emptyList;
	int	deboarded=0;

	index=AssignedCallsDb[carIndex].boardedIndex;

	while(index>=0)
	{
		next=AssignedCallsDb[carIndex].calls[index].next;
		if (AssignedCallsDb[carIndex].calls[index].destination ==destination)
		{	// matched -> delete call
			deboarded++;
			// linked list deletion
			prev=AssignedCallsDb[carIndex].calls[index].prev;
			if (prev<0) // first entry
			{
				AssignedCallsDb[carIndex].boardedIndex=next;
				if (next>=0)
					AssignedCallsDb[carIndex].calls[next].prev=-1;
			}
			else // not first entry
			{
				AssignedCallsDb[carIndex].calls[prev].next=next;
				if (next>=0) // not last entry
				{
					AssignedCallsDb[carIndex].calls[next].prev=prev;
				}
			}

			// Add index item to empty list (deboarded means empty)
			emptyList=AssignedCallsDb[carIndex].empty_list;
			AssignedCallsDb[carIndex].calls[index].next=emptyList;
			AssignedCallsDb[carIndex].calls[index].prev=-1;
			AssignedCallsDb[carIndex].calls[emptyList].prev=index; //old empty entry's previous is current empty
			AssignedCallsDb[carIndex].empty_list=index;
		}
		index=next;
	}
#ifdef UNIT_TEST_DB
	f_DBUnitTest = fopen("DBUnitTest.log", "a");
	fprintf(f_DBUnitTest, "\nAfter RemoveCarCalls for car %d for destination %d:\n",
		carIndex, destination);
    PrintCallList(carIndex, 0, bdInfo.nLandings, -1);
	fclose(f_DBUnitTest);
#endif
	return deboarded;
}
/*******************************************************************************************
 * A function call to Database removing a call
 * Remove the call from AssignedCallsDb, which matches the contents (origin,dest,regtime)
 *	of *mCall
 * One call to this function will remove one call
 *    - For waiting list, this means hallcall cancel and register expected transfer
 *	  - For boarded list, this means car call cancel and delete call forever
 *	return value : OK - call is removed
 *				   ERR - call is not in list
 ********************************************************************************************/

/*
__declspec(dllexport) void RemoveCall(UCHAR carIndex,
									  Call *mCall)
{

	int index,expectedIndex,prev,next,emptyList;
	int flag=FALSE;
	// Waiting passenger deletion
	index=AssignedCallsDb[carIndex].waitingIndex;
	while(index>=0) // Check all waiting list
	{
		if (AssignedCallsDb[carIndex].calls[index].destination ==mCall->destination
			&& AssignedCallsDb[carIndex].calls[index].origin == mCall->origin
			&& AssignedCallsDb[carIndex].calls[index].ctRegistration == mCall->ctRegistration
			&& AssignedCallsDb[carIndex].calls[index].nPassengers==mCall->nPassengers)
		{	// matched -> set to expectedTransferHead
			flag=TRUE;
			expectedIndex=AssignedCallsDb[carIndex].expectedTransferHead;
			
			prev=AssignedCallsDb[carIndex].calls[index].prev;
			next=AssignedCallsDb[carIndex].calls[index].next;
			if (expectedIndex>=0) 
			{	// add to expected Transfer list
				while(AssignedCallsDb[carIndex].calls[expectedIndex].next>=0) 
				{
					expectedIndex=AssignedCallsDb[carIndex].calls[expectedIndex].next;
				}
				// last element is expectedIndex
				AssignedCallsDb[carIndex].calls[expectedIndex].next=index;
				AssignedCallsDb[carIndex].calls[index].next=-1; // end of the item of expectedIndex
				AssignedCallsDb[carIndex].calls[index].prev=expectedIndex; // previous item of last item
			}
			else // first expectedTransferHead
			{
				AssignedCallsDb[carIndex].expectedTransferHead=index;
				AssignedCallsDb[carIndex].calls[index].prev=-1;
				AssignedCallsDb[carIndex].calls[index].next=-1;
			}
			// waiting list update
			if (prev<0) // first entry of waiting Index
			{  
				AssignedCallsDb[carIndex].waitingIndex=next; 
				if (next>=0) // next item exist
				{
					AssignedCallsDb[carIndex].calls[next].prev=-1; // no previous item													
				}
			}
			else // not first entry
			{
				AssignedCallsDb[carIndex].calls[prev].next=next;
				if (next>=0)
					AssignedCallsDb[carIndex].calls[next].prev=prev;
			}
			break;
		}
		index=AssignedCallsDb[carIndex].calls[index].next;
	}

	if (flag==FALSE) // not in waiting list
	{
		// Serviced passenger deletion
		index=AssignedCallsDb[carIndex].boardedIndex;
		while(index>=0)
		{
			if (AssignedCallsDb[carIndex].calls[index].destination ==mCall->destination
				&& AssignedCallsDb[carIndex].calls[index].origin == mCall->origin
				&& AssignedCallsDb[carIndex].calls[index].ctRegistration == mCall->ctRegistration
				&& AssignedCallsDb[carIndex].calls[index].nPassengers==mCall->nPassengers)
			{	// matched -> delete call
				flag=TRUE;
				// linked list deletion
				prev=AssignedCallsDb[carIndex].calls[index].prev;
				next=AssignedCallsDb[carIndex].calls[index].next;
				if (prev<0) // first entry
				{
					AssignedCallsDb[carIndex].boardedIndex=next;
					if (next>=0)
						AssignedCallsDb[carIndex].calls[next].prev=-1;
				}
				else // not first entry
				{
					AssignedCallsDb[carIndex].calls[prev].next=next;
					if (next>=0) // not last entry
					{
						AssignedCallsDb[carIndex].calls[next].prev=prev;
					}
				}

				// Add index item to empty list (deboarded means empty)
				emptyList=AssignedCallsDb[carIndex].empty_list;
				AssignedCallsDb[carIndex].calls[index].next=emptyList;
				AssignedCallsDb[carIndex].calls[index].prev=-1;
				AssignedCallsDb[carIndex].calls[emptyList].prev=index; //old empty entry's previous is current empty
				AssignedCallsDb[carIndex].empty_list=index;
				break;
			}
			index=AssignedCallsDb[carIndex].calls[index].next;
		}
	}

	if (flag==TRUE)
		return OK;
	else
		return ERR;


}
*/
/*******************************************************************************************
 * A function call to register car call when car accepts and register car calls 
 *   for each car call with destination of 'destFloor' from expectedTransferHead list
 ******************************************************************************************/
// shs0714 changing name from PutCarCall to AcceptCarCalls
 __declspec(dllexport) void AcceptCarCalls(UCHAR carIndex, FLOOR_T destination)
 {
	int expectedIndex;
	int lastBoardIndex;
	int next,prev;

	expectedIndex=AssignedCallsDb[carIndex].expectedTransferHead;
	
	lastBoardIndex=AssignedCallsDb[carIndex].boardedIndex;
	if (lastBoardIndex>=0)
		while(AssignedCallsDb[carIndex].calls[lastBoardIndex].next>=0)
		{
			lastBoardIndex=AssignedCallsDb[carIndex].calls[lastBoardIndex].next;
		}

	while(expectedIndex>=0)
	{
		if (AssignedCallsDb[carIndex].calls[expectedIndex].destination==destination)
		{	// call is registered as boarded index
			prev=AssignedCallsDb[carIndex].calls[expectedIndex].prev;
			next=AssignedCallsDb[carIndex].calls[expectedIndex].next;
			if(AssignedCallsDb[carIndex].expectedTransferHead==expectedIndex)
			{
				AssignedCallsDb[carIndex].expectedTransferHead
					=AssignedCallsDb[carIndex].calls[expectedIndex].next;
			}

			if(prev>=0)
			{	// not first entry
				if(AssignedCallsDb[carIndex].calls[expectedIndex].prev!=lastBoardIndex)
				{
					AssignedCallsDb[carIndex].calls[prev].next=next;
					if (next>=0) // not last entry
						AssignedCallsDb[carIndex].calls[next].prev=prev;
				}
			}
			
			// register to the boarded list
			if (lastBoardIndex<0) // no item boarded
			{
				AssignedCallsDb[carIndex].boardedIndex=expectedIndex; // expected car call is transfered to car call
				AssignedCallsDb[carIndex].calls[expectedIndex].prev=-1;
			}
			else // boarded item exist
			{
				AssignedCallsDb[carIndex].calls[lastBoardIndex].next=expectedIndex;
				AssignedCallsDb[carIndex].calls[expectedIndex].prev=lastBoardIndex;
			}
			lastBoardIndex=expectedIndex;
		}
		expectedIndex=AssignedCallsDb[carIndex].calls[expectedIndex].next;
	}
	if(lastBoardIndex>=0)
		AssignedCallsDb[carIndex].calls[lastBoardIndex].next=-1; //last item
	
	if(AssignedCallsDb[carIndex].expectedTransferHead>=0)
	{
		expectedIndex=AssignedCallsDb[carIndex].expectedTransferHead;
		AssignedCallsDb[carIndex].calls[expectedIndex].prev=-1;
	}
	
#ifdef UNIT_TEST_DB
	f_DBUnitTest = fopen("DBUnitTest.log", "a");
	fprintf(f_DBUnitTest, "\nAfter AcceptCarCalls for car %d for destination %d:\n",
		carIndex, destination);
    PrintCallList(carIndex, 0, bdInfo.nLandings, -1);
	fclose(f_DBUnitTest);
#endif
	
}





__declspec(dllexport) void PutCarStateExt(UCHAR carIndex,CarState *car)
{
	memcpy(&carDb[carIndex],car,sizeof(CarState));
}







/*******************************************************************************************
 * A function call from COGS to hoistway executer.
 * Update Hoistway Planner about car state
 * Hoistway Executer monitor the progress of hoistway execution, take actions when necessary
 ********************************************************************************************/
__declspec(dllexport) void MonitorCarState(UCHAR carIndex,TIME_T ctCurrentTime)
{
	GetCarState(carIndex,&localCar[carIndex]);
	ocss_info[carIndex].UpdateState(ctCurrentTime,&localCar[carIndex],true);
}

__declspec(dllexport) int UpdateCarState(
		double time, // for sync
		int carInx,
		CAR_STATE car	  // Car Status(didn't update)
		)
{
	return -1;
}

__declspec(dllexport) int GetTSADelay(int orgLead,int tgtLead,int orgTrail,int tgtTrail,int minSep)
{
	return ocss_info[0].GetTSADelay(orgLead,tgtLead,orgTrail,tgtTrail,minSep);

}

}




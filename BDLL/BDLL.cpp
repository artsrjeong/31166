// ADLL.cpp : Defines the entry point for the DLL application.
//

// SequentialDispatching.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "..\2dsim_defs.h"
#include "time.h"
#include "stdlib.h"
#include "common.h"
#include "stdio.h"

#define	EXTERN extern
#include "..\DllCommon\DllCommon.h"
#include "infbld.h"
#include "e2p.h"
#include "assign.h"
#include "aarule.h"
#include "asslib.h"
#include "infads.h"
#include "infocs.h"
#undef	EXTERN

/* dispaching rule
0. Coinc Call 
1. upper zone to upper car and lower zone to lower car
2. first distribute upper car evenly to give a chance for lower car to move
3. assign lower car in the hoist where upper car has call as much as possible
4. calculate sum of both car's calls and assign a car with minimum of assigned call
5. move upper car to highst position+1 of lower car's assigned call if upper car is highest pos 
6. priority 
*/

enum STATUS {D0,D1,
	CCS,CAS,CSS,HIDE,HOLD,FREE,DOWN_HOLD,DOWN_FREE,UP_FREE,UP_HOLD} 
dummy;
enum CAR_PRIORITY {MAIN_CAR,SUB_CAR,NONE} dummy2;
int car_status[MAX_OCSS];
int next_status[MAXCAR];

int allsummary[MAXCAR];
int n_CCallSum[MAXCAR];
int n_UpStopSum[MAXCAR];
int rrt[MAXCAR];

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
bool bFirst=false;
bool coinc_cc_flag=FALSE;
int opmode=MIT;
int lowerZoneTopFloor;
int upperZoneBottomFloor;
bool bPit[MAX_OCSS];
bool bOverHead[MAX_OCSS];
int Algorithm0(void);
int Algorithm1(void);
int Algorithm2(void);
int Algorithm3(void);
int Algorithm1D(void);
int Algorithm2D2Way(void);

extern "C" 
{
// DLL Maker Must Specify below information 
static char DLL_NAME[50]		="DLL for ensure Interface";
static char DLL_DESCRIPTION[50]	="For ensuring DLL Interface";
static char NO_ALGORITHM = 4; /* Algorithm 1~3 */
static char ALGORITHM_NAME[10][50]=
{	   //12345678901234567890123456789012345678901234567890
		"PI Reuse (PURE UPPEAK)",
		"PI Reuse (in the construction)",
		"PI Reuse (1D TWO WAY)",
		"PI Reuse (2D TWO WAY)",
//		"abc calculation ",
//		"rrt calculation and valence",
};


__declspec(dllexport) void GetDispatcherDLLInfo(char info[13][50])
{
	int count,size;
	strncpy(info[0],DLL_NAME,50);
	strncpy(info[1],DLL_DESCRIPTION,50);
	info[2][0]=NO_ALGORITHM;
	size=NO_ALGORITHM;
	if (size>9)
	{
		size=9;
	}
	for(count=0; count<size && count <10;count++)
	{
		strncpy(info[3+count],ALGORITHM_NAME[count],50);
	}
}

void Initialize2D2Way()
{
	int car,i;
	for(car=CAR1;car<MAX_OCSS;car++)
	{
		if (car<=bd_info.nCars)	
		{
			ocss_info[car].opmode=NOR; // this algorithm for test case 1(pure up peak)
		}
		else
		{
			ocss_info[car].opmode=NAV;
		}
		ocss_info[car].top_pos=bd_info.nUserFloors;
		ocss_info[car].lobby_pos=1;
		ocss_info[car].bottom_pos=1;
		//allowed_mast 설정 모든 층에 HallCall허용
		for(i=ocss_info[car].lobby_pos;i<=ocss_info[car].top_pos;i++)
		{
			ocss_info[car].allowed_mask[i]=S_HC;
			ocss_info[car].allowed_mask[i]|=S_FCC;
		}
		//lower car는 lower zone에 upper car는 upper zone에
		if(car%2==1) // lower car
		{
			ocss_info[car].upper_car_flag=FALSE;
			ocss_info[car].priority=MAIN_CAR;
		}
		else //upper car
		{
			ocss_info[car].upper_car_flag=TRUE;
			ocss_info[car].priority=NONE;
		}
	}
	bld_info.lz_top_pos=bd_info.nUserFloors/2;
	bld_info.uz_bottom_pos=bld_info.lz_top_pos+1;
	bld_info.top_pos=bd_info.nUserFloors;
	bld_info.bottom_pos=1;
	bld_info.ocss_lobby_pos=1;
	
	lowerZoneTopFloor=bd_info.nUserFloors/2;
	upperZoneBottomFloor=bd_info.nUserFloors/2+1;

	sum_rrt=0;
	rrt_cnt=0;
	avg_rrt=RRT_MAX;
}

void Initialize1D()
{
	int car,i;
	for(car=CAR1;car<MAX_OCSS;car++)
	{
		if (car<=bd_info.nCars)	
		{
			ocss_info[car].opmode=NOR; // this algorithm for 1D (2way)
		}
		else
		{
			ocss_info[car].opmode=NAV;
		}
		ocss_info[car].top_pos=bd_info.nUserFloors;
		ocss_info[car].lobby_pos=1;
		ocss_info[car].bottom_pos=1;
		//allowed_mast 설정 모든 층에 HallCall허용
		for(i=ocss_info[car].lobby_pos;i<=ocss_info[car].top_pos;i++)
		{
			ocss_info[car].allowed_mask[i]=S_HC;
			ocss_info[car].allowed_mask[i]|=S_FCC;
		}
		ocss_info[car].priority=NONE;
	}
	bld_info.top_pos=bd_info.nUserFloors;
	bld_info.bottom_pos=1;
	bld_info.ocss_lobby_pos=1;
	sum_rrt=0;
	rrt_cnt=0;
	avg_rrt=RRT_MAX;
}

/* Initialization of Dispatching DLL
 <return value>
	-1 : No such algorithm
	0 : specification error(parameters are not matched)
	1 : success		*/
__declspec(dllexport) int InitializeDispatcher(BUILDING_STRUCTURE *building , CAR_STATE cars[MAXCAR], 
						 PARAMETER *simul, int algorithm_id )
{
	int i;
	int car;
	assign_car=0;
	moveCar=0;
	bFirst=true;
	bFirstAssign=TRUE;
	memcpy(&bd_info,building,sizeof(BUILDING_STRUCTURE));
	for (i=0;i<MAXCAR;i++)
		memcpy(&(car_info[i]),&(cars[i]),sizeof(CAR_STATE));
	memcpy(&param_info,simul,sizeof(PARAMETER));
	al_id=AlgorithmID=algorithm_id;
	
	// for algorithm 1,2,3
	srand( (unsigned)time( NULL ) );

	nPax=0;
	n_move=0;
	n_floor=building->nTotalFloors;
	al_id=AlgorithmID;
	e2p_init();
	assign_init();
	//Initialization ? (e2p.c)
    E2P_Generate_Fuzzy_Set(e2p_light_med_x0,RRT_TYPE,(int *)&RRT_Light_Medium[0]);
    E2P_Generate_RRT_Low_Fuzzy_Set();
    E2P_Generate_RRT_High_Fuzzy_Set();
    E2P_Generate_Fuzzy_Set(e2p_short_x0, RRT_TYPE,  (int *)&RRT_Short[0]);
    E2P_Generate_Fuzzy_Set(e2p_improv_x0, RRT_TYPE, (int *)&RRT_Improv[0]);
    E2P_Generate_Fuzzy_Set(e2p_light_elderly_x0, PWT_TYPE, (int *)&PWT_Light_Elderly[0]);
    E2P_Generate_PWT_None_Fuzzy_Set();
    E2P_Generate_Fuzzy_Set(e2p_light_long_x0, PWT_TYPE, (int *)&PWT_Light_Long[0]);
	for(car=CAR1;car<MAX_OCSS;car++)
	{
		bPit[car]=false;
		bOverHead[car]=false;
	}

	switch(AlgorithmID)
	{	// initialization according to algorithm
	case 0 : 
		for(car=CAR1;car<MAX_OCSS;car++)
		{
			if (car<=building->nCars)	
			{
				ocss_info[car].opmode=MIT; // this algorithm for test case 1(pure up peak)
			}
			else
			{
				ocss_info[car].opmode=NAV;
			}
			ocss_info[car].top_pos=bd_info.nUserFloors;
			ocss_info[car].lobby_pos=1;
			ocss_info[car].bottom_pos=1;
			//allowed_mast 설정 모든 층에 HallCall허용
			for(i=ocss_info[car].lobby_pos;i<=ocss_info[car].top_pos;i++)
			{
				ocss_info[car].allowed_mask[i]=S_HC;
				ocss_info[car].allowed_mask[i] |= S_FCC;
				ocss_info[car].allowed_mask[i] |= S_FCC;
			}
			//lower car는 lower zone에 upper car는 upper zone에
			if(car%2==1) // lower car
			{
				ocss_info[car].upper_car_flag=FALSE;
			}
			else //upper car
			{
				ocss_info[car].upper_car_flag=TRUE;
			}
			ocss_info[car].allowed_mask[ocss_info[car].lobby_pos] |=S_FCC;
		}
		bld_info.lz_top_pos=bd_info.nUserFloors/2;
		bld_info.uz_bottom_pos=bld_info.lz_top_pos+1;
		bld_info.top_pos=bd_info.nUserFloors;
		bld_info.bottom_pos=1;
		bld_info.ocss_lobby_pos=1;

		lowerZoneTopFloor=bd_info.nUserFloors/2;
		upperZoneBottomFloor=bd_info.nUserFloors/2+1;

		sum_rrt=0;
		rrt_cnt=0;
		avg_rrt=RRT_MAX;
		break;
	case 1 : 
		for(car=CAR1;car<MAX_OCSS;car++)
		{
			if (car<=building->nCars)	
			{
				ocss_info[car].opmode=NOR; // this algorithm for test case 1(pure up peak)
			}
			else
			{
				ocss_info[car].opmode=NAV;
			}
			ocss_info[car].top_pos=bd_info.nUserFloors;
			ocss_info[car].lobby_pos=1;
			ocss_info[car].bottom_pos=1;
			//allowed_mast 설정 모든 층에 HallCall허용
			for(i=ocss_info[car].lobby_pos;i<=ocss_info[car].top_pos;i++)
			{
				ocss_info[car].allowed_mask[i]=S_HC;
				ocss_info[car].allowed_mask[i]|=S_FCC;
			}
			//lower car는 lower zone에 upper car는 upper zone에
			if(car%2==1) // lower car
			{
				ocss_info[car].upper_car_flag=FALSE;
			}
			else //upper car
			{
				ocss_info[car].upper_car_flag=TRUE;
			}
			ocss_info[car].priority=NONE;
		}
		bld_info.lz_top_pos=bd_info.nUserFloors/2;
		bld_info.uz_bottom_pos=bld_info.lz_top_pos+1;
		bld_info.top_pos=bd_info.nUserFloors;
		bld_info.bottom_pos=1;
		bld_info.ocss_lobby_pos=1;

		lowerZoneTopFloor=bd_info.nUserFloors/2;
		upperZoneBottomFloor=bd_info.nUserFloors/2+1;

		sum_rrt=0;
		rrt_cnt=0;
		avg_rrt=RRT_MAX;
		break;
	case 2:
		Initialize1D();
		break;
	case 3: //2D 2Way
		Initialize2D2Way();
		break;
	}

	if (building->nCars<=0 || building->nCars > MAXCAR)
	{
		bld_info.nCars=bd_info.nCars=n_car=8;
		return 0;
	}
	else
	{
		bld_info.nCars=bd_info.nCars=n_car=building->nCars;
		return 1; // success
	}
}

void RecordDebug(double CurrentTime)
{
	FILE *fdebug = fopen("c:\debug.csv", "w");
	int car;
	fprintf(fdebug,"pure_rrts,");
	for(car=1;car<MAX_OCSS;car++)
	{
		fprintf(fdebug, "%d,",pure_rrts[car]);
	}
	fprintf(fdebug,"\n");
	fprintf(fdebug,"pure_pwts,");
	for(car=1;car<MAX_OCSS;car++)
	{
		fprintf(fdebug,"%d,",pure_pwts[car]);
	}
	fprintf(fdebug,"\n");
	fprintf(fdebug,"actual_pos,");
	for(car=1;car<MAX_OCSS;car++)
	{
		fprintf(fdebug,"%d,",ocss_info[car].actual_pos);
	}
	fprintf(fdebug,"\n");
	fprintf(fdebug,"NCF,");
	for(car=1;car<MAX_OCSS;car++)
	{
		fprintf(fdebug,"%d,",ocss_info[car].NCF);
	}
	fprintf(fdebug,"\n");
	fprintf(fdebug,"coinc_cc,");
	for(car=1;car<MAX_OCSS;car++)
	{
		fprintf(fdebug,"%d,",coinc_cc[car]);
	}
	fprintf(fdebug,"\n");
	fprintf(fdebug,"coinc_dcc,");
	for(car=1;car<MAX_OCSS;car++)
	{
		fprintf(fdebug,"%d,",coinc_dcc[car]);
	}
	fprintf(fdebug,"\n");
	fprintf(fdebug,"assigned_cnt,");
	for(car=1;car<MAX_OCSS;car++)
	{
		fprintf(fdebug,"%d,",assigned_cnt[car]);
	}
	fprintf(fdebug,"\n");
	fprintf(fdebug,"dbdExpect_cnt,");
	for(car=1;car<MAX_OCSS;car++)
	{
		fprintf(fdebug,"%d,",DbdExpect_cnt[car]);
	}
	fprintf(fdebug,"\n");
	fprintf(fdebug,"hc_assn_idx,");
	for(car=1;car<MAX_OCSS;car++)
	{
		fprintf(fdebug,"%d,",hc_assn_idx[car]);
	}
	fprintf(fdebug,"\n");
	fprintf(fdebug,"nPax_up,");
	for(car=1;car<MAX_OCSS;car++)
	{
		fprintf(fdebug,"%d,",nPax_up[car]);
	}
	fprintf(fdebug,"\n");

	fprintf(fdebug,"nPax_dn,");
	for(car=1;car<MAX_OCSS;car++)
	{
		fprintf(fdebug,"%d,",nPax_dn[car]);
	}
	fprintf(fdebug,"\n");

	fprintf(fdebug,"nUpStop,");
	for(car=1;car<MAX_OCSS;car++)
	{
		fprintf(fdebug,"%d,",nUpStop[car]);
	}
	fprintf(fdebug,"\n");

	fprintf(fdebug,"nDnStop,");
	for(car=1;car<MAX_OCSS;car++)
	{
		fprintf(fdebug,"%d,",nDnStop[car]);
	}
	fprintf(fdebug,"\n");

	fprintf(fdebug,"mov_dir,");
	for(car=1;car<MAX_OCSS;car++)
	{
		switch(ocss_info[car].mov_dir)
		{
		case 1:
			fprintf(fdebug,"UP,");
			break;
		case 2:
			fprintf(fdebug,"DN,");
			break;
		case 0:
			fprintf(fdebug,"NO,");
			break;
		default:
			fprintf(fdebug,"ERR,");
			break;
		}
	}
	fprintf(fdebug,"\n");
	fprintf(fdebug,"pre_dir,");
	for(car=1;car<MAX_OCSS;car++)
	{
		switch(ocss_info[car].pre_dir)
		{
		case 1:
			fprintf(fdebug,"UP,");
			break;
		case 2:
			fprintf(fdebug,"DN,");
			break;
		case 0:
			fprintf(fdebug,"NO,");
			break;
		default:
			fprintf(fdebug,"ERR,");
			break;
		}
	}
	fprintf(fdebug,"\n");
	fclose(fdebug);
	FILE *ffloor = fopen("c:\dFloor.csv","w");
	for(int floor=bld_info.top_pos;floor>=bld_info.bottom_pos;floor--)
	{
		fprintf(ffloor,"%d,",floor);
		for(car=1;car<MAX_OCSS;car++)
		{
			if((ocss_info[car].assigned_calls[floor] & S_UHC)!=0)
				//fprintf(ffloor,"▲");
				fprintf(ffloor,"HU_");
			if((ocss_info[car].assigned_calls[floor] & S_DHC)!=0)
				//fprintf(ffloor,"▼");
				fprintf(ffloor,"HD_");
			if((ocss_info[car].assigned_calls[floor] & S_FCC)!=0)
				//fprintf(ffloor,"●");
				fprintf(ffloor,"CC_");
			if((asslib_DbdExpectCalls[car][floor] & DBD_SUCC)!=0)
				//fprintf(ffloor,"∧");
				fprintf(ffloor,"DU_");
			if((asslib_DbdExpectCalls[car][floor] & DBD_SDCC)!=0)
				//fprintf(ffloor,"∨");
				fprintf(ffloor,"DD");
			fprintf(ffloor," ,");
		}
		fprintf(ffloor,"\n");
	}
	fclose(ffloor);
	/*
	try
	{
		FILE *fpos = fopen("c:\dPos.csv","w");
		if(fpos!=NULL)
		{
			fprintf(fpos,"%f,",CurrentTime);
			for(car=1;car<MAX_OCSS;car++)
			{
				fprintf(fpos,"%f,",car_info[car-1].currentPos);
			}	
			fprintf(fpos,"\n");
			fclose(fpos);
		}
	}
	catch(char *str)
	{
	}
	*/
	try
	{
		FILE *fHandle=fopen("c:\handle.txt","r");
		if(fHandle==NULL)
			return;
		int handle;
		fscanf(fHandle,"%d",&handle);
		fclose(fHandle);
		SendMessage((HWND)handle,0x401,0,0);
	}
	catch(char *str)
	{

	}
	//fclose(ffloor);
}

void CopyToOCSS()
{
    BYTE_T  carPos,dest_mask,dstPos,start,finish,callDir,start2,finish2;
	for(int car=CAR1;car<MAX_OCSS;car++)
	{
		ocss_info[car].oper=car_info[car-1].oper;
		if(ocss_info[car].oper==HOLD &&
			ocss_info[car].hold_pos==ocss_info[car].actual_pos)
		{
			ocss_info[car].car_status=HOLD;
			ocss_info[car].hold_pos=255; //prevent from come in again
		}
		ocss_info[car].actual_pos=car_info[car-1].currentFloor;
		ocss_info[car].NCF=car_info[car-1].NCF;
		ocss_info[car].target_pos=car_info[car-1].target;
		if(ocss_info[car].mov_dir!=NO_DIR)
		{
			ocss_info[car].old_mov_dir=ocss_info[car].mov_dir;
		}
		switch(car_info[car-1].dir)
		{
			// 1=up, -1=down, 0=no dir
		case 1:
			ocss_info[car].pre_dir=UP_DIR;
			if(ocss_info[car].oper==4 || ocss_info[car].oper==5)
				ocss_info[car].mov_dir=UP_DIR;
			else
				ocss_info[car].mov_dir=NO_DIR;
			break;
		case -1:
			ocss_info[car].pre_dir=DOWN_DIR;
			if(ocss_info[car].oper==4 || ocss_info[car].oper==5)
				ocss_info[car].mov_dir=DOWN_DIR;
			else
				ocss_info[car].mov_dir=NO_DIR;
			break;
		case 0:
			ocss_info[car].mov_dir=ocss_info[car].pre_dir=NO_DIR;
			break;
		}
		
		
		for(int floor=0;floor<=bd_info.nUserFloors;floor++)
		{
			ocss_info[car].assigned_calls[floor]=0;
			ocss_info[car].future_calls[floor]=0;
			if(car_info[car-1].n_CCall[floor]>0)
				ocss_info[car].assigned_calls[floor]=S_FCC;
			if(car_info[car-1].n_UpCall[floor]>0)
				ocss_info[car].assigned_calls[floor]|=S_UHC;
			if(car_info[car-1].n_DnCall[floor]>0)
				ocss_info[car].assigned_calls[floor]|=S_DHC;
			if(car_info[car-1].n_UpStop[floor]-car_info[car-1].n_CCall[floor]>0)
			{
				ocss_info[car].future_calls[floor]=S_FCC;
			}
			if(car_info[car-1].n_DnStop[floor]-car_info[car-1].n_CCall[floor]>0)
			{
				ocss_info[car].future_calls[floor]=S_FCC;
			}
		}
//			0=idle, 1=door opening, 2=dwell, 3=door closing,
//				#define     DOORS_CLOSED    0       /* door state = closed          */
//#define     DOORS_CLOSING   1       /* door state = closing         */
//#define     DOORS_OPENED    2       /* door state = opened          */
//#define     DOORS_OPENING   3       /* door state = opening         */

		switch(car_info[car-1].oper)
		{
		case 1:
			ocss_info[car].f_door_state=DOORS_OPENING;
			break;
		case 2:
			ocss_info[car].f_door_state=DOORS_OPENED;
			break;
		case 3:
			ocss_info[car].f_door_state=DOORS_CLOSING;
			break;
		default:
			ocss_info[car].f_door_state=DOORS_CLOSED;
			break;
		}
		if(ocss_info[car].f_door_state==DOORS_CLOSING)
		{
			carPos=ocss_info[car].actual_pos;
			if (ocss_info[car].pre_dir == UP_DIR)
			{
				dest_mask = DBD_SUCC;
				for (dstPos = carPos+1; dstPos <= ocss_info[car].top_pos; dstPos++)
				{
					if ((asslib_DbdAssignCalls[car][carPos][dstPos] & dest_mask) != 0)
					{
						if ((asslib_DbdExpectCalls[car][dstPos] & dest_mask) != 0)
						{
							if ((ocss_info[car].assigned_calls[dstPos] & S_FCC) != 0)
							{
								//   if(asslib_ValidExpectCalls2(msg_origin,callDir,ocss_info[msg_origin].bottom_pos,carPos-1,dstPos)==FALSE)
								asslib_DbdDeassignCall(car,dest_mask,carPos,dstPos);
							}
							else
							{
								asslib_DbdExpectCallsError[12]++;
								//Closing일 때 COGS에서 사람이 좀 있다 탈 수도 있다  
								//asslib_DbdDeassignCall(car,dest_mask,carPos,dstPos);
							}
						}
						else
						{
							asslib_DbdExpectCallsError[7]++;
							asslib_DbdAssignCalls[car][carPos][dstPos] &= ~dest_mask;
						}
					}
				}
			} //UP_DIR
			else if (ocss_info[car].pre_dir == DOWN_DIR)
			{
				dest_mask=DBD_SDCC;
				for (dstPos = ocss_info[car].bottom_pos; dstPos <= (carPos-1); dstPos++)
				{
					if ((asslib_DbdAssignCalls[car][carPos][dstPos] & dest_mask) != 0)
					{
						if ((asslib_DbdExpectCalls[car][dstPos] & dest_mask) != 0)
						{
							if ((ocss_info[car].assigned_calls[dstPos] & S_FCC) != 0)
							{
								//  if(asslib_ValidExpectCalls2(msg_origin,callDir,ocss_info[msg_origin].top_pos,carPos+1,dstPos)==FALSE)
								asslib_DbdDeassignCall(car,dest_mask,carPos,dstPos);
							}
							else
							{
								asslib_DbdExpectCallsError[13]++;
								//asslib_DbdDeassignCall(car,dest_mask,carPos,dstPos);
								//Closing 일때 COGS에서 사람이 좀 있다 탈 수도 있다.
							}
						}
						else
						{
							asslib_DbdExpectCallsError[8]++;
							asslib_DbdAssignCalls[car][carPos][dstPos] &= ~dest_mask;
						}
					}
				}
			} //DOWN_DIR
		}
	}
}

//	RETURN VALUE:
//		or with	0x01	: New assign
//		or with 0x02	: Move Added/Updated
//		or with	0x04	: Check Point specified in DISPATCHING_DECISION_RESULT
//		or with 0x08	: Re-assigned?
__declspec(dllexport) int GetDispatcherDecision(
		double currentTime, // for sync
		CAR_STATE cars[MAXCAR],	  // Car Status(didn't update)
		PASSENGER_STATUS *passengers, // in this structure assignment will be transfered from dispatcher
		MOVE_COMMANDS *moveCommands, // move command from dispatcher
		MOTIONPROFILE_COMMANDS *motionProfileCommand,
		EXTRA_MSGS	*extraMsg	// reserved for extra messages
		) 
{
	int retval=0;

	Preprocessing(currentTime,cars,passengers,moveCommands,motionProfileCommand,
		extraMsg);
	//ocss_info 구조체로 넘긴다.
	CopyToOCSS();
	switch(AlgorithmID)
	{
	case 0:	retval=Algorithm0();
		break;
	case 1: retval=Algorithm1();
		break;
	case 2: retval=Algorithm1D();
		break;
	case 3: retval=Algorithm2D2Way();
		break;
/*
	case 3: retval=Algorithm3();
		break;
	case 2: retval=Algorithm2();
		break;
*/
	}
	RecordDebug(currentTime);
	return retval;
}
} // end of extern "C"

// sequential dispatching/ no movements


int Algorithm3()
{
	int retval=0;
	int i,passID,count;
	int origin,dest;
	int nCurrentCalls=mPassengers.nCurrentCalls;
	static double previousMoveTime=-1;
	MOVE_PARAMETER move;

	if(bFirst)
	{
		bFirst=false;
		for(int i=0;i<n_car;i++)
		{
			memset(&move,0,sizeof(MOVE_PARAMETER));
			move.car=i;
			move.initiation_basis=0; // time base
			move.release_basis=0;
			move.t_initiation=mCurrentTime;
			move.t_release=((i%2)==0) ? mCurrentTime+1000 : mCurrentTime;
			move.destination=((i%2)==0) ? 0 : 1;
			car_status[i]=((i%2)==0) ? DOWN_HOLD : FREE;
			SendMove(move,MOVE_ADD);
			retval|=0x02;
		}
	}
	else
	{
		//upper 카가 위로 움직이기 시작했으면 Lower Car는 1층 이동 대기
		for(int i=0;i<n_car/2;i++)
		{
			int lower_car=i*2;
			int upper_car=i*2+1;
			if((car_info[upper_car].target>car_info[upper_car].floor) &&
				car_info[upper_car].floor==1 &&
				(car_status[lower_car]==DOWN_HOLD) &&
				car_info[lower_car].floor==0)
			{
				car_status[lower_car]=FREE;
				memset(&move,0,sizeof(MOVE_PARAMETER));
				move.car=lower_car;
				move.initiation_basis=0; // time base
				move.release_basis=0;
				move.t_initiation=mCurrentTime;
				move.t_release=mCurrentTime;
				move.destination=1;
				SendMove(move,MOVE_ADD);
				retval|=0x02;
			}
		}
		for(i=0;i<n_car/2;i++)
		{
			int lower_car=i*2;
			int upper_car=i*2+1;
			//lower car 가 pit로 이동하기 시작했으면 Upper Car는 1층 이동 대기
			if((car_info[lower_car].target<car_info[lower_car].floor || (car_info[lower_car].floor==0 && car_status[lower_car]==DOWN_HOLD)) &&
				(car_status[upper_car]==DOWN_HOLD))
			{
				car_status[upper_car]=FREE;
				memset(&move,0,sizeof(MOVE_PARAMETER));
				move.car=upper_car;
				move.initiation_basis=0; // time base
				move.release_basis=0;
				move.t_initiation=mCurrentTime;
				move.t_release=mCurrentTime;
				move.destination=1;
				SendMove(move,MOVE_ADD);
				retval|=0x02;
			}
		}
		//upper 카가 윗쪽에 더이상 콜이 없으면
		for( i=0;i<n_car/2;i++)
		{
			int lower_car=i*2;
			int upper_car=i*2+1;
			int upper_high=0;
			int lower_high=0;
			int lower_high2=0;
			int upper_high2=0;
			allsummary[lower_car]=0;
			allsummary[upper_car]=0;
			n_CCallSum[lower_car]=0;
			n_CCallSum[upper_car]=0;
			n_UpStopSum[upper_car]=0;
			n_UpStopSum[lower_car]=0;
			for(int floor=0;floor<=bd_info.nUserFloors;floor++)
			{
				if(car_info[upper_car].n_CCall[floor]>0 && floor>upper_high)
				{
					upper_high=floor;
					allsummary[upper_car]=S_FCC;
					n_CCallSum[upper_car]+=car_info[upper_car].n_CCall[floor];
				}
				if(car_info[upper_car].n_UpStop[floor]>0 && floor>upper_high2)
				{
					upper_high2=floor;
					allsummary[upper_car]=S_FCC;
					n_UpStopSum[upper_car]+=car_info[upper_car].n_UpStop[floor];
				}
				if(car_info[lower_car].n_CCall[floor]>0 && floor>lower_high)
				{
					lower_high=floor;
					allsummary[lower_car]=S_FCC;
					n_CCallSum[lower_car]+=car_info[lower_car].n_CCall[floor];
				}
				if(car_info[lower_car].n_UpStop[floor]>0 && floor>lower_high2)
				{
					lower_high2=floor;
					allsummary[lower_car]=S_FCC;
					n_UpStopSum[lower_car]+=car_info[lower_car].n_UpStop[floor];
				}
			}
			//upper car call 서비스를 다 했으면
			if(car_info[upper_car].floor==upper_high && car_status[upper_car]==FREE)
			{
				car_status[upper_car]=DOWN_HOLD;
				memset(&move,0,sizeof(MOVE_PARAMETER));
				move.car=upper_car;
				move.initiation_basis=0; // time base
				move.release_basis=0;
				move.t_initiation=mCurrentTime;
				move.t_release=mCurrentTime+1000;
				move.destination=(lower_high>=lower_high2) ? lower_high+1 : lower_high2+1;
				SendMove(move,MOVE_ADD);
				retval|=0x02;
			}
			//lower car가 서비스를 다 끝내거나
			//upper car가 서비스를 다 끝낼 때까지 콜이 없으면 pit로 가서 기다림
			if((car_info[lower_car].floor==lower_high 
				&& car_status[lower_car]==FREE
				&& car_info[lower_car].floor!=0) ||
				(car_info[lower_car].floor==1
				&& car_status[lower_car]==FREE
				&& lower_high==0 
				&& lower_high2==0
				&& car_info[upper_car].floor==upper_high))
			{
				car_status[lower_car]=DOWN_HOLD;
				memset(&move,0,sizeof(MOVE_PARAMETER));
				move.car=lower_car;
				move.initiation_basis=0; // time base
				move.release_basis=0;
				move.t_initiation=mCurrentTime;
				move.t_release=mCurrentTime+1000;
				move.destination=0;
				SendMove(move,MOVE_ADD);
				retval|=0x02;
			}
		//lower car에 할당 있고 Upper Car에 할당 없으면 Upper Car는 UpperZone 바닥 이동 대기
			
			if(car_info[upper_car].floor==1 
				&& allsummary[upper_car]==0
				&& allsummary[lower_car]>0)
			{
				car_status[upper_car]=DOWN_HOLD;
				memset(&move,0,sizeof(MOVE_PARAMETER));
				move.car=upper_car;
				move.initiation_basis=0; // time base
				move.release_basis=0;
				move.t_initiation=mCurrentTime;
				move.t_release=mCurrentTime+1000;
				move.destination=lowerZoneTopFloor+1;
				SendMove(move,MOVE_ADD);
				retval|=0x02;
			}
		}
	}
	if (nCurrentCalls>=1)
	{	
		// 0~1/2 * nUserFloor ==> Car 1
		// 1/2~nUserFloor ==> Car 2
		// 0~nUserFloor ==> 2nd Hoistway Lower Car or 3rd Hoistway Upper Car depending 
		//			on the distance between hall call pos and car's NCF(No direction check)
		//			2nd Hoistway Upper Car to Overhead,3rd Hoistway Lower Car to Pit

		for (i=0;i<nCurrentCalls;i++)
		{
			origin=mPassengers.passengerCall[i].originFloor;
			dest=mPassengers.passengerCall[i].destFloor;
			coinc_cc_flag=FALSE;
			if ((origin>=0) &&  (dest>=0) &&
				(origin<MAXFLOOR) && (origin<MAXFLOOR))
			{// valid passenger request
				passID=mPassengers.passengerCall[i].passengerID;
				if(passID==370)
				{
					int test=1;
				}
				// determine assignments
				// Divided Area
				if (opmode==MIT && origin==1)
				{
					//coinc call check
					for(int i=0;i<n_car;i++)
					{
						if((car_info[i].n_UpStop[dest]-car_info[i].n_CCall[dest])>0)
						{
							assign_car=i;
							coinc_cc_flag=TRUE;
							break;
						}
					}
					//Coinc Call이 없으면 각 카마다 콜 갯수를 세서 분배

					if(coinc_cc_flag==FALSE)
					{
						int min_car=0;
						int scan_start=(dest <= lowerZoneTopFloor) ? 1 : lowerZoneTopFloor+1;
						int scan_stop=(dest <= lowerZoneTopFloor) ? lowerZoneTopFloor : bd_info.nUserFloors;
						int min_callNo=MAXFLOOR*2;
						int min_upStopNo=MAXFLOOR*2;
						int upperNoCall=0;
						for(int i=0;i<(n_car/2);i++)
						{
							int car_no=(dest <= lowerZoneTopFloor) ? i*2 : i*2+1; //if dst is low , check lower car
							//만일 upper car에 할당이 없으면 lower car에 할당 안함
							if(dest<=lowerZoneTopFloor)
							{
								// upper 카에 할당된 콜이 없고 upper car가 내려오거나 맨 뒤층에 있으면 Lower Car에는 할당 안함
								if((n_UpStopSum[car_no+1]-n_CCallSum[car_no+1])<=0 && car_info[car_no+1].target<=car_info[car_no+1].floor)
								{
									//upper카에 콜없는 Car의 갯수가 1 이하이면 할당에 참여 안함
									upperNoCall++;
									if(upperNoCall<=1)
										continue;
								}
							}
							int call_no=0;
							int upStop_no=0;
							for(int j=scan_start;j<=scan_stop;j++)
							{
								if(car_info[car_no].n_UpStop[j]>0)
									upStop_no++;
								if(car_info[car_no].n_CCall[j]>0)
									call_no++;
							}
							if((upStop_no+call_no)<min_upStopNo)
							{
								min_car=car_no;
								min_upStopNo=upStop_no+call_no;
							}
							//upper Zone 일때 1층 문열고 있으면 거기에 할 당
							if(dest>lowerZoneTopFloor)
							{
								if(car_info[car_no].floor==1 && car_info[car_no].oper<=3)
								{
									if(call_no<2 && upStop_no<2)
									{
										min_car=car_no;
										break;
									}
								}
							}
						}
						//upper Zone이고
						//카가 이미 올라가고 있는 것을 선택 했는데 내려오는 카가 있다면 
						//거기에 할당
						if(dest>lowerZoneTopFloor
							&& car_info[min_car].target>car_info[min_car].floor)
						{
							for(int i=0;i<(n_car/2);i++)
							{
								int car_no=(dest <= lowerZoneTopFloor) ? i*2 : i*2+1; //if dst is low , check lower car
								//down car를 찾음
								if(car_info[car_no].target<car_info[car_no].floor)
								{
									int call_no=0;
									int scan_start=(dest <= lowerZoneTopFloor) ? 1 : lowerZoneTopFloor+1;
									int scan_stop=(dest <= lowerZoneTopFloor) ? lowerZoneTopFloor : bd_info.nUserFloors;
									int upStop_no=0;
									for(int j=scan_start;j<=scan_stop;j++)
									{
										if(car_info[car_no].n_UpStop[j]>0)
											upStop_no++;
										if(car_info[car_no].n_CCall[j]>0)
											call_no++;
									}
									if(upStop_no<2)  //upstop 갯수가 적으면 거기 할당
										min_car=car_no;
								}
							}
						}
						assign_car=min_car;
					}
				}
				if(bFirstAssign==TRUE)
				{
					bFirstAssign=FALSE;
					int min_floor=MAXFLOOR;
					//처음 할당할 때는 Upper카중 제일 낮은카에 할당
					for(int i=0;i<(n_car/2);i++)
					{
						int car_no=i*2+1; //check lower car
						if(car_info[car_no].floor<min_floor)
						{
							assign_car=car_no;
							min_floor=car_info[car_no].floor;
						}
					}
				}

				// send assignment
				if (passID>=0 && passID<MAXPAX)
				{	
					count=mPassengers.passengerCall[i].nPassengers;
					SendAssignment(passID,origin,dest,DE_NORMAL_CALL,count,assign_car,
						PAX_ASSIGN);
				}
				
				retval|=0x01; // NEW ASSIGN
			}
		}
	}
	return retval;
}

int MoveHold(int ret,int car,int pos)
{
	if(ocss_info[car].oper==HIDE)
		return ret;
	if(pos==ocss_info[car].actual_pos && ocss_info[car].oper==HOLD)
		return ret;
	//Hold 명령이 이미 갔으면 return
	if(pos==ocss_info[car].hold_pos)
		return ret;
	int retval=0;
	MOVE_PARAMETER move;
	memset(&move,0,sizeof(MOVE_PARAMETER));
	move.car=car-1;
	move.initiation_basis=0; // time base
	move.release_basis=0;
	move.t_initiation=mCurrentTime;
	move.t_release=mCurrentTime+1000;
	move.destination=pos;
	ocss_info[car].hold_pos=pos;
	SendMove(move,MOVE_ADD);
	retval|=0x02;
	return retval;
}

int MoveFree(int ret,int car,int pos)
{
	int retval=0;
	MOVE_PARAMETER move;
	memset(&move,0,sizeof(MOVE_PARAMETER));
	move.car=car-1;
	move.initiation_basis=0; // time base
	move.release_basis=0;
	move.t_initiation=mCurrentTime;
	move.t_release=mCurrentTime+0.1;
	move.destination=pos;
	if(ocss_info[car].car_status==FREE)
		return ret;
	ocss_info[car].car_status=FREE;
	SendMove(move,MOVE_ADD);
	retval|=0x02;
	return retval;
}

int HoistWayManager2D2Way()
{
	int i;
	int retval=0;
	MOVE_PARAMETER move;
	if(bFirst)
	{
		bFirst=false;
		for(int car=1;car<MAX_OCSS;car++)
		{
			retval=MoveHold(retval,car,ocss_info[car].actual_pos);
		}
		return retval;
	}
	// Car Status 변화
	/*
	for(i=0;i<n_car;i++)
	{
		if(car_info[i].target!=car_info[i].floor)
			car_status[i]=CAS;
		if(car_status[i]==CAS && car_info[i].target==car_info[i].floor)
			car_status[i]=CSS;
		if(car_status[i]==CSS && next_status[i]==HOLD)
		{
			next_status[i]=FREE;
			car_status[i]=HOLD;
		}
	}
	*/
	for(i=0;i<MAX_OCSS/2;i++)
	{
		int lower_car=i*2+1;
		int upper_car=i*2+2;
		int upper_high,upper_low,lower_high,lower_low;
		asslib_update_summary(lower_car);
		asslib_update_summary(upper_car);
		upper_high=ads_HighestPosition(upper_car,ocss_info[upper_car].NCF,ocss_info[upper_car].actual_pos,TRUE);
		upper_low=ads_LowestPosition(upper_car,ocss_info[upper_car].NCF,ocss_info[upper_car].actual_pos,TRUE);
		lower_high=ads_HighestPosition(lower_car,ocss_info[lower_car].NCF,ocss_info[lower_car].actual_pos,TRUE);
		lower_low=ads_LowestPosition(lower_car,ocss_info[lower_car].NCF,ocss_info[lower_car].actual_pos,TRUE);
		/*
		if(ocss_info[upper_car].dnsummary!=0 && ocss_info[lower_car].allsummary==0)
		{
			retval=MoveHold(lower_car,upper_low-1);
		}
		*/
		if(ocss_info[lower_car].priority==MAIN_CAR &&
 			ocss_info[lower_car].allsummary!=0 &&
			ocss_info[upper_car].allsummary==0 &&
			ocss_info[upper_car].actual_pos!=bld_info.top_pos+1 &&
			ocss_info[upper_car].oper==HOLD)
		{
			retval=MoveHold(retval,upper_car,bld_info.top_pos+1);
			continue;
		}

		//둘다 HOLD 일 경우 둘중 하나를 풀어준다.
		else if(ocss_info[upper_car].oper>=HIDE &&
			ocss_info[lower_car].oper>=HIDE)
		{
			if(ocss_info[lower_car].allsummary!=0 &&
					ocss_info[upper_car].allsummary!=0)
			{
				if(ocss_info[lower_car].priority==MAIN_CAR)
					retval=MoveFree(retval,lower_car,ocss_info[lower_car].actual_pos);
				else if(ocss_info[upper_car].priority==MAIN_CAR)
					retval=MoveFree(retval,upper_car,ocss_info[upper_car].actual_pos);
				else
				{
					if(ocss_info[lower_car].hold_pos==0)
						retval=MoveFree(retval,upper_car,ocss_info[upper_car].actual_pos);
					else if(ocss_info[upper_car].hold_pos==bld_info.top_pos+1)
						retval=MoveFree(retval,lower_car,ocss_info[lower_car].actual_pos);
				}
			}
			else if(ocss_info[lower_car].allsummary!=0 &&
					ocss_info[upper_car].allsummary==0)
			{
				retval=MoveFree(retval,lower_car,ocss_info[lower_car].actual_pos);
				ocss_info[lower_car].priority==MAIN_CAR;
				ocss_info[upper_car].priority==NONE;
			}
			else if(ocss_info[lower_car].allsummary==0 &&
					ocss_info[upper_car].allsummary!=0)
			{
				retval=MoveFree(retval,upper_car,ocss_info[upper_car].actual_pos);
				ocss_info[upper_car].priority==MAIN_CAR;
				ocss_info[lower_car].priority==NONE;
			}
		}
		
		else if(ocss_info[upper_car].oper==HOLD 
			&& ocss_info[lower_car].mov_dir==DOWN_DIR
			&& ocss_info[upper_car].dnhall_flag==TRUE
			&& ocss_info[upper_car].next_down>ocss_info[lower_car].actual_pos)
		{
			retval=MoveFree(retval,upper_car,ocss_info[upper_car].actual_pos);
		}

		else if(ocss_info[lower_car].oper==HOLD 
			&& ocss_info[upper_car].mov_dir==UP_DIR
			&& ocss_info[lower_car].uphall_flag==TRUE
			&& ocss_info[lower_car].next_up<ocss_info[upper_car].actual_pos)
		{
			retval=MoveFree(retval,lower_car,ocss_info[lower_car].actual_pos);
		}
		
		//lower car Down 서비스 끝내고 홀딩 중일때 upper 카가 너무 내려 오면
		if(ocss_info[upper_car].mov_dir==DOWN_DIR 
			&& ocss_info[lower_car].oper==HOLD
			&& ocss_info[upper_car].next_down<=ocss_info[lower_car].actual_pos)
			retval=MoveHold(retval,lower_car,0);	
		//lower car is moving down
		if(ocss_info[lower_car].mov_dir==DOWN_DIR && ocss_info[lower_car].actual_pos!=0)
		{
			//lower car 가 바닥까지 왔으면 pit에 Hold 시킴
			if(ocss_info[lower_car].dnsummary==0 &&
				(ocss_info[lower_car].atsummary&S_DHC)==0)
			{
				int upper_car_dh=0;
				for(int floor=ocss_info[lower_car].actual_pos;floor<ocss_info[upper_car].actual_pos;floor++)
				{
					if(ocss_info[upper_car].assigned_calls[floor] & S_DHC)
						upper_car_dh++;
				}
				if(upper_car_dh<=1 && ocss_info[lower_car].hold_pos!=0)
				{
					bPit[lower_car]=true;
				}
				//서비스 끝냈으면 일단 홀딩
				else if(ocss_info[lower_car].hold_pos!=0)
					retval=MoveHold(retval,lower_car,ocss_info[lower_car].actual_pos);													
			}
		}

		if(ocss_info[upper_car].mov_dir==UP_DIR && ocss_info[upper_car].actual_pos!=bld_info.top_pos+1)
		{
			if(ocss_info[upper_car].upsummary==0 &&
				(ocss_info[upper_car].atsummary&S_UHC)==0)
			{
				if(ocss_info[upper_car].hold_pos!=bld_info.top_pos+1)
				{
					bOverHead[upper_car]=true;
				}
			}
		}
		if(ocss_info[upper_car].mov_dir==DOWN_DIR && bOverHead[upper_car]==false)
		{
			//upper car 의 next target 이 lower car의 현재 위치보다 낮으면 Hold
			if(ocss_info[upper_car].f_door_state==DOORS_CLOSING &&
				ocss_info[upper_car].pre_dir==DOWN_DIR &&
				ocss_info[upper_car].next_down<=ocss_info[lower_car].actual_pos &&
				ocss_info[upper_car].allsummary!=0 &&
				ocss_info[upper_car].hold_pos==255)
			{
				retval=MoveHold(retval,upper_car,ocss_info[upper_car].actual_pos);								
			}
		}

			//lower car 의 next target 이 upper car의 현재 위치보다 높거나 같으면 Hold
		if(ocss_info[lower_car].mov_dir==UP_DIR && bPit[lower_car]==false)
		{
			if(ocss_info[lower_car].f_door_state==DOORS_CLOSING &&
				ocss_info[lower_car].pre_dir==UP_DIR &&
				ocss_info[lower_car].next_up>=ocss_info[upper_car].actual_pos &&
				ocss_info[lower_car].allsummary!=0 &&
				ocss_info[lower_car].hold_pos==255)
			{
				retval=MoveHold(retval,lower_car,ocss_info[lower_car].actual_pos);								
			}
		}
		//upper car가 Call이 없고 Lower Car가 Call이 있고 Lower Car Pit Hold 상태이면 
		if(ocss_info[upper_car].allsummary==0 &&
			ocss_info[upper_car].f_door_state==DOORS_CLOSING &&
			ocss_info[upper_car].hold_pos==255 &&
			ocss_info[lower_car].allsummary!=0 &&
			ocss_info[lower_car].car_status==HOLD &&
			ocss_info[lower_car].actual_pos==0)
		{
			retval=MoveHold(retval,upper_car,bld_info.top_pos+1);
			ocss_info[lower_car].priority=MAIN_CAR;
			ocss_info[upper_car].priority=NONE;
		}
		//lower car에 Call이 없고 upper Car가 Call이 있고 upper Car overhead Hold 상태이면 
		if(ocss_info[lower_car].allsummary==0 &&
			ocss_info[lower_car].f_door_state==DOORS_CLOSING &&
			ocss_info[lower_car].hold_pos==255 &&
			ocss_info[upper_car].allsummary!=0 &&
			ocss_info[upper_car].car_status==HOLD &&
			ocss_info[upper_car].actual_pos==(bld_info.top_pos+1))
		{
			retval=MoveHold(retval,lower_car,0);
			ocss_info[upper_car].priority=MAIN_CAR;
			ocss_info[lower_car].priority=NONE;
		}
		if(bPit[lower_car] && ocss_info[lower_car].f_door_state==DOORS_CLOSING)
		{
			bPit[lower_car]=false;
			retval=MoveHold(retval,lower_car,0);
			ocss_info[lower_car].priority=NONE;
			ocss_info[upper_car].priority=MAIN_CAR;
		}
		if(bOverHead[upper_car] && ocss_info[upper_car].f_door_state==DOORS_CLOSING)
		{
			bOverHead[upper_car]=false;
			retval=MoveHold(retval,upper_car,bld_info.top_pos+1);
			ocss_info[lower_car].priority=MAIN_CAR;
			ocss_info[upper_car].priority=NONE;
		}
	}
	return(retval);
}

int HoistWayManager()
{
	int i;
	int retval=0;
	MOVE_PARAMETER move;
	if(bFirst)
	{
		bFirst=false;
		for(int i=0;i<n_car;i++)
		{
			memset(&move,0,sizeof(MOVE_PARAMETER));
			move.car=i;
			move.initiation_basis=0; // time base
			move.release_basis=0;
			move.t_initiation=mCurrentTime;
			move.t_release=((i%2)==0) ? mCurrentTime+1000 : mCurrentTime;
			move.destination=((i%2)==0) ? 0 : 1;
			car_status[i]=CCS;
			next_status[i]=((i%2)==0) ? HOLD : FREE;
			SendMove(move,MOVE_ADD);
			retval|=0x02;
		}
		return retval;
	}
	for(i=0;i<MAX_OCSS/2;i++)
	{
		int lower_car=i*2+1;
		int upper_car=i*2+2;
		int upper_high,upper_low,lower_high,lower_low;
		asslib_update_summary(lower_car);
		asslib_update_summary(upper_car);
		upper_high=ads_HighestPosition(upper_car,ocss_info[upper_car].NCF,ocss_info[upper_car].actual_pos,TRUE);
		upper_low=ads_LowestPosition(upper_car,ocss_info[upper_car].NCF,ocss_info[upper_car].actual_pos,TRUE);
		lower_high=ads_HighestPosition(lower_car,ocss_info[lower_car].NCF,ocss_info[lower_car].actual_pos,TRUE);
		lower_low=ads_LowestPosition(lower_car,ocss_info[lower_car].NCF,ocss_info[lower_car].actual_pos,TRUE);

	//upper 카가 위로 움직이기 시작했으면 Lower Car는 1층 이동 대기
		if((ocss_info[upper_car].target_pos>ocss_info[upper_car].actual_pos) &&
			ocss_info[upper_car].actual_pos<=upperZoneBottomFloor &&
			(ocss_info[lower_car].car_status==HOLD) &&
			ocss_info[lower_car].actual_pos==0)
		{
			retval=MoveFree(retval,lower_car,1);
		}

		//lower car 가 pit로 이동하기 시작했으면 Upper Car는 1층 이동 대기
		if((ocss_info[lower_car].target_pos<ocss_info[lower_car].actual_pos || (ocss_info[lower_car].actual_pos==0 && ocss_info[lower_car].car_status==HOLD)) &&
			(ocss_info[upper_car].oper==HOLD) &&
			ocss_info[upper_car].actual_pos!=1)
		{
			retval=MoveFree(retval,upper_car,1);
		}

/*
		int lower_car=i*2;
		int upper_car=i*2+1;
		int upper_high=0;
		int lower_high=0;
		int lower_high2=0;
		int upper_high2=0;
		allsummary[lower_car]=0;
		allsummary[upper_car]=0;
		n_CCallSum[lower_car]=0;
		n_CCallSum[upper_car]=0;
		n_UpStopSum[upper_car]=0;
		n_UpStopSum[lower_car]=0;
		for(int floor=0;floor<=bd_info.nUserFloors;floor++)
		{
			if(car_info[upper_car].n_CCall[floor]>0 && floor>upper_high)
			{
				upper_high=floor;
				allsummary[upper_car]=S_FCC;
				n_CCallSum[upper_car]+=car_info[upper_car].n_CCall[floor];
			}
			if(car_info[upper_car].n_UpStop[floor]>0 && floor>upper_high2)
			{
				upper_high2=floor;
				allsummary[upper_car]=S_FCC;
				n_UpStopSum[upper_car]+=car_info[upper_car].n_UpStop[floor];
			}
			if(car_info[lower_car].n_CCall[floor]>0 && floor>lower_high)
			{
				lower_high=floor;
				allsummary[lower_car]=S_FCC;
				n_CCallSum[lower_car]+=car_info[lower_car].n_CCall[floor];
			}
			if(car_info[lower_car].n_UpStop[floor]>0 && floor>lower_high2)
			{
				lower_high2=floor;
				allsummary[lower_car]=S_FCC;
				n_UpStopSum[lower_car]+=car_info[lower_car].n_UpStop[floor];
			}
		}
*/

		//upper car call 서비스를 다 했으면
		//upper 카가 윗쪽에 더이상 콜이 없으면
		if(ocss_info[upper_car].actual_pos==upper_high 
			&& ocss_info[upper_car].mov_dir==UP_DIR)
		{
			int pos=lower_high+1;
			if(pos==1)
				retval=MoveFree(retval,upper_car,pos);
			else
				retval=MoveHold(retval,upper_car,pos);
		}
		//lower car가 서비스를 다 끝내거나
		//upper car가 서비스를 다 끝낼 때까지 콜이 없으면 pit로 가서 기다림
		if((ocss_info[lower_car].actual_pos==lower_high
			&& ocss_info[lower_car].mov_dir==UP_DIR
			&& ocss_info[lower_car].actual_pos!=0) ||
			(ocss_info[lower_car].actual_pos==1
			&& ocss_info[lower_car].allsummary==0 
			&& ocss_info[upper_car].actual_pos==upper_high
			&& ocss_info[upper_car].mov_dir==UP_DIR))
		{
			if(ocss_info[lower_car].hold_pos!=0)
				retval=MoveHold(retval,lower_car,0);
		}
		//lower car에 할당 있고 Upper Car에 할당 없으면 Upper Car는 UpperZone 바닥 이동 대기
		
		if(ocss_info[upper_car].actual_pos==1 
			&& ocss_info[upper_car].allsummary==0
			&& ocss_info[lower_car].allsummary>0)
		{
			retval=MoveHold(retval,upper_car,lowerZoneTopFloor+1);
		}

		if(ocss_info[upper_car].actual_pos==1 
			&& ocss_info[upper_car].allsummary>0
			&& ocss_info[upper_car].oper==HOLD)
		{
			retval=MoveFree(retval,upper_car,ocss_info[upper_car].actual_pos);
		}
	}
	return(retval);
}



int HoistWayManager_Nor()
{
	int i;
	int retval=0;
	MOVE_PARAMETER move;
	if(bFirst)
	{
		bFirst=false;
		for(int car=1;car<MAX_OCSS;car++)
		{
			retval=MoveHold(retval,car,ocss_info[car].actual_pos);
		}
		return retval;
	}
	// Car Status 변화
	/*
	for(i=0;i<n_car;i++)
	{
		if(car_info[i].target!=car_info[i].floor)
			car_status[i]=CAS;
		if(car_status[i]==CAS && car_info[i].target==car_info[i].floor)
			car_status[i]=CSS;
		if(car_status[i]==CSS && next_status[i]==HOLD)
		{
			next_status[i]=FREE;
			car_status[i]=HOLD;
		}
	}
	*/
	for(i=0;i<MAX_OCSS/2;i++)
	{
		int lower_car=i*2+1;
		int upper_car=i*2+2;
		int upper_high,upper_low,lower_high,lower_low;
		asslib_update_summary(lower_car);
		asslib_update_summary(upper_car);
		upper_high=ads_HighestPosition(upper_car,ocss_info[upper_car].NCF,ocss_info[upper_car].actual_pos,TRUE);
		upper_low=ads_LowestPosition(upper_car,ocss_info[upper_car].NCF,ocss_info[upper_car].actual_pos,TRUE);
		lower_high=ads_HighestPosition(lower_car,ocss_info[lower_car].NCF,ocss_info[lower_car].actual_pos,TRUE);
		lower_low=ads_LowestPosition(lower_car,ocss_info[lower_car].NCF,ocss_info[lower_car].actual_pos,TRUE);
		/*
		if(ocss_info[upper_car].dnsummary!=0 && ocss_info[lower_car].allsummary==0)
		{
			retval=MoveHold(lower_car,upper_low-1);
		}
		*/
		if(ocss_info[lower_car].allsummary!=0 && 
			ocss_info[upper_car].allsummary==0 &&
			ocss_info[upper_car].actual_pos!=bld_info.top_pos+1
			)
		{
			retval=MoveHold(retval,upper_car,bld_info.top_pos+1);
		}
		//둘다 HOLD 일 경우 둘중 하나를 풀어준다.
		else if(ocss_info[upper_car].oper>=HIDE &&
			ocss_info[lower_car].oper>=HIDE &&
			(ocss_info[upper_car].actual_pos-ocss_info[lower_car].actual_pos)>1)
		{
			if(ocss_info[lower_car].allsummary!=0 &&
					ocss_info[upper_car].allsummary!=0)
			{
				if(ocss_info[lower_car].priority==MAIN_CAR)
					retval=MoveFree(retval,lower_car,ocss_info[lower_car].actual_pos);
				else if(ocss_info[upper_car].priority==MAIN_CAR)
					retval=MoveFree(retval,upper_car,ocss_info[upper_car].actual_pos);
				else
				{
					if(ocss_info[lower_car].hold_pos==0)
						retval=MoveFree(retval,upper_car,ocss_info[upper_car].actual_pos);
					else if(ocss_info[upper_car].hold_pos==bld_info.top_pos+1)
						retval=MoveFree(retval,lower_car,ocss_info[lower_car].actual_pos);
				}
			}
			else if(ocss_info[lower_car].allsummary!=0 &&
					ocss_info[upper_car].allsummary==0)
				retval=MoveFree(retval,lower_car,ocss_info[lower_car].actual_pos);
			else if(ocss_info[lower_car].allsummary==0 &&
					ocss_info[upper_car].allsummary!=0)
				retval=MoveFree(retval,upper_car,ocss_info[upper_car].actual_pos);
		}

		else if(ocss_info[lower_car].car_status==HOLD && 
			ocss_info[lower_car].allsummary!=0 &&
			(ocss_info[upper_car].actual_pos-ocss_info[lower_car].actual_pos)>1 &&
			ocss_info[upper_car].allsummary==0 &&
			ocss_info[upper_car].actual_pos!=bld_info.top_pos+1)
		{
			retval=MoveFree(retval,lower_car,ocss_info[lower_car].actual_pos);
			retval=MoveHold(retval,upper_car,lower_high+1);
		}
		
		//lower car is moving down
		if(ocss_info[lower_car].mov_dir==DOWN_DIR)
		{
			//lower car 가 바닥까지 왔으면 pit에 Hold 시킴
			if(ocss_info[lower_car].dnsummary==0 &&
				(ocss_info[lower_car].atsummary&S_DHC)==0)
			{
				if(ocss_info[lower_car].hold_pos!=0)
				{
					retval=MoveHold(retval,lower_car,0);
					ocss_info[lower_car].priority=NONE;
					ocss_info[upper_car].priority=MAIN_CAR;
				}
			}
			
			if(ocss_info[upper_car].car_status==HOLD &&
				ocss_info[upper_car].hold_pos==255 &&
				ocss_info[upper_car].allsummary!=0)
			{
				if(upper_low>ocss_info[lower_car].actual_pos)
				{
					retval=MoveFree(retval,upper_car,ocss_info[upper_car].actual_pos);
					ocss_info[upper_car].priority=SUB_CAR;
				}
				else if((ocss_info[upper_car].actual_pos-ocss_info[lower_car].actual_pos)>3)
				{
					retval=MoveHold(retval,upper_car,ocss_info[lower_car].actual_pos+2);
				}
			}
/*
			if(ocss_info[upper_car].car_status==HOLD &&
			ocss_info[upper_car].actual_pos==bld_info.top_pos+1 &&
			ocss_info[lower_car].actual_pos<=0 &&
			ocss_info[upper_car].allsummary!=0)
			{
				//retval=MoveHold(retval,upper_car,ocss_info[lower_car].actual_pos+2);
				retval=MoveFree(retval,upper_car,ocss_info[upper_car].actual_pos);
				ocss_info[upper_car].priority=SUB_CAR;
			}
*/
		}
		if(ocss_info[upper_car].mov_dir==UP_DIR)
		{
			if(ocss_info[upper_car].upsummary==0 &&
				(ocss_info[upper_car].atsummary&S_UHC)==0)
			{
				if(ocss_info[upper_car].hold_pos!=bld_info.top_pos+1)
				{
					retval=MoveHold(retval,upper_car,bld_info.top_pos+1);
					ocss_info[lower_car].priority=MAIN_CAR;
					ocss_info[upper_car].priority=NONE;
				}
			}


			/*
			if(ocss_info[lower_car].car_status==HOLD &&
			ocss_info[lower_car].actual_pos==0 &&
			ocss_info[upper_car].actual_pos>=bld_info.top_pos &&
			ocss_info[lower_car].allsummary!=0)
			{
				retval=MoveFree(retval,lower_car,ocss_info[lower_car].actual_pos);
				ocss_info[lower_car].priority=SUB_CAR;
			}
			*/
			
			if(ocss_info[lower_car].car_status==HOLD &&
				ocss_info[lower_car].hold_pos==255 &&
				ocss_info[lower_car].allsummary!=0)
			{
				if(lower_high<ocss_info[upper_car].actual_pos)
				{
					retval=MoveFree(retval,lower_car,ocss_info[lower_car].actual_pos);
					ocss_info[lower_car].priority=SUB_CAR;
				}
				else if((ocss_info[upper_car].actual_pos-ocss_info[lower_car].actual_pos)>3)
				{
					retval=MoveHold(retval,lower_car,ocss_info[upper_car].actual_pos-2);
				}
			}

			/*
			if(ocss_info[lower_car].mov_dir==UP_DIR &&
				lower_high>ocss_info[upper_car].actual_pos &&
				ocss_info[upper_car].priority==MAIN_CAR &&
				ocss_info[lower_car].priority==SUB_CAR &&
				ocss_info[lower_car].hold_pos==255)
					retval=MoveHold(retval,lower_car,ocss_info[upper_car].actual_pos-2);
			*/
		}
	}
	return(retval);
}


int Algorithm2D2Way(void)
{
	int rrt;
	int retval=0;
	int i,passID,count;
	int origin,dest;
	int best_car;
	retval=0;
	BYTE_T dir,pos,dst,riser;

	int nCurrentCalls=mPassengers.nCurrentCalls;
	static double previousMoveTime=-1;
	retval=HoistWayManager2D2Way();

	if (nCurrentCalls>=1)
	{	
		// 0~1/2 * nUserFloor ==> Car 1
		// 1/2~nUserFloor ==> Car 2
		// 0~nUserFloor ==> 2nd Hoistway Lower Car or 3rd Hoistway Upper Car depending 
		//			on the distance between hall call pos and car's NCF(No direction check)
		//			2nd Hoistway Upper Car to Overhead,3rd Hoistway Lower Car to Pit

		for (i=0;i<nCurrentCalls;i++)
		{
			pos=mPassengers.passengerCall[i].originFloor;
			dst=mPassengers.passengerCall[i].destFloor;
			if ((pos>=0) &&  (dst>=0) &&
				(pos<MAXFLOOR) && (dst<MAXFLOOR))
			{// valid passenger request
				passID=mPassengers.passengerCall[i].passengerID;
				if(passID==9)
				{
					int debug=1;
				}
				dir=(dst>pos) ? UP_DIR : DOWN_DIR;
				riser=0;
				best_car=assign_new_call ( dir, pos, dst, riser)-1; //index begins with 0
				// send assignment
				if (passID>=0 && passID<MAXPAX)
				{	
					count=mPassengers.passengerCall[i].nPassengers;
					SendAssignment(passID,pos,dst,DE_NORMAL_CALL,count,best_car,
						PAX_ASSIGN);
				}
				retval|=0x01; // NEW ASSIGN
			}
		}
	}
	return retval;
}

int Algorithm1D(void)
{
	int rrt;
	int retval=0;
	int i,passID,count;
	int origin,dest;
	int best_car;
	retval=0;
	BYTE_T dir,pos,dst,riser;

	int nCurrentCalls=mPassengers.nCurrentCalls;
	static double previousMoveTime=-1;

	if (nCurrentCalls>=1)
	{	
		// 0~1/2 * nUserFloor ==> Car 1
		// 1/2~nUserFloor ==> Car 2
		// 0~nUserFloor ==> 2nd Hoistway Lower Car or 3rd Hoistway Upper Car depending 
		//			on the distance between hall call pos and car's NCF(No direction check)
		//			2nd Hoistway Upper Car to Overhead,3rd Hoistway Lower Car to Pit

		for (i=0;i<nCurrentCalls;i++)
		{
			pos=mPassengers.passengerCall[i].originFloor;
			dst=mPassengers.passengerCall[i].destFloor;
			if ((pos>=0) &&  (dst>=0) &&
				(pos<MAXFLOOR) && (dst<MAXFLOOR))
			{// valid passenger request
				passID=mPassengers.passengerCall[i].passengerID;
				if(passID==82)
				{
					int debug=1;
				}
				dir=(dst>pos) ? UP_DIR : DOWN_DIR;
				riser=0;
				best_car=assign_new_call ( dir, pos, dst, riser)-1; //index begins with 0
				// send assignment
				if (passID>=0 && passID<MAXPAX)
				{	
					count=mPassengers.passengerCall[i].nPassengers;
					SendAssignment(passID,pos,dst,DE_NORMAL_CALL,count,best_car,
						PAX_ASSIGN);
				}
				retval|=0x01; // NEW ASSIGN
			}
		}
	}
	return retval;
}

int Algorithm0()
{
	int rrt;
	int retval=0;
	int i,passID,count;
	int origin,dest;
	int best_car;
	retval=0;
	BYTE_T dir,pos,dst,riser;

	int nCurrentCalls=mPassengers.nCurrentCalls;
	static double previousMoveTime=-1;
	retval=HoistWayManager();

	if (nCurrentCalls>=1)
	{	
		// 0~1/2 * nUserFloor ==> Car 1
		// 1/2~nUserFloor ==> Car 2
		// 0~nUserFloor ==> 2nd Hoistway Lower Car or 3rd Hoistway Upper Car depending 
		//			on the distance between hall call pos and car's NCF(No direction check)
		//			2nd Hoistway Upper Car to Overhead,3rd Hoistway Lower Car to Pit

		for (i=0;i<nCurrentCalls;i++)
		{
			pos=mPassengers.passengerCall[i].originFloor;
			dst=mPassengers.passengerCall[i].destFloor;
			if ((pos>=0) &&  (dst>=0) &&
				(pos<MAXFLOOR) && (dst<MAXFLOOR))
			{// valid passenger request
				passID=mPassengers.passengerCall[i].passengerID;
				if(passID==158)
				{
					int debug=1;
				}
				dir=(dst>pos) ? UP_DIR : DOWN_DIR;
				riser=0;
				best_car=assign_new_call ( dir, pos, dst, riser)-1; //index begins with 0
				// send assignment
				if (passID>=0 && passID<MAXPAX)
				{	
					count=mPassengers.passengerCall[i].nPassengers;
					SendAssignment(passID,pos,dst,DE_NORMAL_CALL,count,best_car,
						PAX_ASSIGN);
				}
				retval|=0x01; // NEW ASSIGN
			}
		}
	}
	return retval;
}

// random dispatching/no movements
int Algorithm1()
{
	int rrt;
	int retval=0;
	int i,passID,count;
	int origin,dest;
	int best_car;
	retval=0;
	BYTE_T dir,pos,dst,riser;

	int nCurrentCalls=mPassengers.nCurrentCalls;
	static double previousMoveTime=-1;
	retval=HoistWayManager_Nor();

	if (nCurrentCalls>=1)
	{	
		// 0~1/2 * nUserFloor ==> Car 1
		// 1/2~nUserFloor ==> Car 2
		// 0~nUserFloor ==> 2nd Hoistway Lower Car or 3rd Hoistway Upper Car depending 
		//			on the distance between hall call pos and car's NCF(No direction check)
		//			2nd Hoistway Upper Car to Overhead,3rd Hoistway Lower Car to Pit

		for (i=0;i<nCurrentCalls;i++)
		{
			pos=mPassengers.passengerCall[i].originFloor;
			dst=mPassengers.passengerCall[i].destFloor;
			if ((pos>=0) &&  (dst>=0) &&
				(pos<MAXFLOOR) && (dst<MAXFLOOR))
			{// valid passenger request
				passID=mPassengers.passengerCall[i].passengerID;
				if(passID==49)
				{
					int debug=1;
				}
				dir=(dst>pos) ? UP_DIR : DOWN_DIR;
				riser=0;
				best_car=assign_new_call ( dir, pos, dst, riser)-1; //index begins with 0
				// send assignment
				if (passID>=0 && passID<MAXPAX)
				{	
					count=mPassengers.passengerCall[i].nPassengers;
					SendAssignment(passID,pos,dst,DE_NORMAL_CALL,count,best_car,
						PAX_ASSIGN);
				}
				retval|=0x01; // NEW ASSIGN
			}
		}
	}
	return retval;
}

// sequential disatching/ sequential move(every 5 seconds),car1~car8,dest(random)
int Algorithm2()
{
	int retval;
	int i,passID,count;
	int origin,dest;
	retval=0;
	int nCurrentCalls=mPassengers.nCurrentCalls;
	static double prevMoveTime=0;
	static int moveCar=0;
	MOVE_PARAMETER	move;

	if (nCurrentCalls>=1)
	{	
		for (i=0;i<nCurrentCalls;i++)
		{
			origin=mPassengers.passengerCall[i].originFloor;
			dest=mPassengers.passengerCall[i].destFloor;
			if (origin>=0)
			{
				if (dest>=0)
				{
					if (origin<MAXFLOOR && origin<MAXFLOOR)
					{	// assignments
						if (assign_car==bd_info.nCars)
							assign_car=0;
						passID=mPassengers.passengerCall[i].passengerID;
						if (passID>=0 && passID<MAXPAX)
						{	count=mPassengers.passengerCall[i].nPassengers;
							SendAssignment(passID,origin,dest,DE_NORMAL_CALL,count,assign_car++,
								PAX_ASSIGN);
						}
					}
				}
			}
		}

		retval|=0x01; // NEW ASSIGN
	}
	// move specification
	if (mCurrentTime-prevMoveTime>5)
	{
		prevMoveTime=mCurrentTime;
		memset(&move,0,sizeof(MOVE_PARAMETER));
		move.car=moveCar++;
		if (moveCar>=n_car)
			moveCar=0;
		move.initiation_basis=0; // time base
		move.release_basis=0;
		move.t_initiation=mCurrentTime;
		move.t_release=mCurrentTime;
		move.destination=rand()%bd_info.nTotalFloors;
		SendMove(move,MOVE_ADD);
		retval|=0x02;
	}
	return retval;
}


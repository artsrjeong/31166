#include "stdafx.h"
#include "common.h"
#include "..\common\2DInterfaceData.h"
// call information



#define	EXTERN extern
#include "infbld.h"
#include "e2p.h"
#include "assign.h"
#include "aarule.h"
#include "asslib.h"
#include "infads.h"
#include "infocs.h"
#include "2DGroupDispatcher.h"
#include "infdes.h"
#undef	EXTERN

#define EXTERN   
#include "NPI.h"
#undef EXTERN


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


void CopyToOCSS(CarState state[],TIME_T ctCurrentTime)
{
    BYTE_T  carPos,dest_mask,dstPos,start,finish,callDir,start2,finish2;
	for(int car=CAR1;car<MAX_OCSS;car++)
	{
		if(car>bdInfo.nCars)
		{
			continue;
		}
		ocss_info[car].actual_pos=state[car-1].currentFloor;
		ocss_info[car].NCF=state[car-1].currentNCF;
		ocss_info[car].target_pos=state[car-1].target;
		ocss_info[car].old_mov_dir=ocss_info[car].mov_dir;
		switch(state[car-1].movingDirection)
		{
		case UP:
			ocss_info[car].mov_dir=UP_DIR;
			break;
		case DN:
			ocss_info[car].mov_dir=DOWN_DIR;
			break;
		default:
			ocss_info[car].mov_dir=NO_DIR;
			break;
		}
		switch(state[car-1].preDirection)
		{
		case UP:
			ocss_info[car].pre_dir=UP_DIR;
			break;
		case DN:
			ocss_info[car].pre_dir=DOWN_DIR;
			break;
		default:
			ocss_info[car].pre_dir=NO_DIR;
			break;
		}

//			0=idle, 1=door opening, 2=dwell, 3=door closing,
//#define     DOORS_CLOSED    0       /* door state = closed          */
//#define     DOORS_CLOSING   1       /* door state = closing         */
//#define     DOORS_OPENED    2       /* door state = opened          */
//#define     DOORS_OPENING   3       /* door state = opening         */
		int floor,orgPos,dstPos;
		for(orgPos=0;orgPos<JICA_FLOORS;orgPos++)
		{
			for(dstPos=0;dstPos<JICA_FLOORS;dstPos++)
			{
				asslib_DbdAssignCalls[car][orgPos][dstPos] = 0;
				asslib_DbdAssignCallDstCnts[car][orgPos][dstPos]=0;
			}
		}
		for(floor=0;floor<JICA_FLOORS;floor++)
		{
			ocss_info[car].allowed_mask[floor]=0xff;
			asslib_DbdExpectCalls[car][floor] = 0;
			asslib_DbdAssignCallDirCnts[car][UP_DIR][floor]=0;
			asslib_DbdAssignCallDirCnts[car][DOWN_DIR][floor]=0;
			asslib_DbdExpectCallCnts[car][UP_DIR][floor]=0;
			asslib_DbdExpectCallCnts[car][DOWN_DIR][floor]=0;
		}

		ocss_info[car].f_door_state=state[car-1].frontDoorState;
		int index;

/*
	asslib_DbdAssignCalls[car][orgPos][dstPos] |= callmask;
	asslib_DbdExpectCalls[car][dstPos] |= callmask;
	
	asslib_DbdAssignCallDirCnts[car][callDir][orgPos]++;
	asslib_DbdAssignCallDstCnts[car][orgPos][dstPos]++;
	
	asslib_DbdExpectCallCnts[car][callDir][dstPos]++;
*/

		for(floor=0;floor<JICA_FLOORS;floor++)
		{
			ocss_info[car].assigned_calls[floor]=0;
			index=AssignedCalls[car-1].waitingAtHead[floor];
			while(index!=-1)
			{
				orgPos=AssignedCalls[car-1].calls[index].origin;
				dstPos=AssignedCalls[car-1].calls[index].destination;
				asslib_DbdAssignCallDstCnts[car][orgPos][dstPos]++;

				if(dstPos>orgPos)
				{
					ocss_info[car].assigned_calls[floor]|=S_UHC;
					if(bld_info.fuhc[0][floor].reg_elapsed_car[car] <255)
					{
						bld_info.fuhc[0][floor].reg_elapsed_car[car]=	(BYTE_T)((ctCurrentTime-bld_info.fuhc[0][floor].reg_car[car])/1000);
					}
					asslib_DbdAssignCallDirCnts[car][UP_DIR][orgPos]++;
					asslib_DbdExpectCallCnts[car][UP_DIR][dstPos]++;
					asslib_DbdAssignCalls[car][orgPos][dstPos] |= DBD_SUCC;
					asslib_DbdExpectCalls[car][dstPos] |= DBD_SUCC;
				}
				else
				{
					ocss_info[car].assigned_calls[floor]|=S_DHC;
					if(bld_info.fdhc[0][floor].reg_elapsed_car[car] <255)
					{
						bld_info.fdhc[0][floor].reg_elapsed_car[car]=	(BYTE_T)((ctCurrentTime-bld_info.fdhc[0][floor].reg_car[car])/1000);
					}
					asslib_DbdAssignCallDirCnts[car][DOWN_DIR][orgPos]++;
					asslib_DbdExpectCallCnts[car][DOWN_DIR][dstPos]++;
					asslib_DbdAssignCalls[car][orgPos][dstPos] |= DBD_SDCC;
					asslib_DbdExpectCalls[car][dstPos] |= DBD_SDCC;
				}
				index=AssignedCalls[car-1].calls[index].next;
			}
		}
		asslib_DbdPeopleInCarCnts[car]=0;
		index=AssignedCalls[car-1].boardedIndex;
		while(index!=-1)
		{
			int dest=AssignedCalls[car-1].calls[index].destination;
			ocss_info[car].assigned_calls[dest]|=S_FCC;
			index=AssignedCalls[car-1].calls[index].next;
			asslib_DbdPeopleInCarCnts[car]++;
		}



/*		
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
		*/
	}
}

CHAR NPIDispatcher(Call *new_call,CarState state[],TIME_T ctCurrentTime)
{
	CHAR best_car;
	CopyToOCSS(state,ctCurrentTime); //Changed for 2DHoistway Conversion
	int pos=new_call->origin;
	int dst=new_call->destination;
	int dir=(dst>pos) ? UP_DIR : DOWN_DIR;
	int riser=0;
	best_car=assign_new_call ( dir, pos, dst, riser,ctCurrentTime)-1; //index begins with 0
	RecordDebug((double)ctCurrentTime);
	return best_car;	//
}

void NPIInitialize()
{
	int i;
	bool bPit=false;
	bool bOverHead=false;
	B_init(&bld_info);
	for(int hwyNo=0;hwyNo<bdInfo.nHoistways;hwyNo++)
	{
		if(hwyInfo[hwyNo].bPit)
			bPit=true;
		if(hwyInfo[hwyNo].bOverhead)
			bOverHead=true;
	}
	bld_info.bottom_pos=0;
	if(bPit)
		bld_info.bottom_pos++;
	int userFloor=bdInfo.nLandings;
	if(bPit)
		userFloor--;
	if(bOverHead)
		userFloor--;

	bld_info.top_pos=bld_info.bottom_pos+userFloor-1;
	
	e2p_init();
	assign_init();
	DES_Init();
	G_init(&grp_info);
	for (i=CAR1;i<=CAR8;i++)
	{
		O_init(&ocss_info[i]);
	}

	for(int car=CAR1;car<MAX_OCSS;car++)
	{
		ocss_info[car].carEffPassenger=carInfo[car-1].capacity;
		ocss_info[car].carMaxPassenger=carInfo[car-1].capacity;
		ocss_info[car].top_pos=bld_info.top_pos;
		ocss_info[car].lobby_pos=bdInfo.lobby1Floor;
		ocss_info[car].bottom_pos=bld_info.bottom_pos;
		if(car>bdInfo.nCars)
		{
			ocss_info[car].opmode=NAV;
			continue;
		}
		ocss_info[car].opmode=NOR;
	}
	//Initialization ? (e2p.c)
    E2P_Generate_Fuzzy_Set(e2p_light_med_x0,RRT_TYPE,(int *)&RRT_Light_Medium[0]);
    E2P_Generate_RRT_Low_Fuzzy_Set();
    E2P_Generate_RRT_High_Fuzzy_Set();
    E2P_Generate_Fuzzy_Set(e2p_short_x0, RRT_TYPE,  (int *)&RRT_Short[0]);
    E2P_Generate_Fuzzy_Set(e2p_improv_x0, RRT_TYPE, (int *)&RRT_Improv[0]);
    E2P_Generate_Fuzzy_Set(e2p_light_elderly_x0, PWT_TYPE, (int *)&PWT_Light_Elderly[0]);
    E2P_Generate_PWT_None_Fuzzy_Set();
    E2P_Generate_Fuzzy_Set(e2p_light_long_x0, PWT_TYPE, (int *)&PWT_Light_Long[0]);
}
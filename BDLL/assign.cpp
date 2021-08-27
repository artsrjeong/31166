#include "stdafx.h"
#include "common.h"
#define EXTERN extern
#include "infbld.h"
#include "infocs.h"
#include "queue.h"
#include "exec.h"
#include "infdes.h"
#include "autoadj.h"
#include "e2p.h"
#include "infads.h"
#include "crisprrt.h"
#include "crisppwt.h"
#include "asslib.h"
#include "rrt.h"
#include "pwt.h"
#include "aarule.h"
#include "trafdat.h"
#include "park.h"
#undef EXTERN

#define EXTERN
#include "assign.h"
#undef EXTERN


 /*********************************************************************
 ; Function Name: assign_init
 ;
 ; This procedure is responsible for initializing global variables
 ; related to ICA operation. (ICA - instantaneous Car Assignment)
 ;
 ; Parameters: none
 ;
 ;********************************************************************/
 void assign_init(void)
 {
	 INT_T i,j;
	 
	 /* I N I T I A L I Z E   P U B L I C   G L O B A L S */
	 /* ------------------------------------------------- */
	 ica_xmit_pos_index = 0;
	 ica_xmit_car_id = CAR1;
	 ica_central_group_enabled_flag = FALSE;
	 ica_reassnPosIdx = 0;
	 //SRO
	 ica_reassnGrpIdx = RISER_ONE;
	 assign_affected_passing = 0;
	 
#ifndef CAGS
	 
	 for (i=0; i<MAX_FLOOR_SEG; i++)
	 {
		 ica_rx_fuhc_allowed_mask[i] = 0;
		 ica_rx_fdhc_allowed_mask[i] = 0;
		 ica_rx_ruhc_allowed_mask[i] = 0;
		 ica_rx_rdhc_allowed_mask[i] = 0;
	 }
	 
	 
#endif
	 
	 assign_UnsureTableInit();
	 
	 for (i=0;i<2;i++)
		 for (j=0;j<JICA_FLOORS;j++)
			 car_call_pred[j][i] = FALSE;
		 
	 for (i=0;i<MAX_OCSS;i++)
		 for (j=0;j<JICA_FLOORS;j++)
			 dummy_assigned_calls[i][j]=0;
			 
	 /* I N I T I A L I Z E   L O C A L   G L O B A L S */
	 /* ----------------------------------------------- */
	 ica_lock_out_assignments_flag = FALSE;
	 ica_rat_pwt = 0;
	 ica_pwt_distance = 0;
	 temp_distance = 0;
}

/*********************************************************************
; Function Name: UnsureTableInit
;
; Initialize global variables related to unsure factor based on distance.
;
; Parameters: none
;
;********************************************************************/
void assign_UnsureTableInit(void)  
{
	INT_T distance,baseFactor;
	
	if ((*e2p_DistanceUnsureEnable==FALSE) || (*e2p_neural_enable!=0))
	{ 
		for (distance=0; distance<(2*JICA_FLOORS); distance++)
			assign_DistanceUnsure[distance] = 0;
	}
	else
	{
		for (distance=0; distance<32; distance++)
			assign_DistanceUnsure[distance] = 
			(distance*distance)/ (*e2p_CarToCallUnsureFactor);
		
		baseFactor = (distance*distance)/ (*e2p_CarToCallUnsureFactor);
		
		for (distance=32; distance<(2*JICA_FLOORS); distance++)
			assign_DistanceUnsure[distance] = baseFactor + (distance-32) ;
	}
}

void getHcAssnIdx_2D2Way(BYTE_T Call_Pos,BYTE_T Call_Dst,BYTE_T Call_Mask)
{
	FLAG_T     coinc_cc_flag;
	BYTE_T Value;
	INT_T car;
	int upper_car;
	int min_car=MAX_OCSS;
	int scan_start=(Call_Dst <= bld_info.lz_top_pos) ? 1 : bld_info.uz_bottom_pos;
	int scan_stop=(Call_Dst <= bld_info.lz_top_pos) ? bld_info.lz_top_pos : bld_info.top_pos;
	int min_callNo=bld_info.top_pos*2;
	int min_upStopNo=bld_info.top_pos*2;
	int min_rrt=RRT_MAX;
	int upperNoCall=0;
	int i;
	int less_rrt;
	Value=MAX_OCSS;
	
	for(car=1;car<MAX_OCSS;car++)
	{
		int upper_car=(car%2==0) ? car : car+1;
		int lower_car=(car%2==0) ? car-1 : car;
		int other_car=(car==upper_car) ? lower_car : upper_car;
		int upper_high,upper_low,lower_high,lower_low;
		asslib_update_summary(lower_car);
		asslib_update_summary(upper_car);
		upper_high=ads_HighestPosition(upper_car,ocss_info[upper_car].NCF,ocss_info[upper_car].actual_pos,TRUE);
		upper_low=ads_LowestPosition(upper_car,ocss_info[upper_car].NCF,ocss_info[upper_car].actual_pos,TRUE);
		lower_high=ads_HighestPosition(lower_car,ocss_info[lower_car].NCF,ocss_info[lower_car].actual_pos,TRUE);
		lower_low=ads_LowestPosition(lower_car,ocss_info[lower_car].NCF,ocss_info[lower_car].actual_pos,TRUE);

		hc_assn_idx[car]=0;

		// upperzone call to upper car and lower zone call to lower car
		if((Call_Mask & S_UHC)!=0)
		{
			hc_assn_idx[car]=coinc_dcc[car]*250+coinc_cc[car]*250-nUpStop[car]*50-pure_rrts[car]*2;
			if(ocss_info[car].pre_dir==UP_DIR)
			{
				for(int floor=ocss_info[car].lobby_pos;floor<ocss_info[car].top_pos;floor++)
				{
					if((ocss_info[car].assigned_calls[floor]&S_FCC) !=0)
						hc_assn_idx[car]-=50;
				}
			}
			//too many stop
			if(nUpStop[car]>bld_info.top_pos/2)
				hc_assn_idx[car]-=1000;
		}
		else
		{
			hc_assn_idx[car]=coinc_dcc[car]*250+coinc_cc[car]*250-nDnStop[car]*50-pure_rrts[car]*2;
			if(ocss_info[car].pre_dir==DOWN_DIR)
			{
				for(int floor=ocss_info[car].lobby_pos;floor<ocss_info[car].top_pos;floor++)
				{
					if((ocss_info[car].assigned_calls[floor]&S_FCC) !=0)
						hc_assn_idx[car]-=50;
				}
			}
			//too many stop
			if(nDnStop[car]>bld_info.top_pos/2)
				hc_assn_idx[car]-=1000;
		}
		if(ocss_info[car].target_pos>ocss_info[car].actual_pos && (Call_Mask&S_UHC)!=0 && 
			ocss_info[car].actual_pos>=Call_Pos)
		{
			hc_assn_idx[car]-=200;
		}
		if(ocss_info[car].target_pos<ocss_info[car].actual_pos && (Call_Mask&S_DHC)!=0 && 
			ocss_info[car].actual_pos<=Call_Pos)
		{
			hc_assn_idx[car]-=200;
		}

		if((Call_Mask & S_UHC)!=0)
		{
			if(nPax_up[car]>15)
				hc_assn_idx[car]-=3000;
		}
		if((Call_Mask & S_DHC)!=0)
		{
			if(nPax_dn[car]>15)
				hc_assn_idx[car]-=3000;
		}
		
		// if rrt is too small reduce hc index
		less_rrt=0;
		for(i=1;i<MAX_OCSS;i++)
		{
			if(pure_rrts[car]>pure_rrts[i])
				less_rrt++;
		}
		if(less_rrt>bld_info.nCars/2)
			hc_assn_idx[car]-=2000;
	}
}

void getHcAssnIdx_1D(BYTE_T Call_Pos,BYTE_T Call_Dst,BYTE_T Call_Mask)
{
	FLAG_T     coinc_cc_flag;
	BYTE_T Value;
	INT_T car;
	int upper_car;
	int min_car=MAX_OCSS;
	int scan_start=(Call_Dst <= bld_info.lz_top_pos) ? 1 : bld_info.uz_bottom_pos;
	int scan_stop=(Call_Dst <= bld_info.lz_top_pos) ? bld_info.lz_top_pos : bld_info.top_pos;
	int min_callNo=bld_info.top_pos*2;
	int min_upStopNo=bld_info.top_pos*2;
	int min_rrt=RRT_MAX;
	int upperNoCall=0;
	
	Value=MAX_OCSS;
	
	for(car=1;car<MAX_OCSS;car++)
	{
		hc_assn_idx[car]=coinc_dcc[car]*50+coinc_cc[car]*50-DbdExpect_cnt[car]*50-assigned_cnt[car]*50-pure_rrts[car]*2;
		if(DbdExpect_cnt[car]>=2)
			hc_assn_idx[car]-=100;
		// prefer car which has less assigned call
		for(int floor=ocss_info[car].lobby_pos;floor<ocss_info[car].top_pos;floor++)
		{
			if((ocss_info[car].assigned_calls[floor]&S_FCC) !=0)
				hc_assn_idx[car]-=50;
		}
		if(pure_rrts[car]<3 && (DbdExpect_cnt[car]+assigned_cnt[car])<6)
		{
			hc_assn_idx[car]+=400;
		}
		if(pure_pwts[car]>150)
		{
			hc_assn_idx[car]-=2000;
		}
	}
}

void getHcAssnIdx(BYTE_T Call_Pos,BYTE_T Call_Dst,BYTE_T Call_Mask)
{
	FLAG_T     coinc_cc_flag;
	BYTE_T Value;
	INT_T car;
	int upper_car;
	int min_car=MAX_OCSS;
	int scan_start=(Call_Dst <= bld_info.lz_top_pos) ? 1 : bld_info.uz_bottom_pos;
	int scan_stop=(Call_Dst <= bld_info.lz_top_pos) ? bld_info.lz_top_pos : bld_info.top_pos;
	int min_callNo=bld_info.top_pos*2;
	int min_upStopNo=bld_info.top_pos*2;
	int min_rrt=RRT_MAX;
	int upperNoCall=0;
	
	Value=MAX_OCSS;
	
	for(car=1;car<MAX_OCSS;car++)
	{
		int upper_car=(car%2==0) ? car : car+1;
		int lower_car=(car%2==0) ? car-1 : car;
		int upper_high,upper_low,lower_high,lower_low;
		asslib_update_summary(lower_car);
		asslib_update_summary(upper_car);
		upper_high=ads_HighestPosition(upper_car,ocss_info[upper_car].NCF,ocss_info[upper_car].actual_pos,TRUE);
		upper_low=ads_LowestPosition(upper_car,ocss_info[upper_car].NCF,ocss_info[upper_car].actual_pos,TRUE);
		lower_high=ads_HighestPosition(lower_car,ocss_info[lower_car].NCF,ocss_info[lower_car].actual_pos,TRUE);
		lower_low=ads_LowestPosition(lower_car,ocss_info[lower_car].NCF,ocss_info[lower_car].actual_pos,TRUE);

		hc_assn_idx[car]=0;
		// upperzone call to upper car and lower zone call to lower car
		if(Call_Dst<=bld_info.lz_top_pos && car%2==0)
		{
			hc_assn_idx[car]=-10000;
			continue;
		}
		if(Call_Dst>bld_info.lz_top_pos && car%2==1)
		{
			hc_assn_idx[car]=-10000;
			continue;
		}
		hc_assn_idx[car]=coinc_dcc[car]*1250-DbdExpect_cnt[car]*50-pure_rrts[car];
		if(DbdExpect_cnt[car]>=2)
			hc_assn_idx[car]-=100;
		for(int floor=ocss_info[car].lobby_pos;floor<ocss_info[car].top_pos;floor++)
		{
			if((ocss_info[car].assigned_calls[floor]&S_FCC) !=0)
				hc_assn_idx[car]-=50;
		}
		if(ocss_info[car].target_pos>ocss_info[car].actual_pos && (Call_Mask&S_UHC)!=0 && 
			ocss_info[car].actual_pos>=Call_Pos)
		{
			hc_assn_idx[car]-=200;
		}
		//lobby 에 있으면 assigned count 도 hc에 영향
		if(ocss_info[car].actual_pos<=ocss_info[car].lobby_pos &&
			ocss_info[car].target_pos<=ocss_info[car].actual_pos)
		{
			hc_assn_idx[car]-=assigned_cnt[car]*50;
		}
		if(Call_Dst<=bld_info.lz_top_pos)
		{
			//match call count between upper car and lower car
			if((DbdExpect_cnt[upper_car]-DbdExpect_cnt[lower_car])>=0)
			{
				hc_assn_idx[car]+=(DbdExpect_cnt[upper_car]-DbdExpect_cnt[lower_car])*50;
			}
			//if upper car finished its service don't assign to lower car
			if(ocss_info[upper_car].actual_pos==upper_high
				&& ocss_info[upper_car].mov_dir==UP_DIR)
			{
				hc_assn_idx[car]-=2000;
			}
			//if upper car move down and lower car position is 1, don't assign a call to lower car
			if(ocss_info[lower_car].actual_pos==ocss_info[lower_car].lobby_pos
				&& ocss_info[upper_car].mov_dir==DOWN_DIR)
			{
				hc_assn_idx[car]-=2000;
			}
		}
		if(pure_rrts[car]<3 && (DbdExpect_cnt[car]+assigned_cnt[car])<2)
		{
			hc_assn_idx[car]+=300;
		}
		if(ocss_info[car].mov_dir==DOWN_DIR && DbdExpect_cnt[car]<2)
		{
			hc_assn_idx[car]+=100;
		}
		if(ocss_info[car].actual_pos==ocss_info[car].lobby_pos &&
			ocss_info[car].target_pos<=ocss_info[car].actual_pos &&
			(ocss_info[car].assigned_calls[Call_Dst] & S_FCC)!=0)
		{
			hc_assn_idx[car]+=1250;
		}
	}
}

int assign_new_call (BYTE_T dir,BYTE_T pos,BYTE_T dst,BYTE_T riser)
{
	BYTE_T     msg_temp[15];
	INT_T      assn_incr;
	INT_T      max_assn_idx, assn_idx;
	INT_T      eld_pwt, eld_pwt_none, eld_pwt_very;
	INT_T      rrt, best_rrt ,rrt_low, rrt_med, rrt_high; 
	INT_T      rrt_idx; 
	INT_T      eld_idx_none, eld_idx_very;
	INT_T      eld_idx_incr;
	
	
	BYTE_T     best_car;
	
	BYTE_T     number_affected_calls;
	BYTE_T     i, car, flag;
	BYTE_T     queue_count,dbdQueueCount;
	BYTE_T     call_pos, call_mask, temp_mask, dest_mask, call_type, dest_cc_mask;
	FLAG_T     dest_cc_serviceable;
	BYTE_T     call_dst;
	BYTE_T     distance,distance2,distance3;
	TASK_ID_T  TaskID;
	FLAG_T     coinc_cc_flag,coinc_dcc_flag;
	BYTE_T     coinc_type;
	FLAG_T     ignore_coinc_flag;
	FLAG_T     immediate_car_flag,parkIdleFlag;
	BYTE_T     pre_dir;
	
	FLAG_T     CrowdFlag;
	
	//   FLOAT_T    float1, float2, float3, float4, float5;
	LONG_T     long1, long2, long3, long4, long5;
	//   FLOAT_T    nnrrt;
	INT_T      nn_rrts[MAX_OCSS];
	FLAG_T     coinc_assign;
	BYTE_T     grp_no;
	FLAG_T     grp_available;

	INT_T      p_rrt,p_rrts[MAX_OCSS];
	
	INT_T      pure2_rrts[MAX_OCSS];
	
	BYTE_T     best_car2;
	FLAG_T     chn_check_flag = FALSE,affectPassFlag;
	BYTE_T     tmp_highest_pos,tmp_lowest_pos;
	BYTE_T     doublePenalty,carBit;
	BYTE_T     waitLevel,callDir; 
	BYTE_T     dpcCallFlag,dpcAssignCar,dpcTmp1,dpcTmp2;
	BYTE_T     assignedCar,immediateFlag,stoppedCar,passIdLow,passIdHigh,passTmp[2];
	FLAG_T     dbdCallFlag;
	WORD_T     passId;
	BYTE_T     affectPwtByDst,affectPwtBySrc,affectPwtByDstCar[MAX_OCSS],affectPwtBySrcCar[MAX_OCSS];
	FLAG_T     sendLocalAssignFlag;
	FLAG_T     invChnBestCar;
	BYTE_T     spareCap;

	dest_cc_mask = S_FCC;
	call_pos=pos;
	call_dst=dst;
	if (dir == UP_DIR) call_mask = S_FUHC;
	else if (dir == DOWN_DIR) call_mask = S_FDHC;
	grp_no=0;
	/* calculate rrt for every car */
	for( car=CAR1;car<MAX_OCSS;car++)
	{
		ica_rrts[car] = RSR_MAX_PLUS_1;
		hc_assn_idx[car] = 0;
		pure_rrts[car] = RSR_MAX_PLUS_1;
		pure2_rrts[car] = RSR_MAX_PLUS_1;
		grp_available=TRUE;
		coinc_type=0;

		coinc_cc[car] = FALSE;
		coinc_type_car[car] = 0;
		coinc_dcc[car] = FALSE;
		coinc_dcc_flag = FALSE;

		if (call_dst == 255)   
			dest_cc_serviceable = TRUE;
		else if ((ocss_info[car].allowed_mask[call_dst] & dest_cc_mask) != 0)
			dest_cc_serviceable = TRUE;
		else
			dest_cc_serviceable = FALSE;

		/* I S   C A R   E L I G I B L E   F O R   C A L L ? */
		/* ------------------------------------------------- */
		/* If the car is capable of answering the call, and  */
		/* it is in an available opmode, calculate RRT.      */
		/* ------------------------------------------------- */
		if ( ( (ocss_info[car].allowed_mask[call_pos] & call_mask) != 0) &&
				(asslib_car_available(car)!=asslib_NotAvailable) &&
				(dest_cc_serviceable == TRUE)  &&
				(grp_available == TRUE)) 
		{
			/* C A L C U L A T E   C A R   P R E _ D I R */
			/* ----------------------------------------- */
			pre_dir = asslib_calculate_pre_dir(car);
			rrt = rrt_determine_rrt(car, call_pos, call_dst, call_mask, pre_dir, 
				&coinc_cc_flag, &coinc_type, &distance, &distance2, &spareCap);

         assign_RrtCap[car] = spareCap;

         //DBD-honma spcap 22Mar04
         if ((*e2p_DbdAssignEnable != 0) && ((*e2p_DbdAssignType & AssignTypePassCnts) != 0))
         if (assign_RrtCap[car] >= ocss_info[car].carEffPassenger) 
          {
             rrt += RRT_MAX;
             asslib_AssignRrtMaxCnt++;
			 if (*e2p_debug_enable != 0)
			 {
			 }
          }

			pure_rrts[car] = rrt;

			rrt += (distance2 * (*e2p_UnsureDistance));
			ica_rrts[car] = rrt + assign_DistanceUnsure[distance];
			
			pure2_rrts[car] = pure_rrts[car] + assign_DistanceUnsure[distance];

			//update 99/2/18
			//If there are MIT cars,LobbyUpHall call should not set coinc and distance2.
			// this condition is under testing!!!
			// if ((trafdat_MITCarsNeeded != 0) || (ocss_mit_status_flag != 0))
			// DBD-honma 12Apr04

			if (((*e2p_DbdAssignEnable == 0) && (trafdat_MITCarsNeeded != 0)) || (ocss_mit_status_flag != 0))
			{
				if ((call_pos == bld_info.ocss_lobby_pos) &&
					((call_mask & S_FUHC) != 0))
				{
					ica_rrts[car] = pure_rrts[car] + assign_DistanceUnsure[distance];
					coinc_cc_flag = FALSE;
				}
			}

			//update 99/2/10
			// if the car is parking to this call pos.
			// Parking Car should have some priority than other cars.
			if ((ocss_info[car].opmode == PRK) && (ocss_info[car].target_pos == call_pos))
			{
				//DBD-honma Wrong !!!!coinc_cc_flag = TRUE;
				//coinc_type |= CC_HCwithCC;
			}//if (ocss_info[car].opmode == PRK)
			
			
			/* CHN0039 */
			if ((ocss_chn_status_flag == TRUE) &&
				(call_pos == bld_info.ocss_lobby_pos) &&
				((call_mask & S_FUHC) != 0))
			{
				ica_rrts[car] = pure_rrts[car];
			}
			

			/* C A L C U L A T E   L O N G E S T   P W T */
			/* ----------------------------------------- */
			/* Calculate the longest PWT for any calls   */
			/* affected by the new hall call.            */
			/* ----------------------------------------- */
			eld_pwt = pwt_determine_pwt(car, 
				call_pos, 
				call_dst, //DBD
				ocss_info[car].pre_dir, 
				call_mask,
				rrt,
				distance,&distance3,&affectPwtByDst,
				&spareCap);

			assign_PwtCap[car] = spareCap;
			//DBD-honma spcap 22Mar04
			if ((*e2p_DbdAssignEnable != 0) && ((*e2p_DbdAssignType & AssignTypePassCnts) != 0))
				if (assign_PwtCap[car] >= ocss_info[car].carEffPassenger)
				{
					//02Nov2004 DBD-honma
					pure_rrts[car] = RRT_MAX;
					ica_rrts[car] = RRT_MAX;
					eld_pwt += RRT_MAX; 
					
					asslib_AssignPwtMaxCnt++;
					if (*e2p_debug_enable != 0)
					{
					}
				}
			if (eld_pwt > ica_rrts[car])
			{
				pure_pwts[car] = eld_pwt - ((*e2p_UnsureDistance)*distance2);
				pure_pwts[car] -= assign_DistanceUnsure[distance3 + distance];
#ifdef CAGS
				if ((int)pure_pwts[car] < (int)0)
				{
					/* debug */
					if ((usage == ENGINEER_USE) && (CRT_mode == 1))
					{
						sprintf(debug_msg1,"minas-CAR[%d]rrt%3d:pwt%3d %6.2f\n",
							car,ica_rrts[car],eld_pwt,tm_sys);
						deb_msg(car+CAR4,debug_msg1);
					}
				}
				
				/* testsss */
				total_rrtspwts += pure_pwts[car];
#endif //ifdef CAGS
			} 
			else 
				pure_pwts[car] = pure_rrts[car];
			
			/* E X I T   I F   C A R   H A S   R R T   O F   0 */
			/* ----------------------------------------------- */
			/* If a car has an RRT of 0, it is without a doubt */
			/* the best car to serve the call.  Set assign     */
			/* index of all other cars to 0, set assign index  */
			/* of car with 0 RRT to 100, and exit from loop.   */
			/* ----------------------------------------------- */
			//DBD 20Feb04  ,It is only available for MultipleAssign with ImmediateCase
			//if ((*e2p_DbdAssignEnable == 0) || ((*e2p_DbdAssignType & AssignTypeImmediate) != 0))
			if ((*e2p_DbdAssignEnable == 0) || ((*e2p_DbdAssignType & AssignTypeImmediate) == 0))
				if (rrt == 0)
				{
					
					if ((ocss_info[car].opmode == MIT) && ((call_mask&S_FUHC)!=0) &&
						((call_pos==bld_info.ocss_lobby_pos)||
						((call_pos==bld_info.second_lobby_pos) && (ocss_2nd_lobby_flag == TRUE)))&&
						(ocss_MitPriority[car] != Selected)) 
					{
						// Do nothing
					}
					//TestInProgress           else if (pure_pwts[car] < 80)
					else
					{
						immediate_car_flag = TRUE;
						/*
						if (*e2p_DistanceUnsureEnable == 1)
						{
						hc_assn_idx[car] = 250;
						}
						else 
						hc_assn_idx[car] = 100;
						*/
						//DBD-honma 04Feb2004, add FuzzyOffSet
						if (*e2p_DistanceUnsureEnable == 1)
						{
							hc_assn_idx[car] = 500;
						}
						else 
							hc_assn_idx[car] = 350;
						
					}
					//DBD
					/* TestInProgress
					else
					ica_rrts[car] = pure_pwts[car];
					*/
				}//if (rrt == 0)

				if (affectPwtByDst != 0)
					affectPwtByDst = affectPwtByDst;
				
			//DBD  20Feb04
			// Source Coincident Case
			if (((ocss_info[car].assigned_calls[call_pos] & call_mask) != 0) ||
				(((ocss_info[car].stop_type & call_mask) != 0) && (rrt < 3)))
			{
				coinc_type |= CC_HCwithHC;
				
				if (eld_pwt > ica_rrts[car])
				{
					affectPwtBySrc = 1;
				}
			}
			
			//DBD  29Feb04
			// Source Coincident Case with CC
			if (((ocss_info[car].stop_type & (S_CC+E_CC)) != 0) && (rrt == 0))
			{
				coinc_type |= CC_HCwithCC;
				
				if (eld_pwt > ica_rrts[car])
				{
					affectPwtBySrc = 1;
				}
			}
			
			//DBD 29Feb04 
			if ((ocss_info[car].allsummary == 0) &&
				(ica_rrts[car] == 1) &&(ocss_info[car].actual_pos==call_pos))
			{
				coinc_type |= CC_HCwithCC;
			}
			
			//DBD  02Feb04
			// HallCall Coincident Case
			if ((coinc_type & (CC_HCwithCC + CC_HCwithDCC + CC_HCwithHC)) != 0)
			{
				
				coinc_cc_flag = TRUE;
				/*
				if (pure_pwts[car] > pure_rrts[car]) 
				{
				if (pure_pwts[car] < 60)
				coinc_cc_flag = TRUE;
				else
				coinc_cc_flag = FALSE;
				}
				*/
			}
			/* TestInProgress */
			// DestinationCarCall Coincident Case
			if ((coinc_type & (CC_DCCwithCC + CC_DCCwithDCC + CC_DCCwithHC)) != 0)
			{
				coinc_dcc_flag = TRUE;
			}
			
			coinc_cc[car] = coinc_cc_flag;
			coinc_dcc[car] = coinc_dcc_flag;
			coinc_type_car[car] = coinc_type;
			affectPwtBySrcCar[car] = affectPwtBySrc;
			affectPwtByDstCar[car] = affectPwtByDst;

			//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
			// test010 add penalty for car which has lobby car call 
			
			if ( 
				((ocss_info[car].assigned_calls[bld_info.ocss_lobby_pos]&S_FCC)!=0)
				&& ((ocss_info[car].assigned_calls[bld_info.ocss_lobby_pos]&S_FUHC)==0)
				&& ((call_mask & S_FDHC)!=0)
				)
			{
				if ((ocss_info[car].pre_dir == DOWN_DIR) &&
					(ocss_info[car].NCF < call_pos))
					ica_rrts[car] += 15;
			}
			//dojima support SecondLobby
			else if ( 
				((ocss_info[car].assigned_calls[bld_info.second_lobby_pos]&S_FCC)!=0)
				&& ((ocss_info[car].assigned_calls[bld_info.second_lobby_pos]&S_FUHC)==0)
				&& ((call_mask & S_FDHC)!=0)
				)
			{
				if ((ocss_info[car].pre_dir == DOWN_DIR) &&
					(ocss_info[car].NCF < call_pos))
					ica_rrts[car] += 15;
			}

			//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
			/* check for MIT fisrt ,second car restrain bihind hall call */
			if ((ocss_info[car].opmode == MIT) &&
				! ((call_pos == ocss_info[car].actual_pos) &&
				((call_mask & S_FUHC)!=0)))
			{
				
				if (ocss_MitPriority[car]==Selected)
				{
					ica_rrts[car] += 20;          
				}
				else if (ocss_MitPriority[car]==NextSelected)
				{
					ica_rrts[car] += 40;          
				}
				
				if ((ocss_MitPriority[car]==Selected) || (ocss_MitPriority[car]==NextSelected))
					if (ocss_info[car].f_door_state != DOORS_CLOSED)
					{
						if ( ((call_pos == ocss_info[car].actual_pos) && (call_mask & S_FDHC)) ||
							(call_pos < ocss_info[car].actual_pos))
							ica_rrts[car] += 100;
					}
			}
			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			
			//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
			/*---------- check mit priority for Lobby Position Up Hall Call ----------*/
			//DBD-honma 12Apr04 if ((ocss_info[car].opmode == MIT) || (trafdat_MITCarsNeeded != 0)) 
			if ((ocss_info[car].opmode == MIT) || ((*e2p_DbdAssignEnable == 0)&&(trafdat_MITCarsNeeded != 0))) 
			{
				if ((call_pos==bld_info.ocss_lobby_pos)&&((call_mask&S_FUHC)!=0))
				{
					if (
						(ocss_info[car].opmode == MIT)&&
						(ocss_info[car].NCF==bld_info.ocss_lobby_pos)&&
						((pre_dir==UP_DIR) || (pre_dir==NO_DIR))
						)
					{
						if ((ocss_MitPriority[car] >= NextSelected) && (ocss_MitPriority[car] <= 8))
							ica_rrts[car] += ocss_MitPriority[car];
					}
					else if (ocss_info[car].opmode != MIT) //Variable MIT
					{
						ica_rrts[car] += 15;
					}
				}
				/* for multi lobby  */ 
				else if (ocss_2nd_lobby_flag == TRUE)
				{
					if ((call_pos==bld_info.second_lobby_pos)&&((call_mask&S_FUHC)!=0))
					{
						if ((ocss_MitPriority[car]!=Selected)&&
							(ocss_info[car].actual_pos==bld_info.second_lobby_pos)&&
							((pre_dir==UP_DIR) || (pre_dir==NO_DIR))
							)
						{
							ica_rrts[car] += 5;
						}
					}
					
				}
				
				
			} /* if (ocss_info[car].opmode == MIT) */
			
			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//dojima, When the car has one Lobby UpHallCall,then should not assign 
// another LobbyUpHallCall(second lby) for protection of LNS.
/*---------- check mit priority for Lobby Position Up Hall Call ----------*/
/* Should be check Coinc_Flag  update 99/2/15 */
    //DBD-honma 12Apr04
    if (((ocss_info[car].opmode == MIT) || ((*e2p_DbdAssignEnable==0)&&(trafdat_MITCarsNeeded != 0)) ||
        (*e2p_MultiLobbyHallPenalty!=0) ) && (coinc_cc_flag == FALSE))
    {
       if (((call_pos==bld_info.ocss_lobby_pos)&&((call_mask&S_FUHC)!=0)) &&
          //Add new condition,1999-OCT-05 H.Honma
          (bld_info.second_lobby_pos <= bld_info.top_pos))
       {
         if (
           ((ocss_info[car].assigned_calls[bld_info.second_lobby_pos] & (S_FUHC+E_FUHC))!=0)
           
           ||
           ( ((ocss_info[car].stop_type & (S_FUHC+E_FUHC))!=0)&&(ocss_info[car].stop_pos == bld_info.second_lobby_pos)
            &&(bld_info.ocss_lobby_pos > bld_info.second_lobby_pos) && 
              (coinc_cc_flag == FALSE) )
           )
           {

             ica_rrts[car] += *e2p_MultiLobbyHallPenalty;

           }
       }
/* for second lobby  */ 
      else if ((call_pos==bld_info.second_lobby_pos)&&((call_mask&S_FUHC)!=0))
	  {
		  if (
			  ((ocss_info[car].assigned_calls[bld_info.ocss_lobby_pos] & (S_FUHC+E_FUHC))!=0)
			  ||
			  ( ((ocss_info[car].stop_type & (S_FUHC+E_FUHC))!=0)&&(ocss_info[car].stop_pos == bld_info.ocss_lobby_pos)
			  &&(bld_info.second_lobby_pos > bld_info.ocss_lobby_pos) && 
              (coinc_cc_flag == FALSE) )
			  )
		  {
			  ica_rrts[car] += *e2p_MultiLobbyHallPenalty;
		  }
	  }//else secondLobby UpHallCall
    } /* if (ocss_info[car].opmode == MIT) */
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//dojima
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	/*---------- MIT car should not assigned Interfloor HallCall ----------*/
    //DBD-honma 12Apr04
    if ((ocss_info[car].opmode == MIT) && ((*e2p_DbdAssignEnable==0)&&(trafdat_MITCarsNeeded!=0)) 
		&& (*e2p_VmitEnable != 0))
    {
		if ((trafdat_MITCarsNeeded < asslib_count_normal_cars()) &&
			(ocss_MitPriority[car] == NoDemand))
		{
			if (!
				(((call_mask & S_FUHC)!=0) && 
				((call_pos == bld_info.ocss_lobby_pos)||(call_pos == bld_info.second_lobby_pos)))
				)
				ica_rrts[car] += 20;
		}
    } /* if (ocss_info[car].opmode == MIT) */
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// add *e2p_double_penalty for prevent double assignment 
	doublePenalty = *e2p_double_penalty;
	
	if (*e2p_EmsUserPreferenceEnable != 0)
	{
		if (infbld_EmsDoubleAssignSignal == 0)
			doublePenalty = 255;
		else
			doublePenalty = 0;
	}
	
	if (doublePenalty!=0)
	{
        if ((call_mask & S_FUHC) !=0) temp_mask = S_FDHC;
        else temp_mask = S_FUHC;
		
        if ((ocss_info[car].assigned_calls[call_pos] & temp_mask)!=0)
		{
			ica_rrts[car] += doublePenalty*2;
		}
	}
	
	//--------------------------------------------------------------------------
	// Wheel Chair Car    Penalty     Routine    99-May-20  H.Honma
	//--------------------------------------------------------------------------
	if (*e2p_WcsCar == car)
	{
		if ((coinc_cc[car] == TRUE) && (ica_rrts[car] < 15))
			;
		else
		{
			ica_rrts[car] += *e2p_WcsPenalty;
		}
	}//if (*e2p_WcsCar == car)
	
#ifdef CAGS
	if ((simulation_type == SINGLE) && (tm_sys > 2428.0) &&(tm_sys < 2430.0))
		fprintf(fdeb2,
		"\np[%2d]d[%d]car[%d]ccc[%3d]rrt[%3d]pwt[%3d]fuz[%3d] [%6.2f]",
		pos,dir,car,coinc_cc[car],ica_rrts[car],ica_pwts[car],hc_assn_idx[car],tm_sys);
#endif //CAGS
	
	/* If car is in channelling, determine effect on RRT  */
	/*--------------------------------------------------------------*/
	/* jsr commented Channelling for the moment
	if (ocss_info[car].opmode == CHN)
	{
		if (chn_check_flag == FALSE)
		{
			assign_check_chn (call_pos,call_mask);
			chn_check_flag = TRUE;
		}
		ica_rrts[car] = assign_consider_sector(car,call_pos,pre_dir,call_mask,ica_rrts[car]);
	}
	// this section is moved for correct debug message ,1997/11/27 
	/* If elderly PWT is less than RRT, set it equal to RRT. */
	/* ----------------------------------------------------- */
	if (eld_pwt < ica_rrts[car])
	{
		eld_pwt = ica_rrts[car];
	}
	ica_pwts[car] = eld_pwt;	 
	
	/* D E B U G   M E S S A G E */
	/* ------------------------- */
	if (*e2p_debug_enable != 0)
	{
	} /* End of debug message */
	

			//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			
			/* If RRT is less than maximum, set low, medium and high values */
			/* from fuzzy sets.                                             */
			/* ------------------------------------------------------------ */
			if (ica_rrts[car] <= RRT_X_UPPER)
			{
			/* jsr block because jsr didn't find routine for setting ADJ_AdjustedSet 
			#ifndef CAGS
			rrt_low = ADJ_AdjustedSets.rrt.low[ica_rrts[car]][autoadj_current_array];
			rrt_med = ADJ_AdjustedSets.rrt.medium[ica_rrts[car]][autoadj_current_array];
			rrt_high = ADJ_AdjustedSets.rrt.high[ica_rrts[car]][autoadj_current_array];
			#endif
				*/
				rrt_low = RRT_Light_Low[ica_rrts[car]];
				rrt_med = RRT_Light_Medium[ica_rrts[car]];
				rrt_high = RRT_Light_High[ica_rrts[car]];
				
			}
			/* Otherwise, it's beyond the bounds of the RRT sets, so set */
			/* membership of low and medium to 0, and membership of high */
			/* to 100.                                                   */
			/* --------------------------------------------------------- */
			else
			{
				rrt_low = 0;
				rrt_med = 0;
				rrt_high = 100;
			}
			
			
			/* If, during Up-Peak, a car has a lobby hall call   */
			/* already assigned or the elderly_pwt is beyond the */
			/* bounds of the PWT sets, then set membership so    */
			/* that the lobby hall call is considered to be a    */
			/* 100% affected elderly call.                       */
			/* ------------------------------------------------- */
			
			if ( ( (*e2p_ica_nlb_enable == TRUE) && (ocss_info[car].opmode == MIT) &&
				((ocss_info[car].assigned_calls[bld_info.ocss_lobby_pos] & S_FUHC) != 0) ) ||
				(eld_pwt > PWT_X_UPPER) )
			{
				eld_pwt_none = 0;
				eld_pwt_very = 100;
				/*
				msg_temp[0] = 'T';
				msg_temp[1] = car;
				msg_temp[2] = ocss_info[car].opmode;
				msg_temp[3] = bld_info.ocss_lobby_pos;
				msg_temp[4] = ocss_info[car].assigned_calls[bld_info.ocss_lobby_pos] & S_FUHC;
				msg_temp[5] = eld_pwt;
				msg_temp[6] = call_pos;         
				msg_temp[7] = *e2p_ica_nlb_enable;         
				msg_temp[8] = 2;
				msg_temp[9] = 3;
				msg_temp[10] = 13;
				
				  if (!(sio_TxPort(GSV_TOOL_PORT, msg_temp, 11))) 
				  E_log(DEBUG_TX_ERROR);
				*/
				
			}
			/* else, set none and very from fuzzy sets.          */
			/* ------------------------------------------------- */
			else
			{
			/* jsr block 
			#ifndef CAGS
			eld_pwt_none = ADJ_AdjustedSets.pwt.none[eld_pwt][autoadj_current_array];
			eld_pwt_very = ADJ_AdjustedSets.pwt.elderly[eld_pwt][autoadj_current_array];
			#endif
				*/
				eld_pwt_none = PWT_Light_None[eld_pwt];
				eld_pwt_very = PWT_Light_Elderly[eld_pwt];
			}
			
			//if (ica_rrts[car] != 0) 
			//DBD 20Feb04  ,It is only available for MultipleAssign with ImmediateCase
			//DBD-honma 30Jun2004  , fixed passenger count problem.
			if (((ica_rrts[car] != 0) || ((*e2p_DbdAssignEnable != 0) && ((*e2p_DbdAssignType & AssignTypeImmediate) != 0))) && (ica_rrts[car] < RRT_MAX))
			{
				/* Evaluate low, medium and high RRT sets */
				/* -------------------------------------- */
				for (i=0; i < 3; i++)
				{
					rrt_idx = 0;
					assn_idx = 0;
					
					/* Determine RRT index and rule column indices */
					/* for low, medium and high RRT's.             */
					/* ------------------------------------------- */
					switch (i)
					{
						
						/* Case 0                                         */
						/* Define variables for low RRT fuzzy set         */
						/* ---------------------------------------------- */
					case 0:
						
						rrt_idx = rrt_low;
						if (rrt_idx != 0) 
						{
							/* If no coincident car call, use */
							/* standard assignment matrices.  */
							/* ------------------------------ */
							if (*e2p_DbdAssignEnable == 0)
							{
								if (coinc_cc_flag == FALSE)
								{
									eld_idx_none = *e2p_s_low_none;
									eld_idx_very = *e2p_s_low_elderly;
								}
								else
								{
									eld_idx_none = *e2p_c_low_none;
									eld_idx_very = *e2p_c_low_elderly;
									/* If it has been enabled, evaluate unexpected */
									/* car performance measure.                    */
									/* ------------------------------------------- */
									if (*e2p_ica_pm_unex_car != 0)
									{
										if (eld_idx_none > eld_idx_very)
										{
											eld_idx_incr = (*e2p_ica_pm_unex_car) *
												(eld_idx_none - eld_idx_very);
											
											eld_idx_very += ( (eld_idx_incr + 50) /100);
											
										} /* eld_idx_none > eld_idx_very */
									} /* PM active */
								}
							}
							else //DBD
							{
								if ((coinc_cc_flag == FALSE) && (coinc_dcc_flag == FALSE))
									//if (coinc_cc_flag == FALSE)
								{
									eld_idx_none = *e2p_s_low_none;
									eld_idx_very = *e2p_s_low_elderly;
								}
								/* Otherwise, use coincident call */
								/* assignment matrices.           */
								/* ------------------------------ */
								else //DBD CoincidentCase 
								{
									//DBD
									//DBD  Source and Destination Coincident
									if (((coinc_type & CC_HC) != 0) && ((coinc_type & CC_DCC) != 0))
									{
									/* original 
									eld_idx_none = *e2p_c_low_none;
									eld_idx_very = *e2p_c_low_elderly;
										*/
										/*DBDnew */
										if ((coinc_type & CC_HCwithHC) != 0)
										{
											eld_idx_none = *e2p_dbd_csd_low_none;
											eld_idx_very = *e2p_dbd_csd_low_elderly;
										}
										else 
										{
											eld_idx_none = *e2p_dbd_c_low_none;
											eld_idx_very = *e2p_dbd_c_low_elderly;
										}
										
									}
									else if ((coinc_type & CC_HC) != 0)
									{
									/*  Normal 
									eld_idx_none = *e2p_c_low_none;
									eld_idx_very = *e2p_c_low_elderly;
										*/
										/* DBDnew */
										if (affectPwtByDst > 0)
										{
											eld_idx_none = *e2p_dbd_csrc_low_none;
											eld_idx_very = *e2p_dbd_csrc_low_elderly;
										}
										else
										{
											eld_idx_none = *e2p_dbd_c_low_none;
											eld_idx_very = *e2p_dbd_c_low_elderly;
										}
									}
									else if ((coinc_type & CC_DCC) != 0)
									{
									/*
									eld_idx_none = *e2p_c_low_none -5 ;
									eld_idx_very = *e2p_c_low_elderly;
										*/
										/* Normal 
										eld_idx_none = *e2p_s_low_none;
										eld_idx_very = *e2p_s_low_elderly;
										*/ 
										
										//if (affectPwtBySrc > 0)  
										//if (affectPwtBySrc == 0)  
										//Currently, if the car has just pwt calls.....
										
										/* DBDnew */
										{
											eld_idx_none = *e2p_dbd_cdst_low_none;
											eld_idx_very = *e2p_dbd_cdst_low_elderly;
										}
										
										
										/*
										else
										{
										eld_idx_none = *e2p_dbd_c_low_none;
										eld_idx_very = *e2p_dbd_c_low_elderly;
										}
										*/
										
									}
									/* If it has been enabled, evaluate unexpected */
									/* car performance measure.                    */
									/* ------------------------------------------- */
									if (*e2p_ica_pm_unex_car != 0)
									{
										if (eld_idx_none > eld_idx_very)
										{
											eld_idx_incr = (*e2p_ica_pm_unex_car) *
												(eld_idx_none - eld_idx_very);
											
											eld_idx_very += ( (eld_idx_incr + 50) /100);
											
										} /* eld_idx_none > eld_idx_very */
									} /* PM active */
									
								}//CoincidentCase
							}//DBD AssignEnable
							
             } /* if RRT is a member of low rrt set */
			 break;
			 
			 
			 /* Case 1                                         */
			 /* Define variables for medium RRT fuzzy set      */
			 /* ---------------------------------------------- */
         case 1: 
			 
			 rrt_idx = rrt_med;
			 if (rrt_idx != 0) 
			 {
				 /* If no coincident car call, use */
				 /* standard assignment matrices.  */
				 /* ------------------------------ */
				 if (*e2p_DbdAssignEnable == 0)
				 {
					 if (coinc_cc_flag == FALSE)
					 {
						 eld_idx_none = *e2p_s_med_none;
						 eld_idx_very = *e2p_s_med_elderly;
					 }
					 else
					 {
						 eld_idx_none = *e2p_c_med_none;
						 eld_idx_very = *e2p_c_med_elderly;
						 /* If it has been enabled, evaluate unexpected */
						 /* car performance measure.                    */
						 /* ------------------------------------------- */
						 if (*e2p_ica_pm_unex_car != 0)
						 {
							 if (eld_idx_none > eld_idx_very)
							 {
								 eld_idx_incr = (*e2p_ica_pm_unex_car) *
									 (eld_idx_none - eld_idx_very);
								 
								 eld_idx_very += ( (eld_idx_incr + 50) /100);
								 
							 } /* eld_idx_none > eld_idx_very */
						 } /* PM active */
					 }
				 }
				 else //DBD
				 {
					 if ((coinc_cc_flag == FALSE) && (coinc_dcc_flag == FALSE))
						 //if (coinc_cc_flag == FALSE)
					 {
						 eld_idx_none = *e2p_s_med_none;
						 eld_idx_very = *e2p_s_med_elderly;
					 }
					 /* Otherwise, use coincident call */
					 /* assignment matrices.           */
					 /* ------------------------------ */
					 else //DBD CoincidentCase 
					 {
						 //DBD
						 //DBD  Source and Destination Coincident
						 if (((coinc_type & CC_HC) != 0) && ((coinc_type & CC_DCC) != 0))
						 {
						 /* original 
						 eld_idx_none = *e2p_c_med_none;
						 eld_idx_very = *e2p_c_med_elderly;
							 */
							 /*DBDnew*/
							 if ((coinc_type & CC_HCwithHC) != 0)
							 {
								 eld_idx_none = *e2p_dbd_csd_med_none;
								 eld_idx_very = *e2p_dbd_csd_med_elderly;
							 }
							 else 
							 {
								 eld_idx_none = *e2p_dbd_c_med_none;
								 eld_idx_very = *e2p_dbd_c_med_elderly;
							 }
						 }
						 else if ((coinc_type & CC_HC) != 0)
						 {
						 /*  Normal 
						 eld_idx_none = *e2p_c_med_none;
						 eld_idx_very = *e2p_c_med_elderly;
							 */
							 /* DBDnew */
							 if (affectPwtByDst > 0)
							 {
								 eld_idx_none = *e2p_dbd_csrc_med_none;
								 eld_idx_very = *e2p_dbd_csrc_med_elderly;
							 }
							 else
							 {
								 eld_idx_none = *e2p_dbd_c_med_none;
								 eld_idx_very = *e2p_dbd_c_med_elderly;
							 }
							 
						 }
						 else if ((coinc_type & CC_DCC) != 0)
						 {
						 /*
						 eld_idx_none = *e2p_c_med_none -5 ;
						 eld_idx_very = *e2p_c_med_elderly;
							 */
							 /* Normal 
							 eld_idx_none = *e2p_s_med_none;
							 eld_idx_very = *e2p_s_med_elderly;
							 */
							 
							 //if (affectPwtBySrc > 0)  
							 //Currently, if the car has just pwt calls.....
							 /* DBDnew */
							 {
								 eld_idx_none = *e2p_dbd_cdst_med_none;
								 eld_idx_very = *e2p_dbd_cdst_med_elderly;
							 }
							 
							 /*
							 else
							 {
							 eld_idx_none = *e2p_dbd_c_med_none;
							 eld_idx_very = *e2p_dbd_c_med_elderly;
							 }
							 */
						 }
						 /* If it has been enabled, evaluate unexpected */
						 /* car performance measure.                    */
						 /* ------------------------------------------- */
						 if (*e2p_ica_pm_unex_car != 0)
						 {
							 if (eld_idx_none > eld_idx_very)
							 {
								 eld_idx_incr = (*e2p_ica_pm_unex_car) *
									 (eld_idx_none - eld_idx_very);
								 
								 eld_idx_very += ( (eld_idx_incr + 50) /100);
								 
							 } /* eld_idx_none > eld_idx_very */
						 } /* PM active */
						 
					 }//CoincidentCase
				 }//DBD AssignEnable
            } /* if RRT is a member of medium rrt set */
			break;
			
			/* Case 2                                         */
			/* Define variables for high RRT fuzzy set        */
			/* ---------------------------------------------- */
         case 2:
			 
			 rrt_idx = rrt_high;
			 if (rrt_idx != 0) 
			 {
				 /* If no coincident car call, use */
				 /* standard assignment matrices.  */
				 /* ------------------------------ */
				 if (*e2p_DbdAssignEnable == 0)
				 {
					 if (coinc_cc_flag == FALSE)
					 {
						 eld_idx_none = *e2p_s_high_none;
						 eld_idx_very = *e2p_s_high_elderly;
					 }
					 else
					 {
						 eld_idx_none = *e2p_c_high_none;
						 eld_idx_very = *e2p_c_high_elderly;
					 }
				 }
				 else //DBD
				 {
					 if ((coinc_cc_flag == FALSE) && (coinc_dcc_flag == FALSE))
						 //if (coinc_cc_flag == FALSE)
					 {
						 eld_idx_none = *e2p_s_high_none;
						 eld_idx_very = *e2p_s_high_elderly;
					 }
					 /* Otherwise, use coincident call */
					 /* assignment matrices.           */
					 /* ------------------------------ */
					 else //DBD CoincidentCase 
					 {
						 //DBD
						 //DBD  Source and Destination Coincident
						 if (((coinc_type & CC_HC) != 0) && ((coinc_type & CC_DCC) != 0))
						 {
						 /* original 
						 eld_idx_none = *e2p_c_high_none;
						 eld_idx_very = *e2p_c_high_none;
							 */
							 /* DBDnew */
							 if ((coinc_type & CC_HCwithHC) != 0)
							 {
								 eld_idx_none = *e2p_dbd_csd_high_none;
								 eld_idx_very = *e2p_dbd_csd_high_elderly;
							 }
							 else 
							 {
								 eld_idx_none = *e2p_dbd_c_high_none;
								 eld_idx_very = *e2p_dbd_c_high_elderly;
							 }
							 
						 }
						 else if ((coinc_type & CC_HC) != 0)
						 {
						 /*  Normal 
						 eld_idx_none = *e2p_c_high_none;
						 eld_idx_very = *e2p_c_high_elderly;
							 */
							 /* DBDnew */
							 if (affectPwtByDst > 0)
							 {
								 eld_idx_none = *e2p_dbd_csrc_high_none;
								 eld_idx_very = *e2p_dbd_csrc_high_elderly;
							 }
							 else
							 {
								 eld_idx_none = *e2p_dbd_c_high_none;
								 eld_idx_very = *e2p_dbd_c_high_elderly;
							 }
							 
						 }
						 else if ((coinc_type & CC_DCC) != 0)
						 {
						 /*
						 eld_idx_none = *e2p_c_high_none -5 ;
						 eld_idx_very = *e2p_c_high_elderly;
							 */
							 /* Normal 
							 eld_idx_none = *e2p_s_high_none;
							 eld_idx_very = *e2p_s_high_elderly;
							 */
							 
							 //if (affectPwtBySrc > 0)  
							 //Currently, if the car has just pwt calls.....
							 /* DBDnew */
							 {
								 eld_idx_none = *e2p_dbd_cdst_high_none;
								 eld_idx_very = *e2p_dbd_cdst_high_elderly;
							 }
							 
							 /*
							 else
							 {
							 eld_idx_none = *e2p_dbd_c_high_none;
							 eld_idx_very = *e2p_dbd_c_high_elderly;
							 }
							 */
						 }
						 
					 }//CoincidentCase
				 }//DBD AssignEnable
			 } /* if RRT is a member of fuzzy set high */
			 break;
			 
         default:
			 break;
      } /* switch : set up rrt index and rule column indices */
	  
	  
      /* If RRT index is not 0, continue evaluation. */
      /* ------------------------------------------- */
      if (rrt_idx != 0)
      {
		  
		  /* If elderly PWT is > 0, and elderly index > 0, calculate */
		  /* inrement for assignment index.  Floating point calc's   */
		  /* are used for more accuracy.                             */
		  /* ------------------------------------------------------- */
		  if ( (eld_pwt_none != 0) && (eld_idx_none != 0) )
		  {
			  long1 = rrt_idx;
			  long2 = eld_pwt_none;
			  long3 = eld_idx_none;
			  long4 = 10000;
			  long5 = (long1 * long2 * long3) / long4;
			  
			  assn_incr = long5;
			  
			  assn_idx += assn_incr;
		  }
		  
		  
		  /* If elderly PWT is > 0, and elderly index > 0, calculate */
		  /* inrement for assignment index.  Floating point calc's   */
		  /* are used for more accuracy.                             */
		  /* ------------------------------------------------------- */
		  if ( (eld_pwt_very != 0) && (eld_idx_very != 0) )
		  {  
			  
			  long1 = rrt_idx;
			  long2 = eld_pwt_very;
			  long3 = eld_idx_very;
			  long4 = 10000;
			  long5 = (long1 * long2 * long3) / long4;
			  
			  assn_incr = long5;
			  
			  assn_idx += assn_incr;
		  }
      } /* if rrt idx != 0 */
	  
      hc_assn_idx[car] = hc_assn_idx[car] + assn_idx;
	  
   } /* loop for low, medium, high fuzzy sets */	 
   
   } /* ica_rrts[car] > 1 */
   
		}
		
		
	}
	coinc_assign=FALSE;
	for(car=CAR1;car<MAX_OCSS;car++)
	{
		//HC and DCC are coincident
		if(coinc_cc[car]==TRUE && coinc_dcc[car]==TRUE)
		{
			if((call_mask & S_UHC)!=0 && nPax_up[car]>15)
			{
				continue;
			}
			if((call_mask & S_DHC)!=0 && nPax_dn[car]>15)
			{
				continue;
			}
			coinc_assign=TRUE;
			best_car=car;
			break;
		}
	}
	if(coinc_assign==FALSE)
	{
		if(ocss_info[1].opmode==MIT) // check car1 mode
			getHcAssnIdx(call_pos,call_dst,call_mask); // hc_assn_idx update
//		else if(ocss_info[1].opmode==NOR) //check car1 mode if it is normal 2way
//			getHcAssnIdx_1D(call_pos,call_dst,call_mask);

		/*
		if(al_id==3)
		{
			getHcAssnIdx_2D2Way(call_pos,call_dst,call_mask);
		}
		*/
		max_assn_idx = -10000;
		for (car=CAR1; car<=CAR8; car++)
		{
			if ( (hc_assn_idx[car] > max_assn_idx) ||
				( (hc_assn_idx[car] == max_assn_idx) && (best_car != MAX_OCSS)))
			{
				/* store the best car so far */
				best_car = car;
				max_assn_idx = hc_assn_idx[car];
			}  
		} /* for each car */
	}

	if(al_id==1)
	{
		AARULE_Best_Car = AARULE_Driver(call_pos,call_dst,
			call_mask,
			&hc_assn_idx[0],
			&ica_rrts[0],
			&ica_pwts[0]);
		if (AARULE_Best_Car != MAX_OCSS) {
			AARULE_Affected_HC_Cnt++;
			if (AARULE_Best_Car != best_car) {
				AARULE_Modified_HC_Cnt++;
				best_car = AARULE_Best_Car;
			}
		}
	}

    if ((call_mask & S_UHC) != 0)      dest_mask = DBD_SUCC;
    else if ((call_mask & S_DHC) != 0) dest_mask = DBD_SDCC;
	
	asslib_DbdAssignCall(best_car,dest_mask,call_pos,call_dst);
	if (best_car <= CAR8)
	{
		/* assign call to the best car, and flag it as an old call */
		/* ------------------------------------------------------- */
		asslib_assign_call(best_car, call_mask, call_pos);
		bld_info.old_hc_mask[grp_no][call_pos] |= call_mask;
	} 

	if(pure_rrts[best_car]<RRT_MAX)
	{
		sum_rrt+=pure_rrts[best_car];
		rrt_cnt++;
		avg_rrt=sum_rrt/rrt_cnt;
	}

	return best_car;
}
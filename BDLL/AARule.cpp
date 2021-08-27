/* U10 Project: @(#)aarule.c	2.15	3/23/95	14:22:26 */
/**************************************************************************
;  File Name: AARULE.C - Additional Assignment Rules Module
; 
;  File Description:
;  Provides services to support additional assignment rules once RRT and
;  PWT calculations have been made.
;
;
;  This work and the information it contains are the confidential
;  property of Otis Elevator Company ("Otis").  It is delivered on
;  the express condition that it will be used only for, or on
;  behalf of, Otis; that neither it nor the information it contains
;  will be reproduced, distributed or disclosed, in whole or in part,
;  without the written consent of Otis; and that on demand it and any
;  copies will be promptly returned.
;
;  Unpublished work - Copyright 1992 - 1995 Otis Elevator Company
;  All rights reserved.
;
;
;  Revision History:
;
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  AAA30xxxAAA           Mark Ross             	     04-Nov-93
;
;  A1230203BAA		 Mark Ross		     18-Feb-94
;  Modified to support donwpeak clustering.
;
;  A1330203BAA           Mark Ross		     18-Mar-1994
;  - STR 040.  Add LNS cars to applicable downpeak rules.
;
;  A1330203BAA		 Mark Ross		     24-Mar-94
;  Modified to lockout fo_car_select_tsk during downpeak operation (STR#72)
;
;  A1330203BAA		 Mark Ross		     14-Apr-94
;  Created AARULE_Find_Next_Demand routine. (STR#86)
;
;  A1430203BAA           Mark Ross		     02-May-94
;  - SCR 091.  Modified AARULE_Find_Next_Demand to clear reciprocol
;    car call demand.
;
;  A1430203BAA           Mark Ross		     02-May-94
;  - SCR 095.  Modified AARULE_DnpkRelativePosition to monitor car
;    pre-direction rather moving direction.
;
;  A1630203BAA           Geoff Mochau		     16-Sep-94
;  - SCR 30203-114: Send Top-of-Cluster (Parking Target) messages upon
;    change-of-state.
;
;  A1630203BAA           Paul Simcik/Mark Ross         08 August 1994
;  - SCR 30203-107
;    Modified to support seperation of traffic density and autoadjust
;    features.
;
;  A1730203BAA           Mark Ross       	     18 November 1994
;  - SCR 30203-122.
;    Modified to allow all AARULES to use traffic density info. Removed
;    logic from this module and placed it in AUTOADJ.C
;
;  A1730203BAA           Mark Ross                       19 December 1994
;  - SCR 30203-139
;    Modified to support ICA.C breakup.
;
;  A1730203BAA-12        Mark Ross                     19 December 1994
;  - SCR 30203-164
;    Modified to count contiguous stops rule as applied only when there 
;    actually is some contiguous demand.
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;  Transamerica          Geoff Mochau                  21 February 1995
;  - Substitute all JICA_FLOORS references with MAX_LANDINGS references.
;
;**************************************************************************/
#include "stdafx.h"
#include "common.h"

/**************************************************************************
;  Public Declarations
;*************************************************************************/
#define EXTERN extern   /* declare public variables */
#include "e2p.h"
#include "infocs.h"
#include "exec.h"
#include "inferr.h"
#include "infbld.h"
#include "asslib.h"
#include "assign.h"
#undef EXTERN

/**************************************************************************
;  Local Definitions
;*************************************************************************/
#include "aarule.lh"

/**************************************************************************
;  Public Definitions
;*************************************************************************/
#define EXTERN          /* define public variables */
#include "aarule.h"
#undef EXTERN

/**************************************************************************
;  Public Functions
;*************************************************************************/

enum RULE_PRIORITY {
	PRI_COINC,PRI_SVCWT,PRI_WT,RRT} 
dummy_priority;

/* These are meant to be temporary declarations to allow tracing with emulator */
   BYTE_T Assignment_Cnt; 	    /* add to AARULE_Driver */
   BYTE_T Assignment_Mask;	    /* add to AARULE_Driver */
   INT_T  good_rrt_assignment;      /* add to AARULE_Driver */
   INT_T  good_pwt_assignment;      /* add to AARULE_Driver */
   BYTE_T Car_Call_Before_Count;    /* add to AARULE_Sum_Demand */ 
   BYTE_T Hall_Call_Count;          /* add to AARULE_Sum_Demand */
   BYTE_T Additional_Demand_Count;  /* add to AARULE_Sum_Demand */
   BYTE_T My_Top;
	      
/**************************************************************************
;  Name: AARULE_Driver - Apply additional assignment rules.
;  
;  Description:
;  Apply additional assingment rules. Once RRT and PWT calculations have
;  been made it may be desireable to consider non-time based factors when
;  making an assingment. The additional assingment rules perform this 
;  fuction. Additional assingment rules used are:
;	o Leave parked cars parked.
;	o Give preference to cars with contiguous stops
;	o Give preference to cars with less load.
;	o Give preference to cars with less demand beyond the HC
;  
;  Parameters:
;  Call_Pos       (input) -  Landing of hall call
;  Call_Mask      (input) -  Type of hall call being dispatched
;  Assign_Index   (input) -  Pointer to assignment index array.
;  RRT_Ptr        (input) -  Pointer to RRT array.
;  PWT_Ptr        (input) -  Pointer to PWT array.
;  Value	  (output) - Car number for assignment as determined by 
;		 	     additional assingment rules.
;*************************************************************************/
BYTE_T AARULE_Driver(BYTE_T Call_Pos, BYTE_T Call_Dst,BYTE_T Call_Mask, INT_T *Assign_Index, 
                     INT_T *RRT_Ptr, INT_T *PWT_Ptr) 
   
{
   BYTE_T Rule_Priority;
   BYTE_T direction;
   BYTE_T i,j;
   BYTE_T Car;
   BYTE_T Mask;
   BYTE_T Cnt;
   BYTE_T Assigned_Car;
   
   
   Assigned_Car = MAX_OCSS;
   direction = (Call_Mask >> 1) - 1;
   AARULE_Mask = Call_Mask;
   AARULE_Landing = Call_Pos;
   AARULE_activity[Call_Pos].dir[direction].car = 0;
   AARULE_activity[Call_Pos].dir[direction].reason = 0;
   AARULE_activity[0].dir[1].car = 0;
   AARULE_activity[0].dir[1].reason = 0;
      
   /*  A P P L Y   T H E   A S S I G N M E N T   R U L E S   */
   /*--------------------------------------------------------*/
   /* For each priority, determine if any of the assingment  */
   /* rules has been been given this priority. If so, apply  */
   /* the rule. If not, move onto next priority.	     */
   /*--------------------------------------------------------*/
   for (i=HIGHEST_PRIORITY;i<=LOWEST_PRIORITY;i++) {
 
      /* Do necessary initialization  */
      /*------------------------------*/
      j = 1;
      Rule_Priority = AARULE_Get_Priority(j);
      
      /* Find the rule associated with this priority */
      /* Want rule "j" with priority "i".	     */
      /*---------------------------------------------*/
      while ((Rule_Priority!=i) && (j<=LOWEST_PRIORITY)) {
		j++;
		Rule_Priority = AARULE_Get_Priority(j);
      }
      
      /*  A P P L Y   R U L E   J,   P R I O R I T Y   I   */
      /*---------------------------------------------------*/
      if (j<=LOWEST_PRIORITY) {
			Assigned_Car = AARULE_Apply_Rule(j,
                                          Call_Pos,
										  Call_Dst,
                                          Call_Mask,
                                          Assign_Index,
                                          RRT_Ptr,
                                          PWT_Ptr); 
      }
          
      /*  D E T E R M I N E   A F F E C T   O F   R U L E   J        */
      /*-------------------------------------------------------------*/
      /* Values returned by AARULE_Apply_Rule:			     */
      /*  (1) 1 to 8 - This is a valid car id which indicates that   */
      /*               the assignment rule was applied and an        */
      /*               assignment was made. In this situation, log   */
      /*	       appropriate information and exit.	     */
      /*  (2) Invalid Car ID - An invalid car id is < 1 or > 8. This */
      /*               indicates that the assignment rule could not  */
      /*	       be applied. In this situation, try to apply   */
      /*	       next assignment rule.			     */
      /*-------------------------------------------------------------*/
      if ((Assigned_Car >= CAR1) && (Assigned_Car <= CAR8)) {
 
         switch(j)
         {
            /* Parking rule count.    */
            /*------------------------*/
            case AARULE_PARKED_CARS:
	       AARULE_activity[Call_Pos].dir[direction].car = Assigned_Car;
	       AARULE_activity[Call_Pos].dir[direction].reason = AARULE_PARKED_CARS;
	       AARULE_activity[0].dir[1].car = Assigned_Car;
	       AARULE_activity[0].dir[1].reason = AARULE_PARKED_CARS;
               AARULE_Park_Mod_Cnt++;
	       break;
	 
            /* Contiguous stops rule count  */
            /*------------------------------*/
            case AARULE_CONTIGUOUS_STOPS:
	       AARULE_activity[Call_Pos].dir[direction].car = Assigned_Car;
	       AARULE_activity[Call_Pos].dir[direction].reason = AARULE_CONTIGUOUS_STOPS;
	       AARULE_activity[0].dir[1].car = Assigned_Car;
	       AARULE_activity[0].dir[1].reason = AARULE_CONTIGUOUS_STOPS;
	       AARULE_Contiguos_Mod_Cnt++;
	       break;
	 
            /* Car load rule count    */
            /*------------------------*/
            case AARULE_CAR_LOAD:
	       AARULE_activity[Call_Pos].dir[direction].car = Assigned_Car;
	       AARULE_activity[Call_Pos].dir[direction].reason = AARULE_CAR_LOAD;
	       AARULE_activity[0].dir[1].car = Assigned_Car;
	       AARULE_activity[0].dir[1].reason = AARULE_CAR_LOAD;
	       AARULE_Load_Mod_Cnt++;
	       break;
	 
            /* Demand beyond HC rule count   */
            /*-------------------------------*/
            case AARULE_DEMAND_BEYOND_HC:
	       AARULE_activity[Call_Pos].dir[direction].car = Assigned_Car;
	       AARULE_activity[Call_Pos].dir[direction].reason = AARULE_DEMAND_BEYOND_HC;
	       AARULE_activity[0].dir[1].car = Assigned_Car;
	       AARULE_activity[0].dir[1].reason = AARULE_DEMAND_BEYOND_HC;
	       AARULE_Demand_Beyond_Cnt++;
	       break;
	 
            /* Downpeak rule count           */
            /*-------------------------------*/
            case AARULE_DOWNPEAK:
	       AARULE_activity[Call_Pos].dir[direction].car = Assigned_Car;
	       AARULE_activity[Call_Pos].dir[direction].reason = AARULE_DOWNPEAK;
	       AARULE_activity[0].dir[1].car = Assigned_Car;
	       AARULE_activity[0].dir[1].reason = AARULE_DOWNPEAK;
	       AARULE_Downpeak_Mod_Cnt++;
	       break;
	 
            default:
	       break;
         }
	 break;
      }
    }
   
   /*  M A K E   T H E   A S S I G N M E N T                  */
   /*---------------------------------------------------------*/
   /* At this point either all the active assignment rules    */
   /* have been applied or a particular assignment rule has   */
   /* reduced the assignment to a single car. Make the 	      */
   /* assignment. 					      */
   /*---------------------------------------------------------*/   
   return(Assigned_Car);
   
} /* AARULE_Driver */

     


/**************************************************************************
;  Local Functions - Common Functions
;*************************************************************************/



/**************************************************************************
;  Name: AARULE_Get_Priority - Get the priority assigned to a particular 
;		               additional assignment rule.
;  
;  Description:
;  Return the priority assiciated with the indicated additional assingment
;  rule. A zero value indicates that the rule is disabled. A non-zero value
;  indicates that the rule is enabled and has the given priority.
;
;  Parameters:
;  Rule_Number   (input)   Additional assignment rule number
;  Value	 (output)  Priority associated with assignment rule
;*************************************************************************/
BYTE_T AARULE_Get_Priority(BYTE_T Rule_Number) 
   
{
   BYTE_T Value;
   
   Value = 0;
   
   return(Rule_Number);   
   
} /* AARULE_Get_Priority */

/**************************************************************************
;  Name: AARULE_Exceptions - Look for those situations in which additional
;			     assignment rules should not be applied. 
;
;  Description:
;  There are situations in which the additional assignment rules should
;  not be applied. Specifically, look for coincident car calls and hall
;  calls at a landing where there is a parked/idle car available to 
;  answer that hall call.
;
;  Parameters:   
;  HC_Index_100  (input)    Pointer to hall call assignment index = 100 flag 
;  HC_Index_99   (input)    Pointer to hall call assignment index = 99 flag  
;  Coincident_CC (input)    Pointer to coincident car call
;  Assign_Index  (input)    Pointer to hall call assignment index
;*************************************************************************/
void AARULE_Exceptions(FLAG_T *HC_Index_100, FLAG_T *HC_Index_99,
                       FLAG_T *Coincident_CC, INT_T *Assign_Index) 
   
{
   BYTE_T Car;

   /* Assume conditions do not exist  */
   /*---------------------------------*/
    *HC_Index_100 = FALSE;
    *HC_Index_99 = FALSE;
    *Coincident_CC = FALSE;

   /* Check for special assignment conditions:			    */
   /*    (1) hc_assn_idx[car] = 100				    */
   /*         - RRT = 0, (immediate car flag is set)		    */
   /*	      - There is a parked or idle car at the landing of     */
   /*	        the hall call being assigned.			    */
   /*    (2) hc_assn_idx[car] = 99				    */
   /*	      - MIT cars given preference (RRT used to resolve      */
   /* 	        assignment amoung MIT cars.)		            */
   /*    (3) Coincident Car Calls - A car has a CC at landing of CC */
   /*---------------------------------------------------------------*/
   for (Car=CAR1;Car<=CAR8;Car++) {
     if (*(Assign_Index+Car) == 100) {
       *HC_Index_100 = TRUE;
       break;
     }
   }
   
   for (Car=CAR1;Car<=CAR8;Car++) {
     if (*(Assign_Index+Car) == 99) {
       *HC_Index_99 = TRUE;
       break;
     }
   }

   for (Car=CAR1;Car<=CAR8;Car++) {
     if (*(AARULE_Coincident_CC+Car) == TRUE) {
       *Coincident_CC = TRUE;
       break;
     }
   }
    
} /* AARULE_Exceptions */

BYTE_T AARULE_SvcWait(BYTE_T Call_Pos, BYTE_T Call_Dst,BYTE_T Call_Mask,
                         INT_T *Assign_Index, INT_T *RRT_Ptr, INT_T *PWT_Ptr) 
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
	
	for(int i=0;i<(MAX_OCSS/2);i++)
	{
		int car=(Call_Dst <= bld_info.lz_top_pos) ? i*2+1 : i*2+2; //if dst is low , check lower car
		asslib_update_summary(car);
		//만일 upper car에 할당이 없으면 lower car에 할당 안함
		if(Call_Dst<=bld_info.lz_top_pos)
		{
			upper_car=car+1;
			asslib_update_summary(upper_car);
			// upper 카에 할당된 콜이 없고 upper car가 내려오거나 맨 위층에 있으면 Lower Car에는 할당 안함
			if(ocss_info[upper_car].upsummary<=0 && ocss_info[upper_car].target_pos<=ocss_info[upper_car].actual_pos)
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
			if(ocss_info[car].assigned_calls[j]>0)
				call_no++;
			if(ocss_info[car].future_calls[j]>0)
				upStop_no++;
		}
		
		if((upStop_no+call_no)<min_callNo)
		{
			min_callNo=upStop_no+call_no;
			//if upper car has no call
			if(min_callNo==0 && Call_Dst>bld_info.lz_top_pos)
				min_rrt=RRT_MAX;
		}
		
		//find car with rrt below avg_rrt except upper car has no call
		if(pure_rrts[car]<=avg_rrt && (upStop_no+call_no)<=1 && pure_rrts[car]<min_rrt
			&& !(min_callNo==0 && Call_Dst>bld_info.lz_top_pos)
			)
		{
			min_rrt=pure_rrts[car];
			min_car=car;
		}
		
		// when upper car has no call
		if((upStop_no+call_no)==0 && pure_rrts[car]<min_rrt
			&& min_callNo==0 && Call_Dst>bld_info.lz_top_pos
			)
		{
			min_rrt=pure_rrts[car];
			min_car=car;
		}
	}

	Value=min_car;
	return Value;	
}

BYTE_T AARULE_SvcWait_Nor(BYTE_T Call_Pos, BYTE_T Call_Dst,BYTE_T Call_Mask,
                         INT_T *Assign_Index, INT_T *RRT_Ptr, INT_T *PWT_Ptr) 
{
	FLAG_T     coinc_cc_flag;
	BYTE_T Value;
	INT_T car;
	int min_car=MAX_OCSS;
	int scan_start=(Call_Dst <= bld_info.lz_top_pos) ? 1 : bld_info.uz_bottom_pos;
	int scan_stop=(Call_Dst <= bld_info.lz_top_pos) ? bld_info.lz_top_pos : bld_info.top_pos;
	int min_callNo=bld_info.top_pos*2;
	int min_upStopNo=bld_info.top_pos*2;
	int min_rrt=RRT_MAX;
	int upperNoCall=0;
	int upperCall_no=0;
	int lowerCall_no=0;
	int callNo_arr[MAX_OCSS];
	int upper_car;
	int lower_car;
	int min_lowerPos;
	Value=MAX_OCSS;
	
	for(int i=0;i<MAX_OCSS/2;i++)
	{
		upper_car=i*2+2;
		lower_car=i*2+1;
		asslib_update_summary(upper_car);
		asslib_update_summary(lower_car);
		int call_no=0;
		int upStop_no=0;
		lowerCall_no=0;
		upperCall_no=0;
		for(int j=1;j<=bld_info.top_pos;j++)
		{
			if(ocss_info[upper_car].assigned_calls[j]>0 
				|| ocss_info[upper_car].future_calls[j]>0)
				upperCall_no++;
			if(ocss_info[lower_car].assigned_calls[j]>0 
				|| ocss_info[lower_car].future_calls[j]>0)
				lowerCall_no++;
		}
		callNo_arr[lower_car]=lowerCall_no;
		callNo_arr[upper_car]=upperCall_no;
		if((Call_Mask&S_UHC)!=0)
		{
			car=upper_car;			
			call_no=upperCall_no;
		}
		else if((Call_Mask&S_DHC)!=0)
		{
			car=lower_car;
			call_no=lowerCall_no;
		}
		if((call_no)<min_callNo)
		{
			min_callNo=call_no;
			min_rrt=RRT_MAX;
		}
		
		// when upper car has no call
		if((call_no)==min_callNo && pure_rrts[car]<min_rrt)
		{
			min_rrt=pure_rrts[car];
			min_car=car;
		}

	}

	/*
	if((Call_Mask & S_DHC)!=0 &&
		Call_Pos>ocss_info[min_car].actual_pos &&
		ocss_info[min_car].mov_dir==DOWN_DIR)
	{
		min_callNo=bld_info.top_pos*2;
		for(int i=0;i<MAX_OCSS/2;i++)
		{
			upper_car=i*2+2;
			lower_car=i*2+1;
			if(ocss_info[upper_car].actual_pos==bld_info.top_pos+1 && 
				((ocss_info[upper_car].assigned_calls[Call_Dst]&S_UHC)==0 || Call_Dst==1))
			{
				if(ocss_info[upper_car].dnhall_flag==FALSE)
				{
					min_lowerPos=bld_info.top_pos;
				}
				if(ocss_info[upper_car].dnhall_flag==FALSE && ocss_info[lower_car].actual_pos<min_lowerPos)
				{
					min_lowerPos=ocss_info[lower_car].actual_pos;
					min_car=upper_car;
				}
			}
		}
	}
*/

	Value=min_car;
	return Value;	
}


BYTE_T AARULE_Svc(BYTE_T Call_Pos, BYTE_T Call_Dst,BYTE_T Call_Mask,
                         INT_T *Assign_Index, INT_T *RRT_Ptr, INT_T *PWT_Ptr) 
{
	FLAG_T     coinc_cc_flag;
	BYTE_T Value;
	INT_T car;

	int min_car=0;
	int scan_start=(Call_Dst <= bld_info.lz_top_pos) ? 1 : bld_info.uz_bottom_pos;
	int scan_stop=(Call_Dst <= bld_info.lz_top_pos) ? bld_info.lz_top_pos : bld_info.top_pos;
	int min_callNo=bld_info.top_pos*2;
	int min_upStopNo=bld_info.top_pos*2;
	int upperNoCall=0;
	int min_rrt=RRT_MAX;
	int upper_car;
	Value=MAX_OCSS;
	
	for(int i=0;i<(MAX_OCSS/2);i++)
	{
		int car=(Call_Dst <= bld_info.lz_top_pos) ? i*2+1 : i*2+2; //if dst is low , check lower car
		asslib_update_summary(car);
		//만일 upper car에 할당이 없으면 lower car에 할당 안함
		if(Call_Dst<=bld_info.lz_top_pos)
		{
			upper_car=car+1;
			asslib_update_summary(upper_car);
			// upper 카에 할당된 콜이 없고 upper car가 내려오거나 맨 위층에 있으면 Lower Car에는 할당 안함
			if(ocss_info[upper_car].upsummary<=0 && ocss_info[upper_car].target_pos<=ocss_info[upper_car].actual_pos)
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
			if(ocss_info[car].assigned_calls[j]>0)
				call_no++;
			if(ocss_info[car].future_calls[j]>0)
				upStop_no++;
		}
		if((upStop_no+call_no)<min_callNo)
		{
			min_callNo=upStop_no+call_no;
			min_rrt=RRT_MAX;
		}
		// when upper car has no call
		if((upStop_no+call_no)==min_callNo && pure_rrts[car]<min_rrt)
		{
			min_rrt=pure_rrts[car];
			min_car=car;
		}
	}

	if(bFirstAssign==TRUE)
	{
		bFirstAssign=FALSE;
		int min_floor=bld_info.top_pos;
		//처음 할당할 때는 Upper카중 제일 낮은카에 할당
		for(int i=0;i<(MAX_OCSS/2);i++)
		{
			int car=i*2+2; //check lower car
			if(ocss_info[car].actual_pos<min_floor)
			{
				min_car=car;
				min_floor=ocss_info[car].actual_pos;
			}
		}
	}

	Value=min_car;
	return Value;	
}

BYTE_T AARULE_Svc_Nor(BYTE_T Call_Pos, BYTE_T Call_Dst,BYTE_T Call_Mask,
                         INT_T *Assign_Index, INT_T *RRT_Ptr, INT_T *PWT_Ptr) 
{
	FLAG_T     coinc_cc_flag;
	BYTE_T Value;
	INT_T car;

	int min_car=0;
	int scan_start=(Call_Dst <= bld_info.lz_top_pos) ? 1 : bld_info.uz_bottom_pos;
	int scan_stop=(Call_Dst <= bld_info.lz_top_pos) ? bld_info.lz_top_pos : bld_info.top_pos;
	int min_callNo=bld_info.top_pos*2;
	int min_upStopNo=bld_info.top_pos*2;
	int upperNoCall=0;
	int min_rrt=RRT_MAX;
	int upper_car;
	Value=MAX_OCSS;
	
	for(int i=0;i<(MAX_OCSS/2);i++)
	{
		int car=(Call_Dst <= bld_info.lz_top_pos) ? i*2+1 : i*2+2; //if dst is low , check lower car
		asslib_update_summary(car);
		//만일 upper car에 할당이 없으면 lower car에 할당 안함
		if(Call_Dst<=bld_info.lz_top_pos)
		{
			upper_car=car+1;
			asslib_update_summary(upper_car);
			// upper 카에 할당된 콜이 없고 upper car가 내려오거나 맨 위층에 있으면 Lower Car에는 할당 안함
			if(ocss_info[upper_car].upsummary<=0 && ocss_info[upper_car].target_pos<=ocss_info[upper_car].actual_pos)
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
			if(ocss_info[car].assigned_calls[j]>0)
				call_no++;
			if(ocss_info[car].future_calls[j]>0)
				upStop_no++;
		}
		if((upStop_no+call_no)<min_callNo)
		{
			min_callNo=upStop_no+call_no;
			min_rrt=RRT_MAX;
		}
		// when upper car has no call
		if((upStop_no+call_no)==min_callNo && pure_rrts[car]<min_rrt)
		{
			min_rrt=pure_rrts[car];
			min_car=car;
		}
	}

	if(bFirstAssign==TRUE)
	{
		bFirstAssign=FALSE;
		int min_floor=bld_info.top_pos;
		//처음 할당할 때는 Upper카중 제일 낮은카에 할당
		for(int i=0;i<(MAX_OCSS/2);i++)
		{
			int car=i*2+2; //check lower car
			if(ocss_info[car].actual_pos<min_floor)
			{
				min_car=car;
				min_floor=ocss_info[car].actual_pos;
			}
		}
	}

	Value=min_car;
	return Value;	
}


BYTE_T AARULE_Coinc(BYTE_T Call_Pos, BYTE_T Call_Dst,BYTE_T Call_Mask,
                         INT_T *Assign_Index, INT_T *RRT_Ptr, INT_T *PWT_Ptr) 
{
	FLAG_T     coinc_cc_flag;
	BYTE_T Value;
	INT_T car;
	Value=MAX_OCSS;
	for(car=CAR1;car<MAX_OCSS;car++)
	{
		if((ocss_info[car].future_calls[Call_Dst])>0)
		{
			Value=car;
			coinc_cc_flag=TRUE;
			break;
		}
	}
	
	return Value;
}

BYTE_T AARULE_Coinc_Nor(BYTE_T Call_Pos, BYTE_T Call_Dst,BYTE_T Call_Mask,
                         INT_T *Assign_Index, INT_T *RRT_Ptr, INT_T *PWT_Ptr) 
{
	FLAG_T     coinc_cc_flag;
	BYTE_T Value;
	INT_T car;
	int call_no;
	int scan_start=1;
	int scan_stop=bld_info.top_pos;
	Value=MAX_OCSS;
	for(car=CAR1;car<MAX_OCSS;car++)
	{
		if((ocss_info[car].assigned_calls[Call_Pos] & Call_Mask)!=0)
		{
			call_no=0;
			for(int j=scan_start;j<=scan_stop;j++)
			{
				if(ocss_info[car].assigned_calls[j]>0 || ocss_info[car].future_calls[j]>0)
					call_no++;
			}
			if(call_no<bld_info.top_pos/2 ||
				ocss_info[car].assigned_calls[Call_Dst]>0 || ocss_info[car].future_calls[Call_Dst]>0)
			{
				Value=car;
				coinc_cc_flag=TRUE;
				break;
			}
		}
	}
	return Value;
}


/**************************************************************************
;  Name: AARULE_Apply_Rule - Apply the given additional assigment rule.  
;
;  Description:
;  Apply the given additional assignment rule to the given hall call. Return
;  decision made by additional assignment rule logic.
;
;  Parameters:
;  Rule_Number     (input)   Number of assingment rule to be applied.
;  Call_Pos	   (input)   Landing of hall call
;  Call_Mask       (input)   Hall call type.
;  Value           (output)  Mask indicating cars available for assignment
;			     after applying given assignment rule.
;*************************************************************************/
BYTE_T AARULE_Apply_Rule(BYTE_T Rule_Number, BYTE_T Call_Pos,BYTE_T Call_Dst, BYTE_T Call_Mask,
                         INT_T *Assign_Index, INT_T *RRT_Ptr, INT_T *PWT_Ptr) 
   
{

   BYTE_T Value;
   TASK_ID_T Task_ID;
   
   Value = MAX_OCSS;
   
   switch(Rule_Number)
   {
      /* Rule is disabled	*/
      /*------------------------*/
      case AARULE_COINC:
		  if(ocss_info[1].opmode==MIT)
			Value = AARULE_Coinc(Call_Pos,Call_Dst,Call_Mask,Assign_Index,
                                      RRT_Ptr,PWT_Ptr);
		  else if(ocss_info[1].opmode==NOR)
			Value = AARULE_Coinc_Nor(Call_Pos,Call_Dst,Call_Mask,Assign_Index,
                                      RRT_Ptr,PWT_Ptr);
	 break;
	 
      /* Apply svctime and wating time */
      /*------------------------*/
      case AARULE_SVCWAIT:
		  if(ocss_info[1].opmode==MIT)
			Value = AARULE_SvcWait(Call_Pos,Call_Dst,Call_Mask,Assign_Index,
                                      RRT_Ptr,PWT_Ptr);
		  else if(ocss_info[1].opmode==NOR)
			Value = AARULE_SvcWait_Nor(Call_Pos,Call_Dst,Call_Mask,Assign_Index,
                                      RRT_Ptr,PWT_Ptr);
	
	 break;
	 
      /* Apply contiguous stops rule */
      /*-----------------------------*/
      case AARULE_WAIT:
		  if(ocss_info[1].opmode==MIT)
			Value = AARULE_Svc(Call_Pos,Call_Dst,Call_Mask,Assign_Index,
                                      RRT_Ptr,PWT_Ptr);	 
		  else if(ocss_info[1].opmode==NOR)
			Value = AARULE_Svc(Call_Pos,Call_Dst,Call_Mask,Assign_Index,
                                      RRT_Ptr,PWT_Ptr);	 

			break;
      default:
	 break;
   }
   return(Value);
} /* AARULE_Apply_Rule */


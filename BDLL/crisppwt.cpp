#ifdef CAGS
extern unsigned char afterhallcalls[8];
unsigned char honma_reassign_prev;
#endif
/* U10 Project: @(#)crisppwt.c	1.4	3/23/95	14:22:34 */
/**************************************************************************
;  File Name: CRISPPWT.C - Tasks and procedures for pwt estimator.
;
;  File Description:
;
;  Contains tasks and procedures for handling crisp PWT estimation.
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
;  Unpublished work - Copyright 1994 - 1995 Otis Elevator Company
;  All rights reserved.
;
;
;  Revision History:
;
;  SCN                   Author                        Date
;  ------------------------------------------------------------------------
;  A1730203BAA           Mark Ross                     19 December 1994
;  - SCR 30203-139
;    Created module. Functions in this module were originally contained
;    in the "ica.c" module.
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;  A1730203BAA-12        Mark Ross                     06 February 1995
;  - SCR 30203-181
;    In crisppwt_calculate_pwt removed temporary hall call write to 
;    assigned calls. When task is pre-empted by ring communications to 
;    process a delete message, there appear to be two cars assigned to the
;    hall call. Use snap_shot data for the temporary write.
;
;  Transamerica          Geoff Mochau                  20 February 1995
;  - Removed Advanced-Predirection Metrics to limit RAM requirements.
;
;  J1830203AAA 
;  - SCR 30203-5008    H.Honma (Suzuki)                      26 May  1995
;    Use assign_DistanceUnsure[]  
;  - SCR 30203-5017    H.Honma (Suzuki)                      29 May  1995
;    Support Wheelchair Mode as Extended Car Calls/Hall Calls
;    change call mask variables type from BYTE to INTEGER 
;  - SCR 30203-5009    H.Honma                               29 May  1995
;    Correct PWT calculation
;
;  JAB-30203-AAB         Hideyuki Honma                   08  Apr 1996
;  - SCR 5079:  Correct PWT calculation logic of "crisppwt.c"
;
;  JAB-30203-AAB         Hideyuki Honma                   25  Apr 1996
;  - SCR 5083:  Change PWT calculation
;
;*************************************************************************/
#include "stdafx.h"
#include "common.h"
#ifdef CAGS
#include "global.lit"
#include "global.h"
#endif
/**************************************************************************
;  Public Declarations
;*************************************************************************/
#define EXTERN extern   /* declare public variables */
#ifndef CAGS
#include "inferr.h"
#include "infocs.h"
#include "infrng.h"
#include "sio.h"
#include "exec.h"

#endif //ifndef CAGS

#include "e2p.h"
#include "infmsg.h"
#include "infbld.h"
#include "aarule.h"
#include "assign.h"
#include "asslib.h"
//DBD
#include "infads.h"
#undef EXTERN


/**************************************************************************
;  Public Definitions
;*************************************************************************/
#define EXTERN          /* define public variables */
#include "crisppwt.h"
#undef EXTERN


/**************************************************************************
;  Local Definitions
;*************************************************************************/
#include "crisppwt.lh"

/**************************************************************************
;---------------------------------------------------------------------------
;  P U B L I C   C R I S P   P W T   F U N C T I O N S
;---------------------------------------------------------------------------
;***************************************************************************
;
;
;********************************************************************/
 
/*********************************************************************
; Function Name: crisppwt_calculate_pwt 
;
; Determines PWT value for call.
;
;********************************************************************/
INT_T crisppwt_calculate_pwt(BYTE_T car, BYTE_T pos, BYTE_T dstPos, BYTE_T pre_dir,
                             INT_T mask, INT_T rrt,BYTE_T distance,BYTE_T *distance3,BYTE_T *affectPwtByDst,
                             BYTE_T *spareCap)
{
 
BYTE_T call_dir, previously_assigned;
INT_T  max_elderly;
BYTE_T local_next_comm,localDbdNcf;
FLAG_T temp_top_cc_flag,temp_bottom_cc_flag;
INT_T  highest_call_pos,lowest_call_pos;

BYTE_T finish;
FLAG_T lateHallFlag;

    max_elderly = 0;
    ica_rat_pwt = rrt;
    ica_pwt_distance = distance;
    *distance3 = 0;
    temp_distance = 0;

    lateHallFlag = FALSE;

//DBD-honma spcap 22Mar04
    crisppwt_findPwtFlag = FALSE;
    crisppwt_maxSpareCap = 0;
    

#ifdef CAGS
  afterhallcalls[car] = 0;
#endif

    /* If the OCSS does not have a predirection established, then use the   */
    /* ICSS2's advanced predirection in order to avoid basing an assignment */
    /* on possibly "old" data.  If the OCSS does have a predirection set,   */
    /* then clear the ICSS2's advanced predirection, as it is now out-of-   */
    /* date.                                                                */
    /*----------------------------------------------------------------------*/
   if (ocss_info[car].pre_dir == NO_DIR)
     {
       pre_dir = ica_icss2_advanced_predir[car];
     }
   else
     {
       ica_icss2_advanced_predir[car] = NO_DIR;
     }


    local_next_comm = ocss_info[car].NCF;
    localDbdNcf = local_next_comm;

    if (ocss_info[car].mov_dir == DOWN_DIR)
      {
        /* for eliminate passing dispatch */
        if (((ocss_info[car].actual_pos - ocss_info[car].NCF) >= 2) &&
            (ocss_info[car].target_pos < ocss_info[car].NCF))
            local_next_comm--;
      } /* mov_dir == DOWN_DIR */
    else if (ocss_info[car].mov_dir == UP_DIR)
      {
        /* for eliminate passing dispatch */
        if (((ocss_info[car].NCF - ocss_info[car].actual_pos) >= 2) &&
             (ocss_info[car].target_pos >  ocss_info[car].NCF))
             local_next_comm++;
      } /* mov_dir == UP_DIR */


    /* Determine if call exists already.  If not, place dummy */
    /* hall call into assigned calls array, so that the rrt   */
    /* calculation will return the appropriate RRT.           */
    /* ------------------------------------------------------ */
// DBD    if ( (ocss_info[car].assigned_calls[pos] & mask) != 0)
// 26Feb04   for multiple assignment case.   temporarilly.
    if (0) // shs no use
    {
       previously_assigned = TRUE;
    }
    else
    {
       /* lock out assignment task temporarily so that we */
       /* don't transmit car assignment.                  */
       /* ----------------------------------------------- */
       ica_lock_out_assignments_flag = TRUE;
       previously_assigned = FALSE;
       /* honma
       ocss_info[car].assigned_calls[pos] |= mask;
       */
       dummy_assigned_calls[car][pos] |= mask;

    }
    

    /* Determine direction of call. */
    /* ---------------------------- */
    if ( (mask & S_UHC) != 0)
    {
       call_dir = UP_DIR;
    }
    else
    {
       call_dir = DOWN_DIR;
    }
    
    
    if( (call_dir == pre_dir) &&
        (pos == local_next_comm) &&
        (ica_rat_pwt > RSR_MIN) )
    {
       ica_rat_pwt = RSR_MIN;    
       lateHallFlag = TRUE;
    }

#ifdef CAGS
	if ((tm_sys > 71.0) && (car == CAR1))
         tm_sys = tm_sys;
#endif

    //DBD
    if ((dummy_assigned_calls[car][pos] & mask & E_HC) != 0)
       {
          ica_rat_pwt += RRT_WCS_STOP;
          ica_rat_pwt += RRT_SHORT_RUN;
       }
    else if ((dummy_assigned_calls[car][pos] & mask & S_HC) != 0)
       {
          ica_rat_pwt += RRT_HALL_STOP;
          ica_rat_pwt += RRT_SHORT_RUN;

          //DBd-honma spcap 22Mar04
          *spareCap += 1;  //Add NewPassenger
//          *spareCap += asslib_DbdAssignCallDirCnts[car][call_dir][pos];
          
       }
        

    if (pre_dir == UP_DIR)
      {
        if 
          ((pos!=ocss_info[car].NCF) &&
          ((ocss_info[car].f_door_state==DOORS_CLOSING) ||
          ((ocss_info[car].f_door_state==DOORS_CLOSED) &&
           (ocss_info[car].mov_dir==NO_DIR) &&
           (ocss_info[car].NCF==ocss_info[car].actual_pos))))
           {
            finish = local_next_comm;
           } 
        else 
            finish = local_next_comm - 1;         
/* DBD-honma TestInProgress  08Mar04 
        else if ((*e2p_DbdAssignEnable != 0) && (pos == ocss_info[car].NCF) && (ocss_info[car].actual_pos == ocss_info[car].NCF) &&
                ((ocss_info[car].f_door_state==DOORS_CLOSED)||(ocss_info[car].f_door_state==DOORS_CLOSING)) &&
                (ocss_info[car].target_pos > ocss_info[car].actual_pos)) 
            finish = local_next_comm;         
        else 
            finish = local_next_comm - 1;         
*/

      }
    else if (pre_dir == DOWN_DIR)
      {
        if 
          ((pos!=ocss_info[car].NCF) &&
          ((ocss_info[car].f_door_state==DOORS_CLOSING) ||
          ((ocss_info[car].f_door_state==DOORS_CLOSED) &&
           (ocss_info[car].mov_dir==NO_DIR) &&
           (ocss_info[car].NCF==ocss_info[car].actual_pos))))
           {
             finish = local_next_comm;
           } 
       else 
             finish = local_next_comm + 1;         
    }


/* DBD-honma TestInProgress  08Mar04 */
     if ((*e2p_DbdAssignEnable != 0) && (pos == ocss_info[car].NCF) && 
         (ocss_info[car].actual_pos == ocss_info[car].NCF) &&
         ((ocss_info[car].f_door_state==DOORS_CLOSED)||(ocss_info[car].f_door_state==DOORS_CLOSING))) 
       {
         if (ocss_info[car].target_pos > ocss_info[car].actual_pos)
            localDbdNcf++;
         else if (ocss_info[car].target_pos < ocss_info[car].actual_pos)
            localDbdNcf--; 
       }

    /* Search for calls assigned to the car which would be */
    /* delayed by this new call, if the car were to answer */
    /* this new call first.  Check each call to determine  */
    /* if it would become elderly.                         */
    /* --------------------------------------------------- */
    if (call_dir == UP_DIR)
    {
       if (pre_dir == UP_DIR)
       { // call_dir:UP  pre_dir:UP
         if (local_next_comm <= pos)
         { 
             //DBD25Feb04
             //if (*e2p_DbdAssignEnable != 0)
             if ((*e2p_DbdAssignEnable != 0) && (*e2p_DbdPwtTerminalType != 0))
               highest_call_pos = ads_HighestPosition(car,local_next_comm,dstPos,TRUE);      
               //TestInProgress highest_call_pos = ads_HighestPosition(car,localDbdNcf,localDbdNcf,TRUE);      
             else
              highest_call_pos = ocss_info[car].top_pos;      

             max_elderly = crisppwt_find_elderly (car,
                                                  pos + 1,
                                                  //DBD ocss_info[car].top_pos,
                                                  highest_call_pos,
                                                  (S_UHC+S_CC+E_CC),
                                                  pre_dir,
                                                  max_elderly,
                                                  TRUE,
                                                  distance3,
                                                  TRUE,dstPos,affectPwtByDst, /*DbdFlag,dstPos */
                                                  spareCap);
              //DBD-honma spcap
              *spareCap = 0;

             //DBD25Feb04
             //if (*e2p_DbdAssignEnable != 0)
             if ((*e2p_DbdAssignEnable != 0) && (*e2p_DbdPwtTerminalType != 0))
               lowest_call_pos = ads_LowestPosition(car,local_next_comm,pos,FALSE);      
               //local_next_comm,pos OK??
             else
              {
           
                temp_bottom_cc_flag = asslib_rrt_hc_assigned (car,
                                                              ocss_info[car].top_pos,
                                                              ocss_info[car].bottom_pos,
                                                              S_DHC); 

               if (temp_bottom_cc_flag > 0)
                 lowest_call_pos = ocss_info[car].bottom_pos; 
               else
                 lowest_call_pos = ads_LowestPosition(car,local_next_comm,pos,FALSE);      
              }
             

   //          if (temp_bottom_cc_flag > 0)
   //          DBD 
             if (1) 
             {
               max_elderly = crisppwt_find_elderly (car,
                                                    highest_call_pos,
                                                    lowest_call_pos,
                                               //     ocss_info[car].top_pos,
                                               //     ocss_info[car].bottom_pos,
                                                    S_DHC,
                                                    pre_dir,
                                                    max_elderly,
                                                    TRUE,
                                                    distance3,
                                                    FALSE,255,affectPwtByDst, /*DbdFlag,dstPos*/
                                                    spareCap);
               max_elderly = crisppwt_find_elderly (car,
                                                    lowest_call_pos,
                                                 //      ocss_info[car].bottom_pos,
                                                       finish,
                                                       S_UHC,
                                                       pre_dir,
                                                       max_elderly,
                                                       FALSE,
                                                       distance3,
                                                       FALSE,255,affectPwtByDst, /*DbdFlag,dstPos*/
                                                       spareCap);
             }
             else
             {

               lowest_call_pos = ocss_info[car].bottom_pos;
               while ((lowest_call_pos < local_next_comm) &&
                      (lowest_call_pos < pos) &&
                      ((ocss_info[car].assigned_calls[lowest_call_pos] & (S_CC+S_HC+E_CC+E_HC)) == 0))
                      lowest_call_pos++;


               max_elderly = crisppwt_find_elderly (car,
                                                    highest_call_pos,
                                                //    ocss_info[car].top_pos,
                                                    lowest_call_pos,
                                                    S_DHC,
                                                    pre_dir,
                                                    max_elderly,
                                                    TRUE,
                                                    distance3,
                                                    FALSE,255,affectPwtByDst, /*DbdFlag,dstPos*/
                                                    spareCap);
               max_elderly = crisppwt_find_elderly (car,
                                                    lowest_call_pos,
                                                    finish,
                                                    S_UHC,
                                                    pre_dir,
                                                    max_elderly,
                                                    FALSE,
                                                    distance3,
                                                    FALSE,255,affectPwtByDst, /*DbdFlag,dstPos*/
                                                    spareCap);

             }

         } /* ocss_info[car].NCF < pos */
       else
         {
         // call_dir:UP  pre_dir:UP

              max_elderly = crisppwt_find_elderly (car,
                                                   pos + 1,
                                                   finish,
                                                   S_UHC,
                                                   pre_dir,
                                                   max_elderly,
                                                   FALSE,
                                                   distance3,
                                                   FALSE,dstPos,affectPwtByDst, /*DbdFlag,dstPos*/
                                                   spareCap);
         } /* ocss_info[car].NCF >= pos */
       } /* pre_dir == UP_DIR */
     else if (pre_dir == DOWN_DIR)
      {
         // call_dir:UP  pre_dir:DOWN

          //DBD27Feb04
          //if (*e2p_DbdAssignEnable != 0)
          if ((*e2p_DbdAssignEnable != 0) && (*e2p_DbdPwtTerminalType != 0))
             highest_call_pos = ads_HighestPosition(car,local_next_comm,dstPos,FALSE);      
          else
             highest_call_pos = ocss_info[car].top_pos;      

          max_elderly = crisppwt_find_elderly (car,
                                               pos + 1,
                                               highest_call_pos,
                                              // ocss_info[car].top_pos,
                                               S_UHC,
                                               pre_dir,
                                               max_elderly,
                                               TRUE,
                                               distance3,
                                               FALSE,dstPos,affectPwtByDst, /*DbdFlag,dstPos*/
                                               spareCap);
          //DBD-honma spcap
          *spareCap = 0;
     
          max_elderly = crisppwt_find_elderly (car,
                                              // ocss_info[car].top_pos,
                                               highest_call_pos,
                                               finish,
                                               S_DHC,
                                               pre_dir,
                                               max_elderly,
                                               FALSE,
                                               distance3,
                                               FALSE,255,affectPwtByDst, /*DbdFlag,dstPos*/
                                               spareCap);
       } /* pre_dir == DOWN_DIR */
    } /* call_dir == UP_DIR */
  else /* call_dir == DOWN_DIR */
    {
       if (pre_dir == DOWN_DIR)
       {
         if (local_next_comm >= pos)
         {
            // call_dir:DOWN  pre_dir:DOWN

             //DBD25Feb04
             //if (*e2p_DbdAssignEnable != 0)
             if ((*e2p_DbdAssignEnable != 0) && (*e2p_DbdPwtTerminalType != 0))
               lowest_call_pos = ads_LowestPosition(car,local_next_comm,dstPos,TRUE);      
             else
             lowest_call_pos = ocss_info[car].bottom_pos;      

             max_elderly = crisppwt_find_elderly (car,
                                                  pos - 1,
                                           //       ocss_info[car].bottom_pos,
                                                  lowest_call_pos,
                                                  (S_DHC+S_CC+E_CC),
                                                  pre_dir,
                                                  max_elderly,
                                                  TRUE,
                                                  distance3,
                                                  TRUE,dstPos,affectPwtByDst, /*DbdFlag,dstPos */
                                                  spareCap);
              //DBD-honma spcap
              *spareCap = 0;

             //DBD25Feb04
             //if (*e2p_DbdAssignEnable != 0)
             if ((*e2p_DbdAssignEnable != 0) && (*e2p_DbdPwtTerminalType != 0))
               highest_call_pos = ads_HighestPosition(car,local_next_comm,pos,FALSE); 
               //local_next_comm,pos OK??
             else
              {
                 temp_top_cc_flag = asslib_rrt_hc_assigned (car,
                                                        ocss_info[car].bottom_pos,
                                                        ocss_info[car].top_pos,
                                                        S_UHC);

               if (temp_top_cc_flag > 0)
                 highest_call_pos = ocss_info[car].top_pos; 
               else
                 highest_call_pos = ads_HighestPosition(car,local_next_comm,pos,FALSE);      
              }



      //       if (temp_top_cc_flag > 0)
      //       DBD
             if (1)
             {
               max_elderly = crisppwt_find_elderly (car,
                                                    lowest_call_pos,
                                                    highest_call_pos,
                                                    S_UHC,
                                                    pre_dir,
                                                    max_elderly,
                                                    TRUE,
                                                    distance3,
                                                    FALSE,255,affectPwtByDst, /*DbdFlag,dstPos*/
                                                    spareCap);
               max_elderly = crisppwt_find_elderly (car,
                                                    highest_call_pos,
                                                    finish,
                                                    S_DHC,
                                                    pre_dir,
                                                    max_elderly,
                                                    FALSE,
                                                    distance3,
                                                    FALSE,255,affectPwtByDst, /*DbdFlag,dstPos*/
                                                    spareCap);
             }
             else
             {
               // call_dir:DOWN  pre_dir:DOWN

               highest_call_pos = ocss_info[car].top_pos;
               while ((highest_call_pos > local_next_comm) &&
                      (highest_call_pos > pos) &&
                      ((ocss_info[car].assigned_calls[highest_call_pos] &(S_CC+S_HC+E_CC+E_HC) ) == 0) ) 
                  highest_call_pos--;


               max_elderly = crisppwt_find_elderly (car,
                                                    ocss_info[car].bottom_pos,
                                                    highest_call_pos,
                                                    S_UHC,
                                                    pre_dir,
                                                    max_elderly,
                                                    TRUE,
                                                    distance3,
                                                    FALSE,255,affectPwtByDst, /*DbdFlag,dstPos*/
                                                    spareCap);
               max_elderly = crisppwt_find_elderly (car,
                                                    highest_call_pos,
                                                    finish,
                                                    S_DHC,
                                                    pre_dir,
                                                    max_elderly,
                                                    FALSE,
                                                    distance3,
                                                    FALSE,255,affectPwtByDst, /*DbdFlag,dstPos*/
                                                    spareCap);
             }
         } /* ocss_info[car].NCF > pos */
       else
         {
            // call_dir:DOWN  pre_dir:DOWN

             max_elderly = crisppwt_find_elderly (car,
                                                  pos - 1,
                                                  finish,
                                                  S_DHC,
                                                  pre_dir,
                                                  max_elderly,
                                                  FALSE,
                                                  distance3,
                                                  FALSE,dstPos,affectPwtByDst, /*DbdFlag,dstPos*/
                                                  spareCap);
         } /* ocss_info[car].NCF <= pos */
       } /* pre_dir == DOWN_DIR */
     else if (pre_dir == UP_DIR)
       {
          // call_dir:DOWN  pre_dir:UP

          //DBD27Feb04
          //if (*e2p_DbdAssignEnable != 0)
          if ((*e2p_DbdAssignEnable != 0) && (*e2p_DbdPwtTerminalType != 0))
             lowest_call_pos = ads_LowestPosition(car,local_next_comm,dstPos,FALSE);      
          else
             lowest_call_pos = ocss_info[car].bottom_pos;      

          max_elderly = crisppwt_find_elderly (car,
                                               pos - 1,
                                           //    ocss_info[car].bottom_pos,
                                               lowest_call_pos,
                                               S_DHC,
                                               pre_dir,
                                               max_elderly,
                                               TRUE,
                                               distance3,
                                               FALSE,dstPos,affectPwtByDst, /*DbdFlag,dstPos*/
                                               spareCap);
          //DBD-honma spcap
          *spareCap = 0;

          max_elderly = crisppwt_find_elderly (car,
                                           //    ocss_info[car].bottom_pos,
                                               lowest_call_pos,
                                               finish,
                                               S_UHC,
                                               pre_dir,
                                               max_elderly,
                                               FALSE,
                                               distance3,
                                               FALSE,255,affectPwtByDst, /*DbdFlag,dstPos*/
                                               spareCap);
       }/* pre_dir == UP_DIR */
    } /* call_dir == DOWN_DIR */
    

    /* If call is a temporary call, delete it from system */
    /* -------------------------------------------------- */
    if (previously_assigned == FALSE)
    {
       /* honma
       ocss_info[car].assigned_calls[pos] &= ~mask;
       */
       dummy_assigned_calls[car][pos] &= ~mask;

       /* Enable transmission of car assignments */
       /* -------------------------------------- */
       ica_lock_out_assignments_flag = FALSE;
    }

    //DBD-honma spcap
    if (crisppwt_findPwtFlag == FALSE) *spareCap = 0;
    else *spareCap = crisppwt_maxSpareCap;

    return(max_elderly);
} /* end of ica_calculate_pwt */


/*********************************************************************
; Function Name: crisppwt_find_elderly
;
; Determines maximum waiting time for specified floors.
;
;********************************************************************/
INT_T crisppwt_find_elderly(BYTE_T car, BYTE_T start, BYTE_T stop,
                     INT_T mask, BYTE_T pre_dir, INT_T max_elderly,
                     BYTE_T cond, BYTE_T *distance3, FLAG_T dbdFlag,BYTE_T dstPos,BYTE_T *affectPwtByDst,
                     BYTE_T *spareCap)
{
	BYTE_T upper, lower, floor, start_pos, start_distance;
	FLAG_T ignore_coinc_flag;
	BYTE_T direction;
	BYTE_T distance;
	INT_T  f_eld,r_eld,tmp_eld_wait;
	INT_T  e_mask;
	BYTE_T riser;
	BYTE_T dest_mask,dest_dir;

    riser = infbld_SgoGroupCar[car];

    /* First, make sure starting position doesn't exceed building bounds */
    /* ----------------------------------------------------------------- */
    if (start > ocss_info[car].top_pos || stop > ocss_info[car].top_pos)
	{
		return (max_elderly);
	}

    if (((start == stop)) ||
        (((start < stop)  && ((mask & S_UHC) != 0)) ||
		((start > stop)  && ((mask & S_DHC) != 0))))    
	{
		floor = start;
		start_pos = start;    
		
        //DBD
        if ((mask & S_UHC) != 0)
		{
            e_mask = E_UHC;
            dest_mask = DBD_SUCC;
            dest_dir  = UP_DIR;
		}
        else if ((mask & S_DHC) != 0) 
		{
            e_mask = E_DHC;
            dest_mask = DBD_SDCC;
            dest_dir  = DOWN_DIR;
		} 
		
        if ((mask & S_UHC) != 0)
		{
            for (floor=start; floor<=stop; floor++)
			{
				f_eld = 0;
				r_eld = 0;
				
				//DBD-honma spcap 22Mar04
				if ((ocss_info[car].assigned_calls[floor] & mask & S_CC) != 0)
				{
					if (*spareCap > asslib_DbdCarCallCnts[car][floor]) 
						*spareCap -= asslib_DbdCarCallCnts[car][floor];
					else                    
						*spareCap = 0;
				}
				
				if (((asslib_DbdExpectCalls[car][floor] & dest_mask) != 0) &&
					((dbdFlag == FALSE) || 
					((dbdFlag) && (asslib_ValidExpectCalls(car,dest_dir,floor)))))
					// shs : simplify logic ~A|(A&B)=~A|B
				{
					
					if (*spareCap > asslib_DbdExpectCallCnts[car][dest_dir][floor]) 
						*spareCap -= asslib_DbdExpectCallCnts[car][dest_dir][floor];
					else                    
						*spareCap = 0;
					
				}
				
				if ((ocss_info[car].assigned_calls[floor] & (S_UHC+E_UHC)) != 0) 
				{
					*spareCap += asslib_DbdAssignCallDirCnts[car][dest_dir][floor];
					crisppwt_maxSpareCap = max(crisppwt_maxSpareCap,*spareCap);
					crisppwt_findPwtFlag = TRUE;
				}
				
				
				if ((ocss_info[car].assigned_calls[floor] & (S_UHC+E_UHC)) != 0)
				{
					if (temp_distance != 0)
					{
						ica_pwt_distance += (floor - start_pos) + temp_distance;
						*distance3 += (floor - start_pos) + temp_distance;
						temp_distance = 0;
					}
					else
					{
						ica_pwt_distance += floor - start_pos;
						*distance3  += floor - start_pos;
					}
					
					start_pos = floor;
					
					if ((ocss_info[car].assigned_calls[floor] & S_FUHC) != 0)
					{
						//DBD-honma 04Apr04
						if (*e2p_DbdAssignEnable == 0)
						{
                            f_eld = bld_info.fuhc[riser][floor].reg_elapsed + ica_rat_pwt + 
								assign_DistanceUnsure[ica_pwt_distance];
						}
						else 
						{
                            f_eld = bld_info.fuhc[riser][floor].reg_elapsed_car[car] + ica_rat_pwt + 
								assign_DistanceUnsure[ica_pwt_distance];
						}
						
						//DBD 20Feb04
						if (((dstPos <= ocss_info[car].top_pos) && (dstPos < floor)) ||
							(dstPos == 255))
							*affectPwtByDst += 1;
						
#ifdef WAITIND
						//waitInd
						if (*e2p_WaitIndicatorEnable != 0)
						{
							if (asslib_WaitIndLevel[UP_DIR][floor] == WaitThreeLevel)
                                f_eld = 4000;
						}
#endif
						
#ifdef CAGS
						afterhallcalls[car]++;
#endif
					}
					else
						f_eld = 0;
					
#ifndef CAGS
					//dojima
					if (*e2p_WcsElderlyType != 0)
					{
						if ((ocss_info[car].assigned_calls[floor] & E_FUHC) != 0)
							r_eld = bld_info.e_fuhc[riser][floor].reg_elapsed + ica_rat_pwt +
							assign_DistanceUnsure[ica_pwt_distance];
						else
							r_eld = 0;
					} 
					else //dojima
					{
						if ((ocss_info[car].assigned_calls[floor] & S_RUHC) != 0)
                            r_eld = bld_info.ruhc[riser][floor].reg_elapsed + ica_rat_pwt +
							assign_DistanceUnsure[ica_pwt_distance];
						else
                            r_eld = 0;
					}
#else
					if ((ocss_info[car].assigned_calls[floor] & S_RUHC) != 0)
                        r_eld = bld_info.ruhc[riser][floor].reg_elapsed + ica_rat_pwt +
						assign_DistanceUnsure[ica_pwt_distance];
					else
                        r_eld = 0;
#endif
					/* choose bigger elderly time */
					if (f_eld >= r_eld)
                        tmp_eld_wait = f_eld;
					else
                        tmp_eld_wait = r_eld;
					
					max_elderly = max(max_elderly,tmp_eld_wait);
					
					if (floor != stop)
					{
						if ((ocss_info[car].assigned_calls[floor]  & E_UHC) != 0)
						{
							ica_rat_pwt += RRT_WCS_STOP;
							ica_rat_pwt += RRT_SHORT_RUN;
						}
                        //DBD   else
						else if ((ocss_info[car].assigned_calls[floor] & S_UHC) != 0)
						{
							ica_rat_pwt += RRT_HALL_STOP;
							ica_rat_pwt += RRT_SHORT_RUN;
						} 
					}
				}/* ((ocss_info[car].assigned_calls[floor] & mask & S_UHC .. */
				//DBD
				else if (((ocss_info[car].assigned_calls[floor] & mask & (S_CC+E_CC)) != 0) ||
					(((asslib_DbdExpectCalls[car][floor] & dest_mask) != 0) &&    
					((dbdFlag == FALSE) || 
					((dbdFlag == TRUE) && (asslib_ValidExpectCalls(car,dest_dir,floor))))) ||
					((floor == stop) && (cond == TRUE ))
					||  (dstPos == floor))  //25Feb04
				{
					if (floor != stop)
					{
						if ((ocss_info[car].assigned_calls[floor] & E_CC) != 0)
						{
							ica_rat_pwt += RRT_WCS_STOP;
							ica_rat_pwt += RRT_SHORT_RUN;
						}
						else 
						{
							ica_rat_pwt += RRT_CAR_STOP;
							ica_rat_pwt += RRT_SHORT_RUN;
						}
					}
					else
					{  
						temp_distance += floor - start_pos;
						start_pos = floor;
						
						/* DBD */
						if (((ocss_info[car].assigned_calls[floor] & E_HC) == 0) &&
							((ocss_info[car].assigned_calls[floor]  & S_HC) == 0))
						{	// no hall call
							ica_rat_pwt += RRT_CAR_STOP;
							ica_rat_pwt += RRT_SHORT_RUN;
						}
						//else if (floor == ocss_info[car].top_pos)
						//for DBD case  26Feb04
						else if (floor == stop)
						{
							/*DBD  special case */
							//if terminal postion has hallcall, then add HallCall penalty
							//in case of DBD with AllFloorsKeypad, is it necessasry???
							ica_rat_pwt += RRT_HALL_STOP;
							ica_rat_pwt += RRT_SHORT_RUN;
						}
					}
				} /* car call */
				else /* no calls */
				{
					ica_rat_pwt += RRT_FULL_RUN;
                   }
               } /* for (floor=start .... */
          }/* if ((mask & S_UHC) != 0) */   
       
        else if ((mask & S_DHC) != 0)
          {
            for (floor=start; floor>=stop&&floor>=ocss_info[car].bottom_pos&&floor<=ocss_info[car].top_pos; 
                 floor--)
               {
                  f_eld = 0;
                  r_eld = 0;

                   //DBD-honma spcap 22Mar04
                   if ((ocss_info[car].assigned_calls[floor] & mask & S_CC) != 0)
                   {
                     if (*spareCap > asslib_DbdCarCallCnts[car][floor]) 
                          *spareCap -= asslib_DbdCarCallCnts[car][floor];
                     else                    
                          *spareCap = 0;
                   }

                   if (((asslib_DbdExpectCalls[car][floor] & dest_mask) != 0) &&
                      ((dbdFlag == FALSE) || 
                      ((dbdFlag) && (asslib_ValidExpectCalls(car,dest_dir,floor)))))
                   {

                       if (*spareCap > asslib_DbdExpectCallCnts[car][dest_dir][floor]) 
                           *spareCap -= asslib_DbdExpectCallCnts[car][dest_dir][floor];
                       else                    
                          *spareCap = 0;

                   }

                   if ((ocss_info[car].assigned_calls[floor] & (S_DHC+E_DHC)) != 0) 
                   {
                     *spareCap += asslib_DbdAssignCallDirCnts[car][dest_dir][floor];
                     crisppwt_maxSpareCap = max(crisppwt_maxSpareCap,*spareCap);
                     crisppwt_findPwtFlag = TRUE;
                   }
          
                  if ((ocss_info[car].assigned_calls[floor] & (S_DHC+E_DHC)) != 0)
                    {
                      if (temp_distance != 0)
                        {
                           ica_pwt_distance += (start_pos - floor) + temp_distance;
                           *distance3 += (start_pos - floor) + temp_distance;
                           temp_distance = 0;
                        }
                      else
                        {
                           ica_pwt_distance += start_pos - floor;
                           *distance3 += start_pos - floor;
                        }

                      start_pos = floor;

                      if ((ocss_info[car].assigned_calls[floor] & S_FDHC) != 0)
                        {

                          //DBD-honma 04Apr04
                          if (*e2p_DbdAssignEnable == 0)
                          {
                            f_eld = bld_info.fdhc[riser][floor].reg_elapsed + ica_rat_pwt + 
                               assign_DistanceUnsure[ica_pwt_distance];
                          }
                          else 
                          {
                            f_eld = bld_info.fdhc[riser][floor].reg_elapsed_car[car] + ica_rat_pwt + 
                               assign_DistanceUnsure[ica_pwt_distance];
                          }

                          //DBD 20Feb04
                          if (((dstPos <= ocss_info[car].top_pos) && (dstPos > floor)) ||
                              (dstPos == 255))
                             *affectPwtByDst += 1;

#ifdef WAITIND
                          //waitInd
                          if (*e2p_WaitIndicatorEnable != 0)
                            {
                              if (asslib_WaitIndLevel[DOWN_DIR][floor] == WaitThreeLevel)
                                f_eld = 4000;
                            }
#endif

#ifdef CAGS
        afterhallcalls[car]++;
#endif
                        }
                      else
                        f_eld = 0;
#ifndef CAGS
                      //dojima      
                      if (*e2p_WcsElderlyType != 0)
                        {
                          if ((ocss_info[car].assigned_calls[floor] & E_FDHC) != 0)
                             r_eld = bld_info.e_fdhc[riser][floor].reg_elapsed + ica_rat_pwt +
                                     assign_DistanceUnsure[ica_pwt_distance];
                          else
                            r_eld = 0;
                        }
                      else //dojima
                        {
                          if ((ocss_info[car].assigned_calls[floor] & S_RDHC) != 0)
                             r_eld = bld_info.rdhc[riser][floor].reg_elapsed + ica_rat_pwt +
                                     assign_DistanceUnsure[ica_pwt_distance];
                          else
                            r_eld = 0;
                        }
#else
                      if ((ocss_info[car].assigned_calls[floor] & S_RDHC) != 0)
                         r_eld = bld_info.rdhc[riser][floor].reg_elapsed + ica_rat_pwt +
                                 assign_DistanceUnsure[ica_pwt_distance];
                      else
                         r_eld = 0;
#endif

                      /* choose bigger elderly time */
                      if (f_eld >= r_eld)
                        tmp_eld_wait = f_eld;
                      else
                        tmp_eld_wait = r_eld;

                      max_elderly = max(max_elderly,tmp_eld_wait);

                      if (floor != stop)
                        {
                          if ((ocss_info[car].assigned_calls[floor] & E_DHC) != 0)
                            {
                               ica_rat_pwt += RRT_WCS_STOP;
                               ica_rat_pwt += RRT_SHORT_RUN;
                            }
                           //DBD      else 
                          else if ((ocss_info[car].assigned_calls[floor] & S_DHC) != 0)
                            {
                               ica_rat_pwt += RRT_HALL_STOP;
                               ica_rat_pwt += RRT_SHORT_RUN;
                             }
                        }
                    }/* ((ocss_info[car].assigned_calls[floor] & mask & S_DHC .. */
                  //DBD
                  else if (((ocss_info[car].assigned_calls[floor] & mask & (S_CC+E_CC)) != 0) ||
                           (((asslib_DbdExpectCalls[car][floor] & dest_mask) != 0) &&    
                            ((dbdFlag == FALSE) || 
                            ((dbdFlag == TRUE) && (asslib_ValidExpectCalls(car,dest_dir,floor))))) ||
                            ((floor == stop) && (cond == TRUE ))
                        ||  (dstPos == floor))  //25Feb04
                    {
                      if (floor != stop)
                        {
                          if ((ocss_info[car].assigned_calls[floor] & E_CC) != 0)
                            {
                               ica_rat_pwt += RRT_WCS_STOP;
                               ica_rat_pwt += RRT_SHORT_RUN;
                            }
                           else 
                            {
                               ica_rat_pwt += RRT_CAR_STOP;
                               ica_rat_pwt += RRT_SHORT_RUN;
                            }
                        }
                      else
                        {
                           temp_distance += start_pos - floor;
                           start_pos = floor;

                           if (((ocss_info[car].assigned_calls[floor] & E_HC) == 0) &&
                              ((ocss_info[car].assigned_calls[floor]  & S_HC) == 0))
                             {
                                ica_rat_pwt += RRT_CAR_STOP;
                                ica_rat_pwt += RRT_SHORT_RUN;
                             }
                           //DBD else if (floor == ocss_info[car].bottom_pos)
                           //for DBD case    26Feb04
                           else if (floor == stop)
                             {
                               /*DBD  special case */
                               //if terminal postion has hallcall, then add HallCall penalty
                               //in case of DBD with AllFloorsKeypad, is it necessasry???
                                ica_rat_pwt += RRT_HALL_STOP;
                                ica_rat_pwt += RRT_SHORT_RUN;
                             }
                        }
                    } /* car call */
                  else /* no calls */
                    {
                       ica_rat_pwt += RRT_FULL_RUN;
                    }
     
               } /* for (floor=start .... */
          }/* if ((mask & S_DHC) != 0) */   
      }/* valid start, stop received */    
    
    max_elderly = min (max_elderly, RRT_MAX);
    return (max_elderly);
        
} /* end of crisppwt_find_elderly */




/**************************************************************************
;---------------------------------------------------------------------------
;  L O C A L   C R I S P   P W T   F U N C T I O N S
;---------------------------------------------------------------------------
;***************************************************************************
;
;
;**************************************************************************/

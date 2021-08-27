/* U10 Project: @(#)infocs.c	2.2	3/23/95	14:22:51 */
/**************************************************************************
; File Name: INFOCS.C - OCSS Data Handler
;
; File Description:
; This file contains modules that handle ocss related data. These
; files are reentrant.
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
; Revision History:
;
; SCN                   Author                      Date
; ------------------------------------------------------------------------
; AAA-----AAA           Mark A. Ross              14-NOV-91
; - Created Program.
;
; AAA-----AAA           Jeremy Kezer              12 April 1993
; - Renamed ring1_T3_tout and ring2_T3_tout to ring1_T0_tout and 
;   ring2_T0_tout.
;
; AAA-----AAA           Jeremy Kezer              9 July 1993
; - Modify code to be more consistant with ICSS2 coding standards.
;
; A1530133BAA           Jeremy Kezer              31 August 1993
; - Added cars_available_count and cars_available_mask functions.
;
; A1630133BAA           Jeremy Kezer              29 October 1993
; - Changed return value of above routines to match actual return type.
;
; A1630133BAA           Jeremy Kezer              29 October 1993
; - Updated initialization routine to have all variables.
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;  J1830203AAA           Hideyuki Honma                  08 June 1995
;  - SCR 30203-5002: Support Dual Up Peak
;  - SCR 30203-5035: Support Door Re-Open feature for dispatching hall
;  - SCR 30203-5036: Take into account Nudging state of door
;
;  J1930203AAA           Hideyuki Honma                  27 June 1995
;  - SCR 30203-5027: Multi Lobby Position
;            
;  J2130203AAA           Hideyuki Honma                     31 Nov 1995
;  - SCR 30203-5029: Dispatch during Channeling
;           
;  AAB-30203-3AA         Mark Ross                     21 Dec 1995
;  - SCR 30203-228: OTI SCR 5029.  Dispatch during Channeling       
;
;  AAB-30203-4AA         Jannah Stanley               23 Jan 1996
;  - SCR 30203-256: Initialize new array, hl_active, added to OCSS structure 
;
;  JAB-30203-AAE         Hideyuki Honma              17 Jan 1999
;  - SCR 30203-5xxx:   Add oldFinalLoad,d_passengers,inCarPassengers for V-MIT 
;**************************************************************************/
#include "stdafx.h"
#include "common.h"

/**************************************************************************
;  Public Declarations
;*************************************************************************/
#define EXTERN extern   /* declare public variables */
#include "e2p.h"
#undef EXTERN

/**************************************************************************
;  Public Definitions
;*************************************************************************/
#define EXTERN          /* define public variables */
#include "infocs.h"
#undef EXTERN

/**************************************************************************
;  Local Definitions
;*************************************************************************/
/* None */


/**************************************************************************
;  Public Functions
;*************************************************************************/

/*********************************************************************
; Name: O_init - Initialize ocss data structure
;
; Description: Initialize ocss data for one ocss.
;
; Parameters: Pointer to a particular ocss structure.
;********************************************************************/
void    O_init (OCSS_T *O_ptr) 
   
{ 
  BYTE_T i;   


   for (i=0; i<JICA_FLOORS; i++) 
   {
      O_ptr->allowed_mask[i] = 0;
      O_ptr->assigned_calls[i] = 0;
      O_ptr->hl_active[i] = 0;
   }

   O_ptr->bottom_pos = 0;
   O_ptr->lobby_pos = JICA_FLOORS;
   O_ptr->top_pos = JICA_FLOORS;
   O_ptr->load_state = 0;
   O_ptr->pre_dir = 0;
   O_ptr->mov_dir = 0;
   O_ptr->old_mov_dir = 0;
   O_ptr->actual_pos = JICA_FLOORS;
   O_ptr->old_actual_pos = 0;
   O_ptr->lastStopPos = 255;
   O_ptr->NCF = JICA_FLOORS;
   O_ptr->f_door_state = 0;
   O_ptr->r_door_state= 0;
   O_ptr->load_wt_counts= 0;
   O_ptr->d_min = 0;
   O_ptr->b_max = 0;
   O_ptr->oldFinalLoad = 0;
   O_ptr->b_passengers = 0;
   O_ptr->d_passengers = 0;
   O_ptr->inCarPassengers = 0;
   O_ptr->incar_p = 0;
   /* chage value 1S--> 20S */
   O_ptr->ring1_T0_tout = TIM_20S;
   O_ptr->ring2_T0_tout = TIM_1S;
   O_ptr->ring12_T0_tout = TIM_2S;
   O_ptr->tll_value = TIM_1M;
   O_ptr->tll_tout = TIM_1M;
   O_ptr->tll_status = FALSE;
   O_ptr->old_tll_status = FALSE;
   //update 99/2/18   O_ptr->car_capacity = 0;
   O_ptr->car_capacity = 777;
   O_ptr->opmode = NAV;
   O_ptr->source_ring = RING1;
   O_ptr->ez_bottom_pos = 0;
   O_ptr->ez_top_pos = 0;
   O_ptr->ez_type = 0;
   O_ptr->car_in_ez_flag = FALSE;
   O_ptr->call_allowed_mask = 0;
   O_ptr->mgset_status = 0;
   O_ptr->atsummary = 0;
   O_ptr->upsummary = 0;
   O_ptr->dnsummary = 0;
   O_ptr->allsummary = 0;
   O_ptr->stop_type = 0;
   O_ptr->stop_pos = 0;
   O_ptr->stop_status = 0;
   O_ptr->decel_flag = 0;
   O_ptr->motion_status = 0;
   //update 99/2/16 for Parking  O_ptr->target_pos = 0;
   O_ptr->target_pos = 255;
   O_ptr->crowd_last_pos = 0;
   O_ptr->crowd_last_direction = 0;
   O_ptr->reopen_cnt = 255;
   O_ptr->nudging_detect = FALSE;

   O_ptr->second_lobby_pos = 255;

   ocss_mit_clock_flag = 0;
   ocss_dup_clock_flag = 0;
   ocss_mot_status_flag = 0;
   ocss_mit_status_flag = 0;
   ocss_chn_status_flag = 0;
 
   ocss_2nd_lobby_flag = FALSE;
   ocss_2nd_lobby_type = 0;

}

/*********************************************************************
; Function Name: cars_available_count
;
; Description: Returns the number of cars that are in normal
; operation, within the group.
;
; Parameters: none
;
;********************************************************************/

BYTE_T cars_available_count(void)
   
{
 
 INT_T car;
 BYTE_T count;
 OCSS_T *ocss_ptr;

   /* D E T E R M I N E   N U M B E R   O F   C A R S */
   /* ----------------------------------------------- */
   count = 0;
   for (car=CAR1; car<=CAR8; car++)
   {
      ocss_ptr = &ocss_info[car];
      if (ocss_ptr->opmode == IDL || ocss_ptr->opmode == PRK ||
	  ocss_ptr->opmode == NOR || ocss_ptr->opmode == MIT ||
	  ocss_ptr->opmode == LNS || ocss_ptr->opmode == CHN)
      {
	 count++;
      }
      
   }
   return(count);
}


/*********************************************************************
; Function Name: cars_available_mask
;
; Description: Returns a bitmask with the appropriate bit set for
; each car that is in normal operation.
;
; Parameters: none
;
;********************************************************************/

BYTE_T cars_available_mask(void)
   
{
 
 INT_T car;

 BYTE_T mask;

 OCSS_T *ocss_ptr;

   /* D E T E R M I N E   M A S K S   O F   C A R S */
   /* --------------------------------------------- */
   mask = 0;
   for (car=CAR1; car<=CAR8; car++)
   {
      ocss_ptr = &ocss_info[car];
      if (ocss_ptr->opmode == IDL || ocss_ptr->opmode == PRK ||
	  ocss_ptr->opmode == NOR || ocss_ptr->opmode == MIT ||
	  ocss_ptr->opmode == LNS || ocss_ptr->opmode == CHN)
      {
	 mask |= 0x01 << (car-1);
      }
      
   }
   
   return(mask);
}

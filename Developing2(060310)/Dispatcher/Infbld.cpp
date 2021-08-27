

/* U10 Project: @(#)infbld.c	2.4	3/23/95	14:22:46 */
/**************************************************************************
; File Name: INFBLD.C - Building Data Handler
;
; File Description:
; This file contains modules that handle data for a given instance
; of a building.  These files are reentrant.
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
; AAA-----AAA           Jeremy Kezer              22 January 1993
; - Replaced "bld_info.bot" with "bld_info.bottom".
;
; AAA-----AAA           Jeremy Kezer              24 June 1993
; - Add crowd-related variables to initialization list.
;
; AAA-----AAA           Jeremy Kezer              9 July 1993
; - Modify code to be more consistant with ICSS2 coding standards.
;
; A1630133BAA           Jeremy Kezer              16 November 1993
; - Altered variable names for clarity.
;
; A1130133CAA           Jeremy Kezer              23 March 1994
; - Removed bld_info.num_landings; unused.
;
;  A1530203BAA           Geoff Mochau             14 July 1994
;  - SCR 30203-108  (OECF/OTI Baseline Alignment).
;    Added registration time and hall call count support.
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;  J1830203AAA           Hideyuki Honma                08 June 1995
;  - SCR 30203-5002: Support Dual Up Peak
;
;  J1930203AAA           Hideyuki Honma                27 June 1995
;  - SCR 30203-5027: Multi Lobby Parking
;            
;  J2130203AAA           Hideyuki Honma                     31 Nov 1995
;  - SCR 30203-5029: Dispatch during Channeling
;           
;  AAB-30203-3AA         Mark Ross                     21 Dec 1995
;  - SCR 30203-228: OTI SCR 5029.  Dispatch during Channeling      
;
;*************************************************************************/
#include "stdafx.h"
#include "common.h"

/**************************************************************************
;  Public Declarations
;*************************************************************************/
#define EXTERN extern   /* declare public variables */
#include "infocs.h"
#include "infdes.h"
#undef EXTERN

/**************************************************************************
;  Public Definitions
;*************************************************************************/
#define EXTERN          /* define public variables */
#include "infbld.h"
#undef EXTERN

/**************************************************************************
;  Local Definitions
;*************************************************************************/
/* None */


/**************************************************************************
;  Public Functions
;*************************************************************************/

/*********************************************************************
; Name: B_init - Initialize building data
;
; Description: Initialize all building data. There should be one
; instance of this type of object.
;
; Parameters: NONE
;********************************************************************/
void    B_init (BUILDING_T *B_ptr) 
{
	BYTE_T  i,j,k;
	B_ptr->ocss_lobby_pos = 0;
	B_ptr->second_lobby_pos = 255;
	B_ptr->ads_lobby_pos = 0;
	B_ptr->dup_common_pos = 255;
	B_ptr->top_pos = 0;
	B_ptr->bottom_pos = 0;
	B_ptr->up_ave_reg = 0;
	B_ptr->dn_ave_reg = 0;
	B_ptr->total_ave_reg = 0;
	B_ptr->up_hall_calls = 0;
	B_ptr->dn_hall_calls = 0;
	B_ptr->total_hall_calls = 0;

	for (i=0; i<MAX_LANDINGS; i++)
	{
		//DBD-honma 04Apr04
		for(k=0; k<MAX_OCSS; k++) 
		{
			B_ptr->fuhc[RISER_ONE][i].reg_elapsed_car[k] = 255;
			B_ptr->fdhc[RISER_ONE][i].reg_elapsed_car[k] = 255;
		}

		for (j=0; j<MAX_RISER; j++) //Split 
		{
			B_ptr->fuhc[j][i].low_rsr = RSR_MAX_PLUS_1;
			B_ptr->fuhc[j][i].low_rrt = 0;
			B_ptr->fuhc[j][i].dispatch = FALSE;
			B_ptr->fuhc[j][i].reg_elapsed = 255;    
			B_ptr->fuhc[j][i].reassigned_count = 0;    
			B_ptr->fuhc[j][i].last_boarding_weight = 0;    
			B_ptr->fuhc[j][i].crowd_interval = 0;    
			B_ptr->fuhc[j][i].crowd_current_estimate = 0;    
			B_ptr->fuhc[j][i].crowd_departure_time = 0;    
			B_ptr->fuhc[j][i].crowd_registration_time = 0;    
			B_ptr->fuhc[j][i].crowd_previous_interval = 0;    

			B_ptr->fdhc[j][i].low_rsr = RSR_MAX_PLUS_1;
			B_ptr->fdhc[j][i].low_rrt = 0;
			B_ptr->fdhc[j][i].dispatch = FALSE;
			B_ptr->fdhc[j][i].reg_elapsed = 255;    
			B_ptr->fdhc[j][i].reassigned_count = 0;    
			B_ptr->fdhc[j][i].last_boarding_weight = 0;    
			B_ptr->fdhc[j][i].crowd_interval = 0;    
			B_ptr->fdhc[j][i].crowd_current_estimate = 0;    
			B_ptr->fdhc[j][i].crowd_departure_time = 0;    
			B_ptr->fdhc[j][i].crowd_registration_time = 0;    
			B_ptr->fdhc[j][i].crowd_previous_interval = 0;    

			B_ptr->ruhc[j][i].low_rsr = RSR_MAX_PLUS_1;
			B_ptr->ruhc[j][i].low_rrt = 0;
			B_ptr->ruhc[j][i].dispatch = FALSE;
			B_ptr->ruhc[j][i].reg_elapsed = 255;    
			B_ptr->ruhc[j][i].reassigned_count = 0;    
			B_ptr->ruhc[j][i].last_boarding_weight = 0;    
			B_ptr->ruhc[j][i].crowd_interval = 0;    
			B_ptr->ruhc[j][i].crowd_current_estimate = 0;    
			B_ptr->ruhc[j][i].crowd_departure_time = 0;    
			B_ptr->ruhc[j][i].crowd_registration_time = 0;    
			B_ptr->ruhc[j][i].crowd_previous_interval = 0;    

			B_ptr->rdhc[j][i].low_rsr = RSR_MAX_PLUS_1;
			B_ptr->rdhc[j][i].low_rrt = 0;
			B_ptr->rdhc[j][i].dispatch = FALSE;
			B_ptr->rdhc[j][i].reg_elapsed = 255;    
			B_ptr->rdhc[j][i].reassigned_count = 0;    
			B_ptr->rdhc[j][i].last_boarding_weight = 0;    
			B_ptr->rdhc[j][i].crowd_interval = 0;    
			B_ptr->rdhc[j][i].crowd_current_estimate = 0;    
			B_ptr->rdhc[j][i].crowd_departure_time = 0;    
			B_ptr->rdhc[j][i].crowd_registration_time = 0;    
			B_ptr->rdhc[j][i].crowd_previous_interval = 0;    
			//dojima for WheelChair Call
			B_ptr->e_fuhc[j][i].low_rsr = RSR_MAX_PLUS_1;
			B_ptr->e_fuhc[j][i].low_rrt = 0;
			B_ptr->e_fuhc[j][i].dispatch = FALSE;
			B_ptr->e_fuhc[j][i].reg_elapsed = 255;    
			B_ptr->e_fuhc[j][i].reassigned_count = 0;    
			B_ptr->e_fuhc[j][i].last_boarding_weight = 0;    
			B_ptr->e_fuhc[j][i].crowd_interval = 0;    
			B_ptr->e_fuhc[j][i].crowd_current_estimate = 0;    
			B_ptr->e_fuhc[j][i].crowd_departure_time = 0;    
			B_ptr->e_fuhc[j][i].crowd_registration_time = 0;    
			B_ptr->e_fuhc[j][i].crowd_previous_interval = 0;    

			B_ptr->e_fdhc[j][i].low_rsr = RSR_MAX_PLUS_1;
			B_ptr->e_fdhc[j][i].low_rrt = 0;
			B_ptr->e_fdhc[j][i].dispatch = FALSE;
			B_ptr->e_fdhc[j][i].reg_elapsed = 255;    
			B_ptr->e_fdhc[j][i].reassigned_count = 0;    
			B_ptr->e_fdhc[j][i].last_boarding_weight = 0;    
			B_ptr->e_fdhc[j][i].crowd_interval = 0;    
			B_ptr->e_fdhc[j][i].crowd_current_estimate = 0;    
			B_ptr->e_fdhc[j][i].crowd_departure_time = 0;    
			B_ptr->e_fdhc[j][i].crowd_registration_time = 0;    
			B_ptr->e_fdhc[j][i].crowd_previous_interval = 0;    


			B_ptr->new_hc_mask[j][i] = 0;
			B_ptr->old_hc_mask[j][i] = 0;

		}
	}
}

/*********************************************************************
; Name: G_init - Initialize Group data
;
; Description: Initialize all Group data. There should be one
; instance of this type of object.
;
; Parameters: NONE
;********************************************************************/
void    G_init (GROUP_T *G_ptr) {
  BYTE_T  i,j;


  for (i=0;i<MAX_LANDINGS;i++)
  {
    for (j=0;j<3;j++)
     {
       G_ptr->new_hc_mask[j][i] = 0;
       G_ptr->old_hc_mask[j][i] = 0;
     }
  }

 for (i=0;i<=CAR8;i++)
  {
    Group_No[i] = 0;
    infbld_SgoGroupCar[i] = 0;
    ocss_MitPriority[i] = 15; //15 is NoPriority
    ocss_ParkEnable[i] = FALSE;
    ocss_ParkEnable[i] = TRUE;
  }

 dup_enable_flag = 0;

 infbld_EmsDoubleAssignSignal = 0;
 infbld_EmsPreCrowdSignal = 0;
 infbld_EmsUTurnSignal = 0;
 infbld_EmsAutoLobbyHallSignal = 0;
 infbld_EmsDoorOpenParkSignal = 0;

 //SGO
 infbld_Sgo1EnableFlag = infbld_Sgo2EnableFlag = 0;
 ocss_sgo1_clock_flag = ocss_sgo2_clock_flag = 0;

 //SRO
 infbld_Sro1EnableFlag = 0;
 infbld_SroLowPos = infbld_SroHighPos = 255;

 ems_mit_clock_flag = FALSE;

 rrt_hdcp_hall_stop = ((MIN_HDCP_HALL_DWELL + RRT_HALL_STOP) - 4);
 rrt_hdcp_car_stop  = RRT_CAR_STOP;

 ocss_dpc_clock_flag = 0;
}

/*********************************************************************
; Name: S_init - Initialize Sector data
;
; Description: Initialize all Channeling Sector data. There should be one
; instance of this type of object.
;
; Parameters: NONE
;********************************************************************/
void    S_init (void) {
  BYTE_T  i,j;


  for (i=0;i<9;i++) {
    Sector_Info[i].start = 255;
    Sector_Info[i].end   = 255;
    }
  
  
 for (i=0;i<9;i++)
   Sector_Assign[i] = 255;
  
   No_of_Sector = 0;
}

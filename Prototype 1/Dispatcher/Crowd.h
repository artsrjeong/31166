/* MEIJI-003  support AI crowd senser 95/2/11 H.Honma */
/* U10 Project: @(#)crowd.h	2.1	5/13/94	09:31:27 */
/* SCCS:@(#)crowd.h	1.1	9/17/92 */
/**************************************************************************
;  File Name: CROWD.H - Public Interface for CROWD.C
; 
;  File Description:
;  This file describes the CROWD.C public definitions.  
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
;  Unpublished work - Copyright 1992 Otis Elevator Company
;  All rights reserved.
;
; 
;  Revision History:
; 
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  AAA30xxxAAA           Toshimitsu Mori		14 Dec. 1994
;  - Created program
;
;  J1830203AAA           Hideyuki Honma 		09 June 1995
;  - SCR 30203-5005: Support AI Crowd Floor
;  - SCR 30203-5006: Support AI PreParking
; 
;*************************************************************************/
#ifndef FUZZYEHP_INCLUDED  /* protects from multiple includes */
#define FUZZYEHP_INCLUDED

#define LIGHT_TRAFFIC	1
#define MEDIUM_TRAFFIC  2
#define HEAVY_TRAFFIC   3

#include "infbld.h"

/**************************************************************************
;  Structures
;*************************************************************************/
/* None */   

/**************************************************************************
;  Constants
;*************************************************************************/
/* None */   

/**************************************************************************
;  Data Types
;*************************************************************************/ 
/* None */   

/**************************************************************************
;  Variables
;*************************************************************************/
EXTERN TIMER_T  ehp_crowd_msg_timer;
EXTERN TIMER_T  ehp_crowd_interval_timer;
EXTERN INT_T    crowd_count_timer;
EXTERN INT_T    Traffic_Mode,Traffic_Mode2;
EXTERN INT_T    Traffic_Status,Traffic_Status_Old,Traffic_Status_New;
EXTERN INT_T    Traffic_Status2,Traffic_Status_Old2,Traffic_Status_New2;
EXTERN BYTE_T   Crowd_Floor;
EXTERN BYTE_T   park_crowd_floor;
EXTERN BYTE_T   AssignedCrowdFloor;

/**************************************************************************
;  Function Prototypes
;*************************************************************************/
void   crowd_estimate_tsk (void);
void   ehp_update_crowd_timers (void);
void   crowd_init (void);
void   crowd_parkCrowdCheck(void);
BYTE_T crowd_selectPassingCar(BYTE_T pos);
INT_T  crowd_checkPassingCar(BYTE_T car,BYTE_T temp_dir,BYTE_T pos);
void   send_car_call(BYTE_T,BYTE_T);
void   send_hall_call(BYTE_T,BYTE_T,BYTE_T,FLAG_T);
INT_T  crowd_selectParkCar(BYTE_T pos);
void   crowd_preParkToCrowd(BYTE_T car,BYTE_T pos);
void   crowd_traffic_status_tsk(void);


#endif  /* end of include file definition */


#include "stdafx.h"
#include "common.h"

unsigned char debug_zone_no,debug_ads;
unsigned char sector_deb[8],sector_count_deb;
unsigned int  assign_car_deb,best_car_deb,assign_rrt_deb,best_rrt_deb;
unsigned char debug_step,debug_count,debug_count2;
unsigned int  p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19;
unsigned int  p20,p21,p22,p23;
/*-----------------------------------------------------------------%BSH%----
|  File Name: PARK.C - Tasks and procedures for PARK (U10 E411)
|
|  File Description:
|
|  Contains tasks and procedures for handling PARK and stuff.
|
|  This work and the information it contains are the confidential
|  property of Otis Elevator Company ("Otis").  It is delivered on
|  the express condition that it will be used only for, or on
|  behalf of, Otis; that neither it nor the information it contains
|  will be reproduced, distributed or disclosed, in whole or in part,
|  without the written consent of Otis; and that on demand it and any
|  copies will be promptly returned.
|
|  Unpublished work - Copyright 1992 Otis Elevator Company
|  All rights reserved.
|
|
|
|  Revision History:
|
|  SCN                   Author                        Date
|  ------------------------------------------------------------------------
|  AAA30xxxAAA           Toshimitsu Mori            14 Dec 1994
|  - Created program
|
|  J1830203AAA           Hideyuki Honma             09 Jun 1995
|  - SCR 30203-5030: Parking Operation
|
|  J1930203AAA           Hideyuki Honma             27 Jun 1995
|  - SCR 30203-5027: Multi Lobby Parking 
|
|  J1930203AAA           Hideyuki Honma             04 Jul 1995
|  - SCR 30203-5032: Call Cutout Switch Support        
|
|  J2130203AAA           Hideyuki Honma             19 Sep 1995
|  - SCR 30203-5052: Consider LNS/OLD mode for Lobby parking      
|
|  AAB302033AA            Mark Ross                 17 Oct 1995
|  - SCR 30203-242: OTI SCR 5052. Consider LNS/OLD mode for 
|     Lobby parking 
|
|  J2130203AAA           Hideyuki Honma             05 Oct 1995
|  - SCR 30203-5058: Upgrade Multi Lobby Function.
|
|  AAB302032AA            Mark Ross                 25 Oct 1995
|  - SCR 30203-239: OTI SCR 5058.  Upgrade parking to 
|    support multi lobby function.   Added comments.      
|
|  J2130203AAA           Hideyuki Honma             24 Nov 1995
|  - SCR 30203-5064: Parking by Service Tool Setup    
|
|  AAB-30203-3AA          Mark Ross                 11 Dec 1995
|  - SCR 30203-249: OTI SCR 5064.  Enable/disable
|    parking operation via service tool eeprom setup.     
|
|  AAB302033AA            Mark Ross                 14 Dec 1995 
|  - SCR 30203-258: OTI SCR 5070.  Change condition for 
|    multi-lobby parking.
|
|  JAB-30203-AAB         Hideyuki Honma             01  Apr 1996
|  - SCR 5076:  Correct Parking Logic during Normal operation 
|
|  JAB-30203-AAE         Hideyuki Honma             20 Jan 1999
|  - SCR 30203-5xxx: update VMIT Feature
|
|----------------------------------------------------------------%ESH%-----*/

#include "common.h"

#ifdef CAGS 
#include "global.lit"
#include "global.h"
#include "infbld.h"
#endif//ifdef CAGS

/*------------------------------------------------------------------------
|  Public Declarations
|-------------------------------------------------------------------------*/
#define EXTERN extern   /* declare public variables */
#include "hdw.h"
#include "sio.h"
#include "inferr.h"
#include "infbld.h"
#include "infocs.h"
#include "exec.h"

#include "infmsg.h"
#include "e2p.h"
#include "infads.h"
#include "crowd.h"
#include "asslib.h"
#include "assign.h"
#undef EXTERN


/*------------------------------------------------------------------------
|  Public Definitions
|-------------------------------------------------------------------------*/
#define EXTERN          // define public variables 
#include "park.h"
#undef EXTERN

/*------------------------------------------------------------------------
|  Local Definitions
|-------------------------------------------------------------------------*/
#include "park.lh"

//KidsPark/Atsugi  update for MultiUpPeak  1997/11/13
/*-----------------------------------------------------------------%BSH%----
|  Name : park_TargetEvent
|
|  Description: This task is for Passing Reassignment Logic.
|
|  Input
|        1. car
|
|  Output  
|------------------------------------------------------------------%ESH%---*/
void park_TargetEvent(BYTE_T car)
{

}//END : park_TargetEvent

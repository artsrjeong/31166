/* U10 Project: @(#)pwt.c	1.3	3/23/95	14:23:01 */
/**************************************************************************
;  File Name: PWT.C - Tasks and procedures for pwt estimator.
;
;  File Description:
;
;  Contains tasks and procedures for handling PWT estimation.
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
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  A1730203BAA           Mark Ross                     19 December 1994
;  - SCR 30203-139
;    Created module. Functions in this module were originally contained
;    in the "ica.c" module.
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;  Transamerica          Geoff Mochau                  16 February 1995
;  - Eliminated NN references for RCBII-based (conditional) compilations.
;
;  J1830203AAA        
;  - SCR 30203-5008       H.Honma                            26 May 1995
;  Add distance parameter to _calculate_pwt for use with crisp pwt calculation      
;
;  J2130203AAA           H.Honma                            26 Nov 1995
;  - SCR 30203-5048: Stop Neural-Network during MIT     
;
;  AAB-30203-3AA         Mark Ross                     8 December 1995
;  - SCR 30203-225: OTI SCR 5048. Disable neural network
;    during MIT.
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
#include "infmsg.h"
#include "inferr.h"
#include "infocs.h"
#include "sio.h"
#include "infrng.h"
#include "exec.h"
#endif
#include "e2p.h"
#include "infbld.h"
#include "aarule.h"
#include "crisppwt.h"
//#include "nrlpwt.h"
#include "assign.h"
#undef EXTERN
/**************************************************************************
;  Public Definitions
;*************************************************************************/
#define EXTERN          /* define public variables */
#include "pwt.h"
#undef EXTERN


/**************************************************************************
;  Local Definitions
;*************************************************************************/
/* None */


/**************************************************************************
;---------------------------------------------------------------------------
;  P U B L I C   P W T   F U N C T I O N S
;---------------------------------------------------------------------------
;***************************************************************************
;
;
;********************************************************************/
/*********************************************************************
; Function Name: pwt_determine_pwt
;
; There are currently two different ways to make an RRT prediction within
; this ica.c module - the conventional, sometimes called "CRISP" way of
; predicting RRT and the neural network way.  The method of RRT to be used
; in dispatching decisions is configurable based on a service tool
; parameter.
;
; This procedure calls the appropriate routine to calculate PWT based
; on rrt_type.  
;
;
;********************************************************************/
INT_T pwt_determine_pwt(BYTE_T car, BYTE_T pos, BYTE_T dstPos, 
                        BYTE_T pre_dir, BYTE_T mask, INT_T rrt,
                        BYTE_T distance,BYTE_T *distance3,BYTE_T *affectPwtByDst,
                        BYTE_T *spareCap)
{
	INT_T pwt;

	pwt = crisppwt_calculate_pwt (car, pos, dstPos, pre_dir, mask, rrt, distance,distance3,affectPwtByDst,
                                    spareCap); 
	return (pwt);
}


/*********************************************************************
; Function Name: pwt_update_rrt_timers
;
; This function is called from the timer task, once per second.
; All elapsed registration timers are updated, if they are less than
; 255 seconds.
;
; Parameters: none
;
;********************************************************************/
void pwt_update_rrt_timers(void)
{
   
 REG_INT_T landing,riser,maxRiser,car;

 //SGO
   if(infbld_Sgo1EnableFlag == TRUE)
      maxRiser = 2;
   else
      maxRiser = 1;

   for(riser = 0; riser < maxRiser; riser++)
//for cpuSave,00/10/19    for (landing = 0; landing < MAX_LANDINGS; landing++)
    for (landing = 0; landing <= bld_info.top_pos; landing++)
    {
       if (bld_info.fuhc[riser][landing].reg_elapsed < 254)
       {
          bld_info.fuhc[riser][landing].reg_elapsed++;

       }
          //DBD-honma 04Apr04
          if (*e2p_DbdAssignEnable != 0)
          for (car = CAR1; car <= CAR8; car++)
          {
             if (bld_info.fuhc[riser][landing].reg_elapsed_car[car] < 254)
               bld_info.fuhc[riser][landing].reg_elapsed_car[car]++;
          }
       
       if (bld_info.fdhc[riser][landing].reg_elapsed < 254)
       {
          bld_info.fdhc[riser][landing].reg_elapsed++;

       }
       
          //DBD-honma 04Apr04
          if (*e2p_DbdAssignEnable != 0)
          for (car = CAR1; car <= CAR8; car++)
          {
             if (bld_info.fdhc[riser][landing].reg_elapsed_car[car] < 254)
               bld_info.fdhc[riser][landing].reg_elapsed_car[car]++;
          }

       if (bld_info.ruhc[riser][landing].reg_elapsed < 254)
       {
          bld_info.ruhc[riser][landing].reg_elapsed++;
       }
       
       if (bld_info.rdhc[riser][landing].reg_elapsed < 254)
       {
          bld_info.rdhc[riser][landing].reg_elapsed++;
       }

#ifndef CAGS
//dojima for WheelChairCall
       if (bld_info.e_fuhc[riser][landing].reg_elapsed < 254)
       {
          bld_info.e_fuhc[riser][landing].reg_elapsed++;
       }
       
       if (bld_info.e_fdhc[riser][landing].reg_elapsed < 254)
       {
          bld_info.e_fdhc[riser][landing].reg_elapsed++;
       }
#endif

    } /* for each landing */
}
/* 30203-5008 update rrt_run_simulation to calculate distance add param */
/* U10 Project: @(#)rrt.h	1.2	3/23/95	14:23:04 */
/**************************************************************************
;  File Name: RRT.H - Public Interface for RRT.C
; 
;  File Description:
;  This file describes the rrt.c public definitions.
; 
;  This work and the information it contains are the confidential
;  property of Otis Elevator Company ("Otis").  It is delivered on
;  the express condition that it will be used only for, or on
;  behalf of, Otis; that neither it nor the information it contains
;  will be reproduced, distributed or disclosed, in whole or in part,
;  without the written consent of Otis; and that on demand it and any
;  copies will be promptly returned.
;
;  Unpublished work - Copyright 1994 Otis Elevator Company
;  All rights reserved.
;
; 
;  Revision History:
;
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  A1730203BAA           Mark Ross                     19 December 1994
;  - SCR 30203-139
;    ICA.C breakup. Made appropriate changes for function invocations and
;    breakup of the ICA.C module.
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;  J1830203AAA           H.Honma                          26 May 1995
;  - SCR 30203-5008
;    update rrt_run_simulation to calculate distance add parameter
;  - SCR 30203-5017      H.Honma (Suzuki)                 29 May 1995
;    change call mask variables type from BYTE to INTEGER
;*************************************************************************/
#ifndef RRT_INCLUDED  /* protects from multiple includes */
#define RRT_INCLUDED
#ifndef CAGS
#include "infocs.h"
#endif
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
/* None */

/**************************************************************************
;  Function Prototypes
;*************************************************************************/
INT_T rrt_determine_rrt(BYTE_T car, BYTE_T call_pos, BYTE_T call_dst, BYTE_T call_mask, 
                        BYTE_T pre_dir, FLAG_T *coinc_cc_flag, BYTE_T *coinc_type,  
                        BYTE_T *distance,BYTE_T *distance2,BYTE_T *spareCap);
INT_T rrt_run_simulation(BYTE_T car, BYTE_T start, BYTE_T finish, INT_T mask,
                          FLAG_T stopping_flag, FLAG_T reversal_flag,
                          BYTE_T *cumulativeDistance,BYTE_T *distance2,
                          FLAG_T expectCcCheckFlag,
                          BYTE_T spCapType,BYTE_T *spareCap);

#endif  /* end of include file definition */

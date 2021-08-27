/* U10 Project: @(#)crisprrt.h	1.2	3/23/95	14:22:35 */
/**************************************************************************
;  File Name: CRISPRRT.H - Public Interface for CRISPRRT.C
; 
;  File Description:
;  This file describes the crisprrt.c public definitions.
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
;    ICA.C breakup. Made appropriate changes for function invocations and
;    breakup of the ICA.C module.
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;  J1830203AAA           
;  - SCR 30203-5017      H.Honma (Suzuki)                     29 May 1995
                      change call mask variables type from BYTE  to INTEGER
;  - SCR 30203-5004      H.Honma (Suzuki)                     30 May 1995
;                     support LCADS car call prediction 
;*************************************************************************/
#ifndef CRISPRRT_INCLUDED  /* protects from multiple includes */
#define CRISPRRT_INCLUDED
#include "infocs.h"
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
EXTERN BYTE_T car_call_pred[JICA_FLOORS][2];

/**************************************************************************
;  Function Prototypes
;*************************************************************************/
EXTERN INT_T crisprrt_calculate_rrt(BYTE_T car, BYTE_T pos, BYTE_T dstPos, INT_T mask,BYTE_T pre_dir,FLAG_T *coinc_cc_flag,BYTE_T *coinc_type, BYTE_T *distance,BYTE_T *distance2,BYTE_T *spareCap);

#endif  /* end of include file definition */

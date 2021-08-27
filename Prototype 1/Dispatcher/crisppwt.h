/* U10 Project: @(#)crisppwt.h	1.2	3/23/95	14:22:34 */
/**************************************************************************
;  File Name: CRISPPWT.H - Public Interface for CRISPPWT.C
; 
;  File Description:
;  This file describes the crisppwt.c public definitions.
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
;    ICA.C breakup. Made appropriate changes for function invocations and
;    breakup of the ICA.C module.
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;  J1830203AAA           H.Honma (Suzuki)                   29 May 1995
;  - SCR 30203-5017  change call mask BYTE_T to INTEGER 
;
;  JAB-30203-AAB         Hideyuki Honma                     25  Apr 1996
;  - SCR 5083:  Change PWT calculation
;
;*************************************************************************/
#ifndef CRISPPWT_INCLUDED  /* protects from multiple includes */
#define CRISPPWT_INCLUDED
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
/* None */
/* hhhh */
EXTERN INT_T  dummy_assigned_calls[MAX_OCSS][JICA_FLOORS];

/**************************************************************************
;  Function Prototypes
;*************************************************************************/
INT_T crisppwt_calculate_pwt(BYTE_T car, BYTE_T pos, BYTE_T dstPos, BYTE_T pre_dir, INT_T mask
                             ,INT_T rrt,BYTE_T distance,BYTE_T *distance3,BYTE_T *affectPwtByDst,
                              BYTE_T *spareCap);

#endif  /* end of include file definition */

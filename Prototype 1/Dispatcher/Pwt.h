/* SCR 30203-5008 add distance parameter */
/* U10 Project: @(#)pwt.h	1.2	3/23/95	14:23:01 */
/**************************************************************************
;  File Name: PWT.H - Public Interface for PWT.C
; 
;  File Description:
;  This file describes the nrlpwt.c public definitions.
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
;  J1830203AAA  
;  - SCR 30203-5008       H.Honma                                 26 May 1995
;  Add distance parameter to _calculate_pwt for use with crisp pwt calculation
;*************************************************************************/
#ifndef PWT_INCLUDED  /* protects from multiple includes */
#define PWT_INCLUDED

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
INT_T pwt_determine_pwt(BYTE_T car, BYTE_T pos, BYTE_T dstPos, BYTE_T pre_dir, BYTE_T mask,
                        INT_T rrt, BYTE_T distance,BYTE_T *distance3,BYTE_T *affectPwtByDst,
                        BYTE_T *spareCap);
void  pwt_update_rrt_timers(void);

#endif  /* end of include file definition */
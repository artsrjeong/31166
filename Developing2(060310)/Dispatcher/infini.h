/* U10 Project: @(#)infini.h	1.1	3/23/95	14:22:50 */
/**************************************************************************
;  File Name:  INFINI.H - Public Data Definitions for INFINI.C
;
;  File Description:
;  This file describes the INFINI.C public data.  This file is to be
;  included in any file that uses these services.
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
;  Unpublished work - Copyright 1995 Otis Elevator Company
;  All rights reserved.
;
;
;  Revision History:
;
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Created (renamed) module from "infini_p.h".
;
;********************************************************************/
#ifndef INFINI_INCLUDED  /* protects from multiple includes */
#define INFINI_INCLUDED

/**********************************************************************
; Global include files
;*********************************************************************/
#include "timer.h"

/********************************************************************
; Global initialization information
*********************************************************************/
EXTERN	FLAG_T	icss2_ini;
EXTERN	TIMER_T	icss2_ini_timer;

/*********************************************************************
; Function Prototypes - Initialize global data structure.
**********************************************************************/
void init_data_stores(void);

#endif  /* end of include file definition */


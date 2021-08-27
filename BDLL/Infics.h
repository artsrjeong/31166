/* U10 Project: @(#)infics.h	1.1	3/23/95	14:22:49 */
/*********************************************************************
;  File Name:  INFICS.H - Public Data Definition for INFICS.C
;
;  File Description:
;  This file describes the INFICS.C public data.  This file is to be
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
;  - SCR 30203-38: Created (renamed) module from "infics_p.h".
;
;********************************************************************/
#ifndef INFICS_INCLUDED  /* protects from multiple includes */
#define INFICS_INCLUDED

#include "timer.h"  /* timer type definition */

/********************************************************************
; ICSS Constants
********************************************************************/
#define		ICSS2_ORIGIN	12	/* ICSS2 origin address  	*/
#define         HCMS_ORIGIN     13      /* HCMS origin address          */
#define		ADSS_PU_ORIGIN  15	/* ADSS/PU origin address	*/
#define		EMS_ORIGIN	10	/* EMS origin address		*/
#define		MAX_ICSS	16	/* maximum icss's		*/

/*********************************************************************
; ICSS Structure
; This structure is used in the ICSS_S definition.  This type
; definition will be used by the client module to create an instance
; of an ICSS.
;
; For each ICSS, a timer is maintained along with the status of the
; subsystem (dead or alive).
;********************************************************************/
struct ICSS_S  {
  TIMER_T    tout;
  FLAG_T     alive;
};
typedef struct ICSS_S ICSS_T;

/*********************************************************************
; Global ICSS information data store
;********************************************************************/
EXTERN ICSS_T icss_info[MAX_ICSS];

/*********************************************************************
;  Function Prototypes - initialize an ICSS instance.
;********************************************************************/
void I_init(void);

#endif  /* end of include file definition */


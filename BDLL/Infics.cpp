#include "stdafx.h"

/* U10 Project: @(#)infics.c	2.2	3/23/95	14:22:48 */
/**************************************************************************
; File Name: INFICS.C - ICSS Info Handler
;
; File Description:
; This file contains modules that handle data for a given instance
; of an icss.  These files are reentrant.
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
;
; Created Program.
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;*************************************************************************/
#include "common.h"

/**************************************************************************
;  Public Declarations
;*************************************************************************/
#define EXTERN extern   /* declare public variables */
/* None */
#undef EXTERN

/**************************************************************************
;  Public Definitions
;*************************************************************************/
#define EXTERN          /* define public variables */
#include "infics.h"
#undef EXTERN

/**************************************************************************
;  Local Definitions
;*************************************************************************/
/* None */


/**************************************************************************
;  Public Functions
;*************************************************************************/

/*********************************************************************
; Name: I_init
;
; Description: Initialize ICSS information data store
;
; Parameters: NONE
;********************************************************************/
void I_init(void)
   
{
   
   INT_T i;
   
   /*---------------------------------------------------------
      ; Initialize for ten second timeout, assume all ICSS dead.
      ----------------------------------------------------------*/
   for (i=0; i<MAX_ICSS; i++) 
   {
      icss_info[i].tout = TIM_10S;
      icss_info[i].alive = FALSE;
   }
}

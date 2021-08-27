/* U10 Project: @(#)infini.c	2.7	3/23/95	14:22:49 */
/**************************************************************************
; File Name: INFINI.C - Data Initialization Module
;
; File Description: This module contains functions which will initialize
;	global data structures.
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
;  Unpublished work - Copyright 1991 - 1995 Otis Elevator Company
;  All rights reserved.
;
;
; Revision History:
;
; SCN                   Author                      Date
; ------------------------------------------------------------------------
; AAA-----AAA           Mark A. Ross              14-NOV-91
; - Created Program.
;
; AAA-----AAA           Jeremy Kezer              9 July 1993
; - Modify code to be more consistant with ICSS2 coding standards.
;
; A1530133BAA           Jeremy Kezer              31 August 1993
; - Added generation of light, heavy fuzzy sets for autoadjust feature.
;
; A1430203AAA           Geoff Mochau              29 November 1993
; - Added initialization for Predirection Recommendation.
;
; A1430203BAA           Paul Simcik               03 May 1994
; - SCR 30203-52
;   Added definition of "autoadj.h" to reflect definitions.
;   Hall call density and car call density factors separated.
;
;  A1530203BAA           Paul Simcik/Mark Ross    07 July 1994
;  - SCR 30203-107
;    Modified to support enhanced traffic density feature.
;
;  A1630203BAA           Paul Simcik/Mark Ross    08 August 1994
;  - SCR 30203-107
;    Modified to support seperation of traffic density and autoadjust
;    features.
;
;  A1730203BAA           Mark Ross       	  28 November 1994
;  - SCR 30203-118.
;    Modified to vary fuzzy sets by adjusting coordinate sets, not the
;    fuzzy sets themselves.
;
;  A1730203BAA           Mark Ross                19 December 1994
;  - SCR 30203-139
;    ICA.C breakup. Made appropriate changes for function invocations.
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;  J1830203AAA           Hideyuki Honma                11   June   1996
;  - SCR 30203-5005: Support AI Crowd Floor .
;
;  J1830203AAA           Hideyuki Honma                11   June   1996
;  - SCR 30203-5002: Support Dual Up Peak .
;
;  AAB-30203-3AA         Mark Ross                        17 November 1995
;  - SCR 30203-204: Modified to remove calls to watchdog().
;    Now use POR_NMI_Handler on startup.
;
;  J2130203AAA           Hideyuki Honma                     31 Nov 1995
;  - SCR 30203-5029: Dispatch during Channeling
;
;  AAB-30203-3AA         Mark Ross                           21 Dec 1995
;  - SCR 30203-228: OTI SCR 5029.  Dispatch during Channeling
;
;  JAB-30203-AAE         Hideyuki Honma                      02 Feb 1999
;  - SCR 30203-5xxx: Update Application Initialization
;
;  xxx-31017-xxx         Dan Williams                  January 2004
;  - add initialization for  Destination Entry Dispatcher
;
;*************************************************************************/

#include "stdafx.h"
#include "common.h"
#include "pi.h"
/**************************************************************************
;  Public Declarations
;*************************************************************************/

/**************************************************************************
;  Public Definitions
;*************************************************************************/
#define EXTERN extern   /* declare public variables */
#include "hdw.h"
#include "infocs.h"
#include "infbld.h"
#include "queue.h"
#include "assign.h"

#undef EXTERN

#define EXTERN          /* define public variables */
#include "infini.h"
#undef EXTERN

/**************************************************************************
;  Local Definitions
;*************************************************************************/
/* None */


/**************************************************************************
;  Public Functions
;*************************************************************************/

/*********************************************************************
; Function Name: init_data_stores
;
; Description: This function initializes the global data stores.
;
; Parameters: NONE
;********************************************************************/
void init_data_stores(void)
{
  BYTE_T	i;

  /*---------------------------------------------------------
  ; Initialize EEPROM pointers
  ;----------------------------------------------------------*/
  e2p_init();

  /*----------------------------------------------------------
  ; Initialize building information (grp_info) data structure
  -----------------------------------------------------------*/
  G_init(&grp_info);
  /*----------------------------------------------------------------
  ; Initialize assignment related variables
  -----------------------------------------------------------------*/
  assign_init();
}

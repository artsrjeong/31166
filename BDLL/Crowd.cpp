unsigned char debug_scar;
/* U10 Project: @(#)crowd.c	2.1	5/13/94	09:31:24 */
/* SCCS:@(#)crowd.c	1.1	9/17/92 */
unsigned  char Bob;
unsigned  char ehp_old_cars_in_group;

/**************************************************************************
; File Name: CROWD.C - Tasks and procedures for Crowd
;
; File Description:
;
; Contains tasks and procedures for handling PARK and stugg.
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
;
;  This work and the information it contains are the confidential
;  property of Otis Elevator Company ("Otis").  It is delivered on
;  the express condition that it will be used only for, or on
;  behalf of, Otis; that neither it nor the information it contains
;  will be reproduced, distributed or disclosed, in whole or in part,
;  without the written consent of Otis; and that on demand it and any
;  copies will be promptly returned.
;
;  Unpublished work - Copyright 1992 Otis Elevator Company
;  All rights reserved.
;
;
;
; Revision History:
;
; SCN                   Author                      Date
; ------------------------------------------------------------------------
;  AAA30xxxAAA           Toshimitsu Mori		14 Dec. 1994
;  - Created program
;
;  J1830203AAA           Hideyuki Honma 		09 June 1995
;  - Crowd Operation
;
;  AAB302033AA        Mark Ross                                17 Oct 1995
;  - SCR 30203-243: OTI SCR 5055. Crowd floor service by EMS.
;
;*************************************************************************/
#include "stdafx.h"
#include "common.h"
#include "math.h"

#ifdef CAGS
#include "global.lit"
#include "global.h"
#endif
/**************************************************************************
;  Public Declarations
;*************************************************************************/
#define EXTERN extern   /* declare public variables */
#include "infbld.h"
#include "infmsg.h"
#include "sio.h"
#include "infocs.h"
#include "timer.h"
#include "e2p.h"
#include "inferr.h"
#include "infads.h"
#include "assign.h"
#include "asslib.h"
#undef EXTERN

/**************************************************************************
;  Public Definitions
;*************************************************************************/
#define EXTERN          /* define public variables */
#include "crowd.h"
#undef EXTERN

/**************************************************************************
;  Local Definitions
;*************************************************************************/
#include "crowd.lh"

#define MAX_ESTIMATE	61



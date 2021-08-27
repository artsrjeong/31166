/* U10 Project: @(#)infrng.c    2.13    3/23/95 14:22:52 */
/**************************************************************************
; File Name: INFDES.C - Destination Entry Server Communications Handler
;
; File Description:
; This file contains modules that handle des communications. These files
; are reentrant.
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
;  Unpublished work - Copyright 2004 Otis Elevator Company
;  All rights reserved.
;
;
; Revision History:
;
; SCN                   Author                      Date
; ------------------------------------------------------------------------
;  A1231017AAA          S. Dridi                  25 Mar 2004
;  - SCR 31017-23: Created (renamed from  place holder module  infrng.c)
;
;  A1231017AAA          S. Dridi                  6 Apr 2004
;  - SCR 31017-16: Enable / Disable COP based on hot/cold floors
;
;  A1231017AAA          S. Dridi                  21 Apr 2004
;  - SCR 31017-14: Walk rate from DEC to Car
;
;  A1231017AAA          S. Dridi                  4 May 2004
;  - SCR 31017-15: Handicap support
;
;  A1231017AAA          S. Dridi                  12 May 2004
;  - SCR 31017-14: Extend Dwell Time if passenger walk time hasnt expired
;
;*************************************************************************/
#include "stdafx.h"

//#include "common.h"


/*************************************************************************
;  Public Definitions
;*************************************************************************/

//#define EXTERN          /* define public variables */
//#include "infdes.h"
//#undef EXTERN

/*
unsigned char debug_second,debug_pref,debug_ems;
unsigned int debug_board,debug_deboard,tmpBuf1;
unsigned char infrng1,infrng2,infrng3,infrng4,infrng5,infrng6,Type7Sub100,Type7Sub1004;
unsigned int  debDup1;
unsigned char debDup2[5];
unsigned char dbdDebug[10];
*/
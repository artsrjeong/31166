/**************************************************************************
|  File Name: TRAFDAT.C - Tasks and procedures for traffic database task
|
|  File Description:
|
|  Contains tasks and procedures for Variable MIT Feature.
|
|
|  This work and the information it contains are the confidential
|  property of Otis Elevator Company ("Otis").  It is delivered on
|  the express condition that it will be used only for, or on
|  behalf of, Otis; that neither it nor the information it contains
|  will be reproduced, distributed or disclosed, in whole or in part,
|  without the written consent of Otis; and that on demand it and any
|  copies will be promptly returned.
|
|  Unpublished work - Copyright 1997 Otis Elevator Company
|  All rights reserved.
|
|
|  Revision History:
|
|  SCN               Author                      Date
|  ------------------------------------------------------------------------
|
|=========================================================================
|  JAB-30203-AAE     Hideyuki Honma              17-Jan-1999
|  - CHG 30203-5xxx  Created for U10 V-MIT and ESO
|
|*************************************************************************/
#include "stdafx.h"
#include "common.h"

#ifdef CAGS 
#include "global.lit"
#include "global.h"
#include "infbld.h"
#endif//ifdef CAGS

/**************************************************************************
;  Public Declarations
;*************************************************************************/
#define EXTERN extern   /* declare public variables */
#ifndef CAGS
#include "infbld.h"
#include "infocs.h"
#include "sio.h"
#include "inferr.h"

#endif //ifndef CAGS
#include "e2p.h"
#include "infmsg.h"
#include "density.h"
#include "rrt.h"
#include "pwt.h"
#include "asslib.h"
#include "crisprrt.h"
#include "crowd.h"
#include "infads.h"
#include "assign.h"
#include "park.h"
#include "mit.h"
#include "calender.h"
#include "infdes.h"
#undef EXTERN



/**************************************************************************
;  Public Definitions
;*************************************************************************/
#define EXTERN          /* define public variables */
#ifdef CAGS
#include "fuzzyop.h"
#endif
#include "trafdat.h"
#undef EXTERN


/**************************************************************************
;  Local Definitions
;*************************************************************************/
#define SECONDS_PER_INTERVAL	30
#define PASSENGERS_PER_INTERVAL	8

/* define penalties for MIT lobby RSR calc's */
#define MIT_RLR_REVERSAL	80
#define MIT_RLR_CC		80
#define MIT_RLR_LANDING		20
#define MIT_RLR_BUSY		200

#define MIT_RLR_NAV		10000
#define MIT_RLR_BASE		0

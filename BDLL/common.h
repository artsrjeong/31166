/*---------------------------------------------------------------------------%BSH%----
|  File Name: COMMON.H - Common Public Include File for all C Routines
|
|  File Description:
|  This file contains items that are common throughout the system.  These
|  items are NOT application specific.  They are primitive level items
|  that would be generic to any application.
|
|  These items includes common data types, constants, and compiler
|  dependencies.  Descriptions of common naming comventions is also
|  included.
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
|  Copyright (c) 1996 Otis Elevator Company, OECF, All rights reserved
|
|      SCN           Author                Date
|  -----------------------------------------------------------------------
|  ---30301---     Mark Ross          05-Mar-1996
|  Created module. Based on AAB-30203-AAA.
|
|  ---30301---     Mark Ross          08-Apr-1996
|  Modifications resulting from code review.
|
|  AAB-30301-2AA   Mark Ross          13-Sep-1996
|  - SCR 30301-32. Changed INT_T to "unsigned long" and INT_S to "long".
|
|  AAB-30301-2AA   Mark Ross          01-Nov-1996
|  - SCR 30301-33. Modified to support the use of all.h.
|
|  AAB-30301-3AA   Mark Ross          25-Feb-1997
|  - SCR 30402-03. Modified to support 12 cars, 140 landings, 16 HC sets,
|                  and 16 CC sets. Added common_CallMaskT
|
|  A11-31017-AAA        S. Dridi                        20-Apr04
|  - SCR 31017-xx  Added some common defines
|
|
|----------------------------------------------------------------------------%ESH%---*/
#ifndef common_AlreadyIncludeHeader
#define common_AlreadyIncludeHeader

#include "all.h"

/*---------------------------------------------------------------------------%BSH%----
|  Common Elevator Application Definitions
|----------------------------------------------------------------------------%ESH%---*/

#define		UP_DIR		1	/* Up direction			*/
#define		DOWN_DIR	2	/* Down direction		*/
#define		NO_DIR		0	/* No direction         */
#define     BOTH_DIR    3   /* up and down dir */

#define     common_None      NO_DIR
#define     common_Up        UP_DIR
#define     common_Down      DOWN_DIR
#define     common_Both      BOTH_DIR

#define     FRONT_DOOR       0
#define     REAR_DOOR        1

#define     DOORS_CLOSED    0       /* door state = closed          */
#define     DOORS_CLOSING   1       /* door state = closing         */
#define     DOORS_OPENED    2       /* door state = opened          */
#define     DOORS_OPENING   3       /* door state = opening         */

#define     STANDARD_CALLSET  0         /* for ring message use (extended = HDCP) */
#define     EXTENDED_CALLSET  1

#define     STD_CALL        1           /* DES Call Type */
#define     HDCP_CALL       2


//typedef unsigned short  common_CallMaskT;
typedef unsigned short  common_CarCallMaskT;
typedef unsigned long   common_HallCallMaskT;


/**************************************************************************
;  Define a 2 Byte Data Type
;
;  ex.  INT_T Index;    (declare 2 byte Index variable)
;       For Motorolla, 4 byte is used.
;         Index = 5;     (set 2 byte index value to 5)
;*************************************************************************/
typedef int INT_T;	  /* two byte type */
/**************************************************************************
;  Define a 2 Byte Data Type
;
;  ex.  S_INT_T Index;    (declare 2 byte Index variable)
;         Index = 5;     (set 2 byte index value to 5)
;*************************************************************************/
typedef int S_INT_T;	  /* two byte type */

/**************************************************************************
;  Define a 2 Byte register integer data type
;
;  ex.  REG_INT_T Index;    (declare 4 byte Index variable)
;         Index = 5;     (set 4 byte index value to 5)
;*************************************************************************/
/* #define REG_INT_T  register int	*/  /* two byte type */
#define REG_INT_T   short	  /* two byte type */




#endif

/*  end of common.h */



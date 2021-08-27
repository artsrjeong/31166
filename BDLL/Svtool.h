/* U10 Project: @(#)svtool.h    1.1     2/15/95 09:16:58 */
/**************************************************************************
;  File Name: SVTOOL.H - Public Interface for SVTOOL.C
;
;  File Description:
;  This file describes the SVTOOL.C public definitions.  This file
;  is to be included in any file that uses these services.  See the
;  SVTOOL.C file for detailed usage information.
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
;  SCN                   Author                        Date
;  ------------------------------------------------------------------------
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Created (renamed) module from "svtool_p.h".
;
;  AAB-30301-2AA	 Kate Goldstone		       07-AUG-1996
;  - SCR 30301-7
;*************************************************************************/
#ifndef SVTOOL_INCLUDED  /* protects from multiple includes */
#define SVTOOL_INCLUDED

#ifdef svtool_ModuleDefinition
#define svtool_EXTERN
#else
#define svtool_EXTERN extern
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
svtool_EXTERN FLAG_T  svtool_ChangeDateFlag;
svtool_EXTERN FLAG_T  svtool_ChangeTimeFlag;


/**************************************************************************
;  Function Prototypes
;*************************************************************************/
svtool_EXTERN void SV_Driver(void);
svtool_EXTERN void SV_Init(void);

#endif  /* end of include file definition */


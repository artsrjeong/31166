/*-------------------------------------------------------------- %BMH% --
| File Name: switch.h - Project specific compilation switches.
|
| Description:
|  This file contains compilation switches specific to a project.
|  This file, as it exists in the reuse library, should be used as an
|  example/skeleton only. The user should make a copy of this file,
|  include it as part of the project library, and tailor it to the specific
|  needs of the project.
|
|     SCN       Date    Who      Description
| -----------  ------  -----     -------------------------------------------
| AAA30xxxAAA  960931  BA/SH/MR  Initial Creation
|
|  Copyright (C) 1996 Otis Elevator. All Rights Reserved.
---------------------------------------------------------------- %EMH% --*/
#ifndef switch_DEFINED             // for multiple includes
#define switch_DEFINED

/*-----------------------------------------------------------------------
|  Define compiler dependent switches.
-------------------------------------------------------------------------*/
#ifdef	_MSC_VER

#if	(_MSC_VER >= 1300)
#define	VC70
#else
#define	VC60
#endif
#define	VCPP
#define MSC

#else		// else of _MSC_VER

//#define MT68K_C
//#define DCC68K
#endif

/*-----------------------------------------------------------------------
|  Define project dependent switches.
-------------------------------------------------------------------------*/
#define DISPATCHING_2D				// CSW: defined for 2-D project
/*-----------------------------------------------------------------------
|  Define unit testing switches.
|  - Create and write log file for checking unit testing of Database component
-------------------------------------------------------------------------*/
//#define	UNIT_TEST_DB

/*-----------------------------------------------------------------------
|  Define operating logic dependent switches.
-------------------------------------------------------------------------*/
// In making floor separation table, exception applies 
//	for overhead and pit adjacent floor
#define SEPARATION_FLOOR_EXCEPTION
// use the score of POI : if POI is violated, then return maximum limit value for long
//#define	WITH_POI_SCORE
/*-----------------------------------------------------------------------*/
#endif

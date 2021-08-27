/* SCCS:@(#)park.h	1.1	12/07/93 */
/*-----------------------------------------------------------------%BSH%----
|  File Name:  PARK.H - Public Data Definitions for PARK.C
|
|  File Description:
|  This file describes the PARK.C public data.  This file is to be
|  included in any file that uses these services.
|
|  This work and the information it contains are the confidential
|  property of Otis Elevator Company ("Otis").  It is delivered on
|  the express condition that it will be used only for, or on
|  behalf of, Otis; that neither it nor the information it contains
|  will be reproduced, distributed or disclosed, in whole or in part,
|  without the written consent of Otis; and that on demand it and any
|  copies will be promptly returned.
|
|  Unpublished work - Copyright 1992 Otis Elevator Company
|  All rights reserved.
|
|
|
|  Revision History:
|
|  SCN                   Author                        Date
|  ------------------------------------------------------------------------
|  AAA30xxxAAA           Toshimitsu Mori            14 Dec. 1994
|  - Created program
|
|  J1830203AAA           Hideyuki Honma             09 June 1995
|  - Parking Operation
|
|  JAB30203AAE           Hideyuki Honma             04 Feb  1999
|  - Update Parking Operation( Osaka Doujima building)
|----------------------------------------------------------------%ESH%-----*/


#ifndef PARK_INCLUDED  // protects from multiple includes 
#define PARK_INCLUDED

/*------------------------------------------------------------------------
|  Constants
|-------------------------------------------------------------------------*/
#define         ZoneLby          0
#define         NoZone           0
#define         Zone1            1
#define         Zone2            2
#define         Zone3            3
#define         Zone4            4
#define         Zone5            5
#define         Zone6            6
#define         Zone7            7
#define         Zone8            8
#define         MainLbyZone     10
#define         SecondLbyZone   11
#define         InLobbyZone     12
#define         BaseZone        13
#define         MAX_ZONE        15
//#define         park_Invalid    255
#define         park_Invalid    15

#define         MainLbyId        0
#define         SecondLbyId      1

#define         MAX_LOBBY        2

#define         NoMlb            0
#define         MlbFixed         1
#define         MlbVariable      2

// MIT Priority Define
#define         Release          0
#define         Selected         1
#define         NextSelected     2
#define         Three            3
#define         Four             4
#define         Five             5
#define         Six              6
#define         Seven            7
#define         Eight            8
#define         NoDemand        12
#define         Escape          14
#define         NoPriority      15

// Park Step,this is for debugging Effort.
#define         stepLobbyCheck1         0
#define         stepLobbyCheck2         1
#define         stepLobbyCheck3         2
#define         stepNormalPark          3
#define         stepNormalAllowed       4
#define         stepMotPark             5
#define         stepMitPark             6
#define         stepAdsMitPark          7
#define         stepNormalLobbyPark     8
#define         stepMitMlbVariable      9
#define         stepMitMlbFixed         10
#define         stepTargetEvent         11
#define         stepMitPriorityEvent    12
#define         stepHallFloor           13
#define         stepShouldParkLby       14
#define         stepMzpDesignatedPos    15

#endif  // end of include file definition 

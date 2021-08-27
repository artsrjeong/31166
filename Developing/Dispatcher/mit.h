/*--------------------------------------------------------------%BSH%-----
|  File Name: MIT.H - Public Interface for MIT.C
|
|  File Description:
|  This file describes the mit.c public definitions.
|
|  This work and the information it contains are the confidential
|  property of Otis Elevator Company ("Otis").  It is delivered on
|  the express condition that it will be used only for, or on
|  behalf of, Otis; that neither it nor the information it contains
|  will be reproduced, distributed or disclosed, in whole or in part,
|  without the written consent of Otis; and that on demand it and any
|  copies will be promptly returned.
|
|  Unpublished work - Copyright 1996 - 1998 Otis Elevator Company
|  All rights reserved.
|
|
|  Revision History:
|
|  SCN                   Author	                       Date
|  ------------------------------------------------------------------------
|
|  JAB30203AAE           Hideyuki Honma 		11 Feb  1999
|  - Created.( from Osaka Doujima building/V-MIT)
|  
| ---------------------------------------------------------------%ESH%-----*/

#ifndef MIT_INCLUDED  // protects from multiple includes 
#define MIT_INCLUDED

/*---------------------------------------------------------------%BSH%-----
|  Constants
| ---------------------------------------------------------------%ESH%-----*/
#define   mit_NoPriority            15  /* serving the general floor */
#define   mit_ReleasePriority       0   /* just before closing the door at lobby */
#define   mit_1stPriority            1
#define   mit_2ndPriority            2

#define   mit_DoorOpenAllCars        0
#define   mit_DoorOpen1st2ndCars     1
#define   mit_DoorOpen1stCar         2

#define   mit_StandardMit            0
#define   mit_VariableMit            1
#define   mit_MultiUpPeak            2
#define   mit_Channeling             3

#define   mit_GroupNormal	     0
#define   mit_Group1                 1
#define   mit_Group2                 2
#define   mit_Group3                 3
#define   mit_Group4                 4
#define   mit_MaxGroups              5

#define   mit_MaxLobby               2

#define   mit_ParkAllCars	     0
#define   mit_Park1st2ndCars         1

#define   mit_2ndLobbyParkFixed      0
#define   mit_2ndLobbyParkVariable   1

#define   mit_MaxTimers		     3
#define   mit_TrafficMonitorInterval 0
#define   mit_PeakAtMainLobby        1
#define   mit_PeakAt2ndLobby         2


#define   RequestOcss			0
#define   RequestNoDemand		1

#define   Release0			2
#define   Release15			3
#define   ReleaseGroupOut		4
#define   ReleaseTimeOut		5
#define   ReleaseFindPriorityError	6
/*---------------------------------------------------------------%BSH%-----
|  Data Types
| ---------------------------------------------------------------%ESH%-----*/
/* None */

/*---------------------------------------------------------------%BSH%-----
|  Variables
| ---------------------------------------------------------------%ESH%-----*/
EXTERN BYTE_T   mit_Priority[mit_MaxLobby][mit_MaxGroups][MAX_OCSS];
EXTERN BYTE_T   mit_GroupCar[MAX_OCSS];
EXTERN BYTE_T   mit_2ndFixedLobby[MAX_OCSS];
EXTERN TIMER_T  mit_Timer[mit_MaxTimers];
EXTERN INT_T    mit_ParkTaskId;
EXTERN BYTE_T   mit_CallSet[MAX_OCSS];
EXTERN BYTE_T   mit_lobbyCar[MAX_OCSS];

/*---------------------------------------------------------------%BSH%-----
|  Function Prototypes
| ---------------------------------------------------------------%ESH%-----*/
BYTE_T mit_CheckLobbyPos(BYTE_T car,BYTE_T pos);
void   mit_Init(void);
void   mit_InitCar(BYTE_T car);

void   mit_getPriority(BYTE_T car,BYTE_T pos,BYTE_T typ);

void   mit_releasePriority(BYTE_T car,BYTE_T pos,BYTE_T typ);

void   mit_CheckLobbyCar(BYTE_T car,BYTE_T lobby_pos);
FLAG_T mit_PriorityCarInLobby(BYTE_T car,BYTE_T lbyPos,BYTE_T priority);
BYTE_T mit_CheckLobbyId(BYTE_T car,BYTE_T pos);
void   mit_PriorityCarControl(BYTE_T car,BYTE_T pos);
void   mit_SetInitialPriority(void);
//void   mit_StandardParkTask(void);
//void   mit_2ndLobbyParkTask(void);
BYTE_T mit_CheckCarsAtLobby(BYTE_T grpId,BYTE_T lbyPos);
BYTE_T mit_CheckCarsToLobby(BYTE_T grpId,BYTE_T lbyPos);
//void   mit_ParkTask(void);
//void   mit_UpdateTimers(void);
BYTE_T mit_CarPreNoDemand(BYTE_T car,BYTE_T mitDir);
//void   mit_AdjustRRT(BYTE_T car,BYTE_T deck,BYTE_T callPos,INT_T callMask,
//                     BYTE_T coincLevel,BYTE_T coincType,INT_T *rrt);
//void   mit_NoLobbyCarRRT(BYTE_T car,BYTE_T deck,BYTE_T callPos,INT_T callMask,
//                     BYTE_T coincLevel,BYTE_T coincType,INT_T *rrt);
void   mit_ActualPositionEvent(BYTE_T car);
//void   mit_DeckLoadWeightEvent(BYTE_T car,BYTE_T deck);
void   mit_AnyDemandEvent(BYTE_T car,BYTE_T state);
void   mit_DebugMessages(BYTE_T ori);
void   mit_DebugMessages2(BYTE_T car,BYTE_T pos,BYTE_T oldPri,BYTE_T pri,BYTE_T typ);
void   mit_SendPriority(BYTE_T car, BYTE_T pos, BYTE_T grp, BYTE_T priority);

#endif  /* end of include file definition */

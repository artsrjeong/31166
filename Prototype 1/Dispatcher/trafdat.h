/**************************************************************************
;  File Name: TRAFDAT.H - Public Interface for TRAFDAT.C
;
;  File Description:
;  This file describes the varmit.c public definitions.
;
;  This work and the information it contains are the confidential
;  property of Otis Elevator Company ("Otis").  It is delivered on
;  the express condition that it will be used only for, or on
;  behalf of, Otis; that neither it nor the information it contains
;  will be reproduced, distributed or disclosed, in whole or in part,
;  without the written consent of Otis; and that on demand it and any
;  copies will be promptly returned.
;
;  Unpublished work - Copyright 1997 Otis Elevator Company
;  All rights reserved.
;
;
;  Revision History:
;
;  SCN               Author                      Date
;  ------------------------------------------------------------------------
;  AAB-30299-2AA     Theresa Christy             25 June 1997
;  - SCR 30299-103   Create initial file
;
;  AAB-30299-3AA     Mark Ross                   15 July 1997
;  - SCR 30299-127   - Modified to support new CC and HC serviceability events
;                    - Cleared up warnings and informationals
;
;  AAB-30299-4AA     Theresa Christy/Kate Goldstone      06-Feb-1998
;  - CHG 30299-80.   Changed function calls to reflect new
;                    parameter list needed to support Variable MOT
;*************************************************************************/
#ifndef TRAFDAT_INCLUDED  /* protects from multiple includes */
#define TRAFDAT_INCLUDED


/**************************************************************************
;  Constants
;*************************************************************************/
#define trafdat_TotalInterval 5


/**************************************************************************
;  Data Types
;*************************************************************************/


/**************************************************************************
;  Variables
;*************************************************************************/
EXTERN  INT_T   trafdat_DownToLobbyPsngrCount[trafdat_TotalInterval + 1];
EXTERN  INT_T   trafdat_DownToLobbyCarCallCount[trafdat_TotalInterval + 1];
EXTERN  INT_T   trafdat_UpFromLobbyPsngrCount[trafdat_TotalInterval + 1];
EXTERN  INT_T   trafdat_UpFromLobbyCarCallCount[trafdat_TotalInterval + 1];
EXTERN  INT_T   trafdat_InterfloorPsngrCount[trafdat_TotalInterval + 1];
EXTERN  INT_T   trafdat_InterfloorCarCallCount[trafdat_TotalInterval + 1];
EXTERN  INT_T   trafdat_LobbyDepartureCount[trafdat_TotalInterval + 1];
EXTERN  INT_T   trafdat_LobbyDepartureLoadWeight[trafdat_TotalInterval + 1];
EXTERN  INT_T   trafdat_LobbyArrivalCount[trafdat_TotalInterval +1];
EXTERN  INT_T   trafdat_LobbyArrivalLoadWeight[trafdat_TotalInterval +1];

EXTERN  INT_T totalLobbyArrivalPassengers,totalLobbyDeparturePassengers,totalInterfloorPassengers;
EXTERN  INT_T totalLobby2ArrivalPassengers,totalLobby2DeparturePassengers;
EXTERN  INT_T totalInterfloorBoardPassengers,totalInterfloorDeboardPassengers;
EXTERN  INT_T totalFromLobbyCarCalls,totalToLobbyCarCalls,totalInterfloorCarCalls;
EXTERN  INT_T totalFromLobby2CarCalls,totalToLobby2CarCalls;
EXTERN  INT_T totalBoardPassengers,totalDeboardPassengers;
EXTERN  INT_T trafdat_AveUpFromLobbyPsngr,trafdat_RateOfBuildingUpPsngr;


EXTERN  BYTE_T trafdat_MITCarsNeeded,trafdat_OldMITCarsNeeded;
EXTERN  BYTE_T trafdat_EsoOneMinuteCounter,trafdat_EsoFiveMinuteCounter;
EXTERN  INT_T   trafdat_Eso5minRateData[trafdat_TotalInterval +1];
EXTERN  FLAG_T trafdat_EsoDataFlag,trafdat_EsoVelocityFlag,trafdat_EsoReduceCarFlag;
EXTERN  BYTE_T trafdat_EsoMaxLevel,trafdat_EsoCarsNeeded,trafdat_OldEsoCarsNeeded;
EXTERN  BYTE_T trafdat_EsoCarsRequestMask;		
EXTERN  FLAG_T trafdat_OldOpModeSetTest;

EXTERN  BYTE_T trafdat_EsoCarsAvailableMask,trafdat_EsoOldCarsAvailableMask,
               trafdat_EsoCarsAvailableCount,trafdat_EsoOldCarsAvailableCount;

EXTERN  INT_T  trafdat_MitTrendStartTime,trafdat_MitTrendStopTime;
EXTERN  INT_T  trafdat_InMitTrendTime;

EXTERN  BYTE_T trafdat_MitCarPriorityBit;


#endif  /* end of include file definition */

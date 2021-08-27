/* SCCS:@(#)dpc.h	1.1	09/12/01 */
/*-----------------------------------------------------------------%BSH%----
|  File Name:  DPC.H - Public Data Definitions for DPC.C
|
|  File Description:
|  This file describes the DPC.C public data.  This file is to be
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
|  JAB30203AAJ           Hideyuki Honma             12 Sep 2001
|  - Created program
|----------------------------------------------------------------%ESH%-----*/


#ifndef DPC_INCLUDED  // protects from multiple includes 
#define DPC_INCLUDED

/*------------------------------------------------------------------------
|  Constants
|-------------------------------------------------------------------------*/
#define         Sector1          0
#define         Sector2          1
#define         Sector3          2
#define         Sector4          3
#define         Sector5          4
#define         Sector6          5
#define         Sector7          6
#define         MAX_DPC_SECTOR   7
#define         Sector_Invalid   15

#define         DPC_INVALID     255


#define         DPC_STATIC       0
#define         DPC_FIXED        1


/*------------------------------------------------------------------------
|  Data Types
|-------------------------------------------------------------------------*/

/*------------------------------------------------------------------------
|  Variables
|-------------------------------------------------------------------------*/


//EXTERN FLAG_T park_AvailableFlag[MAX_OCSS];
EXTERN BYTE_T dpc_SectorHave[MAX_OCSS];
EXTERN BYTE_T dpc_SectorOccupied[MAX_DPC_SECTOR+1];
EXTERN BYTE_T dpc_BelongSector[JICA_FLOORS];
EXTERN FLAG_T dpc_EnableFlag,dpc_EnableFlag2,dpc_TriggerFlag,dpc_ConditionFlag;
EXTERN FLAG_T dpc_ChnType,dpc_FixedFlag,dpc_FixedCars,dpc_DataBase;
EXTERN BYTE_T dpc_SectorNumber,dpc_LobbyPos,dpc_AvailableCars;

 struct   dpc_SectorS
          {
             BYTE_T bot;
             BYTE_T top;
             BYTE_T penalty;
             BYTE_T pos;
             BYTE_T priority;
             FLAG_T dpcFlag;
          }; 

 typedef struct dpc_SectorS dpc_SectorT;
EXTERN  dpc_SectorT  dpc_Sector[MAX_DPC_SECTOR+1];


/*------------------------------------------------------------------------
|  Function Prototypes
|-------------------------------------------------------------------------*/

void    dpc_Init(void),dpc_Init2(void);
void    dpc_SectorTest(void);
void    dpc_SendSectorInfo(void);
void    dpc_SendCarSector(BYTE_T car,BYTE_T sec,FLAG_T assign);
void    dpc_SendOneSectorInfo(BYTE_T sec);
FLAG_T  dpc_DownHallCallHave( BYTE_T cr);
void dpc_ClusterRelease(BYTE_T  cr);

#endif  // end of include file definition 

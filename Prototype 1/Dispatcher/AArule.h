/* U10 Project: @(#)aarule.h	2.10	3/23/95	14:22:27 */
/**************************************************************************
;  File Name: AARULE.H - Public Interface for AARULE.C
; 
;  File Description:
;  This file describes the AARULE.C public definitions.  This file is to be
;  included in any file that uses these services.  See the AARULE.C file
;  for detailed usage information.
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
;  Unpublished work - Copyright 1992 - 1995 Otis Elevator Company
;  All rights reserved.
;
; 
;  Revision History:
; 
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  AAA30xxxAAA           Mark Ross            	     05-NOV-93
;
;  A1230203BAA           Mark Ross                   18-FEB-94
;  - Revised and extended AARULE to handle Downpeak.
;
;  A1230203BAA           Geoff Mochau                18-FEB-94
;  - Changed Downpeak action rule symbols.
;
;  A1330203BAA		 Mark Ross		     14-Apr-94
;  - Added AARULE_Find_Next_Demand and AARULE_Map_Demand prototypes. (STR#86)
;
;  A1630203BAA           Paul Simcik/Mark Ross         08 August 1994
;  - SCR 30203-107
;    Modified to support seperation of traffic density and autoadjust
;    features.
;
;  A1730203BAA           Mark Ross       	     18 November 1994
;  - SCR 30203-122.
;    Modified to allow all AARULES to use traffic density info
;
;*************************************************************************/
#ifndef AARULE_INCLUDED  /* protects from multiple includes */
#define AARULE_INCLUDED

/**************************************************************************
;  Constants
;*************************************************************************/
#define AARULE_PARKED_CARS   		1 
#define AARULE_CONTIGUOUS_STOPS   	2 
#define AARULE_CAR_LOAD   		3 
#define AARULE_DEMAND_BEYOND_HC   	4 
#define AARULE_DOWNPEAK		   	5 

#define AARULE_COINC   		1 
#define AARULE_SVCWAIT   	2 
#define AARULE_WAIT   		3 
#define AARULE_RRT	     	4 
#define AARULE_RESERVE		   	5 

#define DOWNPEAK_CLUSTER_MAX       8

#define DOWNPEAK_INIT_VALUE        127

#define DOWNPEAK_NO_CLUSTER        0
#define DOWNPEAK_CLUSTER_ASSIGN    1
#define DOWNPEAK_CLUSTER_INCREASE  2
#define DOWNPEAK_CLUSTER_DECREASE  3
#define DOWNPEAK_CLUSTER_REDEFINE  4
#define DOWNPEAK_CLUSTER_EXIT      5
#define DOWNPEAK_SWAP_CLUSTERS     6
#define DOWNPEAK_SWAP_MODES_IN     7
#define DOWNPEAK_SWAP_MODES_OUT    8

#define DOWNPEAK_DHC_AHEAD_1       11
#define DOWNPEAK_DHC_AHEAD_2_1     121
#define DOWNPEAK_DHC_AHEAD_2_2     122
#define DOWNPEAK_DHC_AHEAD_2_3     123
#define DOWNPEAK_DHC_AHEAD_2_4     124
#define DOWNPEAK_DHC_AHEAD_2_5     125
#define DOWNPEAK_DHC_BEHIND_1      21
#define DOWNPEAK_DHC_BEHIND_2      22
#define DOWNPEAK_DHC_BEHIND_3      23
#define DOWNPEAK_DHC_BEHIND_4      24
#define DOWNPEAK_DHC_BEHIND_5      25
#define DOWNPEAK_UHC_1             31
#define DOWNPEAK_UHC_2             32
#define DOWNPEAK_UHC_3             33
#define DOWNPEAK_DHC_LUCKY_1       41
#define DOWNPEAK_DHC_LUCKY_2       42
#define DOWNPEAK_UHC_LUCKY_1       51
#define DOWNPEAK_DHC_COINCIDENT_1  61
#define DOWNPEAK_UHC_COINCIDENT_1  71

/**************************************************************************
;  Data Types
;*************************************************************************/
struct AARULE_CALL_S  {
                BYTE_T          car;
                BYTE_T          reason;
};
typedef struct AARULE_CALL_S AARULE_CALL_T;

struct AARULE_LANDING_S  {
		AARULE_CALL_T	dir[2];
};
typedef struct AARULE_LANDING_S AARULE_LANDING_T;

struct DOWNPEAK_ACTIVITY_S         /* Predirection Activity Structure */
  {
    BYTE_T car;
    BYTE_T direction;
    BYTE_T floor;
    BYTE_T rule;
    BYTE_T action;
    BYTE_T object;
  };

typedef struct DOWNPEAK_ACTIVITY_S DOWNPEAK_ACTIVITY_T;

/**************************************************************************
;  Variables
;*************************************************************************/
EXTERN BYTE_T AARULE_Best_Car;
EXTERN INT_T  AARULE_Total_HC_Cnt;	/* HC's assigned by ICSS2          */
EXTERN INT_T  AARULE_Affected_HC_Cnt;	/* HC's affected by add. rules     */
EXTERN INT_T  AARULE_Modified_HC_Cnt;	/* HC's assignments modified       */ 
EXTERN INT_T  AARULE_Park_Mod_Cnt;	/* Mod due to Parking rule         */
EXTERN INT_T  AARULE_Load_Mod_Cnt;	/* Mod due to Car Load rule        */
EXTERN INT_T  AARULE_Contiguos_Mod_Cnt;	/* Mod due to Contiguous rule      */
EXTERN INT_T  AARULE_Demand_Beyond_Cnt;	/* Mod due to Demand Beyond rule   */
EXTERN INT_T  AARULE_Downpeak_Mod_Cnt;	/* Mod due to Downpeak rules       */
EXTERN INT_T  AARULE_PLCD_Tie_Cnt;	/* Rules applied but there was tie */
EXTERN BYTE_T AARULE_Mask;		/* Direction of current assignment */
EXTERN BYTE_T AARULE_Landing;		/* Landing of current assignment   */
EXTERN BYTE_T AARULE_Coincident_CC[9];  /* CCC flag array                  */
EXTERN AARULE_LANDING_T AARULE_activity[MAX_LANDINGS]; /* AARULE Activity  */
EXTERN FLAG_T AARULE_Priority_Update;   /* Update AARULE EEPROM values     */
EXTERN BYTE_T AARULE_Update_Cnt;        /* Set of AARULE EEPROM values     */

EXTERN DOWNPEAK_ACTIVITY_T downpeakLastActivity[9];/* Last activities      */
EXTERN BYTE_T downpeakClusterCount;                /* Number of clusters   */
EXTERN BYTE_T downpeakClusterAssignments[9];   /* Cluster -> car           */
EXTERN BYTE_T downpeakClusterDefinition[9];    /* Cluster -> top of cluster*/
EXTERN INT_T  AARULE_DnpkRule11Cnt;	/* Rule 11 Counter		   */
EXTERN INT_T  AARULE_DnpkRule121Cnt;	/* Rule 121 Counter		   */
EXTERN INT_T  AARULE_DnpkRule122Cnt;	/* Rule 122 Counter		   */
EXTERN INT_T  AARULE_DnpkRule123Cnt;	/* Rule 123 Counter		   */
EXTERN INT_T  AARULE_DnpkRule124Cnt;	/* Rule 124 Counter		   */
EXTERN INT_T  AARULE_DnpkRule125Cnt;	/* Rule 125 Counter		   */
EXTERN INT_T  AARULE_DnpkRule21Cnt;	/* Rule 21 Counter		   */
EXTERN INT_T  AARULE_DnpkRule22Cnt;	/* Rule 22 Counter		   */
EXTERN INT_T  AARULE_DnpkRule23Cnt;	/* Rule 23 Counter		   */
EXTERN INT_T  AARULE_DnpkRule24Cnt;	/* Rule 24 Counter		   */
EXTERN INT_T  AARULE_DnpkRule25Cnt;	/* Rule 25 Counter		   */
EXTERN INT_T  AARULE_DnpkRule31Cnt;	/* Rule 31 Counter		   */
EXTERN INT_T  AARULE_DnpkRule32Cnt;	/* Rule 32 Counter		   */
EXTERN INT_T  AARULE_DnpkRule33Cnt;	/* Rule 33 Counter		   */
EXTERN INT_T  AARULE_DnpkRule41Cnt;	/* Rule 41 Counter		   */
EXTERN INT_T  AARULE_DnpkRule42Cnt;	/* Rule 42 Counter		   */
EXTERN INT_T  AARULE_DnpkRule51Cnt;	/* Rule 51 Counter		   */
EXTERN INT_T  AARULE_DnpkRule61Cnt;	/* Rule 61 Counter		   */
EXTERN INT_T  AARULE_DnpkRule71Cnt;	/* Rule 71 Counter		   */
EXTERN INT_T  AARULE_ContiguousStopsRRT;   /* Auto adjusted RRT value      */
EXTERN INT_T  AARULE_ContiguousStopsPWT;   /* Auto adjusted PWT value      */
EXTERN INT_T  AARULE_ParkingRRT;   	   /* Auto adjusted RRT value      */
EXTERN INT_T  AARULE_ParkingPWT;   	   /* Auto adjusted RRT value      */
EXTERN INT_T  AARULE_DownpeakRRT;   	   /* Auto adjusted RRT value      */
EXTERN INT_T  AARULE_DownpeakPWT;   	   /* Auto adjusted RRT value      */
EXTERN INT_T  AARULE_CarLoadRRT;   	   /* Auto adjusted RRT value      */
EXTERN INT_T  AARULE_CarLoadPWT;   	   /* Auto adjusted RRT value      */
EXTERN INT_T  AARULE_DemandBeyondRRT;      /* Auto adjusted RRT value      */
EXTERN INT_T  AARULE_DemandBeyondPWT;      /* Auto adjusted RRT value      */
EXTERN FLAG_T bFirstAssign;              /* first assign */

/**************************************************************************
;  Function Prototypes
;*************************************************************************/
BYTE_T  AARULE_Driver(BYTE_T, BYTE_T,BYTE_T, INT_T *, INT_T *, INT_T *);
void    AARULE_Find_Next_Demand(BYTE_T, BYTE_T, BYTE_T *, BYTE_T *);
void    AARULE_Map_Demand(BYTE_T *, BYTE_T); 
    
/**************************************************************************
;  Macro Functions  (In-Line Code)
;*************************************************************************/
/* None */

#endif  /* end of include file definition */


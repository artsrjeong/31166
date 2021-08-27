/* U10 Project: @(#)autoadj.h	2.6     3/23/95     14:22:32 */
/**************************************************************************
;  File Name: AUTOADJ.H - Public Interface for AUTOADJ.C
; 
;  File Description:
;
;  Contains public declarations of autoadjust-related parameters
;  and literals.
; 
;  This work and the information it contains are the confidential
;  property of Otis Elevator Company ("Otis").  It is delivered on
;  the express condition that it will be used only for, or on
;  behalf of, Otis; that neither it nor the information it contains
;  will be reproduced, distributed or disclosed, in whole or in part,
;  without the written consent of Otis; and that on demand it and any
;  copies will be promptly returned.
;
;  Unpublished work - Copyright 1993 - 1995 Otis Elevator Company
;  All rights reserved.
;
; 
;  Revision History:
; 
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  A1630133BAA           Jeremy Kezer                  16 November 1993
;  - Created file.
;
;
;  A1430203BAA           Paul Simcik                   03 May 1994
;  - SCR 30203-52
;    Autoadjust related variables and constants previously defined in
;    "e2p_p.h" moved to this header file.
;
;
;  A1530203BAA           Mark Ross                     07 July 1994
;  - SCR 30203-104
;    Modified to allow up to 20 (X,Y) pairs to be used to define each 
;    fuzzy set.
;
;  A1530203BAA           Paul Simcik/Mark Ross         07 July 1994
;  - SCR 30203-107
;    Modified to support enhanced traffic density feature.
;
;  A1630203BAA           Paul Simcik/Mark Ross         08 August 1994
;  - SCR 30203-107
;    Modified to support seperation of traffic density and autoadjust
;    features.
;
;  A1730203BAA           Mark Ross       	  28 November 1994
;  - SCR 30203-118.
;    Modified to vary fuzzy sets by adjusting coordinate sets, not the
;    fuzzy sets themselves.
;
;  A1730203BAA           Mark Ross       	     18 November 1994
;  - SCR 30203-122.
;    Modified to allow all AARULES to use traffic density info
;
;*************************************************************************/
#ifndef AUTOADJ_INCLUDED  /* protects from multiple includes */
#define AUTOADJ_INCLUDED

/**************************************************************************
;  Constants
;*************************************************************************/
#define   ADJ_ODD_ARRAYS        0
#define	  ADJ_EVEN_ARRAYS       1
#define	  RRT_X_UPPER		120
#define	  RRT_X_LOWER		0
#define	  RRT_Y_UPPER		100
#define	  RRT_Y_LOWER		0
#define	  PWT_X_UPPER		150
#define	  PWT_X_LOWER		0
#define	  PWT_Y_UPPER		100
#define	  PWT_Y_LOWER		0 
#define autoadj_DnpkRRTPacketId		't'
#define autoadj_DnpkPWTPacketId		'v'
#define autoadj_ContigRRTPacketId	'w'
#define autoadj_ContigPWTPacketId	'x'
#define autoadj_ParkRRTPacketId		'y'
#define autoadj_ParkPWTPacketId		'z'
#define autoadj_LoadRRTPacketId		'{'
#define autoadj_LoadPWTPacketId		'|'
#define autoadj_DemandRRTPacketId	'}'
#define autoadj_DemandPWTPacketId	'~'
#define autoadj_CoordinateSetPacketId	'B'


/**************************************************************************
;  Data Types
;*************************************************************************/
struct ADJ_ADJUST_RRT_S {
			BYTE_T low[RRT_X_UPPER + 1][2];
			BYTE_T medium[RRT_X_UPPER + 1][2];
			BYTE_T high[RRT_X_UPPER + 1][2];
                   };

struct ADJ_ADJUST_PWT_S {
                        BYTE_T none[PWT_X_UPPER + 1][2];
                        BYTE_T elderly[PWT_X_UPPER + 1][2];
                        BYTE_T prolonged[PWT_X_UPPER + 1][2];
                    };

struct ADJ_ADJUSTED_S {
                        struct ADJ_ADJUST_RRT_S rrt;
                        struct ADJ_ADJUST_PWT_S pwt;
		  };

typedef struct ADJ_ADJUSTED_S ADJ_ADJUSTED_T;
/**************************************************************************
;  Variables
;*************************************************************************/

EXTERN BYTE_T autoadj_current_array;
EXTERN ADJ_ADJUSTED_T ADJ_AdjustedSets;
EXTERN BYTE_T RRT_Short[RRT_X_UPPER + 1];
EXTERN BYTE_T RRT_Improv[RRT_X_UPPER + 1];
EXTERN BYTE_T ADJ_RRTMediumCoordinateSet[40];
EXTERN BYTE_T ADJ_PWTElderlyCoordinateSet[40];
EXTERN BYTE_T ADJ_PWTLongCoordinateSet[40];

/**************************************************************************
;  Function Prototypes
;*************************************************************************/
void ADJ_VaryFuzzySets(INT_T, BYTE_T);
void ADJ_VaryCoordinateSet(BYTE_T *, BYTE_T *, INT_T, BYTE_T, BYTE_T *);
void ADJ_VaryRRTandPWT(INT_T, BYTE_T, INT_T *,INT_T *,BYTE_T, BYTE_T, 
		      BYTE_T, BYTE_T, BYTE_T); 

#endif  /* end of include file definition */

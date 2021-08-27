/* U10 Project: @(#)density.h	1.4     3/23/95     14:22:36 */
/**************************************************************************
;  File Name: DENSITY.H - Public Interface for DENSITY.C
; 
;  File Description:
;
;  Contains public declarations of traffic density related parameters
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
;  Unpublished work - Copyright 1994 - 1995 Otis Elevator Company
;  All rights reserved.
;
; 
;  Revision History:
; 
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  A1630203BAA           Paul Simcik/Mark Ross       05 August 1994
;  - SCR 30203-107
;    Created module. Formerly part of AUTOADJ.H. Created new module to 
;    seperate traffic density feature from auto adjust feature. Based on
;    A1530203BAA version of AUTOADJ.H module.
;
;  A1730203BAA           Mark Ross       	     08 November 1994
;  - SCR 30203-119.
;    Modified Traffic Denisty to use Traffic Trend information.
;
;  A1730203BAA           Mark Ross       	     28 November 1994
;  - SCR 30203-118.
;    Modified to vary fuzzy sets by adjusting coordinate sets, not the
;    fuzzy sets themselves. Modified to change all fuzzy set definitions
;    from int type to byte type.
;
;*************************************************************************/
#ifndef DENSITY_INCLUDED  /* protects from multiple includes */
#define DENSITY_INCLUDED


/**************************************************************************
;  Constants
;*************************************************************************/
#define   DENSITY_X_UPPER           100
#define   DENSITY_X_LOWER           0
#define   DENSITY_Y_UPPER           100
#define   DENSITY_Y_LOWER           0
#define   DENSITY_LEN_TYPE          8
#define   DENSITY_INTERVALS_TYPE    7
#define   DENSITY_MAX_INTERVALS     20
#define   DENSITY_MIN_INTERVALS     2
#define   DENSITY_ODD_ARRAYS        0
#define	  DENSITY_EVEN_ARRAYS       1


/**************************************************************************
;  Data Types
;*************************************************************************/
struct DENSITYRES_S {
			BYTE_T HCOver30secMembership;
			BYTE_T HCAveRegTimeMembership;
			BYTE_T CCEnteredPerCarMembership;
			BYTE_T HCEnteredPerCarMembership;
			BYTE_T CCPresentPerCarMembership;
			BYTE_T HCPresentPerCarMembership;
			BYTE_T TrafficDensity;
                 };

struct DENSITY_S {
   			BYTE_T  hallCallsOver30SecCount;
			INT_T   hallCallsRegTimeSum;
			FLOAT_T hallCallsMaxRegTime;
			INT_T	hallCallsEntered;
			INT_T	hallCallsPresent;
			INT_T	hallCallsPresentMax;
			INT_T	hallCallsAnswered;
			INT_T	carCallsEntered;
			INT_T	carCallsPresent;
			INT_T	carCallsPresentMax;
                 };

/**************************************************************************
;  Variables
;*************************************************************************/
EXTERN struct DENSITYRES_S density_results;
EXTERN struct DENSITY_S    density_data[DENSITY_MAX_INTERVALS+1];
EXTERN BYTE_T              density_current_interval;


EXTERN BYTE_T  DENSITY_30Sec_Set[DENSITY_X_UPPER + 1];
EXTERN BYTE_T  DENSITY_HC_Entered_Percar_Set[DENSITY_X_UPPER + 1];
EXTERN BYTE_T  DENSITY_HC_Present_Percar_Set[DENSITY_X_UPPER + 1];
EXTERN BYTE_T  DENSITY_CC_Entered_Percar_Set[DENSITY_X_UPPER + 1];
EXTERN BYTE_T  DENSITY_CC_Present_Percar_Set[DENSITY_X_UPPER + 1];
EXTERN BYTE_T  DENSITY_AvgReg_Set[DENSITY_X_UPPER + 1];
EXTERN BYTE_T  DENSITY_Longest_PWT_Set[DENSITY_X_UPPER + 1];
EXTERN INT_T   DENSITY_CCEnteredPerCarSummary;
EXTERN INT_T   DENSITY_CCPresentPerCarSummary;
EXTERN INT_T   DENSITY_HCEnteredPerCarSummary;
EXTERN INT_T   DENSITY_HCPresentPerCarSummary;
EXTERN BYTE_T  DENSITY_Over30SecRatioSummary;
EXTERN S_INT_T DENSITY_NumberOfIntervalsToUse;
EXTERN INT_T   DENSITY_AveRegTimeSummary;
EXTERN FLAG_T  DENSITY_TrafficTrendIncreasing;

EXTERN INT_T   test_CCE;
EXTERN INT_T   test_CCP;
EXTERN INT_T   test_CCA;
EXTERN INT_T   test_CCPM;
EXTERN INT_T   test_HCE;
EXTERN INT_T   test_HCP;
EXTERN INT_T   test_HCPM;
EXTERN INT_T   test_HCA;
EXTERN INT_T   test_HC30SEC;
EXTERN INT_T   test_HCREGSUM;
EXTERN INT_T   test_DENSITY;



#endif  /* end of include file definition */

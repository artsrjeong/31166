/* U10 Project: @(#)asslib.h	1.2	3/23/95	14:22:30 */
/**************************************************************************
;  File Name: ASSLIB.H - Public Interface for ASSLIB.C
; 
;  File Description:
;  This file describes the asslib.c public definitions.
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
;  Unpublished work - Copyright 1994 - 1995 Otis Elevator Company
;  All rights reserved.
;
; 
;  Revision History:
;
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  A1730203BAA           Mark Ross                     19 December 1994
;  - SCR 30203-139
;    ICA.C breakup. Made appropriate changes for function invocations and
;    breakup of the ICA.C module.
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;  J1830203AAA           H.Honma                         26   May  1995
;  - SCR 30203-5013: 
;    Change return codes from ica_car_available to distinguish Normal/Overload
;    conditions/Out of Group/Other status of car based on Operational Mode. 
;
;  J1830203AAA           H.Honma                         26   May  1995
;  - SCR 30203-5017
;    Support Wheelchair Mode as Extended calls 
;    Change call mask variables type
;
;  J1830203AAA           H.Honma                         26   May  1995
;  - SCR 30203-5014
;    Add asslib_CheckLNSOLD() and asslib_CheckLNStimer for reassign
;
;  J1830203AAA           H.Honma                         08   June  1995
;  - SCR 30203-5002
;    Support Dual Up Peak
;
;  - SCR 30203-5057      H.Honma                       25 Sep 1995
;    WCS Mode Operation Service         
;    
;  -  AAB302032AA        Mark Ross                   20 Oct 1995
;    SCR 30203-244: OTI SCR 5057. WCS Mode Operation Service   
;         
;*************************************************************************/
#ifndef ASSLIB_INCLUDED  /* protects from multiple includes */
#define ASSLIB_INCLUDED

#ifndef CAGS
#include "infocs.h"
#endif

/**************************************************************************
;  Constants
;*************************************************************************/

#define	 asslib_NormalMode     1
#define  asslib_LNSOLDMode     2
#define  asslib_GroupOutMode   3
#define  asslib_NotAvailable   0

 
#define  asslib_STANDARD_CALL  1
#define  asslib_EXTENDED_CALL  2

//waitind
#define  WaitOnlyLantern       0
#define  WaitOneLevel          1
#define  WaitTwoLevel          2
#define  WaitThreeLevel        3


#define		DBD_UpDir	0	/* Up direction			*/
#define		DBD_DownDir	1	/* Down direction		*/
#define		DBD_MaxDir	2	/* Max Directions               */

#define         DBD_SUCC        0x01
#define         DBD_SDCC        0x02

/*
#define         DBD_SFUCC        0x01   //Standard Front Up    Dst Call 
#define         DBD_SFDCC        0x02   //Standard Front Down  Dst Call
#define         DBD_SRUCC        0x04   //Stnndard Rear  Up    Dst Call
#define         DBD_SRDCC        0x08   //Standard Rear  Down  Dst Call
#define         DBD_EFUCC        0x10   //Extend   Front Up    Dst Call
#define         DBD_EFDCC        0x20   //Extend   Front Down  Dst Call
#define         DBD_ERUCC        0x40   //Extend   Rear  Up    Dst Call
#define         DBD_ERDCC        0x80   //Extend   Rear  Down  Dst Call
*/

//DBD Coincident Calls
#define         CC_HCwithCC         0x01   //HallCall with ExistCarCall
#define         CC_HCwithDCC        0x02   //HallCall with ExpectCarCall
#define         CC_HCwithHC         0x04   //HallCall with HallCall (Source Coincident)
#define         CC_DCCwithCC        0x10   //ExpectCarCall with ExistCarCall
#define         CC_DCCwithDCC       0x20   //ExpectCarCall with ExpectCarCall
#define         CC_DCCwithHC        0x40   //ExpectCarCall with HallCall

#define         CC_HC         CC_HCwithCC + CC_HCwithDCC + CC_HCwithHC
#define         CC_DCC        CC_DCCwithCC + CC_DCCwithDCC + CC_DCCwithHC

// BYTE type ,#define         landing_ServiceSize      ((MAX_LANDINGS+7)/8)   
#define         landing_ServiceSize      ((MAX_LANDINGS+31)/32)   

#define         AssignTypeImmediate   0x01   //ImmediateAssignLogic conventional way
#define         AssignTypePassCnts    0x02   //Multi Assign only PassengerCount over capacity
#define         AssignTypeMulti       0x04   //Multi Assign normally
#define         AssignTypeInvChannel  0x08   //Use invisible channeling 

/**************************************************************************
;  Data Types
;*************************************************************************/

struct  landing_ServiceStructT
{
  LONG_T   Mask[landing_ServiceSize]; 
};

typedef struct landing_ServiceStructT   landing_ServiceObjectT;

landing_ServiceObjectT*   asslib_ExpectedLandings();

/**************************************************************************
;  Variables
;*************************************************************************/
EXTERN BYTE_T asslib_CheckLNStimer;

#ifdef WAITIND
EXTERN BYTE_T asslib_WaitIndLevel[DOWN_DIR + 1][MAX_LANDINGS];
EXTERN BYTE_T asslib_WaitRawLevel[DOWN_DIR + 1][MAX_LANDINGS];
EXTERN BYTE_T asslib_WaitIndUpCurrentCnt[DOWN_DIR + 1][MAX_LANDINGS];
#endif

//64Kbyte
//BYTE_T asslib_DbdAssignCalls[MAX_OCSS][Dbd_MaxDir][JICA_FLOORS][JICA_FLOORS];
EXTERN BYTE_T asslib_DbdAssignCalls[MAX_OCSS][JICA_FLOORS][JICA_FLOORS];//0x00:None
                                                                        //0x01:SUCC:
                                                                        //0x02:SDCC: 
//512byte
EXTERN BYTE_T asslib_DbdExpectCalls[MAX_OCSS][JICA_FLOORS]; //0x00:None 0x01:Up 0x02:Down 0x03:Both
EXTERN BYTE_T asslib_DbdExpectCallsError[20];


EXTERN BYTE_T asslib_DbdAssignCallDirCnts[MAX_OCSS][DOWN_DIR+1][JICA_FLOORS];
EXTERN BYTE_T asslib_DbdAssignCallDstCnts[MAX_OCSS][JICA_FLOORS][JICA_FLOORS];
EXTERN BYTE_T asslib_DbdExpectCallCnts[MAX_OCSS][DOWN_DIR+1][JICA_FLOORS];
EXTERN BYTE_T asslib_DbdCarCallCnts[MAX_OCSS][JICA_FLOORS];
EXTERN BYTE_T asslib_DbdPeopleInCarCnts[MAX_OCSS];


EXTERN LONG_T asslib_AssignedCarDiff;
EXTERN LONG_T asslib_AssignPassCnt;
EXTERN LONG_T asslib_AssignedNoCarCnt;
EXTERN LONG_T asslib_AssignRrtMaxCnt;
EXTERN LONG_T asslib_AssignPwtMaxCnt;
EXTERN LONG_T asslib_CarKeepsAssign;
EXTERN LONG_T asslib_ImmDwellCnt;
EXTERN LONG_T asslib_ExtendDwellCnt;
EXTERN LONG_T asslib_RemoveDemandCnt;
EXTERN LONG_T asslib_RemoveOneDemCnt;
EXTERN LONG_T asslib_InvPsgrDataCnt;

/**************************************************************************
;  Function Prototypes
;*************************************************************************/
EXTERN void   asslib_ass_period_tsk(void);
EXTERN void   asslib_assign_call(BYTE_T car, BYTE_T callmask, BYTE_T pos);
EXTERN void   asslib_deassign_call(BYTE_T car, BYTE_T callmask, BYTE_T pos);
EXTERN FLAG_T asslib_car_available(BYTE_T car);
EXTERN BYTE_T asslib_count_normal_cars(void);
EXTERN void   asslib_make_all_calls_new(BYTE_T car);
EXTERN void   asslib_make_one_call_new(BYTE_T car, BYTE_T landing, BYTE_T callmask);
EXTERN void   asslib_make_one_dup_new(BYTE_T car, BYTE_T landing, BYTE_T callmask);
EXTERN FLAG_T asslib_check_allowed_mask_rx (void);
EXTERN void   asslib_update_summary (BYTE_T car);
EXTERN FLAG_T asslib_check_delayed_cars(void);
EXTERN FLAG_T asslib_check_DHBDOS_cars(BYTE_T car, BYTE_T calls_dir, BYTE_T pos);
EXTERN FLAG_T asslib_check_EHS_cars(BYTE_T car, BYTE_T calls_dir, BYTE_T pos);
EXTERN FLAG_T asslib_rrt_hc_assigned(BYTE_T car, BYTE_T start, BYTE_T finish, 
			      INT_T mask);
EXTERN FLAG_T asslib_check_fully_loaded(BYTE_T car, BYTE_T calls_dir, BYTE_T pos);
EXTERN BYTE_T asslib_calculate_pre_dir(BYTE_T car);
EXTERN void   asslib_CheckLNSOLD(BYTE_T car);
EXTERN FLAG_T asslib_check_wheel_chair(BYTE_T car, BYTE_T calls_dir, BYTE_T pos);
EXTERN void   asslib_CheckWCS(BYTE_T car);
EXTERN void   asslib_CheckDHBDOS(BYTE_T car);
EXTERN void   asslib_CheckEHS(BYTE_T car);
EXTERN FLAG_T asslib_AnotherCarAvailable(BYTE_T car,BYTE_T callDir,BYTE_T callPos);

EXTERN void   asslib_addOneAssignCar(BYTE_T car, BYTE_T landing, BYTE_T callmask);

//waitInd
EXTERN BYTE_T asslib_WaitCheckLevel(BYTE_T rrt);
EXTERN void asslib_XmitCarWaitLevel(BYTE_T car, BYTE_T dir, BYTE_T pos, BYTE_T level);


EXTERN FLAG_T asslib_CheckSemiGroupOutCars(BYTE_T car, BYTE_T calls_dir, BYTE_T pos);
EXTERN void   asslib_CheckSemiGroupOut(BYTE_T car);
EXTERN FLAG_T asslib_CheckForRecoverCar(BYTE_T car);
EXTERN void   asslib_IdleCheck(BYTE_T car);

EXTERN BYTE_T asslib_GetAssignedCar(BYTE_T callPos,BYTE_T callDst,BYTE_T callMask,BYTE_T riser);
EXTERN FLAG_T asslib_ValidExpectCalls(BYTE_T car,BYTE_T dir,BYTE_T pos);
EXTERN FLAG_T asslib_ValidExpectCalls2(BYTE_T car,BYTE_T dir,BYTE_T start,BYTE_T finish, BYTE_T pos);

EXTERN void   asslib_DbdAssignCall(BYTE_T car, BYTE_T callmask, BYTE_T orgPos, BYTE_T dstPos);
EXTERN void   asslib_DbdDeassignCall(BYTE_T car, BYTE_T callmask, BYTE_T orgPos, BYTE_T dstPos);

EXTERN void   asslib_DbdSendExpectCalls(BYTE_T car,BYTE_T carDir,BYTE_T callMask,BYTE_T posTyp);
EXTERN void   asslib_DbdClearAllExpectedCalls(BYTE_T carId);
EXTERN void   asslib_DbdClearAllHallCalls(BYTE_T carId);
EXTERN void   asslib_DbdClearExpectedCalls(BYTE_T carId, BYTE_T srcPos);
EXTERN void   asslib_DbdClearHallCalls(BYTE_T carId, BYTE_T srcPos);

EXTERN void  asslib_OneWordToTwoBytes(WORD_T source, BYTE_T *destination);
EXTERN void  asslib_TwoBytesToOneWord(BYTE_T *source, WORD_T *destination);


#endif  /* end of include file definition */

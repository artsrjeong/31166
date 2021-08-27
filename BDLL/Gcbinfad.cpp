#include "stdafx.h"


unsigned char ads_stateLog[24],ads_stateActionLog[8],ads_Tx[5],ads_WriteFlag,ads_WriteDeb1;
/**************************************************************************
; File Name: GCBINFAD.C - ADSS/PU Communications Handler
;
; File Description:
; This file contains modules that handle communications with the ADSS/PU.
; These files are reentrant.
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
; Revision History:
;
;  SCN                   Author                        Date
;  ------------------------------------------------------------------------
;  AAB-30203-3AA         Geoff Mochau                  06 November 1995
;  - SCR 30203-210: Change module names to reflect hardware change-over
;    to OTI's GCB.  This module is derived from version 3.1 of "u10infad.c".
;
;*************************************************************************/
#include "common.h"
#ifdef CAGS
#include "global.lit"
#include "global.h"
#endif

/**************************************************************************
;  Public Declarations
;*************************************************************************/
#define EXTERN extern   /* declare public variables */
#include "infocs.h"
#include "inferr.h"
#include "sio.h"
#include "queue.h"
#include "infini.h"
#include "infics.h"
#include "infmsg.h"
#include "infbld.h"
#include "e2p.h"
#include "crisprrt.h"
#include "assign.h"
#include "asslib.h"
#include "crowd.h"
#undef EXTERN

/**************************************************************************
;  Public Definitions
;*************************************************************************/
#define EXTERN          /* define public variables */
#include "infads.h"
#undef EXTERN

/**************************************************************************
;  Local Definitions
;*************************************************************************/
#include "infads.lh"

#ifdef CAGS
#define ICSS2_ORIGIN    12
#define ADSS_PU_ORIGIN  15
#endif

//for Monitoring.
INT_T  ads_Ring1GetCount,ads_Ring1PutCount;
INT_T  ads_Ring1MaxGetCount,ads_Ring1MaxPutCount,ads_RxqPutCount,ads_RxqMaxPutCount;
INT_T  ads_RxqMaxGetCount,ads_RxqGetCount,ads_TxqPutCount,ads_TxqGetCount;
INT_T  ads_GetCount,ads_PutCount,ads_MaxGetCount,ads_MaxPutCount;
INT_T  ads_type_7_cnt,ads_read_len2,ads_read_len0,ads_read_len1,ads_read_len5;
INT_T  ads_state_chk1,ads_state_chk2;

/*-----------------------------------------------------------------------------
;******************************************************************************
;  L O C A L   A D S S / P U   C O M M U N I C A T I O N   F U N C T I O N S
;******************************************************************************
; Functions used by adss/pu communications task:
;
;	1.  ads_msg_init - select default set of msg. to trans. to ADSS.
;	2.  ads_init - initialize ads/pu communication related information.
;	3.  ads_update_led - update ads related leds.
;	4.  ads_decode_mask - decode message selection mask from ADSS/PU.
;	5.  ads_type_3 - handle type 3 msg's generated by adss/pu.
;	6.  ads_type_5 - handle type 5 msg's generated by adss/pu.
;	7.  ads_type_6 - handle type 6 msg's generated by adss/pu.
;	8.  decode_ads_msg - decode msg's generated by adss/pu.
;	9.  ads_actions - general purpose actions required to support comm.
;		with the adss/pu.
;	10. ads_transition_action - actions performed based on state
;		transitions.
;****************************************************************************
;---------------------------------------------------------------------------*/



#ifndef CAGS


/*-----------------------------------------------------------------------------
;******************************************************************************
;  	A D S S / P U     C O M M U N I C A T I O N S     T A S K
;******************************************************************************
;-----------------------------------------------------------------------------*/


#endif  

 /* 30203-504-1 +START+ create additional adss access routines */
 

/**************************************************************************
; Name: ads_LowestPosition
;
; Description: search for lowest position with demand or predicted demand
;
; Parameters:  car    
;              next_comm - next commitable floor
;              pos - new hall call position
;
;*************************************************************************/
BYTE_T ads_LowestPosition(BYTE_T car,BYTE_T next_comm,BYTE_T pos,FLAG_T expectCcCheckFlag)
{
  BYTE_T lowest_call_pos;
  lowest_call_pos = ocss_info[car].bottom_pos;

	//DBD
	while( (lowest_call_pos < next_comm) && (lowest_call_pos < pos) &&
		 ((ocss_info[car].assigned_calls[lowest_call_pos] &(S_CC+S_HC+E_CC+E_HC)) == 0 &&
		   (ocss_info[car].future_calls[lowest_call_pos] &(S_CC+S_HC+E_CC+E_HC)) == 0) 
		/*
		&&  05/02/24 don't use car_call_pred in 2D due to DES
		(car_call_pred[lowest_call_pos][DOWN_DIR-1] == FALSE) )
		*/
		) 
        {
/*
                  if ( 
					  (*e2p_DbdAssignEnable != 0) &&
                     (((asslib_DbdExpectCalls[car][lowest_call_pos] & DBD_SDCC) != 0) &&
                       ((expectCcCheckFlag == FALSE) ||
                       (asslib_ValidExpectCalls2(car,DOWN_DIR,next_comm,lowest_call_pos+1,lowest_call_pos)))))
                       {
                         break;
                       }
                  else
                       {
                         lowest_call_pos++;
                       }
*/
			lowest_call_pos++;		
        }
	lowest_pos = lowest_call_pos;
  return(lowest_call_pos);
}/* ads_LowestPosition */


/**************************************************************************
; Name: ads_HighestPosition
;
; Description: search for highest position with demand or predicted demand
;
; Parameters:  car
;              next_comm - next commitable floor
;              pos - new hall call position
;
;*************************************************************************/
BYTE_T ads_HighestPosition(BYTE_T car,BYTE_T next_comm,BYTE_T pos,FLAG_T expectCcCheckFlag)
{
  BYTE_T highest_call_pos;
  highest_call_pos = ocss_info[car].top_pos;
        //DBD
	while( (highest_call_pos > next_comm) && (highest_call_pos > pos) &&
		( ((ocss_info[car].assigned_calls[highest_call_pos] & (S_CC+S_HC+E_CC+E_HC)) == 0)
		/*&& (car_call_pred[highest_call_pos][UP_DIR-1] == FALSE)*/ ) ) 
                {
// jsr 05/02/25 for 2D
                  if (((asslib_DbdExpectCalls[car][highest_call_pos] & DBD_SUCC) != 0) &&
                       (asslib_ValidExpectCalls2(car,UP_DIR,next_comm,highest_call_pos-1,highest_call_pos)))
                       {
                         break;
                       }
                  else
                       {
                         highest_call_pos--;
                       }

                }    

    highest_pos = highest_call_pos;

   return(highest_call_pos);

}/* ads_HighestPosition */


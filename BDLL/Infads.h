/* SCCS:@(#)infads_p.h	1.1	9/17/92 */
/**************************************************************************
; File Name:  INFADS_P.H - Public Data Definition for INFADS.C
;
; File Description:
; This file contains public data definitions used for communications
; with the adss/pu.  This file is to be included in any file that uses
; these services.
;
;  This work and the information it contains are the confidential
;  property of Otis Elevator Company ("Otis").  It is delivered on
;  the express condition that it will be used only for, or on
;  behalf of, Otis; that neither it nor the information it contains
;  will be reproduced, distributed or disclosed, in whole or in part,
;  without the written consent of Otis; and that on demand it and any
;  copies will be promptly returned.
;
;  Unpublished work - Copyright 1992 Otis Elevator Company
;  All rights reserved.
;
; Revision History:
;
; SCN                   Author               Date
; --------------------------------------------------------------------
; AAA-----AAA           Mark A. Ross        14-NOV-91
;
; Created Program.
;
; J1830203AAA		H.Honma (Suzuki)                        30 May  1995
;  - SCR 30203-5004 Additional support for ADS routines added
;        infads_CallPrediction()
;        infads_LowestPosition()
;        infads_HighestPosition()
;        function prototypes 
;
;********************************************************************/
#ifndef INFADS_INCLUDED  /* protects from multiple includes */
#define INFADS_INCLUDED

#ifndef CAGS
#include "queue.h"  /* queue type definition */
#include "infocs.h"
#endif

/*********************************************************************
; Public Constants
;********************************************************************/
#define ADS_BUF_SIZE  512  /* ads buf is 512 bytes  */

/****************************************************************************
; Public Data Declarations
;****************************************************************************/
EXTERN QUEUE_T ads_q;		       /* queue used to buffer adss/pu msg */
EXTERN QUEUE_T *ads_q_ptr;	       /* pointer to the ads_q		   */
EXTERN BYTE_T  ads_buf[ADS_BUF_SIZE];  /* data area associated with ads_q  */
EXTERN BYTE_T  *ads_buf_ptr;	       /* pointer to ads_buf		   */
EXTERN BYTE_T  ads_state;	       /* present state of state machine   */
EXTERN BYTE_T  ads_input;	       /* present input type from adss/pu  */
EXTERN FLAG_T  ads_led_transmit;       /* ICSS2 is trans. 6 bytes to ADSS  */
EXTERN FLAG_T  ads_led_receive;        /* ICSS2 is rec. 6 bytes from ADSS  */
EXTERN FLAG_T  ads_led_ENQ;            /* ICSS2 is trans. ENQ to ADSS      */
EXTERN FLAG_T  ads_led_src_ring;       /* Indicates ICSS2 source ring      */
EXTERN FLAG_T  ads_com_ini;	       /* ICSS2 - ADSS/PU comm. init flag  */
EXTERN BYTE_T  ads_icss2_T0[4];	       /* Message type 0 flags.		   */
EXTERN BYTE_T  ads_icss2_T1[4];	       /* Message type 1 flags.		   */
EXTERN BYTE_T  ads_icss2_T2[4];	       /* Message type 2 flags.		   */
EXTERN BYTE_T  ads_icss2_T3[4];	       /* Message type 3 flags.		   */
EXTERN BYTE_T  ads_icss2_T4[4];	       /* Message type 4 flags.		   */
EXTERN BYTE_T  ads_icss2_T6[8];	       /* Message type 6 flags.		   */
EXTERN BYTE_T  ads_icss2_T7[128];      /* Message type 7 flags.		   */
EXTERN FLAG_T  ads_mit_clock_flag;     /* ADS MIT clock on flag        */
//Dentsu
EXTERN FLAG_T  ads_mot_clock_flag;     /* ADS MOT clock on flag        */
//hh
EXTERN FLAG_T  ads_chn_clock_flag;     /* ADS MIT clock on flag        */



/**************************************************************
for LCADS car call prediction  93-12-27  H.Honma
***************************************************************/

struct ADSS_S {
         BYTE_T pred_calls;    /* number of predicted CCs */
         BYTE_T pred_pos;    /* farthest predicted car call position */
              };
typedef struct ADSS_S ADSS_T;

EXTERN ADSS_T ads_call_pred[JICA_FLOORS][2];




/****************************************************************************
; Function Proto-types
;	1. ads_com_tsk() - supports communications with the adss/pu
;       2. ads_init() - initialize adss/pu communications related information
;****************************************************************************/
void ads_com_tsk(void);
void ads_init(void);
void A_init 	(ADSS_T *A_ptr);

void ads_crowd_info(BYTE_T pos,BYTE_T dir);

void ads_CallPrediction(BYTE_T car,BYTE_T start,BYTE_T finish,INT_T mask,
                            FLAG_T adj,FLAG_T stop);
BYTE_T ads_LowestPosition(BYTE_T car,BYTE_T next_comm,BYTE_T pos,FLAG_T expectCcCheckFlag);
BYTE_T ads_HighestPosition(BYTE_T car,BYTE_T next_comm,BYTE_T pos,FLAG_T expectCcCheckFlag);



#endif  /* end of include file definition */


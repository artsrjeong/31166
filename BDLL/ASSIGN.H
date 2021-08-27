/* U10 Project: @(#)assign.h	1.4	3/23/95	14:22:29 */
/**************************************************************************
;  File Name: ASSIGN.H - Public Interface for ASSIGN.C
; 
;  File Description:
;  This file describes the assign.c public definitions.
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
;  SCN                   Author                        Date
;  ------------------------------------------------------------------------
;  A1730203BAA           Mark Ross                     19 December 1994
;  - SCR 30203-139
;    ICA.C breakup. Made appropriate changes for function invocations.
;
;  A1730203BAA           Theresa Christy               19 January 1995
;  - SCR 30203-182
;    Add declaration of routine "assign_nn_weights_tsk" to switch 
;    the neural network weights that will actually be used in calculations
;    from one set of weights to a different set of weights.     
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;  Transamerica          Geoff Mochau                  20 February 1995
;  - Omit NN definitions to limit RAM requirements.
;  - Removed Advanced-Predirection Metrics to limit RAM requirements.
;
;  Transamerica          Geoff Mochau                  21 February 1995
;  - Substitute all JICA_FLOORS references with MAX_LANDINGS references.
;  - Define ICA_QUEUE_SIZE in terms of MAX_LANDINGS.
;
;  J1830203AAA           H.Honma                           25 May 1995
;  - SCR 30203-5008
;    add initialization of unsure table routine 
;    Add definition of assign_UnsureDistance[]
;
;  J1830203AAA           H.Honma                           08 June 1995
;  - SCR 30203-5002: Support Dual Up Peak
;    
;  J1830203AAA           H.Honma                           19 June 1995
;  - SCR 30203-5038: Add affected passing assign
;
;  J2130203AAA           Hideyuki Honma                     31 Nov 1995
;  - SCR 30203-5029: Dispatch during Channeling
;           
;  AAB-30203-3AA         Mark Ross                     21 Dec 1995
;  - SCR 30203-228: OTI SCR 5029.  Dispatch during Channeling      
;
;  AAB-30203-4AA         Hideyuki Honma(at OECF)            22 Jan 1996
;  - SCR 30203-262: Upgrade dispatch during Channeling(Korea job site)
;*************************************************************************/
#ifndef ASSIGN_INCLUDED  /* protects from multiple includes */
#define ASSIGN_INCLUDED

#ifndef CAGS
#include "infocs.h"
#endif
/**************************************************************************
;  Constants
;*************************************************************************/

//DBD #define ICA_QUEUE_SIZE            (100 * 3 ) 
#define ICA_QUEUE_SIZE            (100 * 4 ) 
//DBD 19Feb04
#define DBD_QUEUE_SIZE            (100 * 6 )   //orgPos,dstPos,callMask,PassId(WORD)

#define CRISP_DISPATCHING         0
#define NEURAL_DISPATCHING        1
#define UNCERTAINTY_DISPATCHING   2
#define SEND_TO_TRENDI            0
#define NO_SEND_TO_TRENDI         1
#define FLSD_MAXIMUM_PATH_ARRAY_SIZE   ( ( MAX_LANDINGS * 2 ) + 1 + 1 )
#define FLSD_MAXIMUM_PATH_LENGTH       ( ( MAX_LANDINGS * 2 ) + 1 )
#define NUMBER_OF_NEURONS   55


/**************************************************************************
;  Data Types
;*************************************************************************/

#ifdef U10

struct FIX_LENGTH_STOP_DESC_S
  {
    S_INT_T floor[FLSD_MAXIMUM_PATH_ARRAY_SIZE];
    BYTE_T  in_min_path[FLSD_MAXIMUM_PATH_ARRAY_SIZE];
    BYTE_T  max_path_length;
    BYTE_T  min_path_length;
  };

#endif

/**************************************************************************
;  Variables
;*************************************************************************/
EXTERN BYTE_T ica_xmit_pos_index;
EXTERN BYTE_T ica_xmit_car_id;
EXTERN BYTE_T ica_rx_fuhc_allowed_mask[MAX_FLOOR_SEG];
EXTERN BYTE_T ica_rx_fdhc_allowed_mask[MAX_FLOOR_SEG];
EXTERN BYTE_T ica_rx_ruhc_allowed_mask[MAX_FLOOR_SEG];
EXTERN BYTE_T ica_rx_rdhc_allowed_mask[MAX_FLOOR_SEG];
EXTERN FLAG_T ica_central_group_enabled_flag;
EXTERN BYTE_T ica_reassnPosIdx;
EXTERN BYTE_T ica_reassnGrpIdx;
EXTERN FLAG_T ica_lock_out_assignments_flag; 
EXTERN INT_T  ica_rat_pwt;
EXTERN INT_T  ica_pwt_distance;
EXTERN INT_T  temp_distance;


#ifdef U10
EXTERN FLOAT_T nn_weight[NUMBER_OF_NEURONS];   
#endif

EXTERN INT_T  pure_rrts[MAX_OCSS];
EXTERN INT_T  pure_pwts[MAX_OCSS];
EXTERN S_INT_T  hc_assn_idx[MAX_OCSS];
EXTERN INT_T	nPax_up[MAX_OCSS];
EXTERN INT_T	nPax_dn[MAX_OCSS];
EXTERN INT_T	nUpStop[MAX_OCSS];
EXTERN INT_T	nDnStop[MAX_OCSS];

EXTERN INT_T  sum_rrt;
EXTERN INT_T  avg_rrt;
EXTERN INT_T  rrt_cnt;
EXTERN INT_T  ica_rrts[MAX_OCSS];
EXTERN BYTE_T coinc_cc[MAX_OCSS],coinc_type_car[MAX_OCSS],coinc_dcc[MAX_OCSS];
EXTERN INT_T  stop_no[MAX_OCSS];
EXTERN INT_T  assigned_cnt[MAX_OCSS],DbdExpect_cnt[MAX_OCSS],future_cnt[MAX_OCSS];
EXTERN INT_T  al_id;




EXTERN INT_T  ica_pwts[MAX_OCSS];

EXTERN INT_T  highest_pos,lowest_pos;

/* New calls queue */
/*-----------------*/
EXTERN BYTE_T  ica_call_buf[ICA_QUEUE_SIZE];
EXTERN BYTE_T  *ica_call_buf_ptr;

//DBD 19Feb04
EXTERN BYTE_T  dbd_call_buf[DBD_QUEUE_SIZE];
EXTERN BYTE_T  *dbd_call_buf_ptr;

/* New calls queue */
/*-----------------*/
EXTERN FLAG_T assign_new_exec;
EXTERN FLAG_T reassign_exec;
EXTERN INT_T  assign_new_tskID;
//kagaya
EXTERN INT_T  reassign_tskID;

/* Used for storing neural network path information  */
/*---------------------------------------------------*/
#ifdef U10
EXTERN struct FIX_LENGTH_STOP_DESC_S FLSDpath;  
#endif

/* Used for initializing FLSDpath structure */
/*------------------------------------------*/
#ifdef U10
EXTERN struct FIX_LENGTH_STOP_DESC_S init_FLSDpath;  
#endif
 

/* ICSS2 view of OCSS Predir */
EXTERN BYTE_T ica_icss2_advanced_predir[MAX_OCSS]; 

EXTERN INT_T assign_DistanceUnsure[2*JICA_FLOORS];
#ifndef CAGS
EXTERN INT_T assign_affected_passing;
#else
EXTERN __declspec(dllexport) INT_T assign_affected_passing;
#endif

//DBD-honma 17Mar04  spcap
EXTERN BYTE_T  assign_RrtCap[MAX_OCSS],assign_PwtCap[MAX_OCSS];


/**************************************************************************
;  Function Prototypes
;*************************************************************************/
int assign_new_call (BYTE_T dir,BYTE_T pos,BYTE_T dst,BYTE_T riser);
void   assign_init(void);
void   assign_UnsureTableInit(void);
INT_T assign_check_passing_car(BYTE_T car,BYTE_T temp_dir,BYTE_T pos);
INT_T assign_CheckSpareCapacity(BYTE_T car,BYTE_T temp_dir,BYTE_T pos,BYTE_T *capPwt);
INT_T assign_consider_sector(BYTE_T car,BYTE_T pos,BYTE_T pre_dir,BYTE_T call_mask,INT_T rrt);
void  assign_check_chn(BYTE_T pos,BYTE_T call_mask);
#ifdef U10
void   assign_nn_weights(INT_T);
#endif

#endif  /* end of include file definition */

/* U10 Project: @(#)infrng.h	1.1	3/23/95	14:22:53 */
/**************************************************************************
;  File Name:  INFRNG.H - Public Data Definitions for INFRNG.C
;
;  File Description:
;  This file describes the INFRNG.C public data.  This file is to be
;  included in any file that uses these services.
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
;  Revision History:
;
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Created (renamed) module from "infrng_p.h".
;
;********************************************************************/
#ifndef INFRNG_INCLUDED  /* protects from multiple includes */
#define INFRNG_INCLUDED

#include "queue.h"  /* queue type definition */
#include "timer.h"  /* timer type definition */


/*********************************************************************
; Public Constants
**********************************************************************/
#define RING_BUF_SIZE	2048		/* ring buffers are 512 bytes */
#define MSG_LEN 	6

/*********************************************************************
; Ring Data Types
**********************************************************************/
struct RING_S {
     FLAG_T  send_to_adss;		/* type, subtype filter flag  	*/
     FLAG_T  send_to_adss_origin;	/* origin filter flag		*/
     FLAG_T  send_to_ring;		/* re-transmit to ring flag	*/
     FLAG_T  alive;			/* status of the ring		*/
     BYTE_T  bytes_read;		/* # of bytes read from ring N  */
     TIMER_T tout;			/* ring N timeout		*/
     TIMER_T type3_tout;		/* type 3 message timeout       */
     BYTE_T  old_RX_bytes;		/* # of bytes in partial msg    */
     BYTE_T  old_RX_buff[MSG_LEN];	/* partial message		*/
};
typedef struct RING_S RING_T;


/*********************************************************************
; Global Ring Related Data
**********************************************************************/
EXTERN RING_T ring_info[3];
EXTERN QUEUE_T ring1_q;
EXTERN QUEUE_T ring2_q;
EXTERN QUEUE_T *ring1_q_ptr;
EXTERN QUEUE_T *ring2_q_ptr;
EXTERN BYTE_T  ring1_buf[RING_BUF_SIZE];
EXTERN BYTE_T  ring2_buf[RING_BUF_SIZE];
EXTERN BYTE_T  *ring1_buf_ptr;
EXTERN BYTE_T  *ring2_buf_ptr;

//for Monitoring.
EXTERN INT_T  infrng_Ring1GetCount,infrng_Ring1PutCount;
EXTERN INT_T  infrng_Ring1MaxGetCount,infrng_Ring1MaxPutCount,infrng_RxqPutCount,infrng_RxqMaxPutCount;
EXTERN INT_T  infrng_RxqMaxGetCount,infrng_RxqGetCount;



EXTERN FLAG_T fuzzy_logic_enabled;

/*********************************************************************
; Function Prototypes - Initialize ring data structure.
;
;	1. init_ring_info() - initialize ring information
;	2. clear_sectors() - reset sector definitions
;	3. ring1_com_tsk() - task which supports communication w/ ring1
;	4. ring2_com_tsk() - task which supports communication w/ ring2
;	5. ring1_proc_msg_tsk - processes messages from ring 1
;	6. ring2_proc_msg_tsk - processes messages from ring 2
;	7. ocss_icss_monitor_tsk() - perform maintainence functions
;	8. up_peak_clock_tsk() - issue up peak clock message
;	9. hall_crowd_tsk() - issue hall crowd message
;
**********************************************************************/
void   init_ring_info(void);
void   clear_sectors(void);
EXTERN  void   ring1_com_tsk(void);
void   ring2_com_tsk(void);
void   ring1_proc_msg_tsk (void);
void   ring2_proc_msg_tsk (void);
void   ocss_icss_monitor_tsk(void);
void   infrng_status_msg_tsk(void);
void   ring_enable_tasks(void);
void   ring_disable_tasks(void);
BYTE_T count_normal_cars(void);
void send_cancel_call (BYTE_T landing, BYTE_T callmask);
void  send_cancel_grp_call(BYTE_T landing, INT_T callmask, BYTE_T G_NO);
void send_dispatch_call (BYTE_T landing, BYTE_T callmask);

void test_new_dbd_hall_call(BYTE_T srcPos, BYTE_T dstPos,BYTE_T callMask, BYTE_T callType, WORD_T passId);


#endif  /* end of include file definition */

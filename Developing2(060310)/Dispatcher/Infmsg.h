/* U10 Project: @(#)infmsg.h	1.1	3/23/95	14:22:51 */
/*********************************************************************
;  File Name:  INFMSG.H - Data Definition for INFMSG.C Public Data
;
;  File Description:
;  This file describes the INFMSG.C public data.  This file is to be
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
;  - SCR 30203-38: Created (renamed) module from "infmsg_p.h".
;
;********************************************************************/
#ifndef INFMSG_INCLUDED  /* protects from multiple includes */
#define INFMSG_INCLUDED

/*************************************************************************
;       Message Constants
;*************************************************************************/
#define         MSG_LEN		  6      /* Ring message size		 */
#define		MAX_PROC_MESSAGES 32     /* max. # of msg's to process   */
#define         TOUT		  0      /* Time out			 */
#define         ENQ		  0x05   /* Enquire			 */
#define         ACK		  0x06   /* Acknowledge			 */
#define         NAK		  0x15   /* Negative acknowledgement	 */
#define         BROADCAST	  0      /* Msg destination is broadcast */
#define		NULL		  0      /* empty byte			 */

/*********************************************************************
; MESSAGE Structure
; This structure is used in the MESSAGE_S type definition.  This type
; definition will be used by the client module to create an instance
; of a message.
;
; This type definition is used by the client module to create an
; instance of a message. A message consists of six bytes of RAM used
; to store the message and a pointer to the first byte in the message.
;********************************************************************/
struct MESSAGE_S  {
        BYTE_T  *ptr;      		/* ptr to the start of message	*/
        BYTE_T    msg[MSG_LEN]; 		/* message itself		*/
};
typedef struct MESSAGE_S MESSAGE_T;

/*****************************
; Global Messages
******************************/
EXTERN BYTE_T up_peak_clock_init_msg[MSG_LEN];
EXTERN BYTE_T null_init_msg[MSG_LEN];
EXTERN MESSAGE_T ads_in_msg;
EXTERN MESSAGE_T ads_out_msg;
EXTERN MESSAGE_T incoming_msg1;
EXTERN MESSAGE_T incoming_msg2;
EXTERN MESSAGE_T proc_msg1;
EXTERN MESSAGE_T proc_msg2;
EXTERN MESSAGE_T incoming_msg;
EXTERN MESSAGE_T outgoing_msg;
EXTERN MESSAGE_T up_peak_clock_msg;
EXTERN MESSAGE_T hall_crowd_msg;
EXTERN MESSAGE_T sector1_msg;
EXTERN MESSAGE_T sector2_msg;
EXTERN MESSAGE_T sector3_msg;
EXTERN MESSAGE_T sector4_msg;
EXTERN MESSAGE_T sector5_msg;
EXTERN MESSAGE_T sector6_msg;
EXTERN MESSAGE_T sector7_msg;
EXTERN MESSAGE_T sector8_msg;
EXTERN MESSAGE_T ocss_monitor_msg;
EXTERN MESSAGE_T null_msg;

/*********************************************************************
;  Function Prototypes
;	1. M_checksum - calculate message checksum
;	2. M_framing - examine message for framing error
;	3. M_get_type - extract message type
;	4. M_get_subtype - extract message subtype
;	5. M_get_origin - extract message origin
;	6. M_get_dest - extract message destination
;	7. M_copy - make a copy of a message
;	8. M_init - initialize a message
;********************************************************************/
BYTE_T M_checksum               (BYTE_T *Data_prt, INT_T Size);
BYTE_T M_framing                (BYTE_T *Data_prt, INT_T Size);
BYTE_T M_get_type               (BYTE_T *Data_ptr);
BYTE_T M_get_subtype    	(BYTE_T *Data_ptr);
BYTE_T M_get_origin             (BYTE_T *Data_ptr);
BYTE_T M_get_dest               (BYTE_T *Data_ptr);
void   M_copy                   (BYTE_T *M_src, BYTE_T *M_dest, INT_T Size);
void   M_init                   (MESSAGE_T *M_ptr);


#endif  /* end of include file definition */


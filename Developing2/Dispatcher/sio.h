/*******************************************************************************
;  File Name: SIO.H
;
;  File Descriptions:
;  - Set Interupt Vector for SIO
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
;  SCN                   Author                        Date
;  ------------------------------------------------------------------------
;  XXX-30461-XXX         Takayuki Yoshida              21 September 1999
;  xxx-31017-xxx         Dan Williams                  December 2003
;                        * add setup for  Destination Entry Dispatcher
*******************************************************************************/
#ifndef sio_AlreadyIncludeHeader    /* protects from multiple includes */
#define sio_AlreadyIncludeHeader

#ifdef sio_ModuleDefinition
#define sio_EXTERN
#else
#define sio_EXTERN extern
#endif   // sio_ModuleDefinition

/*******************************************************************************
; Include Global Declarations
*******************************************************************************/
#include "queue.h"

/*----------------------------------------------------------------------------*/
/*******************************************************************************
; Macro Definition
*******************************************************************************/
// Number of Devices and Ports
#define NUM_DEVICES     3
#define NUM_PORTS       NUM_DEVICES*2
#define BUFF_RX_INDEX   0    /* index for RX buffer data */
#define BUFF_TX_INDEX   1    /* index for TX buffer data */

/* port number */
#define SIO1_PORTA  0  //SUB_PORT
#define SIO1_PORTB  1  //DEBUG_PORT, ADSS_PORT
#define SIO2_PORTA  2  //RING_1_PORT
#define SIO2_PORTB  3  //RING_2_PORT
#define SIO3_PORTA  4  //DES_PORT
#define SIO3_PORTB  5  //SV_TOOL_PORT

/* User's port */
#define	ADSS_PORT       SIO1_PORTB
#define	DEBUG_PORT      SIO1_PORTB
#define	GSV_TOOL_PORT   DEBUG_PORT

#define	RING_1_PORT     SIO2_PORTA
#define	RING_2_PORT     SIO2_PORTB

#define DES_PORT        SIO3_PORTA
#define SV_TOOL_PORT    SIO3_PORTB

/*******************************************************************************
; Data Types
*******************************************************************************/
struct PORT_S {
   // Port Address
   LONG_T DataAddress;
   LONG_T CRAddress;
   LONG_T SRAddress;
   // Receive Buffer Data
   QUEUE_T RX_Q;
   BYTE_T *RX_QBuf_Ptr;
   LONG_T  RX_QSz;
   LONG_T  Max_Rx_Buf;
   LONG_T  Max_Rx_Bps;
   LONG_T  Current_Rx_Bps;
   LONG_T  Rx_Bps_Count;
   // Transmit Buffer Data
   QUEUE_T TX_Q;
   BYTE_T *TX_QBuf_Ptr;
   LONG_T  TX_QSz;
   FLAG_T  TX_Busy;
   LONG_T  Max_Tx_Buf;
   LONG_T  Max_Tx_Bps;
   LONG_T  Current_Tx_Bps;
   LONG_T  Tx_Bps_Count;
   // Port Data
   FLAG_T  Break;
   LONG_T  Break_Count;
   LONG_T  Parity_Error_Count;
   LONG_T  Rx_Overrun_Count;
   LONG_T  Framing_Error_Count;
   // Setup
   LONG_T BaudRate;
   BYTE_T Baud_L_RegValue;
   BYTE_T Baud_H_RegValue;
};
typedef struct PORT_S PORT_T;


/******************************************************************************
; Public Data Definitions
******************************************************************************/
sio_EXTERN PORT_T sio_Port[NUM_PORTS];
sio_EXTERN FLAG_T sio_Monitor_Flag; /* flag to reserch that sio_Monito executes */

/******************************************************************************
; Public Functions
*******************************************************************************/
sio_EXTERN void sio_SetBuf(void);
sio_EXTERN void sio_Init(void);
sio_EXTERN void sio_SetBaudRate(BYTE_T portNum, LONG_T baud);
sio_EXTERN void sio_Setup(void);

sio_EXTERN void sio_RingRelay(FLAG_T );

sio_EXTERN LONG_T sio_CheckRx(BYTE_T portNum);
sio_EXTERN LONG_T sio_CheckTx(BYTE_T portNum);
sio_EXTERN FLAG_T sio_TxIsEmpty(BYTE_T portNum);
sio_EXTERN FLAG_T sio_RxPort(BYTE_T portNum, BYTE_T *data, LONG_T size);
sio_EXTERN FLAG_T sio_TxPort(BYTE_T portNum, BYTE_T *Data_Ptr, LONG_T Data_Size);
sio_EXTERN void   sio_setBreak(BYTE_T portNum);
sio_EXTERN void   sio_clearBreak(BYTE_T portNum);
sio_EXTERN FLAG_T sio_BreakStatus(BYTE_T portNum);
sio_EXTERN void   sio_FlushRxPort(PORT_T *port);
sio_EXTERN void   sio_ResetRxPort(BYTE_T portNum);
sio_EXTERN void   sio_FlushTxPort(PORT_T *port);
sio_EXTERN void   sio_Monitor(void);
/* jsr 막아 놓았음
sio_EXTERN interrupt void sio_isr1(void);
sio_EXTERN interrupt void sio_isr2(void);
sio_EXTERN interrupt void sio_isr3(void);
*/

void sio_TxRing(BYTE_T *Data_Ptr, INT_T Data_Size, BYTE_T ringType);

/*----------------------------------------------------------------------------*/
#endif   // sio_AlreadyIncludeHeader

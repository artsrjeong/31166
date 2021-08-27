/* TITAN Project: @(#)queue.h	1.0	3/08/96	14:23:02 */
/**************************************************************************
|  File Name: QUEUE.H - Public Interface for QUEUE.C
|
|  File Description:
|  This file describes the QUEUE.C public definitions.  This file is to be
|  included in any file that uses these services.  See the QUEUE.C file|
|  for detailed usage information.
|
|
|  This work and the information it contains are the confidential
|  property of Otis Elevator Company ("Otis").  It is delivered on
|  the express condition that it will be used only for, or on
|  behalf of, Otis; that neither it nor the information it contains
|  will be reproduced, distributed or disclosed, in whole or in part,
|  without the written consent of Otis; and that on demand it and any
|  copies will be promptly returned.
|
|  Unpublished work - Copyright 1997 Otis Elevator Company
|  All rights reserved.
|
|
|  Revision History:
|
|  SCN             Author                        Date
|  ------------------------------------------------------------------------
|  A1730203BAA     Geoff Mochau                  02 February 1995
|  - SCR 30203-38: Created (renamed) module from "queue_p.h".
|
|  AAA30299J11     T. Mori                       08 February 1996
|
|  AAB-30301-3AA   Mark Ross                     13-Feb-97
|  - SCR 30301-27. Modified to support Titan Group Control Utility
|                  reuse library.
|
|  AAB-30304-4AA   E. Pias                       20-Feb-97
|  - SCR 30402-1   Added function queue_Search.
|*************************************************************************/
#ifndef queue_AlreadyIncludeHeader    /* protects from multiple includes */
#define queue_AlreadyIncludeHeader

#ifdef queue_ModuleDefinition
#define queue_EXTERN
#else
#define queue_EXTERN extern
#endif

/**************************************************************************
|  Constants
|*************************************************************************/
#define    MAX_PUTGETERR        99999L
/**************************************************************************
|  Data Types
|*************************************************************************/

/**************************************************************************
|  Define a queue type in SRAM
|   This structure is used in the Queue type definition.  This type
|   definition will be used by the client module to create an instance
|   of a queue in SRAM.
|
|   The structure contents are PRIVATE.  This structure is given for
|   definition purposes ONLY.
|*************************************************************************/
struct QUEUE_S  {
  BYTE_T *Start; /* pointer to first byte in data buffer */
  BYTE_T *Put;   /* current pointer to put byte in data buffer */
  BYTE_T *Get;   /* current pointer to get byte in data buffer */
  BYTE_T *End;   /* pointer to one byte after data buffer */
  LONG_T  PutAvailBytes;
  LONG_T  GetAvailBytes;
  /* Q_Put Execution Error Count */
  LONG_T errorPut;
  /* Q_Get Execution Error Count */
  LONG_T errorGet;
};
typedef struct QUEUE_S QUEUE_T;


/**************************************************************************
|  Variables
|*************************************************************************/
/* None */


/**************************************************************************
|  Function Prototypes
|*************************************************************************/
queue_EXTERN FLAG_T Q_Get(QUEUE_T *Q, BYTE_T *Data_Ptr, LONG_T Size);
queue_EXTERN FLAG_T Q_Put(QUEUE_T *Q, BYTE_T *Data_Ptr, LONG_T Size);
queue_EXTERN void   Q_Init(QUEUE_T *Q, BYTE_T *Buff_Ptr, LONG_T Q_Size);
queue_EXTERN void   Q_Reset(QUEUE_T *Q);
queue_EXTERN LONG_T  Q_Get_Count(QUEUE_T *Q);
queue_EXTERN LONG_T  Q_Put_Count(QUEUE_T *Q);
queue_EXTERN FLAG_T Q_Full(QUEUE_T *Q);
queue_EXTERN FLAG_T Q_Empty(QUEUE_T *Q);
queue_EXTERN LONG_S queue_Search(QUEUE_T *q, BYTE_T token);

#endif  /* end of include file definition */

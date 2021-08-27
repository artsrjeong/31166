#include "stdafx.h"
#include "common.h"
#include "pi.h"
/* U10 Project: @(#)queue.c	2.2	3/23/95	14:23:01 */
/**************************************************************************
;  File Name: QUEUE.C - Data Queue Module
; 
;  File Description:
;  Provides services that maintain an instance of a queue.
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
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  AAA30xxxAAA           Steve Hatzirallis             30-Mar-92
;  First release - fuzzy ICSS2.
;
; AAA-----AAA           Jeremy Kezer              9 July 1993
; - Modify code to be more consistant with ICSS2 coding standards.
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;*************************************************************************/
#define queue_ModuleDefinition

#include "common.h"
#include "lib.h"
/**************************************************************************
;  Public Declarations
;*************************************************************************/

/**************************************************************************
;  Public Definitions
;*************************************************************************/
#include "queue.h"

/**************************************************************************
;  Local Definitions
;*************************************************************************/
/* None */

/**************************************************************************
;  Public Functions
;*************************************************************************/



/**************************************************************************
;  Name: Q_Get - Get a Certain Number of Data Bytes from a Queue
;  
;  Description:
;  Retreives a given number of bytes from an instance of a queue. 
;  Places the bytes in a given data pointer.  If the queue does not have
;  at least the given number of bytes then a fail flag will be returned.
;
;  During this operation the executive is commanded to not allow further
;  task execution.  This will prevent a new task interrupting this
;  operation and getting data in the middle of this data stream.
; 
;  Parameters:
;  Queue Pointer   (input)   pointer to the queue structure
;  Data Pointer    (input)   pointer to array where data will be stored
;  Data Size       (input)   number of data bytes to be retreived
;  Valid Get       (output)  flag indicating success of operation
;*************************************************************************/
FLAG_T Q_Get(QUEUE_T *Q, BYTE_T *Data_Ptr, LONG_T Size) 
{
   FLAG_T Valid_Get;
   LONG_T Copy_Size;
   LONG_T get_cnt;
   LONG_T imask;

   imask = splx(FULLMASK);
   //imask = exec_SetMask(0xffffffff);

   if (Q->GetAvailBytes < Size)
   {
      Valid_Get = FALSE;
   }
   else 
   {
      if (Size == 0)
         ;
      else 
      {
         if (Size >= (Q->End - Q->Get)) 
         {
            Copy_Size = Q->End - Q->Get;
            Lib_Memcpy(Data_Ptr, Q->Get, Copy_Size);
            Data_Ptr = Data_Ptr + Copy_Size;
            Q->Get = Q->Start;
            Copy_Size = Size - Copy_Size;
         }
         else  
         {
            Copy_Size = Size;
         }
         Lib_Memcpy(Data_Ptr, Q->Get, Copy_Size);

         Q->Get = Q->Get + Copy_Size;
         Q->GetAvailBytes -= Size;
         Q->PutAvailBytes += Size;
      }

      Valid_Get = TRUE;
   }
   /* Increase Error Counter */
   /* A.I. has no tx-message if get_cnt is 0.  02 June 2000 */
   if ((Valid_Get == FALSE ) && (Q->GetAvailBytes != 0))
   {
      if ( (Q->errorGet) < MAX_PUTGETERR )
         Q->errorGet++;
   }

   splx(imask);
   //exec_SetMask(imask);
   return(Valid_Get);
} /* Q_Get */


/**************************************************************************
;  Name: Q_Put - Put a Certain Number of Data Bytes into a Queue
; 
;  Description:
;  Stores a given number of bytes into an instance of a queue.  If the
;  queue does not have enough room to store the data then a fail flag
;  will be returned and none of the data will be placed in the queue.
;
;  During this operation the executive is commanded to not allow further
;  task execution.  This will prevent a new task interrupting this
;  operation and putting data in the middle of this data stream.
; 
;  Parameters:
;  Queue Pointer   (input)   pointer to the queue structure
;  Data Pointer    (input)   pointer to array where data will be retreived
;  Data Size       (input)   number of data bytes to be stored
;  Valid Put       (output)  flag indicating success of operation
;*************************************************************************/
FLAG_T Q_Put(QUEUE_T *Q, BYTE_T *Data_Ptr, LONG_T Size) 
{
   FLAG_T Valid_Put;
   LONG_T Copy_Size;
   LONG_T imask;

   imask = splx(FULLMASK);
   //imask = exec_SetMask(0xffffffff);

   if (Q->PutAvailBytes < Size)
   {
      Valid_Put = FALSE;
   }
   else 
   {
      if (Size == 0)
         ;
      else 
      {
         if (Size >= (Q->End - Q->Put))
         {
            Copy_Size = Q->End - Q->Put;
            Lib_Memcpy(Q->Put, Data_Ptr, Copy_Size);
            Data_Ptr = Data_Ptr + Copy_Size;
            Q->Put = Q->Start;
            Copy_Size = Size - Copy_Size;
         }
         else  
         {
            Copy_Size = Size;
         }

         Lib_Memcpy(Q->Put, Data_Ptr, Copy_Size);

         Q->Put = Q->Put + Copy_Size;
         Q->PutAvailBytes -= Size;
         Q->GetAvailBytes += Size;
      }
      Valid_Put = TRUE;
   }

   /* Increase Error Counter */
   if ((Valid_Put == FALSE) && (Q->PutAvailBytes != 0))
   {
      if ( (Q->errorPut) < MAX_PUTGETERR )
        Q->errorPut++;
   }

   splx(imask);
   //exec_SetMask(imask);
   return(Valid_Put);
} /* Q_Put */


/**************************************************************************
;  Name: Q_Init - Intialize a Queue
; 
;  Description:
;  Sets up the queue buffer and resets all values.
;*************************************************************************/
void Q_Init(QUEUE_T *Q, BYTE_T *Buff_Ptr, LONG_T Q_Size) 
{
   Q->Start = Buff_Ptr;
   Q->End = Q->Start + Q_Size;  /* points to one byte after buffer */
   Q_Reset(Q);
   Q->errorPut = 0;
   Q->errorGet = 0;
   
} /* Q_Init */


/**************************************************************************
;  Name: Q_Reset - Reset a Queue
; 
;  Description:
;  Resets all queue values to an empty queue state.
;*************************************************************************/
void Q_Reset(QUEUE_T *Q) 
{
   Q->Put = Q->Start;
   Q->Get = Q->Start;
   Q->GetAvailBytes = 0;
   Q->PutAvailBytes = (Q->End - Q->Start);
} /* Q_Reset */


/**************************************************************************
;  Name: Q_Get_Count - Get Bytes that Can Be Taken From Queue
; 
;  Description:
;  Returns the number of data bytes currently in the queue.
;
;  Parameters:
;  Count           (output)  stored byte count
;*************************************************************************/
LONG_T Q_Get_Count(QUEUE_T *Q) 
{
/*
LONG_T getCount;
BYTE_T *getPos, *putPos;

   getPos = Q->Get;
   putPos = Q->Put;
   if ( putPos >= getPos )
   {
     getCount = putPos - getPos;
   }
   else if ( putPos < getPos )
   {
     getCount = (putPos - Q->Start) + (Q->End - getPos);
   }
   return(getCount);
*/
   return (Q->GetAvailBytes);
} /* Q_Get_Count */


/**************************************************************************
;  Name: Q_Put_Count - Get Bytes that Can Be Put Into Queue
; 
;  Description:
;  Returns the number of data bytes that can be put in the queue.
;
;  Parameters:
;  Put_Count            (output)  put byte count
;*************************************************************************/
LONG_T Q_Put_Count(QUEUE_T *Q) 
{
/*
LONG_T putCount;
BYTE_T *getPos, *putPos;

   getPos = Q->Get;
   putPos = Q->Put;
   if ( putPos >= getPos )
   {
     putCount = (Q->End - putPos) + (getPos - Q->Start);
   }
   else if ( putPos < getPos )
   {
     putCount = getPos - putPos;
   }

   return(putCount);
*/
   return (Q->PutAvailBytes);
} /* Q_Put_Count */


/**************************************************************************
;  Name: Q_Full - Check to See If A Queue is Full
; 
;  Description:
;  Passes back a flag to indicate whether the queue is full.
;
;  Parameters:
;  Full Flag       (output)  true = queue is full
;*************************************************************************/
FLAG_T Q_Full(QUEUE_T *Q) 
{
   if (Q->PutAvailBytes == 0)
      return(TRUE);
   else 
      return(FALSE);
} /* Q_Full */


/**************************************************************************
;  Name: Q_Empty - Check to See If A Queue is Empty
; 
;  Description:
;  Passes back a flag to indicate whether the queue is empty.
;
;  Parameters:
;  Empty Flag      (output)  true = queue is empty
;*************************************************************************/
FLAG_T Q_Empty(QUEUE_T *Q) 
{
   if (Q->GetAvailBytes == 0)
      return(TRUE);
   else 
      return(FALSE);
} /* Q_Empty */



/**************************************************************************
;  Local Functions
;*************************************************************************/
/* None */
#undef  queue_ModuleDefinition

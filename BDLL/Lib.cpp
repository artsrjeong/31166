#include "stdafx.h"
/**************************************************************************
;  File Name: GCBLIB.C - Library Services
;  
;  Description:
;  Provides a general set of services.
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
;  SCN                   Author                        Date
;  ------------------------------------------------------------------------
;  AAB-30203-3AA         Geoff Mochau                  06 November 1995
;  - SCR 30203-210: Change module names to reflect hardware change-over
;    to OTI's GCB.  This module is derived from version 3.1 of "u10lib.c".
;
;  XXX-30461-XXX         Takayuki Yoshida              27 October 1999
;  - Add Round-Up Function for On Board AI
;*************************************************************************/
#define lib_ModuleDefinition

#include "common.h"

/**************************************************************************
;  Public Declarations
;*************************************************************************/

/**************************************************************************
;  Public Definitions
;*************************************************************************/
#include "lib.h"

/**************************************************************************
;  Local Definitions
;*************************************************************************/
/* None */


/**************************************************************************
;  Public Functions
;*************************************************************************/

/**************************************************************************
;  Name: Lib_Memcpy - Copies a Block of Memory
;  
;  Description:
;  Copies a block of memory from a source buffer to a destination buffer.
;
;  Parameters:
;  To          (Input)     Pointer to the destination address
;  From        (Input)     Pointer to the source address
;  Count       (Input)     Size of buffer to be copied
;*************************************************************************/
void Lib_Memcpy(BYTE_T *To, BYTE_T *From, LONG_T Count) {
  LONG_T i;
  for (i = 0; i < Count; i++)    *To++ = *From++;
} /* Lib_Memcpy */

/**************************************************************************
;  Name: Lib_SRbyte - Shift Right a Block of Memory by One Byte
;  
;  Description:
;  Shifts right a block of memory by one byte.
;
;  Parameters:
;  Buffer      (Input)     Pointer to the start of buffer to shift
;  Count       (Input)     Size of buffer to be shifted
;*************************************************************************/
void Lib_SRbyte(BYTE_T *Buffer, LONG_T Count) {
  LONG_T i;

  Buffer+=(Count-1);
  for (i = 0; i < Count; i++)
    {
      *(Buffer+1) = (BYTE_T)(*Buffer);
      Buffer--;
    }
} /* Lib_SRbyte */


/**************************************************************************
;  Name:  Lib_Memset - Set a Block of Memory to a Specific Value
;  
;  Description:
;  Sets a block of memory to a specific value.
;
;  Parameters:
;  Buffer      (Input)     Pointer to the buffer address
;  Value       (Input)     Value that the buffer data will be set to
;  Count       (Input)     Size of buffer to be copied
;*************************************************************************/
void Lib_Memset(BYTE_T *Buffer, BYTE_T Value, LONG_T Count) {
  LONG_T i;
  LONG_T s1;

  s1=(LONG_T)Buffer;
  for (i = 0; i < Count; i++)
    *Buffer++ = Value;
   

  Buffer=(BYTE_T *)s1;
} /* Lib_Memset */


/**************************************************************************
;  Name: Lib_Memsrch - Memory Byte Search
;  
;  Description:
;  Searches a block of memory for a specific byte value.
;
;  Parameters:
;  Buffer      (Input)     Pointer to the buffer address
;  Value       (Input)     Value to be searched for
;  Max_Buf     (Input)     Max buffer size to search
;  Value_Ptr   (Output)    Pointer to the first occurence of the value
;                          Returns a null pointer (0) when the value is
;                          not found
;*************************************************************************/
LONG_T Lib_Memsrch(BYTE_T *Buffer, BYTE_T Value, LONG_T Max_Buf)
 {
  LONG_T   i;
  LONG_T	Value_Ptr;
  LONG_T	s1;
 
  /* Initialize - Start loop, assume failure */
  /*-----------------------------------------*/
  s1=(LONG_T)Buffer;
  i = 0;
  Value_Ptr = 0;

  /* Search     */
  /*------------*/
  while (i < Max_Buf) {

    /* Did we find the search value ?                   */
    /*    - Yes, set return value, exit loop            */
    /*    - No, increment loop count and buffer pointer */
    /*--------------------------------------------------*/
    if (*Buffer == Value) {
      Value_Ptr = i;
      i = Max_Buf;
    }
    else {
      Buffer++;
      i++;
    }
  }

  if(Value_Ptr==Max_Buf)	Value_Ptr = 0x10;

  Buffer=(BYTE_T *)s1;
  return(Value_Ptr);
    
} /* Lib_Memsrch */


/**************************************************************************
;  Name: Lib_Memrsrch - Memory Reverse Byte Search
;  
;  Description:
;  Searches a block of memory for a specific byte value in the backwards 
;  direction.
;
;  Parameters:
;  Buffer      (Input)     Pointer to the buffer address
;  Value       (Input)     Value to be searched for
;  Max_Buf     (Input)     Max buffer size to search
;  Value_Ptr   (Output)    Pointer to the first occurence of the value
;                          Returns a null pointer (0) when the value is
;                          not found
;*************************************************************************/
BYTE_T *Lib_Memrsrch(BYTE_T *Buffer, BYTE_T Value, LONG_T Max_Buf) {

  LONG_T   i, s1;
  BYTE_T *Value_Ptr;
 
  /* Initialize - Start loop, assume failure, start at end of buffer */
  /*-----------------------------------------------------------------*/
  s1=(LONG_T)Buffer;
  i = 0;
  Value_Ptr = 0;
  Buffer+=(Max_Buf-1);

  /* Search     */
  /*------------*/
  while (i < Max_Buf) {

    /* Did we find the search value ?                             */
    /*    - Yes, set return value, exit loop                      */
    /*    - No, increment loop count and decrement buffer pointer */
    /*------------------------------------------------------------*/
    if (*Buffer == Value) {
      Value_Ptr =  Buffer;
      i = Max_Buf;
    }
    else {
      Buffer--;
      i++;
    }
  }
  Buffer = (BYTE_T *)s1;
  return(Value_Ptr);
    
}  /* Lib_Memrsrch */

/*****************************************************************************
; Name : Lib_RoundUp
; - Parameters FLOAT_T target value
; - Return     LONG_T value to 4 stolen 5 enter
*****************************************************************************/
LONG_T Lib_RoundUp(FLOAT_T *value)
{
FLOAT_T x, y;
LONG_T max, min;
LONG_T ret;

  x = *value;
  max = (LONG_T)(x+1.0F);
  min = (LONG_T)x;
  y = x + 0.5F;

  if ( (y >= min)&&(y < max) )
     ret = min;
  else if ( y >= max )
     ret = max;

  if (ret < 0)  ret = 0;

  return(ret);
}

/*****************************************************************************
; Name : Lib_AbsValue
; - Parameters
;      BYTE_T x
;      BYTE_T y
; - Return
;      BYTE_T  absolute value x and y
*****************************************************************************/
BYTE_T Lib_AbsValue(BYTE_T x, BYTE_T y)
{
BYTE_T ret;

  if ( x >= y )
    ret = x - y;
  else
    ret = y - x;

  return(ret);
}

/**************************************************************************
;  Local Functions
;*************************************************************************/
/* None */

#undef lib_ModuleDefinition

/* U10 Project: @(#)infmsg.c	2.2	3/23/95	14:22:50 */
/**************************************************************************
; File Name: INFMSG.C Message Handler
;
; File Description:
; This file contains modules that handle data for a given instance
; of a message.  These files are reentrant.
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
; Revision History:
;
; SCN                   Author                      Date
; ------------------------------------------------------------------------
; AAA-----AAA           Mark A. Ross              14-NOV-91
; - Created Program.
;
; AAA-----AAA           Jeremy Kezer              9 July 1993
; - Modify code to be more consistant with ICSS2 coding standards.
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;*************************************************************************/
#include "stdafx.h"
#include "common.h"

/**************************************************************************
;  Public Declarations
;*************************************************************************/
#define EXTERN extern   /* declare public variables */
/* None */
#undef EXTERN

/**************************************************************************
;  Public Definitions
;*************************************************************************/
#define EXTERN          /* define public variables */
#include "infmsg.h"
#undef EXTERN

/**************************************************************************
;  Local Definitions
;*************************************************************************/
/* None */


/**************************************************************************
;  Public Functions
;*************************************************************************/

/*********************************************************************
; Name: M_checksum - Calculate the checksum of the message at the given
;                    pointer.
;
; Description:
; Calculates the message checksum by XOR'ing the number of bytes requested
; by a particular invocation. A byte value representing the calculated
; checksum is returned.
;
; Parameters:
; Message Pointer	(input)         pointer to the start of the message.
; Data Size             (input)         number of bytes in the message.
; Checksum Value        (output)        calculated checksum.
;********************************************************************/
BYTE_T  M_checksum (BYTE_T *msg_ptr, INT_T size)
{
   
   BYTE_T  chksm;
   BYTE_T  i;
   
   chksm = 0xff;
   for (i=0; i<size-1; i++) 
   {
      chksm = chksm ^ *msg_ptr;
      msg_ptr++;
   }
   return(chksm);
} /* M_checksum */

/***************************************************************************
; Name: M_framing - Checks the message for framing errors.
;
; Description:
; Checks the message for framing errors. The function is given a pointer
; to the message and the size of the message. The MSB of the first
; message_size - 1 bytes must be one, while the MSB of the last byte
; must be a zero. If these conditions do not hold, a framing error has
; occured. The function will return the number of the byte where the
; error was detected. (0 to message_size - 1)
;
; Parameters:
; Message Pointer:	(input)         pointer to the start of the message.
; Data Size:		(input)         number of bytes in the message.
; Result of Test:	(output)        message_size of test was successful,
;                       		(0 to message_size - 1) if failure.
;****************************************************************************/
BYTE_T  M_framing (BYTE_T *msg_ptr, INT_T size) 
   
{
   
   BYTE_T  i;
   BYTE_T  pos;
   
   i = 0;
   pos = 1;
   /*-----------------------------------------
      ; Test the first five bytes of the message
      ------------------------------------------*/
   while(i<(size-1)) 
   {
      if ((*msg_ptr & 0x80) == 0)
      {
	 i = size;
      }
      else 
      {
	 i++;
	 msg_ptr++;
	 pos = i + 1;
      }
   }
   /*----------------------------------------
      ; Test the last byte of the message
      ----------------------------------------*/
   if ((i == (size-1)) && ((*msg_ptr & 0x80) == 0))
   {
      pos = size;
   }
   return(pos);
}

/*********************************************************************
; Name: M_copy - Copy a message.
;
; Description:
; Copy message pointed to by "src" to message pointed to by "dest" for a
; length equal to "size".
;
; Parameters:
; Source Pointer:	(input)         ptr to the start of the src message.
; Destination Pointer:	(input)         ptr to the start of the dest message.
; Data Size:		(input)         number of bytes to be copied.
;********************************************************************/
void    M_copy (BYTE_T *src_ptr, BYTE_T *dest_ptr, INT_T size) 
   
{
   
   BYTE_T  i;
   
   for (i=1; i<=size; i++) 
   {
      *dest_ptr = *src_ptr;
      dest_ptr++;
      src_ptr++;
   }
}

/*********************************************************************
; Name: M_init - Initialize instance of a message.
;
; Description:
; Initialize the instance of a particular message by setting the
; associated pointer to the first byte of the message.
;
; Parameters:
; Message Pointer:	(input)         pointer to the message instance.
;********************************************************************/
void    M_init (MESSAGE_T *M_ptr)
   
{
   
   INT_T i;
   
   M_ptr->ptr = &(M_ptr->msg[0]);
   for (i=0;i<MSG_LEN;i++)
      M_ptr->msg[i] = 0;
}

/*********************************************************************
; Name: M_get_type - Get message type
;
; Description:
; Receives a pointer to a message and returns the message type. This is a
; common feature of all messages.
;
; Parameters:
; Message Pointer:	(input)         pointer to the start of the message.
; Message Type:		(output)        Message type.
;********************************************************************/
BYTE_T  M_get_type (BYTE_T *msg_ptr)
{
   return((*msg_ptr & 0x70) >> 4);
}

/****************************************************************************
; Name: M_get_subtype - Get message subtype
;
; Description:
; Receives a pointer to a message and returns the message subtype. This is a
; common feature of all messages. (does not apply to message type 5 and
; message type 7)
;
; Parameters:
; Message Pointer     (input)         pointer to the start of the message.
; Message Type        (output)        Message subtype.
;***************************************************************************/
BYTE_T  M_get_subtype (BYTE_T *msg_ptr)
{
   return((*(msg_ptr + 1) & 0x60) >> 5);
}

/****************************************************************************
; Name: M_get_origin - Get message origin
;
; Description:
; Receives a pointer to a message and returns the message origin. This is a
; common feature of all messages.
;
; Parameters:
; Message Pointer    (input)         pointer to the start of the message.
; Message Type       (output)        Message origin.
;
;***************************************************************************/
BYTE_T  M_get_origin (BYTE_T *msg_ptr) 
{
   return(*msg_ptr & 0x0f);
}

/****************************************************************************
; Name: M_get_dest - Get message destination
;
; Description:
; Receives a pointer to a message and returns the message destination.
;
; Parameters:
; Message Pointer     (input)         pointer to the start of the message.
; Message Type        (output)        Message destination.
;
;***************************************************************************/
BYTE_T  M_get_dest (BYTE_T *msg_ptr) 
{
   return(*(msg_ptr + 1) & 0x0f);
}

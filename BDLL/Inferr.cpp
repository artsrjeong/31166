/* U10 Project: @(#)inferr.c	2.3	3/23/95	14:22:47 */
/**************************************************************************
; File Name: INFERR.C - Error Handler
;
; File Description:
; This file contains modules that monitor the occurance of system errors.
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
; A1730203BAA           Geoff Mochau             02 February 1995
; - SCR 30203-38: Reference renamed modules.
;
;*************************************************************************/
#include "stdafx.h"
#include "common.h"

/**************************************************************************
;  Public Declarations
;*************************************************************************/
#define EXTERN extern   /* declare public variables */
#include "pi.h"
#include "calender.h"
#undef EXTERN

/**************************************************************************
;  Public Definitions
;*************************************************************************/
#define EXTERN          /* define public variables */
#include "inferr.h"
#undef EXTERN

/**************************************************************************
;  Local Definitions
;*************************************************************************/
/* None */


/**************************************************************************
;  Public Functions
;*************************************************************************/

/*********************************************************************
; Name: E_init
;
; Description: Initialize all error counters to zero.
;
; Parameters: NONE
;********************************************************************/
void E_init (void) 
   
{
   
   INT_T i,j;

   inferr_Clear();


   /*  tmd  2000/3/2
   for(i=FRAMING_ERROR; i<MAX_ERRORS; i++)	
    {
      error_info.type[i].Count = 0;
     for(j=0;j<10;j++)
     {
      error_info.type[i].Year[j]  = 0;
      error_info.type[i].Month[j] = 0;
      error_info.type[i].Day[j]   = 0;
      error_info.type[i].Hour[j]  = 0;
      error_info.type[i].Minute[j] = 0;
      error_info.type[i].Second[j] = 0;
     }
    }
   */
}

/*********************************************************************
; Name: inferr_Clear
;
; Description: Initialize all error counters to zero.
;
; Parameters: NONE
;********************************************************************/
void inferr_Clear(void) 
   
{
   
   INT_T i,j;

   for(i=0; i<MAX_ERRORS; i++)	
    {
      error_info.type[i].Count = 0;
     for(j=0;j<10;j++)
     {
      error_info.type[i].Year[j]  = 0;
      error_info.type[i].Month[j] = 0;
      error_info.type[i].Day[j]   = 0;
      error_info.type[i].Hour[j]  = 0;
      error_info.type[i].Minute[j] = 0;
      error_info.type[i].Second[j] = 0;
     }
    }

   error_InfoModified = TRUE;

   //Restore into Flash Immediately
   //flash_ErrCheckCount = flash_ErrRestoreInterval;

}

/*********************************************************************
; Name: E_log
;
; Description: Update the occurance of a particular type of error.
;
; Parameters:
; Error Type	(input)	- Specifies the type of error which has been
;			  detected.
;********************************************************************/

void E_log (BYTE_T error_num) 
   
{
  BYTE_T hour,minute,second;
  BYTE_T year,month,day,week,i;
  calender_DataT *calender;

  rtc_GetTime(&hour,&minute,&second);
  rtc_GetDate(&year,&month,&day,&week);

 for(i=9;i>0;i--)
  {
    error_info.type[error_num].Year[i] = error_info.type[error_num].Year[i-1];
    error_info.type[error_num].Month[i] = error_info.type[error_num].Month[i-1];
    error_info.type[error_num].Day[i] = error_info.type[error_num].Day[i-1];
    error_info.type[error_num].Hour[i] = error_info.type[error_num].Hour[i-1];
    error_info.type[error_num].Minute[i] = error_info.type[error_num].Minute[i-1];
    error_info.type[error_num].Second[i] = error_info.type[error_num].Second[i-1];
  }

  error_info.type[error_num].Year[0]	= year;
  error_info.type[error_num].Month[0]	= month;
  error_info.type[error_num].Day[0]	= day;
  error_info.type[error_num].Hour[0]	= hour;
  error_info.type[error_num].Minute[0]	= minute;
  error_info.type[error_num].Second[0]	= second;

   if (error_num < MAX_ERRORS) 
   {
      if (error_info.type[error_num].Count < MAX_ERROR_CNTS) 
      {
	 error_info.type[error_num].Count++;
      }
   }

   error_InfoModified = TRUE;
}

/*********************************************************************
; Name: E_cnt
;
; Description: count the occurance of a particular type of error.
;
; Parameters:
; Error Type	(input)	- Specifies the type of error which has been
;			  detected.
;********************************************************************/

void E_cnt (BYTE_T error_num) 
   
{

   if (error_num < MAX_ERRORS) 
   {
      if (error_info.type[error_num].Count < MAX_ERROR_CNTS) 
      {
	 error_info.type[error_num].Count++;
      }
   }

}

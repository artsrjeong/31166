/*******************************************************************************
;  File Name: TIMER.H
;
;  File Descriptions:
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
;  XXX-30461-XXX         Takayuki Yoshida              13 October 1999
*******************************************************************************/
#ifndef timer_AlreadyIncludeHeader    /* protects from multiple includes */
#define timer_AlreadyIncludeHeader

#ifdef timer_ModuleDefinition
#define timer_EXTERN
#else
#define timer_EXTERN extern
#endif   // timer_ModuleDefinition

/*******************************************************************************
; Constants
*******************************************************************************/
/*--- NOTES --------------------------------------------------------------------
;  The timer constants are used to set up timers.
;  The lowest timer constant is equal to the timer driver task rate.
;  The base value "TIM_100MS" MUST NOT BE ZERO. If you set 0, the exception
;  "devided by zero" causes in the file "timer.c."
------------------------------------------------------------------------------*/
#define TIM_0           0
// TestInHonma #define TIM_100MS       10
#define TIM_100MS       1
#define TIM_200MS       (TIM_100MS * 2)
#define TIM_300MS       (TIM_100MS * 3)
#define TIM_400MS       (TIM_100MS * 4)
#define TIM_500MS       (TIM_100MS * 5)
#define TIM_600MS       (TIM_100MS * 6)
#define TIM_700MS       (TIM_100MS * 7)
#define TIM_800MS       (TIM_100MS * 8)
#define TIM_900MS       (TIM_100MS * 9)
#define TIM_1S          (TIM_100MS * 10)
#define TIM_2S          (TIM_1S * 2)
#define TIM_3S          (TIM_1S * 3)
#define TIM_4S          (TIM_1S * 4)
#define TIM_5S          (TIM_1S * 5)
#define TIM_6S          (TIM_1S * 6)
#define TIM_7S          (TIM_1S * 7)
#define TIM_8S          (TIM_1S * 8)
#define TIM_9S          (TIM_1S * 9)
#define TIM_10S         (TIM_1S * 10)
#define TIM_20S         (TIM_1S * 20)
#define TIM_30S         (TIM_1S * 30)
#define TIM_1M          (TIM_1S * 60)
#define TIM_2M          (TIM_1M * 2)
#define TIM_3M          (TIM_1M * 3)
#define TIM_4M          (TIM_1M * 4)
#define TIM_5M          (TIM_1M * 5)
#define TIM_6M          (TIM_1M * 6)
#define TIM_7M          (TIM_1M * 7)
#define TIM_8M          (TIM_1M * 8)
#define TIM_9M          (TIM_1M * 9)
#define TIM_10M         (TIM_1M * 10)
#define TIM_20M         (TIM_1M * 20)
#define TIM_30M         (TIM_1M * 30)
#define TIM_1H          (TIM_1M * 60)


/**************************************************************************
;  Data Types
;*************************************************************************/
/*
;  The timer type is used to declare a variable that is to be used
;  with the timer constants.
; 
;  ex.  TIMER_T Error_Timeout;  // declare a TIMER_T variable
;         Error_Timeout = TIM_500MS;
;               .
;               .
;         if (Error_Timeout == TIM_0)  ...
;*/
typedef LONG_T TIMER_T;
typedef TIMER_T *TIMER_TABLE_T;  /* define a timer table type */
#define TIMER_TABLE_END   ((TIMER_T *) 0)  /* end of timer table */


/**************************************************************************
;  Variables
;*************************************************************************/
timer_EXTERN  TIMER_T timer_CurrentTime;         /* used with the elapsed routine */
timer_EXTERN  LONG_T  timer_CurrentSystemTicks;  /* 100ms 'ticks' since system started */

timer_EXTERN  FLAG_T  timer_Driver_ExecFlag;  /* flag to reserch that timer_Driver executes */

/**************************************************************************
;  Function Prototypes
;*************************************************************************/
timer_EXTERN void timer_Driver(void);
timer_EXTERN void timer_Init(void);
timer_EXTERN TIMER_T timer_ElapsedInit(void);
timer_EXTERN TIMER_T timer_ElapsedTicks(TIMER_T);
timer_EXTERN TIMER_T timer_ElapsedSeconds(TIMER_T);
timer_EXTERN void timer_GetSystemTime(LONG_T *SystemTime);
timer_EXTERN void timer_GetSystemTicks(LONG_T *SystemTicks);


/**************************************************************************
;  Macro Functions     Inline Codes
;*************************************************************************/
/* groupbus compatible */
#define Get_System_Time(/* LONG_T * */systemTimePtr)        \
        (timer_GetSystemTime(systemTimePtr))  \

#define Current_System_Ticks  timer_CurrentSystemTicks

#endif   // timer_AlreadyIncludeHeader

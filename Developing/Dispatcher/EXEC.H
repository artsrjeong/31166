/*******************************************************************************
;  File Name: EXEC.H
;
;  File Descriptions:
;  - Task Handler
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
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Created (renamed) module from "exec_p.h".
;
; AAA302033AA           Mark Ross                       28 November 1995
; - SCR 30203-209: Removed all references to variables used to 
; monitor executive skews. The idea of an exec skew does not 
; make sense on the GCB  since the timer interrupt is not an 
; NMI as it was with RCBII.        
;
;  XXX-30461-XXX         Takayuki Yoshida              29 September 1999
;  - Change for GCB-1-DX Rev.B1
*******************************************************************************/
#ifndef exec_AlreadyIncludeHeader    /* protects from multiple includes */
#define exec_AlreadyIncludeHeader
       
#ifdef exec_ModuleDefinition
#define exec_EXTERN
#else
#define exec_EXTERN extern
#endif   // exex_ModuleDefinition

/*******************************************************************************
; Constants
*******************************************************************************/
/* task table definitions
;  The task rate constants are used to set up task scheduling rates.
;  The values for the timer constants are multiples of the scheduler
;  rate.  These values only need to be changed when the scheduler rate
;  changes.  There is no limit to the number of timer constants.
; 
;  ex.  With the scheduler rate equal to 20 mS, the constant for an 
;       800 mS task rate would be 40.  Since 800 mS is 40 multiples
;       of the scheduler rate (20 mS).  So the following would be
;       added to the task rate constant list.
; 
;       #define TSK_800MS    40
*/

#define TASK_TABLE_END ((void (*) (void)) 0)  /* end of task table */
#define TSK_0           0                     /* defines an expired timer */
#define TSK_EVENT      -1                     /* defines an event only task */
#define TSK_10MS        1
#define TSK_20MS        (TSK_10MS * 2)
#define TSK_30MS        (TSK_10MS * 3)
#define TSK_40MS        (TSK_10MS * 4)
#define TSK_50MS        (TSK_10MS * 5)
#define TSK_60MS        (TSK_10MS * 6)
#define TSK_70MS        (TSK_10MS * 7)
#define TSK_80MS        (TSK_10MS * 8)
#define TSK_90MS        (TSK_10MS * 9)
#define TSK_100MS       (TSK_10MS * 10)
#define TSK_110MS       (TSK_100MS + TSK_10MS)
#define TSK_120MS       (TSK_100MS + TSK_20MS)
#define TSK_130MS       (TSK_100MS + TSK_30MS)
#define TSK_140MS       (TSK_100MS + TSK_40MS)
#define TSK_150MS       (TSK_100MS + TSK_50MS)
#define TSK_160MS       (TSK_100MS + TSK_60MS)
#define TSK_170MS       (TSK_100MS + TSK_70MS)
#define TSK_180MS       (TSK_100MS + TSK_80MS)
#define TSK_190MS       (TSK_100MS + TSK_90MS)
#define TSK_200MS       (TSK_100MS * 2)
#define TSK_250MS       (TSK_200MS + TSK_50MS)
#define TSK_300MS       (TSK_100MS * 3)
#define TSK_350MS       (TSK_300MS + TSK_50MS)
#define TSK_400MS       (TSK_100MS * 4)
#define TSK_500MS       (TSK_100MS * 5)
#define TSK_600MS       (TSK_100MS * 6)
#define TSK_700MS       (TSK_100MS * 7)
#define TSK_800MS       (TSK_100MS * 8)
#define TSK_900MS       (TSK_100MS * 9)
#define TSK_1S          (TSK_100MS * 10)
#define TSK_1P5S        (TSK_1S + TSK_500MS)
#define TSK_2S          (TSK_1S * 2)
#define TSK_3S          (TSK_1S * 3)
#define TSK_4S          (TSK_1S * 4)
#define TSK_5S          (TSK_1S * 5)
#define TSK_6S          (TSK_1S * 6)
#define TSK_7S          (TSK_1S * 7)
#define TSK_8S          (TSK_1S * 8)
#define TSK_9S          (TSK_1S * 9)
#define TSK_10S         (TSK_1S * 10)
#define TSK_15S         (TSK_1S * 15)
#define TSK_20S         (TSK_1S * 20)
#define TSK_30S         (TSK_1S * 30)
#define TSK_1M          (TSK_1S * 60)
#define TSK_3M          (TSK_1M * 3) 
#define TSK_5M          (TSK_1M * 5) 
#define TSK_10M         (TSK_1M * 10) 
#define TSK_20M         (TSK_1M * 20) 
/* with a 20 mS exec rate the slowest task rate is 20 minutes */

#define EXEC_10MS_COUNT  2
/*******************************************************************************
; Data Types
*******************************************************************************/
struct TASK_S {
   /*------------------------------------------------------------------
    These data items are made public so that monitoring devices can 
      examine the status of the tasks.  Data should only be examined. 
   ------------------------------------------------------------------*/
   /* initialized table data */
   void (* Address) (void);   /* task address */
   char *Name;                /* task name */
   LONG_S Rate;                /* task rate  (use a TSK_x constant) */
   LONG_T Init_Delay;          /* delay before starting (use a TSK_x constant) */
   FLAG_T Schedule_On;        /* task scheduling is enabled */

   /* unitialized data */
   LONG_T Timer;               /* task timer */
   LONG_T Schedule;            /* task scheduled to run count */
   LONG_T Skews;               /* number of times task has skewed */
   LONG_T Max_Skew;            /* maximum task rate frames a task has skewed */
   FLAG_T   Running;          /* task is running */
};
typedef struct TASK_S TASK_T;
typedef LONG_S TASK_ID_T;

/*******************************************************************************
;  Variables
;******************************************************************************/
/* These data items are made public so that monitoring devices can
 * examine the status of the executive and the tasks it is rvvvunning.
 * Data should only be examined. */
exec_EXTERN TASK_T *exec_TaskPtr;      // pointer to start of the task table
exec_EXTERN LONG_T exec_MaxTasks;       // number of tasks in the task table
exec_EXTERN LONG_T exec_duplicateExec;
exec_EXTERN FLAG_T exec_TaskSkewWarning;
exec_EXTERN LONG_T exec_TaskSkews;
exec_EXTERN LONG_T exec_MaxTaskSkew;
exec_EXTERN LONG_T exec_TaskScheduled;  // total number of scheduled tasks
exec_EXTERN LONG_T exec_Count;
exec_EXTERN LONG_T exec_TaskEnableFail;
exec_EXTERN LONG_T exec_TaskDisableFail;
exec_EXTERN LONG_T Exec_Check_Enable_Task_IdOutOfRange;
/* to watch the current task */
exec_EXTERN LONG_T exec_Current_Task_Number;
/******************************************************************************
;  Function Prototypes
;*****************************************************************************/
exec_EXTERN void exec_Init(void);
exec_EXTERN void exec_Driver(void);
exec_EXTERN void exec_TaskHandler(void);
exec_EXTERN TASK_ID_T exec_GetTaskID(void (* Task_Address)());
exec_EXTERN FLAG_T exec_EnableTask(TASK_ID_T Task_ID);
exec_EXTERN FLAG_T exec_DisableTask(TASK_ID_T Task_ID);
exec_EXTERN FLAG_T exec_CheckEnableTask(TASK_ID_T taskID);
exec_EXTERN LONG_S exec_ChangeTaskRate(TASK_ID_T taskId, LONG_S newRate);
exec_EXTERN LONG_S exec_GetTaskRate(TASK_ID_T taskId);

//just test
exec_EXTERN LONG_T exec_MaskFlag;
exec_EXTERN LONG_T exec_SetMask(LONG_T );


exec_EXTERN TASK_ID_T Exec_Get_Task_ID(void (*) ());
exec_EXTERN FLAG_T Exec_Enable_Task(TASK_ID_T);
exec_EXTERN FLAG_T Exec_Enable_And_Run_Task(TASK_ID_T);
exec_EXTERN FLAG_T Exec_Disable_Task(TASK_ID_T);
exec_EXTERN FLAG_T Exec_Modify_Task_Interval(TASK_ID_T, INT_T);


/**************************************************************************
;  Macro Functions     Inline Codes
;*************************************************************************/
// compatible with groupbus
#ifdef TEMP
#define Exec_Enable_Task(/* TASK_ID_T */ taskId)        \
        (exec_EnableTask(taskId))                       \

#define Exec_Check_Enable_Task(/* TASK_ID_T */ taskId)  \
        (exec_CheckEnableTask(taskId))                  \

#define Exec_Get_Task_ID(/* void * */ taskFuncPtr)      \
        (exec_GetTaskID(taskFuncPtr))                   \

#endif

#endif  /* end of include file definition */

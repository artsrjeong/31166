/*******************************************************************************
; File Name: EXEC.T
;
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
;  XXX-30461-XXX         Takayuki Yoshida              29 September 1999
;
;  XXX-30301-XXX         Kenichi  Ohmura               29 January   2002
;    Modified this file so that the GCB3 communication CPU might operate
;    based on the file of GCB1-DX.
;
*******************************************************************************/

/******************************************************************************
;  Public Declarations
;******************************************************************************/
#define EXTERN extern   /* declare public variables */
#include "sio.h"
#include "flash.h"
#include "calender.h"
#include "infbld.h"
#include "infrng.h"
#include "infdes.h"
#include "assign.h"
#include "infads.h"
#include "crowd.h"
#include "asslib.h"
#include "hdw.h"
#include "led.h"
#include "svtool.h"
#undef EXTERN

/**************************************************************************
;  Task Table Definition
;
;  TASK ADDRESS
;    The task address.
;
;  TASK NAME
;    The task name.
;
;  TASK RATE
;    Defines the task rate.  Use one of the pre-defined TSK_x values.  Use
;    the predefind TSK_EVENT value to define an event based task.  This
;    is a task that runs once when it is enabled and then turns off.
;
;  DELAY TIME
;    The delay that is taken before the task is initially started.  Use
;    one of the predefined TSK_x values.  A delay time can be used for
;    two purposes.  One is to provide a means to stagger tasks executing
;    at the same task rate.  This prevents a sudden burst of CPU usage
;    at one specific time.  The second purpose of a delay time is to
;    provide a delay when triggering an event type task.
;
;  INIT STATE
;    TRUE =  Task is initially enabled.
;    FALSE = Task is initially disabled.
;*************************************************************************/
TASK_T Task_Table[] = {  /*
                                                             TASK       DELAY      INIT
    TASK ADDRESS                          TASK NAME          RATE       TIME       STATE
  --------------------------------------------------------------------------------------- */
  /* these three kernel tasks require highest priority */
  { hdw_Monitor,                         "HDW Monitor",     TSK_5S,    TSK_5S,    TRUE },
  { sio_Monitor,                         "SIO Monitor",     TSK_1S,    TSK_40MS,  TRUE },
/* 
  { timer_Driver,                        "Timer Driver",    TSK_100MS, TSK_60MS,  TRUE },
  { ads_com_tsk,                 "ADSS Com",      TSK_20MS,  TSK_20S,   FALSE}, // 3 

  { ring1_com_tsk,               "Ring 1 Com",    TSK_30MS,  TSK_20MS,  TRUE }, // 4 
  { ring2_com_tsk,               "Ring 2 Com",    TSK_30MS,  TSK_40MS,  TRUE }, // 5 
  { DES_TransmitRing,            "DES TRng",      TSK_100MS, TSK_10MS,  TRUE },
  { DES_Manager,                 "DES Mngr",      TSK_200MS, TSK_10MS,  TRUE },
  { DES_StandardMessageHandler,  "DES StdM",      TSK_60MS,  TSK_10MS,  TRUE },
  { DES_CommunicationHandler,    "DES Comm",      TSK_30MS,  TSK_10MS,  TRUE },
  { DES_PriorityMessageHandler,  "DES PriM",      TSK_30MS,  TSK_10MS,  TRUE },

//  { calender_Driver,                     "Calender Drv",    TSK_1S,    TSK_80MS,  TRUE },
  { led_Driver,                          "LED Driver",      TSK_250MS, TSK_100MS, TRUE },
  { SV_Driver,                           "SV Tool Drv",     TSK_100MS, TSK_600MS, TRUE },

  { ring1_proc_msg_tsk,          "Ring 1 Proc",   TSK_50MS,  TSK_30MS,  TRUE },
  { ring2_proc_msg_tsk,          "Ring 2 Proc",   TSK_50MS,  TSK_50MS,  TRUE },
//3
//  { ads_com_tsk,                 "ADSS Com",      TSK_20MS,  TSK_20S,   FALSE}, // 3 


  { infrng_status_msg_tsk,       "Stat msg tsk",  TSK_1S,    TSK_10S,   TRUE },
  { reassign_check_reassign_tsk, "ICA reassign",  TSK_300MS, TSK_0,     FALSE}, // 11 
  { asslib_ass_period_tsk,       "ICA int. ass",  TSK_500MS, TSK_0,     TRUE},
  { trafdat_opmode_tsk,          "TRAF mode sel", TSK_10S,   TSK_100MS, FALSE}, // 14 
  { trafdat_car_select_tsk,      "TRAF MIT cars", TSK_2S,    TSK_200MS, FALSE}, // 15 
  { fo_car_depart_tsk,           "Fuz car deprt", TSK_EVENT, TSK_0,     FALSE}, // 16 
  { fo_car_arrive_tsk,           "Fuz car arriv", TSK_EVENT, TSK_0,     FALSE}, // 17 
  { predir_tsk,                  "Predir",        TSK_EVENT, TSK_0,     FALSE}, // 18 
  { ocss_icss_monitor_tsk,       "O/I Monitor",   TSK_1S,    TSK_200MS, TRUE }, // 19 
  { hdw_AuxMonitor,                      "Hdw Aux Mon",     TSK_1M,    TSK_1M,    TRUE },
  { crowd_estimate_tsk,          "Crowd",         TSK_1S,    TSK_100MS, FALSE}, // 21 
  { assign_new_tsk,              "ICA assign",    TSK_EVENT, TSK_0,     FALSE}, // 22 

  { park_NormalTask,             "PARK NOR",      TSK_1S,    TSK_10S,   TRUE }, // 24 
  { park_MitTask,                "PARK MIT",      TSK_1S,   (TSK_10S+TSK_500MS),  TRUE}, // 24 
  { trafdat_UpdateDatabaseTask,  "Traffic Data ", TSK_1M,    TSK_9S,    TRUE  },  // 25
   { flash_Driver,                   "Flash Mem Drv",    TSK_4S,     TSK_5S,     TRUE  },  // 15
*/
  {TASK_TABLE_END}          /* end terminator */    /* LOWEST priority */
}; /* Task_Table */



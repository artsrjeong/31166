/*******************************************************************************
; File Name: HDW.H  
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
;  XXX-30461-XXX         Takayuki Yoshida              17 September 1999
;  XXX-30301-XXX         Kenichi Ohmura                
*******************************************************************************/
#ifndef hdw_AlreadyIncludeHeader    /* protects from multiple including */
#define hdw_AlreadyIncludeHeader

#ifdef hdw_ModuleDefinition
#define hdw_EXTERN
#else
#define hdw_EXTERN extern
#endif   // hdw_ModuleDefinition


/*******************************************************************************
; Constants
*******************************************************************************/
#define  HDW_10MS_COUNT  2
#define  HDW_20MS_COUNT  4
/*-------------------------------------------
; SCC RTS CONTROL  to WatchDog
--------------------------------------------*/
#define  DOG_REG          *(BYTE_T *)0xA80005
#define  HDW_DOGSET       0x02/* TO RTS OUTPUT LO */
#define  HDW_DOGRESET     0xfd/* TO RTS OUTPUT HI */

/*---------------------------------------------------------
; Code Domain, Loop Counter 
---------------------------------------------------------*/
/* bgrnd_Background()de, hdw_CPUFreeTime ga, 5msec goto  */
/* ni 1 count up sareru you ni shitai.                   */
/* hdw_Monitor()ha, sio ni nyuuryoku sareru timer        */
/* interrupt ni yori, 5sec goto ni yobareru.             */
/* warikomi ga timer interrupt igai nai you ni shi,      */
/* hdw_Monitor() ga yobareru toki ni hdw_CPUFreeTime ga  */
/* 1000 ni nareaba, hdw_CPUFreeTime ha 5msec goto ni 1   */
/* count up sarete iru kotoni naru hazu.                 */
/* sou naru you na ROM5MS to FLASH5NS wo tyousa suru.    */
/* hontou ha keisokuki wo tukatta houga yoi kamo sirenai.*/
/*-------------------------------------------------------*/
/*********** ROM5MS **************************************/
/* UV-EPROM     - TMS27C010A-10                          */
/* ROM EMULATOR - SENNA IV                               */
/* task wo riyou shite, software de tyousa shita tokoro, */
/* SENNA IV to EPROM no kekka ha, onaji de atta.         */
/* 2908 ha, hdw_CPUFreeTime ga 1 kai ooi kurai.(1001)    */
/* 2909 ha, hdw_CPUFreeTime ga 1 kai sukunai kurai.(999) */
/*-------------------------------------------------------*/
//TestInHonma #define  ROM5MS          1669
#define  ROM5MS          2909
//#define  ROM5MS          2400
/*********** FLASH5MS ************************************/
/* FLASH MEMORY - MBM29F2800TA-90PFTN                    */
/* 3841 -                                                */
/*-------------------------------------------------------*/
#define  FLASH5MS        3841
/*********** SRAM5MS *************************************/
/* SRAM - HM628512BLFP-5                                 */
/* 3841 -                                                */
/*-------------------------------------------------------*/
#define  SRAM5MS        3841

/* memory architecture ID */
#define  ROM            0
#define  FLASH          1
#define  SRAM           2

/* Code Area */
#define  ROMSTART       0x00E00000
#define  ROMEND         0x00EFFFFF
#define  FLASHSTART     0x00C00000
#define  FLASHEND       0x00DFFFFF
#define  SRAMSTART      0x00000000
#define  SRAMEND        0x001FFFFF

#define HDW_MAX_REFRESHCOUNTS   2000 /* (hdw_MonitorTask rate[ms])/(timer interrupt rate[ms])*protection(twice) */

/**************************************************************************
;  Global Variables
;*************************************************************************/
hdw_EXTERN   LONG_T    hdw_CPUUse;        /* current CPU utilization (0 - 1000)   */
hdw_EXTERN   LONG_T    hdw_CPUUseOld;    /* current CPU utilization (0 - 1000)   */
hdw_EXTERN   LONG_T    hdw_MaxCPUUse;    /* max CPU utilization (0 - 1000)       */
hdw_EXTERN   LONG_T    hdw_MaxStackUse;  /* max stack use                        */
hdw_EXTERN   LONG_T    hdw_CurStackUse;  /* current stack use                    */
hdw_EXTERN   LONG_T    hdw_CPUFreeTime;  /* used to tally CPU free time          */
hdw_EXTERN   LONG_T    hdw_MAXFreeTime;  /* used to tally CPU free time          */
hdw_EXTERN   LONG_T    hdw_ElapsedMin;    /* elapsed minutes counter              */
hdw_EXTERN   LONG_T    hdw_RefreshCount;      /* Task execution check */
hdw_EXTERN   BYTE_T   hdw_DogFood;          /* SIO.C */
hdw_EXTERN   FLAG_T   hdw_Monitor_Flag;     /* flag to reserch that hdw_Monitor executes */
hdw_EXTERN   FLAG_T   hdw_AuxMonitor_Flag;  /* flag to reserch that hdw_AuxMonitor executes */
hdw_EXTERN   LONG_T    hdw_miscIntCounter;

hdw_EXTERN   LONG_T    hdw_ProgramCounterWhenBusError;
hdw_EXTERN   LONG_T    hdw_ProgramCounterWhenAddressError;
hdw_EXTERN   LONG_T    hdw_ProgramCounterWhenIllegalInstraction;
hdw_EXTERN   FLAG_T    hdw_BusErrorOccured;
hdw_EXTERN   FLAG_T    hdw_AddressErrorOccured;
hdw_EXTERN   FLAG_T    hdw_IllegalInstractionOccured;

/**************************************************************************
;  Local Variables
;*************************************************************************/
/* Add many interrupt counters  09 June 2000 */
hdw_EXTERN   LONG_T    hdw_BusErr_02IntCnt;
hdw_EXTERN   LONG_T    hdw_AdrsErr_03IntCnt;
hdw_EXTERN   LONG_T    hdw_IllegalInst_04IntCnt;
hdw_EXTERN   LONG_T    hdw_DividedByZero_05IntCnt;
hdw_EXTERN   LONG_T    hdw_ChkErr_06IntCnt;
hdw_EXTERN   LONG_T    hdw_CopPro_0DIntCnt;
hdw_EXTERN   LONG_T    hdw_misc08IntCnt;
hdw_EXTERN   LONG_T    hdw_misc09IntCnt;
hdw_EXTERN   LONG_T    hdw_misc0AIntCnt;
hdw_EXTERN   LONG_T    hdw_misc0BIntCnt;
hdw_EXTERN   LONG_T    hdw_misc0CIntCnt;
hdw_EXTERN   LONG_T    hdw_misc0DIntCnt;
hdw_EXTERN   LONG_T    hdw_misc0EIntCnt;
hdw_EXTERN   LONG_T    hdw_misc0FIntCnt;
hdw_EXTERN   LONG_T    hdw_misc18IntCnt;
hdw_EXTERN   LONG_T    hdw_misc19IntCnt;
hdw_EXTERN   LONG_T    hdw_misc1AIntCnt;
hdw_EXTERN   LONG_T    hdw_misc1BIntCnt;
hdw_EXTERN   LONG_T    hdw_misc1CIntCnt;
hdw_EXTERN   LONG_T    hdw_misc1DIntCnt;
hdw_EXTERN   LONG_T    hdw_misc1EIntCnt;
hdw_EXTERN   LONG_T    hdw_misc1FIntCnt;
hdw_EXTERN   LONG_T    hdw_misc20IntCnt;
hdw_EXTERN   LONG_T    hdw_misc21IntCnt;
hdw_EXTERN   LONG_T    hdw_misc22IntCnt;
hdw_EXTERN   LONG_T    hdw_misc23IntCnt;
hdw_EXTERN   LONG_T    hdw_misc24IntCnt;
hdw_EXTERN   LONG_T    hdw_misc25IntCnt;
hdw_EXTERN   LONG_T    hdw_misc26IntCnt;
hdw_EXTERN   LONG_T    hdw_misc27IntCnt;
hdw_EXTERN   LONG_T    hdw_misc28IntCnt;
hdw_EXTERN   LONG_T    hdw_misc29IntCnt;
hdw_EXTERN   LONG_T    hdw_misc2AIntCnt;
hdw_EXTERN   LONG_T    hdw_misc2BIntCnt;
hdw_EXTERN   LONG_T    hdw_misc2CIntCnt;
hdw_EXTERN   LONG_T    hdw_misc2DIntCnt;
hdw_EXTERN   LONG_T    hdw_misc2EIntCnt;
hdw_EXTERN   LONG_T    hdw_misc2FIntCnt;
hdw_EXTERN   LONG_T    hdw_Fpcp_30IntCnt;
hdw_EXTERN   LONG_T    hdw_Fpcp_31IntCnt;
hdw_EXTERN   LONG_T    hdw_Fpcp_32IntCnt;
hdw_EXTERN   LONG_T    hdw_Fpcp_33IntCnt;
hdw_EXTERN   LONG_T    hdw_Fpcp_34IntCnt;
hdw_EXTERN   LONG_T    hdw_Fpcp_35IntCnt;
hdw_EXTERN   LONG_T    hdw_Fpcp_36IntCnt;

hdw_EXTERN   FLAG_T   hdw_watchDogOK;        /* watchdog OK */
hdw_EXTERN   FLAG_T   hdw_wdToggle;

/* stack usage data */
hdw_EXTERN   LONG_T    hdw_initStackPtr;    /* initial stack pointer */
hdw_EXTERN   LONG_T    hdw_lowestStackPtr;  /* lowest postion stack pointer has reached */
hdw_EXTERN   LONG_T    hdw_shallowStackPtr; /* shallow non-zero search */
hdw_EXTERN   LONG_T    hdw_deepStackPtr;    /* deep non-zero search */

hdw_EXTERN   BYTE_T    hdw_WhereIam;

/**************************************************************************
;  Function Prototypes
;*************************************************************************/
#ifndef PC_SIMUL
hdw_EXTERN   void   hdw_Init(void);          /* INIT.C */
#endif
hdw_EXTERN   void   hdw_WatchDog(void);      /* INIT.C */
hdw_EXTERN   void   hdw_Monitor(void);       /* EXEC.T */
hdw_EXTERN   void   hdw_AuxMonitor(void);    /* EXEC.T */
hdw_EXTERN   BYTE_T hdw_WhereAmI(void);
hdw_EXTERN   LONG_T  hdw_Get5msCount(void);   /* BGRND.C, RTC.C */
hdw_EXTERN   void   hdw_Reset(void);          /* SVTOOL.T */


/**************************************************************************
;  Macro Functions  (In-Line Codes)
;*************************************************************************/

#endif   // hdw_AlreadyIncludeHeader


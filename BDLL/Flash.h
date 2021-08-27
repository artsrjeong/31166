/*--------------------------------------------------------------------------
|  File Name : FLASH.H
|
|  Description:
|
|    This module provides services that allow an flash memory access.
|    - MBM29F800TA (Fujitsu)
|    - MBM29F016A  (Fujitsu)
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
|  Unpublished work - Copyright 1992 - 1995 Otis Elevator Company
|  All rights reserved.
|
|
|      SCN               Author             Date
|  -------------------------------------------------------------
|  ---30299/30301---     T. Mori            28-May-1996
|  Created module.
|
|  AAB-30299-2AA      Mark Ross                  02-Apr-1997
|  - SCR 30299-68. Integrated changes in 4/1/97 email from Honma-san.
|                  Write error logs to FLASH during normal operation.
|                  Update RAM copy of error logs with FLASH on power up.
|
|  AAB-30299-2AA     Kate Goldstone              23-Jun-1997
|  - SCR 30299-87. Integrate changes from OTI's June visit.
|
|  AAB-30299-6AA     Hideyuki Honma              11-Oct-1998
|  - CHG 30299-152   Shuttle Car Operation(SCX)
|
|  XXX-30461-XXX     Takayuki Yoshida            08-Jun-1999
|  - Renewal for GCB1-DX Code and Data Domain
|
|  XXX-30461-XXX     Takayuki Yoshida            15-Oct-1999
|  - Renewal for GCB1-DX Rev.1 Code and Data Domain
|----------------------------------------------------------------*/
#ifndef flash_AlreadyIncludeHeader
#define flash_AlreadyIncludeHeader

/*--------------------------------------------------------*/

#ifdef  flash_ModuleDefinition
#define flash_EXTERN
#else
#define flash_EXTERN extern
#endif



/*******************************************************************************
; Include Global Declarations
*******************************************************************************/
//#include "flashdrv.h"
#include "timer.h" //development

/*******************************************************************************
; Macro Definitions
; - Code Domain (MBM29F800TA) Address & Command
; - Data Domain (MBM29F016A)  Address & Command
*******************************************************************************/
#define    FLASH_UNKNOWNAREA     0
#define    FLASH_CODEAREA        1
#define    FLASH_DATAAREA        2

/********************************************
; MBM29F800TA
********************************************/
/*********************************************************
;
; Flash 28F800TA Memory Layout under GCB3 Memory
;     o The 1 device has 19 sectors(#0 - #18).
;     o 1 MBytes(Page0) + 1MBytes(Page1) = 2 MBytes
;     o Using with the 32bit bus of address and data.
;       (16bit bus usable also if changing codes)
;
;
;  ADDRESS
;    FROM
; CPU WATCHING
;            ---------------------------------
;   DFFFFFH  |                               | 
;            |   Sector 18 - 16 kBytes * 2   | 
;            |                               | 
;   DF8000H  ---------------------------------
;   DF7FFFH  |                               | 
;            |   Sector 17 -  8 kBytes * 2   | 
;            |                               | 
;   DF4000H  ---------------------------------
;   DF3FFFH  |                               | 
;            |   Sector 16 -  8 kBytes * 2   | 
;            |                               | 
;   DF0000H  ---------------------------------
;   DEFFFFH  |                               | 
;            |   Sector 15 - 32 kBytes * 2   | 
;            |                               | 
;   DE0000H  ---------------------------------
;   DDFFFFH  |                               | 
;            |   Sector 14 - 64 kBytes * 2   | 
;            |                               | 
;   DC0000H  ---------------------------------
;   DBFFFFH  |                               | 
;            |   Sector 13 - 64 kBytes * 2   | 
;            |                               | 
;   DA0000H  ---------------------------------
;   D9FFFFH  |                               | 
;            |   Sector 12 - 64 kBytes * 2   | 
;            |                               | 
;   D80000H  ---------------------------------
;   D7FFFFH  |                               | 
;            |   Sector 11 - 64 kBytes * 2   | 
;            |                               | 
;   D60000H  ---------------------------------
;   D5FFFFH  |                               | 
;            |   Sector 10 - 64 kBytes * 2   | 
;            |                               | 
;   D40000H  ---------------------------------
;   D3FFFFH  |                               | 
;            |   Sector 9 - 64 kBytes * 2    | 
;            |                               | 
;   D20000H  ---------------------------------
;   D1FFFFH  |                               | 
;            |   Sector 8 - 64 kBytes * 2    | 
;            |                               | 
;   D00000H  ---------------------------------
;   CFFFFFH  |                               | 
;            |   Sector 7 - 64 kBytes * 2    | 
;            |                               | 
;   CE0000H  ---------------------------------
;   CDFFFFH  |                               |
;            |   Sector 6 - 64 kBytes * 2    |
;            |                               |
;   CC0000H  ---------------------------------
;   CBFFFFH  |                               | 
;            |   Sector 5 - 64 kBytes * 2    | 
;            |                               | 
;   CA0000H  ---------------------------------
;   C9FFFFH  |                               | 
;            |   Sector 4 - 64 kBytes * 2    | 
;            |                               | 
;   C80000H  ---------------------------------
;   C7FFFFH  |                               | 
;            |   Sector 3 - 64 kBytes * 2    | 
;            |                               | 
;   C60000H  ---------------------------------
;   C5FFFFH  |                               | 
;            |   Sector 2 - 64 kBytes * 2    | 
;            |                               | 
;   C40000H  ---------------------------------
;   C3FFFFH  |                               | 
;            |   Sector 1 - 64 kBytes * 2    | 
;            |                               | 
;   C20000H  ---------------------------------
;   C1FFFFH  |                               | 
;            |   Sector 0 - 64 kBytes * 2    | 
;            |                               | 
;   C00000H  ---------------------------------
;
*********************************************************/
/* Start Address of Flash Memory Area */
/*------------------------------------*/
#define   FLASHCODE_BASEADRS       0x00C00000

/* Offset Address of Each Sector      */
/*------------------------------------*/
/* 32bit bus */
#define   FLASHCODE2_SECT0         0x00000000
#define   FLASHCODE2_SECT1         0x00020000
#define   FLASHCODE2_SECT2         0x00040000
#define   FLASHCODE2_SECT3         0x00060000
#define   FLASHCODE2_SECT4         0x00080000
#define   FLASHCODE2_SECT5         0x000a0000
#define   FLASHCODE2_SECT6         0x000c0000
#define   FLASHCODE2_SECT7         0x000e0000
#define   FLASHCODE2_SECT8         0x00100000
#define   FLASHCODE2_SECT9         0x00120000
#define   FLASHCODE2_SECT10        0x00140000
#define   FLASHCODE2_SECT11        0x00160000
#define   FLASHCODE2_SECT12        0x00180000
#define   FLASHCODE2_SECT13        0x001a0000
#define   FLASHCODE2_SECT14        0x001c0000
#define   FLASHCODE2_SECT15        0x001e0000
#define   FLASHCODE2_SECT16        0x001f0000
#define   FLASHCODE2_SECT17        0x001f4000
#define   FLASHCODE2_SECT18        0x001f8000
/* Maximum Size to Access One Time          */
/*   Sector 0 - 14 : 64 KBytes/flash memory */
/*   Sector 15     : 32 KBytes/flash memory */
/*   Sector 16, 17 :  8 KBytes/flash memory */
/*   Sector 18     : 16 KBytes/flash memory */
/*   Using double memories, each size * 2.  */
/*------------------------------------------*/
/* 32bit bus - 128 kBytes                        */
/* ("FLASHCODE_MAXSECTORSIZE" in flashdrv.h) * 2 */
#define   MAX_SECTOR_SIZE_DOUBLE  0x00020000

/********************************************
; MBM29F016A
********************************************/
#define   FLASHDATA_BASEADRS      0x400000

/* list of FMEM ID */
#define FMEM_NOTSELECT            0
#define FMEM1                     1
#define FMEM2                     2
#define FMEM3                     3
#define FMEM4                     4
#define FMEM5                     5
#define FMEM6                     6
#define FMEM7                     7
#define FMEM8                     8
#define FMEM_MAXBANKS             9

#define FMEM_BUSY                 1
#define FMEM_FREE                 0

#define FMEM_ERASE_CMD            0
#define FMEM_WRITE_CMD            1

#define FMEM_SECT0                0
#define FMEM_SECT1                1
#define FMEM_SECT2                2
#define FMEM_SECT3                3
#define FMEM_SECT4                4
#define FMEM_SECT5                5
#define FMEM_SECT6                6
#define FMEM_SECT7                7
#define FMEM_SECT8                8
#define FMEM_SECT9                9
#define FMEM_SECT10              10
#define FMEM_SECT11              11
#define FMEM_SECT12              12
#define FMEM_SECT13              13
#define FMEM_SECT14              14
#define FMEM_SECT15              15
#define FMEM_SECT16              16
#define FMEM_SECT17              17
#define FMEM_SECT18              18
#define FMEM_SECT19              19
#define FMEM_SECT20              20
#define FMEM_SECT21              21
#define FMEM_SECT22              22
#define FMEM_SECT23              23
#define FMEM_SECT24              24
#define FMEM_SECT25              25
#define FMEM_SECT26              26
#define FMEM_SECT27              27
#define FMEM_SECT28              28
#define FMEM_SECT29              29
#define FMEM_SECT30              30
#define FMEM_SECT31              31




/*-----------------------------------------------------------------------------
; Saved Data Table ID
-----------------------------------------------------------------------------*/
/* data ID */
#define FLASHSAVE_OLD_DATABASE     0
#define FLASHSAVE_CUR_DATABASE     1
#define FLASHSAVE_FARCCPOS_PRED    2
#define FLASHSAVE_DAY_LEARN        3
#define FLASHSAVE_WEEK_LEARN       4
#define FLASHSAVE_JOBDATAPARAM     5
#define FLASHSAVE_JOBDATACONDITION 6
#define FLASHSAVE_FLOORPOS         7
#define FLASHSAVE_ERRLOGS          8
#define FLASHSAVE_MAXIDS           9


#define   flash_StartAdr         0x400000
#ifdef PC_SIMUL
#define   flash_CopyToRAM        EEPROM_OFFSET
#else
#define   flash_CopyToRAM        0x100000
#endif

#define   flash_UsedSize         0x010000


/*******************************************************************************
; Data Types
*******************************************************************************/
typedef struct flash_SaveDataInfo_S {
  LONG_T OrgPtr;
  LONG_T Size;
  LONG_T DstPtr;
  FLAG_T Run;
} FLASH_SAVEDATA_T;


/*******************************************************************************
; Public Variables
*******************************************************************************/
flash_EXTERN LONG_S  flash_PreviousTaskRate;

flash_EXTERN TIMER_T flash_Time_EraseAllDataChips; 
flash_EXTERN TIMER_T flash_Time_EraseDataChip; 
flash_EXTERN TIMER_T flash_Time_EraseDataFmemSect; 
flash_EXTERN TIMER_T flash_Time_EraseSectsInAllDataChips;
flash_EXTERN TIMER_T flash_Time_BlockErase;
flash_EXTERN TIMER_T flash_Time_ChipErase;


flash_EXTERN  FLAG_T    flash_DataModified;
flash_EXTERN  LONG_T    flash_WriteCycle;
flash_EXTERN  LONG_T    flash_ErrWriteCycle;

flash_EXTERN  FLAG_T    flash_BackupRequestFlag;
flash_EXTERN  LONG_T    flash_BackupWriteCycle;

flash_EXTERN  LONG_T    flash_ErrRestoreInterval;
flash_EXTERN  LONG_T    flash_ErrCheckCount;

flash_EXTERN  LONG_T    flash_BackupRestoreInterval;
flash_EXTERN  LONG_T    flash_BackupCheckCount;

flash_EXTERN  LONG_T    flash_ErrDataPoulingCnt,flash_ErrDataEraseToggleCnt,flash_ErrDataEraseTimeOutCnt;


/*******************************************************************************
; Public Functions
*******************************************************************************/
flash_EXTERN void flash_InitData(void);

flash_EXTERN FLAG_T flash_BuffWrite(LONG_T offset, BYTE_T *data, LONG_T bytes);
flash_EXTERN FLAG_T flash_BuffFill(LONG_T destAdrs, BYTE_T value, LONG_T length);
flash_EXTERN FLAG_T flash_BuffCopy(BYTE_T bank, BYTE_T sectId, LONG_T offset, LONG_T size);
flash_EXTERN FLAG_T flash_BlockErase(BYTE bank, BYTE_T sectId);
flash_EXTERN FLAG_T flash_ChipErase(BYTE bank);
flash_EXTERN FLAG_T flash_BlockProgram(BYTE_T bank, BYTE_T sectId, LONG_T offset, LONG_T bytes, FLAG_T autoErase);
flash_EXTERN void   flash_BuffRead(BYTE_T *storePtr, LONG_T flashAdrs, LONG_T size, BYTE_T bank);

flash_EXTERN FLAG_T flash_GetAddressPerOneDataFmem(LONG_T serialAdrs, LONG_T *oneChipAdrs, BYTE_T *bank);
flash_EXTERN FLAG_T flash_GetSerialAddressOfDataFmem(LONG_T *serialAdrs, LONG_T oneChipAdrs, BYTE_T bank);
flash_EXTERN FLAG_T flash_SelectDataFmemBank(BYTE_T fmemBank);
flash_EXTERN BYTE_T flash_GetCurrentDataFmemBank(void);
flash_EXTERN BYTE_T flash_GetCurrentDataFmemState(void);
flash_EXTERN FLAG_T flash_FinishAccessDataFmem(void);

flash_EXTERN FLAG_T flash_EraseDataChip(BYTE_T bank);
flash_EXTERN FLAG_T flash_EraseAllDataChips(void);
flash_EXTERN FLAG_T flash_EraseDataFmemSect(BYTE_T bank, BYTE_T sector);
flash_EXTERN FLAG_T flash_EraseSectsInAllDataChips(BYTE_T sector);
flash_EXTERN void   flash_SaveDataToFmem(BYTE_T bank, BYTE_T *dstPtr, BYTE_T *orgPtr, LONG_T size);
flash_EXTERN LONG_T flash_ReadDataFromFmem(BYTE_T bank, BYTE_T *dstPtr, BYTE_T *orgPtr, LONG_T size);

flash_EXTERN FLAG_T flash_RunToSaveData(BYTE_T saveId, LONG_T org, LONG_T size, LONG_T dst);

flash_EXTERN  void  flash_Initialize();
flash_EXTERN  void  flash_EraseE2pArea();
flash_EXTERN  void  flash_EraseErrArea();
flash_EXTERN  void  flash_dumpDataE2pArea(BYTE_T *, LONG_T);
flash_EXTERN  void  flash_dumpDataErrArea(BYTE_T *, LONG_T);
flash_EXTERN  void  flash_dumpDataServiceArea(void);
flash_EXTERN  void  flash_dumpDataMapArea(void);
flash_EXTERN  void  flash_Driver();
flash_EXTERN  void  flash_dumpBackupArea(BYTE_T *, LONG_T);


flash_EXTERN  BYTE_T  flash_ReadHallService(BYTE_T data,BYTE_T car,BYTE_T deck,BYTE_T pos,BYTE_T set);
flash_EXTERN  BYTE_T  flash_ReadCarService(BYTE_T data,BYTE_T car,BYTE_T deck,BYTE_T pos,BYTE_T set);
flash_EXTERN  BYTE_T  flash_ReadMapHallButton(BYTE_T data,BYTE_T hcc,BYTE_T button,BYTE_T pos);



/* honma 17Nov2003 
#ifdef GASIS

  #define    flash_UsedSize  2
  flash_EXTERN  FLAG_T   flash_DataModified;
  flash_EXTERN  LONG_T   flash_ErrCheckCount;
  flash_EXTERN  int      flash_CopyToRAM;
  flash_EXTERN  void     flash_Driver();

#else
*/

/*---------------------------------------------------------------*/
#endif
/*  end of flash.h */


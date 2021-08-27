/*******************************************************************************
;  File Name: CALENDER.H
;
;  File Descriptions:
;
;  Revision History:
;
;  SCN                   Author                        Date
;  ------------------------------------------------------------------------
;  
*******************************************************************************/
#ifndef calender_AlreadyIncludeHeader    /* protects from multiple includes */
#define calender_AlreadyIncludeHeader

#ifdef calender_ModuleDefinition
#define calender_EXTERN
#else
#define calender_EXTERN extern
#endif   // calender_ModuleDefinition

/*******************************************************************************
; Constants
*******************************************************************************/

/*******************************************************************************
; Data Types
*******************************************************************************/
typedef struct calender_Data_S {
  BYTE_T   Year;
  BYTE_T   Month;
  BYTE_T   Day;
  BYTE_T   DayOfWeek;
  BYTE_T   Hour;
  BYTE_T   Minute;
  BYTE_T   Second;
} calender_DataT;

/*******************************************************************************
; Public Variables
*******************************************************************************/

/******************************************************************************
; Public Functions
*******************************************************************************/
calender_EXTERN BYTE_T calender_CalcDayOfWeek(BYTE_T year, BYTE_T month, BYTE_T day);
calender_EXTERN calender_DataT *calender_GetData(void);
calender_EXTERN void   calender_SetDate(BYTE_T year, BYTE_T month, BYTE_T day, BYTE_T dayOfWeek);
calender_EXTERN void   calender_SetTime(BYTE_T hour, BYTE_T minute, BYTE_T second);
calender_EXTERN void   calender_Driver(void);
calender_EXTERN void   calender_Init(void);

#endif   // calender_AlreadyIncludeHeader

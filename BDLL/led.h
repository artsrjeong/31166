/*******************************************************************************
;  File Name: LED.H  for GCB-1-DX Rev.C1
;
;  File Descriptions:
;  - 
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
;  XXX-30461-XXX         Takayuki Yoshida              27 September 1999
*******************************************************************************/
#ifndef led_AlreadyIncludeHeader    /* protects from multiple includes */
#define led_AlreadyIncludeHeader

#ifdef led_ModuleDefinition
#define led_EXTERN
#else
#define led_EXTERN extern
#endif   // led_ModuleDefinition


/*******************************************************************************
; MACRO DEFINITIONS
*******************************************************************************/
/* ID list of 7 segment LED */
#define LED_7SEG1_ID         0
#define LED_7SEG2_ID         1
#define LED_7SEG3_ID         2
#define LED_7SEG4_ID         3
#define LED_MAX_7SEG_ID      4

/* list how to use 7 segment LED */
#define LED_7SEG1_NOTUSED    LED_7SEG1_ID
#define LED_7SEG_ACCESSFMEM  LED_7SEG2_ID
#define LED_7SEG_ONBAISTATUS LED_7SEG3_ID
#define LED_7SEG_PREDLEVEL   LED_7SEG4_ID

#define LED_7SEG_OFF         0xff
#define LED_7SEGDOT_ON       0xff
#define LED_7SEGDOT_OFF      0x00
#define LED_7SEGDOT_CURR     0x01

/* list of LED ID */
#define LED8_ID          0
#define LED9_ID          1
#define LED10_ID         2
#define LED11_ID         3
#define LED12_ID         4
#define LED13_ID         5
#define LED14_ID         6
#define LED15_ID         7

/* list how to use LEDs */
/* for AI
#define LED_HDWSYSTEM    LED8_ID
#define LED_RTCBATTERY   LED9_ID
#define LED_RTCCALENDER  LED10_ID
#define LED_POWERFAIL    LED11_ID
#define LED_JOBPARAM     LED12_ID
#define LED_GRPCOM       LED13_ID
#define LED14_NOTUSED    LED14_ID
#define LED15_NOTUSED    LED15_ID
*/
#define LED_HDWSYSTEM    LED8_ID
#define LED_RTCBATTERY   LED9_ID
#define LED_RTCCALENDER  LED10_ID
#define LED_ADSCOM       LED11_ID
#define LED_RING1COM     LED12_ID
#define LED_RING2COM     LED13_ID
#define LED_FUZZYON      LED14_ID
#define LED15_NOTUSED    LED15_ID


/* list of LED state */
#define LED_ON           0
#define LED_OFF          1
#define LED_BLINKING     2

/* honma 17Nov2003 
#ifdef GASIS
  led_EXTERN  BYTE_T  SLED1_REG;
  led_EXTERN  BYTE_T  SLED2_REG;
#else
*/

  #define  SLED1_REG    *(char *)0xa20000
  #define  SLED2_REG    *(char *)0xa28000
  #define  SLED3_REG    *(char *)0xa30000
  #define  SLED4_REG    *(char *)0xa38000


/*  7 segment bit definition  */
/*             D7             */
/*         ----------         */
/*     D2 |          | D6     */
/*        |    D1    |        */
/*         ----------         */
/*     D3 |          | D5     */
/*        |    D4    |        */
/*         ----------    D0   */
/*                       --   */
#define  LED7SEG_N0    0xC0   
#define  LED7SEG_N1    0xF9   
#define  LED7SEG_N2    0xA4   
#define  LED7SEG_N3    0xB0   
#define  LED7SEG_N4    0x99   
#define  LED7SEG_N5    0x92   
#define  LED7SEG_N6    0x82   
#define  LED7SEG_N7    0xF8   
#define  LED7SEG_N8    0x80   
#define  LED7SEG_N9    0x90   
#define  LED7SEG_DOT   0x7F   

#define  LED7SEG_101   3   
#define  LED7SEG_102   2   
#define  LED7SEG_103   1   
#define  LED7SEG_104   0   

#define  LED7SEG_CA    0x88    
//#define  LED7SEG_CB    0xc1    
#define  LED7SEG_CC    0xC6    
//#define  LED7SEG_CD    0x85    
#define  LED7SEG_CE    0x86    
#define  LED7SEG_CF    0x8E    
//#define  LED7SEG_CG    0x09    
#define  LED7SEG_C_HYPHEN    0xBF
#define  LED7SEG_C_BLANK     0xFF

/*******************************************************************************
; DATA CONSTANTS
*******************************************************************************/

/*******************************************************************************
; Variables
*******************************************************************************/
led_EXTERN FLAG_T led_Driver_Flag;

led_EXTERN      FLAG_T  led_toggle_flag;
led_EXTERN      LONG_T   led_toggle_count;

led_EXTERN  BYTE_T  seg_disp_data1;
led_EXTERN  BYTE_T  seg_disp_data2;
led_EXTERN  BYTE_T  seg_disp_data3;
led_EXTERN  BYTE_T  seg_disp_data4;

led_EXTERN  BYTE_T  seg_disp_table[42];
led_EXTERN  BYTE_T  led1Data,led2Data,led3Data,led4Data,ledData;

/*******************************************************************************
; Functions
*******************************************************************************/
led_EXTERN void   led_Init(void);
led_EXTERN void   led_Set7SegCharacter(BYTE_T segId, BYTE_T character, BYTE_T dotOnOff);
led_EXTERN FLAG_T led_SetLedState(BYTE_T led, BYTE_T state);

/* Task Handler */
led_EXTERN void   led_Driver(void);

/*******************************************************************************
; Inline Codes
*******************************************************************************/

#endif   // led_AlreadyIncludeHeader

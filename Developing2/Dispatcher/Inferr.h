/* U10 Project: @(#)inferr.h	1.3	3/23/95	14:22:47 */
/*********************************************************************
;  File Name:  INFERR.H - Public Data Definition for INFERR.C
;
;  File Description:
;  This file describes the INFERR.C public data.  This file is to be
;  included in any file that uses these services.
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
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Created (renamed) module from "inferr_p.h".
;
;  A1730203BAA-12        Mark Ross                     09 February 1995
;  - SCR 30203-124
;    Modified reassign_consider_reassn to log NO_ASSIGNED_CAR error rather
;    than BAD_CASE error when there is no car assigned to a hall call being
;    considered for reassignment.
;
;  Transamerica          Geoff Mochau                  28 February 1995
;  - Lower MAX_ERRORS to minimize RAM requirements.
;
;  JAB30203AAE           Hideyuki Honma                12 March    1999
;  - SCR 30203-xx:  Add Time Stamp for Error logs.
;
;********************************************************************/
#ifndef INFERR_INCLUDED  /* protects from multiple includes */
#define INFERR_INCLUDED

/*******************************************************************
;  Error Constants
********************************************************************/
//deefine		MAX_ERRORS            105   
#define		MAX_ERRORS            150   /* 105 types of errors (max.)   */
#define		ERROR_STR_CHK         100   /* 100-104 error strage check */
//#define		MAX_ERROR_CNTS	    65000 
#define		MAX_ERROR_CNTS	    99999 /* max # of error occurances  */

#define		FRAMING_ERROR		0    /* framing error		    */
#define		CHECKSUM_ERROR   	1    /* checksum error		    */
#define		RING1_RX_ERROR	 	2    /* ring 1 receive error	    */
#define		RING1_TX_ERROR	 	3    /* ring 1 transmit error	    */
#define		RING2_RX_ERROR	 	4    /* ring 2 receive error	    */
#define		RING2_TX_ERROR	 	5    /* ring 2 transmit error	    */
#define		SVT_RX_ERROR	 	6    /* service tool receive error  */
#define		SVT_TX_ERROR	 	7    /* service tool transmit error */
#define		ADSS_RX_ERROR	 	8    /* adss/pu receive error	    */
#define		ADSS_TX_ERROR	 	9    /* adss/pu transmit error	    */
#define		BAD_CASE	 	10   /* out of range case 	    */
//#define         Q_READ                  11     queue read failed           
#define         Q_WRITE_RING            11   // queue write failed - RING 
#define         Q_WRITE_ADS             12   // queue write failed - ADS
#define		TSK_ENABLE_FAIL  	13   /* task enable failed          */
#define		TSK_DISABLE_FAIL 	14   /* task disable fail   	    */
#define         ICA_REASSN_CNT   	15   /* count of ICA reassignments  */
#define         ICA_PACMAN_CNT   	16   /* count of coincident calls   */
#define         RRT_GENERATE_SET_FAIL   17   /* gen RRT fuzzy set error     */
#define         PWT_GENERATE_SET_FAIL   18   /* gen PWT fuzzy set error     */
#define         GENERATE_COORDINATE_SET_ERROR   19   /* gen coordinate err  */
#define         ICA_DISABLE_ERROR       20   /* disable ICA error           */
#define         INTERVALS_OUT_OF_RANGE  21   /* disable ICA error           */
#define         NO_ASSIGNED_CAR         22   /* No car assinged error       */
#define         CALLMASK_FAIL           23   /* new call's mask error       */
//kagaya
#define         PassingReassign1        24   
#define         PassingReassign2        25
#define         PassingReassign3        26
#define         PassingReassign4        27
#define         PassingReassign5        28
#define         PassingReassign6        29
#define         PassingReassign7        30
#define         PassingReassign8        31
#define         PassingReassign9        32
#define         PassingReassign10       33
#define         PassingReassign11       34
#define         PassingReassign12       35
#define         PassingReassign13       36
#define         PassingReassign14       37
#define         PassingReassign15       38
#define         PassingReassign16       39
#define         PassingReassign17       40
#define         PassingReassign18       41
#define         IcaPacmanReassign       42
#define         ExecDuplicate           43
#define         NoAssignedLowZone       44
#define         NoAssignedHighZone      45
#define         DEBUG_TX_ERROR          46 
#define         DupOneCallNew           47
#define         NoLocalAssign           48
#define         NoLocalAssign2          49
#define         NoDupLocalAssign        50
#define         NoDupLocalAssign2       51
#define         TopPosError             52
#define         DupGroupNoError         53
#define         IdlError                54
#define         E2pDefaultValueCount    55  /* E2P Default Value Count */
#define         E2pDefaultValueCount2   56  /* E2P Default Value Count2 */

//DES testing ***temp***
#define         DES_RxInvalidChksum     57
#define         DES_RxByteTimeout       58
#define         DES_RxNoStart2          59
#define         DES_RxInvalidMsgLen     60
#define         DES_RxMsgTimeout        61
#define         DES_RxTooManyMsgs		62
#define         DES_RxInvalidState		63
#define         DES_TxInvalidLength     64
#define         DES_RxMultiAck          65
#define         DES_RxBytesThrownOut    66
#define         DES_RxUnknownClass       67
#define         DES_RxValidMsg          68
#define         DES_RxMultiNak          69
#define         DES_CallEnter           70
#define         DES_AckResponse          71
#define         DES_CallEntByt2          72
#define         DES_CallEnterInvId      73
#define         DES_TxAssignCall      74
#define         DES_TxQuePurge1      75
#define         DES_TxQuePurge2      76
#define         DES_PowerUpTouch       77
#define         DES_PowerUpKey         78
#define         DES_PowerUpInvId       79
#define         DES_PowerUpBytErr      80
#define         DES_DecOnline          81
#define         DES_DecOnlineInvalid   82
#define         DES_DecOnlineBytErr    83
#define         DES_DesOnline          84
#define         DES_DesOnlineInvalid   85
#define         DES_DesOnlineBytErr    86
#define         DES_TxDisabled         87
#define         DES_TxRetransmit       88
#define         StartSynchCmd          89
#define         PowerUpWaitTimeout     90
#define         DES_InvManagerState    91
#define         SynchSuccess           92
#define         desOnlineTimeout       93
#define         sendDedOnline          94
#define         sendModeSelect         95

// end of DES testing

#define  HDW_BUSERROR                               100     /* Bus Error Vec-2 */
#define  HDW_ADDRESSERROR                           101     /* Address Error Vec-3 */
#define  HDW_ILLEGALINST                            102     /* Illegal Instruction Vec-4 */
#define  HDW_DIVBYZERO                              103     /* Integer Divide by Zero Vec-5 */
#define  HDW_CHKINST                                104     /* CHK,CHK2 Instruction Vec-6 */
#define  HDW_COPROPROTOCOL                          105     /* Coprocessor Protocol Violation Vec-13 */
#define  HDW_FPBSUC                                 106     /* FP Branch or Set on Unordered Condition V48*/
#define  HDW_FPINEXRESULT                           107     /* FP Inexact Result V49*/
#define  HDW_FPDIVBYZERO                            108     /* FP Divide by Zero V50 */
#define  HDW_FPUNDERFLOW                            109     /* FP Underflow V51 */
#define  HDW_FPOPEERROR                             110    /* FP Operand Error V52 */
#define  HDW_FPOVERFLOW                             111    /* FP Overflow V53 */
#define  HDW_FPSIGNAN                               112    /* FP Signaling NAN V54 */
#define  RTC_BATTERY_LOW                            113     /* AI System Exception 2 */
#define  SIO1A_PARITY_ERR                           114
#define  SIO1B_PARITY_ERR                           115
#define  SIO2A_PARITY_ERR                           116
#define  SIO2B_PARITY_ERR                           117
#define  SIO3A_PARITY_ERR                           118
#define  SIO3B_PARITY_ERR                           119
#define  SIO1A_RXOVERRUN                            120
#define  SIO1B_RXOVERRUN                            121
#define  SIO2A_RXOVERRUN                            122
#define  SIO2B_RXOVERRUN                            123
#define  SIO3A_RXOVERRUN                            124
#define  SIO3B_RXOVERRUN                            125
#define  SIO1A_FRAMING_ERR                          126
#define  SIO1B_FRAMING_ERR                          127
#define  SIO2A_FRAMING_ERR                          128
#define  SIO2B_FRAMING_ERR                          129
#define  SIO3A_FRAMING_ERR                          130
#define  SIO3B_FRAMING_ERR                          131
#define  FLASHDRV_ERASECHIPCODE_ERR                 132    /* Code Chip Erase Error*/
#define  FLASHDRV_ERASESECTCODE_ERR                 133    /* Code Sect Erase Error*/
#define  FLASHDRV_WRITECODE_ERR                     134    /* Code Write Error*/
#define  FLASHDRV_ERASECHIPDATA_ERR                 135    /* Data Chip Erase Error*/
#define  FLASHDRV_ERASESECTDATA_ERR                 136    /* Data Sect Erase Error*/
#define  FLASHDRV_WRITEDATA_ERR                     137    /* Data Write Error*/
#define  WATCHDOG_RESET                             138
#define  FLASH_BANK_ERR                             139
#define  FLASH_SECTOR_ERR                           140
#define  FLASH_BUFWRITEERR                          141
#define  FLASH_BUFFILLERR                           142
#define  FLASH_BUFCOPYERR                           143
#define  FLASH_BLOCKPROGRAMERR                      144
#define  FLASH_BLOCKERASEERR                        145

/*********************************************************************
; ERROR Structure
; This structure is used in the ERROR_S type definition.
;
; This type definition is used by the client module to create an
; instance of an ERROR. The error structure is just an arrary of
; integers used to log the number of occurances of a particular error.
;********************************************************************/
struct ERROR_SP {
//update 99/3/12
        INT_T  Count;
        BYTE_T Year[10];
        BYTE_T Month[10];
        BYTE_T Day[10];
        BYTE_T Hour[10];
        BYTE_T Minute[10];
        BYTE_T Second[10];
};
typedef struct ERROR_SP ERROR_ST;

struct ERROR_S2
{
  ERROR_ST  type[MAX_ERRORS];
};

typedef struct ERROR_S2 ERROR_T2;

#ifdef CAGS
EXTERN ERROR_T2 error_info;
#else
#define    ERROR_INFO_PTR   0x60000
EXTERN ERROR_T2 error_info;
#endif

EXTERN LONG_T error_Size;
EXTERN FLAG_T  error_InfoModified;


#define  EOS             sizeof (ERROR_ST)
#define  ErrTable(ID)    (void *)(ERROR_INFO_PTR+(ID*EOS))

/*********************************************************************
;  Function Prototypes
;	1. E_init - initialize error structure
;	2. E_log - log the occurance of a particular type of error
;   3. E_cnt - count the occurance of a particular error_num (no time stamp)
;********************************************************************/
void E_init (void);
void E_log  (BYTE_T error_num);
void E_cnt  (BYTE_T error_num);
void inferr_Clear(void);

#endif  /* end of include file definition */

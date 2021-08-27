/**************************************************************************
;  File Name:  INFDES.H - Public Data Definitions for INFDES.C
;
;  File Description:
;  This file describes the INFDES.C public data.  This file is to be
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
;  Unpublished work - Copyright 2004 Otis Elevator Company
;  All rights reserved.
;
;
;  Revision History:
;
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  A1231017AAA          S. Dridi                  25 Mar 2004
;  - SCR 31017-23: Created (renamed from  place holder module  infrng.h)
;  A1231017AAA          S. Dridi                  6 Apr 2004
;  - SCR 31017-16: Enable / Disable COP based on hot/cold floors
;  A1231017AAA          S. Dridi                  21 Apr 2004
;  - SCR 31017-14: Walk rate from DEC to Car
;
;  A1231017AAA          S. Dridi                  4 May 2004
;  - SCR 31017-15: Handicap support
;
;  A1231017AAA          S. Dridi                  12 May 2004
;  - SCR 31017-14: Extend Dwell Time if passenger walk time hasnt expired
;
;  A1231017AAB           S Dridi                       2 March    2005
;  - SCR 186:  translation of Building Floors to Group Floors (negative floors).
;********************************************************************/
#ifndef INFDES_INCLUDED  /* protects from multiple includes */
#define INFDES_INCLUDED

#include "timer.h"  /* timer type definition */

/**************************************************************************
;  Public Constants
;*************************************************************************/
#define DEC_TYPE_TOUCHSCRN    1
#define DEC_TYPE_KEYPAD       2
#define DEC_TYPE_UNKNOWN      255

#define MAX_DECS_PER_FLOOR    8

#define MAX_FLOOR_SEG_CCSERVICE  4

#define DBD_COP_INIT  0
#define DBD_COP_HOT   1
#define DBD_COP_COLD  2

#define COLD_FLOOR    0

#define COP_FORCE_MSG_TRUE     255
#define COP_FORCE_MSG_FALSE    0
#define COP_FORCE_MSG_TEST     254

#define DBD_INVALID_FLOOR    255
#define BLD_INVALID_FLOOR    128

#define MAX_NUM_SECTORS     ((BYTE_T) 7)
#define NUM_MIT_CAR_SAMPLES ((BYTE_T) 5)
#define SECTOR_LATCH_TIME   ((BYTE_T) 3)
#define MAX_SECTOR_STEP     ((BYTE_T) 1)

#define SECTOR_UNDEFINED    ((BYTE_T) 0xFF)
#define SECTOR_INITIALIZED  ((BYTE_T) 0x01)
#define SECTOR_ASSIGNED     ((BYTE_T) 0x02)

#define MAX_HC_TRAVEL_RATE   15       // max. time for handicap travel rate req by code    1.5 feet per second
#define MIN_STD_WALK_RATE    40       // min time is zero set to 4 feet per second

#define MIN_CAR_CALL_DWELL   2        // ocss door open dwell based on std car call
#define MIN_HALL_CALL_DWELL  4        // ocss door open dwell based on hall call

#define MIN_HDCP_HALL_DWELL  30
#define MAX_HDCP_HALL_DWELL  48


static const unsigned short ausCRC16Table[ 256 ] = 
{
    0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011, 
    0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022, 
    0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072, 
    0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041, 
    0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2, 
    0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1, 
    0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1, 
    0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082, 
    0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192, 
    0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1, 
    0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1, 
    0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2, 
    0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151, 
    0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162, 
    0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132, 
    0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101, 
    0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312, 
    0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321, 
    0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371, 
    0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342, 
    0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1, 
    0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2, 
    0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2, 
    0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381, 
    0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291, 
    0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2, 
    0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2, 
    0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1, 
    0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252, 
    0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261, 
    0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231, 
    0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
};


WORD CRC16(BYTE * lpBuff, DWORD ulLength, WORD  usCRC);
/**************************************************************************
;  Public structures
;*************************************************************************/

struct DEC_STATUS_S {
     BOOL    online;                    // dec is online or offline
     BYTE_T  deviceType;		        // dec device type
};
typedef struct DEC_STATUS_S DEC_STATUS_T;


struct DEC_STATUS_BITS_S {
     BYTE_T  online;                    // dec is online
     BYTE_T  keypad;		            // dec is a keypad
     BYTE_T  touchscreen;	            // dec is a touchscreen
};
typedef struct DEC_STATUS_BITS_S DEC_STATUS_BITS_T;


struct DEC_IDS_S {
	BYTE_T    decId;                                                //device address
	BYTE_T    door;                                                 // door = 0 is, 1 is rear
	BYTE_T    distance[E2P_MAX_CARS];
	BYTE_T    stdWalkTime[E2P_MAX_CARS];                        //std call walk time from this dec to cars A...H
	BYTE_T    hdcpTravelTime[E2P_MAX_CARS];                     //hc call walk time from this dec to cars A...H
};
typedef struct DEC_IDS_S DEC_IDS_T;


struct FLOOR_DEC_S {
	 BYTE_T       decCount;                                         //number of decs on the floor, 0 = HB, 1..8 decs
	 DEC_IDS_T    decInfo[MAX_DECS_PER_FLOOR];                    //struct of device add, std car walk, hc car walk
};
typedef struct FLOOR_DEC_S FLOOR_DEC_T;


struct DBD_WALK_TIMER_S {
     BYTE_T       fuhc_carWalkTimer [E2P_MAX_CARS];             // fuhc+efuhc car walk timer 0 = no walk timer being done
     BYTE_T       fdhc_carWalkTimer [E2P_MAX_CARS];             // fdhc+efdhc car walk timer 0 = no walk timer being done
     BYTE_T       ruhc_carWalkTimer [E2P_MAX_CARS];             // ruhc+eruhc car walk timer 0 = no walk timer being done
     BYTE_T       rdhc_carWalkTimer [E2P_MAX_CARS];             // rdhc_erdhc car walk timer 0 = no walk timer being done
};
typedef struct DBD_WALK_TIMER_S DBD_WALK_TIMER_T;


struct DES_BuildingToFloor_S{
  BYTE_T FloorPos;
  };
typedef struct DES_BuildingToFloor_S DES_BuildingToFloor_T;


struct DBD_SECTOR_S  {
    BYTE_T   bottom;
    BYTE_T   top;
    BYTE_T   state;
};
typedef struct DBD_SECTOR_S DBD_SECTOR_T;


/**************************************************************************
;  Public data
;*************************************************************************/
//DES placeholder  start
EXTERN BYTE_T dbd_dummyByte1;        //to use for debugging with service tool
EXTERN BYTE_T dbd_dummyByte2;

//Dave Hughes testing
EXTERN BYTE_T des_PotCC_enablePublish;
EXTERN BYTE_T des_PotCC_carId;
EXTERN BYTE_T des_PotCC_door;
EXTERN BYTE_T des_PotCC_segment;
EXTERN BYTE_T des_PotCC_type;
EXTERN BYTE_T des_PotCC_lowMask;
EXTERN BYTE_T des_PotCC_hiMask;
EXTERN WORD_T des_PotCC_mask;

//DES : public definitions
EXTERN BYTE_T des_rx_fcc_allowed_mask[MAX_FLOOR_SEG_CCSERVICE];
EXTERN BYTE_T des_rx_rcc_allowed_mask[MAX_FLOOR_SEG_CCSERVICE];
EXTERN BYTE_T des_rxAllDataMask;

EXTERN BYTE_T des_cop_enablePublish;
EXTERN BYTE_T des_cop_enableFlag;
EXTERN BYTE_T des_cop_carId;

EXTERN BYTE_T des_dwell_enablePublish;
EXTERN BYTE_T des_dwell_carId;
EXTERN BYTE_T des_dwell_callMask;
EXTERN BYTE_T des_dwell_position;
EXTERN BYTE_T des_dwell_doorTime;

EXTERN DBD_SECTOR_T sectorDefinition[MAX_NUM_SECTORS];

EXTERN BYTE_T assignedToSectorMask[64];
EXTERN BYTE_T sectorCarMask;

EXTERN BYTE_T DBD_sectorCount;
EXTERN LONG_T DBD_sectorNumChangeCount;
EXTERN LONG_T DBD_sectorAssignedCount;
EXTERN LONG_T DBD_sectorCarRrtFailsCount;

EXTERN LONG_T DES_BldFloorInvalidCnt;

EXTERN BYTE_T mitCarCount[NUM_MIT_CAR_SAMPLES];
EXTERN BYTE_T mitCarCountAvg;
EXTERN BYTE_T sectorLatchTmr;
EXTERN BYTE_T sectorCount;

//Redundancy
EXTERN BYTE_T DES_MyGroupId;
EXTERN BYTE_T DES_OtherGroupId;
EXTERN FLAG_T DES_OtherGroupAlive;
EXTERN FLAG_T DES_PrimaryGcbFlag;
EXTERN FLAG_T DES_SecondaryGcbFlag;
EXTERN BYTE_T DES_MyGcbStatus;
EXTERN BYTE_T DES_OtherGcbStatus;
EXTERN FLAG_T DES_ColdFloorDispFlag;
EXTERN TIMER_T  DES_OtherGcbArbMsgTimeout;
EXTERN TIMER_T  DES_OtherGcbChkSumMsgTimeout;
EXTERN TIMER_T  DES_noDecOnlineTimer;
EXTERN TIMER_T  DES_SendDecInfoTimer;
EXTERN TIMER_T  DES_NoPrimaryTimer;


EXTERN BYTE_T arbState;         // todo : move to .lh after testing
EXTERN BYTE_T arbStateTimer;    // todo : move to .lh after testing
EXTERN BYTE_T arbStateLatchUnavailTimer; // todo : move to .lh after testing
EXTERN BYTE_T paramReqTimer;    // todo : move to .lh after testing
EXTERN BYTE_T otherGcbArbState; // todo : move to .lh after testing
EXTERN FLAG_T requestPrimaryGcbFlag;    // todo : move to .lh after testing
EXTERN FLAG_T requestSecondaryGcbFlag;  // todo : move to .lh after testing
EXTERN WORD_T DES_CheckSum[8];
EXTERN WORD_T DES_OtherGcbChkSum[8];
EXTERN FLAG_T DES_CheckSumFlag;
EXTERN BOOL   DES_DecStatusChange;
EXTERN FLAG_T DES_SendDecInfoBurstFlag;
EXTERN BYTE_T DES_DecInfoIndex;
EXTERN LONG_T DES_CHECKSUM_ERR[8];
EXTERN BOOL   DES_CheckSumReadyToPublish;
EXTERN BOOL   DES_CalcChecksum;

/**************************************************************************
;  Public Functions
;*************************************************************************/
void Contract_Data_Checksum(BYTE_T Section);
void DES_noDecOnline(void);
void DBD_assignCarToSector(BYTE_T selectedCar, BYTE_T dest);

void DBD_CalculateCopEnable(BYTE_T car, BYTE_T landing, WORD_T callmask, BYTE_T preDir, BYTE_T oldMovingDir);
void DBD_CalculateDwellFromWalkTimes(WORD_T callmask, BYTE_T landing, BYTE_T car, BYTE_T doorState);

BYTE_T DBD_checkForSectorCar(BYTE_T dest, BYTE_T selectedCar, INT_T *assignIndex, INT_T *rrt, INT_T *pure_rrt);
void DBD_CheckSectorCount(BYTE_T mitCarsNeeded);
void DBD_checkSectorDwellTime(BYTE_T carId);
void DBD_ClearCarWalkTimer(BYTE_T floorPos, BYTE_T carId, BYTE_T callMask);

WORD_T DBD_GetCarDirection(BYTE_T dirIndex);
BYTE_T DBD_GetCarWalkTime(BYTE_T callType, BYTE_T floorPos, BYTE_T decId, BYTE_T carId);
BYTE_T DBD_GetCarWalkTimer(BYTE_T floorPos, BYTE_T carId, WORD_T callMask);
BYTE_T DBD_GetClosestKeypad(BYTE_T landing, BYTE_T carId);
BOOL DBD_GetDecStatus(DEC_STATUS_T *decStatusPtr, BYTE_T decNum);
BOOL DBD_GetDesActive(void);
BOOL DBD_GetDesOnline(void);
BYTE_T DBD_GetFloorDecCount (BYTE_T floorPosition);
BYTE_T DBD_GetFloorFromBuilding (BYTE_T buildingPosition);

BYTE_T DBD_pickBestCar(BYTE_T carMask, INT_T *assignIndex, INT_T *rrt, INT_T *pure_rrt);
void DBD_removeCarFromSectorAssignments(BYTE_T selectedCar);

void DBD_SendCarSector(BYTE_T car,BYTE_T sec,FLAG_T assign);
void DBD_SendSectorInfo(BYTE_T sectorCount);
void DBD_SetCarWalkTimer(BYTE_T carId, BYTE_T callMask, BYTE_T callPos, BYTE_T callType, BYTE_T decId);

void DBD_UpdateSectorDefinition(BYTE_T newSectorCount);
void DBD_UpdateWalkTimers(void);

void DES_BuildingToFloorInit(void);
void DES_CommunicationHandler(void);
void DES_FloorDecIdsInit(void);
void DES_Init(void);
void DES_MessageDistributer(BYTE_T msgClass, BYTE_T dataLen, BYTE_T *msgData);
void DES_ManagerTsk(void);
void DES_PriorityMessageHandler(void);

void DES_psgrAssigned(WORD_T psgrId, BYTE_T carId);
void DES_psgrNotAssigned(WORD_T psgrId, BYTE_T reasonCode);
void DES_publishExpectedCarCalls(BYTE_T carId, BYTE_T doorType, BYTE_T callType, BYTE_T segment, WORD_T landingMask);

BOOL DES_ReenterAnnouncement(WORD_T deviceAddress, BYTE_T messageId, BYTE_T param1, BYTE_T param2, BYTE_T param3);
BOOL DES_SendMsg(BYTE_T msgClass, BYTE_T dataLen, BYTE_T *msgData);
BOOL DES_SendCarAssignment(WORD_T deviceAddress, BYTE_T carId, WORD_T direction, WORD_T sequenceNumber);
BOOL DES_SendSynchronize(void);
BOOL DES_SendDedOnline(void);
BOOL DES_SendDedXon(void);
BOOL DES_SendDedXoff(void);
BOOL DES_SendDecModeSelect(WORD_T deviceAddress, BYTE_T mode);
void DES_StandardMessageHandler(void);

void DES_TestPort(void);
void DES_TransmitDriver(void);
void DES_TransmitRing(void);

BYTE_T getCopLastFloorType(BYTE_T carId);
BOOL getCopEnableCmd(BYTE_T carId);

void initializeDecOnlineStatus(void);

void publishCopEnableMsg(void);
void publishPsgrRequestAssignment(WORD_T psgrId, BYTE_T carId, BYTE_T destPos);
void publishPsgrRequestInitialize(WORD_T psgrId, BYTE_T srcPos, BYTE_T srcDoorType, BYTE_T destDoorType, BYTE_T callSet, BYTE_T callDir);
void publishPsgrRequestStatus(WORD_T psgrId, BYTE_T psgrStatus, BYTE_T reasonCode);
void publishSetDoorTime(BYTE_T carId, BYTE_T callMask, BYTE_T position, BYTE_T doorTime);
void publishParamUpdReq(BYTE_T carId, BYTE_T paramId);
void publishGcbArbitrationStatus(BYTE_T param1, BYTE_T param2, BYTE_T param3);
void DES_processOtherGroupArbMsg(BYTE_T param1, BYTE_T param2, BYTE_T param3);
void DBD_RecoverForSecondary(void);

void setCopEnableCmd(BYTE_T carId, BOOL enableCmd, BYTE_T forceMsg);
void setCopLastFloorType(BYTE_T carId, BYTE_T floorType);

void updateDecOnlineStatus(BYTE_T *statusBits);
void updateDecTypeStatus(BYTE_T deviceType, BYTE_T *statusBits);
void publishCheckSumMsg(BYTE_T Section, BYTE_T Report_or_Request);
void DES_SendDecInfo(BYTE_T index, BYTE_T door_type);
BYTE_T DES_FindServiceableLanding(BYTE_T start_landing);

#endif  /* end of include file definition */

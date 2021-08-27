#define DEC_TYPE_TOUCHSCRN    1
#define DEC_TYPE_KEYPAD       2
#define DEC_TYPE_UNKNOWN      255

#define MAX_DECS_PER_FLOOR    8

#define MAX_FLOOR_SEG_CCSERVICE  4

#define DBD_COP_INIT  0
#define DBD_COP_HOT   1
#define DBD_COP_COLD  2

#define COLD_FLOOR    0

#define COP_FORCE_MSG  TRUE

#define DBD_INVALID_FLOOR    255

#define MAX_NUM_SECTORS     ((BYTE_T) 7)
#define NUM_MIT_CAR_SAMPLES ((BYTE_T) 5)
#define SECTOR_LATCH_TIME   ((BYTE_T) 3)
#define MAX_SECTOR_STEP     ((BYTE_T) 1)

#define SECTOR_UNDEFINED    ((BYTE_T) 0xFF)
#define SECTOR_INITIALIZED  ((BYTE_T) 0x01)
#define SECTOR_ASSIGNED     ((BYTE_T) 0x02)


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

EXTERN BYTE_T mitCarCount[NUM_MIT_CAR_SAMPLES];
EXTERN BYTE_T mitCarCountAvg;
EXTERN BYTE_T sectorLatchTmr;
EXTERN BYTE_T sectorCount;

/**************************************************************************
;  Public Functions
;*************************************************************************/

void DBD_assignCarToSector(BYTE_T selectedCar, BYTE_T dest);

BYTE_T DBD_checkForSectorCar(BYTE_T dest, BYTE_T selectedCar, INT_T *assignIndex, INT_T *rrt, INT_T *pure_rrt);
void DBD_CheckSectorCount(BYTE_T mitCarsNeeded);
void DBD_checkSectorDwellTime(BYTE_T carId);
void DBD_ClearCarWalkTimer(BYTE_T floorPos, BYTE_T carId, BYTE_T callMask);

WORD_T DBD_GetCarDirection(BYTE_T dirIndex);
BYTE_T DBD_GetCarWalkTime(BYTE_T callType, BYTE_T floorPos, BYTE_T decId, BYTE_T carId);
BYTE_T DBD_GetCarWalkTimer(BYTE_T floorPos, BYTE_T carId, WORD_T callMask);
BOOL DBD_GetDecStatus(DEC_STATUS_T *decStatusPtr, BYTE_T decNum);
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
void DES_Manager(void);
void DES_PriorityMessageHandler(void);

void DES_psgrAssigned(WORD_T psgrId, BYTE_T carId);
void DES_psgrNotAssigned(WORD_T psgrId, BYTE_T reasonCode);
void DES_publishExpectedCarCalls(BYTE_T carId, BYTE_T doorType, BYTE_T callType, BYTE_T segment, WORD_T landingMask);

BOOL DES_ReenterAnnouncement(WORD_T deviceAddress, BYTE_T messageId, BYTE_T param1, BYTE_T param2, BYTE_T param3);
BOOL DES_SendMsg(BYTE_T msgClass, BYTE_T dataLen, BYTE_T *msgData);
BOOL DES_SendCarAssignment(WORD_T deviceAddress, BYTE_T carId, WORD_T direction, WORD_T sequenceNumber);
BOOL DES_SendSynchronize(void);
BOOL DES_SendDedOnline(void);
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

void setCopEnableCmd(BYTE_T carId, BOOL enableCmd, BOOL forceMsg);
void setCopLastFloorType(BYTE_T carId, BYTE_T floorType);

void updateDecOnlineStatus(BYTE_T *statusBits);
void updateDecTypeStatus(BYTE_T deviceType, BYTE_T *statusBits);

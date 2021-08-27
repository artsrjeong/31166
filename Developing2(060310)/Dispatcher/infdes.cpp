#include "stdafx.h"
#include "common.h"

#define EXTERN extern
#ifndef CAGS
#include "infbld.h"
#include "infocs.h"
#include "sio.h"
#include "inferr.h"
#endif //ifndef CAGS
#include "e2p.h"
#include "infmsg.h"
#include "density.h"
#include "rrt.h"
#include "pwt.h"
#include "asslib.h"
#include "crisprrt.h"
#include "crowd.h"
#include "infads.h"
#include "assign.h"
#include "park.h"
#include "mit.h"
#include "calender.h"
#undef EXTERN

#define EXTERN 
#include "infdes.h"
#undef EXTERN 

FLOOR_DEC_T  floorDecInfo[E2P_MAX_LANDINGS];

/**********************************************************************************************************
;  Name: DBD_GetFloorDecCount - return the count of DECs at a floor.
;
;  Description:
;
;   : Return : 0 for no DECs, cool floor, or 1..8 DECs on that floor.
;***********************************************************************************************************/
BYTE_T DBD_GetFloorDecCount(BYTE_T floorPos)
{
BYTE_T deviceCount = 0;

  if ((floorPos < E2P_MAX_LANDINGS) && (floorPos >= 0))
  {
    deviceCount = floorDecInfo[floorPos].decCount;
  }
  return(deviceCount);
} //DBD_GetFloorDecCount

/**************************************************************************
;  Name: DES_Init - Initialize DES data
;
;  Description:
;  Performs these functions:
;    o initialize receive and transmit message queues.
;
;*************************************************************************/
void DES_Init(void)
{
	BYTE_T i, segment, floorId, carIdx, stateIdx;
	WORD_T deviceId;

	// Initialize the receive que for call entered messages.

	DES_FloorDecIdsInit();      //initialize floorDecIds structure from dec configuration data

} /* DES_Init */

/**************************************************************************
;  Name: DES_FloorDecInit - Initialize FloorDecInfo data
;
;
;*************************************************************************/
void DES_FloorDecIdsInit(void)
{
	BYTE_T floorId, deviceCount, carIdx, rateStd, rateHc, walkTime, decIdx;
	WORD_T decRecordIdx, distance, remain;
	static DECDeviceRecordT *DECInfo;

	// initialize FloorDecTable

	for ( floorId = 0; (floorId < E2P_MAX_LANDINGS); floorId++) {
		deviceCount=1; //jsr
		floorDecInfo[floorId].decCount = deviceCount;

		// if no decs on floor zap walk times to max for up to 8 decs per floor, 8 cars per dec COLD FLOOR
		if ( deviceCount == 0) {
			for ( decIdx = 0; (decIdx < MAX_DECS_PER_FLOOR); decIdx++) {
				// default door to front, std time to immediate, hdcp to hdcp rate
				floorDecInfo[floorId].decInfo[deviceCount].door  = FRONT_DOOR;
				floorDecInfo[floorId].decInfo[deviceCount].decId = 0;
				for ( carIdx = 0; (carIdx < E2P_MAX_CARS); carIdx++ ) {
					floorDecInfo[floorId].decInfo[deviceCount].distance[carIdx] = 0;
					floorDecInfo[floorId].decInfo[decIdx].stdWalkTime[carIdx] = 0;
					floorDecInfo[floorId].decInfo[decIdx].hdcpTravelTime[carIdx] = 0;
				}
			}
		}
	}      //for ( floorId = 0; (floorId < E2P_MAX_LANDINGS); floorId++)
} //* end DES_FloorDecIdsInit();

void DES_psgrAssigned(WORD_T psgrId, BYTE_T carId)
{
	WORD_T deviceAddress, sequenceNumber;
}
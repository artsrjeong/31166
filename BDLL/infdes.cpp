#include "stdafx.h"
#include "common.h"

#define EXTERN extern
#include "infocs.h"
#include "infbld.h"
#include "infrng.h"
#include "infmsg.h"
#include "sio.h"
#undef EXTERN

#define EXTERN
#include "infdes.h"
#undef EXTERN


/**********************************************************************************************************
;  Name: DBD_removeCarFromSectorAssignments -  
;                                
;
;  Description:
;    o  
; 
;   
;***********************************************************************************************************/
void DBD_removeCarFromSectorAssignments(BYTE_T selectedCar)
{
BYTE_T i, bottomPos, topPos;
BYTE_T carBit;

  carBit = 1 << (selectedCar-1);
  
  if ( (sectorCarMask & carBit) != 0)
  {
    bottomPos = bld_info.ocss_lobby_pos + 1;
    topPos   = bld_info.top_pos;
  
    for (i=bottomPos; i <= topPos; i++)
    {
       assignedToSectorMask[i] &= ~carBit;    
    } 
    sectorCarMask &= ~carBit;

   // publish car has been deassigned from sector.
   DBD_SendCarSector(selectedCar, 0, FALSE);

    //TBD : ?does sectorDefinition.state need to have an unassigned state??
  }

} // DBD_removeCarFromSectorAssignments

// Temp for debug publishes...
BYTE_T desSectorAssigned[9];
/*-----------------------------------------------------------------%BSH%----
|  Name: DBD_SendCarSector();  
|  Description:
|
|------------------------------------------------------------------%ESH%---*/
void DBD_SendCarSector(BYTE_T car,BYTE_T sec,FLAG_T assign)
{
  BYTE_T msg_buffer[6];
  BYTE_T funcCode, bottom, top;
  BYTE_T assignedSector;  // temp for debug publishes
   
  if (assign == TRUE) 
  {
     funcCode = 3 << 4;       // assign : function Code == 2 ==> ?change code for risa display?
     bottom = sectorDefinition[sec].bottom;
     top = sectorDefinition[sec].top;
     desSectorAssigned[car] = sec;  // temp for debug publishes
     assignedSector = sec;          // temp for debug publishes
  }
  else               
  {
     funcCode = 4 << 4;        // deassign : function Code == 3 ==> ?change code for risa display?
     bottom = INVALID_POSITION;
     top    = INVALID_POSITION;
     assignedSector =  desSectorAssigned[car];  // temp for debug publishes
  }

      msg_buffer[0] = 0xec;                                   /* byte 1 - type 6, from origin 12 = GCSS  */
      msg_buffer[1] = 0xc0 | car;                             /* byte 2 - subtype 4,destination          */
      msg_buffer[2] = 0x80 | funcCode | assignedSector;       /* byte 3 - FuncCode , SectorID            */
      msg_buffer[3] = 0x80 | top;                             /* byte 4 - top                            */
      msg_buffer[4] = 0x80 | bottom;                          /* byte 5 - bottom                         */
      msg_buffer[5]   = M_checksum(msg_buffer,MSG_LEN);
   
      sio_TxRing(msg_buffer, MSG_LEN, RING1);
}//DBD_SendCarSector


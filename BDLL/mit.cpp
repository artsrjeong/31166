/*---------------------------------------------------------------%BSH%-----
|  File Name: MIT.C - Tasks and procedures for standard MIT operation
|
|  File Description:
|  Contains tasks and procedures for standard MIT handlings and stuff.
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
|  Unpublished work - Copyright 1996 - 1998 Otis Elevator Company
|  All rights reserved.
|
|
|  Revision History:
|
|  SCN              Author                       Date
|  ------------------------------------------------------------------------
|
|  JAB30203AAE           Hideyuki Honma 		11 Feb  1999
|  - Created.( from Osaka Doujima building/V-MIT)
|  
|----------------------------------------------------------------%ESH%-----*/

/*-------------------------------------------------------------------------
|  Public Declarations
|--------------------------------------------------------------------------*/
#include "stdafx.h"
#include "common.h"

#ifdef CAGS 
#include "global.lit"
#include "global.h"
#include "infbld.h"
#endif//ifdef CAGS

#define EXTERN extern   /* declare public variables */
#ifndef CAGS
#include "hdw.h"
#include "sio.h"
#include "inferr.h"
#include "infbld.h"
#include "infocs.h"
#include "exec.h"
#endif //ifndef CAGS

#include "infmsg.h"
#include "e2p.h"
#include "infads.h"
#include "crowd.h"
#include "asslib.h"
#include "assign.h"
#undef EXTERN

#define EXTERN          // define public variables 
#include "mit.h"
#undef EXTERN

unsigned char debug_mit1,debug_mit2,debug_mit3;
unsigned char debug_setini;

/*-------------------------------------------------------------------------
|  Local Definitions
|--------------------------------------------------------------------------*/


INT_T    CurrentCars;
INT_T    CurrentDecks;


/*--------------------------------------------------------------------------
|---------------------------------------------------------------------------
|  P U B L I C   M I T   F U N C T I O N S
|---------------------------------------------------------------------------
|--------------------------------------------------------------------------*/
/*---------------------------------------------------------------%BSH%-----
| Function Name: mit_Init
|
| This task is responsible for initialization for standard MIT task
|
| On powering up, all variables concerned with standard MIT operation
| should be set the default value here.
|
| Parameters: none
|
|----------------------------------------------------------------%ESH%-----*/
void mit_Init(void)
 {
  BYTE_T  h,i,j;
  BYTE_T  mitDir,callMask;

  for (h=0; h<2; h++) //Number of Lobby
    for (i=0; i<=4; i++) // Number of Group
      for (j=CAR1; j<=CAR8; j++) //Number of Car
           mit_Priority[h][i][j] = mit_NoPriority;

  for (i=CAR1;i<=CAR8; i++)
     {
         //Shizuoka
         mit_GroupCar[i] = *(e2p_MitGroupCar1 + (i-1));
         mit_2ndFixedLobby[i] = 0;
     }
 } //  mit_init  

/*---------------------------------------------------------------%BSH%-----
| Function Name: mit_InitCar
|
| This task is responsible for initialization for standard MIT task
|
| On powering up, all variables concerned with standard MIT operation
| should be set the default value here.
|
| Parameters: car
|
|----------------------------------------------------------------%ESH%-----*/
void mit_InitCar(BYTE_T car)
 {
  BYTE_T  h,i,j;

  for (h=0; h<2; h++) //LOBBY
    for (i=0; i<4; i++) //GROUP
          mit_Priority[h][i][car] = mit_NoPriority;

 } //  mit_InitCar  

/*---------------------------------------------------------------%BSH%-----
|----------------------------------------------------------------%ESH%-----*/




/*---------------------------------------------------------------%BSH%-----
| Function Name: mit_CheckLobbyPos(car,position)
|                     Check whether the current position is lobby# or not
|
| Parameters: car
|           : position
|
| Return    : lobby number
|                           0: Here is No Lobby Position
|                           1: Here is Main Lobby Position
|                           2: Here is Second Lobby Position
|----------------------------------------------------------------%ESH%-----*/
BYTE_T  mit_CheckLobbyPos(BYTE_T car,BYTE_T pos)
 {
  BYTE_T  tlobby;
  BYTE_T  rflag;

  rflag = 0;
  tlobby = 0;


   if (ocss_info[car].lobby_pos == pos)
         tlobby = 1;
   else if (ocss_info[car].second_lobby_pos == pos)
         tlobby = 2;

  return (tlobby);

 }//mit_CheckLobbyPos


/*---------------------------------------------------------------%BSH%-----
| Function Name: mit_GetPriority
|
| This task is responsible to give the new MIT priority value.
| This task is called when Target and NCF(with ETA) becomes lobby and the
| car has no demmand or final demand is lobby.
|
| Parameters: car
|             pos(lobby)
|----------------------------------------------------------------%ESH%-----*/
void mit_getPriority(BYTE_T car,BYTE_T pos,BYTE_T typ)
 {
  BYTE_T  cr,lbyId,grpId,oldPri;
  BYTE_T  mit_priority_tail,priorityBit,carBit,tmpPri,pri;
#ifdef CAGS
  BYTE_T debug_msg1[50];
#endif
 
  mit_priority_tail = 0;
  
  lbyId = mit_CheckLobbyId(car,pos);
  grpId = mit_GroupCar[car];

  oldPri = mit_Priority[lbyId][grpId][car];

  if (mit_Priority[lbyId][grpId][car] <= MAX_OCSS)
     return;



  //New way like OCSS MitPriority detection, 99/2/18 update
  //check the current MitPrority status.

   priorityBit = 0;

   for (cr=CAR1;cr<=CAR8;cr++)
      {   
        tmpPri = mit_Priority[lbyId][grpId][cr];
        if ((tmpPri >= 1) && (tmpPri <= 8))
          {
            carBit |= 0x01 << (cr-1);
            priorityBit |= 0x01 << (tmpPri - 1);
          }
      }//for..
   for (pri=1; pri<=8; pri++)
     {
        if ((priorityBit & (0x01 << (pri-1)) ) == 0)
          {
            mit_Priority[lbyId][grpId][car] = pri;
            mit_SendPriority(car,pos,grpId,pri);
            break;
          }
     }//for (cr=car_Car1; cr<=CAR8; cr++)  

   if (mit_Priority[lbyId][grpId][car] == oldPri)
     {
       //this should be some problem,
       mit_SendPriority(car,pos,grpId,oldPri);
     }

#ifdef CAGS
     mit_DebugMessages(1);
#endif
#ifndef CAGS
     mit_DebugMessages2(car,pos,oldPri,mit_Priority[lbyId][grpId][car],typ);
#endif //ifndef CAGS

 }  // mit_getPriority(BYTE_T car,BYTE_T pos)


/*---------------------------------------------------------------%BSH%-----
| Function Name: mit_ReleasePriority
|
| This task is responsible to clear the current MIT priority value.
|
|
| Parameters: car
|             pos(lobby)
|----------------------------------------------------------------%ESH%-----*/
void mit_releasePriority(BYTE_T car,BYTE_T pos,BYTE_T typ)
 {
  BYTE_T   cr,oldPri,i;
  BYTE_T   lbyId,grpId;
  BYTE_T   mitCarBit=0,priorityBit=0,priorityNum=0;
  FLAG_T   priorityOk = TRUE;
#ifdef CAGS
  BYTE_T debug_msg1[50];
#endif

  lbyId = mit_CheckLobbyId(car,pos);
  grpId = mit_GroupCar[car];

  if (pos > bld_info.top_pos) return;

  oldPri = mit_Priority[lbyId][grpId][car];

  if ((mit_Priority[lbyId][grpId][car] <= MAX_OCSS) 
      && (mit_Priority[lbyId][grpId][car] != 0))
     {
        if (mit_Priority[lbyId][grpId][car] != mit_1stPriority)
          {
            //there should be error logging!!

            //CCB should be this work because after GCB set LanternMethod is Normal
            //msg_SendFlashLamp(car,door,pos,common_Up,(*e2p_FlickerInterval1*100));
          }//if (mit_Priority[lbyId][grpId][car] != mit_1stPriority)


        for (cr=CAR1; cr < MAX_OCSS; cr++)
           {
              if ((mit_Priority[lbyId][grpId][cr] > mit_Priority[lbyId][grpId][car]) &&
                 (mit_Priority[lbyId][grpId][cr] <= MAX_OCSS) )
                {
                   mit_Priority[lbyId][grpId][cr]--;

                   mitCarBit |= 0x01 << (cr-1);
                
                   priorityBit |= 0x01 << (mit_Priority[lbyId][grpId][cr]-1);
                   priorityNum++;

                   mit_SendPriority(cr,pos,grpId,mit_Priority[lbyId][grpId][cr]);
                }
           }
     }//if (mit_Priority[lbyId][grpId][car] <= car_MaxCars)

     //this logic is for MIT priority failure logic
     if ((oldPri == 1)&&(priorityNum != 0)&& ((0x01&priorityBit)==0) )
       {
          priorityOk = FALSE;
#ifndef CAGS
          mit_DebugMessages2(car,pos,oldPri,255,ReleaseFindPriorityError);
#endif 

          for (i=0;i<8 && priorityOk == FALSE;i++)
             {
               mitCarBit = 0;
               priorityBit = 0x00;
               priorityNum = 0;

            for (cr=CAR1; cr < MAX_OCSS; cr++)
               {
                if ((mit_Priority[lbyId][grpId][cr] > mit_Priority[lbyId][grpId][car]) &&
                    (mit_Priority[lbyId][grpId][cr] <= MAX_OCSS) )
                   {
                     mit_Priority[lbyId][grpId][cr]--;
                     mitCarBit |= 0x01 << (cr-1);
                     priorityBit |= 0x01 << (mit_Priority[lbyId][grpId][cr]-1);
                     priorityNum++;
                     mit_SendPriority(cr,pos,grpId,mit_Priority[lbyId][grpId][cr]);
#ifndef CAGS
                     mit_DebugMessages2(cr,pos,255,mit_Priority[lbyId][grpId][cr],typ);
#endif //ifndef CAGS
                 
                   }
               }//for (cr=CAR1; cr < MAX_OCSS; cr++)

               if ((priorityNum != 0) && ((0x01 & priorityBit)!=0))
                 priorityOk = TRUE;
             }//for (i=0;i<8 && priorityOk == FALSE;i++)
       }//if ((oldPri == 1)&&(priorityNum != 0)&& ((0x01&priorityBit)==0) )

       /* Titan way */
       if ((asslib_car_available(car)!=asslib_NotAvailable)&&(mit_Priority[lbyId][grpId][car]<=MAX_OCSS))
         mit_Priority[lbyId][grpId][car] = mit_ReleasePriority;
       else
         mit_Priority[lbyId][grpId][car] = mit_NoPriority;

#ifdef CAGS
       mit_DebugMessages(1);
#endif

#ifndef CAGS
       mit_DebugMessages2(car,pos,oldPri,mit_Priority[lbyId][grpId][car],typ);
#endif 
 }  // mit_releasePriority(BYTE_T car,BYTE_T pos)

/*-----------------------------------------------------------------%BSH%----
| Function Name: mit_SendPriority
|
| Issues a Type 7 Subtype 35 message to GCS MIT_PRIORITY to 
| a priority Requested Car
|
| Parameters: landing  - landing where call shall be canceled.
|             car      - the to park.
|
|------------------------------------------------------------------%ESH%---*/
void mit_SendPriority(BYTE_T car, BYTE_T pos, BYTE_T grp, BYTE_T priority)
{

#ifndef CAGS

BYTE_T *msg_ptr;	/* message pointer for ring messages */
BYTE_T lbyId=0,lbyBit=0,grpBit=0,carBit=0;

   if (pos == ocss_info[car].lobby_pos)
     lbyBit = 0x00;
   else if (pos == ocss_info[car].second_lobby_pos)
     lbyBit = 0x40;
   
   grpBit = 0x00;

   carBit = car;

    // 99/2/16  correct RRT calculation for MitPriority Car
    if (priority == 1)
      {
        ocss_info[car].stop_type |= S_FUHC;
        ocss_info[car].stop_pos  = pos;

        if (ocss_info[car].f_door_state == DOORS_CLOSED)
           ocss_info[car].stop_status = STOP_NOT_STARTED;
        else
           ocss_info[car].stop_status = STOP_IN_PROGRESS;
      }   

    /*----------------------------------------
    ; Transmit the message on both rings
    ------------------------------------------*/
    /* B U I L D   C A R S   M E S S A G E */
    /* ----------------------------------- */
    msg_ptr = outgoing_msg.ptr;
   *msg_ptr++ = 0xfc;                   /* byte 1 - type 7, from origin 12 = GCSS     */
   *msg_ptr++ = 0xa3;                   /* byte 2 - subtype 35                        */
   *msg_ptr++ = 0x80 | lbyBit           /* byte 3 - bit 6 LbyId 0:Main 1:2nd          */
                     | grpBit           /* byte 3 - bits 4-5 grpId 0:Std 1:Low 2:High */
                     | carBit ;         /* byte 3 - bits 0-3 Destination CarID        */
   *msg_ptr++ = 0x80 | priority;        /* byte 4 - bits 0-3 MIT Priority value       */
   *msg_ptr++ = 0x80; 		        /* byte 5 - bits 0-6 Variable Intervel Dispatch*/
   *msg_ptr   = M_checksum(outgoing_msg.ptr,MSG_LEN);
   
   sio_TxRing(outgoing_msg.ptr, MSG_LEN, RING1);

#endif


#ifdef CAGS    
    // 99/2/16  correct RRT calculation for MitPriority Car
    if (priority == 1)
      { 
         ocss_info[car].stop_type |= S_FUHC;
         ocss_info[car].stop_pos  = pos;
         if (ocss_info[car].f_door_state == DOORS_CLOSED)
            ocss_info[car].stop_status = STOP_NOT_STARTED;
         else
            ocss_info[car].stop_status = STOP_IN_PROGRESS;
      }   
#endif

}// END: mit_SendPriority()

/*-----------------------------------------------------------------%BSH%----
|  Name : mit_DebugMessages2
|
|  Description: This routine is for MIT Debugging Effort.
|               MIT Debug Messages send to TRENDI PC TOOL.
|
|  Input
|        1. None
|
|  Output  None
|------------------------------------------------------------------%ESH%---*/
void mit_DebugMessages2(BYTE_T car,BYTE_T pos,BYTE_T oldPri,BYTE_T pri,BYTE_T typ)
{

#ifndef CAGS

   BYTE_T     msgBuf[15];

	 // D E B U G   M E S S A G E 
	 // ------------------------- 
	 if (*e2p_debug_enable != 0)
	 {
              msgBuf[0] = 'K';
              msgBuf[1] = car;
              msgBuf[2] = pos;
              msgBuf[3] = oldPri;
              msgBuf[4] = pri;
              msgBuf[5] = typ;
              msgBuf[6] = 255;
              msgBuf[7] = 13;

              if (!(sio_TxPort(GSV_TOOL_PORT,msgBuf, 8)))
                E_log(DEBUG_TX_ERROR); 

	 } // End of debug message 

#endif //ifndef CAGS

#ifdef CAGS

 long diff,aClock;
 BYTE_T msg[150];

#define   RequestOcss			0
#define   RequestNoDemand		1

#define   Release0			2
#define   Release15			3
#define   ReleaseGroupOut		4
#define   ReleaseTimeOut		5

 BYTE_T *mitStepString[20] = {"Get Ocs ",
                              "Get 12  ",
                              "Rel 0   ",
                              "Rel 15  ",
                              "Rel Grp ",
                              "Rel Time"};

     if ((usage == ENGINEER_USE) && (CRT_mode == 1))
     {
     }//if ((usage == ENGINEER_USE

#endif //ifdef CAGS

}//END: void mit_DebugMessages2

/*---------------------------------------------------------------%BSH%-----
| Function Name: mit_CheckLobbyCar
|
| This task check whether the car is stopping a lobby position for MIT operatioon
| (whether the car should get a Mit Priority)
|
| Parameters:   car
|               lobby_pos
|
|----------------------------------------------------------------%ESH%-----*/
void mit_CheckLobbyCar(BYTE_T car, BYTE_T lbyPos)
 {
  FLAG_T stoppingForMitLobby;
  BYTE_T preNoDemand,mitDir,lbyId,grpId;
  stoppingForMitLobby = FALSE;
  mitDir = UP_DIR;

  grpId = mit_GroupCar[car];

  preNoDemand = mit_CarPreNoDemand(car,mitDir);

  if (ocss_info[car].actual_pos == lbyPos)
    {
      if (ocss_info[car].target_pos != lbyPos)
         return;
    }


  if ((mit_Priority[0][grpId][car]==15)&&(preNoDemand<=1))
    {

    } //if ((car_GetAnyCallDemand(car)!=0)&&(preNoDemand==FALSE))
  else //No-Demand
    {
       if (mit_Priority[0][grpId][car]==12)
          {
            stoppingForMitLobby = TRUE;
          }
      else if (((preNoDemand==TRUE)||(preNoDemand==2)) &&//HallCallDemand,this should be e2pSetup,
                 (asslib_calculate_pre_dir(car) == mitDir))
          {
              stoppingForMitLobby = TRUE;
          }
     }//No-Demand


   if (stoppingForMitLobby == TRUE)
     {
       debug_mit1 = 10;
       //       mit_getPriority(car,lbyPos);
     }
} // end of mit_CheckLobbyCar()


/*---------------------------------------------------------------%BSH%-----
| Function Name: mit_PriorityCarInLobby
|
| Parameters:   car
|               lobby_pos
|
| Return:       flag whether the priority car is in lobby pos
|----------------------------------------------------------------%ESH%-----*/
FLAG_T mit_PriorityCarInLobby(BYTE_T car,BYTE_T lbyPos,BYTE_T priority)
{
  BYTE_T i,grpId,lbyId;
  FLAG_T priorityCarInLobby;

    priorityCarInLobby = FALSE;
    grpId = mit_GroupCar[car];
    lbyId = mit_CheckLobbyId(car,lbyPos);

    for (i=CAR1;i<MAX_OCSS;i++)
      {
       if (mit_Priority[lbyId][grpId][i] == priority)
         {
           priorityCarInLobby = TRUE;
           break;
         }
      }//for (i=car_Car1;i<car_MaxCars;i++)
  return(priorityCarInLobby);
}// end of mit_PriorityCarInLobby()





/*---------------------------------------------------------------%BSH%-----
|---------------------------------------------------------------------------
|  L O C A L   M I T   F U N C T I O N S
|---------------------------------------------------------------------------
|----------------------------------------------------------------%ESH%-----*/

/*---------------------------------------------------------------%BSH%-----
| Function Name: mit_CheckLobbyId
|
| This task is responsible to check Lobby Id
|
|
| Parameters: car
|             pos
|
| Return    : lobbyId(0:MainLobby or 1:2ndLobby)
|----------------------------------------------------------------%ESH%-----*/
BYTE_T mit_CheckLobbyId(BYTE_T car,BYTE_T pos)
{
 BYTE_T   lbyId;

  lbyId = 0;

  if (ocss_info[car].lobby_pos == pos)       lbyId = 0;
  else if (ocss_info[car].second_lobby_pos == pos)  lbyId = 1;

 return(lbyId);
}


/*---------------------------------------------------------------%BSH%-----
| Function Name: mit_CheckCarsAtLobby
|
| This task is responsible for calculate number of car at Lobby with Mit_Priority
|
| Parameters: grpId
|             lbyPos
|
| Parameters: number of cars in a lobby
|
|----------------------------------------------------------------%ESH%-----*/
BYTE_T mit_CheckCarsAtLobby(BYTE_T grpId,BYTE_T lbyPos)
{

 BYTE_T car,deck,NoOfCars,lbyId;

  NoOfCars = 0;
  lbyId = 0;

   if (lbyPos == bld_info.ocss_lobby_pos)       lbyId = 0;
   else if (lbyPos == bld_info.second_lobby_pos)  lbyId = 1;

   for (car=CAR1; car<MAX_OCSS; car++)
      {
        if ((ocss_info[car].target_pos == lbyPos) &&
           (ocss_info[car].NCF == lbyPos) &&
           (mit_Priority[grpId][lbyId][car] <= MAX_OCSS))
           {
              NoOfCars++;
              mit_lobbyCar[car] = lbyPos;
           }
      }

 return(NoOfCars);

} // mit_CheckCarsAtLobby()

/*---------------------------------------------------------------%BSH%-----
| Function Name: mit_CheckCarsToLobby
|
| This task is responsible for calculate number of car parking to Lobby
|
| Parameters: grpId
|             lbyId
|
| Return    : number of cars to a lobby
|----------------------------------------------------------------%ESH%-----*/
BYTE_T mit_CheckCarsToLobby(BYTE_T grpId,BYTE_T lbyPos)
{
 BYTE_T car,deck,NoOfCars,lbyId;

  NoOfCars = 0;
  lbyId = 0;

   if (lbyPos == bld_info.ocss_lobby_pos)       lbyId = 0;
   else if (lbyPos == bld_info.second_lobby_pos)  lbyId = 1;

   for (car=CAR1; car<MAX_OCSS; car++)
      {
        if ((ocss_info[car].target_pos == lbyPos) &&
           (mit_Priority[grpId][lbyId][car] > MAX_OCSS) &&
           (mit_Priority[grpId][lbyId][car] == 12))
           // or Car has only Lobby CarCall or UpHallCall in Lobby Pos
           {
              NoOfCars++;
              mit_lobbyCar[car] = lbyPos;
           }
      }

 return(NoOfCars);

} // mit_CheckCarsToLobby()

/*---------------------------------------------------------------%BSH%-----
| Function Name: mit_UpdateTimers
|
| This task is responsible for updating Timers related MIT
|
| Parameters: None
|
| Return    : None
|----------------------------------------------------------------%ESH%-----*/
void mit_UpdateTimers(void)
{
 BYTE_T i;
 FLAG_T loadFlag;

   for (i=0; i<mit_MaxTimers; i++)
     {
       if (mit_Timer[i] != 0)
         {
          mit_Timer[i]--;
         }
     }//for ..

} // mit_UpdateTimers()

/*---------------------------------------------------------------%BSH%-----
| Function Name: mit_CarPreNoDemand
|
| This task is responsible whether the car is PreNoDemand state
|
| Parameters: carId
|             mitDir
|
| Return    : TRUE:NoDemand/FALSE:AnyDemand
|----------------------------------------------------------------%ESH%-----*/
BYTE_T mit_CarPreNoDemand(BYTE_T car,BYTE_T mitDir)
{
 BYTE_T pos,deck,stopPos,targetPos;
 BYTE_T returnValue;
 INT_T  hallMask;

   returnValue = TRUE;
   hallMask = 0;
   if (mitDir == UP_DIR)        hallMask = S_FDHC;

    stopPos = ocss_info[car].NCF;
    targetPos = ocss_info[car].target_pos;
    for (pos=0;pos<=ocss_info[car].top_pos;pos++)
     {
      if ((pos==stopPos)&&(pos==targetPos))
       {
        //channeling
        if (ocss_info[car].opmode != CHN)
        if (((ocss_info[car].assigned_calls[pos]&hallMask)!=0)||
          ((ocss_info[car].stop_type&hallMask)!=0))
         {
           returnValue = 0;
           break;
         }
       }
      else
       {

          if ((ocss_info[car].assigned_calls[pos] & S_CC)!=0)
          {
            returnValue = 1;
            break;
          }
          else if ((ocss_info[car].assigned_calls[pos] & S_HC)!=0)
          {
            returnValue = 2;
            //break;
          }
       }
     }//for (pos=0....

 return(returnValue);

} // mit_CarPreNoDemand()





void mit_DebugMessages(BYTE_T ori)
{
#ifdef CAGS
  BYTE_T debug_msg1[50];
     if ((usage == ENGINEER_USE) && (CRT_mode == 1))
     {
       sprintf(debug_msg1, "MITG %x %x %x %x %x %x %x %x [%4.0f]\n",
               mit_Priority[0][0][CAR1],
               mit_Priority[0][0][CAR2],
               mit_Priority[0][0][CAR3],
               mit_Priority[0][0][CAR4],
               mit_Priority[0][0][CAR5],
               mit_Priority[0][0][CAR6],
               mit_Priority[0][0][CAR7],
               mit_Priority[0][0][CAR8],
               tm_sys);
       if (ori == 0)
        deb_msg(9,debug_msg1);
       else if (ori == 1)
        deb_msg(10,debug_msg1);
     }

#endif
}//mit_DebugMessages..

unsigned char dpc1;
unsigned int  dpc2;
/*-----------------------------------------------------------------%BSH%----
|  File Name: DPC.C - Tasks and procedures for Down Peak Channeling
|
|  File Description:
|
|  Contains tasks and procedures for handling DPC  and stuff.
|
|  This work and the information it contains are the confidential
|  property of Otis Elevator Company ("Otis").  It is delivered on
|  the express condition that it will be used only for, or on
|  behalf of, Otis; that neither it nor the information it contains
|  will be reproduced, distributed or disclosed, in whole or in part,
|  without the written consent of Otis; and that on demand it and any
|  copies will be promptly returned.
|
|  Unpublished work - Copyright 2001 Otis Elevator Company
|  All rights reserved.
|
|
|
|  Revision History:
|
|  SCN                   Author                        Date
|  ------------------------------------------------------------------------
|  JAB30203AAJ           Hideyuki Honma             12 Sep 2001
|  - Created program
|
|----------------------------------------------------------------%ESH%-----*/

#include "stdafx.h"
#include "common.h"

#ifdef CAGS 
#include "global.lit"
#include "global.h"
#include "infbld.h"
#endif//ifdef CAGS

/*------------------------------------------------------------------------
|  Public Declarations
|-------------------------------------------------------------------------*/
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


/*------------------------------------------------------------------------
|  Public Definitions
|-------------------------------------------------------------------------*/
#define EXTERN          // define public variables 
#include "dpc.h"
#undef EXTERN

/*------------------------------------------------------------------------
|  Local Definitions
|-------------------------------------------------------------------------*/

 
/*------------------------------------------------------------------------
|  Local Variables
|-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
|    Public Functions
|-------------------------------------------------------------------------*/

/*-----------------------------------------------------------------%BSH%----
| Function Name: dpc_Init
|
| This procedure is responsible for initializing global variables
| related to DPC operation.
|
| Parameters: none
|
|------------------------------------------------------------------%ESH%---*/

void dpc_Init(void)
{
  BYTE_T i,j,flag,tmpFlag,k,sec;
  BYTE_T secBot,secTop,availableCars,maxSector;
  BYTE_T servFloors,secRange,secRemain,fixedCars;
  FLAG_T chnFlag,fixedFlag;  

  
  dpc_ConditionFlag = FALSE;
  dpc_ChnType = *e2p_DpcType;
  secBot = *e2p_DpcLobbyPos + 1;
  secTop = bld_info.top_pos;
  dpc_SectorNumber = *e2p_DpcSectorNumber;
  dpc_LobbyPos = *e2p_DpcLobbyPos;
  fixedFlag = FALSE;
  dpc_FixedFlag = dpc_FixedCars = 0;

    if (*e2p_DpcType == 0)
    {
      dpc_ChnType = DPC_STATIC;
    }
    else if (*e2p_DpcType == 1)
    {
      dpc_ChnType = DPC_FIXED;
    }
    else dpc_ChnType = DPC_STATIC;

    dpc_AvailableCars = cars_available_count();

    if ((dpc_ChnType == DPC_STATIC) ||
       (((dpc_EnableFlag == TRUE)&&(ocss_dpc_clock_flag == FALSE))||
       ((dpc_EnableFlag == FALSE)&&(ocss_dpc_clock_flag == TRUE))))
      {

        if (dpc_AvailableCars != cars_available_count())
          {
            for (sec=0;sec<=MAX_DPC_SECTOR;sec++)
               {
                  dpc_SendCarSector(sec+1,sec,FALSE);
               } 
          }

        for (i=0;i<=MAX_DPC_SECTOR;i++)
          {
            dpc_Sector[i].bot = 0;
            dpc_Sector[i].top = 0;
            dpc_SectorOccupied[i] = 0;
          }

        for (i=0;i<MAX_OCSS;i++)
          {
             dpc_SectorHave[i] = Sector_Invalid; 
          }

        for (i=0;i<=bld_info.top_pos;i++)
            dpc_BelongSector[i] = 255;

      }//if (dpc_ChnType == DPC_STATIC
 

    if ((dpc_SectorNumber < 1)&&(dpc_AvailableCars > 1))
        dpc_SectorNumber = dpc_AvailableCars - 1;

    if ((dpc_SectorNumber > 0)&&(dpc_AvailableCars > 1)&&(secTop>secBot)&&
        (dpc_SectorNumber < dpc_AvailableCars))
        dpc_ConditionFlag = TRUE;
 
    if (((dpc_ConditionFlag == TRUE) && (ocss_dpc_clock_flag == TRUE)) &&
          (*e2p_DpcEnable != 0))
      {
         dpc_EnableFlag = TRUE;
    
         if (dpc_ChnType == DPC_FIXED)
           {
              fixedFlag = TRUE; fixedCars = 0;
              if (*e2p_DpcSector1Bot < (dpc_LobbyPos+1)) fixedFlag = FALSE;

              if (fixedFlag == TRUE)
              for (i=0;i<(dpc_SectorNumber-1);i++)
                 {
                    if (*(e2p_DpcSector1Bot+i) <= *(e2p_DpcSector1Top+i))
                      {
                        if ( (*(e2p_DpcSector1Top+i)+1) == (*(e2p_DpcSector1Bot+i+1)) )
                           fixedCars++;
                        else
                           fixedFlag = FALSE;
                      }
                    else
                      fixedFlag = FALSE; 
                 }

               if (fixedFlag == TRUE)
               if (*(e2p_DpcSector1Top+(dpc_SectorNumber-1)) == bld_info.top_pos)
                  fixedCars++;
               else fixedFlag = FALSE;

               if ((fixedCars != dpc_SectorNumber) || (fixedCars >= cars_available_count()))
                  dpc_ChnType = DPC_STATIC; 

               dpc_FixedCars = fixedCars;

               if (dpc_ChnType == DPC_FIXED)
                 {
                    for (i=0;i<dpc_SectorNumber;i++)
                       {
                          dpc_Sector[i].bot = *(e2p_DpcSector1Bot+i);
                          dpc_Sector[i].top = *(e2p_DpcSector1Top+i);
                       }
                 }
           }//DPC_FIXED

           if (dpc_ChnType == DPC_STATIC)
             {
                if (dpc_SectorNumber >= dpc_AvailableCars)
                    dpc_SectorNumber = dpc_AvailableCars - 1;
         
                //caluculate Static Sector
                servFloors = secTop - secBot + 1;
                secRange   = servFloors / dpc_SectorNumber;
                secRemain  = servFloors % dpc_SectorNumber;

                dpc_Sector[0].bot = secBot;
                dpc_Sector[0].top = secBot + secRange - 1;

                if (secRemain>0)
                  {
                    dpc_Sector[0].top+=1;
                    secRemain--;
                  }

                for (i=1; i<dpc_SectorNumber; i++)
                   {
                      dpc_Sector[i].bot = dpc_Sector[i-1].top+1;
                      dpc_Sector[i].top = dpc_Sector[i].bot + secRange - 1;
                      if (secRemain>0)
                        {
                           dpc_Sector[i].top+=1;
                           secRemain--;
                        }
                   }

             }//STATIC 
           else //FIXED
             {
       
             }//FIXED

          dpc_SendSectorInfo();
      }//if (dpc_ConditionFlag == TRUE)
    else //dpc_ConditionFlag == FALSE)
     {
        if (dpc_EnableFlag == TRUE)
          {
            dpc_EnableFlag = FALSE;
            dpc_ConditionFlag = FALSE;
        
            for (sec=0;sec<=MAX_DPC_SECTOR;sec++)
               {
                  dpc_SendCarSector(sec+1,sec,FALSE);
               } 
          }
     }//dpc_ConditionFlag == FALSE


    for (i=0;i<dpc_SectorNumber;i++)
       {
          dpc_Sector[i].penalty = *(e2p_DpcSector1Penalty+i);
       }

    for (i=0; i<dpc_SectorNumber; i++)
       {
          for (j=dpc_Sector[i].bot; j<=dpc_Sector[i].top; j++)
             {
               dpc_BelongSector[j]=i;
             } 
        }
                                           
}  /* dpc_Init */


/*-----------------------------------------------------------------%BSH%----
| Function Name: dpc_SectorTest
|
| This procedure is responsible for Testing Sectors
|
| Parameters: none
|
|------------------------------------------------------------------%ESH%---*/

void dpc_SectorTest(void)
{
  BYTE_T i,j,flag,tmpFlag,k,sec;
  
  if ((dpc_EnableFlag == FALSE) &&(dpc_EnableFlag2 == FALSE)) return;

  dpc_SendSectorInfo();
  
  for (sec=0;sec<dpc_SectorNumber;sec++)
     {
       dpc_SendCarSector(sec+1,sec,TRUE);
     }                                           
}  /* dpc_SectorTest */

/*-----------------------------------------------------------------%BSH%----
|  Name: dpc_SendSectorInfo();  
|  Description:
|
|------------------------------------------------------------------%ESH%---*/
void dpc_SendSectorInfo(void)
{
  BYTE_T car,sec;           /* clustered car */
  BYTE_T *msg_ptr;	/* message pointer for ring messages */


    for (sec=0;sec<MAX_DPC_SECTOR;sec++)
       {
         if (sec < dpc_SectorNumber)
           {
             msg_ptr = outgoing_msg.ptr;
             *msg_ptr++ = 0xec;                         /* byte 1 - type 6, from origin 12 = GCSS  */
             *msg_ptr++ = 0xa0 | sec;                   /* byte 2 - subtype 2, sector number       */
             *msg_ptr++ = 0x80 | (dpc_Sector[sec].bot); /* byte 3 - starting floor                 */
             *msg_ptr++ = 0x80 | (dpc_Sector[sec].top); /* byte 4 - end floor                      */
             *msg_ptr++ = 0x80;                         /* byte 5 - reserved                       */
             *msg_ptr   = M_checksum(outgoing_msg.ptr,MSG_LEN);
   
             sio_TxRing(outgoing_msg.ptr, MSG_LEN, RING1);
           }
       }
}//dpc_SendSector

/*-----------------------------------------------------------------%BSH%----
|  Name: dpc_SendOneSectorInfo(sec);  
|  Description:
|
|------------------------------------------------------------------%ESH%---*/
void dpc_SendOneSectorInfo(BYTE_T sec)
{
  BYTE_T car;           /* clustered car */
  BYTE_T *msg_ptr;	/* message pointer for ring messages */

      if (sec < dpc_SectorNumber)
      {
        msg_ptr = outgoing_msg.ptr;
        *msg_ptr++ = 0xec;                          /* byte 1 - type 6, from origin 12 = GCSS  */
        *msg_ptr++ = 0xa0 | sec;                    /* byte 2 - subtype 2, sector number       */
        *msg_ptr++ = 0x80 | (dpc_Sector[sec].bot);  /* byte 3 - starting floor                 */
        *msg_ptr++ = 0x80 | (dpc_Sector[sec].top);  /* byte 4 - end floor                      */
        *msg_ptr++ = 0x80;                          /* byte 5 - reserved                       */
        *msg_ptr   = M_checksum(outgoing_msg.ptr,MSG_LEN);
   
        sio_TxRing(outgoing_msg.ptr, MSG_LEN, RING1);
      }
}//dpc_SendSector

/*-----------------------------------------------------------------%BSH%----
|  Name: dpc_SendCarSector();  
|  Description:
|
|------------------------------------------------------------------%ESH%---*/
void dpc_SendCarSector(BYTE_T car,BYTE_T sec,FLAG_T assign)
{
  BYTE_T *msg_ptr;	/* message pointer for ring messages */
  BYTE_T funcCode;
   
  if (assign == TRUE) funcCode = 3 << 4;
  else               funcCode = 4 << 4;

      msg_ptr = outgoing_msg.ptr;
      *msg_ptr++ = 0xec;                        /* byte 1 - type 6, from origin 12 = GCSS  */
      *msg_ptr++ = 0xc0 | car;                  /* byte 2 - subtype 4,destination          */
      *msg_ptr++ = 0x80 | funcCode | sec;       /* byte 3 - FuncCode , SectorID            */
      *msg_ptr++ = 0x80;                        /* byte 4 - reserved                       */
      *msg_ptr++ = 0x80;                        /* byte 5 - reserved                       */
      *msg_ptr   = M_checksum(outgoing_msg.ptr,MSG_LEN);
   
      sio_TxRing(outgoing_msg.ptr, MSG_LEN, RING1);
}//dpc_SendCarSector


/*-----------------------------------------------------------------%BSH%----
|  Name: dpc_DownHallCallHave();  
|  Description:
|
|------------------------------------------------------------------%ESH%---*/
FLAG_T  dpc_DownHallCallHave( BYTE_T cr)
  {
  BYTE_T  i, flag2;
  flag2 = FALSE;

  // OsakaOBP for (i=1; i<TOP_FLOOR && flag2==TRUE; i++)
  for (i=1; i<=bld_info.top_pos && flag2==FALSE; i++)
      {
         if ((ocss_info[cr].assigned_calls[i] & S_FDHC)!=0)
             flag2 = TRUE;
      }
    return (flag2);
  }


/*-----------------------------------------------------------------%BSH%----
|  Name: dpc_ClusterRelease();  
|  Description:
|
|------------------------------------------------------------------%ESH%---*/
void dpc_ClusterRelease(BYTE_T  cr)
  {
  BYTE_T  flag1,i;

  flag1 = TRUE;

  if (dpc_DownHallCallHave(cr) == TRUE) flag1 = FALSE;

  if ((dpc_SectorHave[cr]!=Sector_Invalid) && (flag1==TRUE))
      {
         if ((ocss_info[cr].pre_dir == DOWN_DIR) &&
             (asslib_car_available(cr) == asslib_NormalMode) && //add 18Sep01
             (ocss_info[cr].opmode != IDL)&&(ocss_info[cr].opmode != PRK) && //add 19Sep01
             /*
               ((ocss_info[cr].actual_pos>=chn_sector[sector_have[cr]][0]) &&
                (ocss_info[cr].actual_pos<=chn_sector[sector_have[cr]][1])) )
             */
             (ocss_info[cr].actual_pos > dpc_LobbyPos))
           {
                flag1 = FALSE;
           }

      if (flag1==TRUE)
          {
            if (dpc_SectorOccupied[dpc_SectorHave[cr]]==cr)
               dpc_SectorOccupied[dpc_SectorHave[cr]]=0;
            dpc_SendCarSector(cr,dpc_SectorHave[cr],FALSE);
            dpc_SectorHave[cr] = Sector_Invalid;
          }
      }

  }//void dpc_ClusterRelease(BYTE_T  cr)



// END DPC.C 
/* dpc.c */




#include "stdafx.h"
#include "common.h"

#define EXTERN extern
#include "infbld.h"
#include "infocs.h"
#include "queue.h"
#include "exec.h"
#include "infdes.h"
#include "crisprrt.h"
#include "crisppwt.h"
#include "assign.h"
#include "sio.h"
#include "inferr.h"
#include "asslib.h"
#include "pi.h"
#include "infmsg.h"
#include "infads.h"
#include "dpc.h"
#include "mit.h"
#include "infics.h"
#include "park.h"
#undef EXTERN

/*************************************************************************
;  Public Definitions
;*************************************************************************/
#define EXTERN          /* define public variables */
#include "infrng.h"
#undef EXTERN

/**************************************************************************
;  Local Definitions
;*************************************************************************/
#include "infrng.lh"

unsigned char debug_second,debug_pref,debug_ems;
unsigned int debug_board,debug_deboard,tmpBuf1;
unsigned char infrng1,infrng2,infrng3,infrng4,infrng5,infrng6,Type7Sub100,Type7Sub1004;
unsigned int  debDup1;
unsigned char debDup2[5];
unsigned char dbdDebug[10];

/*********************************************************************
; Function Name: test_new_hall_call()
;
; Description: This task checks to see if a new hall call has been
;              received.  If so, it stores it and enables the hall
;              call assignment task to run.
;
; Parameters: landing  - landing of new hall call
;             destination
;             callmask - bit mask containing new hall call(s)
;             riser    - riser Number of new hall call(s)
;********************************************************************/
//DBD
extern "C" __declspec(dllexport) void test_new_hall_call(BYTE_T landing, BYTE_T callmask, BYTE_T riser, BYTE_T destPos)
{

   BYTE_T    msg_temp[10],car,assigned_car;
   BYTE_T    call_que[4];
   TASK_ID_T TaskID;

   WORD_T    passId;
   BYTE_T    passTmp[2];

dbdDebug[3]++;
   if ((fuzzy_logic_enabled == TRUE && ica_central_group_enabled_flag == TRUE) &&
      (riser < MAX_RISER))
   {
dbdDebug[4]++;
      /* if call is not in old hc mask, it must be new. */
      /* ---------------------------------------------- */
      if ( (bld_info.old_hc_mask[riser][landing] & callmask) == 0)
      {
dbdDebug[5]++;
         /* place call in new and old masks, with landing of call. */
         /* ------------------------------------------------------ */
        // if ((bld_info.new_hc_mask[riser][landing] & callmask) == 0)  DBD-honma 23Mar04
        // It is not needed to check for DBD, all passenger queues should be handled.
        if (1)
        {
dbdDebug[6]++;
         bld_info.new_hc_mask[riser][landing] |= callmask;

         /* place call in new calls queue */
         call_que[0] = landing;
         call_que[1] = callmask;
         call_que[2] = riser;
         call_que[3] = destPos; //call_dst

//DBD         Q_Put(ica_call_q_ptr, call_que, 3);
         Q_Put(ica_call_q_ptr, call_que, 4);


         /* Debug message for new hall call */
         /* ------------------------------- */
         if (*e2p_debug_enable != 0)
         {
            msg_temp[0] = 'H';
            msg_temp[1] = landing;
            msg_temp[2] = callmask;
//DBD
            msg_temp[3] = destPos;
            msg_temp[4] = 13;

            if (!(sio_TxPort(GSV_TOOL_PORT, msg_temp, 5)))
               E_log(DEBUG_TX_ERROR);

         }


         /* schedule ica assignment task. */
         /* ----------------------------- */
         Exec_Enable_And_Run_Task(assign_new_tskID);

         /* Initialize reassignment counter. */
         /* -------------------------------- */
         switch(callmask)
         {
            case S_FUHC:
               bld_info.fuhc[riser][landing].reassigned_count = 0;
               break;

            case S_FDHC:
               bld_info.fdhc[riser][landing].reassigned_count = 0;
               break;

            case S_RUHC:
               bld_info.ruhc[riser][landing].reassigned_count = 0;
               break;

            case S_RDHC:
               bld_info.rdhc[riser][landing].reassigned_count = 0;
               break;
         } /* switch(callmask) */
       } /* new_hc_mask == 0 */
      //add Failure Management 00/09/01
       else  //NewHcMask != 0 and OldHcMask == 0
             //Dispach is On and Assigned Car is not assigned yet
       {
          assigned_car = 0;
          for (car=CAR1; car<=CAR8; car++)
          {
             if ( ((ocss_info[car].assigned_calls[landing] & callmask) != 0) &&
                 (riser == infbld_SgoGroupCar[car]) )
             {
                assigned_car = car;
                 break;
             }
          } // for each car

          if (assigned_car == 0)
          {
            // reset old & new hc masks
            // ------------------------
/*
            bld_info.old_hc_mask[landing] &= ~callmask;
            bld_info.new_hc_mask[landing] &= ~callmask;
            test_new_hall_call(landing, callmask);
*/
            E_log(NoLocalAssign2);
          }
          //DBD
          else
          {
              asslib_DbdExpectCallsError[6]++;
          }
         /* Debug message for new hall call */
         /* ------------------------------- */
         //DBD
/*
                 if (*e2p_debug_enable != 0)
                 {
                  msg_temp[0] = 'Z';
                  msg_temp[1] = landing;
                  msg_temp[2] = callmask;
                  msg_temp[3] = riser;
                  msg_temp[4] = destPos;
                  msg_temp[5] = 2;
                  msg_temp[6] = assigned_car;
                  msg_temp[7] = 13;

                  #ifndef CAGS
                     if (!(sio_TxPort(GSV_TOOL_PORT,msg_temp,8)))
                         E_log(DEBUG_TX_ERROR);
                  #endif //ifndef CAGS
                 }
*/
       }//else failure logic  NewHcMask

      } /* a new call */
//add New Failure Management for Dojima 00/08/31
      else //There must be a assigned car,
      {
//DBD
        if (*e2p_DbdAssignEnable != 0)
        {
         //DBD
         bld_info.new_hc_mask[riser][landing] |= callmask;

         /* place call in new calls queue */
         call_que[0] = landing;
         call_que[1] = callmask;
         call_que[2] = riser;
         call_que[3] = destPos; //call_dst

         Q_Put(ica_call_q_ptr, call_que, 4);

/*
         asslib_OneWordToTwoBytes(passId,&passTmp[0]);
         Q_Put(ica_call_q_ptr, passTmp, 2);
*/
         /* Debug message for new hall call */
         /* ------------------------------- */
         //DBD
/*
                 if (*e2p_debug_enable != 0)
                 {
                  msg_temp[0] = 'Z';
                  msg_temp[1] = landing;
                  msg_temp[2] = callmask;
                  msg_temp[3] = riser;
                  msg_temp[4] = destPos;
                  msg_temp[5] = 1;
                  msg_temp[6] = 255;
                  msg_temp[7] = 13;

                  #ifndef CAGS
                     if (!(sio_TxPort(GSV_TOOL_PORT,msg_temp,8)))
                         E_log(DEBUG_TX_ERROR);
                  #endif //ifndef CAGS
                 }
*/
         /* schedule ica assignment task. */
         /* ----------------------------- */
         //DBD
         Exec_Enable_And_Run_Task(assign_new_tskID);

        }



         assigned_car = 0;
         for (car=CAR1; car<=CAR8; car++)
         {
            if ( ((ocss_info[car].assigned_calls[landing] & callmask) != 0) &&
                 (riser == infbld_SgoGroupCar[car]) )
            {
               assigned_car = car;
                break;
            }
         } // for each car

        if (assigned_car == 0)
        {
          // reset old & new hc masks
          // ------------------------
          /* DBD
           bld_info.old_hc_mask[riser][landing] &= ~callmask;
           bld_info.new_hc_mask[riser][landing] &= ~callmask;
           test_new_hall_call(landing, callmask, riser, destPos);
           */
           E_log(NoLocalAssign);
        }
      }//else failure logic

   } /* fuzzy logic and central group enabled */
}



/*********************************************************************
; Function Name: ocss_car_cnt
;
; Description: This procedure counts the number of OCSS's which are
;              active. This is done by checking the status message
;              timeout.
;
; Parameters:
;
;********************************************************************/
BYTE_T ocss_car_cnt()

{

   OCSS_T *ocss_ptr;
   BYTE_T car;
   BYTE_T loc_cig_mask;

   /*--------------------------------------------------------------------
   ; Determine the number of cars in the group.
   ----------------------------------------------------------------------*/
   old_number_of_cars = number_of_cars;
   loc_cig_mask = 0;
   number_of_cars = 0;
   for (car=CAR1;car<=CAR8;car++)
   {
      ocss_ptr = &ocss_info[car];
      if (ocss_ptr->ring12_T0_tout > 0)
      {
        loc_cig_mask = (loc_cig_mask | (0x01 << (car-1)));
        number_of_cars++;
      }
   }

   return(loc_cig_mask);
}

/*********************************************************************
; Function Name: ring_tout
;
; Description: This function updates a car's response timeout. A response
;              timeout is maintained for each ring. Using this timeout
;              information, "ocss_icss_monitor_tsk" will determine a car's
;              source ring. The source ring for each car is used to determine
;              the source of messages for the adss/pu.
;
; Parameters:
; Message Pointer       (input)  Pointer to start of the ring message
; Ring Number           (input)  Ring through which the message was received.
;
;********************************************************************/
void ring_tout(BYTE_T *ptr, BYTE_T ring)
{

   BYTE_T msg_origin;

   msg_origin = *(ptr) & 0x0f;

   if ((msg_origin >= CAR1) && (msg_origin <= CAR8))
   {
      switch(ring)
      {

   /*---------------------------------------------------------
   ; Type three ring message received through ring1, therefore:
   ;     - Set ring1 response timeout.
   ----------------------------------------------------------*/
   case RING1:
      ocss_info[msg_origin].ring1_T0_tout = TIM_2S;
      break;

   /*-----------------------------------------------------------------
   ; Type three ring message received through ring2, therefore:
   ;    - Set ring2 response timeout.
   ------------------------------------------------------------------*/
   case RING2:
      ocss_info[msg_origin].ring2_T0_tout = TIM_2S;
      break;

   /*----------------------------------------------------
   ; Bad case, log the error.
   ----------------------------------------------------*/
   default:
      E_log(BAD_CASE);
      break;
      }
   }
}

void status_msg(BYTE_T *msg_ptr, BYTE_T ring_num)
{
   BYTE_T  msg_origin;
   OCSS_T *ocss_ptr;
   BYTE_T loc_cig_mask;
   TASK_ID_T FuzzyTaskID;
   BYTE_T i,j;
   BYTE_T    msg_temp[10];

   BYTE_T  carPos,dest_mask,dstPos,start,finish,callDir,start2,finish2;

   /* determine message origin */
   msg_origin = M_get_origin(msg_ptr);

   /*--------------------------------------------------------------------
   ; If this is the first status message received for this car, let ADSS
   ; know about this as soon as possible.
   ;---------------------------------------------------------------------*/
   if (msg_origin >= CAR1 && msg_origin <= CAR8)
   {
      /* define ocss pointer */
      /*-----------------------*/
      ocss_ptr = &ocss_info[msg_origin];

      switch(M_get_subtype(msg_ptr))
      {
   /*--------------------------
   ; Sub-type 0
   ---------------------------*/
   case 0:

              if (ocss_ptr->ring12_T0_tout == 0)
              {
                 loc_cig_mask = ocss_car_cnt();
                 ocss_trans_car_cnt(loc_cig_mask);
              }
              ocss_ptr->ring12_T0_tout = TIM_2S;

       ring_info[ring_num].send_to_adss = ads_icss2_T0[0];
       ocss_ptr->load_state = (*(msg_ptr + 1) >> 2) & 0x07;
       ocss_ptr->pre_dir = *(msg_ptr + 1) & 0x03;

      //DBD-honma 07Mar04
       ocss_ptr->f_oldDoorState = ocss_ptr->f_door_state;
       ocss_ptr->r_oldDoorState = ocss_ptr->r_door_state;

       ocss_ptr->f_door_state = *(msg_ptr + 3) & 0x03;
       ocss_ptr->r_door_state = (*(msg_ptr + 3) >> 2) & 0x03;

       ocss_ptr->old_mov_dir = ocss_ptr->mov_dir;
       ocss_ptr->mov_dir = (*(msg_ptr + 3) >> 4) & 0x03;

            //DBD-honma 17FEB04
            if (ocss_ptr->f_door_state != ocss_ptr->f_oldDoorState)
              {
                switch (ocss_ptr->f_door_state)
                {
                   case DOORS_CLOSED:
                            break;
                   case DOORS_OPENING:
                            break;
                   case DOORS_OPENED:
                            break;
                   case DOORS_CLOSING:

                   if ((*e2p_DbdAssignEnable != 0) && (ocss_ptr->f_oldDoorState == DOORS_OPENED))
                   {
                     carPos = ocss_info[msg_origin].actual_pos;

                     // DBD : If car is leaving the lobby, Remove any sector assignments
                     if ((carPos == bld_info.ocss_lobby_pos) && (ocss_info[msg_origin].pre_dir == UP_DIR))
                     {
                        DBD_removeCarFromSectorAssignments(msg_origin);
                     }

                     if (ocss_info[msg_origin].pre_dir == UP_DIR)
                       {
                         dest_mask = DBD_SUCC;
                         start = carPos+1;
                         finish = ocss_info[msg_origin].top_pos;
                         callDir = UP_DIR;
                         start2 = ocss_info[msg_origin].bottom_pos;
                         finish2 = carPos-1;
                       }
                     else if (ocss_info[msg_origin].pre_dir == DOWN_DIR)
                       {
                         dest_mask = DBD_SDCC;
                         start = ocss_info[msg_origin].bottom_pos;
                         finish = carPos-1;
                         callDir = DOWN_DIR;
                         start2 = ocss_info[msg_origin].top_pos;
                         finish2 = carPos+1;
                       }
                    if (ocss_info[msg_origin].pre_dir == UP_DIR)
                      {
                     for (dstPos = carPos+1; dstPos <= ocss_info[msg_origin].top_pos; dstPos++)
                       {
                         if ((asslib_DbdAssignCalls[msg_origin][carPos][dstPos] & dest_mask) != 0)
                           {
                             if ((asslib_DbdExpectCalls[msg_origin][dstPos] & dest_mask) != 0)
                               {
                                  if ((ocss_info[msg_origin].assigned_calls[dstPos] & S_FCC) != 0)
                                    {
                                   //   if(asslib_ValidExpectCalls2(msg_origin,callDir,ocss_info[msg_origin].bottom_pos,carPos-1,dstPos)==FALSE)
                                          asslib_DbdDeassignCall(msg_origin,dest_mask,carPos,dstPos);
                                    }
                                   else
                                    {
                                          asslib_DbdExpectCallsError[12]++;
                                          asslib_DbdDeassignCall(msg_origin,dest_mask,carPos,dstPos);

                                          if (*e2p_debug_enable != 0)
                                            {
                                              msg_temp[0] = '[';
                                              msg_temp[1] =  3;
                                              msg_temp[2] = msg_origin;
                                              msg_temp[3] = carPos;
                                              msg_temp[4] = dstPos;
                                              msg_temp[5] = 1;
                                              msg_temp[6] = 255;
                                              msg_temp[7] = 13;

                                              if (!(sio_TxPort(GSV_TOOL_PORT,msg_temp,8)))
                                                E_log(DEBUG_TX_ERROR);
                                            }

                                    }
                               }
                              else
                               {
                                asslib_DbdExpectCallsError[7]++;
                                asslib_DbdAssignCalls[msg_origin][carPos][dstPos] &= ~dest_mask;
                               }
                           }
                       }
                      } //UP_DIR
                    else if (ocss_info[msg_origin].pre_dir == DOWN_DIR)
                      {
                     for (dstPos = ocss_info[msg_origin].bottom_pos; dstPos <= (carPos-1); dstPos++)
                       {
                         if ((asslib_DbdAssignCalls[msg_origin][carPos][dstPos] & dest_mask) != 0)
                           {
                             if ((asslib_DbdExpectCalls[msg_origin][dstPos] & dest_mask) != 0)
                               {
                                  if ((ocss_info[msg_origin].assigned_calls[dstPos] & S_FCC) != 0)
                                    {
                                    //  if(asslib_ValidExpectCalls2(msg_origin,callDir,ocss_info[msg_origin].top_pos,carPos+1,dstPos)==FALSE)
                                          asslib_DbdDeassignCall(msg_origin,dest_mask,carPos,dstPos);
                                    }
                                   else
                                    {
                                          asslib_DbdExpectCallsError[13]++;
                                          asslib_DbdDeassignCall(msg_origin,dest_mask,carPos,dstPos);
                                          if (*e2p_debug_enable != 0)
                                            {
                                              msg_temp[0] = '[';
                                              msg_temp[1] =  3;
                                              msg_temp[2] = msg_origin;
                                              msg_temp[3] = carPos;
                                              msg_temp[4] = dstPos;
                                              msg_temp[5] = 2;
                                              msg_temp[6] = 255;
                                              msg_temp[7] = 13;

                                              if (!(sio_TxPort(GSV_TOOL_PORT,msg_temp,8)))
                                                E_log(DEBUG_TX_ERROR);
                                            }
                                    }
                               }
                              else
                               {
                                asslib_DbdExpectCallsError[8]++;
                                asslib_DbdAssignCalls[msg_origin][carPos][dstPos] &= ~dest_mask;
                               }
                           }
                       }
                      } //DOWN_DIR

                     }//if ((*e2p_DbdAssignEnable != 0) && (ocss_ptr->f_oldDoorState == DOORS_OPENED))
                            break;
                   default:
                            break;
                }
              }
//dojima
            if ((ocss_ptr->mov_dir != ocss_ptr->old_mov_dir) &&
//update              (ocss_ptr->mov_dir != NO_DIR))
              (ocss_ptr->mov_dir == NO_DIR))
              {
                ocss_ptr->lastStopPos = ocss_ptr->actual_pos;
              }


//kagaya
            ocss_ptr->oldNextCom = ocss_ptr->NCF;
            ocss_ptr->NCF =  *(msg_ptr + 4) & 0x7f;

//kagaya
            //OpMode ChangeInState
            if ((ocss_ptr->opmode) != (*(msg_ptr + 2) & 0x7f))
             {
              ocss_ptr->oldOpMode = ocss_ptr->opmode;
              ocss_ptr->opmode = *(msg_ptr + 2) & 0x7f;

                //OsakaObp
                if ((dpc_EnableFlag == TRUE) &&
                  ((ocss_info[msg_origin].opmode == IDL)||(ocss_info[msg_origin].opmode == PRK)))
                   dpc_ClusterRelease(msg_origin);

                //DBD
                 //DBD check if the values are cleared properly.
                if ((ocss_info[msg_origin].opmode == IDL) && (*e2p_DbdAssignEnable != 0))
                {
                   asslib_update_summary(msg_origin);
                   if (ocss_info[msg_origin].allsummary == 0)
                   {

                        for (i=0; i<=ocss_info[msg_origin].top_pos; i++)
                        {

                           //DBD-honma 01Apr04
                           if ((asslib_DbdAssignCallDirCnts[msg_origin][UP_DIR][i] != 0) ||
                              (asslib_DbdAssignCallDirCnts[msg_origin][DOWN_DIR][i] != 0) ||
                              (asslib_DbdExpectCallCnts[msg_origin][UP_DIR][i] != 0) ||
                              (asslib_DbdExpectCallCnts[msg_origin][DOWN_DIR][i] != 0) ||
                              (asslib_DbdCarCallCnts[msg_origin][i] != 0) || 
                              (asslib_DbdPeopleInCarCnts[msg_origin] != 0)) 
                              {
                                 asslib_DbdExpectCallsError[14]++;

                                asslib_DbdAssignCallDirCnts[msg_origin][UP_DIR][i] = 
                                asslib_DbdAssignCallDirCnts[msg_origin][DOWN_DIR][i] = 
                                asslib_DbdExpectCallCnts[msg_origin][UP_DIR][i] =  
                                asslib_DbdExpectCallCnts[msg_origin][DOWN_DIR][i] = 
                                asslib_DbdCarCallCnts[msg_origin][i] = 
                                asslib_DbdPeopleInCarCnts[msg_origin] = 0;
                                if (*e2p_debug_enable != 0)
                                {
                                 msg_temp[0] = '[';
                                 msg_temp[1] =  10;
                                 msg_temp[2] = msg_origin;
                                 msg_temp[3] = i;
                                 msg_temp[4] = asslib_DbdAssignCallDirCnts[msg_origin][UP_DIR][i];
                                 msg_temp[5] = asslib_DbdAssignCallDirCnts[msg_origin][DOWN_DIR][i];
                                 msg_temp[6] = asslib_DbdPeopleInCarCnts[msg_origin];
                                 msg_temp[7] = 13;

                                 if (!(sio_TxPort(GSV_TOOL_PORT,msg_temp,8)))
                                    E_log(DEBUG_TX_ERROR);
                                }
                              }
                           
                           

                           if ((bld_info.fuhc[0][i].reg_elapsed_car[msg_origin] != 255) ||
                              (bld_info.fdhc[0][i].reg_elapsed_car[msg_origin] != 255))
                             {

                                 asslib_DbdExpectCallsError[17]++;

                                 bld_info.fuhc[0][i].reg_elapsed_car[msg_origin] =
                                 bld_info.fdhc[0][i].reg_elapsed_car[msg_origin] = 0;

                                if (*e2p_debug_enable != 0)
                                {
                                 msg_temp[0] = '[';
                                 msg_temp[1] =  11;
                                 msg_temp[2] = msg_origin;
                                 msg_temp[3] = i;
                                 msg_temp[4] = bld_info.fuhc[0][i].reg_elapsed_car[msg_origin];
                                 msg_temp[5] = bld_info.fdhc[0][i].reg_elapsed_car[msg_origin];
                                 msg_temp[6] = 255;
                                 msg_temp[7] = 13;

                                 if (!(sio_TxPort(GSV_TOOL_PORT,msg_temp,8)))
                                    E_log(DEBUG_TX_ERROR);
                                }
                             }

                           if (((asslib_DbdExpectCalls[msg_origin][i] & DBD_SUCC) != 0) ||
                              ((asslib_DbdExpectCalls[msg_origin][i] & DBD_SDCC) != 0))

                           {

                             if (*e2p_debug_enable != 0)
                             {
                              msg_temp[0] = '[';
                              msg_temp[1] =  2;
                              msg_temp[2] = msg_origin;
                              msg_temp[3] = i;
                              msg_temp[4] = asslib_DbdExpectCalls[msg_origin][i];
                              msg_temp[5] = 255;
                              msg_temp[6] = 255;
                              msg_temp[7] = 13;

                              if (!(sio_TxPort(GSV_TOOL_PORT,msg_temp,8)))
                                E_log(DEBUG_TX_ERROR);
                             }

                             asslib_DbdExpectCalls[msg_origin][i] = 0;
                             asslib_DbdExpectCallsError[2]++;
                           }
/* TestInHonma
                             for (j=0; j<=ocss_info[msg_origin].top_pos; j++)
                             {
                               if (((asslib_DbdAssignCalls[msg_origin][i][j] & DBD_SUCC) != 0) ||
                                  ((asslib_DbdAssignCalls[msg_origin][i][j] & DBD_SDCC) != 0))
                               {
                                 asslib_DbdAssignCalls[msg_origin][i][j] = 0;
                                 asslib_DbdExpectCallsError[3]++;
                               }
                             }
*/
                        }
                    }//allsummary
                }//IDL


             }//OpMode ChangeInState

             tll_update(msg_ptr,0);

              /* U S E   U P D A T E D   I N F O */
              /* ------------------------------- */
              update_stopping_info(msg_origin);
              test_lobby_arrival(msg_origin);
              test_lobby_departure(msg_origin);
              test_epbhc_trigger(msg_origin);
              test_approaching_stop(msg_origin);
             /* Test to see if car was available to answer hall calls and is no */
             /* longer avialable to answer hall calls. If so, reassign them.     */
             /*----------------------------------------------------------*/
          if ( ((infrng_carWasAvailableFlags[msg_origin] == asslib_NormalMode)
             || (infrng_carWasAvailableFlags[msg_origin] == asslib_LNSOLDMode)
             || (infrng_carWasAvailableFlags[msg_origin] == asslib_GroupOutMode))
             && (asslib_car_available(msg_origin) == asslib_NotAvailable) )
            {
                // 18-may test :: 
                // Clear all Destination entry calls, then make any remaining calls new...
                asslib_DbdClearAllExpectedCalls(msg_origin);
                asslib_DbdClearAllHallCalls(msg_origin);
                asslib_RemoveDemandCnt++;

                asslib_make_all_calls_new(msg_origin);
                mit_releasePriority(msg_origin,ocss_info[msg_origin].actual_pos,ReleaseGroupOut);
              //OsakaOBP
              if (dpc_EnableFlag == TRUE)
               {
                 dpc_Init();
                 dpc_ClusterRelease(msg_origin);
               }
            }
            /* Perform LNS and OLD check  */
            /*------------------------------------*/
           else if (
                    (asslib_car_available(msg_origin) == asslib_LNSOLDMode) ||
                    (((ocss_info[msg_origin].opmode == MIT)||
                      (ocss_info[msg_origin].opmode == CHN)) &&
                     ((ocss_info[msg_origin].load_state == LOAD4) ||
                     (ocss_info[msg_origin].load_state == LOAD5))
                    )
                   )
             {
                /* just test for mask 7/31 honma  */
                if (*e2p_lnsold_enable == 1)
                   {
                     asslib_CheckLNSOLD(msg_origin);
                   }
             } /* LNS/OLD check */
             else if ( ((infrng_carWasAvailableFlags[msg_origin] == asslib_NormalMode)
              || (infrng_carWasAvailableFlags[msg_origin] == asslib_LNSOLDMode))
              && (asslib_car_available(msg_origin) == asslib_GroupOutMode) )
             {
                switch(ocss_info[msg_origin].opmode)
                 {
                   case WCS:
                            asslib_CheckWCS(msg_origin);
                            break;
                   case DHB:
                   case DOS:
                            asslib_CheckDHBDOS(msg_origin);
                            break;
                   //Aiful
                   case EHS:
                            asslib_CheckEHS(msg_origin);
                            break;
                   case DCP:
                            // DBD : leave car in DCP (don't reassign/remove demand).
                            if ( (asslib_count_normal_cars() >= 1) && (*e2p_DbdAssignEnable == 0) )
                                asslib_make_all_calls_new(msg_origin);
                            break;
                   //Add 13May2003    for Revo System...
                   case WRM:
                   case PKS:
                   case CDO:
                   case REI:
                   case EPS:
                            asslib_CheckSemiGroupOut(msg_origin);
                            break;
                   default:
                            break;
                 }//switch
             }
             //OsakaObp
             else if (((infrng_carWasAvailableFlags[msg_origin] == asslib_GroupOutMode) ||
                      (infrng_carWasAvailableFlags[msg_origin] == asslib_NotAvailable)) &&
                      (asslib_car_available(msg_origin) == asslib_NormalMode))
             {
               //Update for SemiGroupOutMode 30Jun2003   From Aiful Jobsite.
               asslib_CheckForRecoverCar(msg_origin);
               if (dpc_EnableFlag == TRUE)
                  dpc_Init();
             }

            infrng_carWasAvailableFlags[msg_origin] = asslib_car_available(msg_origin);
        break;

   /*--------------------------
   ; Sub-type 1
   ---------------------------*/
   case 1:

 //OsakaObp
      if (ocss_ptr->ring12_T0_tout == 0)
      {
         loc_cig_mask = ocss_car_cnt();
         ocss_trans_car_cnt(loc_cig_mask);
      }
      ocss_ptr->ring12_T0_tout = TIM_2S;

      ring_info[ring_num].send_to_adss = ads_icss2_T0[1];
      ocss_ptr->old_actual_pos = ocss_ptr->actual_pos;
      ocss_ptr->actual_pos = *(msg_ptr + 4) & 0x7f;

      //DBD-honma 07Mar04
      ocss_ptr->f_oldDoorState = ocss_ptr->f_door_state;
      ocss_ptr->r_oldDoorState = ocss_ptr->r_door_state;

      ocss_ptr->f_door_state = *(msg_ptr + 3) & 0x03;
      ocss_ptr->r_door_state = (*(msg_ptr + 3) >> 2) & 0x03;
      ocss_ptr->old_mov_dir = ocss_ptr->mov_dir;
      ocss_ptr->mov_dir = (*(msg_ptr + 3) >> 4) & 0x03;

//dojima
            if ((ocss_ptr->mov_dir != ocss_ptr->old_mov_dir) &&
              (ocss_ptr->mov_dir == NO_DIR))
              {
                ocss_ptr->lastStopPos = ocss_ptr->actual_pos;
              }

            //DBD-honma 17FEB04
            if (ocss_ptr->f_door_state != ocss_ptr->f_oldDoorState)
              {
                switch (ocss_ptr->f_door_state)
                {
                   case DOORS_CLOSED:
                            break;
                   case DOORS_OPENING:
                            break;
                   case DOORS_OPENED:
                            break;
                   case DOORS_CLOSING:


                   if ((*e2p_DbdAssignEnable != 0) && (ocss_ptr->f_oldDoorState == DOORS_OPENED))
                   {
                     carPos = ocss_info[msg_origin].actual_pos;

                     // DBD : If car is leaving the lobby, Remove any sector assignments
                     if ((carPos == bld_info.ocss_lobby_pos) && (ocss_info[msg_origin].pre_dir == UP_DIR))
                     {
                        DBD_removeCarFromSectorAssignments(msg_origin);
                     }

                     if (ocss_info[msg_origin].pre_dir == UP_DIR)
                       {
                         dest_mask = DBD_SUCC;
                         start = carPos+1;
                         finish = ocss_info[msg_origin].top_pos;
                         callDir = UP_DIR;
                         start2 = ocss_info[msg_origin].bottom_pos;
                         finish2 = carPos-1;
                       }
                     else if (ocss_info[msg_origin].pre_dir == DOWN_DIR)
                       {
                         dest_mask = DBD_SDCC;
                         start = ocss_info[msg_origin].bottom_pos;
                         finish = carPos-1;
                         callDir = DOWN_DIR;
                         start2 = ocss_info[msg_origin].top_pos;
                         finish2 = carPos+1;
                       }
                    if (ocss_info[msg_origin].pre_dir == UP_DIR)
                      {
                     for (dstPos = carPos+1; dstPos <= ocss_info[msg_origin].top_pos; dstPos++)
                       {
                         if ((asslib_DbdAssignCalls[msg_origin][carPos][dstPos] & dest_mask) != 0)
                           {
                             if ((asslib_DbdExpectCalls[msg_origin][dstPos] & dest_mask) != 0)
                               {
                                  if ((ocss_info[msg_origin].assigned_calls[dstPos] & S_FCC) != 0)
                                    {
                                   //   if(asslib_ValidExpectCalls2(msg_origin,callDir,ocss_info[msg_origin].bottom_pos,carPos-1,dstPos)==FALSE)
                                          asslib_DbdDeassignCall(msg_origin,dest_mask,carPos,dstPos);
                                    }
                                   else
                                    {
                                          asslib_DbdExpectCallsError[12]++;
                                          asslib_DbdDeassignCall(msg_origin,dest_mask,carPos,dstPos);

                                          if (*e2p_debug_enable != 0)
                                            {
                                              msg_temp[0] = '[';
                                              msg_temp[1] =  3;
                                              msg_temp[2] = msg_origin;
                                              msg_temp[3] = carPos;
                                              msg_temp[4] = dstPos;
                                              msg_temp[5] = 1;
                                              msg_temp[6] = 255;
                                              msg_temp[7] = 13;

                                              if (!(sio_TxPort(GSV_TOOL_PORT,msg_temp,8)))
                                                E_log(DEBUG_TX_ERROR);
                                            }

                                    }
                               }
                              else
                               {
                                asslib_DbdExpectCallsError[7]++;
                                asslib_DbdAssignCalls[msg_origin][carPos][dstPos] &= ~dest_mask;
                               }
                           }
                       }
                      } //UP_DIR
                    else if (ocss_info[msg_origin].pre_dir == DOWN_DIR)
                      {
                     for (dstPos = ocss_info[msg_origin].bottom_pos; dstPos <= (carPos-1); dstPos++)
                       {
                         if ((asslib_DbdAssignCalls[msg_origin][carPos][dstPos] & dest_mask) != 0)
                           {
                             if ((asslib_DbdExpectCalls[msg_origin][dstPos] & dest_mask) != 0)
                               {
                                  if ((ocss_info[msg_origin].assigned_calls[dstPos] & S_FCC) != 0)
                                    {
                                    //  if(asslib_ValidExpectCalls2(msg_origin,callDir,ocss_info[msg_origin].top_pos,carPos+1,dstPos)==FALSE)
                                          asslib_DbdDeassignCall(msg_origin,dest_mask,carPos,dstPos);
                                    }
                                   else
                                    {
                                          asslib_DbdExpectCallsError[13]++;
                                          asslib_DbdDeassignCall(msg_origin,dest_mask,carPos,dstPos);
                                          if (*e2p_debug_enable != 0)
                                            {
                                              msg_temp[0] = '[';
                                              msg_temp[1] =  3;
                                              msg_temp[2] = msg_origin;
                                              msg_temp[3] = carPos;
                                              msg_temp[4] = dstPos;
                                              msg_temp[5] = 2;
                                              msg_temp[6] = 255;
                                              msg_temp[7] = 13;

                                              if (!(sio_TxPort(GSV_TOOL_PORT,msg_temp,8)))
                                                E_log(DEBUG_TX_ERROR);
                                            }
                                    }
                               }
                              else
                               {
                                asslib_DbdExpectCallsError[8]++;
                                asslib_DbdAssignCalls[msg_origin][carPos][dstPos] &= ~dest_mask;
                               }
                           }
                       }
                      } //DOWN_DIR

                     }//if ((*e2p_DbdAssignEnable != 0) && (ocss_ptr->f_oldDoorState == DOORS_OPENED))

                            break;
                   default:
                            break;
                }
              }




      ocss_ptr->motion_status = *(msg_ptr + 1) & 0x03;

      /* S E T   E X P R E S S   Z O N E   F L A G */
      /* ----------------------------------------- */
      if ( *(msg_ptr + 3) & 0x80)
      {
         ocss_ptr->car_in_ez_flag = TRUE;
      }
      else
      {
         ocss_ptr->car_in_ez_flag = FALSE;
      }


      /* set decel flag */
      if (ocss_ptr->motion_status == 2)
      {
         ocss_ptr->decel_flag = TRUE;
      }
      else
      {
         ocss_ptr->decel_flag = FALSE;
      }


      /* S E T   C A L L S   E N A B L E D   B I T S */
      /* ------------------------------------------- */
      ocss_ptr->call_allowed_mask = *(msg_ptr + 2) & 0x7f;
      /* REHC is in next byte of message */
      ocss_ptr->call_allowed_mask |= ( *(msg_ptr + 3) & 0x40) << 1;


      /* U S E   U P D A T E D   I N F O */
      /* ------------------------------- */
      update_stopping_info(msg_origin);

      test_lobby_arrival(msg_origin);
      test_lobby_departure(msg_origin);

      test_epbhc_trigger(msg_origin);

   break;

   /*--------------------------
   ; Sub-type 2
   ---------------------------*/
   case 2:
      ring_info[ring_num].send_to_adss = ads_icss2_T0[2];
   break;

   /*---------------------------
   ; Sub-type 3
   ----------------------------*/
   case 3:
      ring_info[ring_num].send_to_adss = ads_icss2_T0[3];
   break;

   /*---------------------------
   ; default
   ---------------------------*/
   default:
      E_log(BAD_CASE);
   break;
      } /* switch (subtype) */
   } /* msg from valid car ID */
}


/*********************************************************************
; Function Name: process_msg
;
; Description: This function decodes messages based on message type.
;
; Parameters:
; Message Pointer:      (input)  Pointer to the message to be decoded.
; Ring Number:          (input)  Ring through which message was received.
;********************************************************************/
void process_msg (BYTE_T *msg_ptr, BYTE_T ring_num)

{

   BYTE_T msg_origin;

   ring_info[ring_num].tout = TIM_2S;

   switch(M_get_type(msg_ptr))
   {
      /*----------------------------------------------
      ; Type 0 - Status Message
      ------------------------------------------------*/
      case 0:
         /* Determine message origin */
         msg_origin = M_get_origin(msg_ptr);

         /* If message originated from a car, handle specially */
         if (msg_origin >= CAR1 && msg_origin <= CAR8)
         {

            ring_tout(msg_ptr, ring_num); /* Update car timeout */

            /* If message is from source ring for car, process it. */

            if (ocss_info[msg_origin].source_ring == ring_num)
            {
               status_msg(msg_ptr, ring_num);
            }
         }
         else /* non-car type 0 status msg */
         {
            status_msg(msg_ptr, ring_num);
         }
         break;


      /*------------------------------------------------
      ; default - Log error and continue
      -------------------------------------------------*/
      default:
         E_log(BAD_CASE);
         break;

   }  /* case */
}  /* process_msg */

/*********************************************************************
; Function Name: ring_proc
;
; Description: This function will process ring messages based on message
;              type, subtype, and origin for a given ring.
;
; Parameters:
; Message Pointer       (Input) - Pointer to incoming message buffer.
; Ring Number           (Input) - Ring number.
; Ring Queue Pointer    (Input) - Pointer to appropriate ring buffer
;********************************************************************/
void ring_proc(BYTE_T *ring_msg, BYTE_T ring_num, QUEUE_T *ring_q_ptr)
{
   BYTE_T       msg_count;
   FLAG_T       empty_buffer;
   RING_T       *ring_ptr;

   /*-------------------------------------------
      ; Do some initialization
      --------------------------------------------*/
   msg_count = 0;
   empty_buffer = FALSE;
   ring_ptr = &ring_info[ring_num];

   /*---------------------------------------------------------------------
      ; Attempt first read
      ;     - if read fails (buffer empty), exit ring_proc.
      ---------------------------------------------------------------------*/
   if (!(Q_Get(ring_q_ptr,ring_msg,MSG_LEN)))
   {
      empty_buffer = TRUE;
   }


   /*---------------------------------------------------------------------
      ; Process up to 160 ring messages......
      ---------------------------------------------------------------*/
   while ( (msg_count < MAX_PROC_MESSAGES) && (!empty_buffer) )
   {

      /*---------------------------------------------------------
         ; Assume message will not be sent to the ADSS/PU
         ----------------------------------------------------------*/
      ring_ptr->send_to_adss = FALSE;
      ring_ptr->send_to_adss_origin = FALSE;

      /*---------------------------------------------------------
         ; Process message based on type and origin.
         ----------------------------------------------------------*/
      process_msg(ring_msg,ring_num);
      process_origin(ring_msg,ring_num);

      /*----------------------------------------------------------
         ; 1. Transmit to ADSS/PU if:
         ;             - the send_to_adss flag is set
         ;             - adss communications has been established
         ;        - message has past origin test
         ----------------------------------------------------------*/
      if  ((ring_ptr->send_to_adss) && (icss_info[ADSS_PU_ORIGIN].alive)  &&
           (ring_ptr->send_to_adss_origin))
      {

         if (!(Q_Put(ads_q_ptr,ring_msg,MSG_LEN)))
         {
//            E_log(Q_WRITE);
            E_log(Q_WRITE_ADS);
         }

      }

      /*-----------------------------------------------------------------
         ; Attempt to perform the next read.
         ;    - increment msg_count
         ;    - if MAX_PROC_MESSAGES have been read, break out of message
         ;      processing loop.
         ;    - if read is unsucessful, brake out of message processing loop.
         ;    - if read is sucessful, process new message.
         -------------------------------------------------------------------*/
      msg_count++;
      if (msg_count < MAX_PROC_MESSAGES)
      {
         if (!(Q_Get(ring_q_ptr, ring_msg, MSG_LEN)))
         {
            empty_buffer = TRUE;
         }
      }
   }//while

   infrng3 = msg_count;
}

/*********************************************************************
; Function Name: send_cancel_grp_call
;
; Issues a Type 2 Subtype 2 message to cancel call from all cars in
; group.
;
; Parameters: landing  - landing where call shall be canceled.
;             callmask - type of call to cancel.
;             grp_no   - group number
;********************************************************************/
void send_cancel_grp_call (BYTE_T landing, INT_T callmask,BYTE_T grp_no)

{

BYTE_T message[6],tmpGrpNo,car;

//SGO
//update 00/12/08
    if (infbld_Sgo1EnableFlag == TRUE) tmpGrpNo = grp_no + 1;
    else                              tmpGrpNo = grp_no;

//SRO for Cyuou-Mitsu  00/10/19
    if (infbld_Sro1EnableFlag == TRUE)
      {
        //RISER_TWO is SCX car's meaning.
        if (grp_no == RISER_ONE)
        {
          if ((landing == infbld_SroLowPos) || (landing == infbld_SroHighPos))
             tmpGrpNo = RISER_ONE + 1;
          else
             tmpGrpNo = 0;  //It's for general floor,not SeparateRiser Floor.
        }
      }

    /* build up cancel call message to clear call from all subsystems */
    message[0] = 0xac; /* type 2, from icss2 */
    message[1] = 0x80; /* subtype 0, to all cars */

// update for Australia 99/9/27 Honma
// Initialize
    message[2] = 0x80;
    message[3] = 0x80;

//update  if (dup_enable_flag == FALSE)
  if (1) //update 24Jan01
  {
    /* Define message contents, based on call type. */
    /* -------------------------------------------- */
    if (callmask == S_FUHC)
    {
       message[2] = 0x82; /* front up hall call */
       bld_info.fuhc[grp_no][landing].dispatch = FALSE;
       bld_info.fuhc[grp_no][landing].reg_elapsed = 255;
       //DBD 06Apr04 honma
       if (*e2p_DbdAssignEnable != 0)
       for (car=CAR1;car<=CAR8;car++)
        bld_info.fuhc[grp_no][landing].reg_elapsed_car[car] = 255;
    }
//update for Australia 99/9/27 Honma
    else if (callmask == S_FDHC)
    {
       message[2] = 0x84; /* front down hall call */
       bld_info.fdhc[grp_no][landing].dispatch = FALSE;
       bld_info.fdhc[grp_no][landing].reg_elapsed = 255;
       //DBD 06Apr04 honma
       if (*e2p_DbdAssignEnable != 0)
       for (car=CAR1;car<=CAR8;car++)
        bld_info.fdhc[grp_no][landing].reg_elapsed_car[car] = 255;
    }
//update for Australia 99/9/27 Honma
    else if (callmask == S_RUHC)
    {
       message[3] = 0x82; /* rear up hall call */
       bld_info.ruhc[grp_no][landing].dispatch = FALSE;
       bld_info.ruhc[grp_no][landing].reg_elapsed = 255;
    }
//update for Australia 99/9/27 Honma
    else if (callmask == S_RDHC)
    {
       message[3] = 0x84; /* rear up hall call */
       bld_info.rdhc[grp_no][landing].dispatch = FALSE;
       bld_info.rdhc[grp_no][landing].reg_elapsed = 255;
    }

    bld_info.new_hc_mask[grp_no][landing] &= ~callmask;
  }//if (dup_enable_flag == FALSE

    message[3] = message[3] | (tmpGrpNo << 4); /* Destination Group */
    message[4] = 0x80 | landing;
    message[5] = M_checksum(message,MSG_LEN);

    /*----------------------------------------
    ; Transmit the message on both rings
    ------------------------------------------*/
    sio_TxRing(message, MSG_LEN, RING1);

}

void test_approaching_stop(BYTE_T car)
{

}

void  test_epbhc_trigger(BYTE_T car)
{
}

void  test_lobby_departure(BYTE_T car)
{
}

void  test_lobby_arrival(BYTE_T car)
{
}

void  update_stopping_info(BYTE_T car)
{
}

void  tll_update (BYTE_T *msg_ptr, BYTE_T msg_type)
{
}

void  ocss_trans_car_cnt(BYTE_T loc_cig_mask)
{
}

void  process_origin (BYTE_T *msg_ptr, BYTE_T ring_num)
{

}

/*********************************************************************
; Function Name: ic_update_msg          MESSAGE TYPE 5
;
; Description: This function decodes the information control parameter
;              update message and extracts any necessary information.
;
; Parameters:
; 1. Message Pointer    (input)  Pointer to the first byte of the ring msg
;
;********************************************************************/
void ic_update_msg(BYTE_T *msg_ptr, BYTE_T ring_num)

{

   BYTE_T       msg_dest;
   BYTE_T       msg_parameter_num;
   BYTE_T               msg_origin;
   BYTE_T               msg_id;
   INT_T                msg_data,msg_data2;
   BYTE_T   i,j,k;
   INT_T    peopleWeight,halfWeight;

   /*----------------------------------------------------------
   ; Extract message destination to determine if message should
   ; be transmitted to the adss/pu
   ;-----------------------------------------------------------*/
   msg_dest = (*(msg_ptr + 1) & 0x0f);
   if ((msg_dest == ADSS_PU_ORIGIN) || (msg_dest == BROADCAST))
   {
      ring_info[ring_num].send_to_adss = TRUE;
   }
   else
   {
      ring_info[ring_num].send_to_adss = FALSE;
   }

   /*-----------------------------
   ; Extract the parameter number
   ;------------------------------*/
   msg_parameter_num = (*(msg_ptr + 2) & 0x7f);

   /*---------------------------------
   ; Extract ID (car = 1, group = 0)
   ;---------------------------------*/
   msg_id = ((*(msg_ptr + 1) & 0x40) >> 6);

   /*-----------------------------
   ; Extract the data from message
   ;------------------------------*/
   msg_data = (((*(msg_ptr + 1) & 0x20) << 10) |
                       ((*(msg_ptr + 4) & 0x7f) << 8)  |
                       ((*(msg_ptr + 1) & 0x10) << 3)  |
                        (*(msg_ptr + 3) & 0x7f));

   /* Parameter 62 is Crowd Floor Service Position */
   /*---------------------------------------------------*/
   if (msg_parameter_num == 62)
   {
      msg_data2 = (*(msg_ptr + 4) & 0x07);

      msg_data = (((*(msg_ptr + 1) & 0x20) << 10) |
/*                    ((*(msg_ptr + 4) & 0x7f) << 8)    | */
                         ((*(msg_ptr + 1) & 0x10) << 3)   |
                          (*(msg_ptr + 3) & 0x7f));
   }
   msg_origin = *(msg_ptr) & 0x0f;

   /*-----------------------------------------------------------------
   ; If the destination is ICSS2 and it is a car related parameter,
   ; case on the parameter number and store the information.
   ;-----------------------------------------------------------------*/
   if ( ((msg_dest == ICSS2_ORIGIN)||(msg_dest == BROADCAST)) && (msg_id == 1) )
   {
      switch(msg_parameter_num)
      {
         /*---------------------------
         ; Parameter 48 - Car Capacity
         ----------------------------*/
         case 48:
            if ((msg_origin >= CAR1) && (msg_origin <= CAR8))
            {
               ocss_info[msg_origin].car_capacity = msg_data;

               //DBD-honma spcap 23Mar04
               if (*e2p_trafdatPassengerWeight !=0)
                  peopleWeight = *e2p_trafdatPassengerWeight;
               else
                  peopleWeight = 65;

                  halfWeight = peopleWeight/2;

               if (msg_data >= peopleWeight)
                 {
                  ocss_info[msg_origin].carMaxPassenger = msg_data/peopleWeight;
                  ocss_info[msg_origin].carEffPassenger = msg_data/peopleWeight;
                 }
               else
                 {
                  ocss_info[msg_origin].carMaxPassenger = 22;
                  ocss_info[msg_origin].carEffPassenger = 22;
                 }


            }
            break;

         /*-----------------------------
         ; Parameter 49 - Lobby Landing
         ------------------------------*/
         case 49:
            if (msg_origin >= CAR1 && msg_origin <= CAR8)
            {
               ocss_info[msg_origin].lobby_pos = msg_data;
               bld_info.ocss_lobby_pos = msg_data;
            }
            else if (msg_origin == ADSS_PU_ORIGIN)
            {
               bld_info.ads_lobby_pos = msg_data;
            }
            break;

         /*------------------------------
         ; Parameter 50 - Bottom Landing
         -------------------------------*/
         case 50:
            if (msg_origin >= CAR1 && msg_origin <= CAR8)
            {
               ocss_info[msg_origin].bottom_pos = msg_data;
               bld_info.bottom_pos = min(bld_info.bottom_pos, msg_data);
            }
            break;

         /*---------------------------
         ; Parameter 51 - Top Landing
         ----------------------------*/
         case 51:
            if (msg_origin >= CAR1 && msg_origin <= CAR8)
            {

             //update for Australia JobSite, 00/09/28
               if (msg_data >= JICA_FLOORS)
                E_log(TopPosError);

               ocss_info[msg_origin].top_pos = msg_data;
               bld_info.top_pos = max(bld_info.top_pos, msg_data);
            }
            break;

            /*---------------------------------
               ; Parameter 52 - EZ bottom landing
               ----------------------------------*/
         case 52:
            if ( (msg_origin >= CAR1) && (msg_origin <= CAR8) )
            {
               ocss_info[msg_origin].ez_bottom_pos = msg_data;
               if ( (ocss_info[msg_origin].ez_top_pos < INVALID_POSITION) &&
                   (ocss_info[msg_origin].ez_bottom_pos < INVALID_POSITION) &&
                   (ocss_info[msg_origin].ez_top_pos >
                      ocss_info[msg_origin].ez_bottom_pos) &&

                   (ocss_info[msg_origin].top_pos >=
                      ocss_info[msg_origin].ez_top_pos) &&

                   (ocss_info[msg_origin].bottom_pos <=
                      ocss_info[msg_origin].ez_bottom_pos) )
               {
                  ocss_info[msg_origin].ez_type = 1;
               }
               else
               {
                  ocss_info[msg_origin].ez_type = 0;
               }
            }
            break;

           /*------------------------------
           ; Parameter 53 - EZ top landing
           -------------------------------*/
         case 53:
            if ( (msg_origin >= CAR1) && (msg_origin <= CAR8) )
            {
               ocss_info[msg_origin].ez_top_pos = msg_data;
               if ( (ocss_info[msg_origin].ez_top_pos < INVALID_POSITION) &&
                   (ocss_info[msg_origin].ez_bottom_pos < INVALID_POSITION) &&
                   (ocss_info[msg_origin].top_pos >= ocss_info[msg_origin].ez_top_pos) &&
                   (ocss_info[msg_origin].bottom_pos <= ocss_info[msg_origin].ez_bottom_pos) &&
                   (ocss_info[msg_origin].ez_top_pos > ocss_info[msg_origin].ez_bottom_pos) )
               {
                  ocss_info[msg_origin].ez_type = 1;
               }
               else
               {
                  ocss_info[msg_origin].ez_type = 0;
               }
            }
            break;

         /*---------------------------
         ; Parameter 54 - EZ run time
         ----------------------------*/
         case 54:
            if ( (msg_origin >= CAR1) && (msg_origin <= CAR8) )
            {
               ocss_info[msg_origin].ez_runtime = msg_data;
            }
            break;

         /*---------------------------
         ; Parameter 56 - target floor
         ----------------------------*/
         case 56:
            if ( (msg_origin >= CAR1) && (msg_origin <= CAR8) )
            {
               ocss_info[msg_origin].target_pos = msg_data;

               //update 99/2/16
               park_PreTarget[msg_origin] = ocss_info[msg_origin].target_pos;

                 test_approaching_stop(msg_origin);
               //kidsPark/Atugi update for MultiLobby Function 1997/11/13
               if ((ocss_2nd_lobby_flag == TRUE) &&
                 (ocss_2nd_lobby_type == MlbVariable))
                 {
                   park_TargetEvent(msg_origin);
                 }
            }
            break;

         /*---------------------------------
         ; Parameter 58 - 2nd Lobby Landing
         -----------------------------------*/
         case 58:
            if ( (msg_origin >= CAR1) && (msg_origin <= CAR8) )
            {

               /* Process invalid second lobby value */
                 /*-----------------------------------------*/
                 if ((msg_data > bld_info.top_pos) ||
                      (msg_data == bld_info.ocss_lobby_pos) )
                 {
                    ocss_info[msg_origin].second_lobby_pos = 255;
                    if (ocss_2nd_lobby_type != 0)
                    {
                       ocss_2nd_lobby_type = NoMlb;
                       for (i=CAR1;i<=CAR8;i++)
                       {
                          if ((asslib_car_available(i)!=asslib_NotAvailable) &&
                               (ocss_info[i].second_lobby_pos != 255))
                          {
                             /* Fixed Multi Lobby Type */
                             //update 1997/11/27
                             ocss_2nd_lobby_type = MlbFixed;
                          }
                       }
                    }
                 }

                 /* Accept second lobby definition  */
                 /*--------------------------------------*/
                 else if (bld_info.second_lobby_pos >= msg_data)
                 {
                    bld_info.second_lobby_pos = msg_data;
                    ocss_info[msg_origin].second_lobby_pos = msg_data;
                    /* Variable Multi Lobby Type */
                    ocss_2nd_lobby_type = MlbVariable;
                 }

                 /* Else..... reset second lobby position  */
                 /*--------------------------------------------*/
                 else
                 {
                    ocss_info[msg_origin].second_lobby_pos = 255;
                 }

                 if (ocss_2nd_lobby_type == MlbVariable)
                 {
                    for (i=CAR1;i<=CAR8;i++)
                    {
                       debug_second = i;
                       debug_second = ocss_info[i].opmode;

                       if ((asslib_car_available(i)!=asslib_NotAvailable) &&
                            (ocss_info[i].second_lobby_pos == 255))
                       {
                          /* Fixed Multi Lobby Type */
                          ocss_2nd_lobby_type = MlbFixed;
                       }
                    }
                 }
              }
            break;

         /*---------------------------
         ; Parameter 59 - Reopen Count
         ----------------------------*/
         case 59:
            if ( (msg_origin >= CAR1) && (msg_origin <= CAR8) )
            {
               ocss_info[msg_origin].reopen_cnt = msg_data;
            }
            break;


         /*---------------------------------------
         ; Parameter 61 - DUPC Top Common Position
         -----------------------------------------*/
         case 61:
            if (msg_origin >= CAR1 && msg_origin <= CAR8)
            {
              // Update 24Feb03  HigashiShinagawa  ...
              // bld_info.dup_common_pos = msg_data;
                 if (msg_data <= bld_info.top_pos)
                 {
                    bld_info.dup_common_pos = max(bld_info.ocss_lobby_pos, msg_data);
                 }
                 else
                    bld_info.dup_common_pos = bld_info.ocss_lobby_pos;
            }
            break;

         /*---------------------------------------
         ; Parameter 62 - Crowd Floor Service Position (related EMS)
         -----------------------------------------*/
         case 62:
            if (msg_origin >= CAR1 && msg_origin <= CAR8)
            {
                 if (msg_data2 <= 4)
                 {
                    if (msg_data <= bld_info.top_pos)
                    {
                     OCSS_Crowd_Floor[msg_data2+1] = (INT_T)msg_data;
                    }
                    else
                     OCSS_Crowd_Floor[msg_data2+1] = 255;
                 }
            }
            break;

#ifdef U10
         /*---------------------------
         ; Parameter 63 - assign nn weights
         ----------------------------*/
         case 63:
            assign_nn_weights(msg_data);
            break;
#endif

         /*--------------------------
         ; default - do nothing.
         ---------------------------*/
         default:
            break;
      }
   }
}

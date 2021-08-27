#include "stdafx.h"
#include "common.h"

#define EXTERN extern
#include "infocs.h"
#include "infbld.h"
#include "infads.h"
#include "rrt.h"
#include "Asslib.h"
#undef EXTERN

#define EXTERN
#include "crisprrt.h"

#undef EXTERN

/*********************************************************************
; Function Name: crisprrt_calculate_rrt
;
; Calculates the RRT value for a particular hall call, for each car. 
;
;
; Parameters: car           - car ID
;             pos           - landing where call has been entered
;             mask          - type of call (hall call only)
;             pre_dir       - predirection of car
;             coinc_type    - set to coincident call type
;	      distance      - distance
;
;********************************************************************/
INT_T crisprrt_calculate_rrt(BYTE_T car, BYTE_T pos, BYTE_T dstPos, INT_T mask, BYTE_T pre_dir, 
                      FLAG_T *coinc_cc_flag, BYTE_T *coinc_type, BYTE_T *distance,BYTE_T *distance2,BYTE_T *spareCap)
{
	INT_T  rrt;
	FLAG_T stopping_flag, stopping_hall_call_flag;
	FLAG_T temp_bottom_cc_flag, temp_top_cc_flag;
	INT_T  highest_call_pos, lowest_call_pos;
	FLAG_T highest_call_rev_flag, lowest_call_rev_flag;
	INT_T  calls_dir;
	FLAG_T ez_running_flag;
	BYTE_T local_NCF,i;
	FLAG_T tempflag1, tempflag2;
	INT_T  e_mask;
	
    /* I N I T I A L I Z E   F L A G S */
    /* ------------------------------- */
	
	*distance = 0;
	*distance2 = 0;
	
	//DBD
	*coinc_cc_flag = FALSE;
	*coinc_type = 0;


    /* D E F I N E   L O C A L _ N E X T _ C O M M */
    /* ------------------------------------------- */
    local_NCF = ocss_info[car].NCF;
	/*jsr 
	아마도 ocss 와의 통신 Delay를 고려하여 NCF에 조금더 여유를 줌
	*/
    /* If car is moving downwards, determine if next commitable position */
    /* should be decremented to ensure that car can stop at landing.     */
    /* ----------------------------------------------------------------- */
    if (ocss_info[car].mov_dir == DOWN_DIR)
    {
       /* for eliminate passing dispatch H.honma */   
		//modified for a while
        if (((ocss_info[car].actual_pos - ocss_info[car].NCF) >= 2) &&
             (ocss_info[car].target_pos < ocss_info[car].NCF) ) 
             local_NCF--;

    } /* mov_dir == DOWN_DIR */
    /* If car is moving upwards, determine if next commitable position */
    /* should be incremented to ensure that car can stop at landing.   */
    /* --------------------------------------------------------------- */
    else if (ocss_info[car].mov_dir == UP_DIR)
    {
        /* for eliminate passing dispatch H.honma  */
        if (((ocss_info[car].NCF - ocss_info[car].actual_pos) >= 2) &&
             (ocss_info[car].target_pos > ocss_info[car].NCF) )
             local_NCF++;

    } /* mov_dir == UP_DIR */
       

       
    /*  I N I T I A L I Z E   S T O P P I N G   F L A G  */
    /*  -----------------------------------------------  */
    /* Set stopping flag.  The stopping flag is true IF  */
    /* Front and rear doors are closed, AND              */
    /* the stopping position is the next commitable, AND */
    /* the stopping call mask is not zero or there are   */
    /* calls pending at the stopping position)           */ 
    /*  -----------------------------------------------  */
    stopping_flag = FALSE;
/* jsr
	문이 닫혀 있고 local_NCF 가 현재 위치와 다르면
	
*/
	//jsr stopping flag 세팅
    if ( (ocss_info[car].f_door_state == DOORS_CLOSED) || 
         ((ocss_info[car].f_door_state == DOORS_CLOSING) &&
         (local_NCF != ocss_info[car].actual_pos)) ) 
       { 
         /* if calls are or were pending, set flag */
		   /*jsr
			현재 멈춰 있고 call이 있으면
		   */
         if ( ( (ocss_info[car].stop_pos == local_NCF) &&
              (ocss_info[car].stop_type != 0) )||
            ( (ocss_info[car].target_pos == local_NCF) &&
              (ocss_info[car].assigned_calls[local_NCF] != 0) ) )
            {
                stopping_flag = TRUE;
            } /* calls are or were pending */
       } /* car doors closed */
    
    /* I N I T I A L I Z E   R R T */
    /* --------------------------- */
    rrt = RSR_MIN;  //relative system response

    /* I N I T I A L I Z E   C A L L   D I R E C T I O N */
    /* ------------------------------------------------- */
    /* Based on direction of call, define call direction */
    /* ------------------------------------------------- */
    if ( (mask & S_UHC) != 0)
    {
       calls_dir = UP_DIR;
    }
    else
    {
       calls_dir = DOWN_DIR;
    }
    
    /* U P D A T E   S T O P P I N G _ H A L L _ C A L L _ F L A G */ 
    /*---------------------------------------------------------------------*/
    stopping_hall_call_flag = FALSE;


    //DBD-honma spcap  22Mar04
    //If the cas has the hall call already.
    /*
    if ((ocss_info[car].assigned_calls[pos] & mask) != 0)
    {
       *spareCap += asslib_DbdAssignCallDirCnts[car][calls_dir][pos];
    }
   */

    /* Calls/Hall Calls added extend call h.honma */                                               
    /*--------------------------------------------------------------------------*/
    if ((mask & S_FUHC) != 0)      e_mask = E_FUHC + E_FCC;
    else if ((mask & S_FDHC) != 0) e_mask = E_FDHC + E_FCC;

    if (((ocss_info[car].f_door_state == DOORS_CLOSED) || 
         (ocss_info[car].f_door_state == DOORS_CLOSING)) &&
         (ocss_info[car].target_pos == local_NCF) &&
       (((ocss_info[car].assigned_calls[local_NCF] & (e_mask + mask + S_CC)) != 0) ||
        ((ocss_info[car].stop_type & (e_mask + mask + S_CC)) != 0)))
        {
			/*jsr
			닫혀 있고 target_pos이 local_NCF 이면서 call 이 있으면

			*/
           stopping_hall_call_flag = TRUE;
        }
    
       /* B E G I N   R R T   P R O C E S S I N G */
       /* --------------------------------------- */

       
       /* If the car is in a valid EZ with dummy floors, */
       /* set the EZ flag true.                          */
       /* ---------------------------------------------- */
       ez_running_flag = FALSE;

       if( (ocss_info[car].ez_type != 0) &&
           (ocss_info[car].ez_bottom_pos >= ocss_info[car].bottom_pos) &&
           (ocss_info[car].ez_bottom_pos < ocss_info[car].top_pos) &&
           (ocss_info[car].ez_top_pos <= ocss_info[car].top_pos) &&
           (ocss_info[car].ez_top_pos > ocss_info[car].bottom_pos) &&
           (ocss_info[car].ez_bottom_pos <= ocss_info[car].ez_top_pos) )
          {
			  /*jsr
				express Zone안에 있으면

			  */
             if ( (local_NCF <= ocss_info[car].ez_top_pos) &&
                  (local_NCF >= ocss_info[car].ez_bottom_pos) )
                     ez_running_flag = TRUE;
          }

       /* Calculate RRT for a car traveling DOWN, REVERSING   */
       /* direction, and traveling UP to the hall call floor. */
       /* --------------------------------------------------- */
       if ( (calls_dir == UP_DIR) && (pre_dir == DOWN_DIR) )
       {

             //DBDSP
             lowest_call_pos = ads_LowestPosition(car,local_NCF,pos,TRUE);
     
             if (lowest_call_pos != pos)  
                lowest_call_rev_flag = TRUE;
             else 
             { 
                lowest_call_rev_flag = FALSE;
                car_call_pred[pos][DOWN_DIR-1]=FALSE;
             }
          
             /*--------------------------------------------------------------*/
             //DBDSP
             rrt += rrt_run_simulation(car,
                                       local_NCF,
                                       lowest_call_pos,
                                       (S_DHC + S_CC + E_CC),
                                       stopping_flag,
                                       lowest_call_rev_flag,
                                       distance,distance2,TRUE,
                                       FALSE,spareCap);  //spcap
          
          
             /*----------------------------------------------------------------*/
             rrt += rrt_run_simulation(car,
                                       lowest_call_pos,
                                       pos,
                                       S_UHC,
                                       TRUE,
                                       FALSE,
                                       distance,distance2,FALSE,
                                       1,spareCap); //spcap
             //DBD  Coincident Check
             if ((asslib_DbdExpectCalls[car][pos] & DBD_SUCC) != 0)
               {
                 *coinc_cc_flag = TRUE;
                 *coinc_type |= CC_HCwithDCC;
               }

             //DBD 30Jan04  Honma
             //Check if the hallcall's destination have coincident car calls. 
             // 10Feb04
             if (((asslib_DbdExpectCalls[car][dstPos] & DBD_SUCC) != 0) ||
                 ((ocss_info[car].assigned_calls[dstPos] & S_UHC) != 0))
               { 
                 *coinc_cc_flag = FALSE;
                 *coinc_type |= CC_DCCwithDCC;

                 if ((ocss_info[car].assigned_calls[dstPos] & S_UHC) != 0)
                   {
                    *coinc_cc_flag = FALSE; // shs : no need to specify because upper part alread set it
                    *coinc_type |= CC_DCCwithHC;
                   }
               }

             /* Calculate Express Zone run time if car is in express */
             /* zone with no demand                                  */
             /* -----------------------------------------------------*/
             if ((pos > local_NCF) && (lowest_call_pos == local_NCF)
                && (ez_running_flag == TRUE))
               { 
                  rrt += 2*(local_NCF - ocss_info[car].ez_bottom_pos);
                  //dojima
                  rrt += RRT_SHORT_RUN;
               }
       } /* RRT for DOWN, REVERSAL, UP */
       
       
       /* Else, calculate RRT for a car traveling UP, REVERSING */
       /* direction, and traveling DOWN to the hall call floor. */
       /* ----------------------------------------------------- */
       else if ( (calls_dir == DOWN_DIR) && (pre_dir == UP_DIR) ) 
       {

             highest_call_pos = ads_HighestPosition(car,local_NCF,pos,TRUE);
       
             if (highest_call_pos != pos)  
                highest_call_rev_flag = TRUE;
             else
             { 
               highest_call_rev_flag = FALSE;
               car_call_pred[pos][UP_DIR-1]=FALSE;
             }
          
             /*-----------------------------------------------------------------*/
             //DBDSP
             rrt += rrt_run_simulation(car,
                                       local_NCF,
                                       highest_call_pos,
                                       (S_UHC + S_CC + E_CC),
                                       stopping_flag,
                                       highest_call_rev_flag,
                                       distance,distance2,TRUE,
                                       FALSE,spareCap); //spcap
            
             rrt += rrt_run_simulation(car,
                                       highest_call_pos,
                                       pos,
                                       S_DHC,
                                       TRUE,
                                       FALSE,
                                       distance,distance2,FALSE,
                                       1,spareCap); //spcap

             //DBD  Coincident Check
             if ((asslib_DbdExpectCalls[car][pos] & DBD_SDCC) != 0)
               {
                 *coinc_cc_flag = TRUE;
                 *coinc_type |= CC_HCwithDCC;
               }

             //DBD 30Jan04  Honma
             //Check if the hallcall's destination have coincident car calls. 
             // 10Feb04
             if (((asslib_DbdExpectCalls[car][dstPos] & DBD_SDCC) != 0) || 
                 ((ocss_info[car].assigned_calls[dstPos] & S_DHC) != 0))
               { 
                 *coinc_cc_flag = FALSE;
                 *coinc_type |= CC_DCCwithDCC;

                 if ((ocss_info[car].assigned_calls[dstPos] & S_DHC) != 0)
                   {
                    *coinc_cc_flag = FALSE; // shs_redundant
                    *coinc_type |= CC_DCCwithHC;
                   }
               }

             /* Calculate Express Zone run time if car is in express */
             /* zone with no demand                                  */
             /* -----------------------------------------------------*/
             if ((pos < local_NCF) && (highest_call_pos == local_NCF)
                && (ez_running_flag == TRUE))
               { 
                  rrt += 2*(ocss_info[car].ez_top_pos - local_NCF);
                  //dojima
                  rrt += RRT_SHORT_RUN;
               }
  
       } /* RRT for UP, REVERSAL, DOWN */
       
       
       /* Else, the car is traveling in the direction of the hall */
       /* call.  There are two possible reasons:  1) the call is  */
       /* above the car, so no direction reversal is required;    */
       /* 2) the call is below the car, requiring the car to      */
       /* travel around the whole building.                       */
       /* ------------------------------------------------------- */
       else if ( (calls_dir == UP_DIR) && (pre_dir == UP_DIR) )
       {

         /* Tempflag1 is TRUE if doors are closed or closing. */
         /* ------------------------------------------------- */
         if (((ocss_info[car].f_door_state == DOORS_CLOSING) ||
            (ocss_info[car].f_door_state == DOORS_CLOSED)) &&
            ((ocss_info[car].r_door_state == DOORS_CLOSING) ||
            (ocss_info[car].r_door_state == DOORS_CLOSED)))
           {
             if ((ocss_info[car].reopen_cnt!=255) &&
                 (ocss_info[car].nudging_detect==FALSE) &&
                 (ocss_info[car].f_door_state==DOORS_CLOSING))
               {// reopen에 의해 Closing 하고 있다.
                  tempflag1 = FALSE;  // shs tempflag1-> closing_flag
               }
            else
                  tempflag1 = TRUE;
           }
		   //jsr 열려 있거나 nudging중이면
         else
           {
              if ((ocss_info[car].reopen_cnt != 255)&&
                 (ocss_info[car].nudging_detect==TRUE))
                tempflag1 = TRUE;
              else
                tempflag1 = FALSE;
           }
  
  
         /* Tempflag2 is TRUE if...                       */
         /* (car moving and call not assigned already) OR */
         /* --------------------------------------------- */
		 // shs_below block can be specified different way 
		 // tempflag2 = !stopping_flag;
		 // and tempflag2 is not good variable name -->moving_flag
         if (stopping_flag == FALSE) 
           {
              tempflag2 = TRUE; 
           } 
         else
           {
              tempflag2 = FALSE;
           }
    
         /* If the position is greater than the next commitable, OR */
         /* ( 1)  The position is equal to the next commitable, AND */
         /*   2)  The car has demand pending, AND                   */
         /*   3)  car doors are closing or closed, AND              */
         /*   4)  (car is moving AND no calls assigned) OR          */
         /* ------------------------------------------------------- */
         if ((pos < local_NCF) ||
             (pos == local_NCF && ocss_info[car].allsummary != 0 &&
             tempflag1 == TRUE &&
             tempflag2 == TRUE) )
           {
              highest_call_pos = ads_HighestPosition(car,local_NCF,pos,TRUE);
              /*
              if (highest_call_pos != pos)  highest_call_rev_flag = TRUE;
              else                          highest_call_rev_flag = FALSE;
              */

              //DBDSP
              rrt += rrt_run_simulation(car,
                                        local_NCF,
                                        highest_call_pos,
                                        (S_UHC + S_CC + E_CC),
                                        stopping_flag,
                                        TRUE,
                                        distance,distance2,TRUE,
                                        FALSE,spareCap); //spcap

              lowest_call_pos = ads_LowestPosition(car,local_NCF,pos,FALSE);
       

              if (lowest_call_pos != pos)  lowest_call_rev_flag = TRUE;
              else
                { 
                        lowest_call_rev_flag = FALSE;
                        car_call_pred[pos][DOWN_DIR-1]=FALSE;
                }

              rrt += rrt_run_simulation(car,
                                        highest_call_pos,
                                        lowest_call_pos,
                                        S_DHC,
                                        TRUE,
                                        lowest_call_rev_flag,
                                        distance,distance2,FALSE,
                                        FALSE,spareCap); //spcap


              rrt += rrt_run_simulation(car,
                                        lowest_call_pos,
                                        pos,
                                        S_UHC,
                                        TRUE,
                                        FALSE,
                                        distance,distance2,FALSE,
                                        1,spareCap);

              //DBD  Coincident Check   29Jan04 Honma
              /* TestInProgress
              if ((asslib_DbdExpectCalls[car][pos] & DBD_SDCC) != 0)
                  *coinc_cc_flag = FALSE;
              */
              if ((asslib_DbdExpectCalls[car][pos] & DBD_SUCC) != 0)
                {
                  *coinc_cc_flag = TRUE;
                  *coinc_type |= CC_HCwithDCC;
                }


              //DBD 30Jan04  Honma
              //Check if the hallcall's destination have coincident car calls. 
              if (((asslib_DbdExpectCalls[car][dstPos] & DBD_SUCC) != 0) &&
                 (asslib_ValidExpectCalls2(car,UP_DIR,lowest_call_pos,local_NCF-1,dstPos)))
                { 
                  *coinc_cc_flag = FALSE;
                  *coinc_type |= CC_DCCwithDCC;
                }
              // DBD 10Feb04
              if (((ocss_info[car].assigned_calls[dstPos] & S_UHC) != 0) &&
                   (dstPos < local_NCF))
                {
                  *coinc_cc_flag = FALSE;
                  *coinc_type |= CC_DCCwithHC;
                }

              /* Calculate Express Zone run time if car is in express */
              /* zone with no demand                                  */
              /* -----------------------------------------------------*/
              if ( (pos < local_NCF) && (highest_call_pos == local_NCF)
                  && (ez_running_flag == TRUE) )
                { 
                   rrt += 2*(ocss_info[car].ez_top_pos - local_NCF);
                   //dojima
                   rrt += RRT_SHORT_RUN;
                }
           } /* car above call */
         else
           {
             /* call above car */
             if (pos >= local_NCF)
               {

                  highest_call_pos = ads_HighestPosition(car,local_NCF,pos,TRUE);      
 
                  //DBDSP
                  rrt += rrt_run_simulation(car,
                                            local_NCF,
                                            pos,
                                            (S_UHC + S_CC + E_CC),
                                            stopping_flag,
                                            FALSE,
                                            distance,distance2,TRUE,
                                            2,spareCap); //spcap

                  //DBD 30Jan04  Honma
                  //Check if the hallcall's destination have coincident car calls. 
                  if (((ocss_info[car].assigned_calls[dstPos] & (S_CC + E_CC)) != 0) ||
                      (((asslib_DbdExpectCalls[car][dstPos] & DBD_SUCC) != 0) &&
                       (asslib_ValidExpectCalls2(car,UP_DIR,local_NCF,dstPos-1,dstPos))))
                    { 
                      *coinc_cc_flag = FALSE;
                      if ((ocss_info[car].assigned_calls[dstPos] & (S_CC+E_CC)) != 0)
                        *coinc_type |= CC_DCCwithCC;
                      else
                        *coinc_type |= CC_DCCwithDCC;
                    }
                  // DBD 10Feb04 
                  if ((ocss_info[car].assigned_calls[dstPos] & S_UHC) != 0)
                    {
                      *coinc_cc_flag = FALSE;
                      *coinc_type |= CC_DCCwithHC;
                    }

                  // DBD 20Feb04 
                  

                  //special clear routine
                  for (i=pos;i<=ocss_info[car].top_pos;i++)
                     {
                       car_call_pred[i][UP_DIR-1] = FALSE;
                     }
               }
           }
       } /* UP call and UP car */
       
       
       /* Else, if the car and hall call are both in the down   */
       /* direction, there are two possible cases for answering */
       /* the call:  1) the call is below the car, requiring no */
       /* direction reversal; 2) the call is above the car,     */
       /* requiring the car to travel around the whole          */
       /* building.                                             */
       /* ----------------------------------------------------- */
       else if ( ( calls_dir == DOWN_DIR) && (pre_dir == DOWN_DIR) )
       {
          /* Tempflag is TRUE if doors are closed or closing */
          if ( ((ocss_info[car].f_door_state == DOORS_CLOSING) ||
                (ocss_info[car].f_door_state == DOORS_CLOSED)) && 
               ((ocss_info[car].r_door_state == DOORS_CLOSING) ||
                (ocss_info[car].r_door_state == DOORS_CLOSED)) )
              {
                  if ((ocss_info[car].reopen_cnt!=255) &&
                     (ocss_info[car].nudging_detect==FALSE) &&
                     (ocss_info[car].f_door_state==DOORS_CLOSING))
                    {
                      tempflag1 = FALSE; 
                    }
                 else
                      tempflag1 = TRUE;
              }
          else
              {
                 if ((ocss_info[car].reopen_cnt != 255)&&
                    (ocss_info[car].nudging_detect==TRUE))
                   tempflag1 = TRUE;
                 else
                   tempflag1 = FALSE;
              }
  
          /* tempflag2 is TRUE if...                       */
          /* (car moving and call not assigned already) OR */
          /* --------------------------------------------- */
          if (stopping_flag == FALSE)  
            {
               tempflag2 = TRUE;
            }
         else
            {
               tempflag2 = FALSE;
            }
    
          /* If the position is greater than the next commitable, OR */
          /* ( 1)  The position is equal to the next commitable, AND */
          /*   2)  The car has demand pending, AND                   */
          /*   3)  car doors are closing or closed, AND              */
          /*   4)  (car is moving AND no calls assigned) OR          */
          /* ------------------------------------------------------- */
          if ( (pos > local_NCF) || 
              (pos == local_NCF &&
              ocss_info[car].allsummary != 0 &&
              tempflag1 == TRUE &&
              tempflag2 == TRUE) )
            {
              lowest_call_pos = ads_LowestPosition(car,local_NCF,pos,TRUE);
              /*
               if (lowest_call_pos != pos)  lowest_call_rev_flag = TRUE;
               else                         lowest_call_rev_flag = FALSE;
              */
              //DBDSP
              rrt += rrt_run_simulation(car,
                                        local_NCF,
                                        lowest_call_pos,
                                        (S_DHC + S_CC + E_CC),
                                        stopping_flag,
                                        TRUE,
                                        distance,distance2,TRUE,
                                        FALSE,spareCap); //spcap


              highest_call_pos = ads_HighestPosition(car,local_NCF,pos,FALSE);
 
              if (highest_call_pos != pos)  highest_call_rev_flag = TRUE;
              else{
                          highest_call_rev_flag = FALSE;
                          car_call_pred[pos][UP_DIR-1]=FALSE;
                  }

              rrt += rrt_run_simulation(car,
                                        lowest_call_pos,
                                        highest_call_pos,
                                        S_UHC,
                                        TRUE,
                                        highest_call_rev_flag,
                                        distance,distance2,FALSE,
                                        FALSE,spareCap); //spcap


              rrt += rrt_run_simulation(car,
                                        highest_call_pos,
                                        pos,
                                        S_DHC,
                                        TRUE,
                                        FALSE,
                                        distance,distance2,FALSE,
                                        1,spareCap); //spcap
              //DBD  Coincident Check
              /* TestInProgress
              if ((asslib_DbdExpectCalls[car][pos] & DBD_SUCC) != 0)
                  *coinc_cc_flag = FALSE;
              */
              if ((asslib_DbdExpectCalls[car][pos] & DBD_SDCC) != 0)
                {
                  *coinc_cc_flag = TRUE;
                  *coinc_type |= CC_HCwithDCC;
                }
              //DBD 30Jan04  Honma
              //Check if the hallcall's destination have coincident car calls. 
              if (((asslib_DbdExpectCalls[car][dstPos] & DBD_SDCC) != 0) && 
                 (asslib_ValidExpectCalls2(car,DOWN_DIR,highest_call_pos,local_NCF+1,dstPos)))
                { 
                  *coinc_cc_flag = FALSE;
                  *coinc_type |= CC_DCCwithDCC;
                }
              // DBD 10Feb04
              if (((ocss_info[car].assigned_calls[dstPos] & S_DHC) != 0) &&
                  (dstPos > local_NCF))
                {
                  *coinc_cc_flag = FALSE;
                  *coinc_type |= CC_DCCwithHC;
                }


              /* Calculate Express Zone run time if car is in express */
              /* zone with no demand                                  */
              /* -----------------------------------------------------*/
              if ( (pos > local_NCF) && (lowest_call_pos == local_NCF)
                 && (ez_running_flag == TRUE) )
                { 
                    rrt += 2*(local_NCF - ocss_info[car].ez_bottom_pos);
                    //dojima
                    rrt += RRT_SHORT_RUN;
                }
            } /* car below call */
           /* otherwise, if car above call */
           else if (pos <= local_NCF)
            {

              lowest_call_pos = ads_LowestPosition(car,local_NCF,pos,TRUE);
              //DBDSP
              rrt += rrt_run_simulation(car,
                                        local_NCF,
                                        pos,
                                        (S_DHC + S_CC + E_CC),
                                        stopping_flag,
                                        FALSE,
                                        distance,distance2,TRUE,
                                        2,spareCap); //spcap

              //DBD 30Jan04  Honma
              //Check if the hallcall's destination have coincident car calls. 
              if (((ocss_info[car].assigned_calls[dstPos] & (S_CC + E_CC)) != 0) ||
                  (((asslib_DbdExpectCalls[car][dstPos] & DBD_SDCC) != 0) && 
                     (asslib_ValidExpectCalls2(car,DOWN_DIR,local_NCF,dstPos+1,dstPos))))
                {
                  *coinc_cc_flag = FALSE;
                  if ((ocss_info[car].assigned_calls[dstPos] & (S_CC+E_CC)) != 0)
                    *coinc_type |= CC_DCCwithCC;
                  else
                    *coinc_type |= CC_DCCwithDCC;
                }
              // DBD 10Feb04 
              if ((ocss_info[car].assigned_calls[dstPos] & S_DHC) != 0)
                {
                  *coinc_cc_flag = FALSE;
                  *coinc_type |= CC_DCCwithHC;
                }

              //special clear routine
               for (i=0;i<=pos;i++)
                 {
                   car_call_pred[i][DOWN_DIR-1] = FALSE;
                 }
            } /* car above call */
       } /* DOWN call and DOWN car */
       
       
       /* Otherwise, the car is at rest and travel to the hall */
       /*  call will not require a direction reversal.         */
       /* ---------------------------------------------------- */
       else if (pre_dir == NO_DIR)
       {

         if (pos > local_NCF)
          {
            if (calls_dir == UP_DIR)
             {

                lowest_call_pos = ads_LowestPosition(car,local_NCF,pos,FALSE);
                if (lowest_call_pos != pos)  lowest_call_rev_flag = TRUE;
                else                         lowest_call_rev_flag = FALSE;

                rrt += rrt_run_simulation(car,
                                          local_NCF,
                                          lowest_call_pos,
                                          S_DHC,
                                          TRUE,
                                          lowest_call_rev_flag,
                                          distance,distance2,TRUE,
                                          FALSE,spareCap); //spcap
                //DBD
                rrt += rrt_run_simulation(car,
                                          lowest_call_pos,
                                          pos,
                                          S_UHC,
                                          TRUE,
                                          FALSE,
                                          distance,distance2,FALSE,
                                          FALSE,spareCap); //spcap

				              //Check if the hallcall's destination have coincident car calls. 
              if (((ocss_info[car].assigned_calls[dstPos] & (S_CC + E_CC)) != 0) ||
                  (((asslib_DbdExpectCalls[car][dstPos] & DBD_SUCC) != 0)))
                {
                  *coinc_cc_flag = FALSE;
                  if ((ocss_info[car].assigned_calls[dstPos] & (S_CC+E_CC)) != 0)
                    *coinc_type |= CC_DCCwithCC;
                  else
                    *coinc_type |= CC_DCCwithDCC;
                }
              // DBD 10Feb04 
              if ((ocss_info[car].assigned_calls[dstPos] & S_UHC) != 0)
                {
                  *coinc_cc_flag = FALSE;
                  *coinc_type |= CC_DCCwithHC;
                }


             } /* UP hall call, STOPPED car */
           else /* DOWN hall call, STOPPED car */
             {
			   /*jsr
                lowest_call_pos = ads_LowestPosition(car,local_NCF,pos,FALSE);

                if (lowest_call_pos != pos)  lowest_call_rev_flag = TRUE;
                else                         lowest_call_rev_flag = FALSE;

                rrt += rrt_run_simulation(car,
                                          local_NCF,
                                          lowest_call_pos,
                                          S_DHC,
                                          TRUE,
                                          TRUE,
                                          distance,distance2,TRUE,
                                          FALSE,spareCap); //spcap
				*/
                highest_call_pos = ads_HighestPosition(car,local_NCF,pos,FALSE);

                if (highest_call_pos != pos)  highest_call_rev_flag = TRUE;
                else                          highest_call_rev_flag = FALSE;

                rrt += rrt_run_simulation(car,
                                          local_NCF,
                                          highest_call_pos,
                                          S_UHC,
                                          TRUE,
                                          highest_call_rev_flag,
                                          distance,distance2,FALSE,
                                          FALSE,spareCap); //spcap
                rrt += rrt_run_simulation(car,
                                          highest_call_pos,
                                          pos,
                                          S_DHC,
                                          TRUE,
                                          FALSE,
                                          distance,distance2,FALSE,
                                          FALSE,spareCap); //spcap
             } /* DOWN hall call, STOPPED car */
          } /* call above car */
        else /* call at or below car */
          {
             if (calls_dir == UP_DIR)
               {
                  highest_call_pos = ads_HighestPosition(car,local_NCF,pos,FALSE);

                  if (highest_call_pos != pos)  highest_call_rev_flag = TRUE;
                  else                          highest_call_rev_flag = FALSE;

                  rrt += rrt_run_simulation(car,
                                            local_NCF,
                                            highest_call_pos,
                                            S_UHC,
                                            TRUE,
                                            TRUE,
                                            distance,distance2,TRUE,
                                            FALSE,spareCap); //spcap

                  lowest_call_pos = ads_LowestPosition(car,local_NCF,pos,FALSE);

                  if (lowest_call_pos != pos)  lowest_call_rev_flag = TRUE;
                  else                         lowest_call_rev_flag = FALSE;

                  rrt += rrt_run_simulation(car,
                                            highest_call_pos,
                                            lowest_call_pos,
                                            S_DHC,
                                            TRUE,
                                            lowest_call_rev_flag,
                                            distance,distance2,FALSE,
                                            FALSE,spareCap); //spcap
                  rrt += rrt_run_simulation(car,
                                            lowest_call_pos,
                                            pos,
                                            S_UHC,
                                            TRUE,
                                            FALSE,
                                            distance,distance2,FALSE,
                                            FALSE,spareCap); //spcap
              if ((asslib_DbdExpectCalls[car][dstPos] & DBD_SUCC) != 0)
                {
                  *coinc_cc_flag = FALSE;
                    *coinc_type |= CC_DCCwithDCC;
                }
               } /* UP hall call, STOPPED car */
             else /* DOWN hall call, STOPPED car */
               {
                  highest_call_pos = ads_HighestPosition(car,local_NCF,pos,FALSE);

                  if (highest_call_pos != pos)  highest_call_rev_flag = TRUE;
                  else                          highest_call_rev_flag = FALSE;
 
                  rrt += rrt_run_simulation(car,
                                            local_NCF,
                                            highest_call_pos,
                                            S_UHC,
                                            TRUE,
                                            highest_call_rev_flag,
                                            distance,distance2,TRUE,
                                            FALSE,spareCap); //spcap
                  rrt += rrt_run_simulation(car,
                                            highest_call_pos,
                                            pos,
                                            S_DHC,
                                            TRUE,
                                            FALSE,
                                            distance,distance2,FALSE,
                                            FALSE,spareCap); //spcap
               } /* DOWN hall call, STOPPED car */
          } /* call at or below car */
       } /* car is stopped */
       /* A L L   D O N E ! */
       /* ----------------- */
       return(rrt);
}

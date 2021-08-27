/* U10 Project: @(#)rrt.c	1.3	3/23/95	14:23:04 */
/**************************************************************************
;  File Name: RRT.C - Tasks and procedures for rrt estimator.
;
;  File Description:
;
;  Contains tasks and procedures for handling RRT estimation.
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
;  Unpublished work - Copyright 1994 - 1995 Otis Elevator Company
;  All rights reserved.
;
;
;  Revision History:
;
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  A1730203BAA           Mark Ross                     19 December 1994
;  - SCR 30203-139
;    Created module. Functions in this module were originally contained
;    in the "ica.c" module.
;
;  A1730203BAA           Theresa Christy               02 February 1995
;  - SCR 30203-183
;    The routine "nrlrrt_calc_nnrrt" that calculates the output of the
;    neural network is passed an rrt value.  Changed the rrt value to 
;    the minimum rrt calculation instead of the conventional rrt calculation.
;    Note: It is easy to return to the original neural network instead
;    of using the minimum neural network by replacing calls to
;    "minrrt_calc_min_rrt" with calls to "crisprrt_calculate_rrt" in the
;    subroutine "rrt_determine_rrt" when e2p_neural_enable = NEURAL_DISPATCHING
;    For training weights, the value passed to "nrlrrt_calc_nnrrt" when
;    e2p_neural_enable is not set to NEURAL_DISPATCHING and is not
:    set to UNCERTAINTY_DISPATCHING should be the appropriate rrt
;    value for the type of network is being trained (i.e. min_rrt for
;    minimum neural network and uncertainty neural network, rrt_value for
;    original neural network.)
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;  J1830203AAA           H.Honma                                 29 May 1995
;  - SCR 30203-5017: Support Wheelchair   
;  - SCR30203-5004:  Use LCADS information    30 May 1995
;
;  J2130203AAA           H.Honma                                 26 Nov 1995
;  - SCR 30203-5048: Stop Neural-Network during MIT   
;
;  AAB-30203-3AA         Mark Ross                     8 December 1995
;  - SCR 30203-225: OTI SCR 5048. Disable neural network
;    during MIT.   
;
;  J2130203AAA           H.Honma                            27 Nov 1995
;  - SCR 30203-5066: Uncertainty Distance with Express Zone
;  
;  AAB-30203-3AA         Mark Ross                     13 December 1995
;  - SCR 30203-251: OTI SCR 5066.  Modify uncertainty factor 
;    to account for express zone.
;
;  J2130203AAA           H.Honma                            30 Nov 1995
;  - SCR 30203-5068: Consider operational mode with
;     Neural Network    
;
;  - AAB302032AA        Mark Ross                       19 Dec 1995
;    SCR 30203-253: OTI SCR 5068.  Consider DHB, DOS,
;    DCP, and WCS for neural network by moving code from
;    individual rrt modules (crisprrt.c, minrrt.c, ect...) to rrt.c 
;    which is common to all forms of the rrt calculation.
;
;  AAB-30203-4AA         Mark Ross                          25 Jan 1996
;  - SCR 30203-267: EEPROM enable/disable for NN training.
;
;  JAB-30203-AAB         Hideyuki Honma                   22  Mar 1996
;  - SCR 5073:  Upgrade dispatch during Channeling(Senri-cyuoo) 
;
;  JAB-30203-AAB         Hideyuki Honma                   01  Apr 1996
;  - SCR 5074:  Use decel bit of motion status for RRT calculation 
;
;  JAB-30203-AAB         Hideyuki Honma                   08  Apr 1996
;  - SCR 5078:  Correct RRT calculation logic of "crisprrt.c"
;
;*************************************************************************/
#include "stdafx.h"
#include "common.h"
#ifdef CAGS
#include "global.lit"
#include "global.h"
#endif
/**************************************************************************
;  Public Declarations
;*************************************************************************/
#define EXTERN extern   /* declare public variables */
#ifndef CAGS
#include "inferr.h"
#include "infocs.h"
#include "infrng.h"
#include "sio.h"
#include "exec.h"
#include "dpc.h"
#endif
#include "e2p.h"
#include "infmsg.h"
#include "infbld.h"
#include "crisprrt.h"
//#include "nrlrrt.h"
//#include "minrrt.h"
#include "assign.h"
#include "asslib.h"
#undef EXTERN


/**************************************************************************
;  Public Definitions
;*************************************************************************/
#define EXTERN          /* define public variables */
#include "rrt.h"
#undef EXTERN


/**************************************************************************
;  Local Definitions
;*************************************************************************/
/* None */
#ifdef CAGS
unsigned char beforeadscalls[MAX_OCSS];
extern unsigned char beforehallcalls[MAX_OCSS];
unsigned char spareCap[MAX_OCSS];
unsigned char rrtAdsPred1;
extern void deb_msg(int i, char *mes);
#endif

/**************************************************************************
;---------------------------------------------------------------------------
;  P U B L I C   R R T   F U N C T I O N S
;---------------------------------------------------------------------------
;***************************************************************************
;
;
;********************************************************************/


/*********************************************************************
; Function Name: rrt_determine_rrt
;
; There are currently two different ways to make an RRT prediction within
; this ica.c module - the conventional, sometimes called "CRISP" way of
; predicting RRT and the neural network way.  The method of RRT to be used
; in dispatching decisions is configurable based on a service tool 
; parameter.
;
; This procedure calls the appropriate routine to calculate RRT based
; on rrt_type.  Since the neural network uses the crisp RRT value as
; an input the procedure crisprrt_calculate_rrt must be called in the case
; of rrt_type = CRISP_DISPATCHING and in the case rrt_type = NEURAL_DISPATCHING
; 
;********************************************************************/
INT_T rrt_determine_rrt(BYTE_T car, BYTE_T call_pos, BYTE_T call_dst, BYTE_T call_mask, 
                        BYTE_T pre_dir, FLAG_T *coinc_cc_flag, BYTE_T *coinc_type,  
                        BYTE_T *distance,BYTE_T *distance2,BYTE_T *spareCap)
{
	FLOAT_T nn_rrt;
	INT_T rrt_value;
	INT_T crisp_rrt;
	INT_T min_rrt; 
	INT_T calls_dir;
	FLAG_T able_to_serve_flag;
	BYTE_T i,j;
	BYTE_T dest_cc_mask;
	able_to_serve_flag = TRUE;
	
    /* I N I T I A L I Z E   C A L L   D I R E C T I O N */
    /* ------------------------------------------------- */
    /* Based on direction of call, define call direction */
    /* ------------------------------------------------- */
    if ( (call_mask & S_UHC) != 0)
    {
		calls_dir = UP_DIR;
    }
    else
    {
		calls_dir = DOWN_DIR;
    }
    
    // todo: specify front or rear destination call.
    dest_cc_mask = S_FCC;
    
	
    /* U P D A T E   C A L L S   S U M M A R Y */
    /* --------------------------------------- */
    /* Looks at all pending calls to determine */
    /* the number of calls above, below, and   */
    /* at the car's position.                  */
    /* --------------------------------------- */
    asslib_update_summary(car);
    
	
    /* T E S T   A L L O W E D   M A S K */
    /* --------------------------------- */
    /* Check allowed mask to see if this */
    /* car can serve call                */
    /* --------------------------------- */
    if ( (ocss_info[car].allowed_mask[call_pos] & call_mask) == 0)
    {
		able_to_serve_flag = FALSE;
    }
	
    /* T E S T   A L L O W E D   M A S K */
    /* --------------------------------- */
    /* Check allowed mask to see if this */
    /* car can serve destination         */
    /* --------------------------------- */
    if ( (call_dst != 255) && 
		((ocss_info[car].allowed_mask[call_pos] & dest_cc_mask) == 0) )
    { // destination 될 수 없는 곳은 콜을 받지 않는다.
		// call_dst 일 가능성이 높다.
		able_to_serve_flag = FALSE;
    }
	
	if (able_to_serve_flag == TRUE)
	{
		rrt_value = crisprrt_calculate_rrt(car, call_pos, call_dst, call_mask, 
			pre_dir, coinc_cc_flag, coinc_type, distance,  distance2, spareCap);
		return (rrt_value);       
	}
	else /* able_to_serve_flag == FALSE */
	{
		return (RRT_MAX);
	}
}

/*********************************************************************
; Function Name: rrt_run_simulation
;
; This procedure determines the time required to travel between
; START and FINISH floors by evaluating the stops that the car
; will answer along the way.
;
; The STOPPING variable indicates if the car will stop prior to the
; start of the run simulation.  If STOPPING is TRUE, assume that the car
; will begin the run at rest regardless of the car's current motion state.
; Otherwise, use the car's current motion state to find the travel time
; to the first floor in the run.
;
; The REVERSAL parameter indicates whether the FINISH floor is a 
; direction reversal floor (cyclic RSR) or the floor at which the
; RSR value is being calculated.  If REVERSAL is TRUE, assume the
; FINISH floor is a reversal floor.  Note that all types of stops
; at the FINISH floor will be factored into the simulation time
; for a reversal calculation, but not for a calculation which ends
; at the RSR hall call floor.
;
*********************************************************************/
//DBD
INT_T rrt_run_simulation(BYTE_T car, BYTE_T start, BYTE_T finish, 
                         INT_T mask, FLAG_T stopping_flag, 
                         FLAG_T reversal_flag,
                         BYTE_T * cumulativeDistance,
                         BYTE_T * distance2,
                         FLAG_T expectCcCheckFlag,
                         BYTE_T spCapType,BYTE_T *spareCap)
{
INT_T  simulation_time, car_travel_time;
BYTE_T distance,distance1,floor;
REG_INT_T count;
INT_T  old_mask,e_mask;
BYTE_T dest_mask,dest_dir;

    simulation_time = 0;
    old_mask = mask;

    if ((mask & S_FUHC) != 0)   
      {
        e_mask = E_FUHC;
        dest_mask = DBD_SUCC;
        dest_dir  = UP_DIR;
      }
    else if ((mask & S_FDHC) != 0)  
      {
        e_mask = E_FDHC;
        dest_mask = DBD_SDCC;
        dest_dir  = DOWN_DIR;
      }
    
//DBD-honma spcap 20Mar04    if (start != finish)
//for SpareCapacity Check
    if (1)
    {
       /* Calculate the distance (in number of floors) between the START  */
       /* and FINISH positions.  Also, if the car will travel through the */
       /* express zone during the simulation, add the express zone time   */
       /* to the total simulation time.                                   */
       /* --------------------------------------------------------------- */
       if (start > finish)
       {
          distance = start - finish;
          distance1 = start - finish; // shs : can be replaced by distance1=distance;

          if (ocss_info[car].ez_type == 1) 
          {
				//jsr 050225 for 2D

          }
       }
       
       /*  Handle finish > start  */
       /*---------------------------*/
       else
       {
          distance = finish - start;
          distance1 = finish - start;

          if (ocss_info[car].ez_type == 1) 
          {
             if ((finish > ocss_info[car].ez_top_pos) &&
                 (start < ocss_info[car].ez_bottom_pos))
             {
                	distance1 -= (ocss_info[car].ez_top_pos-ocss_info[car].ez_bottom_pos);
             }
          
             else if ((start >= ocss_info[car].ez_bottom_pos)&&
                      (start <ocss_info[car].ez_top_pos))
             { // shs_suspicious : why is start in between express zone?
			 	// if that is acceptable, why finish doesn't have that condition
               /* test1031 */
               distance1 -= (ocss_info[car].ez_top_pos - start);// shs :indentation needed
            }
          }
       }
       
        *cumulativeDistance += distance1; // distance excluding expresszone 


       /* Determine the time it will take the car to travel to the */
       /* first floor in the simulation, based on the STOPPING     */
       /* parameter or whether the car is currently stopped        */
       /* or moving.  Note that this variable is adjusted          */
       /* throughout the simulation to take into account the added */
       /* travel time caused by stopping the car.                  */
       /* -------------------------------------------------------- */
       if ((ocss_info[car].decel_flag == TRUE) || 
           (stopping_flag == TRUE) || (ocss_info[car].mov_dir == NO_DIR)
            || (ocss_info[car].f_door_state != DOORS_CLOSED) )
       {
             car_travel_time = RRT_SHORT_RUN;

         //DBD-honma spcap   19Mar04 
         if (spCapType == 2) // SameDirection, and Car is moving to the call.
         {
            if (ocss_info[car].f_door_state == DOORS_CLOSED)  
            {
               *spareCap += asslib_DbdPeopleInCarCnts[car];

                 if (((ocss_info[car].assigned_calls[start] & mask & S_HC) != 0) ||
                    ((ocss_info[car].stop_type & mask & S_HC) != 0))
                  {
                    // *spareCap += boardingRate;
                    *spareCap += asslib_DbdAssignCallDirCnts[car][dest_dir][start];
                  }

                 if (((ocss_info[car].assigned_calls[start] & mask & S_CC) != 0) ||
                    ((ocss_info[car].stop_type & mask & S_CC) != 0))
                  {
                    //Check Terminal Car Call ??
                    if (((ocss_info[car].pre_dir == UP_DIR) &&
                       (ocss_info[car].actual_pos < ocss_info[car].lastStopPos)) ||
                       ((ocss_info[car].pre_dir == DOWN_DIR) &&
                       (ocss_info[car].actual_pos > ocss_info[car].lastStopPos)))
                      {
                       *spareCap -= asslib_DbdPeopleInCarCnts[car];
                      }
                    else 
                      {
                       if (*spareCap > asslib_DbdCarCallCnts[car][start]) 
                           *spareCap -= asslib_DbdCarCallCnts[car][start];
                       else                    
                          *spareCap = 0;
                      }
                  }
            }//DOOR_CLOSED
          else//NotClosed
            {

               *spareCap += asslib_DbdPeopleInCarCnts[car];
               //*spareCap = asslib_DbdPeopleInCarCnts[car]+asslib_DbdAssignCallDirCnts[car][dest_dir][start];
                 
                 if (((ocss_info[car].assigned_calls[start] & mask & S_HC) != 0) ||
                    ((ocss_info[car].stop_type & mask & S_HC) != 0))
                  {
                    // *spareCap += boardingRate;
                    *spareCap += asslib_DbdAssignCallDirCnts[car][dest_dir][start];
                  }

                 if (((ocss_info[car].assigned_calls[start] & mask & S_CC) != 0) ||
                    ((ocss_info[car].stop_type & mask & S_CC) != 0))
                  {
                    //Check Terminal Car Call ??
                    if (((ocss_info[car].pre_dir == UP_DIR) &&
                       (ocss_info[car].actual_pos < ocss_info[car].lastStopPos)) ||
                       ((ocss_info[car].pre_dir == DOWN_DIR) &&
                       (ocss_info[car].actual_pos > ocss_info[car].lastStopPos)))
                      {
                       *spareCap -= asslib_DbdPeopleInCarCnts[car];
                      }
                    else 
                      {
                       if (*spareCap > asslib_DbdCarCallCnts[car][start]) 
                           *spareCap -= asslib_DbdCarCallCnts[car][start];
                       else                    
                          *spareCap = 0;
                      }
                  }
            }// NotClosed
          
         }//  if (spCapType == 2)
         else if (spCapType == 1)  //Next Trip.
         {
             if ((ocss_info[car].assigned_calls[start] & mask & S_HC) != 0) 
             {
                *spareCap += asslib_DbdAssignCallDirCnts[car][dest_dir][start];
             }

             if ((ocss_info[car].assigned_calls[start] & mask & S_CC) != 0) 
             {
                if (*spareCap > asslib_DbdCarCallCnts[car][start]) 
                    *spareCap -= asslib_DbdCarCallCnts[car][start];
                else                    
                    *spareCap = 0;
             }
         }

       }// if ((ocss_info[car].decel_flag == TRUE) ||  (stopping_flag == TRUE) || (ocss_info[car].mov_dir == NO_DIR)
       else
       {
             car_travel_time = RRT_FULL_RUN;

         //DBD-honma spcap 19Mar04
         if (spCapType == 2) // SameDirection, and Car is moving to the call.
          {
            //if(deck_GetLoad(car,deck) != 0)
             {
               if (ocss_info[car].f_door_state == DOORS_CLOSED)  
                   *spareCap += asslib_DbdPeopleInCarCnts[car];
               /*
               else //is that possible ?????????????????????
               */
             }
          }
        
         /* GCB3 
              if(spCapType == 2)
              {
               if(deck_GetLoad(car,deck) != 0)
                {
                 if(door_AllDoorsInState(car,deck,door_Closed))
                   *spareCap += deck_GetInCarPassengers(car,deck)*10;
                 else
                   *spareCap += (deck_GetLoad(car,deck)/2)*1150/650;
                }
              }
         */
       }

       
       for (count=1; count<=distance; count++)
       {
         /* Add the travel time to the next floor to the     */
         /* total simulation time, and set the floor pointer */      
         /* to the next floor to check for a stop.           */    
         /* ------------------------------------------------ */    
         simulation_time += car_travel_time;
    
         if (start > finish)
          {
             floor = start - count;
          }
        else
          {
             floor = start + count;
          }
    

         /* If we are calculating the simulation time to a direction   */
         /* reversal floor for cyclic RSR, all possible steps between  */
         /* START and FINISH, inconclusive, must be factored into the  */
         /* simulation time.                                           */
         /*                                                            */
         /* If we are calculating the time it takes to reach the floor */
         /* for which we are calculating the RSR value, do NOT factor  */
         /* in any calls being asnwered at the hall call floor.        */
         /* ---------------------------------------------------------- */
         if ( (floor != finish) || (reversal_flag == TRUE) )
           {
             /* During a direction reversal simulation, the final  */
             /* floor must be checked for both types of hall calls */
             /* and car calls, regardless of the actual mask sent  */
             /* to this routine.                                   */
             /* -------------------------------------------------- */
             if (floor == finish)  // reversal_flag == TRUE
               {
                  mask |= (S_HC | S_CC | S_EHC | E_HC | E_CC | E_EHC);
               }
       
             /* If there is a hall call assigned at this floor, add */ 
             /* on the hall stop penalty.  Also, set the car travel */
             /* time for this next floor to indicate a short run    */
             /* because the car is going to stop.                   */
             /* --------------------------------------------------- */
             /* add extended call H.H */
             if ( (ocss_info[car].assigned_calls[floor] & e_mask & E_HC) != 0)
               {
                  simulation_time += RRT_WCS_STOP;
                  car_travel_time  = RRT_SHORT_RUN;
               }
             else if ( (ocss_info[car].assigned_calls[floor] & mask & E_CC) != 0)
               {
                  simulation_time += RRT_WCS_STOP;
                  car_travel_time  = RRT_SHORT_RUN;
               } 
             else if ( (ocss_info[car].assigned_calls[floor] & mask & S_HC) != 0)
               {
                  simulation_time += RRT_HALL_STOP;
                  car_travel_time  = RRT_SHORT_RUN;
#ifdef CAGS
                  beforehallcalls[car]++;
#endif 
               }
             /* otherwise, if there is a car call at this floor, add on */
             /* the car stop penalty.  Also, set the car travel time    */
             /* for this next floor to indicate a short run because     */
             /* the car is going to stop.                               */
             /* ------------------------------------------------------- */
             else if ( (ocss_info[car].assigned_calls[floor] & mask & S_CC) != 0)
               {
                  simulation_time += RRT_CAR_STOP;
                  car_travel_time  = RRT_SHORT_RUN;
               }
             else if (  
                     ( (((old_mask & S_UHC)!=0)&&(car_call_pred[floor][0]==TRUE)) 
                    || (((old_mask & S_DHC)!=0)&&(car_call_pred[floor][1]==TRUE)) )  )
               {
                  simulation_time += RRT_CAR_STOP;
                  car_travel_time  = RRT_SHORT_RUN;
                  /* clear car_call_pred */
                  if ((old_mask & S_UHC)!=0)
                    car_call_pred[floor][0]=FALSE;
                  else
                    car_call_pred[floor][1]=FALSE;	
               }

              /* Otherwise, the car is not stopping at this floor.  Set the car */
              /* travel time for the next floor to indicate a full speed run.   */
              /* -------------------------------------------------------------- */
             else
               {
                  car_travel_time = RRT_FULL_RUN;

                 /* Possibly, e2p_UnsureDistance should be RRT_UNSURE_DISTANCE */
               }

             /* clear car call prediction */
             if ((old_mask & S_UHC)!=0)
                car_call_pred[floor][0]=FALSE;
             else
                car_call_pred[floor][1]=FALSE;
           } /* floor != finish or reversal_flag == TRUE */
       } /* count loop */
    } /* start != finish */
    //DBD-honma spcap 20Mar04
    else //start == finish
    {

    }


    return (simulation_time);
}


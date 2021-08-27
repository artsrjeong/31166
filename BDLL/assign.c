unsigned char debugFuz[10],debugCcc[10],debugRrt[10],debugPwt[10],debugCoinc;

/* U10 Project: @(#)assign.c	1.5	3/23/95	14:22:28 */
/**************************************************************************
;  File Name: ASSIGN.C - Tasks and procedures for call assignemnt
;
;  File Description:
;
;  Contains tasks and procedures for handling hall calls and stuff.
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
;  SCN                   Author                        Date
;  ------------------------------------------------------------------------
;  A1730203BAA           Mark Ross                     19 December 1994
;  - SCR 30203-139
;    Created module. Functions in this module were originally contained
;    in the "ica.c" module.
;
;  A1730203BAA           Theresa Christy               04 January 1995
;  - SCR 30203-137.
;    Modified sets of 48 (0 - 47) weights to sets of 55 weights (0 - 54)
;    to support configurable neural network inputs. (i.e. Busy Floor 1, Busy
;    Floor 2, Busy Floor 3 and Traffic Density)
;
;  A1730203BAA           Theresa Christy               19 January 1995
;  - SCR 30203-182
;    Add routine "assign_nn_weights" to switch the neural network
;    weights that will actually be used in calculations from one`
;    set of weights to a different set of weights.
;
;  A1730203BAA            Theresa Christy              02 February 1995
;  - SCR 30203-184
;    Added initializations for additional sets of neural network weights
;    FLOAT_T samlun_minRTweight[NUMBER_OF_NEURONS];
;    (Sample Building, Lunch Traffic, minimum neural network)
;    FLOAT_T samlun_origTDweight[NUMBER_OF_NEURONS];
;    (Sample Building, Lunch Traffic, original neural network with Traffic
;     Density Input = 1)
;    FLOAT_T samlun_origBFweight[NUMBER_OF_NEURONS];
;    (Sample Building, Lunch Traffic, original neural network with Busy
;     Floor 1 = 20)
;    FLOAT_T samlun_origAAweight[NUMBER_OF_NEURONS];
;    (Sample Building, Lunch Traffic trained with AutoAdjust Feature active
;    FLOAT_T samday_minRTweight[NUMBER_OF_NEURONS];
;    (Sample Building, All Day weights, minimum neural network)
;
;  A1730203BAA            Theresa Christy              19 January 1995
;  - SCR 30203-185
;    Initialize default neural network weights to Sample Building
;    All Day Weights.
;
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Reference renamed modules.
;
;  Transamerica          Geoff Mochau                  20 February 1995
;  - Omit NN references to limit RAM requirements.
;  - Removed Advanced-Predirection Metrics to limit RAM requirements.
;
;  A1730203BAA           Theresa Christy               24 February 1995
;  - SCR 30203-188: Added data sets for real time minimum neural network.
;    Sets are for BUS and MOT traffic for the Sample Building.
;
;  J1830203AAA           Hideyuki Honma                     25 May 1995
;  - SCR 30203-5008: Add initialization of unsure table routine.
;
;  - SCR 30203-5008       H.Honma                             26 May 1995
;  Add distance parameter to _calculate_pwt for use with crisp pwt calculation
;    update calls to rrt_determine_rrt - add distance to call
;
;  - SCR 30203-5012       H.Honma                             26 May 1995
;  Add prevent duplicate dispatch and no dispatch
;
;  - SCR 30203-5013       H.Honma                             26 May 1995
;  Change return codes from asslib_car_available to distinguish Normal/Overload
;  conditions/Out of Group/Other status of car based on Operational Mode.
;
;  - SCR 30203-5002       H.Honma                             7 June 1995
;  Support Dual Up Peak Dispatch
;
;  - SCR 30203-5005,5006   H.Honma                           11 June 1995
;  Support AI Crowd Floor & Pre-Parking service
;
;  - SCR 30203-5035        H.Honma                           11 June 1995
;  Support debug message for Door re-open dispatching
;
;  - SCR 30203-5038        H.Honma                           18 June 1995
;  Add affected passing assign
;
;  J1930203AAA           Hideyuki Honma                      29 June 1995
;  - SCR 30203-5027: Multi Lobby Parking Dispatching
;
;  J2130203AAA           Hideyuki Honma                      22 Sep 1995
;  - SCR 30203-5055: Crowd Floor Service with EMS
;
;  J2130203AAA           Hideyuki Honma                      05 Oct 1995
;  - SCR 30203-5060: Correct "ica_icss2_advanced_predir" variable
;  
;  AAB302033AA        Mark Ross                              17 Oct 1995
;  - SCR 30203-243: OTI SCR 5055. Crowd floor service by EMS.
;  - SCR 30203-240: OTI SCR 5060. Correct "ica_icss2_advanced_
;    predir" variable.
;
;  AAB302033AA        Mark Ross                              23 Oct 1995
;  - SCR 30203-243: OTI SCR 5055. Modified crowd code
;    to improve efficientcy.
;
;  AAB-30203-3AA         Mark Ross                        17 November 1995
;  - SCR 30203-204: Modified to remove calls to watchdog().
;    Now use POR_NMI_Handler on startup.
;
;  AAB-30203-3AA         Mark Ross                        6 December 1995
;  - SCR 30203-254: OTI SCR 5061 - Correct Dual Up Peak.
;
;  J2130203AAA           Hideyuki Honma                     18 Oct 1995
;  - SCR 30203-5062: Update Dispatching during MIT
;
;  AAB-30203-3AA         Mark Ross                     11 December 1995
;  - SCR 30203-255: OTI SCR 5062.  Update dispatching during
;    MIT to set second selected car's rrt to 255.
;
;  AAB-30203-3AA         Mark Ross                     13 December 1995
;  - SCR 30203-251: OTI SCR 5066.  Modify uncertainty factor
;    to account for express zones..
;
;  J2130203AAA           Hideyuki Honma                     31 Nov 1995
;  - SCR 30203-5029: Dispatch during Channeling
;
;  AAB-30203-3AA         Mark Ross                     21 Dec 1995
;  - SCR 30203-228: OTI SCR 5029.  Dispatch during Channeling
;
;  AAB-30203-3AA         Jannah Stanley                     15 Jan 1996
;  - SCR 30203-231: NN Training for AAB302033AA baseline.
;
;  AAB-30203-4AA         Hideyuki Honma(at OECF)            22 Jan 1996
;  - SCR 30203-262: Upgrade dispatch during Channeling(Korea job site)
;
;  AAB-30203-4AA         Theresa Christy                    30 Jan 1996
;  - SCR 30203-268: Added neural network weights for 4AA
;     baseline.
;
;  AAB-30203-4AA         Jannah Stanley                    8  Feb 1996
;  - SCR 30203-273: Added neural network weights for 4AA
;     baseline for min RRT  & UCF Networks. Removed obsolete weights.
;
;  JAB-30203-AAB         Hideyuki Honma                   22  Mar 1996
;  - SCR 5073:  Upgrade dispatch during Channeling(Senri-cyuoo)
;
;  JAB-30203-AAB         Hideyuki Honma                   25  Apr 1996
;  - SCR 5083:  Change PWT calculation
;
;*************************************************************************/
//#include "common.h"

/**************************************************************************
;  Public Declarations
;*************************************************************************/


extern "C" __declspec(dllexport) void assign_new_tsk (void)
{
	return;
	/*
	BYTE_T     msg_temp[15];
	INT_T      assn_incr;
	INT_T      max_assn_idx, assn_idx;
	INT_T      eld_pwt, eld_pwt_none, eld_pwt_very;
	INT_T      rrt, best_rrt ,rrt_low, rrt_med, rrt_high; 
	INT_T      rrt_idx; 
	INT_T      eld_idx_none, eld_idx_very;
	INT_T      eld_idx_incr;
	
	
	BYTE_T     best_car;
	INT_T      hc_assn_idx[MAX_OCSS];
	
	BYTE_T     number_affected_calls;
	BYTE_T     i, car, flag;
	BYTE_T     queue_count,dbdQueueCount;
	BYTE_T     call_pos, call_mask, temp_mask, dest_mask, call_type, dest_cc_mask;
	FLAG_T     dest_cc_serviceable;
	BYTE_T     call_dst;
	BYTE_T     distance,distance2,distance3;
	TASK_ID_T  TaskID;
	BYTE_T     dir;
	FLAG_T     coinc_cc_flag,coinc_dcc_flag;
	BYTE_T     coinc_cc[MAX_OCSS],coinc_type,coinc_type_car[MAX_OCSS],coinc_dcc[MAX_OCSS];
	FLAG_T     ignore_coinc_flag;
	FLAG_T     immediate_car_flag,parkIdleFlag;
	BYTE_T     pre_dir;
	
	FLAG_T     CrowdFlag;
	
	//   FLOAT_T    float1, float2, float3, float4, float5;
	LONG_T     long1, long2, long3, long4, long5;
	//   FLOAT_T    nnrrt;
	INT_T      nn_rrts[MAX_OCSS];
	FLAG_T     coinc_assign;
	BYTE_T     grp_no;
	FLAG_T     grp_available;

	INT_T      p_rrt,p_rrts[MAX_OCSS];
	
	INT_T      pure2_rrts[MAX_OCSS];
	
	BYTE_T     best_car2;
	INT_T      pure_rrts[MAX_OCSS],pure_pwts[MAX_OCSS];
	FLAG_T     chn_check_flag = FALSE,affectPassFlag;
	BYTE_T     tmp_highest_pos,tmp_lowest_pos;
	BYTE_T     doublePenalty,carBit;
	BYTE_T     waitLevel,callDir; 
	BYTE_T     dpcCallFlag,dpcAssignCar,dpcTmp1,dpcTmp2;
	BYTE_T     assignedCar,immediateFlag,stoppedCar,passIdLow,passIdHigh,passTmp[2];
	FLAG_T     dbdCallFlag;
	WORD_T     passId;
	BYTE_T     affectPwtByDst,affectPwtBySrc,affectPwtByDstCar[MAX_OCSS],affectPwtBySrcCar[MAX_OCSS];
	FLAG_T     sendLocalAssignFlag;
	FLAG_T     invChnBestCar;
	BYTE_T     spareCap;

	*/
	/* S E A R C H   F O R   N E W   C A L L S */
	/* --------------------------------------- */
	/* If no new calls in queue, then we're    */
	/* done.                                   */
	/* --------------------------------------- */

	/*
	if (reassign_exec == TRUE) // reassign task is in processing : shs
	{
		Exec_Enable_Task(assign_new_tskID); // enable task of assign new
		return;
	}
	
	assign_new_exec = TRUE; // assign_new_tsk is in processing
	
	queue_count = Q_Get_Count(ica_call_q_ptr);
	
	//DBD 19Feb04
	dbdQueueCount = Q_Get_Count(dbd_call_q_ptr);
	
	//DBD-honma  03Mar04 #ifdef DBD
	dbdCallFlag = FALSE;
	
	if ((queue_count < 4) && (dbdQueueCount < 6))
	{
		assign_new_exec = FALSE;
		return;
	}
	
	// Otherwise, get call type and position from new calls queue. 
	// ----------------------------------------------------------- 
	//DBD Call
	if (dbdQueueCount >= 6)   
	{
		Q_Get(dbd_call_q_ptr, &call_pos, 1);
		Q_Get(dbd_call_q_ptr, &call_dst,1); 
		Q_Get(dbd_call_q_ptr, &call_mask, 1);
		Q_Get(dbd_call_q_ptr, &call_type, 1);
		
		//Q_Get(ica_call_q_ptr, &passIdLow,1);   
		//Q_Get(ica_call_q_ptr, &passIdHigh,1);  
		
		Q_Get(dbd_call_q_ptr, &passTmp[0],2);   
		asslib_TwoBytesToOneWord(&passTmp[0],&passId);
		//Incorrect !!!grp_no = 255;
		grp_no = 0;
		dbdCallFlag = TRUE;
		
		// todo: specify front or rear destination call.
		dest_cc_mask = S_FCC;
	}
	//Normal Call
	else 
	{
		Q_Get(ica_call_q_ptr, &call_pos, 1);
		Q_Get(ica_call_q_ptr, &call_mask, 1);
		Q_Get(ica_call_q_ptr, &grp_no,1);   
		//DBD
		Q_Get(ica_call_q_ptr, &call_dst,1); 
		
		dest_cc_mask = S_FCC;
	}
	//DBD-honma 03Mar04 #endif
	*/
}

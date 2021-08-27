#include "stdafx.h"
#include "common.h"

#define EXTERN extern 
#include "infbld.h"
#include "infrng.h"
#include "assign.h"
#include "infdes.h"
#include "inferr.h"
#include "exec.h"
#include "infmsg.h"
#include "sio.h"
#undef EXTERN

#define EXTERN
#include "Asslib.h"
#undef EXTERN


/* U10 Project: @(#)asslib.c	1.5	3/23/95	14:22:30 */
unsigned char asslibMakeAll;

/*********************************************************************
; Function Name: asslib_count_normal_cars()
;
; Description: This routine calculates the number of cars in the
;              group, who are capable of answering a hall call.
;              This procedure counts the cars in the snapshot version
;              of the ocss information.  It does not check the "live"
;              ocss data.
;
; Parameters: none
;********************************************************************/
BYTE_T asslib_count_normal_cars(void)
{

   INT_T car, i;

   i = 0;
   for (car=CAR1; car <= CAR8; car++)
   {
      if (ocss_info[car].opmode == IDL || ocss_info[car].opmode == PRK ||
          ocss_info[car].opmode == NOR || ocss_info[car].opmode == LNS ||
          ocss_info[car].opmode == ANS || ocss_info[car].opmode == MIT ||
          ocss_info[car].opmode == CHN)
      {
         i++;
      }
   }

   return(i);
}

/*********************************************************************
; Function Name: asslib_car_available
;
; This procedure tests the operational mode of the car.  If the car
; is capable of serving hall calls, it returns "asslib_NormalMode" .
;
; If the car is in LNS or OLD operational mode then it returns
; "asslib_LNSOLDMode".  If the car is in DCP/DHB/DOS, then return
; "asslib_GroupOutMode".  Otherwise, return "asslib_NotAvailable".
;
; The car is available for boarding only if "asslib_NormalMode" is
; returned.
;
; Parameters: car - car ID
;
;********************************************************************/

FLAG_T asslib_car_available(BYTE_T car)
{
	FLAG_T returnCode;
	
	switch(ocss_info[car].opmode)
	{
		
	case NOR:
	case PRK:
	case MIT:
	case ACP:
	case IDL:
	case CHN:
	case ANS:
	case ATT:
		returnCode = asslib_NormalMode;
		break;
		
	case LNS:
	case OLD:
		returnCode = asslib_LNSOLDMode;
		break;
		
	case DCP:
	case DHB:
	case DOS:
	case WCS:
	case EHS:
		returnCode = asslib_GroupOutMode;
		break;
		
	default:  returnCode = asslib_NotAvailable;
	}
	return(returnCode);
}

/*********************************************************************
; Function Name: asslib_update_summary
;
; Tests to see if all allowed masks for cars have been received.
;
; Parameters:  car - Car ID
;
;********************************************************************/
void asslib_update_summary (BYTE_T car)
{
	
#ifndef CAGS
	REG_INT_T i;
#endif
#ifdef CAGS
	INT_T i;
#endif
	
	BYTE_T current_pos;
    /* Initialize summary variables */
    /* ---------------------------- */
    ocss_info[car].upsummary = 0;
    ocss_info[car].dnsummary = 0;
	
    /* determine current car position */
    /* ------------------------------ */
    if (ocss_info[car].mov_dir == NO_DIR)
    {
		current_pos = ocss_info[car].actual_pos;
    }
    else
    {
		current_pos = ocss_info[car].NCF;
    }
	
	
    /* Scan through all landings for calls, and update summary */
    /* variables as needed.                                    */
    /* ------------------------------------------------------- */
	ocss_info[car].dnhall_flag=FALSE;
	ocss_info[car].uphall_flag=FALSE;
	assigned_cnt[car]=0;
	DbdExpect_cnt[car]=0;
	future_cnt[car]=0;
	nPax_up[car]=0;
	nPax_dn[car]=0;
	nUpStop[car]=0;
	nDnStop[car]=0;
	ocss_info[car].next_up=ocss_info[car].top_pos;
	ocss_info[car].next_down=ocss_info[car].bottom_pos;
    for (i=ocss_info[car].bottom_pos; i<=ocss_info[car].top_pos; i++)
    {
		if (i == current_pos)
		{
			ocss_info[car].atsummary = ocss_info[car].assigned_calls[i];
		}
		else if (i > current_pos)
		{
			ocss_info[car].upsummary |= ocss_info[car].assigned_calls[i];
		}
		else
		{
			ocss_info[car].dnsummary |= ocss_info[car].assigned_calls[i];
		}
		if((ocss_info[car].assigned_calls[i]&S_UHC)!=0)
			ocss_info[car].uphall_flag=TRUE;
		if((ocss_info[car].assigned_calls[i]&S_DHC)!=0)
			ocss_info[car].dnhall_flag=TRUE;
		
		if(i>current_pos)
		{
			if(ocss_info[car].assigned_calls[i]!=0 
				&& ocss_info[car].next_up>i
				&& (ocss_info[car].assigned_calls[i]&(S_UHC+S_CC))!=0)
				ocss_info[car].next_up=i;
		}
		else if(i<current_pos)
		{
			if(ocss_info[car].assigned_calls[i]!=0 
				&& ocss_info[car].next_down<i 
				&& (ocss_info[car].assigned_calls[i]&(S_DHC+S_CC))!=0)
				ocss_info[car].next_down=i;
		}
		
		if(asslib_DbdAssignCallDirCnts[car][UP_DIR][i]>0)
		{
			nPax_up[car]+=asslib_DbdAssignCallDirCnts[car][UP_DIR][i];
			nUpStop[car]++;
		}
		if(asslib_DbdAssignCallDirCnts[car][DOWN_DIR][i]>0)
		{
			nPax_dn[car]+=asslib_DbdAssignCallDirCnts[car][DOWN_DIR][i];
			nDnStop[car]++;
		}
		if(asslib_DbdExpectCallCnts[car][UP_DIR][i]>0)
		{
			nUpStop[car]++;
		}
		if(asslib_DbdExpectCallCnts[car][DOWN_DIR][i]>0)
		{
			nDnStop[car]++;
		}
		if(ocss_info[car].assigned_calls[i]>0)
			assigned_cnt[car]++;
		if(asslib_DbdExpectCalls[car][i]>0)
			DbdExpect_cnt[car]++;
		if(ocss_info[car].future_calls[i]>0)
			future_cnt[car]++;
    }
	
    /* build overall call summary */
    /* -------------------------- */
    ocss_info[car].allsummary = ocss_info[car].upsummary |
		ocss_info[car].dnsummary |
		ocss_info[car].atsummary;
}

//DBD
FLAG_T asslib_ValidExpectCalls(BYTE_T car,BYTE_T dir,BYTE_T pos)
{
	FLAG_T  validFlag = 0;
	BYTE_T  flr,carPos,start,finish,dest_mask;
	
	carPos = ocss_info[car].NCF;
	
	validFlag = FALSE;
	if (dir == UP_DIR)  
    { 
		start = carPos;
		finish = pos - 1;
		dest_mask = DBD_SUCC;
    }
	else
    { 
		start = pos + 1;
		finish = carPos;
		dest_mask = DBD_SDCC;
    }
	
	for (flr = start; flr <= finish; flr++)
    {
		if ((asslib_DbdAssignCalls[car][flr][pos] & dest_mask) != 0)
		{
            validFlag = TRUE;
            break;
		}
    }
	
	return (validFlag);
}



//DBD
FLAG_T asslib_ValidExpectCalls2(BYTE_T car,BYTE_T dir,BYTE_T start,BYTE_T finish, BYTE_T pos)
//FLAG_T asslib_ValidExpectCalls2(BYTE_T car,BYTE_T dir,BYTE_T pos)
{
	FLAG_T  validFlag = 0;
	BYTE_T  chk1,chk2;
	BYTE_T  flr,dest_mask;
	
	
	validFlag = FALSE;
	
	if (finish > ocss_info[car].top_pos) 
		return (validFlag);
	
	if (dir == UP_DIR)  
    { 
		chk1 = start;
		chk2 = finish;
		dest_mask = DBD_SUCC;
    }
	else
    { 
		//start = pos + 1;
		//finish = carPos;
		chk1 = finish;
		chk2 = start;
		dest_mask = DBD_SDCC;
    }
	
	
	for (flr = chk1; flr <= chk2; flr++)
    {
		if ((asslib_DbdAssignCalls[car][flr][pos] & dest_mask) != 0)
		{
            validFlag = TRUE;
            break;
		}
    }
	
	return (validFlag);
}

 /*********************************************************************
 ; Function Name: asslib_calculate_pre_dir
 ;
 ; This task will calculate car predirection, based on calls assigned
 ; to car.  This is done because of ring and other delays; sometimes
 ; the OCSS predirection does not take into account a call that has
 ; been assigned, but has not been processed by the OCSS.
 ; 
 ;
 ; Parameters: car - car whose predirection we are testing
 ;
;********************************************************************/
BYTE_T asslib_calculate_pre_dir(BYTE_T car)
{
	
	BYTE_T pos;
	BYTE_T pre_dir;
	FLAG_T rev_flag;
	
    rev_flag = FALSE;
    
    
    /* I N I T I A L   P R E _ D I R   S A M E   A S   O C S S */
    /* ------------------------------------------------------- */
    pre_dir = ocss_info[car].pre_dir;
    
    
    /* I S   C A R   I N   V A L I D   M O D E ? */
    /* ----------------------------------------- */
    if ((ocss_info[car].opmode == NOR || 
		ocss_info[car].opmode == WCS || ocss_info[car].opmode == LNS)  ||
		//update for UppeakOperatDion 99/2/16  the condition is under testing
        ((ocss_info[car].opmode == MIT || ocss_info[car].opmode == CHN) && (pre_dir == DOWN_DIR) &&
        (ocss_info[car].NCF == ocss_info[car].lobby_pos || ocss_info[car].NCF == ocss_info[car].second_lobby_pos))
		)
    {
		
		/* Only assess predirection if car is stopping */
		/* ------------------------------------------- */
		if (ocss_info[car].NCF == ocss_info[car].target_pos)
		{
			if ( (ocss_info[car].mov_dir != NO_DIR) &&
				(ocss_info[car].mov_dir == ocss_info[car].pre_dir) )
			{
				rev_flag = TRUE;
				
				/* C A R   M O V I N G   D O W N */
				/* ----------------------------- */
				if ( (ocss_info[car].mov_dir == DOWN_DIR) &&
					(ocss_info[car].NCF != ocss_info[car].bottom_pos) )
				{
					/* If pending down hall call at target, or if we */
					/* are stopping for a down hall call at target,  */
					/* do not override OCSS predirection.            */
					/* --------------------------------------------- */
					/* jsr
					내려가는 중에 다음 정차할 곳에서 Down Hall Call이 있으면
					기존 pre_direction 을 유지
					*/

					pos = ocss_info[car].NCF;
					if ( ( (ocss_info[car].assigned_calls[pos] & (S_DHC+E_DHC)) != 0) ||
						( (ocss_info[car].stop_type & (S_DHC+E_DHC)) != 0) )
					{
						rev_flag = FALSE;
					}
					
					/* If there is a pending call below the target, */
					/* do not override OCSS predirection.           */
					/* -------------------------------------------- */
					for (pos=ocss_info[car].bottom_pos; pos < ocss_info[car].NCF; pos++)
					{
						if (ocss_info[car].assigned_calls[pos] != 0)
						{
							rev_flag = FALSE;
							break;
						}
					} /* loop from bottom to (next commitable - 1) */
				} /* mov_dir == DOWN && NCF != bottom */
				
				/* C A R   M O V I N G   U P */
				/* ------------------------- */
				if ( (ocss_info[car].mov_dir == UP_DIR) &&
					(ocss_info[car].NCF != ocss_info[car].top_pos) )
				{
					/* If pending up hall call at target, or if we */
					/* are stopping for a up hall call at target,  */
					/* do not override OCSS predirection.          */
					/* ------------------------------------------- */
					pos = ocss_info[car].NCF;
					if ( ( (ocss_info[car].assigned_calls[pos] & (S_UHC+E_UHC)) != 0) ||
						( (ocss_info[car].stop_type & (S_UHC+E_UHC)) != 0) )
					{
						rev_flag = FALSE;
					}
					
					/* If there is a pending call above the target, */
					/* do not override OCSS predirection.           */
					/* -------------------------------------------- */
					for (pos=ocss_info[car].NCF+1; pos <= ocss_info[car].top_pos; pos++)
					{
						if (ocss_info[car].assigned_calls[pos] != 0)
						{
							rev_flag = FALSE;
							break;
						}
					} /* loop from (next commitable + 1) to top */
				} /* mov_dir == UP && NCF != top */
				
			} /* mov_dir != NONE && mov_dir == pre_dir */
		} /* NCF == target_floor */
    } /* car is available */
    
    
    /* I F   R E Q U I R E D ,   O V E R R I D E   O C S S   P R E _ D I R */
    /* ------------------------------------------------------------------- */
    /* If rev_flag is TRUE, then the last predirection received from the   */
    /* OCSS is incorrect.  Set the predirection to the opposite of the     */
    /* OCSS predirection.                                                  */
    /* ------------------------------------------------------------------- */
    if (rev_flag == TRUE)
    {
		if (ocss_info[car].pre_dir == UP_DIR)
		{
			pre_dir = DOWN_DIR;
		}
		else if (ocss_info[car].pre_dir == DOWN_DIR)
		{
			pre_dir = UP_DIR;
		}
    }
    
    return(pre_dir);
}


void   asslib_DbdAssignCall(BYTE_T car, BYTE_T callmask, BYTE_T orgPos, BYTE_T dstPos)
{
	BYTE_T callDir;
	
	if ((callmask & DBD_SUCC) != 0) callDir = UP_DIR;
	else                            callDir = DOWN_DIR;
	
	asslib_DbdAssignCalls[car][orgPos][dstPos] |= callmask;
	asslib_DbdExpectCalls[car][dstPos] |= callmask;
	
	asslib_DbdAssignCallDirCnts[car][callDir][orgPos]++;
	asslib_DbdAssignCallDstCnts[car][orgPos][dstPos]++;
	
	asslib_DbdExpectCallCnts[car][callDir][dstPos]++;
	
#ifdef CAGS
	dispEnteredDestinationInfo(car, dstPos, asslib_DbdExpectCalls[car][dstPos]);
#endif
}

void   asslib_DbdDeassignCall(BYTE_T car, BYTE_T callmask, BYTE_T orgPos, BYTE_T dstPos)
{
	BYTE_T callDir,dstCnt;
	
	BYTE_T    msg_temp[10];
	
	if ((callmask & DBD_SUCC) != 0) callDir = UP_DIR;
	else                            callDir = DOWN_DIR;
	
	dstCnt = asslib_DbdAssignCallDstCnts[car][orgPos][dstPos];
	
	
    asslib_DbdAssignCalls[car][orgPos][dstPos] &= ~callmask;
	//    asslib_DbdExpectCalls[car][dstPos] &= ~callmask;
	
    if (asslib_DbdExpectCallCnts[car][callDir][dstPos] < dstCnt)
		asslib_DbdExpectCallsError[9]++;
    if (asslib_DbdAssignCallDirCnts[car][callDir][orgPos] < dstCnt)
		asslib_DbdExpectCallsError[10]++;
    if (dstCnt == 0)
    {
		asslib_DbdExpectCallsError[11]++;
    }
	
    asslib_DbdExpectCallCnts[car][callDir][dstPos] -= asslib_DbdAssignCallDstCnts[car][orgPos][dstPos];
    //DBD-honma 18mar04
    asslib_DbdCarCallCnts[car][dstPos] += asslib_DbdAssignCallDstCnts[car][orgPos][dstPos];
    asslib_DbdPeopleInCarCnts[car] += asslib_DbdAssignCallDstCnts[car][orgPos][dstPos];
	
    if (ocss_info[car].carEffPassenger < asslib_DbdPeopleInCarCnts[car])
    {
		asslib_DbdExpectCallsError[16]++;
		//Temporary !!!!!!!!!!!
		asslib_DbdPeopleInCarCnts[car] = ocss_info[car].carEffPassenger;
    }   
	
    asslib_DbdAssignCallDirCnts[car][callDir][orgPos] -= asslib_DbdAssignCallDstCnts[car][orgPos][dstPos];
	
    asslib_DbdAssignCallDstCnts[car][orgPos][dstPos] = 0;
	
    if (asslib_DbdExpectCallCnts[car][callDir][dstPos] == 0)
		asslib_DbdExpectCalls[car][dstPos] &= ~callmask;
	
#ifdef CAGS
    dispEnteredDestinationInfo(car, dstPos, asslib_DbdExpectCalls[car][dstPos]);
#endif
}

/*********************************************************************
; Function Name: asslib_xmit_car_ass 
;
; This routine transmits car assignment information to the specified
; car OCSS.  The landing index and call type must be provided.
;
; Parameters: car   - Car ID to receive message
;             index - landing index to determine block of 10 landings
;             mask  - call type being assigned.
;
;********************************************************************/
void asslib_xmit_car_ass(BYTE_T car, BYTE_T index, INT_T mask)
{
}

/*********************************************************************
; Function Name: asslib_assign_call
;
; This task is responsible for assigning a hall call to the specified
; car, and transmitting the assignment to the car.
;
; Parameters: car - car being assigned
;             callmask - bitmask of call being assigned
;             pos - floor positiong of call being assigned
;********************************************************************/
void asslib_assign_call(BYTE_T car, BYTE_T callmask, BYTE_T pos)
{
	BYTE_T pos_index;
	
	pos_index = pos/10;
	ocss_info[car].assigned_calls[pos] |= callmask;
	asslib_xmit_car_ass(car, pos_index, callmask);
}

 /*********************************************************************
 ; Function Name: asslib_rrt_hc_assigned
 ;
 ; Searches between the specified start and finish landings to
 ; determine if the specified type of call exists.
 ;
;********************************************************************/
FLAG_T asslib_rrt_hc_assigned(BYTE_T car, BYTE_T start, BYTE_T finish,
                              INT_T mask)
{
	
	unsigned char floor, up_dir;
	INT_T e_mask;
	
    /* D E T E R M I N E   D I R E C T I O N    O F   S C A N */
    /* ------------------------------------------------------ */
    if (finish > start)
    {
		up_dir = TRUE;
    }
    else
    {
		up_dir = FALSE;
    }
	
	if ((mask & S_FUHC) != 0) e_mask = E_FUHC;
	else if ((mask & S_FDHC) != 0)  e_mask = E_FDHC;
	
    /* I N I T I A L I Z E   F L O O R   P O S I T I O N */
    /* ------------------------------------------------- */
    floor = start;
	
	
    /* T E S T   F O R   C A L L   I N   R A N G E */
    /* ------------------------------------------- */
    while (floor != finish)
    {
		
		/* If a call exists, return TRUE */
		/* ----------------------------- */
		if ( (ocss_info[car].assigned_calls[floor] & e_mask) != 0)
		{
			return(asslib_EXTENDED_CALL);
		}
		else if ( (ocss_info[car].assigned_calls[floor] & mask) != 0)
		{
			return(asslib_STANDARD_CALL);
		}
		
		
		/* Determine whether to increment or decrement floor position */
		/* ---------------------------------------------------------- */
		if (up_dir == TRUE)
		{
			floor++;
		}
		else
		{
			floor--;
		}
    }
	
    return(FALSE);
}


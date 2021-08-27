#include "stdafx.h"
#include <math.h>
#include <crtdbg.h>
#include "HoistwayPlanGen.h"
#include "HoistwayPlannerDefs.h"
#include "HoistwayPlanSelection.h"
#define	EXTERN extern
#include "HoistwaySchedule.h"
#include "HoistwayPlanner.h"
#undef	EXTERN
#include "..\common\2DSIM_DEFS.H"

#ifdef	WITH_DEBUG
#include <time.h>
extern int virtualAssign;
extern int d_passengerNo;
#endif

extern FILE *debug_output;

extern double ESPNSPDelay(double leadDist,			// lead car travel distance
				   double trailingDist,		// trailing car travel distance
				   double jerk,				// max jerk for both cars
				   double accel,			// max acceleration for both cars
				   double velocity,			// max velocity for both cars
				   double initSeparation,	// distance between the origin of the two cars
				   double MSD,				// minimum separation distance
				   double accelForced
				   );

// print lead node assignment
void LeadNodesAssignment::print(FILE *output, int size)
{
	for (int i=0; i<size; i++) 
	{
		if ( assignments[i] >=0)	
			_RPT2(_CRT_WARN, "Lead Node %d: %d\n", i+1, assignments[i]+1);
		else
			_RPT1(_CRT_WARN, "Lead Node %d: No Assignment\n", i+1);
	}
}

// define '=' for LeadNodesAssignment
LeadNodesAssignment & LeadNodesAssignment::operator = (LeadNodesAssignment &other)
{
	lead_nodes_assigned = other.lead_nodes_assigned;
	last_assigned_pocket_index = other.last_assigned_pocket_index;
	memcpy((void *)assignments, (void *)other.assignments, MAX_STOP*sizeof(short));
	return (*this);
}


// print content of the pocket
void Pocket::print( FILE *output)
{
	if (left_node != -1 )
		_RPT1(_CRT_WARN,"Left Floor = %d\n",left_floor);
	else
		_RPT0(_CRT_WARN, "No left node\n");

	if (center_node != -1)  // shs change
		_RPT1(_CRT_WARN,"Center Floor = %d\n",center_floor);
	else
		_RPT0(_CRT_WARN, "No center node\n");

	if (right_node != -1)   // shs change
		_RPT1(_CRT_WARN,"Right Floor = %d\n",right_floor);
	else
		_RPT0(_CRT_WARN, "No right node\n");

}

// print the content of a hoistway plan
void HoistwayPlan::print(FILE *output)
{
	fprintf(output, "Upper Car Sequence:\n");
	sequences[UPPER_CAR].print(sequences[LOWER_CAR], output);

	fprintf(output, "Lower Car Sequence:\n");
	sequences[LOWER_CAR].print(sequences[UPPER_CAR], output);

}


// initialize parameters for a hoistway plan algorithm
HoistwayPlanGen::HoistwayPlanGen(int UH,int UL,int LH,int LL, STRecord *upper_table, STRecord *lower_table)
{
	UpperHighest = UH;
	UpperLowest = UL;
	LowerHighest = LH;
	LowerLowest = LL;
	UpperSeparationTable = upper_table;
	LowerSeparationTable = lower_table;
}

// shs wrapping
/*
void HoistwayPlanGen::InitHoistwayPlanGen(int UH,int UL,int LH,int LL, STRecord *upper_table, STRecord *lower_table)
{
	UpperHighest = UH;
	UpperLowest = UL;
	LowerHighest = LH;
	LowerLowest = LL;
	UpperSeparationTable = upper_table;
	LowerSeparationTable = lower_table;
}

*/
// CSW: new initialization for adopting interface
void HoistwayPlanGen::InitHoistwayPlanGen(BuildingInfo &bdInfo, 
										  HoistwayInfo &hwyInfo)
{
	int hwyId=0;
	UpperHighest = bdInfo.nLandings-1;
	UpperLowest = 1;
	LowerHighest = bdInfo.nLandings -2;
	LowerLowest = 0;
	UpperSeparationTable = new STRecord[bdInfo.nLandings+1];
	LowerSeparationTable = new STRecord[bdInfo.nLandings+1];
	hwyId=hwyInfo.idHwy;
	MakeUpSeparationFloorTable(hwyId);
	/*
	for (int i = 0; i < bdInfo.nLandings; i++)
	{
		UpperSeparationTable[i].current_floor = i;
		UpperSeparationTable[i].the_other_floor = hwyInfo.upperSeparationTable[i];
		LowerSeparationTable[i].current_floor = i;
		LowerSeparationTable[i].the_other_floor = hwyInfo.lowerSeparationTable[i];
	}
	*/
}

void HoistwayPlanGen::MakeUpSeparationFloorTable(int hwyID)
{
	int n_car,ufloor,lfloor,floor;
	int n_totalfloor;
	n_car=hwyInfo[hwyID].nCars;
	n_totalfloor=bdInfo.nLandings;

	// separation table setup from minimum separation distance
	if (n_car>1) // 2-D hoistway
	{	
		ufloor=0;lfloor=0;
		// lower car separation table setting for minumum separatoin table
		for (lfloor=0;lfloor<n_totalfloor;lfloor++)
		{
			for (;ufloor<n_totalfloor;ufloor++)
			{
				if (bdInfo.landingPosition[lfloor]+param.MinSeparationDist<=bdInfo.landingPosition[ufloor])
				{
					LowerSeparationTable[lfloor].current_floor = lfloor;
					LowerSeparationTable[lfloor].the_other_floor = ufloor;
					break;
				}
			}
			if (ufloor>=n_totalfloor)
			{
				for (floor=lfloor;floor<n_totalfloor;floor++)
				{
					LowerSeparationTable[floor].current_floor = floor;
					LowerSeparationTable[floor].the_other_floor = INVALIDFLOOR;
				}
				break;
			}
		}
		// upper car separation table setting for minimum separation distance
		for (ufloor=n_totalfloor-1;ufloor>=0;ufloor--)
		{
			for (;lfloor>=0;lfloor--)
			{
				if (bdInfo.landingPosition[lfloor]+param.MinSeparationDist<=bdInfo.landingPosition[ufloor])
				{
					UpperSeparationTable[ufloor].current_floor = ufloor;
					UpperSeparationTable[ufloor].the_other_floor = lfloor;
					break;
				}
			}
			if (lfloor<0)
			{	// lower zone upper car doesn't have separation floor
				for (;ufloor>=0;ufloor--)
				{	
					UpperSeparationTable[ufloor].current_floor = ufloor;
					UpperSeparationTable[ufloor].the_other_floor = INVALIDFLOOR;
				}
				break;
			}
		}
	}
	else	// 1-D hoistway
	{
		for (floor=0;floor<n_totalfloor;floor++)
		{
			UpperSeparationTable[floor].current_floor = floor;
			UpperSeparationTable[floor].the_other_floor = n_totalfloor-1;
			LowerSeparationTable[floor].current_floor = floor;
			LowerSeparationTable[floor].the_other_floor = 0;
		}
	}

#ifdef	SEPARATION_FLOOR_EXCEPTION
	// set separation distance exception for pit and first user floor, overhead and last user floor
	if (hwyInfo[hwyID].bPit) // pit exist
	{
		for (floor=1; floor <n_totalfloor && UpperSeparationTable[floor].the_other_floor==INVALIDFLOOR;floor++)
		{
			UpperSeparationTable[floor].the_other_floor=0;	// pit
		}
		LowerSeparationTable[0].the_other_floor=1; // lower runby's safey floor is first user floor
	}
	if (hwyInfo[hwyID].bOverhead) // overhead exist
	{
		for (floor=n_totalfloor-2;floor>0 && LowerSeparationTable[floor].the_other_floor==INVALIDFLOOR;floor--)
		{
			LowerSeparationTable[floor].the_other_floor=n_totalfloor-1; // overheaad
		}
		UpperSeparationTable[n_totalfloor-1].the_other_floor=n_totalfloor-2; // upper runby's safey floor is last user floor
	}
#endif

}

// search the minimum separation table for the corresponding minimum separation floor
// if no such floor can be found, return NO_FLOOR
int HoistwayPlanGen::MinimumSeparationFloor(int floor, STRecord *table)
{
	int table_size = UpperHighest - LowerLowest + 1;
	int safe_floor = NO_FLOOR;
	// find the minimum separation floor
	for (int j=0; j<table_size; j++)
	{
		if ( table[j].current_floor == floor )
		{
			safe_floor = table[j].the_other_floor;
			break;
		}
	}
	return safe_floor;
}

// Algorithm for full-collective algorithm
// This algorithm requires input of committed sequence and assigned calls.
// the solution is in the Array feasible_sequences
int HoistwayPlanGen::FullCollective(StopSequence &committed, 
									DestinationEntryCalls &Hcalls, 
									StopSequence &feasible_sequence, 
									unsigned short car_capacity,
									CarState carState)
{

	timer.start();								// start the timer

	feasible_sequence.copy(committed);         // copy committed sequence first
//	DestinationEntryCalls Hcalls;
//	Hcalls.copy(assigned_calls);

	int curr_dir = carState.preDirection;     // set predirection as the current direction
	int start = feasible_sequence.origin();
	int moving_dir = carState.movingDirection;

	// check if there is a call can be picked up at the first and only stop
	// if yes, change the curr_dir as the call direction
	CALL_INDEX_T callIndex = Hcalls.sequence_start;
	CALL_INDEX_T prevInd;
	PLAN_INDEX_T firstStop = feasible_sequence.sequence_start;
	if ( feasible_sequence.sequence_start != -1 && feasible_sequence[firstStop].next == -1)
	{
		FLOOR_T firstFloor = feasible_sequence[firstStop].floor;
		int ind = -1;
		while (callIndex != -1)
		{
			// CSW060207: If there is hall call between first stop and start and there is capacity to pick it up, break
			if ((Hcalls[callIndex].direction() == moving_dir ) &&
				(Hcalls[callIndex].origin * moving_dir < firstFloor* moving_dir ) &&
				(Hcalls[callIndex].origin * moving_dir >= start* moving_dir ) && 
				(feasible_sequence.current_on_board < car_capacity) )
			{
				ind = callIndex;
				break;
			}
			// if there is nobody on board after the first stop, check whether the first call can be picked up
			if ((Hcalls[callIndex].origin == firstFloor) && (feasible_sequence[firstStop].on_board == 0) )
			// CSW0501003: if the first stop is an empty stop
			// if ((Hcalls[callIndex].origin == firstFloor) && feasible_sequence[firstStop].empty )
			{
				// if the current direction is no direction, set the start value				
				if (curr_dir == NO_DIR)
				{
					curr_dir = Hcalls[callIndex].direction();
					ind = callIndex;
					if (curr_dir == DN)
						start = UpperHighest; 
					else
						start = LowerLowest;
					break;
				}
				// CSW060112: Added to handle possible predirection change
				else if (curr_dir == Hcalls[callIndex].direction() )		// if found a call with same direction
				{
					start = firstFloor;
					ind = callIndex;
					break;
				}
				// if the direction is different and the first stop is a demand stop or served stop
				else if (feasible_sequence[firstStop].type == STOP_TYPE_DEMAND || feasible_sequence[firstStop].type == STOP_TYPE_SERVED)
				{
					ind = callIndex;
	//				start = firstFloor;
				}
			}
			prevInd = callIndex;
			callIndex = Hcalls[callIndex].next;
		}
		if (ind != -1)
		{
			curr_dir = Hcalls[ind].direction();
			// if need to pick up a call at first floor, then start == first floor
			if (Hcalls[ind].origin == firstFloor)
				start = firstFloor;
		}
	}


	// if there is no direction, use the direction of the first call
	if ( (curr_dir == NO_DIR) && !Hcalls.empty())
	{
		if (carState.currentNCF == Hcalls[Hcalls.sequence_start].origin)
			curr_dir = Hcalls[Hcalls.sequence_start].direction();					// the direction of the first calls
		else
			curr_dir = GetDir(carState.currentNCF, Hcalls[Hcalls.sequence_start].origin);	// direction to the first call
	}

	Hcalls.sort();				// sort the calls by its origin
	while (!Hcalls.empty() )
	{
		int i, prev_i;
		i = Hcalls.sequence_start;
		// look for calls of the same direction
		while (i != -1)
		{
			if (Hcalls[i].direction() == curr_dir)    // if the calls is of the same direction
			{
				bool flag = false;           // flag for checking the condition
				if ((curr_dir == UP) && (Hcalls[i].origin >= start)) 
					flag = true;
				else if ((curr_dir == DN) && (Hcalls[i].origin <= start))
					flag = true;
				// if pass the test, and the car capacity is enough to merge the call, else do nothing 
				if ( flag && feasible_sequence.check_and_merge(Hcalls, i, car_capacity))
				{
//					Hcalls.MoveOriginForward(i, Hcalls[i].origin);		// move calls with the same origin forward
					if (i == Hcalls.sequence_start)
						Hcalls.sequence_start = Hcalls[i].next;
					else
						Hcalls[prev_i].next = Hcalls[i].next;
					i = Hcalls[i].next;					
				}
				else {
					prev_i = i;
					i = Hcalls[i].next;
				}
			}
			else
			{
				prev_i = i;
				i = Hcalls[i].next;
			}
		}
		int ind, prev_ind;
		// change direction and look the other way
		if (curr_dir == UP)
		{
			curr_dir = DN;
			int highest = LowerLowest;                        // lowest possible floor
			ind = -1;
			i = Hcalls.sequence_start;
			while (i != -1)
			{
				if (Hcalls[i].direction() == curr_dir)    // if the calls is of the same direction
				{
					if (Hcalls[i].origin > highest)
					{
						highest = Hcalls[i].origin;
						ind = i;
						prev_ind = prev_i;
					}
				}
				prev_i = i;
				i = Hcalls[i].next;
			}
			if (ind >= 0)   // if found
			{
				feasible_sequence.append(Hcalls,ind);		  // append it to the feasible solution
				start = highest;                              // update the start position	
//				Hcalls.MoveOriginForward(ind, Hcalls[i].origin);		// move calls with the same origin forward
				if (ind == Hcalls.sequence_start)
					Hcalls.sequence_start = Hcalls[ind].next;
				else
					Hcalls[prev_ind].next = Hcalls[ind].next;
			}	
		}
		else        // direction is  down
		{
			curr_dir = UP;
			int lowest = UpperHighest;            // highest possible floor
			ind = -1;
			i = Hcalls.sequence_start;
			while (i != -1)
			{
				if (Hcalls[i].direction() == curr_dir)    // if the calls is of the same direction
				{
					if (Hcalls[i].origin < lowest)
					{
						lowest = Hcalls[i].origin;
						ind = i;
						prev_ind = prev_i;
					}
				}
				prev_i = i;
				i = Hcalls[i].next;
			}
			if (ind >= 0)   // if found
			{
				feasible_sequence.append(Hcalls,ind);        // append it to the feasible solution
				// Hcalls.MoveOriginForward(ind,Hcalls[ind].origin);		// move calls with the same origin forward
				start = lowest;                              // update the start position
				if (ind == Hcalls.sequence_start)
					Hcalls.sequence_start = Hcalls[ind].next;
				else
					Hcalls[prev_ind].next = Hcalls[ind].next;
			}
		}

	}

	feasible_sequence.UpdateOnBoard();                         // clean up data structure

	fprintf(stdout,"Time Elapsed = %f seconds\n", timer.read());
	feasible_sequence.print(feasible_sequence, stdout);						// print results on screen	
	return 0;
}


/* Proposition 2 for Enumerative Algorithm, Refer to the document 2-D Hoistway Coordination: Design Specification
 * This algorithm checks the assign calls and combine the calls whose direction and origin are the same. 
 * The proposition will be run right after a call is fixed in the committed sequence
 */
PLAN_INDEX_T HoistwayPlanGen::EnumerativeProp2(StopSequence &committed, 
									   DestinationEntryCalls &assigned_calls, 
									   FLOOR_T origin, unsigned short capacity)
{
	int i,prev_i, old_i;               // old_i is for recording the previous i that has been processed for return

	i = assigned_calls.sequence_start;
	old_i = prev_i = -1;

	while (i!= -1) 
	{
		if ( (origin == assigned_calls[i].origin) && committed.check_and_merge(assigned_calls, i, capacity))
		{
			committed.UpdateOnBoard();						// Update On-board info	
			old_i = i;
			i = assigned_calls.RemoveFromList(prev_i);      // remove the call from the list
		}
		else
		{
			prev_i = i;
			i = assigned_calls[i].next;
		}
	}
	return old_i;
}

/* Proposition 3 for Enumerative Algorithm, Refer to the document 2-D Hoistway Coordination: Design Specification
 * This algorithm examines the committed sequence. If it finds a assigned call whose origin is in the sequence already
 * It will combine the assigned calls into the sequence. If the call is in the wrong direction, returns 1, else returns 0
 */
int HoistwayPlanGen::EnumerativeProp3(StopSequence &committed, 
									  DestinationEntryCalls &assigned_calls,
									  unsigned short capacity)
{
	int i = assigned_calls.sequence_start;
	int prev_i = -1;

	// loop through every call, check whether it should be merged into the committed sequence by proposition 3
	while (i != -1)
	{
		int dir = assigned_calls[i].direction();
		int origin = assigned_calls[i].origin;
		int dest = assigned_calls[i].destination;
		PLAN_INDEX_T s = committed.sequence_start;
		bool flag = false;
		while (s != -1)
		{
			int f1, f2, dir2;
			// if the same origin is found, check direction and decide whether this can be combined
			if ( committed[s].floor == origin) {
				if ( -1 == committed[s].next )						// if s is the last stop, append the call at the end
				{
					committed.merge(s, assigned_calls,i);  // merge the node at the end of the sequence
					committed.UpdateOnBoard();						// Update On-board info					
					/*if (i == assigned_calls.sequence_start) 
						assigned_calls.sequence_start = assigned_calls[i].next;
					else
						assigned_calls[prev_i].next = assigned_calls[i].next;
					i = assigned_calls[i].next;
					*/
					i = assigned_calls.RemoveFromList(prev_i);
					flag = true;
					break;
				}
				else 
				{
					PLAN_INDEX_T s2 = committed[s].next;						// get the next stop
					f1 = committed[s].floor;
					f2 = committed[s2].floor;
					dir2 = GetDir(f1, f2);
					if (committed[s2].empty)							// if it is an empty move to the next stop, consider as no direction
						dir2 = NO_DIR;
					if ( (dir == dir2) || (dir2 == NO_DIR))  // merge the call if the same direction or no direction
					{
						int index = s;
						int index_next = s2;
						int flag = TRUE;
						while ( ( index_next != -1) && (committed[index].floor*dir < dest*dir) && (committed[index].floor*dir < committed[index_next].floor*dir)) // CSW060110: Use < instead of <= since the capacity means when the car left
						{
							// if the capacity of the stop is full
							if ( committed[index].on_board >= capacity*PASSENGER_UNIT)
								flag = FALSE;
							index = index_next;
							index_next = committed[index_next].next;
						}	
						
						if (flag)
						{
							committed.check_and_merge(assigned_calls, i, capacity);
							committed.UpdateOnBoard();						// Update On-board info	
							i = assigned_calls.RemoveFromList(prev_i);      // remove the call from the list	
							flag = true;
							break;
						}
					}
					else // call has different direction than the sequence
						return 1;									// violation of feasibility occurs
				}
			}
			s = committed[s].next;				// move to the next s
		}
		if (flag)
		{
			if (prev_i == -1)
				i = assigned_calls.sequence_start;
			else
				i = assigned_calls[prev_i].next;
		}
		else
		{ 
			prev_i = i;
			i = assigned_calls[i].next;
			
		}
	}
	
	return 0;
}

/* Feasible rule 4 specified in the document 2-D Hoistway Coordination: Design Specification
 * This algorithm compare the sequence and the unassigned calls. It will pick up any calls originate between two stops
 * with the same direction
 */
void HoistwayPlanGen::FeasibleRule4(StopSequence &committed, 
									DestinationEntryCalls &assigned_calls, 
									unsigned short capacity)
{

	PLAN_INDEX_T s2, s1 = committed.sequence_start;
	int f1, f2, dir;

	if (-1 == s1)			// no need to check if the first node is null
		return;

	s2 = committed[s1].next;
	if (-1 == s2)			// no need to check if the second node is null
		return;

	while (s2 != -1)
	{
		f1 = committed[s1].floor;
		f2 = committed[s2].floor;

		dir = GetDir(f1,f2);	// determine the direction from s
		int i = assigned_calls.sequence_start;
		int prev_i = -1;

		// if no calls left to be processed
		if (i == -1)
			break;

		while ( i != -1)
		{
			int call_dir = assigned_calls[i].direction();
			if (call_dir == dir)
			{
				int origin = assigned_calls[i].origin;
				int dest = assigned_calls[i].destination;

				// if the origin is between f1 and f2 & calls can be merged 
				if ( (UP == call_dir) && (f1 <= origin) && (f2 > origin))
				{
					int index = s1;
					int index_next = s2;
					int flag = TRUE;
					while ( ( index_next != -1) && (committed[index].floor*dir < dest*dir) && (committed[index].floor*dir < committed[index_next].floor*dir)) // CSW060110: Use < instead of <= since the capacity means when the car left
					{
						// if the capacity of the stop is full
						if ( committed[index].on_board >= capacity*PASSENGER_UNIT)
							flag = FALSE;
						index = index_next;
						index_next = committed[index_next].next;
					}	
					
					if (flag)
					{
						committed.check_and_merge(assigned_calls, i, capacity);
						i = assigned_calls.RemoveFromList(prev_i);      // remove the call from the list	
						s2 = committed[s1].next;				// update the new s2
						f2 = committed[s2].floor;
					}
					else
					{
						prev_i = i;
						i = assigned_calls[i].next;
					}
					
				}
				else if ( (DN == call_dir) && (f1 >= origin) && (f2 < origin))
				{
					int index = s1;
					int index_next = s2;
					int flag = TRUE;
					while ( ( index_next != -1) && (committed[index].floor*dir < dest*dir) && (committed[index].floor*dir < committed[index_next].floor*dir)) // CSW060110: Use < instead of <= since the capacity means when the car left
					{
						// if the capacity of the stop is full
						if ( committed[index].on_board >= capacity*PASSENGER_UNIT)
							flag = FALSE;
						index = index_next;
						index_next = committed[index_next].next;
					}
					if (flag)
					{
						committed.check_and_merge(assigned_calls, i, capacity);	
						i = assigned_calls.RemoveFromList(prev_i);      // remove the call from the list	
						s2 = committed[s1].next;				// update the new s2
						f2 = committed[s2].floor;
					}
					else
					{
						prev_i = i;
						i = assigned_calls[i].next;
					}
				}
				else  // move to the next i
				{
					prev_i = i;
					i = assigned_calls[i].next;
				}
			}
			else   // move to the next i
			{
				prev_i = i;
				i = assigned_calls[i].next;
			}
		}
		s1 = s2;			// advance to the next node
		s2 = committed[s2].next;
	}
	
}

// Algorithm for enumerative all feasible sequences
// This algorithm requires input of committed sequence and assigned calls.
// the solution is in the Array feasible_sequences
int HoistwayPlanGen::Enumerative(StopSequence &committed, 
								 DestinationEntryCalls &assigned_calls, 
								 StopSequence *solution_list, 
								 DestinationEntryCalls *scheduled_calls,
								 unsigned short capacity,
								 CarState state)
{

	int total_sequence = 0;							// number of feasible sequences enumerated
	int stack_ind = 0;

	// output log file
	FILE *output = stdout;
#ifdef WITH_DEBUG
	output = fopen("EnumerativeSequence.txt","w");
	// Print committed sequence & Hall calls
	fprintf(output, "Committed Sequence :");
	committed.PrintSequence(output);
	assigned_calls.PrintCalls(output);
#endif

	timer.start();

	// Initialization
	EnumerativeInstance stack[MAX_STACK_SIZE];			// Use array to represent stack
	// EnumerativeInstance stack[16];			// Use array to represent stack
	// fprintf(debug_output, "Size of EnumerativeInstance = %d, size of DestinationEntryCalls = %d, size of StopSequence = %d\n", sizeof(EnumerativeInstance), sizeof(DestinationEntryCalls), sizeof(StopSequence));

	EnumerativeInstance ei, *new_instance;							// an enumerative instance


	// Initialize the stack
	stack[stack_ind].committed_sequence = committed; 
	stack[stack_ind].Hcalls = assigned_calls; 
	stack[stack_ind].Hcalls.sort();					// sort the calls by its origins
	EnumerativeProp3(stack[stack_ind].committed_sequence, stack[stack_ind].Hcalls, capacity);		// proposition 3 on the instance
	stack[stack_ind].call_to_append = -1;		// means no need to consider append
	stack[stack_ind].call_to_append_prev = -1;

//	EnumerativeProp2(ei.Hcalls);										// Run proposition 2 on assigned calls
	stack_ind ++;


	// Use stack to for enumerate all possibilities
	while (stack_ind != 0)  // shs change != to >
	{
		stack_ind--;

		if (stack[stack_ind].Hcalls.sequence_start == -1)             // if no calls left to be processed
		{
			// add current committed sequence to the existing solution
			if (total_sequence >= MAX_SEQUENCES)
			{
				fprintf(debug_output,"Number of solutions:%d greater than %d in Enumerative function\n",total_sequence, MAX_PLANS);
				break; 
			}
			solution_list[total_sequence] = stack[stack_ind].committed_sequence;
			scheduled_calls[total_sequence] = stack[stack_ind].Hcalls;
			total_sequence++;	
			continue;									// continue the next while
		}

		ei = stack[stack_ind];			// pop the stack
		// append call if necessary
		if (ei.call_to_append != -1)
		{
			int i = ei.call_to_append;
			int prev_i = ei.call_to_append_prev;
			PLAN_INDEX_T last_floor = ei.committed_sequence.LastFloor();
			while ( -1 != i)
			{			
				bool flag = false;
				int call_origin = ei.Hcalls[i].origin;
				int call_dir = ei.Hcalls[i].direction();
				// if down calls whose origin below nearest up call above
				if ((call_origin > last_floor) && (DN == call_dir))
					flag = true;
				// or if up calls whose origin above nearest down call below 
				if ((call_origin < last_floor) && (UP == call_dir))
					flag = true;
				if (flag) 
				{	
		
					ei.committed_sequence.append(ei.Hcalls, i);	// append the call to the end of the sequence	
					ei.Hcalls.RemoveFromList(prev_i);					    // remove the call from the list
					prev_i = EnumerativeProp2(ei.committed_sequence, ei.Hcalls, call_origin, capacity);    // run proposition 3
					// CSW: prev_i will return the last call being processed by EnumerativeProp2
					if (prev_i != -1) 
						i = ei.Hcalls[prev_i].next;
					else
					{
						prev_i = i;
						i = ei.Hcalls[i].next;
					}
					// If there is still call left to be processed, push  next call to stack
					if (i != -1) 
					{
						stack[stack_ind].call_to_append = i;
						stack[stack_ind].call_to_append_prev = prev_i;
						stack_ind++;
					}
					break;
				}
				else 
				{
				  prev_i = i;
				  i = ei.Hcalls[i].next;
				}
			}

		}


		EnumerativeProp3(ei.committed_sequence, ei.Hcalls, capacity);		// proposition 3 on the instance
		FeasibleRule4(ei.committed_sequence, ei.Hcalls, capacity);        // Check Feasible rule 4 

		int last_floor = ei.committed_sequence.LastFloor();
		int above_floor = INFINITE_FLOOR, above_index, above_prev_index;
		int below_floor = NO_FLOOR, below_index, below_prev_index;
		int i = ei.Hcalls.sequence_start;
		int prev_i = -1;
		CHAR flag = false;				// flag for whether up call above or down call below has been found
		// find the calls whose origins are nearest above and below the last stop, to avoid duplicated sequence
		while (i != -1)
		{               
			int call_origin = ei.Hcalls[i].origin;
			int call_dir = ei.Hcalls[i].direction();                   // get direction of the calls
			// if there is an up call above current floor, find the one with the nearest origin
			if (( call_origin >= last_floor) && (call_origin < above_floor) && (UP == call_dir) )
			{
				above_floor = call_origin;
				above_index = i;
				above_prev_index = prev_i;
			}
			// if there is an down call bellow current floor, find the one with the nearest origin
			else if (( call_origin < last_floor) && (call_origin > below_floor) && (DN == call_dir))
			{
				below_floor = call_origin;
				below_index = i;
				below_prev_index = prev_i;
			}
			prev_i = i;
			i = ei.Hcalls[i].next;
		}
		// if there is an up call above current floor, process this call
		if ((above_floor != INFINITE_FLOOR) && (stack_ind < MAX_STACK_SIZE))
		{
			new_instance = &stack[stack_ind++];
			*new_instance = ei;
			new_instance->committed_sequence.append(new_instance->Hcalls, above_index);	// append the call to the end of the sequence
			new_instance->committed_sequence.UpdateOnBoard();
			i = new_instance->Hcalls.RemoveFromList(above_prev_index);					// remove the call from list
			EnumerativeProp2(new_instance->committed_sequence, new_instance->Hcalls, above_floor,capacity);    // run proposition 3
			new_instance->call_to_append = -1;
			flag = true;
		}
		// if there is an down call below current floor, process this call 
		if ((below_floor != NO_FLOOR) && (stack_ind < MAX_STACK_SIZE))
		{
			new_instance = &stack[stack_ind++];
			*new_instance = ei;
			new_instance->committed_sequence.append(new_instance->Hcalls, below_index);	// append the call to the end of the sequence	
			new_instance->committed_sequence.UpdateOnBoard();
			new_instance->Hcalls.RemoveFromList(below_prev_index);					// remove the call from list
			EnumerativeProp2(new_instance->committed_sequence, new_instance->Hcalls, below_floor, capacity);    // run proposition 3
			new_instance->call_to_append = -1;
			flag = true;
		}

		// CSW060110: Include down call above, but less or equal to up call above origin. And up call below, but greater or equal to down call below origin
		i = ei.Hcalls.sequence_start;
		prev_i = -1;
		int down_origin = above_floor +1;
		int up_origin = below_floor -1;
		while (i != -1)
		{
			int call_origin = ei.Hcalls[i].origin;
			int call_dir = ei.Hcalls[i].direction();
			
			// process down call above whose origin is between above_floor and last_floor
			if ( (call_dir == DN) && (call_origin <= above_floor) && (call_origin >= last_floor) && (call_origin != down_origin) )
			{
				down_origin = call_origin;
				new_instance = &stack[stack_ind++];
				*new_instance = ei;
				new_instance->committed_sequence.append(new_instance->Hcalls, i);	// append the call to the end of the sequence	
				new_instance->committed_sequence.UpdateOnBoard();
				new_instance->Hcalls.RemoveFromList(prev_i);					// remove the call from list
				EnumerativeProp2(new_instance->committed_sequence, new_instance->Hcalls, call_origin, capacity);    // run proposition 3
				new_instance->call_to_append = -1;
				flag = true;
			}
			// process up call below whose origin is between belowfloor and last_floor
			if ( (call_dir == UP) && (call_origin >= below_floor) && (call_origin <= last_floor) && (call_origin != up_origin) )
			{
				up_origin = call_origin;
				new_instance = &stack[stack_ind++];
				*new_instance = ei;
				new_instance->committed_sequence.append(new_instance->Hcalls, i);	// append the call to the end of the sequence	
				new_instance->committed_sequence.UpdateOnBoard();
				new_instance->Hcalls.RemoveFromList(prev_i);					// remove the call from list
				EnumerativeProp2(new_instance->committed_sequence, new_instance->Hcalls, call_origin, capacity);    // run proposition 3
				new_instance->call_to_append = -1;
				flag = true;
			}
			prev_i = i;
			i = ei.Hcalls[i].next;
		}
		if ((!flag)&& (stack_ind < MAX_STACK_SIZE)) // push ei to stack, so down calls above and up calls below can be processed
		{
			ei.call_to_append = ei.Hcalls.sequence_start;
			ei.call_to_append_prev = -1;
			stack[stack_ind++] = ei;
		}
	}
	
#ifdef WITH_DEBUG
	// print sequence of stops only
	fprintf(output, "Elapsed time = %f seconds\n", timer.read());
	for (int i = 0; i< total_sequence; i++)
	{
		fprintf(output,"Sequence %d: ",i+1);
		solution_list[i].PrintSequence(output);
	}
	fclose(output);
#endif

	return total_sequence;
}


/*
 * Algorithm to find the next node from start. Return the pointer to the next node and the lowest node along the sequence
 * Return NULL if no such nodes can be found
 */

PLAN_INDEX_T HoistwayPlanGen::FindNextEdgeNode(StopSequence &ss, PLAN_INDEX_T start, PLAN_INDEX_T &lowest)
{
	PLAN_INDEX_T s1, s2;
	int dir;

	// if the first node is an empty move, then there is no lowest point. 
	if (ss[start].empty)
		lowest = -1;
	else
		lowest =start; 

	s2 = -1; 
	s1 = start;
	if (s1!= -1)
	{
		s2 = ss[s1].next;
	}

	// if no next node, there is no next edge node
	if (s2 == -1)
	{
		lowest=-1;
		return s2;
	}

	// if move to the next is empty
	if (ss[s2].empty)
	{
		lowest = -1;			// if there is an empty move, the center node should be -1
		// skip all the empty move
		while (ss[s2].empty)
		{
			s1 = s2;
			s2 = ss[s2].next;
			if (s2 == -1)
				break;
		}
		
		if (s1 == -1)
			return -1;

		s2 = ss[s1].next;
		if (s2 == -1)
			return -1;
	}
	else
	{
		dir = GetDir(ss[s1].floor, ss[s2].floor);
		// search the lowest point, record it if found
		while ( (s2 != -1) && (dir == DN))
		{
			if ( ss[s2].empty)
			{
				lowest = -1;
				break;
			}
			lowest = s2;          // remember the lowest node so far
			s1 = s2;
			s2 = ss[s2].next;
			if (s2 != -1)
				dir = GetDir(ss[s1].floor, ss[s2].floor);
			else           // if no next node, then there is no center node
				lowest = -1;
		}
		if (s2 == -1)
			return -1;
		// CSW050826: if s2 is empty, this pocket does not have lowest point
		else if (ss[s2].empty)
			lowest = -1;
		// CSW050826: end

		// loop through all empty move
		while (ss[s2].empty)
		{
			s1 = s2;
			s2 = ss[s2].next;
			if (s2 == -1)
				break;
		}

		if (s1 == -1)
			return -1;
		else
		{
			s2 = ss[s1].next;
			if (s2 == -1)
				return -1;
		}
	}

	dir = GetDir(ss[s1].floor, ss[s2].floor);
	// if the next direction is down, return the edge
	if (DN == dir)
	{		
		return s1;
	}
	else // climb until empty move or no node or change direction
	{
		// go until next direction is down or next node is empty or next node is NULL
		while ( (s2 != -1) && (dir!=DN))
		{
			s1 = s2;
			s2 = ss[s1].next;
			if (s2 != -1)
			{
				dir = GetDir(ss[s1].floor, ss[s2].floor);
				if (ss[s2].empty)
					break;
			}
		}
		return s1;
	}
	return s1;
}


// Find Pocket defined by a lower car sequence
int HoistwayPlanGen::FindPocket(StopSequence &seq, Pocket *pocket, CarState &state)
{
	int dir;
	int num_pockets = 0;


	if ( seq.sequence_start == -1)
		return num_pockets;

	int s1 = seq.sequence_start, s2;
	s2 = seq[s1].next;

    if (s2 == -1)	// shs debug : one stop should make pocket. if not, there will be no plan
	{				// shs debug add pocket begin
		// Make one pocket which have current floor as the left node
		pocket[num_pockets].left_node=s1;
		pocket[num_pockets].left_floor=seq[s1].floor;
		pocket[num_pockets].center_node=-1;
		pocket[num_pockets].right_node=-1;
		pocket[num_pockets].center_floor = pocket[num_pockets].center_floor = NO_FLOOR;
		num_pockets++;	// shs debug add pocket end
		return num_pockets;
	}


	// initialization for finding pockets
	// Establish left node of the first pocket
	dir = GetDir(seq[s1].floor, seq[s2].floor);

    // if there is nobody on the car
	if (seq.current_on_board == 0)
	{	// if the car has not committed to stop
		if ((seq.current_NCF != state.target) && (seq[s1].type != STOP_TYPE_PIVOT))
		// if (seq.current_NCF != seq[s1].floor)
		{
			pocket[num_pockets].left_node = -1;
			pocket[num_pockets].left_floor = seq.current_NCF;
			pocket[num_pockets].center_node = -1;
			if (DN == dir)
			{
				pocket[num_pockets].right_node = s1;
				pocket[num_pockets].right_floor = seq.origin();
			}
			else
				pocket[num_pockets].right_node = FindNextEdgeNode(seq, s1, pocket[num_pockets].center_node);
		}
		else
		{
			if ( (seq[s1].type == STOP_TYPE_DEMAND) || (seq[s1].type == STOP_TYPE_SERVED)|| (seq[s1].type == STOP_TYPE_PIVOT))
			{
				pocket[num_pockets].left_node = s1;
				pocket[num_pockets].left_floor = seq[s1].floor;
				if (DN == dir)
				{
					pocket[num_pockets].right_node = FindNextEdgeNode(seq, s1, pocket[num_pockets].center_node);
				}
				else
				{
					// if there are passengers on board
					if (seq[s1].on_board>0)
					{
						pocket[num_pockets].center_node = s1;
						pocket[num_pockets].center_floor = seq[s1].floor;
					}
					// use right node instead of center node. 
					pocket[num_pockets].right_node = FindNextEdgeNode(seq, s1, pocket[num_pockets].right_node);
				}
			}
			else 
			{
				// CSW050831:  left node should always be the node
				pocket[num_pockets].left_node = s1;
				pocket[num_pockets].left_floor = seq[s1].floor;
				// CSW050831: end
				pocket[num_pockets].right_node = FindNextEdgeNode(seq, s1, pocket[num_pockets].center_node);
			}
		}
	}
	else if (DN == dir)
	{
		pocket[num_pockets].left_node = s1;
		// pocket[num_pockets].left_floor = seq.origin();
		pocket[num_pockets].left_floor = seq[s1].floor;
		pocket[num_pockets].right_node = FindNextEdgeNode(seq, s1, pocket[num_pockets].center_node);
	}
	else if (UP == dir) //CSW: left node equal to center node equal to s1
	{
		// CSW050901 : add conditional stop if the car is moving
		if (seq.current_NCF < seq[s1].floor)
		{
			PLAN_INDEX_T newInd = seq.NewStop();
			seq[newInd].type = STOP_TYPE_CONDITIONAL;
			seq[newInd].boarding = seq[newInd].deboarding = 0;
			seq[newInd].empty = false;
			seq[newInd].floor = seq.current_NCF;
			seq[newInd].next = seq.sequence_start;
			seq.sequence_start = newInd;
			pocket[num_pockets].left_node = pocket[num_pockets].center_node = newInd;
			pocket[num_pockets].left_floor = pocket[num_pockets].center_floor = seq.current_NCF;
			seq.UpdateOnBoard();
		}
		// CSW050901 : end
		else
		{
			pocket[num_pockets].left_node = pocket[num_pockets].center_node = s1;
			pocket[num_pockets].left_floor = pocket[num_pockets].center_floor= seq[s1].floor;
		}
		pocket[num_pockets].right_node = FindNextEdgeNode(seq, seq.sequence_start, pocket[num_pockets].center_node);
	}
		

	// set center node for the first pocket
	if (pocket[num_pockets].center_node != -1)
		pocket[num_pockets].center_floor=seq[pocket[num_pockets].center_node].floor;
	else
		pocket[num_pockets].center_floor = NO_FLOOR;

	// find the rest of the pockets
	while (pocket[num_pockets].right_node != -1)
	{
		num_pockets ++;
		pocket[num_pockets-1].right_floor = seq[pocket[num_pockets-1].right_node].floor;
		if (pocket[num_pockets-1].center_node != -1)
			pocket[num_pockets-1].center_floor = seq[pocket[num_pockets-1].center_node].floor;
		else
			pocket[num_pockets-1].center_floor = NO_FLOOR;
		// move to the next pockets
		pocket[num_pockets].left_node = pocket[num_pockets-1].right_node;
		pocket[num_pockets].left_floor = pocket[num_pockets-1].right_floor;
		pocket[num_pockets].right_node = FindNextEdgeNode(seq,pocket[num_pockets].left_node, pocket[num_pockets].center_node);
	}

	// add the last pocket to the list
	// the last pocket only has left node
	num_pockets++;

	// print all pockets
	/*
	for (int i=0; i< num_pockets; i++)
	{
		_RPT1(_CRT_WARN,"Pocket %d:\n", i+1);
		pocket[i].print(stdout);
	}
	*/
	return num_pockets;
}


/* 
 * Algorithm for finding lead nodes and recession nodes for the upper car
 */
int HoistwayPlanGen::FindLeadNodes(StopSequence &seq, Pocket *lead_nodes)
{
	int dir = seq.InitialDir();
	int num_lead_nodes = 0;

	int s1 = seq.sequence_start, s2;

	if ( s1 == -1)
		return num_lead_nodes;
	else
	{
		s2 = seq[s1].next;
		// if node s2 is empty or NULL
		if ( (s2 == -1) || (seq[s2].empty))
		{
			lead_nodes[num_lead_nodes].center_node = s1;
			lead_nodes[num_lead_nodes].center_floor = seq[s1].floor;
			lead_nodes[num_lead_nodes].left_node = s1;
			lead_nodes[num_lead_nodes].left_floor = seq[s1].floor;
			lead_nodes[num_lead_nodes].right_node = -1;
			num_lead_nodes++;
		}
	}


	// Loop through all nodes in the sequence
	while (s2 != -1)
	{
		// Loop through all the empty move
		while ((s2 != -1) && (seq[s2].empty))
		{
			s1 = s2;
			s2 = seq[s2].next;
		}

		// CSW051003: Added code to prevent index to be -1
		if (s2 != -1)
			dir = GetDir(seq[s1].floor, seq[s2].floor);
		else
			dir = NO_DIR;

		// if first node, consider the direction from NCF
		if (s1 == seq.sequence_start)
		{
			CHAR dirInit = GetDir(seq.current_NCF, seq[s1].floor);
			if (dirInit == NO_DIR)
			{
				lead_nodes[num_lead_nodes].left_node = s1;
				lead_nodes[num_lead_nodes].left_floor = seq[s1].floor;
			}
			else if (dirInit == DN)
			{
				// if this is an empty move 
				if (seq[s1].empty) 
					lead_nodes[num_lead_nodes].left_node = -1;
				else
				{
					lead_nodes[num_lead_nodes].left_node = s1;
					lead_nodes[num_lead_nodes].left_floor = seq[s1].floor;
				}

			}
			else if (dirInit == UP)
			{
				// if this is an empty move 
				if (seq[s1].empty) 
					lead_nodes[num_lead_nodes].left_node = -1;
				else
				{
					lead_nodes[num_lead_nodes].left_node = s1;
					lead_nodes[num_lead_nodes].left_floor = seq[s1].floor;
				}

			}
		}
	
		if (UP == dir)
		{
			// if not the first node, specify left node
			if (s1 != seq.sequence_start)
			{
				lead_nodes[num_lead_nodes].left_node = -1;
			}
			// lead_nodes[num_lead_nodes].left_node = -1;
			lead_nodes[num_lead_nodes].center_node = s1;
			lead_nodes[num_lead_nodes].center_floor = seq[s1].floor;
			// find the right node

			while ((s2 != -1) && (! seq[s2].empty))
			{
				dir = GetDir(seq[s1].floor, seq[s2].floor);
				if (DN == dir)
					break;
				s1 = s2;
				s2 = seq[s2].next;			
			}
			// lead_nodes[num_lead_nodes].right_node = s1;
			// lead_nodes[num_lead_nodes].right_floor = seq[s1].floor;
			// CSW050909: check the condition to whether this should be empty
			if ((s2 != -1) && (seq[s2].empty))
			{
				lead_nodes[num_lead_nodes].right_node = -1;

			}
			else
			{
				lead_nodes[num_lead_nodes].right_node = s1;
				lead_nodes[num_lead_nodes].right_floor = seq[s1].floor;
			}
			// CSW050909: end
		}
		else
		{
			// if this is not the first node, specify the left node 
			if (seq.sequence_start != s1)
			{
				// if this is an empty move 
				if (seq[s1].empty) 
						lead_nodes[num_lead_nodes].left_node = -1;
				else
				{
					lead_nodes[num_lead_nodes].left_node = s1;
					lead_nodes[num_lead_nodes].left_floor = seq[s1].floor;
				}
			}
			
			// find the lead node
			while ((s2 != -1) && (!seq[s2].empty))
			{
				dir = GetDir(seq[s1].floor, seq[s2].floor);
				if (UP == dir)
					break;
				s1 = s2;
				s2 = seq[s2].next;			
			}
			lead_nodes[num_lead_nodes].center_node = s1;
			lead_nodes[num_lead_nodes].center_floor = seq[s1].floor;
	
			if ( (s2 == -1) || (seq[s2].empty))
			{
				lead_nodes[num_lead_nodes].right_node = -1;
			}
			else
			{
				// find the right recession node
				while ((s2 != -1) && (! seq[s2].empty))
				{
					dir = GetDir(seq[s1].floor, seq[s2].floor);
					if (DN == dir)
						break;
					s1 = s2;
					s2 = seq[s2].next;			
				}
				// lead_nodes[num_lead_nodes].right_node = s1;
				// lead_nodes[num_lead_nodes].right_floor = seq[s1].floor;
				// CSW050909: check the condition to whether this should be empty
				if ((s2 != -1) && (seq[s2].empty))
				{
					lead_nodes[num_lead_nodes].right_node = -1;

				}
				else
				{
					lead_nodes[num_lead_nodes].right_node = s1;
					lead_nodes[num_lead_nodes].right_floor = seq[s1].floor;
				}
				// CSW050909: end
			}
		}		
		num_lead_nodes++;		
	}

	// print all lead nodes
	/*
	for (int i=0; i< num_lead_nodes; i++)
	{
		_RPT1(_CRT_WARN,"Lead Nodes %d:\n", i+1);
		lead_nodes[i].print(stdout);
	}
	*/
    return num_lead_nodes;
}

/*
 * Algorithm for enumerating all possible assignment of nodes to pockets
 */
int HoistwayPlanGen::EnumerateNodesAssignment(StopSequence &upper_sequence, 
											  StopSequence &lower_sequence, 
											  LeadNodesAssignment *assignments, 
											  Pocket *lead_nodes, 
											  Pocket *pockets,
											  int total_nodes,
											  int total_pockets)
{
	LeadNodesAssignment stack[MAX_STACK_SIZE], ass;

	// initialize stack for enumeration
	int stack_ind = 1;
	stack[0].last_assigned_pocket_index = stack[0].lead_nodes_assigned = 0;
	int total_solutions = 0;

	// set initial values of the assignment
	for (int i=0; i<total_nodes; i++)
		stack[0].assignments[i] = -1;

	while (stack_ind != 0)
	{
		// pop the first node from the stack
		stack_ind --; 
		ass = stack[stack_ind];

		PLAN_INDEX_T left_node = lead_nodes[ass.lead_nodes_assigned].left_node;
		int left_safe_floor = LowerLowest;
		// find minimum separation floor for left node
		if (left_node != -1)
		{
			int left_floor = lead_nodes[ass.lead_nodes_assigned].left_floor;
			left_safe_floor = MinimumSeparationFloor(left_floor, UpperSeparationTable);
		}

		PLAN_INDEX_T lead_node = lead_nodes[ass.lead_nodes_assigned].center_node;
		int lead_floor = lead_nodes[ass.lead_nodes_assigned].center_floor;
		int safe_floor = MinimumSeparationFloor(lead_floor, UpperSeparationTable);
		// Check assigning lead nodes to each of the pocket
		for (int i = ass.last_assigned_pocket_index; i<total_pockets; i++) 
		{
			// if there is a left recession node		
			if (left_node != -1) 
			{
				// if the left node is not empty
					// check whether it can pass the left edge of the pocket				
					// check the left edge of the pocket
					if ( (pockets[i].left_floor > left_safe_floor) && (i != ass.last_assigned_pocket_index))
						break;		// break the for loop, it is not possible to assign the node to this pocket, or pockets after
			}
			
			// check lead node
			PLAN_INDEX_T center_node = pockets[i].center_node;
			// if there is a center node, check whether the lead node can be safely put inside
			if (center_node != -1)
			{
				if ( (pockets[i].center_floor > safe_floor))
					continue;		// This pocket cannot accommodate the node, check the next pocket
			}
			
			// create a new node and put it into stack
			if (stack_ind < MAX_STACK_SIZE)
			{
				if ( ( safe_floor < pockets[i].left_floor) ||  ( safe_floor < pockets[i].right_floor))
				{
					stack[stack_ind] = ass;
					stack[stack_ind].last_assigned_pocket_index = i;
					stack[stack_ind].assignments[ass.lead_nodes_assigned] = i;
					stack[stack_ind].lead_nodes_assigned++;
					stack_ind ++;
				}
				else
				{
					stack[stack_ind] = ass;
					stack[stack_ind].last_assigned_pocket_index = i;
					stack[stack_ind].assignments[ass.lead_nodes_assigned] = -1;
					stack[stack_ind].lead_nodes_assigned++;
					stack_ind ++;
				}		
			}
			// if all nodes are assigned
			if (stack[stack_ind-1].lead_nodes_assigned >= total_nodes)
			{	
				stack_ind --;
				assignments[total_solutions] = stack[stack_ind];
				total_solutions ++;
				if (total_solutions >= MAX_PLANS)
					return total_solutions;
			}
		}
		// process the node
	}
	return total_solutions;
}

/*
 * Algorithm for generating a coordinated plan for a feasible assignment
 */
int HoistwayPlanGen::GenerateHoistwayPlan(HoistwayPlan &feasible_plans, 
								  LeadNodesAssignment &lns,
								  Pocket *origin_lead_nodes,
								  Pocket *origin_pockets,
								  int num_lead_nodes,
								  int num_pockets,
								  CarState &uCarState,
								  CarState &lCarState)
{
	Pocket lead_nodes[MAX_STOP], pockets[MAX_STOP];
	PLAN_INDEX_T s1 , s2, s3;

	// copy lead node and pocket info to local variables
	memcpy(lead_nodes, origin_lead_nodes, num_lead_nodes*sizeof(Pocket));
	memcpy(pockets, origin_pockets, num_pockets*sizeof(Pocket));

	// check the first left node of the upper sequence
	// if there is a need for yield node, create it
	int pocketInd = lns.assignments[0];
	PLAN_INDEX_T nodeIndex;
	short h = NO_FLOOR;
	// find the highest node and index
	for (int i=0; i<=pocketInd; i++)
	{
		// CSW: Take the latest node whose floor greater or equal to h 
		if (  (pockets[i].left_node!= -1) &&(pockets[i].left_floor >= h))
		{
			h = pockets[i].left_floor; 
			nodeIndex = pockets[i].left_node;
		}
		// CSW050910: Add to make sure that right node is considered too
		if ( (i != pocketInd) && (pockets[i].right_node!= -1) &&(pockets[i].right_floor >= h))
		{
			h = pockets[i].right_floor; 
			nodeIndex = pockets[i].right_node;
		}
		// CSW050910: end
	}

	// check whether yield node is needed to clear the previous pockets
	if (h != NO_FLOOR)
	{
		int safe_floor = MinimumSeparationFloor(h,LowerSeparationTable);
		PLAN_INDEX_T leftIndex = lead_nodes[0].left_node;
		if (leftIndex == -1)
		{
			leftIndex = feasible_plans.sequences[UPPER_CAR].sequence_start;
			lead_nodes[0].left_node = leftIndex;
			lead_nodes[0].left_floor = feasible_plans.sequences[UPPER_CAR][leftIndex].floor;
		}
		if (feasible_plans.sequences[UPPER_CAR][leftIndex].floor < safe_floor)
		{
			PLAN_INDEX_T newInd; 
			Stop *new_stop;
			// CSW051012: If the stop is actually a yield already, use the index
			if (feasible_plans.sequences[UPPER_CAR][leftIndex].type == STOP_TYPE_YIELD)
			{
				newInd = leftIndex;
				new_stop = &feasible_plans.sequences[UPPER_CAR][newInd];
			}
			else
			{
				newInd= feasible_plans.sequences[UPPER_CAR].NewStop();
				new_stop = &feasible_plans.sequences[UPPER_CAR][newInd];
				new_stop->next = feasible_plans.sequences[UPPER_CAR].sequence_start;
				feasible_plans.sequences[UPPER_CAR].sequence_start = newInd;
			}
			new_stop->floor = safe_floor;
			new_stop->boarding = new_stop->deboarding = 0;
			new_stop->empty = true;						// it should be an ampty move since this is an yield move
			new_stop->type = STOP_TYPE_YIELD;			// This is a yield stop
			if (GetDir(feasible_plans.sequences[UPPER_CAR].current_NCF, safe_floor) == UP)
				feasible_plans.AddArrivalPrecedence(UPPER_CAR, newInd, nodeIndex);
			else
			{
				feasible_plans.AddArrivalPrecedence(LOWER_CAR, nodeIndex, newInd);
				new_stop->type = STOP_TYPE_CONDITIONAL;			// This is a conditional stop
			}

			lead_nodes[0].left_node = newInd;
			lead_nodes[0].left_floor = safe_floor;

		}
		else if (feasible_plans.sequences[UPPER_CAR][leftIndex].floor == safe_floor)
		{
			PLAN_INDEX_T nextIndex = feasible_plans.sequences[LOWER_CAR][nodeIndex].next;
			if ( (nextIndex != -1) && (GetDir(h , feasible_plans.sequences[LOWER_CAR][nextIndex].floor) == DN))
			{
				feasible_plans.AddDeparturePrecedence(LOWER_CAR, nodeIndex, leftIndex);
			}
		}
	}


	// end of checking whether yield node is needed to clear the previous pockets

	int prev_pocket = -1;
	// loop through all lead node assignment
	for (int j=0; j<num_lead_nodes; j++)
	{
		int pocket_ind = lns.assignments[j];				// the index of pocket the node is assigned to
		// if the node is not assigned, check whether conditioal stop needed, then continue to the next pocket
		if (pocket_ind <0)
		{
			// CSW050804: Add conditional stop to the next pocket if necessary
			if (prev_pocket == -1)
				s1 = feasible_plans.sequences[UPPER_CAR].sequence_start;
			else 
				s1 = lead_nodes[j].center_node;

			int next_pocket = lns.assignments[j+1];
			int safe_floor = NO_FLOOR;
			// find the highest edge node need to be passed
			for (int k = prev_pocket+1; k<= next_pocket; k++)
			{
				int node = pockets[k].left_node;		
				if ((node != -1) && (feasible_plans.sequences[LOWER_CAR][node].floor > safe_floor))
				{
					s2 = node;
					safe_floor = feasible_plans.sequences[LOWER_CAR][node].floor;
				}
				// CSW050910: Add to make sure that right node is considered too
				node = pockets[k].right_node;		
				if ((k != next_pocket) && (node != -1) && (feasible_plans.sequences[LOWER_CAR][node].floor > safe_floor))
				{
					s2 = node;
					safe_floor = feasible_plans.sequences[LOWER_CAR][node].floor;
				}
				// CSW050910: end
			}
			if (safe_floor != NO_FLOOR)
			{
				safe_floor = MinimumSeparationFloor(safe_floor,LowerSeparationTable);
				if ((s3 = feasible_plans.sequences[UPPER_CAR][s1].next) != -1)
				{
					if ( feasible_plans.sequences[UPPER_CAR][s3].floor < safe_floor)
					{
						if (feasible_plans.sequences[UPPER_CAR][s1].floor > safe_floor)
						{
							PLAN_INDEX_T new_index = feasible_plans.sequences[UPPER_CAR].NewStop();
							Stop *new_stop = &feasible_plans.sequences[UPPER_CAR][new_index];
							new_stop->floor = safe_floor;
							new_stop->boarding = new_stop->deboarding = 0;
							new_stop->empty = feasible_plans.sequences[UPPER_CAR][s3].empty;		// empty or not should be the same as the next node
							new_stop->type = STOP_TYPE_CONDITIONAL;			// it is a conditional stop
							feasible_plans.AddArrivalPrecedence(LOWER_CAR,s2, new_index);
							feasible_plans.AddDeparturePrecedence(LOWER_CAR,s2, new_index);
							new_stop->next = s3;
							feasible_plans.sequences[UPPER_CAR][s1].next = new_index;
						}
						else if (feasible_plans.sequences[UPPER_CAR][s1].floor == safe_floor)
						{
							feasible_plans.AddArrivalPrecedence(LOWER_CAR,s2, s1);
							feasible_plans.AddDeparturePrecedence(LOWER_CAR,s2, s1);
						}
						else if (feasible_plans.sequences[UPPER_CAR][s1].floor < safe_floor)
						{
							PLAN_INDEX_T new_index;
							Stop *new_stop;
							// CSW051012: Added to merge yield stop when encountered
							if (feasible_plans.sequences[UPPER_CAR][s1].type == STOP_TYPE_YIELD)
							{
								new_index = s1;
								new_stop = &feasible_plans.sequences[UPPER_CAR][s1];
							}
							else if (s3 != -1 && feasible_plans.sequences[UPPER_CAR][s3].type == STOP_TYPE_YIELD)
							{
								new_index = s3;
								new_stop = &feasible_plans.sequences[UPPER_CAR][s3];
							}
							else
							{
								new_index = feasible_plans.sequences[UPPER_CAR].NewStop();
								new_stop = &feasible_plans.sequences[UPPER_CAR][new_index];
								new_stop->next = s3;
								feasible_plans.sequences[UPPER_CAR][s1].next = new_index;
							}
							new_stop->floor = safe_floor;
							new_stop->boarding = new_stop->deboarding = 0;
							new_stop->empty = feasible_plans.sequences[UPPER_CAR][s3].empty;		// empty or not should be the same as the next node
							new_stop->type = STOP_TYPE_YIELD;			// it is a conditional stop
							feasible_plans.AddArrivalPrecedence(LOWER_CAR,s2, new_index);
							feasible_plans.AddDeparturePrecedence(LOWER_CAR,s2, new_index);
							s1 = new_index;
						}
						lead_nodes[j].right_node = lead_nodes[j+1].left_node = s1; // update new recession node
						lead_nodes[j].right_floor = lead_nodes[j+1].left_floor = s1; // update new recession floor
					}
				}
			}
			// CSW050804: end
			continue;
		}
		// end of handling unassigned node


		// if currently there is no center node in the assigned pocket, check whether there is a need to create one
		if (pockets[pocket_ind].center_node == -1)
		{
			int safe_floor = MinimumSeparationFloor(lead_nodes[j].center_floor,UpperSeparationTable);
			for (int k = j+1; k< num_lead_nodes; k++)
			{
				// if the node is assigned to the same pocket
				if (lns.assignments[k] == pocket_ind)
				{
					// find the node woth the lowest minimum separation floor 
					int floor = MinimumSeparationFloor(lead_nodes[k].center_floor,UpperSeparationTable);
					if (floor < safe_floor) 
						safe_floor = floor;
				}
				else		// if the next node is not assigned to the same pocket, break the for loop
					break;
			}
		
			CHAR flag = false;			// flag for testing whether there is a need for new center node
			s1 = pockets[pocket_ind].left_node;   // start looking from left edge of the pocket
			if (s1==-1)  // CSW: can only be -1 for the first pocket
			{
				// CSW050829: check the first node to see if there is a need to add yield node
				s2 = feasible_plans.sequences[LOWER_CAR].sequence_start;
				if (safe_floor < feasible_plans.sequences[LOWER_CAR][s2].floor)
				{
					PLAN_INDEX_T new_ind;
					Stop *new_stop;
					// CSW051012 : merging yield stop if the type of the first stop is yield
					if (feasible_plans.sequences[LOWER_CAR][s2].type == STOP_TYPE_YIELD)
					{
						new_ind = s2;
						new_stop = &feasible_plans.sequences[LOWER_CAR][s2];
					}
					else
					{
						new_ind = feasible_plans.sequences[LOWER_CAR].NewStop();
						new_stop = &feasible_plans.sequences[LOWER_CAR][new_ind];
						new_stop->next=feasible_plans.sequences[LOWER_CAR].sequence_start;
						feasible_plans.sequences[LOWER_CAR].sequence_start=new_ind;
					}
					new_stop->floor = safe_floor;
					new_stop->boarding = new_stop->deboarding = 0;
					new_stop->empty = true;						// it should be an empty move since this is an yield move
					new_stop->type = STOP_TYPE_YIELD;			// This is a yield stop
					pockets[pocket_ind].left_node = new_ind;
					pockets[pocket_ind].left_floor = safe_floor;
					pockets[pocket_ind].center_node = new_ind;		// use this as the new center node
					pockets[pocket_ind].center_floor = safe_floor;	// update center floor information	
				}
				else
				{
					pockets[pocket_ind].left_node =pockets[pocket_ind].center_node =  s2;
					pockets[pocket_ind].left_floor = pockets[pocket_ind].center_floor = feasible_plans.sequences[LOWER_CAR][s2].floor;
				}
			}
			else
			{		// shs debug end
				s2 = feasible_plans.sequences[LOWER_CAR][s1].next; 
				while ( (s2 != -1) && (!feasible_plans.sequences[LOWER_CAR][s2].empty) )					// search the first empty move
				{
					s1 = s2;
					s2 = feasible_plans.sequences[LOWER_CAR][s2].next;
				}
				PLAN_INDEX_T lowest_node = s1;
				FLOOR_T lowest_floor = feasible_plans.sequences[LOWER_CAR][s1].floor;
				if (s2!= -1 && feasible_plans.sequences[LOWER_CAR][s2].floor < lowest_floor)
				{
					lowest_node = s2;
					lowest_floor = feasible_plans.sequences[LOWER_CAR][s2].floor;
				}
				if ( lowest_floor > safe_floor)
				{
					PLAN_INDEX_T new_ind; 
					Stop *new_stop;
					// CSW051012: Check whether s1 or s2 is yield stop, if yes, merge the yield stop
					if ( feasible_plans.sequences[LOWER_CAR][s1].type == STOP_TYPE_YIELD)
					{
						new_ind = s1;
						new_stop = &feasible_plans.sequences[LOWER_CAR][s1];
					}
					else if (s2 != -1 && feasible_plans.sequences[LOWER_CAR][s2].type == STOP_TYPE_YIELD)
					{
						new_ind = s2;
						new_stop = &feasible_plans.sequences[LOWER_CAR][s2];

					}
					else
					{
						new_ind = feasible_plans.sequences[LOWER_CAR].NewStop();
						new_stop = &feasible_plans.sequences[LOWER_CAR][new_ind];
						feasible_plans.sequences[LOWER_CAR][new_ind].next = s2;			// insert the stop
						feasible_plans.sequences[LOWER_CAR][s1].next = new_ind;

					}
					new_stop->floor = safe_floor;
					new_stop->boarding = new_stop->deboarding = 0;
					new_stop->empty = true;						// it should be an ampty move since this is an yield move
					new_stop->type = STOP_TYPE_YIELD;			// This is a yield stop		
					pockets[pocket_ind].center_node = new_ind;		// use this as the new center node
					pockets[pocket_ind].center_floor = safe_floor;	// update center floor information						
				}
				else	// specify the lowest node as center node
				{					
					pockets[pocket_ind].center_node = lowest_node;		// use this as the new center node
					pockets[pocket_ind].center_floor = lowest_floor ;	// update center floor information	
				}
			}
		}
		// end of handling no center node situation
	
		// compare center node of both lead nodes and pockets, add precedence relationship if necessary 
		s1 = pockets[pocket_ind].center_node;
		s2 = lead_nodes[j].center_node;
		int safe_floor = MinimumSeparationFloor(feasible_plans.sequences[UPPER_CAR][s2].floor,UpperSeparationTable);
		if (safe_floor == pockets[pocket_ind].center_floor)
		{
			CHAR dirPocket = GetDir(pockets[pocket_ind].left_floor, pockets[pocket_ind].center_floor);
			if (DN == dirPocket)
			{
				feasible_plans.AddArrivalPrecedence(LOWER_CAR,s1, s2);
			}
			PLAN_INDEX_T next = feasible_plans.sequences[LOWER_CAR][s1].next;
			if (next != -1)
				dirPocket = GetDir(pockets[pocket_ind].center_floor, feasible_plans.sequences[LOWER_CAR][next].floor);
			else
				dirPocket = NO_DIR;

			if (UP == dirPocket) 
			{
				// check whether the previous lead nodes has set the predecessor constraints 
				feasible_plans.AddDeparturePrecedence(UPPER_CAR, s2, s1);
			}
		}
		// end of comparing lead nodes and center nodes

		// check whether it is necessary to create a yield stop for the upper car to clear right edge of pocket
		s2 = pockets[pocket_ind].right_node;
		s1 = lead_nodes[j].right_node;
		// if no right node, use left node of the next lead nodes
		if (s1 == -1)
				s1 = lead_nodes[j+1].left_node;
		// if there is no right node, start from center node and keep climbing until the next node is empty or down
		if (s1 == -1)
		{
			s1 = lead_nodes[j].center_node;
			s3 = feasible_plans.sequences[UPPER_CAR][s1].next;
			// CSW050825: keep searching until encounter empty move or down
			while (s3 != -1) 
			{
				//CSW050908: update right node and left node based on current situation
				if (GetDir(feasible_plans.sequences[UPPER_CAR][s1].floor, feasible_plans.sequences[UPPER_CAR][s3].floor) != UP)
				{
					lead_nodes[j].right_node = lead_nodes[j+1].left_node = s1;
					lead_nodes[j].right_floor = lead_nodes[j+1].left_floor = feasible_plans.sequences[UPPER_CAR][s1].floor;
					break;
				}
				// if move to the next is empty, break too
				if (feasible_plans.sequences[UPPER_CAR][s3].empty)
				{
					if (feasible_plans.sequences[UPPER_CAR][s1].floor < feasible_plans.sequences[UPPER_CAR][s3].floor)
					{
						lead_nodes[j].right_node  = s1;
						lead_nodes[j].right_floor =  feasible_plans.sequences[UPPER_CAR][s1].floor;
						lead_nodes[j+1].left_node = s3;
						lead_nodes[j+1].left_floor = feasible_plans.sequences[UPPER_CAR][s3].floor;
					}
					else
					{
						lead_nodes[j].right_node  = s1;
						lead_nodes[j].right_floor =  feasible_plans.sequences[UPPER_CAR][s1].floor;
						// use left node from the start
						lead_nodes[j+1].left_node = lead_nodes[j].center_node;
						lead_nodes[j+1].left_floor = lead_nodes[j].center_floor;
					}
					break;
				}
				// CSW050908: end
				s1 = s3;
				s3 = feasible_plans.sequences[UPPER_CAR][s3].next;
			}
			lead_nodes[j].right_node = s1;
			lead_nodes[j].right_floor = feasible_plans.sequences[UPPER_CAR][s1].floor;
			// CSW050825: end
		}
		// end of locating appropriate right node

		// check next pocket assignment as well
		int next_pocket = -1;
		if (j < num_lead_nodes-1)
			next_pocket = lns.assignments[j+1];
		// if there is a right pocket node, and next lead node is not assigned to the same pocket
		if ((s2 != -1) && (next_pocket != pocket_ind))
		{
			int safe_floor = MinimumSeparationFloor(feasible_plans.sequences[LOWER_CAR][s2].floor, LowerSeparationTable);
			// if s1 is the last leading node, the right node need to clear any remaining pockets
			if (next_pocket == -1)
				next_pocket = num_pockets-1;
			// loop throught the remaining pockets
			for (int k=pocket_ind+1; k<= next_pocket; k++)
			{
				// find the pocket with the highest edge node, update the floor to clear the node
				if ( pockets[k].left_floor > feasible_plans.sequences[LOWER_CAR][s2].floor)
				{
					s2 = pockets[k].left_node;
					safe_floor = MinimumSeparationFloor(feasible_plans.sequences[LOWER_CAR][s2].floor, LowerSeparationTable);
				}
				// CSW050910: Add to make sure that right node is considered too
				if ((pockets[k].right_node!= -1) &&(k != next_pocket) &&( pockets[k].right_floor > feasible_plans.sequences[LOWER_CAR][s2].floor))
				{
					s2 = pockets[k].right_node;
					safe_floor = MinimumSeparationFloor(feasible_plans.sequences[LOWER_CAR][s2].floor, LowerSeparationTable);
				}
				// CSW050910: end
			}
			
			// CSW050908: Modified for yield stop on the right edge  
			s3 = feasible_plans.sequences[UPPER_CAR][s1].next;
			FLOOR_T f3, f1 = feasible_plans.sequences[UPPER_CAR][s1].floor;
			if (s3 == -1)
				f3 = f1;
			else
				f3 = feasible_plans.sequences[UPPER_CAR][s3].floor;
			// if the right recession node does not clear the right edge node of the pocket
			if (f1 <= safe_floor)
			{
				if ((f1 < safe_floor) && (f3 < safe_floor))
				{
					PLAN_INDEX_T new_index; 
					Stop *new_stop; 
					// CSW051012: Added to merge yield stop when encountered
					if (feasible_plans.sequences[UPPER_CAR][s1].type == STOP_TYPE_YIELD)
					{
						new_index = s1;
						new_stop = &feasible_plans.sequences[UPPER_CAR][s1];
					}
					else if (s3 != -1 && feasible_plans.sequences[UPPER_CAR][s3].type == STOP_TYPE_YIELD)
					{
						new_index = s3;
						new_stop = &feasible_plans.sequences[UPPER_CAR][s3];
					}
					else
					{
						new_index = feasible_plans.sequences[UPPER_CAR].NewStop();
						new_stop = &feasible_plans.sequences[UPPER_CAR][new_index];
						new_stop->next = s3;
						feasible_plans.sequences[UPPER_CAR][s1].next = new_index;
					}
					new_stop->floor = safe_floor;
					new_stop->boarding = new_stop->deboarding = 0;
					new_stop->empty = true;				// empty or not should be the same as the next node
					new_stop->type = STOP_TYPE_YIELD;			// it is a yield stop
					feasible_plans.AddArrivalPrecedence(UPPER_CAR,new_index, s2);
					lead_nodes[j].right_node = lead_nodes[j+1].left_node = new_index; // update new recession node
					lead_nodes[j].right_floor = lead_nodes[j+1].left_floor = safe_floor; // update new recession floor
					// check whether there is a need to add departure precedence
					if ( (s3 != -1) && (GetDir(safe_floor,f3) == DN)) 
						feasible_plans.AddDeparturePrecedence(LOWER_CAR,s2, new_index);  // lower car should go before upper car can go
					// s1 = new_index;						// move s1 to the next stop
				}  
				else if ((f1 == safe_floor) && (lead_nodes[j].center_floor < f1)) // if it is exactly the same, and upward motion
				{
					feasible_plans.AddArrivalPrecedence(UPPER_CAR,s1, s2);
				}
				else if ( (f3 == safe_floor) && (f1 < f3))
				{
					feasible_plans.AddArrivalPrecedence(UPPER_CAR,s3, s2);
				}
			}		
			// end of right node does not clear the right edge of pocket
		}
		// end of handling next node not assigned to the same pocket

	}

	for (j=0; j<num_lead_nodes; j++)
	{
		int pocket_ind = lns.assignments[j];				// the index of pocket the node is assigned to
		// if the node is not assigned, check whether conditioal stop needed, then continue to the next pocket
		if (pocket_ind <0)
			continue;
		s1 = lead_nodes[j].left_node;
		//  check whether the check should start from the previous lead nodes
		if  (j > 0) 
		{
			if ( (lead_nodes[j-1].right_node != -1 ) && (lead_nodes[j-1].right_floor > pockets[pocket_ind].left_floor))
				s1 = lead_nodes[j-1].right_node;
			else if (lead_nodes[j-1].center_floor > pockets[pocket_ind].left_floor)
				s1 = lead_nodes[j-1].center_node;
		}

		// CSW050822: Check whether there is a need to update the left node of the next pocket
		// if the it is pure downward from the previous right node, it should be updated
		if (( prev_pocket != pocket_ind) && (pocket_ind !=0))
		{
			PLAN_INDEX_T lowerIndex, lowerIndex1;
			if (prev_pocket >= 0)
				lowerIndex = lowerIndex1 = pockets[prev_pocket].right_node;
			else
			{
				lowerIndex = lowerIndex1 = pockets[0].center_node;
				if ((pockets[0].right_node != -1) && (pockets[0].right_floor > pockets[0].center_floor))
					lowerIndex = lowerIndex1 = pockets[0].right_node;

			}
			while (lowerIndex != pockets[pocket_ind].left_node)
			{
				PLAN_INDEX_T nextIndex = feasible_plans.sequences[LOWER_CAR][lowerIndex].next; 
				if (feasible_plans.sequences[LOWER_CAR][lowerIndex].floor > feasible_plans.sequences[LOWER_CAR][nextIndex].floor)
					lowerIndex = nextIndex;
				else
					break;
			}
			// if it is downward all the way, use the previous pocket left node as the left node of the next pocket
			if (lowerIndex == pockets[pocket_ind].left_node)
				pockets[pocket_ind].left_node = lowerIndex1;
		}
		// CSW050822: end
		PLAN_INDEX_T limit1, limit2;							// limit nodes for end of while loop
		// if there is a left recession node, compare downward sequence against left part of the pocket
		if (s1 != -1)
		{
			s3 = feasible_plans.sequences[UPPER_CAR][s1].next;
			
			// step through left part of the pocket, compare against the downward sequence of upper car
			// set limit to be the center node
			limit2 = lead_nodes[j].center_node; 
			// CSW050908: go further if necessry to accomodate the situation where the lead node can extend to next pocket
			PLAN_INDEX_T limitIndex = feasible_plans.sequences[UPPER_CAR][limit2].next;
			if (( limitIndex != -1) &&( feasible_plans.sequences[UPPER_CAR][limit2].floor > feasible_plans.sequences[UPPER_CAR][limitIndex].floor))
				limit2 = limitIndex;		
			// CSW050908: end

			// CSW050909: Consider situation where there are more than one pocket between two lead nodes assignment
			PLAN_INDEX_T startInd = prev_pocket;
			if (startInd != pocket_ind)
				startInd++;
			

			for (int k = startInd; k <= pocket_ind; k++)
			{
				// if k is from previous pocket, find the proper section from the previous section
				if (k != pocket_ind)
				{
					SHORT floorLimit = NO_FLOOR;
					for (int l = k+1; l<= pocket_ind; l++)
					{
						if (pockets[l].left_floor > floorLimit)
							floorLimit = pockets[l].left_floor;
					}
					// if there are larger left floors, no need to consider pokcet k, condinue
					if (floorLimit >= pockets[k].left_floor)
						continue;
				
					s2 = limit1 = pockets[k].left_node;
					while ((limit1 != -1) && (limit1 != pockets[k].center_node))
					{
						if (feasible_plans.sequences[LOWER_CAR][limit1].floor < floorLimit)
							break;
						limit1 = feasible_plans.sequences[LOWER_CAR][limit1].next;
					}
				}
				else
				{
					s2 = pockets[pocket_ind].left_node;  // get the left most edge of the pocket
					limit1 = pockets[pocket_ind].center_node;
					int lowest_floor = INFINITE_FLOOR;
					// check if there is any node assigned to the same pocket ahead of this node,
					// if yes, the precedence relationship must be set for the previous lead nodes already
					// find the proper section of the pocket to compare to the current lead node
					for (int k=j-1; k>=0; k--) 
					{
						// if assigned to the same pocket, find the lowest lead node
						if (lns.assignments[k] == pocket_ind)
						{
							if (lowest_floor > lead_nodes[k].center_floor)
								lowest_floor = lead_nodes[k].center_floor;
						}
						/* else
							break;
							*/
					}
					if (lowest_floor < INFINITE_FLOOR)		// if such a lead node exists
					{
						int safe_floor = MinimumSeparationFloor(lowest_floor,UpperSeparationTable);
						// find the proper section of the pocket to start searching
						while ( (s2 != limit1) && (feasible_plans.sequences[LOWER_CAR][s2].floor > safe_floor))
							s2 = feasible_plans.sequences[LOWER_CAR][s2].next;
					}
					limitIndex = feasible_plans.sequences[LOWER_CAR][limit1].next;
					if (( limitIndex != -1) &&( feasible_plans.sequences[LOWER_CAR][limit1].floor > feasible_plans.sequences[LOWER_CAR][limitIndex].floor))
						limit1 = limitIndex;

				}
		
				// find proper section (s2,  limit1) to use
				while ( (s2 != limit1) && (s2 != -1))
				{
					int safe_floor = MinimumSeparationFloor(feasible_plans.sequences[LOWER_CAR][s2].floor ,LowerSeparationTable);  // minimum separation floor
					
					// step through upper sequence for proper place to put conditional stops
					// CSW051112: not go as far to limit2 for s1
					while ( (s3 != -1)&& (feasible_plans.sequences[UPPER_CAR][s3].floor >= safe_floor) && (s1 != limit2))
					{
						s1 = s3;
						s3 = feasible_plans.sequences[UPPER_CAR][s3].next;
					} 
					// if the stop meet minimum separation floor exactly, and s1 is a bottom node
					if ( (feasible_plans.sequences[UPPER_CAR][s1].floor == safe_floor) && ( s1 == lead_nodes[j].center_node))
					{
						if ((s3!= -1)&& (GetDir(feasible_plans.sequences[UPPER_CAR][s1].floor, feasible_plans.sequences[UPPER_CAR][s3].floor) == UP))
						{
							feasible_plans.AddArrivalPrecedence(LOWER_CAR, s2, s1);
						}
						if ((s3!= -1)&& (GetDir(feasible_plans.sequences[UPPER_CAR][s1].floor, feasible_plans.sequences[UPPER_CAR][s3].floor) == DN))
						{
							feasible_plans.AddDeparturePrecedence(LOWER_CAR, s2, s1);
						}
					}
					// if the stop meet minimum separation floor exactly, and s1 is a left node
					else if ( (feasible_plans.sequences[UPPER_CAR][s1].floor == safe_floor) && ( s1 == lead_nodes[j].left_node))
					{
						feasible_plans.AddDeparturePrecedence(LOWER_CAR, s2, s1);
					}
					// else if the stop meet minimum separation floor exactly, specify both arrival and departure predecessor
					else if ( (feasible_plans.sequences[UPPER_CAR][s1].floor == safe_floor) )
					{
						feasible_plans.AddDeparturePrecedence(LOWER_CAR, s2, s1);
						// CSW051118: Added to make sure it was a downward movement
						if (lead_nodes[j].left_node != -1 && lead_nodes[j].left_floor > feasible_plans.sequences[UPPER_CAR][s1].floor)
							feasible_plans.AddArrivalPrecedence(LOWER_CAR, s2, s1);
					}
					// if there is a downward move from s1 to s3 (s3 != NULL) and floor of s3 < safe_floor
					else if ( (feasible_plans.sequences[UPPER_CAR][s1].floor > safe_floor) && (s3 != -1) && (feasible_plans.sequences[UPPER_CAR][s3].floor < safe_floor ))
					{
						PLAN_INDEX_T new_index = feasible_plans.sequences[UPPER_CAR].NewStop();
						Stop *new_stop = &feasible_plans.sequences[UPPER_CAR][new_index];
						new_stop->floor = safe_floor;
						new_stop->boarding = new_stop->deboarding = 0;
						new_stop->empty = feasible_plans.sequences[UPPER_CAR][s3].empty;				// empty or not should be the sae as the next node
						new_stop->type = STOP_TYPE_CONDITIONAL;			// it is a conditional stop
						feasible_plans.AddDeparturePrecedence(LOWER_CAR, s2, new_index);
						feasible_plans.AddArrivalPrecedence(LOWER_CAR, s2, new_index);
						new_stop->next = feasible_plans.sequences[UPPER_CAR][s1].next;
						feasible_plans.sequences[UPPER_CAR][s1].next = new_index;
						s1 = new_index;								// move s1 to the next node
					}
					s2 = feasible_plans.sequences[LOWER_CAR][s2].next;
				} 
			}
		}
	
		// s1 = feasible_plans.sequences[UPPER_CAR][lead_nodes[j].center_node].next;		// start processing from next node of lead node
		s1 = lead_nodes[j].center_node;			// start processing from lead node
		s2 = pockets[pocket_ind].center_node;		// get the bottom node of the pocket
		// if there is a right edge node, compare upward sequence against right part of the pocket
		if ( (pockets[pocket_ind].right_node != -1) && (s2 != -1))
		{
			s3 = feasible_plans.sequences[LOWER_CAR][s2].next;
			// step through upward sequence, compare against the right part of the pocket
			limit1 = lead_nodes[j].right_node;
			if (limit1 != -1)					// take next node of right node as limit
				limit1 = feasible_plans.sequences[UPPER_CAR][limit1].next;
			else  // keep moving until encounter an empty move or end of sequence
			{
				limit1 = lead_nodes[j].center_node;
				int next = feasible_plans.sequences[UPPER_CAR][limit1].next;
			    while ((next != -1) && !feasible_plans.sequences[UPPER_CAR][next].empty)
				{
					limit1 = next;
					next = feasible_plans.sequences[UPPER_CAR][next].next;
				}
				limit1 = next;
			}
			int lowest_floor = INFINITE_FLOOR;
			// check if there is any node assigned to the same poket after of this node,
			// if yes, find the proper section on the right side of the pocket
			for (int k=j+1; k < num_lead_nodes; k++) 
			{
				// if lead node assigned to the same pocket
				if (lns.assignments[k] == pocket_ind)
				{
					// find the lowest floor among all lead nodes assigned to the pocket
					if (lowest_floor > lead_nodes[k].center_floor)
						lowest_floor = lead_nodes[k].center_floor;
				}
				else
					break;
			}
			PLAN_INDEX_T s4 = s1;
			if (lowest_floor < INFINITE_FLOOR)
			{
				// set the limit of the lead node section to be less than the lowest lead nodes on the right
				while ( (s4 != limit1) && (feasible_plans.sequences[UPPER_CAR][s4].floor < lowest_floor))
					s4 = feasible_plans.sequences[UPPER_CAR][s4].next;
				limit1 = s4;
			}
			//  CSW050907: set limit to be the pocket of the right node
			limit2 = pockets[pocket_ind].right_node;  
			s4 = feasible_plans.sequences[LOWER_CAR][limit2].next;
			// if there is right node, it can keep going up
			while (( s4 != -1) && (feasible_plans.sequences[LOWER_CAR][s4].floor >= feasible_plans.sequences[LOWER_CAR][limit2].floor))
			{
				limit2 = s4;
				s4 = feasible_plans.sequences[LOWER_CAR][s4].next;
			}
			while ((s1 != limit1) && (s1 != -1))
			{
				int safe_floor = MinimumSeparationFloor(feasible_plans.sequences[UPPER_CAR][s1].floor,UpperSeparationTable);  // minimum separation floor
						
				// if there is a pocket right edge node, set the limit to be its next node
				// step through right side of the pocket for proper place to put safety stops
				while ( (s3 != -1)&& (feasible_plans.sequences[LOWER_CAR][s3].floor <= safe_floor) && (s2 != limit2 ))
				{
					s2 = s3;
					s3 = feasible_plans.sequences[LOWER_CAR][s3].next;
				} 
				
				// if the stop meet minimum separation floor exactly, and s2 is a center node
				if ( (feasible_plans.sequences[LOWER_CAR][s2].floor == safe_floor)&& ( s2 == pockets[pocket_ind].center_node))
				{
					// CSW051118: further check to make sure the lower car is going up next
					if ((s3 != -1) && (feasible_plans.sequences[LOWER_CAR][s3].floor > feasible_plans.sequences[LOWER_CAR][s2].floor))
						feasible_plans.AddDeparturePrecedence(UPPER_CAR, s1, s2);
				}
				 	// if the stop meet minimum separation floor exactly, and s2 is a right edge node
				else if ( (feasible_plans.sequences[LOWER_CAR][s2].floor == safe_floor)&& ( s2 == pockets[pocket_ind].right_node))
				{
					feasible_plans.AddArrivalPrecedence(UPPER_CAR, s1, s2);
				}
				// if the stop meet minimum separation floor exactly, specify both arrival and departure precedessor
				else if (feasible_plans.sequences[LOWER_CAR][s2].floor == safe_floor)
				{
					feasible_plans.AddArrivalPrecedence(UPPER_CAR, s1, s2);
					// CSW050921: Added condition to check the next stop too
					if ((s3!= -1) && (feasible_plans.sequences[LOWER_CAR][s3].floor > safe_floor))
						feasible_plans.AddDeparturePrecedence(UPPER_CAR, s1, s2);
				}			
				// if s2 less than the floor, but s3 is greater than the floor
				else if ( (feasible_plans.sequences[LOWER_CAR][s2].floor < safe_floor) && (s3 != -1) && (feasible_plans.sequences[LOWER_CAR][s3].floor > safe_floor))
				{
					PLAN_INDEX_T new_index = feasible_plans.sequences[LOWER_CAR].NewStop();
					Stop *new_stop = &feasible_plans.sequences[LOWER_CAR][new_index];
					new_stop->floor = safe_floor;
					new_stop->boarding = new_stop->deboarding = 0;
					new_stop->empty = feasible_plans.sequences[LOWER_CAR][s3].empty;				// empty or not should be the same as the next node
					new_stop->type = STOP_TYPE_CONDITIONAL;			// it is a conditional stop
					feasible_plans.AddArrivalPrecedence(UPPER_CAR, s1, new_index);
					feasible_plans.AddDeparturePrecedence(UPPER_CAR, s1, new_index);
					new_stop->next = feasible_plans.sequences[LOWER_CAR][s2].next;
					feasible_plans.sequences[LOWER_CAR][s2].next = new_index;
					if (s2 == pockets[pocket_ind].right_node)
					{
						pockets[pocket_ind].right_node = pockets[pocket_ind+1 ].left_node = new_index;
						pockets[pocket_ind].right_floor = pockets[pocket_ind+1 ].left_floor= safe_floor;
					}
					s2 = new_index;								// move s2 to the newly inserted stop
				}
				s1 = feasible_plans.sequences[UPPER_CAR][s1].next;
			} 
		}
		prev_pocket = pocket_ind;
	}
	feasible_plans.sequences[UPPER_CAR].UpdateOnBoard();				// reset the index for upper sequence
	feasible_plans.sequences[LOWER_CAR].UpdateOnBoard();				// reset the index for lower sequence
	CleanUpPrecedence(feasible_plans);
	return 0;
}

// generate plan from call list and status
T_HPSCORE HoistwayPlanGen::generatePlan(int hwy, 
										HoistwayInfo &hwyInfo, 
										CarInfo car[],
										CarState &uCarState,
										CarState &lCarState,
										CallList &uCallList, 
										CallList &lCallList, 
										HoistwayPlan &plan)
{
	StopSequence upper_sequence, lower_sequence;
	
	int lowerCarID,upperCarID;
	T_HPSCORE score;

	lowerCarID = hwyInfo.idLowerCar;
	upperCarID = hwyInfo.idUpperCar;

	score = GetBestPlan(plan,uCallList,lCallList,hwyInfo,hwy, car, uCarState, lCarState); // CSW: 050805 use newer function
	// score = GetBestPlanEnumerateSequence(plan,uCallList,lCallList,hwyInfo,hwy, car, uCarState, lCarState); // CSW: 050805 use newer function
	return score;
}


// Extract committed stop sequence from the call list(Call list should have information of dest,passengers...)
void HoistwayPlanGen::BuildCommittedSequence(StopSequence &committed,
						  DestinationEntryCalls &committedCalls,
						  CallList &callList,
						  CarState &state)
{
	CALL_INDEX_T boardId;
	PLAN_INDEX_T stopInd;


	committed.reset();

	boardId=committedCalls.sequence_start;  
	committed.current_NCF=state.currentNCF;

	
	// if the car is not in motion, it must stop at the current target, create the stop first
	// shs0726 fix : even if it is committed call, there should be 
	//			consideration of synchronization problem at origin floor
	if (state.currentFloor == state.target)
	{	
		stopInd = committed.NewStop();
		committed[stopInd].floor=state.target;
		// If there were calls picked up at the current floor
		if (countArrivedAtOrigin(committedCalls,state.currentFloor)>0)
		{	// de call's origin ?
			committed[stopInd].type=STOP_TYPE_SERVED;
			committed[stopInd].empty=false;		
		}
		else
		{	// not de call's origin ? current stop floor should be first stop
			if (boardId == -1)
			{
				committed[stopInd].empty=true;
				if (state.frontDoorState == DOORS_CLOSED)
					committed[stopInd].type=STOP_TYPE_PARKING;
				else
					committed[stopInd].type=STOP_TYPE_SERVED;
			}
			else
			{
				committed[stopInd].empty=false;
				if (state.frontDoorState == DOORS_CLOSED)
					committed[stopInd].type=STOP_TYPE_CONDITIONAL;
				else
					committed[stopInd].type=STOP_TYPE_SERVED;
			}
		}
		committed[stopInd].next = -1;
		committed.sequence_start = stopInd;
	}
	// if passed deceleration point, it has to stop anyway
	// need to revise code once the type of stop is decided
	// CSW050929: added condition for deceleration point
	else if ((state.currentNCF == state.target)	&& state.decelDecided)		
	{
		stopInd = committed.NewStop();
		committed[stopInd].floor=state.target;	
		committed[stopInd].empty=true;
		committed[stopInd].type=STOP_TYPE_PIVOT;
		committed[stopInd].next = -1;
		committed.sequence_start = stopInd;
	}


	UCHAR load = 0; 
	while(boardId>=0)	// boarded index
	{
		PLAN_INDEX_T s1, s2;
		CHAR dir = committedCalls[boardId].direction();
		FLOOR_T floor = committedCalls[boardId].destination;
		load += committedCalls[boardId].passengers;

		s1 = -1;
		s2 = committed.sequence_start;

		// find a place to insert the node
		while (s2 != -1)
		{
			if ( committed[s2].floor*dir <= floor*dir)
			{
				s1 = s2;
				s2 = committed[s2].next;
			}
			else
				break;
		}

		if (s1 == -1) // insert at the head
		{
			stopInd = committed.NewStop();
			committed[stopInd].floor = committedCalls[boardId].destination;
			committed[stopInd].type = STOP_TYPE_DEMAND;
			committed[stopInd].deboarding = (UCHAR) committedCalls[boardId].passengers;
			committed.InsertHead(stopInd);					// insert this node as head
			committed.LinkDestination(stopInd, committedCalls, boardId);
		}
		else if (committed[s1].floor == floor)
		{
			committed.combine(s1, 0, committedCalls[boardId].passengers);
			committed.LinkDestination(s1, committedCalls, boardId);
		}
		else
		{
			stopInd = committed.NewStop();
			committed[stopInd].floor = committedCalls[boardId].destination;
			committed[stopInd].type = STOP_TYPE_DEMAND;
			committed[stopInd].deboarding = (UCHAR) committedCalls[boardId].passengers;
			committed.InsertAfter(s1, stopInd);					// insert this node after s1
			committed.LinkDestination(stopInd, committedCalls, boardId);
		}
		boardId = committedCalls[boardId].next;
	}
	committedCalls.sequence_start = -1;					// mark there is no call left to be processed
	committed.current_on_board = load;
	committed.UpdateOnBoard();
}



int  HoistwayPlanGen::ExtractWaitingCalls(DestinationEntryCalls &DEcalls, CallList &assignedCall)
{	// waiting 
	CALL_INDEX_T waitId, ind, prevInd;

	prevInd = -1;

	for (int i=0; i< MAX_FLOOR; i++)
	{
		waitId=assignedCall.waitingAtHead[i];
		while (waitId>=0)
		{
			ind = DEcalls.NewCall();
			// CSW051116: If not enough memory return -1
			if (ind < 0)
				return -1;
			DEcalls[ind].passengers=assignedCall.calls[waitId].nPassengers;
			DEcalls[ind].destination=assignedCall.calls[waitId].destination;
			DEcalls[ind].origin=assignedCall.calls[waitId].origin;			
			DEcalls[ind].reg_time=MAKE_FLOAT_TIME(assignedCall.calls[waitId].ctRegistration);///(INT_TIME_SCALE*1.0);
			DEcalls[ind].next=-1;	
			DEcalls[ind].status=PASSENGER_WAITING; // shs0719
		
			if (DEcalls.sequence_start == -1)
			{
				DEcalls.sequence_start = ind;
				prevInd = ind;
			}
			else
			{
				DEcalls[prevInd].next = ind;
				prevInd = ind;
			}

			waitId=assignedCall.calls[waitId].next;
		}
	}

		// CSW050907 : If there are expected calls, transfer to WaitingAtTail
	waitId = assignedCall.expectedTransferHead;
	while(waitId >= 0)
	{
		ind = DEcalls.NewCall();
		// CSW051116: If not enough memory return -1
		if (ind < 0)
				return -1;
		DEcalls[ind].passengers=assignedCall.calls[waitId].nPassengers;
		DEcalls[ind].destination=assignedCall.calls[waitId].destination;
		DEcalls[ind].origin=assignedCall.calls[waitId].origin;			
		DEcalls[ind].reg_time=MAKE_FLOAT_TIME(assignedCall.calls[waitId].ctRegistration);///(INT_TIME_SCALE*1.0);
		DEcalls[ind].next=-1;	
		DEcalls[ind].status=PASSENGER_WAITING; // shs0719
	
		if (DEcalls.sequence_start == -1)
		{
			DEcalls.sequence_start = ind;
			prevInd = ind;
		}
		else
		{
			DEcalls[prevInd].next = ind;
			prevInd = ind;
		}
		waitId=assignedCall.calls[waitId].next;
	}
	// CSW050907 : end
	return 0;
}

void HoistwayPlanGen::CleanUpPrecedence(HoistwayPlan &plan)
{
	PLAN_INDEX_T  other;
	// if the first stop floor equal to NCF, remove arrival precedence associate with this stop
	if ( plan.sequences[LOWER_CAR][0].floor == plan.sequences[LOWER_CAR].current_NCF)
	{
		other = plan.sequences[LOWER_CAR][0].arrival_predecessor;
		if ( other != -1)
		{
			plan.sequences[UPPER_CAR][other].arrival_successor = -1;
			plan.sequences[LOWER_CAR][0].arrival_predecessor = -1;
		}
		other = plan.sequences[LOWER_CAR][0].arrival_successor;
		if ( other != -1)
		{
			plan.sequences[UPPER_CAR][other].arrival_predecessor = -1;
			plan.sequences[LOWER_CAR][0].arrival_successor = -1;
		}
	}

	// if the first stop floor equal to NCF, remove arrival precedence associate with this stop
	if ( plan.sequences[UPPER_CAR][0].floor == plan.sequences[UPPER_CAR].current_NCF)
	{
		other = plan.sequences[UPPER_CAR][0].arrival_predecessor;
		if ( other != -1)
		{
			plan.sequences[LOWER_CAR][other].arrival_successor = -1;
			plan.sequences[UPPER_CAR][0].arrival_predecessor = -1;
		}
		other = plan.sequences[UPPER_CAR][0].arrival_successor;
		if ( other != -1)
		{
			plan.sequences[LOWER_CAR][other].arrival_predecessor = -1;
			plan.sequences[UPPER_CAR][0].arrival_successor = -1;
		}
	}
}

// count the passengers of committed call's origin
int HoistwayPlanGen::countArrivedAtOrigin(DestinationEntryCalls committedCalls, FLOOR_T origin)
{
	int passengers;
	int callIndex;
	passengers=0;
	callIndex=committedCalls.sequence_start;

	while(callIndex>=0)
	{
		if (committedCalls.calls[callIndex].origin==origin)
			passengers+=committedCalls.calls[callIndex].passengers;
		callIndex=committedCalls.calls[callIndex].next;

	}
	return passengers;
}

/*
 * Generate best hoistway plan given two sequence
 * This function only keep the best plan as plans are generated. 
 * All non-best plans are discarded
 */
int HoistwayPlanGen::GetBestHoistwayPlan(StopSequence &upper_sequence, 
									 StopSequence &lower_sequence,
									 DestinationEntryCalls &upper_calls,
									 DestinationEntryCalls &lower_calls,
									 HoistwayPlan &bestPlan,
									 T_HPSCORE	&bestScore,
									 CarState &uCarState,
									 CarState &lCarState,
									 int upperCarID,
									 int lowerCarID)
{	
	timer.start();						// start the timer

	Pocket pockets[MAX_STOP];		// pocket of the lower car
	Pocket lead_nodes[MAX_STOP];		// lead nodes defined for the upper car
	LeadNodesAssignment stack[MAX_STACK_SIZE], ass;
	HoistwayPlan candidatePlan;
	HoistwaySchedule Hs;

	T_HPSCORE	score; 
	short bestPlanIndex=0; // for ensuring there is 
	bestScore = (T_HPSCORE) T_HPSCORE_MAX; // CSW101905 : cast type to avoid warning messages
	Hs.SetCarStates(&uCarState,&lCarState);

	FILE *fp = stdout;			//CSW050913
#ifdef	WITH_DEBUG
	char FileName[100];
	sprintf(FileName,"HSchedule-%d-%d",lowerCarID,upperCarID);
	if (virtualAssign>=0)
	{
		sprintf(FileName,"%sV%d",FileName,virtualAssign);
	}
	
	strcat(FileName,".txt");
#ifdef	CUMULATIVE_HP_LOG
	if (d_passengerNo<=0)
	{
		time_t ltime;
		fp=fopen(FileName,"w");  // clear older contents

		fprintf(fp, "\n\n////////////////////////////////////////////////////\n");//insert date and time
		time( &ltime );
		fprintf(fp, ", time: %s\n", ctime( &ltime ) );
	}
	else
	{
		fp=fopen(FileName,"a");
	}
#else
	fp=fopen(FileName,"w");
#endif

	fprintf(fp,"\n=== Passenger %d ===\n",d_passengerNo);
#endif

	int numPlans = 0;
	// if only one car in the hoistway, the other car ID is less than 0
	// return the sequence. No coordination needed. 
	if ( lowerCarID <0 || upperCarID<0 )
	{
		bestPlan.sequences[UPPER_CAR] = upper_sequence;
		bestPlan.sequences[LOWER_CAR] = lower_sequence;
		bestPlan.de_calls[UPPER_CAR] = upper_calls;
		bestPlan.de_calls[LOWER_CAR] = lower_calls;
		bestPlanIndex = 0;
		numPlans =  1;
	}
	// CSW051003: Check whether the NCF of the two car already has problem
	// if so, return no plan
	else if (CheckCarState(uCarState,lCarState))
	{	// if no plan, send plan with sequence_start=-1
		bestPlan.sequences[UPPER_CAR].sequence_start=-1;
		bestPlan.sequences[LOWER_CAR].sequence_start=-1;
		numPlans =  0;
	}
	else
	{
		// LeadNodesAssignment assignments[MAX_PLANS];		// The array of all possible assignments
		int num_pocket = FindPocket(lower_sequence, pockets,lCarState);			// find pockets of the lower car 
		
		int num_lead_node = FindLeadNodes(upper_sequence, lead_nodes);			// find lead nodes for the upper car

		// CSW051112: Add this function to check parking 
		CheckParking(lowerCarID, lCarState, num_pocket,lower_sequence, pockets, 
			upperCarID, uCarState, num_lead_node, upper_sequence, lead_nodes);
		// initialize stack for enumeration
		int stack_ind = 1;
		stack[0].last_assigned_pocket_index = stack[0].lead_nodes_assigned = 0;

		// set initial values of the assignment
		for (int i=0; i<num_lead_node; i++)
			stack[0].assignments[i] = -1;

		while (stack_ind != 0)
		{
			// pop the first node from the stack
			stack_ind --; 
			ass = stack[stack_ind];

			PLAN_INDEX_T left_node = lead_nodes[ass.lead_nodes_assigned].left_node;
			// CSW050910: Flag to indicate the lead node is assigned to previous pocket, if it is set, it cannot be no assignment (-1)
			BOOL flagAssignedEarlier = false;	
			// CSW050910: end
			int left_safe_floor = LowerLowest;
			// find minimum separation floor for left node
			if (left_node != -1)
			{
				int left_floor = lead_nodes[ass.lead_nodes_assigned].left_floor;
				left_safe_floor = MinimumSeparationFloor(left_floor, UpperSeparationTable);
			}

			PLAN_INDEX_T lead_node = lead_nodes[ass.lead_nodes_assigned].center_node;
			int lead_floor = lead_nodes[ass.lead_nodes_assigned].center_floor;
			int safe_floor = MinimumSeparationFloor(lead_floor, UpperSeparationTable);

			// CSW051112: Check the lead node to assigned. If it is a parking node assigned by the dispatcher
			// it can only be assigned to the last pocket
			if (ass.lead_nodes_assigned == num_lead_node-1)
			{
				if (upper_sequence[lead_node].type == STOP_TYPE_PARKING	&& upper_sequence[lead_node].floor == parkingInfo[upperCarID].parkingLandingIndex) 
					ass.last_assigned_pocket_index = num_pocket -1;
			}

			// Check assigning lead nodes to each of the pocket
			for (int i = ass.last_assigned_pocket_index; i<num_pocket; i++) 
			{
				// if there is a left recession node		
				if (left_node != -1) 
				{
					// if the left node is not empty
						// check whether it can pass the left edge of the pocket				
						// check the left edge of the pocket
						if ( (pockets[i].left_floor > left_safe_floor) && (i != ass.last_assigned_pocket_index))
							break;		// break the for loop, it is not possible to assign the node to this pocket, or pockets after
				}
				
				// check lead node
				PLAN_INDEX_T center_node = pockets[i].center_node;
				// if there is a center node, check whether the lead node can be safely put inside
				if (center_node != -1)
				{
					if ( (pockets[i].center_floor > safe_floor))
						continue;		// This pocket cannot accommodate the node, check the next pocket
				}
				
				// create a new node and put it into stack
				if (stack_ind < MAX_STACK_SIZE)
				{	
					// if the safe floor is less than either left or right edge of the pocket, or this is the last pocket or this is the last lead node
					if ( ( safe_floor < pockets[i].left_floor) || ( safe_floor < pockets[i].right_floor) || 
						(ass.lead_nodes_assigned == num_lead_node -1) )
					{	
						stack[stack_ind] = ass;
						stack[stack_ind].last_assigned_pocket_index = i;
						stack[stack_ind].assignments[ass.lead_nodes_assigned] = i;
						stack[stack_ind].lead_nodes_assigned++;
						stack_ind ++;				
						flagAssignedEarlier = true;
					}
					else
					{
						// if the node is assigned earlier, it must has assignment in later pockets, not accept no assignment
						if (!flagAssignedEarlier)	
						{					
							stack[stack_ind].assignments[ass.lead_nodes_assigned] = -1;
							stack[stack_ind].last_assigned_pocket_index = i;
							stack[stack_ind] = ass;
							stack[stack_ind].lead_nodes_assigned++;
							stack_ind ++;
						}
					}		
				}
				// if all nodes are assigned
				if (stack[stack_ind-1].lead_nodes_assigned >= num_lead_node)
				{	
					stack_ind --;
					// assignments[numPlans] = stack[stack_ind];
					candidatePlan.sequences[UPPER_CAR] = upper_sequence;
					candidatePlan.sequences[LOWER_CAR] = lower_sequence;
					candidatePlan.de_calls[UPPER_CAR] = upper_calls;
					candidatePlan.de_calls[LOWER_CAR] = lower_calls;
					GenerateHoistwayPlan(candidatePlan, stack[stack_ind], lead_nodes, pockets, num_lead_node, num_pocket, uCarState, lCarState);
					CheckPivot(candidatePlan, uCarState,lCarState);
					// CSW051118: Added to check TSA delay
					// If exception such as not enough memory happen, return 0 plan
					if ( CheckTSADelay(uCarState,lCarState,upperCarID,lowerCarID, candidatePlan) < 0)
						return 0;
					score=Hs.TimingAndScoring(upperCarID,lowerCarID, candidatePlan, numPlans, fp);

					if (numPlans==0)
					{	// for ensuring there is at least plan exist even though bestScore exceeds T_HPSCORE_MAX
						bestScore= score;
						bestPlan=candidatePlan;
						bestPlanIndex=numPlans;
					}

					if (score < bestScore)
					{
						bestScore = score;
						bestPlan = candidatePlan;
						bestPlanIndex = numPlans;
					}

					numPlans ++;
					if (numPlans >= MAX_PLANS)
						break;
				}
			}
			// process the node
		}
	}

#ifdef	WITH_DEBUG		
	if (numPlans>0)
	{
		
		fprintf(fp,"* Best Plan :  candidate %d\n",bestPlanIndex);
		Hs.TimingAndScoring(upperCarID, lowerCarID, bestPlan, bestPlanIndex, fp);
		fprintf(fp,"Score : %f\n",bestScore);
	}
	else
	{	
		if (virtualAssign<0)
			_RPT2(_CRT_WARN,"NO BEST PLAN for upper=%d,lower=%d\n",upperCarID,lowerCarID);
		else
			_RPT1(_CRT_WARN,"No Best Plan for virtual assign %d\n",virtualAssign);
		fprintf(fp,"No Best Plan found\n");
	}
	fclose(fp);
#endif
	if (numPlans<=0)
	{	// if no plan, send plan with sequence_start=-1
		bestPlan.sequences[UPPER_CAR].sequence_start=-1;
		bestPlan.sequences[LOWER_CAR].sequence_start=-1;
	}

	return numPlans; 
}
/*
 * Generate best hoistway plan given the current assigned calls
 * It takes advantage of the GetBestHoistwayPlan()
 * So less memory is required
 */
T_HPSCORE HoistwayPlanGen::GetBestPlan(HoistwayPlan &plan,
								 CallList &upperCalls,
								 CallList &lowerCalls,
								 HoistwayInfo &hwyInfo,
								 int hwy,
								 CarInfo car[],
								 CarState &uCarState,
								 CarState &lCarState)
{
	StopSequence upper_sequence, lower_sequence;

	T_HPSCORE	score;
	int upperCarID,lowerCarID;

	upperCarID=hwyInfo.idUpperCar;
	lowerCarID=hwyInfo.idLowerCar;

	StopSequence upper_committed;
	DestinationEntryCalls upper_calls, lower_calls;

	//CSW051017 : check whether there is a need to modify NCF of the car 
	if (upperCarID >=0 && lowerCarID >= 0)
		CheckCarState(uCarState,lCarState);

	if (upperCarID >= 0)
	{
		// CSW051116 : If there not enough memory, return the max score
		if ( upper_calls.CopyCommittedCalls(upperCalls) < 0)			// copy committed calls from Callist
			return (T_HPSCORE)T_HPSCORE_MAX;
		BuildCommittedSequence(upper_committed, upper_calls, upperCalls, uCarState);
		// CSW051116 : If there not enough memory, return the max score
		if ( ExtractWaitingCalls(upper_calls, upperCalls) < 0)
			return (T_HPSCORE)T_HPSCORE_MAX;
		FullCollective( upper_committed, upper_calls, upper_sequence, car[upperCarID].capacity, uCarState );
	// add first stop if the sequence is empty
		if (upper_sequence.empty())
			upper_sequence.AddParkingStop(uCarState);	
	}
	else
	{
		upper_sequence.reset();
		upper_sequence.current_NCF = UpperHighest;
	}

	
	// making lower sequences 
	StopSequence lower_committed;	// committed sequence

	if (lowerCarID >= 0)
	{
		// CSW051116 : If there not enough memory, return the max score
		if ( lower_calls.CopyCommittedCalls(lowerCalls) < 0)			// copy committed calls from Callist
			return (T_HPSCORE)T_HPSCORE_MAX;
		BuildCommittedSequence(lower_committed,lower_calls, lowerCalls, lCarState);
		// CSW051116 : If there not enough memory, return the max score
		if ( ExtractWaitingCalls(lower_calls, lowerCalls) < 0)
			return (T_HPSCORE)T_HPSCORE_MAX;
		FullCollective( lower_committed, lower_calls, lower_sequence, car[lowerCarID].capacity, lCarState ); // sequence generation
		if (lower_sequence.empty())
			lower_sequence.AddParkingStop(lCarState);
	}
	else
	{
		lower_sequence.reset();
		lower_sequence.current_NCF = LowerLowest;
	}

	int candidatePlanSize; 

	// CSW050903: Add pivot stop 
	upper_sequence.AddPivotStop(uCarState);
	lower_sequence.AddPivotStop(lCarState);
	// CSW050903: end
	candidatePlanSize=GetBestHoistwayPlan(upper_sequence,lower_sequence, upper_calls, lower_calls, plan, score, 
		uCarState, lCarState, upperCarID, lowerCarID);
	
	return score;
}


void HoistwayPlanGen::CheckPivot(HoistwayPlan &plan, CarState& uCarState, CarState &lCarState)
{
	// check whether upper car or lower car need pivot stop
	char pivotCar = -1;				// initialize pivot car index to -1, no pivot car
	char otherCar = -1;				// initialize the other car index to -1
	CHAR d1, d2;
	// check lower car first

	
	pivotCar = LOWER_CAR;
	otherCar = UPPER_CAR;
	PLAN_INDEX_T sequence_start = plan.sequences[pivotCar].sequence_start;
	if ( (sequence_start != -1) && (plan.sequences[pivotCar][sequence_start].type == STOP_TYPE_PIVOT) )
	{	
		PLAN_INDEX_T ind = plan.sequences[pivotCar][sequence_start].next;
		d1 = lCarState.movingDirection;
		// CSW050929: Added to avoid -1 index
		if (ind != -1)
			d2 = GetDir(plan.sequences[pivotCar][sequence_start].floor, plan.sequences[pivotCar][ind].floor);
		else
			d2 = NO_DIR;

		char flag = false;
		PLAN_INDEX_T indOther = -1;
		// if there is no change of direction and the car's NCF has not reached the target, there is no need for pivot stop
		if ((d1 == d2)&&(lCarState.target != plan.sequences[pivotCar].current_NCF)&& (ind != -1))
		{
			flag = true;
			if (plan.sequences[pivotCar][sequence_start].arrival_successor >=0)
			{
				indOther = plan.sequences[pivotCar][sequence_start].arrival_successor;
				plan.sequences[otherCar][indOther].arrival_predecessor = -1;
				flag = true;
			}
			if (plan.sequences[pivotCar][sequence_start].departure_successor >=0)
			{
				indOther = plan.sequences[pivotCar][sequence_start].departure_successor;
				plan.sequences[otherCar][indOther].departure_predecessor = -1;
				flag = true;
			}
			if (plan.sequences[pivotCar][sequence_start].arrival_predecessor>=0)
			{
				flag = false;
				plan.sequences[pivotCar][sequence_start].type = STOP_TYPE_CONDITIONAL;
			}
			if (plan.sequences[pivotCar][sequence_start].departure_predecessor >=0)
			{
				flag = false;
				plan.sequences[pivotCar][sequence_start].type = STOP_TYPE_CONDITIONAL;
			}
			
		}
		// CSW051004: if pivot stop equal to NCF and no direction change, it is an candidate for removing too
		else if ((d1 == d2) && (lCarState.target == plan.sequences[pivotCar].current_NCF) && (ind != -1))
		{
			// CSW050929: Added check deceleration condition
			if (( plan.sequences[pivotCar][sequence_start].floor == plan.sequences[pivotCar][ind].floor) || (lCarState.decelDecided ==FALSE))
			{
				flag = true;
				if (plan.sequences[pivotCar][sequence_start].arrival_successor >=0)
				{
					indOther = plan.sequences[pivotCar][sequence_start].arrival_successor;
					plan.sequences[otherCar][indOther].arrival_predecessor = -1;
					flag = true;
				}
				if (plan.sequences[pivotCar][sequence_start].departure_successor >=0)
				{
					indOther = plan.sequences[pivotCar][sequence_start].departure_successor;
					plan.sequences[otherCar][indOther].departure_predecessor = -1;
					flag = true;
				}
			}  
			// CSW051012: Remove from previous if statement
			if (plan.sequences[pivotCar][sequence_start].arrival_predecessor>=0)
			{
				flag = false;
				plan.sequences[pivotCar][sequence_start].type = STOP_TYPE_CONDITIONAL;
			}
			if (plan.sequences[pivotCar][sequence_start].departure_predecessor >=0)
			{
				flag = false;
				plan.sequences[pivotCar][sequence_start].type = STOP_TYPE_CONDITIONAL;
			}
		}
		// CSW050913: end
		if (flag)
		{
			PLAN_INDEX_T indNext = plan.sequences[pivotCar][sequence_start].next;
			plan.sequences[pivotCar][sequence_start].next = plan.sequences[pivotCar].empty_list;
			plan.sequences[pivotCar].empty_list = sequence_start;
			plan.sequences[pivotCar].sequence_start = indNext;
			plan.sequences[pivotCar].UpdateOnBoard();			// CSW050908: Update index after pivot stop is removed
			// CSW051006: Check the other car, if it is conditional stop and all precedence constraints are cancelled, remove the stop
			if (indOther != -1 && plan.sequences[otherCar][indOther].type == STOP_TYPE_CONDITIONAL)
			{
				Stop *s = &plan.sequences[otherCar][indOther];
				if (s->departure_predecessor == -1 && s->arrival_predecessor == -1)  //CSW051019 : use && instead of &
				{
					PLAN_INDEX_T prevInd = plan.sequences[otherCar].sequence_start;
					if (prevInd == indOther)
					{
						plan.sequences[otherCar].sequence_start = plan.sequences[otherCar][prevInd].next;
						plan.sequences[otherCar].free(indOther);
					}
					else
					{
						while ((prevInd != -1) && plan.sequences[otherCar][prevInd].next != indOther)
							prevInd = plan.sequences[otherCar][prevInd].next;
						plan.sequences[otherCar][prevInd].next = plan.sequences[otherCar][indOther].next;
						plan.sequences[otherCar].free(indOther);

					}
					plan.sequences[otherCar].UpdateOnBoard();
				}
			}
		}	
	} 
	
	pivotCar = UPPER_CAR;
	otherCar = LOWER_CAR;
	sequence_start = plan.sequences[pivotCar].sequence_start;
	if ( (sequence_start != -1) && (plan.sequences[pivotCar][sequence_start].type == STOP_TYPE_PIVOT) )
	{	
		PLAN_INDEX_T ind = plan.sequences[pivotCar][sequence_start].next;
		d1 = uCarState.movingDirection;
		if (ind != -1)
			d2 = GetDir(plan.sequences[pivotCar][sequence_start].floor, plan.sequences[pivotCar][ind].floor);
		else
			d2 = NO_DIR;
		char flag = false;
		PLAN_INDEX_T indOther = -1;
		// if there is no change of direction
		if ((d1 == d2)&&(uCarState.target != plan.sequences[pivotCar].current_NCF)&& (ind != -1))
		{
			flag = true;
			if (plan.sequences[pivotCar][sequence_start].arrival_successor >=0)
			{
				indOther = plan.sequences[pivotCar][sequence_start].arrival_successor;
				plan.sequences[otherCar][indOther].arrival_predecessor = -1;
				flag = true;
			}
			if (plan.sequences[pivotCar][sequence_start].departure_successor >=0)
			{
				indOther = plan.sequences[pivotCar][sequence_start].departure_successor;
				plan.sequences[otherCar][indOther].departure_predecessor = -1;
				flag = true;
			}
			if (plan.sequences[pivotCar][sequence_start].arrival_predecessor>=0)
			{
				flag = false;
				plan.sequences[pivotCar][sequence_start].type = STOP_TYPE_CONDITIONAL;
			}
			if (plan.sequences[pivotCar][sequence_start].departure_predecessor >=0)
			{
				flag = false;
				plan.sequences[pivotCar][sequence_start].type = STOP_TYPE_CONDITIONAL;
			}
		}
		// CSW051004: if pivot stop equal to NCF and no direction change, it is an candidate for removing too
		else if ( (d1 == d2) && uCarState.target == plan.sequences[pivotCar].current_NCF && (ind != -1))
		{
			// CSW050929: Added check deceleration condition
			if ( ( plan.sequences[pivotCar][sequence_start].floor == plan.sequences[pivotCar][ind].floor) || (uCarState.decelDecided == FALSE) )
			{
				flag = true;
				if (plan.sequences[pivotCar][sequence_start].arrival_successor >=0)
				{
					indOther = plan.sequences[pivotCar][sequence_start].arrival_successor;
					plan.sequences[otherCar][indOther].arrival_predecessor = -1;
					flag = true;
				}
				if (plan.sequences[pivotCar][sequence_start].departure_successor >=0)
				{
					indOther = plan.sequences[pivotCar][sequence_start].departure_successor;
					plan.sequences[otherCar][indOther].departure_predecessor = -1;
					flag = true;
				}
			}
			// CSW051012: removed from previous if statement
			if (plan.sequences[pivotCar][sequence_start].arrival_predecessor>=0)
			{
				flag = false;
				plan.sequences[pivotCar][sequence_start].type = STOP_TYPE_CONDITIONAL;
			}
			if (plan.sequences[pivotCar][sequence_start].departure_predecessor >=0)
			{
				flag = false;
				plan.sequences[pivotCar][sequence_start].type = STOP_TYPE_CONDITIONAL;
			}
		}
		// CSW050913: end
		if (flag)
		{
			PLAN_INDEX_T indNext = plan.sequences[pivotCar][sequence_start].next;
			plan.sequences[pivotCar][sequence_start].next = plan.sequences[pivotCar].empty_list;
			plan.sequences[pivotCar].empty_list = sequence_start;
			plan.sequences[pivotCar].sequence_start = indNext;
			plan.sequences[pivotCar].UpdateOnBoard();			// CSW050908: Update index after pivot stop is removed
			 // CSW051006: Check the other car, if it is conditional stop and all precedence constraints are cancelled, remove the stop
			if (indOther != -1 && plan.sequences[otherCar][indOther].type == STOP_TYPE_CONDITIONAL)
			{
				Stop *s = &plan.sequences[otherCar][indOther];
				if (s->departure_predecessor == -1 && s->arrival_predecessor == -1)	//CSW051019 : use && instead of &
				{
					PLAN_INDEX_T prevInd = plan.sequences[otherCar].sequence_start;
					if (prevInd == indOther)
					{
						plan.sequences[otherCar].sequence_start = plan.sequences[otherCar][prevInd].next;
						plan.sequences[otherCar].free(indOther);
					}
					else
					{
						while ((prevInd != -1) && plan.sequences[otherCar][prevInd].next != indOther)
							prevInd = plan.sequences[otherCar][prevInd].next;
						plan.sequences[otherCar][prevInd].next = plan.sequences[otherCar][indOther].next;
						plan.sequences[otherCar].free(indOther);

					}
					plan.sequences[otherCar].UpdateOnBoard();
				}
			}
		}		
	}
}


/*
 * Generate best hoistway plan given the current assigned calls
 * This function uses enumerated sequence instead of Full-collective sequence
 */
T_HPSCORE HoistwayPlanGen::GetBestPlanEnumerateSequence(HoistwayPlan &bestPlan,
								 CallList &upperCalls,
								 CallList &lowerCalls,
								 HoistwayInfo &hwyInfo,
								 int hwy,
								 CarInfo car[],
								 CarState &uCarState,
								 CarState &lCarState)
{
	StopSequence upper_sequences[MAX_SEQUENCES], lower_sequences[MAX_SEQUENCES];

	T_HPSCORE	score, bestScore;
	HoistwayPlan plan;
	int upperCarID,lowerCarID;

	upperCarID=hwyInfo.idUpperCar;
	lowerCarID=hwyInfo.idLowerCar;

	StopSequence upper_committed;
	DestinationEntryCalls upper_calls, lower_calls;
	// for recording the assignment of calls to stop 
	DestinationEntryCalls upper_dispatched_calls[MAX_SEQUENCES], lower_dispatched_calls[MAX_SEQUENCES];
	int numUpperSequences = 0;
	int numLowerSequences = 0;

	if (upperCarID >= 0)
	{
		upper_calls.CopyCommittedCalls(upperCalls);			// copy committed calls from Callist
		BuildCommittedSequence(upper_committed, upper_calls, upperCalls, uCarState);
		ExtractWaitingCalls(upper_calls, upperCalls);
		numUpperSequences = Enumerative(upper_committed, upper_calls, upper_sequences, upper_dispatched_calls,car[upperCarID].capacity, uCarState);
	}
	else
	{
		upper_sequences[0].reset();
		upper_sequences[0].current_NCF = UpperHighest;
	}

	
	// making lower sequences 
	StopSequence lower_committed;	// committed sequence

	if (lowerCarID >= 0)
	{
		lower_calls.CopyCommittedCalls(lowerCalls);			// copy committed calls from Callist

		BuildCommittedSequence(lower_committed,lower_calls, lowerCalls, lCarState);
		ExtractWaitingCalls(lower_calls, lowerCalls);
		numLowerSequences = Enumerative(lower_committed, lower_calls, lower_sequences, lower_dispatched_calls,car[lowerCarID].capacity, lCarState);
	}
	else
	{
		lower_sequences[0].reset();
		lower_sequences[0].current_NCF = LowerLowest;
	}

	int candidatePlanSize = 0; 
	bestScore = (T_HPSCORE) T_HPSCORE_MAX;	// CSW101905 : cast type to avoid warning messages
	int bestPlanIndex = 0;

	for (int i=0; i< numUpperSequences; i++)
	{
		upper_sequences[i].AddPivotStop(uCarState);
		for (int j=0; j<numLowerSequences; j++)
		{
			if (i == 0)	// only do this for the first time
				lower_sequences[j].AddPivotStop(lCarState);
			// CSW050903: end
			candidatePlanSize+=GetBestHoistwayPlan(upper_sequences[i],lower_sequences[j], upper_dispatched_calls[i], 
				lower_dispatched_calls[j], plan, score, uCarState, lCarState, upperCarID, lowerCarID);
			if (bestScore > score)
			{
				bestScore = score;
				bestPlan = plan;
				bestPlanIndex = j;
			}
		}
	}

#ifdef	WITH_DEBUG
	char FileName[100];
	sprintf(FileName,"HSchedule-%d-%d",lowerCarID,upperCarID);
	HoistwaySchedule Hs;
	strcat(FileName,".txt");
	FILE *fp=fopen(FileName,"w");  // clear older contents
	fprintf(fp,"Best plan using Enumerative Method\n");
	Hs.SetCarStates(&uCarState,&lCarState);
	Hs.TimingAndScoring(upperCarID, lowerCarID, bestPlan, bestPlanIndex, fp);
	fprintf(fp,"number of plans evaluated: %d\n",candidatePlanSize);
	fclose(fp);
#endif
		
	return bestScore;
}
// CSW051014 : Check the state of the two cars, return 0 if the state is feasible
// State check: compare the NCF of the two cars, if there is NCF conflict, resolve the conflict if possible
// Otherwise return 1 
int HoistwayPlanGen::CheckCarState(CarState &uCarState, CarState &lCarState)
{

	// check whether minimum separation is maintained for NCF
	FLOOR_T upperSafeFloor = MinimumSeparationFloor(lCarState.currentNCF,LowerSeparationTable);
	FLOOR_T lowerSafeFloor = MinimumSeparationFloor(uCarState.currentNCF,UpperSeparationTable);
	if ( (uCarState.currentNCF < upperSafeFloor) || (lCarState.currentNCF > lowerSafeFloor))
	{
		if ((lCarState.movingDirection == DN) && (lCarState.target <= lowerSafeFloor))
			lCarState.currentNCF = lowerSafeFloor;
		else if ((uCarState.movingDirection == UP) && (uCarState.target >= upperSafeFloor))
			uCarState.currentNCF = upperSafeFloor;
		else
			return 1;
	}
		
	return 0;
}


// Check TSA delay in lead car trailing car situation. If the delay is not enough, return 1. Otherwise return 0
int HoistwayPlanGen::CheckTSADelay(CarState &uCarState, CarState &lCarState, int uCarID, int lCarID, HoistwayPlan &plan)
{
	// if the two cars are not moving in the same direction, no need for TSA delay
	// if (uCarState.movingDirection * lCarState.motionState != 1)
	if (uCarState.movingDirection * lCarState.movingDirection != 1)
		return 0;

	int dir = uCarState.movingDirection;		// get the moving direction ( both car should be moving at the same direction 

	if (dir == UP)
	{
		PLAN_INDEX_T start = plan.sequences[LOWER_CAR].sequence_start;
		// if the following car wants to reverse direction, or the first stop of following car is equal to the current NCF
		if ( plan.sequences[LOWER_CAR][start].floor <= lCarState.currentNCF) 
			return 0;
		double tsaDelay;
		double leadDist=fabs((double)bdInfo.landingPosition[uCarState.target]-bdInfo.landingPosition[uCarState.origin]);
		double trailingDist=fabs((double)bdInfo.landingPosition[plan.sequences[LOWER_CAR][start].floor]-bdInfo.landingPosition[lCarState.origin]);
		double initSeparation=fabs((double)bdInfo.landingPosition[uCarState.origin]-bdInfo.landingPosition[lCarState.origin]);
		double MSD=(double)param.MinSeparationDist;
		tsaDelay=ESPNSPDelay( leadDist,		// lead car travel distance
						trailingDist,		// trailing car travel distance
					(double)carInfo[uCarID].maxJerk,				// max jerk for both cars
					(double)carInfo[uCarID].maxAcceleration,		// max acceleration for both cars
					(double)carInfo[uCarID].maxVelocity,			// max velocity for both cars
						initSeparation,	// distance between the origin of the two cars
						MSD,				// minimum separation distance
					(double)carInfo[uCarID].Afd
					) * 1000;				// convert second to millisecond
		if (lCarState.tStartOper - uCarState.tStartOper < tsaDelay)
		{
//			return 1;		// for checking
			PLAN_INDEX_T newInd = plan.sequences[LOWER_CAR].NewStop();
			if (newInd < 0)
				return -1;
			Stop *s;
			s = &plan.sequences[LOWER_CAR][newInd];
			s->floor = lCarState.currentNCF;			// keep the current target stored in car state
			s->type = STOP_TYPE_PIVOT;				// this is a pivot stop
			s->next = plan.sequences[LOWER_CAR].sequence_start;
			plan.sequences[LOWER_CAR].sequence_start = newInd;			// link the new node at head
			s->on_board = plan.sequences[LOWER_CAR].current_on_board;
			plan.sequences[LOWER_CAR].UpdateOnBoard();
			return 1;
		}
		else
			return 0;
	}
	else		// direction is DN lower car is the lead car and upper car is the trailing car
	{
		PLAN_INDEX_T start = plan.sequences[UPPER_CAR].sequence_start;
		// if the following car wants to reverse direction, or the first stop of following car is equal to the current NCF
		if (  plan.sequences[UPPER_CAR][start].floor >= uCarState.currentNCF )
			return 0;
		double tsaDelay;
		double leadDist=fabs((double)bdInfo.landingPosition[lCarState.target]-bdInfo.landingPosition[lCarState.origin]);
		double trailingDist=fabs((double)bdInfo.landingPosition[plan.sequences[UPPER_CAR][start].floor]-bdInfo.landingPosition[uCarState.origin]);
		double initSeparation=fabs((double)bdInfo.landingPosition[lCarState.origin]-bdInfo.landingPosition[uCarState.origin]);
		double MSD=(double)param.MinSeparationDist;
		tsaDelay=ESPNSPDelay( leadDist,		// lead car travel distance
						trailingDist,		// trailing car travel distance
					(double)carInfo[lCarID].maxJerk,				// max jerk for both cars
					(double)carInfo[lCarID].maxAcceleration,		// max acceleration for both cars
					(double)carInfo[lCarID].maxVelocity,			// max velocity for both cars
						initSeparation,	// distance between the origin of the two cars
						MSD,				// minimum separation distance
						(double)carInfo[lCarID].Afd
					)* 1000;				// convert second to millisecond
		if (uCarState.tStartOper - lCarState.tStartOper < tsaDelay)
		{
//			return 1;		// for checking
			PLAN_INDEX_T newInd = plan.sequences[UPPER_CAR].NewStop();
			if (newInd < 0)
				return -1;
			Stop *s;
			s = &plan.sequences[UPPER_CAR][newInd];
			s->floor = uCarState.currentNCF;		// use the current next committable floor
			s->type = STOP_TYPE_PIVOT;				// this is a pivot stop
			s->next = plan.sequences[UPPER_CAR].sequence_start;
			plan.sequences[UPPER_CAR].sequence_start = newInd;			// link the new node at head
			s->on_board = plan.sequences[UPPER_CAR].current_on_board;
			plan.sequences[UPPER_CAR].UpdateOnBoard();
			return 1;
		}
		else
			return 0;
	}

	return 0;
}

//******************************************************************************************************
//* Check whether parking stop is needed at the end of stop sequence. The parking information is stored 
//* in extern variable parkingInfo. If parkingLandingIndex is within the range of index of the car, 
//* planner will add a parking stop at the end of the sequence
//******************************************************************************************************
void HoistwayPlanGen::CheckParking(int idLower,						// lower car id
									CarState &lCarState,			// lower car state
									int &nPockets,					// number of pockets
									StopSequence &lowerSequence,	// lower car sequence
									Pocket *pockets,				// pockets defined by lower sequence
									int idUpper,					// upper car id
									CarState &uCarState,			// upper car state
									int &nLeadNodes,				// number of leadnodes
									StopSequence &upperSequence,	// upper car sequence
									Pocket *leadNodes				// lead nodes defined by upper sequence
									)
{
	FLOOR_T lowerParkingLanding = parkingInfo[idLower].parkingLandingIndex;
	FLOOR_T upperParkingLanding = parkingInfo[idUpper].parkingLandingIndex;

	// if both cars' landing positions are leagal, check whether there is a conflict
	if ( lowerParkingLanding >= LowerLowest && lowerParkingLanding <= LowerHighest &&
		  upperParkingLanding >= UpperLowest && upperParkingLanding <= UpperHighest)
	{
		FLOOR_T safeLanding = MinimumSeparationFloor(lowerParkingLanding,LowerSeparationTable);
		if (upperParkingLanding < safeLanding)
			upperParkingLanding = safeLanding;
	}

	// if the lower parking landing is leagal, add stop to the end of sequence and  as right node of last pocket
	if ( lowerParkingLanding >= LowerLowest && lowerParkingLanding <= LowerHighest)
	{
		PLAN_INDEX_T indStop = lowerSequence.NewStop();
		PLAN_INDEX_T ind = lowerSequence.sequence_start;
		if (ind == -1)
			lowerSequence.sequence_start = indStop;
		else	
		{
			// loop until the end of the sequence
			while (lowerSequence[ind].next != -1)
				ind = lowerSequence[ind].next;
			// if the car is moving to a yield stop and the stop is not a pivot point, make sure the landing is the parking landing
			if (lowerSequence[ind].type == STOP_TYPE_PARKING && ind == lowerSequence.sequence_start && 
				lCarState.motionState == MOVING && lCarState.currentNCF != lowerSequence[ind].floor)
			{
				lowerSequence[ind].floor = lowerParkingLanding;
				lowerSequence[ind].type = STOP_TYPE_PARKING;
			}
			else			// else create a new parking stop
			{
				lowerSequence[ind].next = indStop;
				lowerSequence[indStop].type = STOP_TYPE_PARKING;
				lowerSequence[indStop].floor = lowerParkingLanding;
				pockets[nPockets-1].right_node = indStop;
				pockets[nPockets-1].right_floor = lowerParkingLanding;
			}
		}
	}

	// if the upper parking landing is leagal, add stop to the end of sequence and add one more lead node
	if ( upperParkingLanding >= UpperLowest && upperParkingLanding <= UpperHighest)
	{
		PLAN_INDEX_T indStop = upperSequence.NewStop();
		PLAN_INDEX_T ind = upperSequence.sequence_start;
		if (ind == -1)
			lowerSequence.sequence_start = indStop;
		else	
		{
			while (upperSequence[ind].next != -1)
				ind = upperSequence[ind].next;
			// if the car is movint to a parking stop and has not decided to decel, make sure the landing is the parking landing
			if (upperSequence[ind].type == STOP_TYPE_PARKING && ind == upperSequence.sequence_start&&
				 uCarState.motionState == MOVING && uCarState.currentNCF != upperSequence[ind].floor)
			{
				upperSequence[ind].floor = upperParkingLanding;
				upperSequence[ind].type = STOP_TYPE_PARKING;
			}
			else		// create new parking stop
			{
				upperSequence[ind].next = indStop;
				upperSequence[indStop].type = STOP_TYPE_PARKING;
				upperSequence[indStop].floor = upperParkingLanding;
				leadNodes[nLeadNodes].left_node = leadNodes[nLeadNodes].right_node;
				leadNodes[nLeadNodes].left_floor = leadNodes[nLeadNodes].right_floor;
				leadNodes[nLeadNodes].center_floor = upperParkingLanding;
				leadNodes[nLeadNodes].center_node = indStop;
				nLeadNodes++;
			}
		}
	}

}
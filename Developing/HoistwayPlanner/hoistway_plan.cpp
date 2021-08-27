// Hoistway_Plan.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "hoistway_plan.h"
#include "timer.h"
#include <crtdbg.h>
#include "hoistwayplanselection.h"
#include "..\COGS\2dsim_defs.h"

FILE * debug_output = stdout;

// copy function of stop
// Only copy the cotent of the stop. Links are not copied
void Stop::copy( Stop &other)
{
	floor = other.floor;
	type = other.type;
	on_board = other.on_board;
	index = other.index;
	empty = other.empty;
	boarding = other.boarding;
	deboarding = other.deboarding;
	first_origin_call = other.first_origin_call;
	first_dest_call = other.first_dest_call;
	// shs adds
	arrival_predecessor=other.arrival_predecessor;
	arrival_successor=other.arrival_successor;
	departure_predecessor=other.departure_predecessor;
	departure_successor=other.departure_successor;
	next=other.next;
}

// Only copy the cotent of the stop. Links are not copied
void Stop::CopyStop( Stop &other)
{
	floor = other.floor;
	type = other.type;
	on_board = other.on_board;
	index = other.index;
	empty = other.empty;
	boarding = other.boarding;
	deboarding = other.deboarding;
	first_origin_call = other.first_origin_call;
	first_dest_call = other.first_dest_call;
	next=other.next;
}


Stop & StopSequence::operator [](PLAN_INDEX_T ind)	// returns the DestinationEntry at ind
{
	if ( (ind >= 0) && (ind <MAX_STOP))
		return sequence[ind];
	else
	{
		fprintf(debug_output, "Index out of range in stop sequence: %d\n",ind);
		// getchar();            // make the program pause, so one can find where the error occur with debugger
		return sequence[0];   // return the first element
	}; 
};

// construct stop sequence by array
StopSequence::StopSequence(int *sq, int size)
{
  reset();

  for (int i=0; i< size; i++)
  {
	  
	  int st1 = empty_list;
	  if (i ==0)               // set sequence to the first one
	  {
		  current_NCF = sq[0];           // set first floor as Current NCF
		  current_on_board = size;       // equals to the number of committed sequence
	  }
	  (*this)[i].floor = sq[i];
	  (*this)[i].type = STOP_TYPE_DEMAND;
	  (*this)[i].on_board = (size -i -1) * PASSENGER_UNIT;
	  (*this)[i].empty = false;
	  (*this)[i].deboarding = 1;
  }
  if (size > 0)
  {
    (*this)[size-1].next = -1;				// mark the end of the sequence
	sequence_start = 0;					// mark the start of the sequence
  }
  empty_list = size;
  UpdateOnBoard();
};


StopSequence::StopSequence()									// default construtor, set some parameters
{
		reset();
}

// constructor using another stop sequence
StopSequence::StopSequence( StopSequence &other)
{
	copy(other);
};



// Reset the stop sequence so it can be reused. 
void StopSequence::reset()
{
	current_on_board = 0;
	empty_list = 0;
	current_NCF = NO_FLOOR;
	for (int i=0; i<MAX_STOP; i++)
	{
		(*this)[i].next = i+1;
		(*this)[i].first_dest_call = (*this)[i].first_origin_call = -1;
		(*this)[i].arrival_predecessor = (*this)[i].arrival_successor = -1;
		(*this)[i].departure_predecessor = (*this)[i].departure_successor = -1;
		(*this)[i].boarding = (*this)[i].deboarding = 0;
	}
	(*this)[MAX_STOP-1].next = -1;
	sequence_start = -1;
}


// check whether the sequence is empty or not
bool StopSequence::empty()
{
	if ( sequence_start == -1)
		return true;
	else
		return false;
}

// copy from the other stop sequence
void StopSequence::copy(StopSequence &other)
{
	current_NCF = other.current_NCF;                         // intitial floor position
	current_on_board = other.current_on_board;               // initial passengers 
	int sq2 = other.sequence_start;

	memcpy((void *)sequence, (void *)other.sequence, MAX_STOP*sizeof(Stop));
	sequence_start = other.sequence_start;
	empty_list = other.empty_list;
}

// copy the stop info from other sequence and rearrange the index.
// links information of other stop is not copied
void StopSequence::CopyStop(StopSequence &other)
{
	if (!empty())       // if sequence is not empty
		 reset();

	current_NCF = other.current_NCF;                         // intitial floor position
	current_on_board = other.current_on_board;               // initial passengers 
	int sq2 = other.sequence_start;

	// if there is no call
	if (sq2 == -1)
		return;

	int ind = 0; // index of the stop sequence
	sequence_start = 0;
	while (sq2 != -1)
	{
		(*this)[ind].CopyStop(other[sq2]);
		(*this)[ind].arrival_predecessor = (*this)[ind].departure_predecessor = -1;
		sq2 = other[sq2].next;
		ind++;
	}
	empty_list = ind;
	(*this)[ind-1].next = -1;
	
}

// operator overload for = sign
StopSequence & StopSequence::operator = ( StopSequence & oper2) 
{
  copy(oper2);
  return(*this);
};

// return a index of a new stop that can be used
// the new stop will be disconnected from the list of free stops
PLAN_INDEX_T StopSequence::NewStop()
{
	// if run out of available memory, return -1
	if (empty_list == -1)
		return -1;

	int ind = empty_list;

	empty_list = (*this)[empty_list].next;
	(*this)[ind].reset();						// clean the content of the new stop
	return ind;
}
// return the stop ind back to the empty list
void StopSequence::free(PLAN_INDEX_T ind)
{
	(*this)[ind].next = empty_list;
	empty_list = ind;
}
// Inserting s as the head of the stop sequence
void StopSequence::InsertHead(PLAN_INDEX_T s)
{
  (*this)[s].next = sequence_start;
  sequence_start = s;
}

// Inserting s as the head of the stop sequence
void StopSequence::InsertAfter(int a, int s)
{
	(*this)[s].next = (*this)[a].next;
	(*this)[a].next = s;
}


// comebine the stop of a and s
void StopSequence::combine(PLAN_INDEX_T a, unsigned short boarding, unsigned short deboarding)
{
	if (boarding>0 || deboarding>0)
		(*this)[a].type=STOP_TYPE_DEMAND;
	(*this)[a].deboarding += deboarding;
	(*this)[a].boarding += boarding;
}



// return the first location of the sequence 
int StopSequence::origin()
{

	if ( current_NCF != NO_FLOOR)
		return current_NCF;
    
	if (sequence_start != -1)
		return (*this)[sequence_start].floor;
	else
		return NO_FLOOR;
}

// return the initial direction of the sequence
int StopSequence::InitialDir()
{
	int f1, f2;
	f1 = f2 = NO_FLOOR;

	if ( current_NCF != NO_FLOOR)		// if there is an initial floor
	{
		f1 = current_NCF;
		if (sequence_start != -1)
			f2 = (*this)[sequence_start].floor;
		else
			return NO_DIR;

		int next_index = (*this)[sequence_start].next;
		if ( (f1 == f2) && (next_index != -1))
			f2 = (*this)[next_index].floor;
	}
	else if (sequence_start != -1)					// if no initial floor and there is a first node
	{
		f1 = (*this)[sequence_start].floor;
		int s = (*this)[sequence_start].next;
		if (s != -1)
		{
			int next_index = (*this)[sequence_start].next;
			f2 = (*this)[next_index].floor;
		}
	}
	if ( ( f1 == NO_FLOOR) || ( f2 == NO_FLOOR))
		return NO_DIR;

	if (f1 - f2 <0)
		return UP;
	else if (f1 == f2)
		return NO_DIR;
	else
		return DN;
}


// The direction of the last subsequence 
int StopSequence::LastDir()	
{
	// get the initial direction first
	int dir = InitialDir();

	// if less than two stops in the sequence, return the initial direction
	if (sequence_start == -1)
		return dir;
	else if ((*this)[sequence_start].next == -1)
		return dir;

	int s = sequence_start;
	do		// shs suspicious (while ~ ) is correct? 
	{
		int f1 = (*this)[s].floor;
		s = (*this)[s].next;
		int f2 = (*this)[s].floor;
		dir = GetDir(f1,f2);
		f1 = f2;
	} while ((*this)[s].next != -1);
	return dir;
}


// The direction of the last subsequence 
int StopSequence::LastFloor()	
{
	// get the initial direction first
	int floor = current_NCF;

	// if less than two stops in the sequence, return the initial direction
	if (sequence_start == -1)
		return floor;

	int s = sequence_start;
    while (s!=-1) 
	{
		floor = (*this)[s].floor;
		s = (*this)[s].next;
	}
	return floor;
}


// append a sequence at the end of the existing sequence
int StopSequence::append(DestinationEntryCalls &other, PLAN_INDEX_T entry_index)
{
	if (sequence_start == -1)
	{
		sequence_start = NewStop();				// get a new stop node for the sequence_start
		int next_index = NewStop();				// get another node for the second stop
		(*this)[sequence_start].next = next_index;	// link to the second node
		(*this)[next_index].next = -1;				// terminate the node
		
		(*this)[sequence_start].floor = other[entry_index].origin;
		(*this)[sequence_start].boarding = (UCHAR) other[entry_index].passengers;
		(*this)[sequence_start].type = STOP_TYPE_DEMAND;
		// (*this)[sequence_start].first_origin_call = entry_index;
		LinkOrigin(sequence_start, other, entry_index);

		(*this)[next_index].floor = other[entry_index].destination;
		(*this)[next_index].deboarding = (UCHAR)other[entry_index].passengers;
		(*this)[next_index].type = STOP_TYPE_DEMAND;
		// (*this)[next_index].first_dest_call = entry_index;
		LinkDestination(sequence_start, other, entry_index);
		return 0;
	}

	int sq1 = sequence_start;
	// find the last node in the sequence
	while ((*this)[sq1].next != -1)
		sq1 = (*this)[sq1].next;

	// if the same floor, combine node
	// if ((*this)[sq1].floor == other[entry_index].origin)
	// CSW050913: if it is demand, combine it
	if ( ((*this)[sq1].type == STOP_TYPE_DEMAND) && ((*this)[sq1].floor == other[entry_index].origin))
	{
		combine(sq1,other[entry_index].passengers,0);  // +boarding,deboarding people only
		LinkOrigin(sq1,other, entry_index);
	}// CSW050913: if it is served stop, and there are passenger on board, then combine the call
	// else if (((*this)[sq1].type == STOP_TYPE_SERVED) && ((*this)[sq1].floor == other[entry_index].origin)&&
	//	((*this)[sq1].on_board != 0))
	// CSW051004: if it is served stop, then combine the call
	else if (((*this)[sq1].type == STOP_TYPE_SERVED) && ((*this)[sq1].floor == other[entry_index].origin))
	{
		combine(sq1,other[entry_index].passengers,0);  // +boarding,deboarding people only
		LinkOrigin(sq1,other, entry_index);
	}// CSW050914: if it is pivot stop, and it has reached NCF, then combine the call
	else if (((*this)[sq1].type == STOP_TYPE_PIVOT) && ((*this)[sq1].floor == other[entry_index].origin)&&
		((*this).current_NCF == (*this)[sq1].floor))
	{
		combine(sq1,other[entry_index].passengers,0);  // +boarding,deboarding people only
		LinkOrigin(sq1,other, entry_index);
	}
	// CSW051113: if it is a parking stop, then combine the call
	else if (((*this)[sq1].type == STOP_TYPE_PARKING) && ((*this)[sq1].floor == other[entry_index].origin))
	{
		combine(sq1,other[entry_index].passengers,0);  // +boarding,deboarding people only
		LinkOrigin(sq1,other, entry_index);
	}
	else  // else create a new node and put the origin infomation in the node
	{
		int new_stop = NewStop();
		(*this)[sq1].next = new_stop;				// connect the new stop to the list
		(*this)[new_stop].floor = other[entry_index].origin;
		(*this)[new_stop].boarding = (UCHAR)other[entry_index].passengers;
		(*this)[new_stop].type = STOP_TYPE_DEMAND;
		LinkOrigin(new_stop, other, entry_index);
		sq1 = new_stop;
	}

	// add the destination infor in the list

	int new_stop = NewStop();
	(*this)[sq1].next = new_stop;				// connect the new stop to the list
	(*this)[new_stop].floor = other[entry_index].destination;
	(*this)[new_stop].deboarding = (UCHAR) other[entry_index].passengers;
	(*this)[new_stop].type = STOP_TYPE_DEMAND;
	LinkDestination(new_stop, other, entry_index);

	UpdateOnBoard();				// reset on board values
	return 0;
}


// merge the sequence starting from s
// must find s first before this function can be called
int StopSequence::merge(int s, 
						 DestinationEntryCalls &sp, 
						 PLAN_INDEX_T ind)        // merge the subsequence 
{
	int sub_dir;
	
	// merge starting at s
	int merge_start = s;

	if (merge_start == -1)                    // if null is returned, append the sequence to the end
	{
		append(sp,ind);
		return 1;
	}

	int next, index_new;
	int f1, f2;

	sub_dir = sp[ind].direction();
	f1 = (*this)[merge_start].floor;
	f2 = sp[ind].origin;
	if (sub_dir == UP)   // direction is up
	{		
		if ( ( f2 < f1) && (merge_start == sequence_start))   // merge before sequence
		{
			index_new = NewStop();
			(*this)[index_new].floor = f2;
			(*this)[index_new].boarding = (UCHAR) sp[ind].passengers;
			(*this)[index_new].type = STOP_TYPE_DEMAND;
			InsertHead(index_new);                        // insert it as head of the linked list
			merge_start = index_new;
			LinkOrigin(index_new, sp, ind);					// link boarding 
		}
		else if (f1 == f2)
		{
			combine(merge_start, sp[ind].passengers,0);
			(*this)[merge_start].type = STOP_TYPE_DEMAND;		// CSW: 072505 make sure it is a demand stop
			LinkOrigin(merge_start, sp, ind);					// link boarding
		}
		else
		{
			int index_new = NewStop();
			(*this)[index_new].floor = f2;
			(*this)[index_new].boarding = (UCHAR) sp[ind].passengers;
			(*this)[index_new].type = STOP_TYPE_DEMAND;
			InsertAfter(merge_start, index_new);
			merge_start = index_new;
			LinkOrigin(index_new, sp, ind);					// link boarding
		}
		// find the next position to insert destination	
		f2 = sp[ind].destination;
		next = (*this)[merge_start].next;
		while ( (next != -1))
		{
			f1 = (*this)[next].floor;
			if (f1 <= f2)
			{
				merge_start = next;
				next = (*this)[merge_start].next;
			}
			else
				break;
		}
		// insert destination
		if ((*this)[merge_start].floor == f2)
		{
			combine(merge_start, 0,sp[ind].passengers);
			LinkDestination(merge_start, sp, ind);					// link boarding
		}
		else
		{
			int index_new = NewStop();
			(*this)[index_new].floor = f2;
			(*this)[index_new].deboarding = (UCHAR) sp[ind].passengers;
			(*this)[index_new].type = STOP_TYPE_DEMAND;
			InsertAfter(merge_start, index_new);
			merge_start = index_new;
			LinkDestination(index_new, sp, ind);					// link boarding
		}
	}
	else         // when the direction is down
	{		
		if ( ( f2 > f1) && (merge_start == sequence_start))   // merge before sequence
		{
			index_new = NewStop();
			(*this)[index_new].floor = f2;
			(*this)[index_new].boarding = (UCHAR)sp[ind].passengers;
			(*this)[index_new].type = STOP_TYPE_DEMAND;
			InsertHead(index_new);                        // insert it as head of the linked list
			merge_start = index_new;
			LinkOrigin(index_new, sp, ind);					// link boarding 
		}
		else if (f1 == f2)
			{
			combine(merge_start, sp[ind].passengers ,0);
			LinkOrigin(merge_start, sp, ind);					// link boarding
		}
		else 
		{
			int index_new = NewStop();
			(*this)[index_new].floor = f2;
			(*this)[index_new].boarding = (UCHAR) sp[ind].passengers;
			(*this)[index_new].type = STOP_TYPE_DEMAND;
			InsertAfter(merge_start, index_new);
			merge_start = index_new;
			LinkOrigin(index_new, sp, ind);					// link boarding
		}
		// find the next position to insert destination	
		f2 = sp[ind].destination;
		next = (*this)[merge_start].next;
		while ( (next != -1))
		{
			f1 = (*this)[next].floor;
			if (f1 >= f2)
			{
				merge_start = next;
				next = (*this)[merge_start].next;
			}
			else
				break;
		}
		// insert destination
		if ((*this)[merge_start].floor == f2)
		{
			combine(merge_start, 0,sp[ind].passengers);
			LinkDestination(merge_start, sp, ind);					// link deboarding
		}
		else
		{
			int index_new = NewStop();
			(*this)[index_new].floor = f2;
			(*this)[index_new].deboarding = (UCHAR) sp[ind].passengers;
			(*this)[index_new].type = STOP_TYPE_DEMAND;
			InsertAfter(merge_start, index_new);
			merge_start = index_new;
			LinkDestination(index_new, sp, ind);					// link deboarding
		}
	}
	UpdateOnBoard();				// reset on board values
	return 1;
}



/* this funciton merge the sequence to the last subsequence with the same direction */
int StopSequence::check_and_merge(DestinationEntryCalls &sp, 
						 PLAN_INDEX_T entry_ind, 
						 unsigned short capacity)             // merge the subsequence 
{
	int sub_dir;

	// find direction of the sub sequence
	sub_dir = sp[entry_ind].direction();
	
	// search for a subsequence to merge 
	int merge_start = -1;

    if (sequence_start == -1)						// if the sequence is empty, append the sequence to the end
	{
		append(sp, entry_ind);
		return 1;
	}

	// if a subsequence can be found 
	merge_start = FindMergeNode(sub_dir, sp, entry_ind);          // Find where to merge it

	// if merge point is not at the end of the sequence
	if (merge_start != -1)
	{
		// check capacity
		int f2 = sp[entry_ind].destination;		// compare to the destination of the call
		int index = merge_start;

		//CSW051115: if the index is sequence start, check the initial capacity
		if (index == sequence_start)
		{
			if ( current_on_board >= capacity)
				return 0;
		}
		// if the destination greater than the stop when up or less than the stop when down
		while ( ( index != -1) && ((*this)[index].floor*sub_dir <= f2*sub_dir))
		{
			// if the capacity of the stop is full
			if ( (*this)[index].on_board >= capacity*PASSENGER_UNIT)
				return 0;
			index = (*this)[index].next;
		}
		return merge(merge_start, sp, entry_ind);						// merge the sequence from the starting point
	}
	return 0;
}


/* this funciton find the last subsequence of a particular direction  */
/* return 1 if such a subsequence can be found */
int StopSequence::LocateSub(int dir, PLAN_INDEX_T start, PLAN_INDEX_T end)                // merge the subsequence 
{
	int next_dir, curr_dir = NO_DIR;
	// if no sequence is specified, return 0
	if (-1 == sequence_start)   
		return 0;

	int st1, st2;
	st1 = st2 = sequence_start;

	int f1, f2;             // two floor values to determine direction
	next_dir = InitialDir();
	f1 = f2 = origin();

	while (st2 != -1)
	{ 
		if (next_dir == dir) 
		{
			if (next_dir != curr_dir)   // if there is a direction change
			{
				start = end = st1;
				end = st2;
			}
			else
				end = st2;
			
		}

		curr_dir = next_dir;        // reset current direction

		f1 = f2;
		st1 = st2;
		st2 = (*this)[st2].next;
        if (st2 != -1)
			f2 = (*this)[st2].floor;    
		// find direction to the next stop
		next_dir = GetDir(f1,f2);	
	}
	
	return 1;
}


/*
 * Find Merge point given sequence and direction 
 * Return NULL if the sequence can only be append at the end
 */

int StopSequence::FindMergeNode(int dir, 
								 DestinationEntryCalls &c, 
								 PLAN_INDEX_T ind)
{
	int next_dir, curr_dir;
	int merge_node = -1;

	// if no sequence is specified, return NULL
	if (-1 == sequence_start)   
		return -1;

	int  st1, st2;
	st1 = sequence_start;

	// Check NCF and first node
	if ( current_NCF != (*this)[st1].floor)
	{
		curr_dir = GetDir(current_NCF, (*this)[st1].floor);
		if ( (curr_dir == DN) &&(current_NCF >= c[ind].origin) && (c[ind].origin >= (*this)[st1].floor ) && (c[ind].direction() == curr_dir) )
		{
			merge_node = st1;
	//		return merge_node;
		}
		else if ( (curr_dir == UP) && (current_NCF <= c[ind].origin) && (c[ind].origin <= (*this)[st1].floor )&& (c[ind].direction() == curr_dir) )
		{
			merge_node = st1;
	//		return merge_node;
		}
	}

	int f1, f2;             // two floor values to determine direction
	curr_dir = InitialDir();


	f1 = f2 = (*this)[st1].floor;
	if (st1 != -1) {
		st2 = (*this)[st1].next;
		if ( st2 != -1)
			f2 = (*this)[st2].floor;
	}

	// not use GetDir function because we want to maintain the current direction for No_dir
	if (f1 < f2)
		next_dir = UP;
	else if (f1 > f2)
		next_dir = DN;
	else
		next_dir = curr_dir;

	while (st1 != -1)
	{ 
		if (next_dir == dir)                // if next direction is the same
		{			
			if ( ((*this)[st1].floor >= c[ind].origin) && (dir == DN))

				merge_node = st1;
			else if ( ((*this)[st1].floor <= c[ind].origin) && (dir == UP))
				merge_node = st1;
		}
		// CSW050822 : if NO_DIR and current floor the passenger arrive at the current floor,set merge node at current floor
		// else if ( (next_dir == NO_DIR) && ((*this)[st1].floor == c[ind].origin))
		else if (next_dir == NO_DIR) // CSW050907 : does not have to depend on origin
			merge_node = st1;
		// CSW050822 : end
		else    // if next direction is on opposite direction, set merge point to NULL
			merge_node = -1;

		curr_dir = next_dir;        // reset current direction

		f1 = f2;
		st1 = st2;
        if (st2 != -1)
		{
			st2 = (*this)[st2].next;
			if (st2 != -1)
		 		f2 = (*this)[st2].floor; 
		}
		// find direction to the next stop
		// not use GetDir function because we want to maintain the current direction for No dir 
		if (f1 < f2)
			next_dir = UP;
		else if (f1 > f2)
			next_dir = DN;
		else
			next_dir = curr_dir;	
	}
	return merge_node;
}


// link the call with the same boarding to the corresponding stop
void StopSequence::LinkOrigin(PLAN_INDEX_T s, DestinationEntryCalls &c, PLAN_INDEX_T entry_index)
{
	if ((*this)[s].first_origin_call == -1)
	{
		(*this)[s].first_origin_call = entry_index;
		c[entry_index].next_same_boarding = -1;
	}
	else
	{
		int ind = (*this)[s].first_origin_call;
		while (c[ind].next_same_boarding != -1)
			ind = c[ind].next_same_boarding;
		c[ind].next_same_boarding = entry_index;
		c[entry_index].next_same_boarding = -1;
	}
}

// link the call with the same deboarding to the corresponding stop
void StopSequence::LinkDestination(PLAN_INDEX_T s, DestinationEntryCalls &c, PLAN_INDEX_T entry_index)
{
	if ((*this)[s].first_dest_call == -1)
	{
		(*this)[s].first_dest_call = entry_index;
		c[entry_index].next_same_deboarding = -1;
	}
	else
	{
		int ind = (*this)[s].first_dest_call;
		while (c[ind].next_same_deboarding != -1)
			ind = c[ind].next_same_deboarding;
		c[ind].next_same_deboarding = entry_index;
		c[entry_index].next_same_deboarding = -1;
	}
}


/*  Update on_board valua for each stop 
 *  This function should be executed after merge operation is finished. 
 *  This function will set on_board value and figure out empty or none-empty move
 */

void StopSequence::UpdateOnBoard()
{
	int ind = 0;
	int on_board = current_on_board*PASSENGER_UNIT;

	int s = sequence_start;

	while (s != -1)
	{
		if (on_board <= 0)
			(*this)[s].empty = true;
		else
			(*this)[s].empty = false;
		on_board = on_board + ((*this)[s].boarding - (*this)[s].deboarding) * PASSENGER_UNIT;
		(*this)[s].on_board = on_board;
		(*this)[s].index = ind;
		s = (*this)[s].next;
		ind ++;
	}
}

void StopSequence::debugPrint(void) // print stop sequence into debug pane of VC++
{
#ifdef	WITH_DEBUG
	int s = sequence_start;
	_RPT2(_CRT_WARN,"Current NCF = %d, Initial Passengers = %d\n", current_NCF, current_on_board);

	while (s != -1) 
	{
		_RPT3(_CRT_WARN, "[%d] stop %d: floor = %d\n", s, (*this)[s].index, (*this)[s].floor);
		s = (*this)[s].next;
	}
#endif
}

// print content of the stop sequence to a file
void StopSequence::print(StopSequence &other, FILE *output)
{
	fprintf(output, "Current NCF = %d, Initial Passengers = %d\n", current_NCF, current_on_board);

	int s = sequence_start;
	while (s != -1) 
	{
		fprintf(output, "stop %d: floor = %d\n", (*this)[s].index, (*this)[s].floor);
		fprintf(output, "boarding = %d, deboarding = %d, on board = %.1f\n", (*this)[s].boarding, (*this)[s].deboarding, (float)(*this)[s].on_board/PASSENGER_UNIT);
		if ((*this)[s].empty)
			fprintf(output,"Empty move to the stop\n");
		else
			fprintf(output,"None-empty move to the stop\n");

		switch ((*this)[s].type)
		{
		case STOP_TYPE_DEMAND:
			fprintf(output,"Stop Type: Demand\n");
			break;
		case STOP_TYPE_CONDITIONAL:
			fprintf(output,"Stop Type: Conditional\n");
			break;
		case STOP_TYPE_YIELD:
			fprintf(output,"Stop Type: Yield\n");
			break;
		case STOP_TYPE_PARKING:
			fprintf(output,"Stop Type: Parking\n");
			break;
		case STOP_TYPE_SERVED:
			fprintf(output,"Stop Type: Served\n");
			break;
		case STOP_TYPE_PIVOT:
			fprintf(output,"Stop Type: Pivot\n");
			break;
		default:
			break;
		}
		
		// if there is a arrival precedessor
		if ((*this)[s].arrival_predecessor != -1)
		{
			int s2 = (*this)[s].arrival_predecessor;
			fprintf(output, "Arrival Precedessor :%d\n",other[s2].index);
		}

		// if there is a arrival successor
		if ((*this)[s].arrival_successor != -1)
		{
			int s2 = (*this)[s].arrival_successor;
			fprintf(output, "Arrival Successor :%d\n",other[s2].index);
		}

		// if there is a departure precedessor
		if ((*this)[s].departure_predecessor != -1)
		{
			int s2 = (*this)[s].departure_predecessor;
			fprintf(output, "Departure Precedessor :%d\n",other[s2].index);
		}

		// if there is a departure precedessor
		if ((*this)[s].departure_successor != -1)
		{
			int s2 = (*this)[s].departure_successor;
			fprintf(output, "Departure Successor :%d\n",other[s2].index);
		}

		fprintf(output,"\n");
		s = (*this)[s].next;
	}
}

// comparison between Destination Entry calls, used for the sorting routine
bool DestinationEntry:: operator > ( DestinationEntry &other)
{
	short my_direction = direction();
	short other_direction = other.direction(); 

	// up is greater than dn
	if (my_direction > other_direction)
		return true;

	// for upward call
	if (my_direction == UP)
	{
		// the smaller the origin, the higher priority
		if ( origin < other.origin) 
			return true;
		else if ( (origin == other.origin) && (reg_time < other.reg_time))
			return true;
		else 
			return false;
	}
	else  // for downward call
	{
		// the larger the origin, the higher priority
		if ( origin > other.origin) 
			return true;
		else if ( (origin == other.origin) && (reg_time < other.reg_time))
			return true;
		else 
			return false;		
	}
}

DestinationEntry & DestinationEntryCalls::operator [](PLAN_INDEX_T ind)	// returns the DestinationEntry at ind
{
	if ( (ind >= 0) && (ind <MAX_CALL))
		return calls[ind];
	else
	{
		fprintf(debug_output,"Index out of range in DestinationEntryCalls: %d\n",ind);
		// getchar();            // make the program pause, so one can find where the error occur with debugger
	}; 
	return calls[0];		  // return the first element
};

// only print the stop sequence to a file
void StopSequence::PrintSequence(FILE *output)
{
	PLAN_INDEX_T s = sequence_start;

	while (s != -1) 
	{
		fprintf(output, "%d ", (*this)[s].floor);
		s = (*this)[s].next;
	}
	fprintf(output,"\n");
}



// reset the content of destination entry calls
void DestinationEntryCalls::reset()
{    
	empty_list = 0;
	for (int i=0; i<MAX_CALL; i++)
	{
		calls[i].next = i+1;
		calls[i].next_same_boarding = calls[i].next_same_deboarding = -1;
	}
	calls[MAX_CALL-1].next = -1;
	sequence_start = -1;
}

// return a index of a new call that can be used
// the new call will be disconnected from the list of free calls
PLAN_INDEX_T DestinationEntryCalls::NewCall()
{
	// if run out of available memory, return -1
	if (empty_list == -1)
		return -1;

	int ind = empty_list;

	empty_list = (*this)[empty_list].next;
	return ind;
}
// initialize the destination entry calls
void DestinationEntryCalls::init(int inputs[MAX_CALL][2],int num_calls)
{
	reset();
	// if the number of calls is greater than the maxinum number of calls, only takes up to MAX_CALL
	if (num_calls >= MAX_CALL)
	{
		num_calls = MAX_CALL;
		empty_list = -1;
	}
	else
		empty_list = num_calls;				// set the empty list 
	sequence_start = 0;					// set sequence start
	for (int i=0; i<num_calls; i++)
	{
		calls[i].origin = inputs[i][0];
		calls[i].destination = inputs[i][1];
		calls[i].passengers = 1;
	}
	calls[num_calls-1].next = -1;		// mark the end of the entry calls
	empty_list = num_calls;
}

// copy the content of the other DestinationEntryCalls
void DestinationEntryCalls::copy(DestinationEntryCalls &other)
{
	int other_index = other.sequence_start;
	int my_index = 0;

	reset();			// reset my structure

	// CSW051111: start index should not be the same as the others
	// sequence_start = other_index;			// set the sequence start
	sequence_start = -1;			// set the sequence start
	// loop until the end of the other calls
	while (other_index != -1)
	{
		calls[my_index].origin = other[other_index].origin;
		calls[my_index].destination = other[other_index].destination;
		calls[my_index].passengers = other[other_index].passengers;
		other_index = other[other_index].next;
		my_index++;
	}
	if (my_index>0)
	{
		calls[my_index-1].next = -1;		// mark the end of the calls
	}
	empty_list = my_index;
}

// return the number of entry calls 
int DestinationEntryCalls::size()
{
	int index = sequence_start;
	int s = 0;

	while (index != -1) 
	{
		index = calls[index].next;
		s++;
	}
	return s;
}

// check whether the sequence is empty or not
bool DestinationEntryCalls::empty()
{
	if ( sequence_start == -1)
		return true;
	else
		return false;
}
// Sort the calls based on their priority
// Selection sort is implemented

void DestinationEntryCalls::sort()
{
	PLAN_INDEX_T start, pre_start, candidate, pre_candidate;

	// if the entry calls is empty, return immediately
	if (sequence_start == -1)
		return;

	pre_candidate = candidate = sequence_start;
	// find the largest call and use it as sequence_start 
	while ( (candidate = calls[candidate].next) != -1)
	{
		// if candidate is greater than sequence_start, swap it
		if (calls[candidate] > calls[sequence_start])
		{
			PLAN_INDEX_T temp;
			// if candidte is not the second entry call
			if (pre_candidate != sequence_start)
			{
				calls[pre_candidate].next = sequence_start;
				temp = calls[candidate].next;
				calls[candidate].next = calls[sequence_start].next;
				calls[sequence_start].next = temp;
			}
			else
			{
				temp = calls[candidate].next;
				calls[candidate].next = sequence_start;
				calls[sequence_start].next = temp;
			}
			// update candidate, pre_candidate and sequence_start
			pre_candidate = sequence_start;
			sequence_start = candidate;
			candidate = pre_candidate;
		}
		pre_candidate = candidate;  // move to the next candidate
	}

	pre_start = start = sequence_start;
	if (sequence_start != -1)
		start = calls[sequence_start].next;
    // find the rest of the calls
	while (start != -1)
	{
		pre_candidate = candidate = start;
		while ( (candidate = calls[candidate].next) != -1)
		{
			// if candidate is greater than sequence_start, swap it
			if (calls[candidate] > calls[start])
			{
				PLAN_INDEX_T temp;

				// if candidate is not the next call of start
				if (start != pre_candidate)
				{
					temp = calls[candidate].next;
					calls[candidate].next = calls[start].next;
					calls[start].next = temp;
					calls[pre_candidate].next = start;
					calls[pre_start].next = candidate;
				}
				else
				{
					temp = calls[candidate].next;
					calls[candidate].next = start;
					calls[start].next = temp;
					calls[pre_start].next = candidate;
				}
				// update start, pre_candidate and candidate
				pre_candidate = start;
				start = candidate;
				candidate = pre_candidate;
			}
			pre_candidate = candidate;		// move to the next candidate
		}
		pre_start = start;
		start = calls[start].next;
	}

}


// Move calls with the same origin to the head of the list
// So they can be processed first
void DestinationEntryCalls::MoveOriginForward(PLAN_INDEX_T start, PLAN_INDEX_T origin)
{

	PLAN_INDEX_T candidate, pre_candidate;

	if (start == -1)
		return;
	
	pre_candidate = start;
	candidate = calls[start].next;

	while (candidate != -1)
	{
		// if found a call that should be move forward
		if (calls[candidate].origin == origin)
		{
			if (pre_candidate != start)		// if precandidate is not the same as star
			{
				calls[pre_candidate].next = calls[candidate].next;
				calls[candidate].next = calls[start].next;
				calls[start].next = candidate;
				start = candidate;
				candidate = calls[pre_candidate].next;
			}
			else // else precandidate is equal to start, does not need to swap
			{
				pre_candidate = candidate;
				start = candidate;
				candidate = calls[candidate].next;		// advance to the next candidate
			}
		}
		else   // advance to the next candidate
		{
		  pre_candidate = candidate;
		  candidate = calls[candidate].next;
		}
	}
}

// Print Unprocessed calls in DestinationEntryCalls
void DestinationEntryCalls::PrintCalls(FILE *output)
{
	int ind = 0;
	int i = sequence_start;

	while (i != -1)
	{
		fprintf(output, "Calls %d: origin:%d, destination: %d, passengers: %.1f\n", ++ind, calls[i].origin, 
			calls[i].destination, (float) calls[i].passengers);  
		i = calls[i].next;
	}
}
// remove the call whose index is calls[prev_ind].next from list 
// return the index of the next call on the list
// if prev_ind = -1, the next one is sequence_start
PLAN_INDEX_T DestinationEntryCalls::RemoveFromList(PLAN_INDEX_T prev_ind) 
{
	int i = sequence_start;

	if (-1 == i)
		return -1;

	if (prev_ind == -1)
	{
		i = sequence_start;
		sequence_start = calls[i].next;
	}
	else
	{
		i = calls[prev_ind].next;
		calls[prev_ind].next = calls[i].next;
	}

	return calls[i].next;
}

// copy committed calls from Call list
int DestinationEntryCalls::CopyCommittedCalls(CallList &committedCalls)
{
	CALL_INDEX_T ind = committedCalls.boardedIndex;

	reset();

	CALL_INDEX_T prevInd;

	// copy calls there are boarded 
	while (ind != -1) 
	{
		CALL_INDEX_T newInd = NewCall();

		if (newInd >= 0)
		{
			calls[newInd].origin = committedCalls.calls[ind].origin;
			calls[newInd].destination = committedCalls.calls[ind].destination;
			calls[newInd].passengers = committedCalls.calls[ind].nPassengers;
			calls[newInd].reg_time = MAKE_FLOAT_TIME(committedCalls.calls[ind].ctRegistration); //shs0729
			calls[newInd].status=PASSENGER_BOARDED;  // shs0719
			calls[newInd].next = -1;
			if (sequence_start == -1)
			{
				sequence_start = newInd;
				prevInd = newInd;
			}
			else
			{
				calls[prevInd].next = newInd;
				prevInd = newInd;
			}
		}
		else		// CSW051116: if there is not enough memory for the calls, return -1
			return -1;
		ind = committedCalls.calls[ind].next;
	}

	return 0;
}

// add a parking stop at the beginning of sequence based on car state
void StopSequence::AddParkingStop(CarState &state)
{
	PLAN_INDEX_T stopInd = NewStop();
	sequence[stopInd].floor=state.target;

	// CSW051012: Use yield stop if the car is still moving. It must be moving to a yield stop
	// CSW051113: After adding dispatching parking scheme, it could be moving to a parking stop
	if (state.motionState == MOVING)
	{
		// sequence[stopInd].type=STOP_TYPE_YIELD;
		sequence[stopInd].type=STOP_TYPE_PARKING;
	}
	else
	{
		sequence[stopInd].type=STOP_TYPE_PARKING;
	}
	sequence[stopInd].on_board=0;
	sequence[stopInd].empty=true;
	sequence[stopInd].next = -1;
	sequence_start = stopInd;
}

// add a pivot stop at the beginning of sequence based on car state
void StopSequence::CheckPivotStop(CarState &state)
{
	if (sequence_start == -1)
		return ;

	CHAR d1, d2;
	PLAN_INDEX_T next = sequence[sequence_start].next;
	d1 = GetDir(state.currentFloor, sequence[sequence_start].floor);
	d2 = GetDir(state.currentFloor, sequence[next].floor);

	// CSW050901: If it is a pivot stop and the car keep moving along the direction, considering remove this stop
	if ( (sequence[sequence_start].type == STOP_TYPE_PIVOT)  &&(d1 == d2) && (state.target != sequence[sequence_start].floor))
	{
		// if there is is a precedence constrait on this stop, change the type to conditional
		if ((sequence[sequence_start].arrival_predecessor>=0) || (sequence[sequence_start].departure_predecessor>=0)||
			(sequence[sequence_start].arrival_successor>=0) || (sequence[sequence_start].departure_successor>=0))		
		{
			sequence[sequence_start].type = STOP_TYPE_CONDITIONAL;
			
		}
		else  // otherwise it can be removed
		{
			sequence[sequence_start].next = empty_list;
			empty_list = sequence_start;
			sequence_start = next;
		}
	}
	// CSW050901: end 

}

// add a pivot stop at the beginning of sequence based on car state
PLAN_INDEX_T StopSequence::AddPivotStop(CarState &state)
{
	if (sequence_start == -1)
		return -1;

	// if the currentNCF is equal to the first stop, there is no need for pivot stop
	if (state.currentNCF == sequence[sequence_start].floor)
		return -1;


	PLAN_INDEX_T stopInd = NewStop();
	sequence[stopInd].floor=state.currentNCF;
	sequence[stopInd].type=STOP_TYPE_PIVOT;
	sequence[stopInd].on_board=0;
	sequence[stopInd].empty=true;
	sequence[stopInd].next = sequence_start;
	sequence_start = stopInd;
	UpdateOnBoard();
	return stopInd;
	
}
/***************************************************************************************************
 * Add Arrival prededence constraint from index fromIndex to toIndex. The variable fromCar should  *
 * be either 0 (lower car) or 1 (upper car). It indicates which car is the relationship from       *
 ***************************************************************************************************/
void HoistwayPlan::AddArrivalPrecedence(UCHAR fromCar, PLAN_INDEX_T fromIndex, PLAN_INDEX_T toIndex)
{
	UCHAR toCar;
	if (UPPER_CAR == fromCar)
		toCar = LOWER_CAR;
	else
		toCar = UPPER_CAR;

	PLAN_INDEX_T otherIndex = sequences[fromCar][fromIndex].arrival_successor;
	if (otherIndex != -1)
		sequences[toCar][otherIndex].arrival_predecessor = -1;
	sequences[fromCar][fromIndex].arrival_successor = toIndex;

	otherIndex = sequences[toCar][toIndex].arrival_predecessor;
	if (otherIndex != -1)
		sequences[fromCar][otherIndex].arrival_successor = -1;
	sequences[toCar][toIndex].arrival_predecessor = fromIndex;

	// CSW051116: make sure there is no dead lock 
	if (sequences[fromCar][fromIndex].arrival_predecessor == toIndex)
	{
		sequences[fromCar][fromIndex].arrival_predecessor = -1;
		sequences[toCar][toIndex].arrival_successor = -1;
	}
}

/*****************************************************************************************************
 * Add Departure prededence constraint from index fromIndex to toIndex. The variable fromCar should  *
 * be either 0 (lower car) or 1 (upper car). It indicates which car is the relationship from         *
 *****************************************************************************************************/
void HoistwayPlan::AddDeparturePrecedence(UCHAR fromCar, PLAN_INDEX_T fromIndex, PLAN_INDEX_T toIndex)
{
	UCHAR toCar;
	if (UPPER_CAR == fromCar)
		toCar = LOWER_CAR;
	else
		toCar = UPPER_CAR;

	PLAN_INDEX_T otherIndex = sequences[fromCar][fromIndex].departure_successor;
	if (otherIndex != -1)
		sequences[toCar][otherIndex].departure_predecessor = -1;
	sequences[fromCar][fromIndex].departure_successor = toIndex;

	otherIndex = sequences[toCar][toIndex].departure_predecessor;
	if (otherIndex != -1)
		sequences[fromCar][otherIndex].departure_successor = -1;
	sequences[toCar][toIndex].departure_predecessor = fromIndex;

	// CSW051116: make sure there is no dead lock 
	if (sequences[fromCar][fromIndex].departure_predecessor == toIndex)
	{
		sequences[fromCar][fromIndex].departure_predecessor = -1;
		sequences[toCar][toIndex].departure_successor = -1;
	}
}

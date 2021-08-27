#ifndef	_HOISTWAY_PLAN_H
#define	_HOISTWAY_PLAN_H

#include <stdio.H>
#include "..\common\2DInterfaceData.h"

// CSW051019: Turn on WITH_DEBUG only in debug mode
#ifdef _DEBUG
#define WITH_DEBUG
#endif

/*
#ifndef WITH_DEBUG
#define WITH_DEBUG
#endif
*/



/* define types of stops */
#define STOP_TYPE_DEMAND	0
#define	STOP_TYPE_CONDITIONAL	1
#define STOP_TYPE_YIELD		2
#define STOP_TYPE_PARKING	3
#define STOP_TYPE_SERVED    4   //jsr added for reopen 
#define STOP_TYPE_PIVOT		5	//CSW added for handling pivot stop

/* define predecessor type */
#define STOP_PREDECESSOR_ARRIVAL	0
#define STOP_PREDECESSOR_DEPARTURE	1

/* define car direction */
#define UP				1
#define DN				-1
#define NO_DIR			0
#define NO_PREDIRECTION	-2

// CSW questions: what are these for?
// shs exp: this is for array index when the direction is used for array 
//        ex. HallCall[UP_INDEX] means up hall call ....
#define MAXDIR				2
#define UP_INDEX			0
#define DN_INDEX			1
// CSW 

#define NO_FLOOR	-1						// represent the Current NCF is not specified
#define INFINITE_FLOOR		9999                    // represent very large floor number


// define the number of passenger units each passenger represent. For calculating capacity
#define PASSENGER_UNIT	100

// typedef unsigned char FLOOR_T;	// floor index type
//  typedef short	PLAN_INDEX_T;			// index to use in hoistway plan as pointers


// return direction given two floor number
inline int GetDir(int f1, int f2)
{
	if ( (f1 == (FLOOR_T) NO_FLOOR) || (f2 == (FLOOR_T) NO_FLOOR))
		return NO_DIR;
	else if (f1 - f2 <0)							// determine the direction from s
		return UP;
	if (f1 == f2)
		return NO_DIR;
	else
		return DN;
}

/* definition of data in each stop */
class Stop
{
public:
	FLOOR_T floor;			// the floor of the stop
	unsigned char type;			// which type of stop
	unsigned char boarding;		// Number of passengers boarding the car
	unsigned char deboarding;      // Number of passengers deboarding the car
	unsigned short on_board;		// number of passenger units on board when the car departs the stop, use short for easy process
	bool empty;						// empty or non-empty when the car move to the stop
	PLAN_INDEX_T index;				// index of stop order in a sequence
	PLAN_INDEX_T departure_predecessor;		// index of the stop of the other car that must precedes this
	PLAN_INDEX_T departure_successor;		// index of departure successor
	PLAN_INDEX_T arrival_successor;			// index of arrival successor;
	PLAN_INDEX_T arrival_predecessor;		// index of arrival precedessor;
	PLAN_INDEX_T next;						// index of the next stop of the same car, can also be used to link the next available data
	PLAN_INDEX_T first_origin_call;			// index to the first call whose origin is the floor
	PLAN_INDEX_T first_dest_call;			// index to the first call whose destination is the floor
	inline reset() {				// clear the content of the stop
		type = STOP_TYPE_DEMAND;    
		on_board = 0;
		boarding = deboarding = 0;
		departure_predecessor = arrival_predecessor = next = -1;
		arrival_successor = departure_successor = -1;
	};
	void copy(Stop &other);			// copy the content of other stop
	void CopyStop(Stop &other);		// only copy information related to the stop, not including link information
	// shs add begin for timing calculation
	/* Timing can be separated from this class since timing does not need to be passed between modules
	 * Timing module only need this for evaluating plans, so it can have its own struture for timing
	 */
	// shs fix2 : easy implementation, optimization later
	// CSW: The following can be easily implemented in timing module. Using a struct to contain these variables. 
	/* 
	double arrival_time;  // arrival time to the position of this floor
	double nominal_door_time; // nominal door dwell time + door open + door close time
	double time_extended_during_stop; // only extended door time for keeping separation and precedence relationship
	double departure_time; // departure time after calibration
	*/
	/* for example
	  struct StopTimes
	  {
	    double arrival_time;  // arrival time to the position of this floor
	    double nominal_door_time; // nominal door dwell time + door open + door close time
	    double time_extended_during_stop; // only extended door time for keeping separation and precedence relationship
	    double departure_time; // departure time after calibration
	  }
	  StopTimes time[MAX_STOP];  // corresponding times for each stop
	  These timing is only local to timing module, other module will not need it. 
	  */

	// shs add end
};

/* definition of a destination entry */
class DestinationEntry
{
public:
	FLOOR_T origin;				// the origin floor of the call
	FLOOR_T destination;			// the destination floor of the call
	PLAN_INDEX_T next;							// link to the next unregistered call
	PLAN_INDEX_T next_same_boarding;				// index of the call that has the same boarding
	PLAN_INDEX_T next_same_deboarding;			// index of the call that has the same deboarding
	// shs add begin for scoring
	unsigned short passengers;    // number of passengers associated with this entry call, use short for easy process
	char status; // waiting/boarded/deboarded
	float reg_time; // registering of this destination entry call time for calculating metrics
	float origin_position_time; // calculated or passed time for car to arrive at the origin floor position
	float dest_position_time; // calculated time for car to arrivae at the destination floor position
	// shs add end
	inline int direction()					// return the direction of the call
	{
		return GetDir(origin, destination);
	}
	bool operator > (DestinationEntry &other);    // define the comparison function for DestinationEntry
};

// class for handing destination entry calls
class DestinationEntryCalls
{
//private:
public:
	DestinationEntry calls[MAX_CALL];	// destination entry calls associated with the plan
	PLAN_INDEX_T empty_list;					// linked list of available stops can be used in the sequence
	PLAN_INDEX_T sequence_start;
public:
	friend class HoistwayPlanGen;			// let class HoistwayPlanAlg access private members
	friend class StopSequence;				// let class StopSequence access private members
	friend class HoistwaySchedule;		// let class HoistwaySchedule access private members
	DestinationEntry & operator [](PLAN_INDEX_T ind);	// returns the DestinationEntry at ind
	void reset();							// reset the content so it can be reused. 
	CALL_INDEX_T NewCall();					// return index of a new call in the structure
	void init(int inputs[][2],int num_calls); // initialize the class with a integer matrix
	void copy(DestinationEntryCalls &other);			// copy the content of the other calls
	int size();								// return the size of the calls in the structure
	bool empty();							// Check whether there is call remaining
	void sort();							// sort the destination entry calls based on origin and destination
	void MoveOriginForward(PLAN_INDEX_T start, PLAN_INDEX_T origin);    // move calls with the same origin after index start forward, so they can be processed first
	void PrintCalls(FILE *output);			// print calls in the data structure
	PLAN_INDEX_T RemoveFromList(PLAN_INDEX_T prev_ind);  // remove the call whose index is calls[prev_ind].next from list 
	int CopyCommittedCalls(CallList &);           // copy committed calls from Call list
};

/* definition of stop sequence */
class StopSequence  
{
//private:
public:
	FLOOR_T current_NCF;							// current NCF (replace  init_floor)
	unsigned char current_on_board;					// number of passengers on board initially
	Stop sequence[MAX_STOP];						// a sequence of stops
	PLAN_INDEX_T empty_list;							// linked list of available stops can be used in the sequence
	PLAN_INDEX_T sequence_start;						// index to the start of the sequence					
public:
	friend class HoistwayPlanGen;							// Let HoistwayPlanAlg to access private members
	friend class HoistwaySchedule;
	StopSequence();									// default construtor, set some parameters
	Stop & operator [](PLAN_INDEX_T ind);	// returns the DestinationEntry at ind
	StopSequence(int *sequence, int size);					// construct stop sequence by array
	StopSequence(StopSequence &other);					// constructor using another stop sequence
	void reset();											// Reset the content of the stop sequence, so it can be reused
	bool empty();											// check if the sequence is empty or not
	PLAN_INDEX_T NewStop();									// return index a new stop in the structure
	void free(PLAN_INDEX_T ind);								// free stop ind
	void InsertHead(PLAN_INDEX_T s);							// insert as the head of the linked list
    void InsertAfter(int a, int s);							// insert s afer a
	void combine(PLAN_INDEX_T a, unsigned short boarding, unsigned short deboarding);	// combine node a and s. a and s must be on the same floor
	int append(DestinationEntryCalls &sp, PLAN_INDEX_T ind);	// append the entry call to the end
	int check_and_merge( DestinationEntryCalls &c, PLAN_INDEX_T ind, unsigned short capacity);	    // merge the call specified by the index if there is enough capacity  
	int merge( int s, DestinationEntryCalls &c, PLAN_INDEX_T ind);        // merge the sequence from the point s
	int LocateSub( int dir, PLAN_INDEX_T start, PLAN_INDEX_T end);		// locate subsequence of a particular direction
	int FindMergeNode(int dir, DestinationEntryCalls &c, PLAN_INDEX_T ind);		// Find the node where the sequence can be inserted after
	void LinkOrigin(PLAN_INDEX_T s, DestinationEntryCalls &c, PLAN_INDEX_T ind);    // link call ind as the same origin from the stop
	void LinkDestination(PLAN_INDEX_T s, DestinationEntryCalls &c, PLAN_INDEX_T ind);    // link call ind as the same destination from the stop
	int number_of_stops();									// return number of stops in the sequence
	void copy(StopSequence &other);						// copy the content of the other stop sequence
	void CopyStop(StopSequence &other);			// only copy information related to the stop, not including the link information
	StopSequence & operator = ( StopSequence & oper2);	// operator = for StopSequence
	int origin();				// the starting position of the sequence
	int InitialDir();			// the initial direction of the sequence 
	int LastDir();				// The direction of the last subsequence 
	int LastFloor();            // the last floor of the committed sequence
    void UpdateOnBoard();			// reset the order index of the sequence
	void print(StopSequence &other, FILE *output);	// print result to an output file
	void PrintSequence(FILE *output);		// print sequence only to the output file
	void AddParkingStop(CarState &state);	// add a parking stop as the first stop
	void CheckPivotStop(CarState &state);	// check whether there is a need to add pivot stop
	PLAN_INDEX_T AddPivotStop(CarState &state);		// check whether pivot stop should be added
	void debugPrint(void); // print for debug pane
};



// shs add begin
#define	LOWER_CAR	0
#define	UPPER_CAR	1
#define	MAX_CARS_IN_HOISTWAY	2			// Maximum # of cars in a hoistway : in 2-D, it is 2
/* definition of hoistway plan */
class HoistwayPlan
{
public:
	StopSequence sequences[MAX_CARS_IN_HOISTWAY];				// sequences of cars in hoistway
	DestinationEntryCalls	de_calls[MAX_CARS_IN_HOISTWAY];		// calls assiged to cars 
	void print(FILE *output);
	void AddArrivalPrecedence(UCHAR fromCar, PLAN_INDEX_T fromIndex, PLAN_INDEX_T toIndex);
	void AddDeparturePrecedence(UCHAR fromCar, PLAN_INDEX_T fromIndex, PLAN_INDEX_T toIndex);
};

/* CSW: Need discussion 
// shs add end

//jsr add begin
// car operation
#define	IDLE 					0
#define	OPENING 				1
#define	DWELL 					2
#define	CLOSING 				3
#define	RUNNING 				4
#define	FORCED_MOVE 			5
#define	FORCED_PARKING 			6
#define	END_OF_FORCED_PARKING 	7
#define WAIT_INITIATE 			8
#define WAIT_RELEASE 			9
#define	WAIT_CAR_RANGE 			10
#define	MOVE_CAR_RANGE 			11
#define CLOSED_HOLD 			12		// Force not to move, keeping door closed
										// after start delay, delayed opening, etc

struct NextState
{
  int oper;           // current operation: 0=idle, 1=door opening, 2=dwell, 3=door closing,
                      //   4=running,5= wait while closed for (start delay,stop delay,conditional stop)
  double t_end_oper;  // time to end of operation
  double t_next_event; // time to check conditional stop... etc
  int target;		  // for the case of running
};

// stucture for car state
struct CarState
{
	FLOOR_T current_NCF;							// current NCF (replace  init_floor)
	unsigned char current_on_board;					// number of passengers on board initially
	NextState	car_state;							// state of the car
};
*/
#endif
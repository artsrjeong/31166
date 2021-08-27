#ifndef	_HP_DEFS_
#define	_HP_DEFS_

#include "hoistway_plan.h"
typedef	float T_HPSCORE;		// shs changed for cogs for returning score in floating point
#define	T_HPSCORE_MAX	2147483647		// for Maximum value of long int (shs0719)

/**************************************************************************************************
 * These data structures are static information that should be initialized. Various modules may need
 * the same information. 
 **************************************************************************************************/

//typedef long T_TIME;					// unit is milli seconds, adjust to different precision if necessary 
typedef	double T_TIME;				// change for COGS. integer represenation for floating point not good in PC

struct HPBuildingInfo				// building information that should be updated during initialization
{
	unsigned char	nCars;			// total number of cars in a group
	unsigned char	nHoistways;	// total number of hoistway in a group
	FLOOR_T	lobby1Floor;			// floor index of lobby 1
	FLOOR_T	lobby2Floor;			// floor index of lobby 2
	FLOOR_T	lobby3Floor;			// floor index of lobby 3
	FLOOR_T	rest1Floor;			// floor index of restaurant 1
	FLOOR_T	rest2Floor;			// floor index of restaurant 2
	FLOOR_T	EZ1Top;				// floor index of express zone 1 top
	FLOOR_T	EZ1Bottom;			// floor index of express zone 1 bottom
	FLOOR_T	EZ2Top;				// floor index of express zone 2 top
	FLOOR_T	EZ2Bottom;			// floor index of express zone 2 bottom
	FLOOR_T	EZ3Top;				// shs new
	FLOOR_T	EZ3Bottom;			// shs new
	double	floorPosition[MAX_FLOOR];	// floor height for building  shs from hoistwayInfo
	FLOOR_T	nUserFloor;				// number of floor can be accessed by passengers
};
// shs: change some names of below class
struct HPHoistwayInfo					// Static hoistway information corresponding to each hoistway
{
	bool	pit;						// true means pit exists
	bool	overhead;					// true means overhead exists
//	FLOOR_T	lowerSeparationTable[MAX_FLOOR];	// separation table for lower car
//	FLOOR_T	upperSeparationTable[MAX_FLOOR];	// separation table for upper car
	int		nCars;			// number of cars in this hoistway
	char	lowerCarID;			// index of lower car in the hoistway, -1 represent no lower car
	char	upperCarID;			// index of upper car in the hoistway, -1 represent no upper car
};

// StaticHoistwayInfo hoistway_info[MAX_HOISTWAY];		// to be delared as global variable somewhere

struct HPCarInfo						// static information corresponding to each car
{
	double	maxJerk;							// maximum jerk value
	double	maxAcceleration;							// maximum acceleration rate
	double	maxVelocity;						// maximum speed of the car
	double	safetyDeceleration;					// safety deceleration 
	double	startDelay;		// start delay for a car
	double	stopDelay;		// stop delay for a car
	double	doorOpeningTime;				// door opening time in seconds
	double	doorClosingTime;				// door closing time in seconds
	double	boardingRatio;
	short	capacity; // persons
	short	nDoors;
	double	doorWidth;
	unsigned char callMask[MAX_FLOOR];  //	shs change [car][floor] =>[floor]
	// 2-D related
	char	hoistwayID;					// index of hoistway this car belongs
	int upperOrLower;					// upper or lower car
	int otherCarID;						// ID of other car

	// unsigned char call_mask;	
	// CSW: ask about the meaning of call_mask	
};
// StaticCarInfo car_info[MAX_CAR];			// to be delared as global variable somewhere

struct HPParameter						// Global parameters, to be updated as the code developed		
{
	T_TIME t_car_min_dwell;
	T_TIME t_hall_min_dwell;
	T_TIME t_hall_max_dwell;
	T_TIME t_lobby_dwell;
	T_TIME boardingDeboardingTime;
	//unsigned int AllowMask[MAX_CAR][MAX_FLOOR]; no use
	// UINT	numberOfInformation; //suspicious to include it
	bool bReopenEnabled;					// true means reopen is enabled
	double A1S,A2S,A1W,A2W,W,V; // parameter for scoring
	FLOOR_T	initialFloor[MAX_CAR];
};

// shs : NO global data would be managed between various component.(HP,HE,DD,Car)
//		each component has its own managed data
/**************************************************************************************************
 * These data structures are global information that can only be updated by the hoistway plan
 * executer. These are dynamic data that will change over time and shoule be globally accessed.
 * Various modules can read from these data structure instead of passing information back and forth
 **************************************************************************************************/

struct AssignedDestinationEntryCall		// the destination entry that can be accessed by all modules
{
	FLOOR_T origin;				// the origin floor of the call
	FLOOR_T destination;			// the destination floor of the call
	PLAN_INDEX_T next;					// link to the next call
	unsigned short passengers;		// number of passengers associated with this entry call, use short for easy process
	char status;					// waiting/boarded
//	char assignment;				// which car the called is assigned to, -1 means unsigned
									// shs deleted, assignment information is in the index
	T_TIME reg_time;				// The time when this call is registered, in milli seconds
};


struct AssignedDestinationEntryCalls		// structure to keep track of every call from registered being served
{
	PLAN_INDEX_T	empty_list;			// index to first available global destinationEntry
	//PLAN_INDEX_T	deboardedIndex;
	PLAN_INDEX_T	boardedIndex;	// index to the first entry call already picked up by the car for each car
	PLAN_INDEX_T	waitingIndex;   // index to the first entry call assigned to a car, but not yet picked up
//	unsigned short	unassigned_list;	// index to the first unassigned call
	AssignedDestinationEntryCall calls[MAX_CALL];	// memory space for storing all global destination entry
};

// AssignedDestinationEntryCalls	assignedCalls[MAX_CAR];

// structure for car status
struct HPCarStatus
{
	double currentTime;
	int	CarID;
	int currentNCF;
	int movingDirection;
	int currentOperation;
	int currentLoad; // persons boarded TBD
};

struct PlannerSpec
{
	HPBuildingInfo BuildingInfo;
	HPHoistwayInfo HoistwayInfo[MAX_HOISTWAY];
	HPCarInfo	CarInfo[MAX_CAR];
	HPParameter	Param;
};

// CSW051112: Added for storing parking information
struct ParkingInfo
{
	FLOOR_T lowParkingRange;
	FLOOR_T highParkingRange; 
	FLOOR_T parkingLandingIndex; 
	TIME_T delay;
	SHORT option;

};

#endif

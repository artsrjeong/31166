#ifndef	_2D_INTERFACE_DATA_
#define	_2D_INTERFACE_DATA_

#include "..\common\all.h"			//CSW051213: changed from COGS to common


// shs0708 added for hard coded file loading for hoistway manager
#define COGS_FILE				"2DSim.EXE"
#define GROUP_DISPATCHER_FILE	"Dispatcher.DLL"
// CSW051213: Combine HP and HPE into IHC.DLL
// #define	HOISTWAY_PLANNER_FILE	"HoistwayPlanner.DLL"		
// #define	HOISTWAY_EXECUTOR_FILE	"HExec.DLL"
#define IHC_FILE				"IHC.DLL"
#define	NO_DISPATCHER	"NO_DISPATCHER"

#define MAX_CAR 16			// maximum number of cars
#define MAX_HOISTWAY 9		// maximum number of hoistways

// Define the limits that affect memory requirement for various data structures
#define MAX_FLOOR 128				// max number of floors in a building
#define MAX_CALL  512				// max number of destination entry calls a plan can have //From the GD SRS
#define MAX_STOP  128				// max number of stops in a stop sequence
#define INVALID_FLOOR 255

// Define types for hoistway manager
typedef LONG	TIME_T;					// in milliseconds
typedef UCHAR	FLOOR_T;				// floor index type, 255 represents no floor
typedef SHORT	PLAN_INDEX_T;			// index to use in hoistway plan as pointers
typedef SHORT	CALL_INDEX_T;			// index for calls
typedef SHORT	PASSENGER_CAPACITY_T;	// type for passenger units in 1/10 of a passenger
typedef	ULONG	HPSCORE_T;				// type for hoistway plan score

// operation mode from the infocs.h of NPI
#define         NAV		0       /* Not available		*/
#define         EPC		1       /* Emergency power correction	*/
#define         COR		2       /* Correction			*/
#define         EFS		3       /* SES phase II			*/
#define         EFO		4       /* SES phase I			*/
#define         EQO		5       /* Earthquake operation		*/
#define         EPR		6       /* Emergency power rescue	*/
#define         EPW		7       /* Emergency power wait for NOR */
#define         OLD		8       /* Overload			*/
#define         ISC		9       /* Independent service		*/
#define         ATT		10      /* Attedent service		*/
#define         DTC		11      /* Door time close protection	*/
#define         DTO		12      /* Door time open protection	*/
#define         CTL		13      /* Car to landing		*/
#define         CHC		14      /* Cutoff hall calls		*/
#define         LNS		15      /* Load non-stop		*/
#define         MIT		16      /* Moderate incoming traffic	*/
#define         DCP		17      /* Delayed car			*/
#define         ANS		18      /* Anti-nuisance		*/
#define         NOR		19      /* Normal			*/
#define         ARD		20      /* Automatic return		*/
#define         PRK		21      /* Park				*/
#define         IDL		22      /* Idle				*/
#define         PKS		23      /* Park switch			*/
#define         GCB		24      /* General control of buttons	*/
#define         EHS		25      /* Emergency hospital service	*/
#define         ROT		26      /* Riot				*/
#define         INI		27      /* Initialization		*/
#define         INS		28      /* Inspection			*/
#define         ESB		29      /* Emergency stop button	*/
#define         DHB		30      /* Door hold button		*/
#define         ACP		31      /* Anti-crime protection	*/
#define         WCO		32      /* Wild car protection		*/
#define         DBF		33      /* Drive brake failure		*/
#define         SAB		34      /* Shabat service		*/
#define         EFP		35      /* SES power interuption 	*/
#define         CRL		36      /* Secure than shut down	*/
#define         CRO		37      /* Card reader operaiton	*/
#define         CES		38      /* Car call express service	*/
#define         DOS		39      /* Door open hold switch	*/
#define         WCS		40      /* Wheel chair service		*/
#define         REC		41      /* Recover operation		*/
#define         OHT		42      /* Overheat operations		*/
#define         ARE		43      /* Automatic rescue operation	*/
#define		EPD		44	/* Manual Emergency power wait  */
#define     GAP     45      /* Motor column abnormal    */
#define     HBP     46      /* Hall button protection   */
#define		OOS		47	/* Out of Service		*/
#define		SCX		48	/* Shuttle car operation	*/
#define		EMT		49	/* Emergency med. car retrevial */
#define		EMK		50	/* Emergency med. car control   */
#define		EPT		51	/* Emergency power transfer     */
#define		SCO		52	/* Swing car operation		*/
#define     CHN     64      /* Channelling mode         */
#define		EPO		65	/* Emergency power operation	*/
#define		ERO		66	/* Emergency rescue operation   */
#define		CBP		67	/* Car button protection	*/
#define		DDO		68	/* Disable door open		*/

// Add New Operation    13May2003
// Especially for Revo,Eco..
#define         PFO             70      /* Pit Flood operation          */
#define         CDO             71      /* Cooler Drain operation       */
#define         SRO             72      /* Separate Riser Operation     */
#define         DLF             73      /* Door Lock Failure            */
#define         LPS             74      /* Low Oil Pressure             */
#define         STP             75      /* Car Stall Protection         */
#define         RCY             76      /* Telescopic Cylinder Recycle  */
#define         REI             77      /* Remote Elevator Inspection   */
#define         KEH             78      /* Kobe Emergency Hospital      */
#define         LOO             79      /* Low Oil operation      */
#define         SRF             80      /* Slacken Rope Function        */
#define         BKF             81      /* Brake Function               */
#define         CAO             82      /* Cylinder Adjustable operation*/
#define         ACO             83      /* Anti Crime operation         */
#define         EPS             84      /* Express Priority Service     */
#define         WRM             85      /* Warm operation               */
#define         OIC             86      /* Outside Inspection Car       */
#define         KIS             87      /* Kobe Independent Service     */
#define         KNO             88      /* Kobe No Operation            */
#define         ARS             89      /* Automatic Rescue Service    */
#define         ARO            103      /* Automatic Rescue Operation  */
#define         DLM            104      /* Door Lock Monitoring        */
#define         EAR            105      /* Emergency Automatic Rescue  */
#define         HAD            106      /* Hoistway Access Detection   */
#define         DAR            107      /* Drive Fault with Automatic Run */
#define         DCS            108      /* Door Check Sequence         */
#define         UFS            109      /* Up Final Limit Switch       */
#define			ONE_UP_ONE_DOWN	110		/* One up One down policy -SeongRak */
#define		NUL		127	/* Null opmode, don't care.     */

/**************************************************************************************************
 * These data structures are static information that should be initialized. Various modules may need
 * the same information. 
 **************************************************************************************************/
struct BuildingInfo				// static building information 
{
	UCHAR	nCars;				// total number of cars in a group
	UCHAR	nHoistways;			// total number of hoistway in a group
	FLOOR_T	lobby1Floor;		// floor index of lobby 1
	FLOOR_T	lobby2Floor;		// floor index of lobby 2
	FLOOR_T	lobby3Floor;		// floor index of lobby 3
	FLOOR_T	rest1Floor;			// floor index of restaurant 1
	FLOOR_T	rest2Floor;			// floor index of restaurant 2
	FLOOR_T	EZ1Top;				// floor index of express zone 1 top
	FLOOR_T	EZ1Bottom;			// floor index of express zone 1 bottom
	FLOOR_T	EZ2Top;				// floor index of express zone 2 top
	FLOOR_T	EZ2Bottom;			// floor index of express zone 2 bottom
	FLOOR_T	EZ3Top;				// floor index of express zone 3 top
	FLOOR_T	EZ3Bottom;			// floor index of express zone 3 bottom
	ULONG	landingPosition[MAX_FLOOR];	// floor position for each floor, in millimeters
	FLOOR_T	nLandings;					// number of landings, including pit, overhead, virtual stops in express zone
	int	f_offset;		// floor offset for logical floor to numerical floor(pit exist then -1, else 0)
} ;

struct HoistwayInfo				// static hoistway information per hoistway
{
	CHAR	idHwy;				// hoistway ID
	BOOL	bPit;				// true means pit exists
	BOOL	bOverhead;			// true means overhead exists
//	FLOOR_T	lowerSeparationTable[MAX_FLOOR];	// separation table for lower car
//	FLOOR_T	upperSeparationTable[MAX_FLOOR];	// separation table for upper car
	UCHAR	nCars;				// number of cars in this hoistway
	CHAR	idLowerCar;			// index of lower car in the hoistway, -1 represent no lower car
	CHAR	idUpperCar;			// index of upper car in the hoistway, -1 represent no upper car
} ;
// e.g., HoistwayInfo hoistway_info[MAX_HOISTWAY];		

 struct CarInfo						// static information per car
{
	USHORT	Afd;					// forced deceleration by modeling of motion properties
									// for calculating emergency stop position(ESP)
	USHORT	maxJerk;				// maximum jerk value in mm/s/s/s
	USHORT	maxAcceleration;		// maximum acceleration rate in mm/s/s
	USHORT	maxVelocity;			// maximum speed of the car in mm/s
	TIME_T	tDoorOpening;			// door opening time in milliseconds
	TIME_T	tDoorClosing;			// door closing time in milliseconds
	USHORT	boardingRatio;			// percentage [CSW: currently not used]
	USHORT	capacity;				// max number of passengers the car can carry
	UCHAR	nDoors;					// number of doors
	USHORT	doorWidth;				// door width in mm
	UCHAR	hoistwayID;				// index of hoistway this car belongs
	USHORT	callMask[MAX_FLOOR];	// floor mask per floor for each car
	/* Definition of each bit:
	 * Bit 0 : Front car call
	 * Bit 1 : Front up hall call
	 * Bit 2 : Front down hall call
	 * Bit 3 : Front emergency hall call
	 * Bit 4 : Rear car call
	 * Bit 5 : Rear up hall call
	 * Bit 6 : Rear down hall call
	 * Bit 7 : Rear emergency hall call
	 * Bit 8 : Extended front car call
	 * Bit 9 : Extended front up hall call
	 * Bit 10 : Extended front down hall call
	 * Bit 11 : Extended front emergency hall call
	 * Bit 12 : Extended rear car call
	 * Bit 13 : Extended rear up hall call
	 * Bit 14 : Extended rear down hall call
	 * Bit 15 : Extended rear emergency hall call
     */
	TIME_T	tStartDelay;
	TIME_T	tStopDelay;
} ;
// e.g., CarInfo car_info[MAX_CAR];			

struct Parameter						// static global parameters		
{
	TIME_T tCarMinDwell;				
	TIME_T tHallMinDwell;
	TIME_T tHallMaxDwell;
	TIME_T tLobbyDwell;
	TIME_T tBoardingDeboardingTime;
	BOOL bReopenEnabled;				// true means reopen is enabled
	ULONG A1S,A2S,A1W,A2W,W,V;			// parameter for scoring
	CHAR GDAlgorithmID;					// algorithm ID for group dispatching
	ULONG MinSeparationDist;		// Minimum separation distance SHS
	BOOL bDwellTimeExtendACS; //dwell time extend to avoid conditional stop
	BOOL bRunDelayACS; // run delay to avoid conditional stop
	BOOL bReducedProfile; // use reduce profile

// To be implemented in User Configuration dialog box and 2dp file:
	BOOL bParkingEnabled;				// iff (idle car) parking is enabled
};

/**************************************************************************************************
 * These are the data that should be updated dynamically
 **************************************************************************************************/

/* need to define call type here
   Check the definition in NPI */

struct Call							// description of each destination entry call
{
public:
	FLOOR_T origin;					// origin floor of the call
	FLOOR_T destination;			// destination floor of the call
 	UCHAR nPassengers;				// number of passengers associated with this entry call
	// Only 1 byte is needed.
	// Front and rear, emergency and normal, extended and standard.
	// Probably, 8bits are needed. 
	// front Normal, front emergency, rear normal, rear emergency,
	// extended front normal, extended front emergency, extended rear normal, extended rear emergency
	UCHAR callType;
	// registered time is moved from CallListEntry to here shs0712
	TIME_T ctRegistration;			// clock time when this call is registered, in milliseconds
};

struct CallListEntry:public Call    // entry that will be used in CallList
{
	CALL_INDEX_T next;				// link to the next call in CallList
	CALL_INDEX_T prev;				// link to the previous call in CallList
	// shs0712 delete status and ctRegistration from CallListEntry
//	UCHAR status;					// waiting/boarded/deboarded
//	TIME_T ctRegistration;			// clock time when this call is registered, in milliseconds
};

struct CallList							// tracks every call (both hall and car) being served
{
	CALL_INDEX_T	empty_list;			// index to first available call list entry
	CallListEntry   calls[MAX_CALL];	// memory space for storing all call list entries
	CALL_INDEX_T	boardedIndex;		// first entry in list of call already picked up by the car
	UCHAR			nBoardedList;		// number of passengers in car (number of entries in boardedIndex list)
//	CALL_INDEX_T	waitingIndex;		// first entry in list of call assigned to a car but not yet picked up
	CALL_INDEX_T	waitingAtHead[MAX_FLOOR];  // first entry in list of calls waiting at given floor
	CALL_INDEX_T	waitingAtTail[MAX_FLOOR];  // last entry in list of calls waiting at given floor
	CALL_INDEX_T	expectedTransferHead;	// first entry in list of calls whose hall call has been
									// cancelled but has yet to have its car call accepted
};
// e.g., CallList AssignedCalls[MAX_CAR];    // calls assigned to each car

// Door State (New)-Change name DOORS_XXX to DOOR_XXX for better naming (shs0708)
// revive the DOORS_XXX by discussion (shs0711)
#define     DOORS_CLOSED    0       // door state = closed          
#define     DOORS_CLOSING   1       // door state = closing         
#define     DOORS_OPENED    2       // door state = opened          
#define     DOORS_OPENING   3       // door state = opening			


// motion state types
#define STANDING 0
#define MOVING 1  // jsr 2005,07,07 originally it was RUNNING but it's same as COGS macro
// shin050812
// new motion state types
//#define STOPPED 0
//#define ACCEL 1
//#define DECEL 2
//#define FULL_SPEED 3

/* TO BE DELETED
#define IDLE 0
#define	RUNNING 4
#define STANDING 5
#define NOT_AVAILABLE 6 
*/

// door cycle types
#define DOOR_NOT_OPEN	0
#define DOOR_NORMAL		1
#define DOOR_HURRIED	2
#define DOOR_CLOSE_CUT	3
#define DOOR_DELAYED	4

// timing related definition
#define	ARRIVAL_TIMING	0
#define	DEPARTURE_TIMING	1
/*
  Use standard list for operation mode in infocs.h
*/

// structure for car state
struct CarState
{
	TIME_T ctCurrent;		// clock time when the state is recorded
	FLOOR_T currentNCF;		// current NCF of the car
	CHAR movingDirection;	// the direction the car is moving
	CHAR preDirection;       // NPI need this value
	UCHAR nPassengers;		// number of passengers on board 
	UCHAR OPMode;			// operation mode
	UCHAR motionState;		
	UCHAR frontDoorState;
	UCHAR rearDoorState;
	TIME_T tStartOper;		// start time of current operation
	FLOOR_T target;			// target floor of the car, if the car is moving
	FLOOR_T currentFloor;	// current floor for every event
	FLOOR_T	origin;
	BOOL decelDecided; // TRUE when deceded to stop FALSE after stop
	UCHAR	nBoard;		// boarding people
	UCHAR	nDeboard;	// deboarding people
};

struct CarCommand
{
	UCHAR motionCommand;	// standing vs running	
	FLOOR_T target;			// target floor for the run
	TIME_T runDelay;		// delay before start of the run
	UCHAR runProfile;		// index of profile to use for the run
	UCHAR frontDoorCommand;	// front door command type
	TIME_T frontDoorDwell;	// front door dwell time
	UCHAR rearDoorCommand;	// rear door command type
	TIME_T rearDoorDwell;	// rear door dwell time
	SHORT pre_dir;          // predirection for COGS to board passenger
};


#include "..\HoistwayPlanner\Hoistway_plan.h"  // shs0711
#define    OK          1	// Add error codes.
#define		ERR			0x80	// error mask
#define		ERR_FULL	0x81  // shs add error code for DB
#endif

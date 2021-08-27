// 2DSIM_DEFS.H
// COGS COMMON STRUCTURE & CONSTANT DEFINITION FILE
#ifndef	_2DSIM_DEFS_H
#define	_2DSIM_DEFS_H

#define ENGINEVERSION "060119"
#define	VERSION_SUMMARY _T("Prototype 1")
#define	VERSION_DETAIL _T("Timing fixed with TSA when leading car running")

#ifdef	NDEBUG
#define	EXECUTABLE_TYPE	"Release Executable"
#else
#define	EXECUTABLE_TYPE	"Debug Executable"
#endif

#define MAX_NUMBER_TRAFFIC_FILE 25
// shs0708 define Time and Speed scale for making int from floating time
#define INT_TIME_SCALE 1000   // multiplay this value with time parameter to get integer time
							// 1000 means, time unit is 1/1000 second
#define	INT_SPEED_SCALE 1000	// multiply this value with speed parameter to get integer speed value
							// 1000 means, speed unit is 1/100. ie(1 means 0.001 m/s)
#define	INT_DIST_SCALE	1000  // multiply this value to the float or double variable to get scaled integer

// shin051025 To calculate more precisely.
// Consider "Improve Float Consistency" in the Optimazations Category of C/C++ project setting.
//#define	MAKE_FLOAT_TIME(x)	(float(x)/INT_TIME_SCALE)
#define	MAKE_FLOAT_TIME(x)	(float)(((double)(x))/((double)(INT_TIME_SCALE)))
//#define	MAKE_FLOAT_SPEED(x)	(float(x)/INT_SPEED_SCALE)
#define	MAKE_FLOAT_SPEED(x)	(float)(((double)(x))/((double)(INT_SPEED_SCALE)))
#define MAXPAX     2000 // from 1000 //from 2000
#define MAXMOVE    1000
#define MAXHWY     16
#define MAXCAR     16
#define MAXFLOOR   140
#define MAXCARSIZE 50
#define MAXCARLOG  2000
#define MAXREC     200
#define MAXBREAKPOINT     20
#define MAXSPECIALEVENT	100


#define UP				1
#define DN				-1
#define NO_DIR			0
#define NO_PREDIRECTION	-2
#define MAXDIR				2
#define UP_INDEX			0
#define DN_INDEX			1
#define INVALIDFLOOR	255
#define EPSILON			1.0E-8
#define PRECISION		1.0E-5  // precision for GetTraversalTime computation

#define NUM_BREAK_MAX_VELOCITY		8
#define NUM_BREAK_MAX_ACCELERATION	7
#define NUM_BREAK_NEITHER			5

#define UPPER_CAR 1
#define LOWER_CAR 0

#define START_DELAY_PERIOD -1
#define STOP_DELAY_PERIOD -2

#define VIOLATION_NULL	0X01
#define VIOLATION_H		0X02
#define VIOLATION_C		0X04
#define VIOLATION_D		0X08
#define VIOLATION_HC	0X10
#define VIOLATION_HD	0X20
#define VIOLATION_CD	0X40
#define VIOLATION_HCD	0X80

#define NULL_MASK	0x01
#define H_MASK		0x02
#define C_MASK		0x04
#define D_MASK		0x08
#define HC_MASK		0x10
#define CD_MASK		0x20
#define HD_MASK		0x40


#define    WINDOWCOLOR  RGB( 50, 50,100)   //RGB(  0,  0,150)
#define    DARKGREEN    RGB(  0,128,  0)
#define    GREEN        RGB(100,200,100)
#define    LIGHTGREEN   RGB(200,255,200)

#define    DARKRED      RGB(230,  0,  0)
#define    RED          RGB(255,100,100)
#define    LIGHTRED     RGB(255,180,180)
#define    LIGHTLRED    RGB(255,200,255)
#define    LIGHTMAGENTA RGB(255,100,180)

#define    PINK         RGB(255,150,255)

#define    DARKBLUE     RGB(  0,  0,160)
#define    BLUE         RGB(100,100,255)
#define    LIGHTBLUE    RGB(100,255,255)

#define    YELLOW       RGB(255,255,128)
#define    LIGHTYELLOW  RGB(255,255,200)
#define    ORANGE       RGB(255,200,150)

#define    DARKPURPLE   RGB(128,128,192)
#define    PURPLE       RGB(190, 50,255)
#define    LIGHTPURPLE  RGB(220,220,255)
#define    PURPLE2      RGB(168,  0,168)

#define    BLACK        RGB(  0,  0,  0)

#define    DARKGRAY     RGB( 90, 90, 90)
#define    DDARKGRAY    RGB( 60, 60, 60)
#define    GRAY         RGB(192,192,192)
#define    MASK_COLOR   RGB( 90, 90, 90)
#define    LIGHTGRAY    RGB(210,210,210)

#define    DARKGRAYBLUE  RGB( 50, 50,100)
#define    LIGHTGRAYBLUE RGB(190,190,220)

#define    BRIGHTWHITE  RGB(250,250,250)
#define    WHITE        RGB(255,255,255)

#define    BROWN        RGB(180,  0,  0)

#define    LOBBYRED     RGB(255,200,200)
#define    CAFEBLUE     RGB(200,200,255)

#define    UPCOLOR      RGB(190,255,190)
#define    DNCOLOR      RGB(255,190,190)
#define    BSCOLOR      RGB(190,190,255)

// current car operation
#define	IDLE 0
#define	OPENING 1
#define	DWELL 2
#define	CLOSING 3
#define	RUNNING 4
#define	FORCED_MOVE 5
#define	FORCED_PARKING 6
#define	END_OF_FORCED_PARKING 7
#define WAIT_INITIATE 8
#define WAIT_RELEASE 9
#define	WAIT_CAR_RANGE 10
#define	MOVE_CAR_RANGE 11
#define V_CALL_STOP 12

#define	DISPATCH_NEW_ASSIGN	1
#define	DISPATCH_NO_ASSIGN	0

// MOVE command Type definition
#define	MOVE_NORMAL	0  //' NO ACTION ONLY REFERENCE
#define	MOVE_ACCEPT	1
#define	MOVE_REJECT	2
#define	MOVE_ADD	3
#define	MOVE_DELETE	4

// CALL TYPE definition
#define	DE_NORMAL_CALL		1	// NORMAL DESTINATION ENTRY CALL
#define	DE_HANDICAP_CALL	2	// DESTINATION ENTRY CALL BY HANDICAPPED PERSON

// Passenger command/status definition
#define	PAX_ERROR		-1
#define	PAX_NORMAL		0
#define	PAX_ASSIGN		1
#define	PAX_DEASSIGN	2	// TBD
#define	PAX_REASSIGN	3	// TBD
#define	PAX_DE_ENTERED	8	// FROM COGS TO GET ASSIGNMENTS

#define	MSGID_SPECIALTIME	0
#define	MSGLEN_SPECIALTIME	2	// high time, low time (x10)
#define	MSGID_CARRANGE	1
#define	MSGLEN_CARRANGE	 3		// car id(4bit MSB)/door(4bit LSB), bottom, high
#define	MSGID_CALLSERVED 2
#define	MSGLEN_CALLSERVED	3	// car id(4bit) 
							// 4bit(FCC,FHU,FHD,FEHS),4bit(RCC,RHU,RHD,REHS)
							// Floor (8bit)

struct SYSTEM{
  double t_nextEvent;
  int y_nextEvent;
  int i_nextEvent;
};

struct PARAMETER{
	int initial_position[MAXCAR];
	double door_open, door_close;
	int capacity;
	//BOOL pit;           // =1 if there is a pit; 0 otherwise
	//BOOL overhead;      // =1 if there ia an overhead floor; 0 otherwise
	int f_offset;       // index of lowest floor (which can be negative if there is a pit)
	//int n_user_floor;   // number of user floors
	double t_dwell[MAXCARSIZE][MAXCARSIZE];
	//MaxJerk, MaxAcceleration, MaxVelocity, StartDelay and StopDelay apply to every car
	double MaxJerk;			//	m/s^3	
	double MaxAcceleration;	//	m/s^2
	double MaxVelocity;		//	m/s
	double StartDelay;		//	s
	double StopDelay;			//	s
	double MinSeparationDist;	//  meter unit minimum separation distance
	
	int AdjacentLimit;  // adjacent floor concept
	double t_car_min_dwell;
	double t_hall_min_dwell;
	double t_hall_max_dwell;
	double t_lobby_dwell;
	int bReopenEnabled; // reopen enabled?
	double additionalStopTime; // additional stop time
	double boardingRatio;	// boarding ratio(%)
	double boardingDeboardingTime; // boarding + deboarding time per person
	int bMotorGen; // Motor Generator?
	int transferTime;
	int transferFloorTime;
	unsigned int AllowMask[MAXCAR][MAXFLOOR];
	unsigned int DestinationEntryInstalled[MAXFLOOR];
	int simulationType;
	int buildingPopulation;
	UINT numOfInformation;	//To check the violation for n pieces of information
	// dll related parameters
	char dispatcherDLLPath[150];
	char dispatcherDLLName[80];
	int	bAutomaticDispatching;
	char   dispatchingAlgorithmName[80];
	int	algorithmID;
	int duty;
	int dutyID;
	int m_violationHCD;
	BOOL bDwellTimeExtendACS; //dwell time extend to avoid conditional stop
	BOOL bRunDelayACS; // run delay to avoid conditional stop
	BOOL bReducedProfile; // use reduce profile
};

struct PASSENGER{
  double t_arrival;
  int f_origin, f_dest;
  int assignment;
  double t_endwait_AP, t_endwait_CP, t_door_opening, t_door_opened, t_door_close, t_departure;
  int new_registration_direction;      // traditional definition of a new hall call registration:
      // when there is no other passenger on floor registering call in same direction
  int new_registration_car_direction;  // destination entry definition: when there is no other
      // passenger assigned to the same car in the same direction
  int reregister;  // traditional definition of re-registration
  double t_reregister;  // time when re-registration occurred
  int q_next;
  int kindFromTo;  // 16 or 1 (not used by simulator, but copied to new PRO file)
  int weight;      // (not used by simulator, but copied to new PRO file)
  int violation;	// amount of information to passenger, which makes voilation
  BYTE rejection;	//rejection status for each passenger according to the amount of information
};

struct MOVE_PARAMETER
{
  int car,                    // index of car to be moved
	  destination,            // floor index
	  initiation_basis,       // 0=time-based; otherwise, condition-based
	  release_basis,
	  initiation_relativity,  // -2=if other car is at position strictly lower than the threshold
	  release_relativity,     //   position; -1=if other car is at or below threshold; 1=if other car
	                          //   is at or above threshold; 2=if other car is strictly above threshold
	  initiation_direction,   // 1=up, 0=either direction, -1=down
	  release_direction;
  double min_dwell,           // minimum door dwell time if door is opened
	  t_initiation,           // earliest initiation time
	  t_release,              // release time (if time-based)
	  initiation_threshold,   // threshold distance (in m) for condition-based initiation
	  release_threshold,
	  initiation_delay,       // additional delay in condition-based initiation
	  release_delay;
  int	cmd;	// 0: normal, 1:Added, 2:Rejected, 3:Deleted
};

struct HOISTWAY{
  int n_car_in_hwy;  // number of cars in hoistway (1 or 2)
  int i_car;         // index to first car in hoistway
};

struct CAR_STATE{
  int i_hwy;          // hoistway index
  int lowerOrUpper;		// Lower car: 0(LOWER_CAR) Upper Car: 1(UPPER_CAR)
  int oper;           // current operation: 0=idle, 1=door opening, 2=dwell, 3=door closing,
                      //   4=running, 5=forced move, 6=forced parking, 7=temporarily after end
                      //   of forced parking, 8=waiting for move initiation, 9=waiting for move
                      //   release
  double t_start_oper;	// time to start this operation shs0826
  double t_end_oper;  // time to end of operation
  int floor, target;
  int currentFloor; // current floor for every event
  //jsr
  double currentPos;
  int NCF;
  /////////////////////////////////////////////
//double currentPos, targetPos;
//int commitFloor;
  int dir;            // 1=up, -1=down, 0=no dir
  int pending_move;   // 0=no, 1=yes, 2=yes -- at a new hall call, this code forces a move to be
                      // processed before UpdateTarget can be called
  int other_car_waiting;  // 0=no, 1=yes
  int i_move;         // index of move for oper=5,6,8,9
  int n_pax_in_car;     // total number of passengers in the car
  int n_CCall[MAXFLOOR], q_CCall[MAXFLOOR];                        // car calls - pax on car
  int n_UpStop[MAXFLOOR], n_DnStop[MAXFLOOR];                      // future car calls
  int n_UpCall[MAXFLOOR], q_UpCall[MAXFLOOR], z_UpCall[MAXFLOOR];  // up hall calls
  int n_DnCall[MAXFLOOR], q_DnCall[MAXFLOOR], z_DnCall[MAXFLOOR];  // down hall calls
  //int n_VCall[MAXFLOOR];  // hide/unhide calls
  int mp_n_break;					// number of breakpoints
  double mp_break[MAXBREAKPOINT];	// time of breakpoint
  double mp_jerk[MAXBREAKPOINT];	// jerk value between breakpoint and next one
  double mp_accel[MAXBREAKPOINT];	// acceleration value at breakpoint
  double mp_speed[MAXBREAKPOINT];	// speed value at breakpoint
  double mp_pos[MAXBREAKPOINT];		// position value at breakpoint
  int range_bottom;					// floor index of bottom of car range
  int range_top;					// floor index of top of car range
  double max_decel_bottom;			// position of bottom of range where car can move at full decel
  double max_decel_top;				// position of top of range where car can move at full decel
// Recorded status:
  double t_run_start;    // starting time of run
  double t_decel;        // time at which car decelled into current floor
  double t_actual_position;  // time at which car reaches actual position of target
  double t_run_end;      // ending time of run
  int origin;            // origin floor
  int n_pax_run;		 // number of passenger during the run
  int door_cycle_in_car_cycle;  // if doors were opened during the car cycle
  double t_door_opened;  // time when door was fully opened
  int n_deboard;         // save number of deboards so dwell time can be computed
  int n_board;
  double t_door_dwelled;
  int n_run_normal;      // number of normal runs
  int n_run_hide;        // number of hide and actual unhide moves
  int n_cycle_door;      // number of door cycles
  char best_responder;   // indicates best car to respond to call
//double t_to_respond;   // time until car arrives and boards pax to answer call
  int dwell_floor;       // store data for next door dwell move
  double dwell_initiate, dwell_release;
  int extra_dwell;       // if a special move to hold the doors longer has been used during the run
  int changed_target;    // if original target of run was changed during the run (0=normal demand,
						 //   1=changed target during run,2=car range run only,3=mixed run ending
						 //   in demand target, 4=mixed run ending in car range run)
//double t_next_hide;    // time of next hiding move of car
  double tRunDelay;		 // delay before start of the run
};

struct BREAK_TABLE{
	int num;
	double time[MAXBREAKPOINT];
	double jerk[MAXBREAKPOINT];
	double accel[MAXBREAKPOINT];
	double velocity[MAXBREAKPOINT];
	double position[MAXBREAKPOINT];
};

struct CAR_LOG{
  int car;
  char type;
  double t_run_start, t_run_end, t_decel, t_actual_position; 
  int origin, dest, n_pax_run;
  int door_cycle_in_car_cycle;
  double t_door_opened, t_door_dwelled, t_door_closed;
  int n_deboard, n_board;
  double t_released;
  int extra_dwell;
  int changed_target;
};

struct FLOOR_STATE{
  int n_arrival, n_departure;
  int length, max_length;
  double t_break, length1, length2;
};

struct	BUILDING_STRUCTURE
{
	char mBuildingName[100];  // Building Name
	// Floor index and count strategy
	// Index begin from pit (0)
	// Total Floor includes pit/overhead
	// ex) if service floor is 2 and pit exist ==> Total floor =3
	//	   if service floor is 3 and pit and overhead ==> 5
	int nUserFloors;	// floors of user (exclude count of pit/overhead)
	int nTotalFloors; // userfloor+pit+overhead
	BOOL pit,overhead;	// 1  if exist, 0 if not.
	int lobby1Floor;	// User floor index of lobby floor
	int lobby2Floor;	// Lobby Floor 2
	int lobby3Floor;	// Lobby Floor 3
	int rest1Floor;	// Restaurant Floor 1
	int rest2Floor;	// Restaurant floor 2

	char floorLabel[MAXFLOOR][50]; // Floor Label
	double interfloorHeight[MAXFLOOR]; // inter floor height ie. 4(m)
	double floorPosition[MAXFLOOR]; // floor position ie. floorPosition[7] = 28 (m)
	int	floorPopulation[MAXFLOOR];

	int expressZoneBottom;
	int expressZoneTop;

	int nCars;  // No of Cars
	int nHoistways; // No of Hoistways
	// int cars;
	// int hoistways;

	int doorWidth;
	int doorType;
	int nDoors;
};

// FOR CALLING DISPATCHER DLL


struct PASSENGER_CALL
{
	int originFloor; // car send
	int destFloor;   // car send
	int	assignment;	// dispatcher send
	unsigned int callType; // Not used, Call type(Handicapped call.../Rear)
	int nPassengers; // number of passengers
	int passengerID; // passenger id
	int cmd; // 0: not assigned,1: Assign, 2: Deassign, 3: Reassign
}; // call

struct PASSENGER_STATUS
{
	int nCurrentCalls; // no of passenger calls
	PASSENGER_CALL passengerCall[MAXPAX];
};


struct MOTIONPROFILE_PARAMETER
{
	int carID;
	int nBreakPoints;
  	double mp_break[MAXBREAKPOINT];	// time of breakpoint
  	double mp_jerk[MAXBREAKPOINT];	// jerk value between breakpoint and next one
	int status; // 0: command, 1: reject, 2 accept
}; // parameter for motion profile

struct MOTIONPROFILE_COMMANDS
{
	int nCmds;  // number of motion parameter commands
	MOTIONPROFILE_PARAMETER motion[MAXCAR]; // motion specifier
};

#define	SIZE_EXTRAMSG	200

// command and status for reserved, series of commands or status
struct EXTRA_MSGS
{
	int nBytes;
	char cmdMsg[SIZE_EXTRAMSG];  // ID,bytes,Data,ID,bytes,Data... in order
}; 

struct MOVE_COMMANDS
{
	int nMoves;	// car send, dispatcher send
	int iMove; // current move processing, car send
	MOVE_PARAMETER Move[MAXMOVE]; // car send, dispatcher send
};	// commands for moves

extern "C" {
typedef	void (*DDGetInfo)(char info[12][50]);
typedef int (*DDInit)(BUILDING_STRUCTURE *building , 
			CAR_STATE cars[MAXCAR], PARAMETER *simul, int algorithm_id );
typedef int (*DDUpdate)(double currentTime, // for sync
		CAR_STATE cars[MAXCAR],	  // Car Status(didn't update)
		PASSENGER_STATUS *passengers,
									  // in this structure assignment will be transfered from dispatcher
		MOVE_COMMANDS *moveCommands, // move command from dispatcher
		MOTIONPROFILE_COMMANDS *motionCommands, // profile commands for maximum two cars in a hoistway
		EXTRA_MSGS	*extraMsg	// reserved for extra messages
	); // HOISTWAY INFORMATION
}
#endif

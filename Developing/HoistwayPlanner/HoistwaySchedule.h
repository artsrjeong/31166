#ifndef	_HS_H
#define	_HS_H
#include "Hoistway_Plan.h"
#include "HoistwayPlannerDefs.h"	// Added by ClassView


struct CoordinationDirector
{
	int car; // upper car or lower car
	int current;	// 
	int predecessor;
	int relation;	//STOP_PREDECESSOR_ARRIVAL or STOP_PREDECESSOR_DEPARTURE
};

#define USERFLOOR(x) (x + f_offset)
struct StopTimes
{
	int		sLink;		 // pointing to the sequence
	int		stopType;	 // stop type 
	float decelTime;    // deceleration timing
	float arrivalTime;  // arrival time to the position of this floor
	float nominalStopDuration; // nominal door dwell time + door open + door close time
	float extendedStopDuration; // only extended door time for keeping separation and precedence relationship
	float departureTime; // departure time after calibration
	float extendedDelayedStartDuration; // delayed start duration for TSA delay while door is closed but not to start
};

EXTERN StopTimes timing[MAX_CARS_IN_HOISTWAY][MAX_STOP];  // corresponding times for each stop

class HoistwaySchedule
{
public:
	int getPOICount(int POI);
	T_HPSCORE getPOIScore(void);
	LONG GetTSADelay(int orgLead,int tgtLead,int orgTrail,int tgtTrail,int MSD,int Afd);
	void TSAConsiderationForOneRun(CoordinationDirector direct);
	void TSAConsideration(void);
	HoistwayPlan plan;

	CarState carState[MAX_CARS_IN_HOISTWAY];
	void	SetCarStates(CarState *uCarState,CarState *lCarState);
	T_HPSCORE	TimingAndScoring(int upperCar,int lowerCar,HoistwayPlan &plan);
	T_HPSCORE	TimingAndScoring(int upperCar,int lowerCar,HoistwayPlan &plan, short planIndex, FILE *);  // CSW: 050805, added for debug purpose

	int nStop[MAX_CARS_IN_HOISTWAY];	// # of stop sequences in each car
	void Initialize(int upperCarID,int lowerCarID);
	void SetPlan(HoistwayPlan &planCandidate);
	void NominalTiming(void);
	void FollowPlanTiming(void);
	void SeparationTiming(void);

	CoordinationDirector	FindFirstRelation(int currentPointer[]
		,int currentRelation[]); // find first relationship and return that information
	void FollowOneCoordination(CoordinationDirector direct);// use found relationship and coordinate timing according to that
	void UpdateCurrentPointer(int currentPointer[]
		,int currentRelation[],CoordinationDirector direct); // update current pointer for next usage after current found relationship
	int	propagateDelay(int car,float extendTime,int propagetStartseq); 

	// calculate waiting time and service time scoring
	float getTimingScore(void);
	T_HPSCORE Scoring(void);
	
	void	T_ResetDETimes(void); // temporary for calculating shs

	float	RunTime(int car,int origin,int dest);
	float	DecelTime(int car,int origin,int dest);
	float	DoorDwellTime(int car,int floor,int board,int deboard,int type); // door dwell time for normal demand stop
	float	DoorTime(int car,int floor,int board,int deboard,int type); // door time according to type
	float	doorClosingTime[MAX_CARS_IN_HOISTWAY];
	float	doorOpeningTime[MAX_CARS_IN_HOISTWAY];
	float	doorOperatingTime[MAX_CARS_IN_HOISTWAY];
	float	JERK[MAX_CARS_IN_HOISTWAY];
	float	ACC[MAX_CARS_IN_HOISTWAY];
	float	VEL[MAX_CARS_IN_HOISTWAY];
	UINT	AFD[MAX_CARS_IN_HOISTWAY];
	float	SAFETY_DECEL[MAX_CARS_IN_HOISTWAY];
	float	startDelay[MAX_CARS_IN_HOISTWAY];
	float	stopDelay[MAX_CARS_IN_HOISTWAY];

	float	BoardK[2]; // Vi(0: waiting),Wi(1: boarded)
	float	WaitTimeK[2]; // A1W, A2W
	float	ServiceTimeK[2]; // A1S, A2S

	float FloorHeightTable[MAX_FLOOR];
	int lobby1Floor;
	int lobby2Floor;
	int lobby3Floor;
	float t_car_min_dwell;
	float t_hall_min_dwell;
	float t_hall_max_dwell;
	float	t_lobby_dwell;
	int	capacity[MAX_CARS_IN_HOISTWAY];
	int f_offset;
	int NoCarsInHoistway;  // if 1-D, calculation applies only to sequence not coordination //TBD shs
	
	int carID[MAX_CARS_IN_HOISTWAY]; // shs050810 for 1-D consideration added carid
									// if value is -1 then car does not exist

	float fullSpeedThres[MAX_CARS_IN_HOISTWAY];
	float fullAccThres[MAX_CARS_IN_HOISTWAY];

	void print(FILE *output);
	void timingPrint(FILE *output);

};
#endif

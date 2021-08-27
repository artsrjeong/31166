// HoistwayPlanSelection.cpp
//	Module for selecting best hoistway plan from the candidates
#include "stdafx.h"
#include <math.h>
#include <crtdbg.h>
#include "HoistwayPlanSelection.h"
#define	EXTERN
#include "HoistwaySchedule.h"
#undef	EXTERN
#define	EXTERN	extern
#include "HoistwayPlanner.h"
#undef	EXTERN
#include "..\common\2DSIM_DEFS.H"
#include "..\common\2dinterfacefunctiontype.h"
#include "..\HoistwayPlanner\ESP-NSP.H"

//jsr: GetCarState become local function
//extern GetCarState_T	GetCarState;
extern CallList		uCallList,lCallList;

#ifdef	WITH_DEBUG
extern int virtualAssign;
extern int d_passengerNo;
#endif
// door dwell time for normal demand stop
// currently not consider type
//	mimic of DwellTimeCurrent , CAGS dwell time based
// argument car is upper=1, lower=0
float	HoistwaySchedule::DoorDwellTime(int car,int floor,int board,int deboard,int type)
{
	int total;
	float min_dwell, dwell, lobbyDwell, stopDwell;
	float NorioriTime;
	// shs consider type
	/*
	if (type==HANDICAPPED_STOP)
		return 0;
	*/
	if(board > capacity[car])
		board = capacity[car];

	total = board + deboard;


	if (USERFLOOR(floor) == lobby1Floor || USERFLOOR(floor) == lobby2Floor)
	{
		if((board != 0) && (deboard != 0))
			lobbyDwell = t_hall_max_dwell;
		else
			lobbyDwell = t_lobby_dwell;
	}
	else
		lobbyDwell = 0.0;

	if (board == 0)
		stopDwell = t_car_min_dwell;
	else if (deboard == 0)
		stopDwell = t_hall_min_dwell;
	else
		stopDwell = t_hall_max_dwell;
	
	//min_dwell = max(lobbyDwell, stopDwell);
	if (lobbyDwell>stopDwell)
	{
		min_dwell=lobbyDwell;
	}
	else
	{
		min_dwell=stopDwell;
	}

	
	NorioriTime = 1.0;
	dwell = 2.1F + total*NorioriTime/2.0F; //Later, decide first parameter whether it is 2.1 or 2.0.
	if(dwell > 9.2)
		dwell -= 0.1F;
	
	if (dwell < min_dwell)
		return min_dwell;
	else
		return dwell;
}

float  HoistwaySchedule::DecelTime(int car,int origin,int dest)
{
	float dist;
	float acc,vel,jerk;
	float decelTime;

	dist = (float)fabs(FloorHeightTable[origin]-FloorHeightTable[dest]);

	acc=ACC[car];vel=VEL[car];jerk=JERK[car];
	if (dist<=0.001)  // dist is few return 0
		return 0;
	if (dist>=fullSpeedThres[car])
	{
		decelTime=dist/vel;
	}
	else if (dist>=fullAccThres[car])
	{
		decelTime=-acc/(2*jerk)+(float)(sqrt(acc*acc*acc+4*dist*jerk*jerk)/(2*jerk*sqrt(acc)));
	}
	else
	{
		decelTime=(float)pow(dist/(2*jerk),1/3);
	}
	decelTime+=MAKE_FLOAT_TIME(carInfo[car].tStartDelay);
	return decelTime;
}

float HoistwaySchedule::RunTime(int car,int origin,int dest)
{
	float dist;
	float runTime;
	float acc,vel,jerk;

	dist=(float)fabs(FloorHeightTable[origin]-FloorHeightTable[dest]);
	acc=ACC[car];vel=VEL[car];jerk=JERK[car];
	if (dist<=0.001)
	{
		runTime=0;
	}
	else if (dist>=fullSpeedThres[car])
	{
		runTime=dist/vel+acc/jerk+vel/acc;
	}
	else if (dist>=fullAccThres[car])
	{
		runTime=acc/jerk+(float)(sqrt(acc*acc*acc+4*dist*jerk*jerk)/(jerk*sqrt(acc)));
	}
	else
	{
		runTime=(float)pow(32*dist/jerk,1/3);
	}
	if (runTime>0)
		runTime+=MAKE_FLOAT_TIME((carInfo[car].tStartDelay+carInfo[car].tStopDelay));
	return runTime;
}

// Compensate the static infomation to float value
void HoistwaySchedule::Initialize(int upperCarID,int lowerCarID)
{
	int car;
	float jerk,acc,vel;
	int car_id;

	carID[LOWER_CAR]=lowerCarID; // shs050810 For 1-D check
	carID[UPPER_CAR]=upperCarID; // shs050810 For 1-D check
	//plan=(HoistwayPlan)NULL;

//	DoorOpeningTime=1.5; // second
//	DoorClosingTime=2.5; // second
	f_offset=bdInfo.f_offset;

	for (car=0;car<MAX_CARS_IN_HOISTWAY;car++)
	{
		car_id=carID[car];
		capacity[car]=carInfo[car_id].capacity;

		doorOpeningTime[car]=MAKE_FLOAT_TIME(carInfo[car_id].tDoorOpening);
		doorClosingTime[car]=MAKE_FLOAT_TIME(carInfo[car_id].tDoorClosing);

		jerk=JERK[car]=MAKE_FLOAT_SPEED(carInfo[car_id].maxJerk);
		acc=ACC[car]=MAKE_FLOAT_SPEED(carInfo[car_id].maxAcceleration);
		vel=VEL[car]=MAKE_FLOAT_SPEED(carInfo[car_id].maxVelocity);

		AFD[car]=carInfo[car_id].Afd;  // INTEGER
//		SAFETY_DECEL[car]=3;//

		fullSpeedThres[car]=(acc*acc*vel+vel*vel*jerk)/(jerk*acc);
		fullAccThres[car]=(2*acc*acc*acc)/(jerk*jerk);
		doorOperatingTime[car]=doorOpeningTime[car]+doorClosingTime[car];
		startDelay[car]=MAKE_FLOAT_TIME(carInfo[car_id].tStartDelay);
		stopDelay[car]=MAKE_FLOAT_TIME(carInfo[car_id].tStopDelay);
		
	}
	lobby1Floor=bdInfo.lobby1Floor;
	lobby2Floor=bdInfo.lobby2Floor;
	lobby3Floor=bdInfo.lobby3Floor;

	t_car_min_dwell=MAKE_FLOAT_TIME(param.tCarMinDwell);
	t_hall_min_dwell=MAKE_FLOAT_TIME(param.tHallMinDwell);
	t_hall_max_dwell=MAKE_FLOAT_TIME(param.tHallMaxDwell);
	t_lobby_dwell=MAKE_FLOAT_TIME(param.tLobbyDwell);
	BoardK[0]=MAKE_FLOAT_TIME(param.V);//waiting
	BoardK[1]=MAKE_FLOAT_TIME(param.W); // boarding
	WaitTimeK[0]=MAKE_FLOAT_TIME(param.A1W);	// A1W
	WaitTimeK[1]=MAKE_FLOAT_TIME(param.A2W);	// A2W
	ServiceTimeK[0]=MAKE_FLOAT_TIME(param.A1S); // A1S
	ServiceTimeK[1]=MAKE_FLOAT_TIME(param.A2S); // A2S

	for (int i=0;i<MAX_FLOOR;i++)
		FloorHeightTable[i]=MAKE_FLOAT_SPEED(bdInfo.landingPosition[i]);

	previousStopFloor[UPPER_CAR]=::previousStopFloor[upperCarID];
	previousStartTime[UPPER_CAR]=::previousStartTime[upperCarID];
	previousStopFloor[LOWER_CAR]=::previousStopFloor[lowerCarID];
	previousStartTime[LOWER_CAR]=::previousStartTime[lowerCarID];
	
}

void HoistwaySchedule::SetPlan(HoistwayPlan &planCandidate)
{
	plan=planCandidate;
	int car,stopLink;
	int stopNo;
	for (car=0;car<MAX_CARS_IN_HOISTWAY;car++) // for each car in a hoistway
	{
		if (carID[car]<0) 
			continue; // shs050810 For 1-D check 

		stopLink=plan.sequences[car].sequence_start;
		stopNo=0;
		while (stopLink!=-1)
		{
			timing[car][stopNo].sLink=stopLink; // sequence link not index
			timing[car][stopNo].stopType=plan.sequences[car].sequence[stopLink].type;
			stopLink=plan.sequences[car].sequence[stopLink].next;
			stopNo++;
		}
		nStop[car]=stopNo;	// number of stops
	}
}

// after SetPlan nominal timing can be calculated
void HoistwaySchedule::NominalTiming(void)
{
	int car,stopLink;
	int prevFloor,currentFloor;
	int stopNo;
	int boarding,deboarding;
	int type;
	//int oper;
	float eventTime;
	float prevTime;

	int carid;
	for (car=0;car<MAX_CARS_IN_HOISTWAY;car++) // for each car in a hoistway
	{
		carid=carID[car];
		if (carid<0) continue; // shs050810 for 1-D consideration	
//		GetCarState(carid,&carState[car]); // shs050810
		
		eventTime=MAKE_FLOAT_TIME(carState[car].tStartOper);

		if (carState[car].motionState==MOVING)
		{	
			prevTime=eventTime;
			prevFloor=carState[car].origin;
			stopNo=0;
		}
		else
		{	// STOP
			stopLink=timing[car][0].sLink;
			currentFloor=plan.sequences[car].sequence[stopLink].floor;
			type=plan.sequences[car].sequence[stopLink].type;
			// assume when stop, UpdateState in exec didn't update board,deboard
			boarding=carState[car].nBoard;
			deboarding=carState[car].nDeboard;
			prevFloor=currentFloor;
			switch (carState[car].frontDoorState)
			{	
				case DOORS_CLOSING:
					if (type==STOP_TYPE_DEMAND && currentFloor==prevFloor) 
					{	// reopen case
						eventTime=MAKE_FLOAT_TIME(carState[car].ctCurrent); // closed and reopen start time
						prevTime=eventTime+doorOpeningTime[car]
							+doorClosingTime[car]+
							DoorDwellTime(car,prevFloor,plan.sequences[car].sequence[stopLink].boarding,
							0,STOP_TYPE_DEMAND);
					}
					else
					{	// closed time
						prevTime=eventTime+doorClosingTime[car];
						eventTime=prevTime;
					}
					break;
				case DOORS_OPENED:
					prevTime=eventTime+doorClosingTime[car]
						+DoorDwellTime(car,prevFloor,boarding+plan.sequences[car].sequence[stopLink].boarding,
							deboarding,STOP_TYPE_DEMAND);// at opened deboard is updated
					eventTime-=doorOpeningTime[car]; // compensation for opening time

					break;
				case DOORS_OPENING:
					prevTime=eventTime+doorClosingTime[car]
						+doorOpeningTime[car]
						// COGS update board at door opening end,deboard at door dwell end, so it is needed to update by only plan
						// boarding and deboarding is not yet updated
						+DoorDwellTime(car,prevFloor,plan.sequences[car].sequence[stopLink].boarding,
							plan.sequences[car].sequence[stopLink].deboarding,STOP_TYPE_DEMAND); // Door time synchro
					break;
				default: // DOORS_CLOSED
					prevTime=MAKE_FLOAT_TIME(carState[car].ctCurrent); // current time
					if (carState[car].origin!=currentFloor)  // just arrived
					{	
						eventTime=prevTime;
					}
					if (plan.sequences[car].sequence[stopLink].type==STOP_TYPE_DEMAND)
					{   // Adding open,close,dwell time
						// boarding debaording 
						prevTime+=DoorTime(car,prevFloor,/*boarding+*/plan.sequences[car].sequence[stopLink].boarding,
							plan.sequences[car].sequence[stopLink].deboarding/*deboarding*/,STOP_TYPE_DEMAND);
					}
					break;
			}
			timing[car][0].arrivalTime=eventTime;
			timing[car][0].departureTime=prevTime;
			timing[car][0].nominalStopDuration=prevTime-eventTime;
			timing[car][0].extendedStopDuration=0;
			stopNo=1;
		}

//		prevFloor=previousStopFloor[car];
//		prevTime=previousStartTime[car];
		for (;stopNo<nStop[car];stopNo++)
		{
			stopLink=timing[car][stopNo].sLink;
			currentFloor=plan.sequences[car].sequence[stopLink].floor;
			type=plan.sequences[car].sequence[stopLink].type;

			timing[car][stopNo].decelTime=prevTime+DecelTime(car,prevFloor,currentFloor);
			timing[car][stopNo].arrivalTime=prevTime+RunTime(car,prevFloor,currentFloor);
			timing[car][stopNo].extendedStopDuration=0;	
			/* Calculate Door Cycle Time */
			if (isDoorOpenStopType(type))
			{	// Nominal Door Open
				boarding=plan.sequences[car].sequence[stopLink].boarding;
				deboarding=plan.sequences[car].sequence[stopLink].deboarding;
				
				timing[car][stopNo].nominalStopDuration=DoorTime(car,currentFloor,boarding,deboarding,type);
				timing[car][stopNo].departureTime=timing[car][stopNo].arrivalTime
					+timing[car][stopNo].nominalStopDuration;
			
			}
			else
			{	// NO NOMINAL DOOR Open
				timing[car][stopNo].nominalStopDuration=0;
				timing[car][stopNo].departureTime=timing[car][stopNo].arrivalTime;
			}

			/* Set unavoidable stop into consideration for new stop */
			if (isUnavoidableStopType(type) || 
				(carState[car].decelDecided==TRUE && stopNo==0))
			{
				prevTime=timing[car][stopNo].departureTime;
				prevFloor=currentFloor;
			}
		}
	}
}


void HoistwaySchedule::FollowPlanTiming(void)
{
	int currentRelation[MAX_CARS_IN_HOISTWAY];
	int currentPointer[MAX_CARS_IN_HOISTWAY];
	CoordinationDirector direct;
// tbd
	int flag;
	flag=0;
	int car;
	
	for (car=0;car<MAX_CARS_IN_HOISTWAY;car++)
	{
		if (carID[car]<0) return;  // shs050810 for 1-D consideration
		currentRelation[car]=0;
		currentPointer[car]=0;
	}
	while(flag<MAX_CARS_IN_HOISTWAY)	// till search completed
	{
		direct=FindFirstRelation(currentPointer,currentRelation);
		
		if (direct.car==-1) // search completed Nothing to do
		{
			break;
		}
		else
		{
			FollowOneCoordination(direct); //timing adjustment for coordination
			UpdateCurrentPointer(currentPointer,currentRelation,direct); // one step further to pointer
			//	check complete of sequence
			flag=0;
			for (int i=0;i<MAX_CARS_IN_HOISTWAY;i++)
			{
				if (currentPointer[i]>=nStop[i])
					flag+=1;
			}
		}
	}
}
// to use scoring for POI
/*
#ifndef	WITH_POI_SCORE
#define	WITH_POI_SCORE
#endif
*/
T_HPSCORE HoistwaySchedule::Scoring(void)
{
	T_HPSCORE score,POIScore;
	// scoring for timing 
	score=getTimingScore();
	// scoring for POI
	POIScore=getPOIScore();
#ifdef	WITH_POI_SCORE
	score+=POIScore;
#endif

	return score;
}

void	HoistwaySchedule::SetCarStates(CarState *uCarState,CarState *lCarState)
{
	memcpy(&carState[0],lCarState,sizeof(CarState));
	memcpy(&carState[1],uCarState,sizeof(CarState));
}

T_HPSCORE HoistwaySchedule::TimingAndScoring(int upperCar,int lowerCar,HoistwayPlan &candidate)
{
	T_HPSCORE score;
	Initialize(upperCar,lowerCar);
	SetPlan(candidate);
	//T_ResetDETimes(); //temporary shs
	NominalTiming();
	FollowPlanTiming();
	TSAConsideration();
	score=Scoring();
	return score;
}
/*
 * Calculate timing and scoring given a hoistway plan
 * The car Id is for the class to retrieve related hoistway info
 * Debug info will be dumped to a file. 
 */
T_HPSCORE HoistwaySchedule::TimingAndScoring(int upperCar,int lowerCar,HoistwayPlan &candidate, short planIndex, FILE *fp)
{
	T_HPSCORE score;
	Initialize(upperCar,lowerCar);
	SetPlan(candidate);

#ifdef	WITH_DEBUG
	fprintf(fp,"Candidate %d\n",planIndex);
	plan.print(fp);
#endif
	// T_ResetDETimes(); //temporary shs
	NominalTiming();

#ifdef	WITH_DEBUG 
	fprintf(fp,"Nominal Timing\n");
	timingPrint(fp);
#endif
	FollowPlanTiming();
#ifdef	WITH_DEBUG		
	fprintf(fp,"Planned Timing\n");
	timingPrint(fp);
#endif
	TSAConsideration();
#ifdef	WITH_DEBUG
	fprintf(fp,"TSA considered Timing\n");
	timingPrint(fp);
#endif

	score=Scoring();
#ifdef	WITH_DEBUG			// CSW072505: print for debug only
	fprintf(fp,"Score=%f\n\n",score);
	
#endif

	return score;
}

HPSCORE_T	HoistwayPlanSelection(int upperCarID,int lowerCarID,
			 int candidateSize,
			HoistwayPlan *candidatePlan,HoistwayPlan *bestPlan)
{
	int iPlan;
	T_HPSCORE bestScore,currentScore;
	int bestPlanIndex=-1;
	HoistwaySchedule Hs;
	//HoistwayPlan best;
	bestScore=(T_HPSCORE)T_HPSCORE_MAX;
	char FileName[100];
	sprintf(FileName,"HSchedule-%d-%d",lowerCarID,upperCarID);
#ifdef	WITH_DEBUG
	if (virtualAssign>=0)
	{
		sprintf(FileName,"%sV%d",FileName,virtualAssign);
	}

	strcat(FileName,".txt");

	FILE *fp;
	fp=fopen(FileName,"w");  // clear older contents
	fclose(fp);
#endif
	Hs.Initialize(upperCarID,lowerCarID);
	for (iPlan=0;iPlan<candidateSize;iPlan++)
	{
		Hs.SetPlan(candidatePlan[iPlan]);
#ifdef	WITH_DEBUG
		fp=fopen(FileName,"a");				// CSW072505: only print for debug
		fprintf(fp,"Candidate %d\n",iPlan);
		Hs.plan.print(fp);
		fclose(fp);
#endif

		// temporary
//		Hs.T_ResetDETimes();

		Hs.NominalTiming();


#ifdef	WITH_DEBUG // CSW072505: only print for debug
		fp=fopen(FileName,"a");
		fprintf(fp,"Nominal Timing\n");
		Hs.timingPrint(fp);
		fclose(fp);
#endif

		Hs.FollowPlanTiming();

#ifdef	WITH_DEBUG		// CSW072505: print for debug only
		fp=fopen(FileName,"a");
		fprintf(fp,"Planned Timing\n");
		Hs.timingPrint(fp);
		fclose(fp);
#endif
		Hs.TSAConsideration();
#ifdef	WITH_DEBUG
		fp=fopen(FileName,"a");
		fprintf(fp,"TSA considered Timing\n");
		Hs.timingPrint(fp);
		fclose(fp);
#endif

		currentScore=Hs.Scoring();

#ifdef	WITH_DEBUG			// CSW072505: print for debug only
		fp=fopen(FileName,"a");
		fprintf(fp,"Score=%f\n\n",currentScore);
		fclose(fp);
#endif

		if (iPlan==0)
		{
			bestScore=currentScore;
			bestPlanIndex=iPlan;
		}
		if (bestScore>currentScore)
		{
			bestScore=currentScore;
			bestPlanIndex=iPlan;
		}
	}

	
	if (bestPlanIndex>=0)
	{
		*bestPlan=candidatePlan[bestPlanIndex];
#ifdef	WITH_DEBUG			//	CSW072505: print for debug only
		fp=fopen(FileName,"a");
		fprintf(fp,"* Best Plan : %d candidate *\n",bestPlanIndex);
		candidatePlan[bestPlanIndex].print(fp);
		Hs.SetPlan(*bestPlan);
		Hs.NominalTiming();
		Hs.FollowPlanTiming();
		Hs.TSAConsideration();
		Hs.timingPrint(fp);
		fprintf(fp,"Score : %f\n",bestScore);
		fclose(fp);
#endif

		return (HPSCORE_T) bestScore;
	}
	else
	{
		//bestPlan=NULL;
		bestPlan->sequences[UPPER_CAR].sequence_start=-1;
		bestPlan->sequences[LOWER_CAR].sequence_start=-1;
#ifdef	WITH_DEBUG
		fp=fopen("HScheduleDebug.txt","a");
		if (virtualAssign<0)
			_RPT2(_CRT_WARN,"NO BEST PLAN for upper=%d,lower=%d\n",upperCarID,lowerCarID);
		else
			_RPT1(_CRT_WARN,"No Best Plan for virtual assign %d\n",virtualAssign);

		fprintf(fp,"No Best Plan found\n");
		fclose(fp);
#endif
		return 0;
	}
	
}

#define	MAX_RELATION	2		// ARRIVAL and DEPARTURE PRECEDESSOR
// find first relationship(predecessor) starting from currentPointer
//	 and return that information in to CoordinationDirector structure
CoordinationDirector	HoistwaySchedule::FindFirstRelation(int currentPointer[]
		,int currentRelation[])
{
	int car,othercar;
//	int point[MAX_CARS_IN_HOISTWAY],relation[MAX_CARS_IN_HOISTWAY];
	CoordinationDirector direct[MAX_CARS_IN_HOISTWAY];
	CoordinationDirector codi;
	int link,foundFlag;
	int tpoint,relationId,tpred;
	int currentLowerIndex,currentUpperIndex;
	int lowerPredecessorIndex,upperPredecessorIndex;

	for (car=0;car<MAX_CARS_IN_HOISTWAY;car++)
	{
		foundFlag=0; direct[car].car=-1; // not found
		tpoint=currentPointer[car];
		relationId=currentRelation[car];
		othercar=(car+1)%MAX_CARS_IN_HOISTWAY;
		while(tpoint<nStop[car])
		{
			link=timing[car][tpoint].sLink;
			for (;relationId<MAX_RELATION && foundFlag==0;relationId++)
			{
				switch (relationId)
				{
				case 0: 
					if (plan.sequences[car].sequence[link].arrival_predecessor!=-1)
					{
						direct[car].car=car; // car which is successor
						direct[car].current=tpoint; // pointing the timing array index for successor
						tpred=plan.sequences[car].sequence[link].arrival_predecessor;	
						direct[car].predecessor=tpred; // pointing the predecessor in plan sequence
						//direct[car].predecessor=plan.sequences[othercar].sequence[tpred].index;
#ifdef	WITH_DEBUG
						_ASSERT(plan.sequences[othercar].sequence[tpred].index>=0);

						if(timing[othercar][plan.sequences[othercar].sequence[tpred].index].sLink!=tpred)
						{
							_RPT0(_CRT_WARN,"link mismatch\n");
						}
#endif
						direct[car].relation=STOP_PREDECESSOR_ARRIVAL;

						foundFlag=1;
					}
					break;
				case 1:
					if (plan.sequences[car].sequence[link].departure_predecessor!=-1)
					{
						direct[car].car=car;
						direct[car].current=tpoint;
						tpred=plan.sequences[car].sequence[link].departure_predecessor;
						direct[car].predecessor=tpred;
						//direct[car].predecessor=plan.sequences[othercar].sequence[tpred].index;
#ifdef	WITH_DEBUG
						_ASSERT(plan.sequences[othercar].sequence[tpred].index>=0);
						if(timing[othercar][plan.sequences[othercar].sequence[tpred].index].sLink!=tpred)
						{
							_RPT0(_CRT_WARN,"link mismatch\n");
						}
#endif
						direct[car].relation=STOP_PREDECESSOR_DEPARTURE;

						foundFlag=1;
					}
					break;
				}
			}
			if (foundFlag)
			{	//	update current pointer for later use (not to search searched area)
				currentPointer[car]=tpoint;
				currentRelation[car]=direct[car].relation;
				break;
			}
			relationId=0;
			tpoint++;
		}
	}
	// Compare direct to find which is better and error check
	if (MAX_CARS_IN_HOISTWAY==2)
	{
		if (direct[LOWER_CAR].car==-1)
		{
			if (direct[UPPER_CAR].car==-1)
			{
				codi.car=-1;
			}
			else
			{
				codi=direct[UPPER_CAR];
			}
		}
		else if (direct[UPPER_CAR].car==-1)
		{
			codi=direct[LOWER_CAR];
		}
		else
		{
			_ASSERT(direct[LOWER_CAR].current>=0 && direct[UPPER_CAR].current>=0);
			link=timing[LOWER_CAR][direct[LOWER_CAR].current].sLink;
			currentLowerIndex=plan.sequences[LOWER_CAR].sequence[link].index;
			lowerPredecessorIndex=plan.sequences[UPPER_CAR].sequence[direct[LOWER_CAR].predecessor].index;

			link=timing[UPPER_CAR][direct[UPPER_CAR].current].sLink;
			currentUpperIndex=plan.sequences[UPPER_CAR].sequence[link].index;
			upperPredecessorIndex=plan.sequences[LOWER_CAR].sequence[direct[UPPER_CAR].predecessor].index;

			if (lowerPredecessorIndex<currentUpperIndex)
			{	// Lower car's coordination director is earlier
				if (upperPredecessorIndex<currentLowerIndex)
				{ // check
	#ifdef	WITH_DEBUG
					_RPT0(_CRT_WARN,"HM Link Error-Crossed 1\n");
	#endif
					codi.car=-1;
				}
				else
				{
					codi=direct[LOWER_CAR];
				}
			}
			else if (lowerPredecessorIndex>currentUpperIndex)
			{
				if (upperPredecessorIndex>currentLowerIndex)
				{ // check
	#ifdef	WITH_DEBUG
					_RPT0(_CRT_WARN,"HM Link Error-Crossed 2\n");
	#endif
					codi.car=-1;
				}
				codi=direct[UPPER_CAR];
			}
			else
			{	// lowerPredecessor==currentUpperIndex
				if (upperPredecessorIndex<currentLowerIndex)
				{ // Lower car passed upper car predecessor already, arrange upper car
					codi=direct[UPPER_CAR];
				}
				else if (upperPredecessorIndex>currentLowerIndex)
				{
					codi=direct[LOWER_CAR];
				}
				else
				{	// arrival and departure prededence is same node for upper car and lower car
					if (direct[LOWER_CAR].relation<direct[UPPER_CAR].relation)
					{
						codi=direct[LOWER_CAR];
					}
					else if (direct[LOWER_CAR].relation>direct[UPPER_CAR].relation)
					{
						codi=direct[UPPER_CAR];
					}
					else
					{
		#ifdef	WITH_DEBUG
						_RPT0(_CRT_WARN,"Error of linking\n");
		#endif
						codi.car=-1;
					}
				}
			}
		}
	}
	else	// not applicable
	{	// 1-D
	}
	return codi;
}

#define	DOOR_EXTENSION	0

// Only assume two cars in a hoistway
// use found relationship and coordinate timing according to that
void HoistwaySchedule::FollowOneCoordination(CoordinationDirector direct)
{
	int car,othercar;
	int tpredIndex;
	float extendTime;
	double extendTimeTSA=0;

	car=direct.car;
	othercar=(car+1)%MAX_CARS_IN_HOISTWAY; //two cars in a hoistway
	tpredIndex=plan.sequences[othercar].sequence[direct.predecessor].index;

	if (plan.sequences[car].sequence[timing[car][direct.current].sLink].type==STOP_TYPE_CONDITIONAL)
	{	// current conditional stop
		if (plan.sequences[othercar].sequence[direct.predecessor].type==STOP_TYPE_CONDITIONAL)
		{	// predecessor conditional stop
			if (direct.relation==STOP_PREDECESSOR_ARRIVAL)
			{
				if (timing[car][direct.current].decelTime<timing[othercar][tpredIndex].decelTime)
				{	// predecessor's decel is after the successor's decel
					extendTime=timing[othercar][tpredIndex].decelTime-timing[car][direct.current].decelTime;
					propagateDelay(car,extendTime,direct.current-1); 
				}
			}
			else if (direct.relation==STOP_PREDECESSOR_DEPARTURE)
			{
				if (timing[car][direct.current].decelTime<timing[othercar][tpredIndex].departureTime)
				{
					extendTime=timing[othercar][tpredIndex].decelTime-timing[car][direct.current].decelTime;
					propagateDelay(car,extendTime,direct.current); 
				}			
			}
		}
		else
		{
			if (direct.relation==STOP_PREDECESSOR_ARRIVAL)
			{
				if (timing[car][direct.current].decelTime<timing[othercar][tpredIndex].arrivalTime)
				{
					extendTime=timing[othercar][tpredIndex].arrivalTime-timing[car][direct.current].decelTime;
					propagateDelay(car,extendTime,direct.current-1); 
				}
			}
			else if (direct.relation==STOP_PREDECESSOR_DEPARTURE)
			{
				if (timing[car][direct.current].decelTime<timing[othercar][tpredIndex].departureTime)
				{
					extendTime=timing[othercar][tpredIndex].departureTime-timing[car][direct.current].decelTime;
					propagateDelay(car,extendTime,direct.current); 
				}			
			}	
		}
	}	// end current conditional stop processing
	else
	{	// shs need check predecessor is conditional stop
		if (plan.sequences[othercar].sequence[direct.predecessor].type==STOP_TYPE_CONDITIONAL)
		{
			if (direct.relation==STOP_PREDECESSOR_ARRIVAL)
			{
				if (timing[car][direct.current].arrivalTime<timing[othercar][tpredIndex].decelTime)
				{
					extendTime=timing[othercar][tpredIndex].decelTime-timing[car][direct.current].arrivalTime;
					propagateDelay(car,extendTime,direct.current-1); 
				}
			}
			else if (direct.relation==STOP_PREDECESSOR_DEPARTURE)
			{
				if (timing[car][direct.current].decelTime<timing[othercar][tpredIndex].departureTime)
				{
					extendTime=timing[othercar][tpredIndex].decelTime-timing[car][direct.current].departureTime;
					propagateDelay(car,extendTime,direct.current); 
				}			
			}
		}
		else
		{
			if (direct.relation==STOP_PREDECESSOR_ARRIVAL)
			{	// timing from previous stop's departure time adjustment is needed
				if (timing[othercar][tpredIndex].arrivalTime>
					timing[car][direct.current].arrivalTime)
				{ // adjust departure timing of stop [direct.current-1] and afterwards
					// propagate delay 'extendTime' of car from seqNo, fromOperation
					extendTime=timing[othercar][tpredIndex].arrivalTime-timing[car][direct.current].arrivalTime;
					propagateDelay(car,extendTime,direct.current-1); 
				}
			}
			else if (direct.relation==STOP_PREDECESSOR_DEPARTURE)
			{	// timing from current stop door dwell time adjustment is needed
				if (timing[othercar][tpredIndex].departureTime>
					timing[car][direct.current].departureTime)
				{ // adjust departure timing of stop [direct.current-1] and afterwards
					extendTime=timing[othercar][tpredIndex].departureTime-timing[car][direct.current].departureTime;
					// propagate delay 'extendTime' of car from seqNo (currently door extension)
					propagateDelay(car,extendTime,direct.current); 
				}
			}
		}
	}	// end not current conditional stop
}

void HoistwaySchedule::UpdateCurrentPointer(int currentPointer[]
		,int currentRelation[],CoordinationDirector direct) // update current pointer for next usage after current found relationship
{
	if (direct.car<0 || direct.car>MAX_CARS_IN_HOISTWAY)
		return;
	currentPointer[direct.car]=direct.current;
	if (direct.relation==STOP_PREDECESSOR_ARRIVAL)
	{
		currentRelation[direct.car]=STOP_PREDECESSOR_DEPARTURE;
	}
	else if (direct.relation==STOP_PREDECESSOR_DEPARTURE)
	{
		currentRelation[direct.car]=STOP_PREDECESSOR_ARRIVAL;
		currentPointer[direct.car]+=1;
	}
	else
	{
	}
}

//
int	HoistwaySchedule::propagateDelay(int car,float extendTime,int propagateStartseq)
{
	int seq;
	int ret=FALSE; // FALSE: cannot correct timing, TRUE: correct timing

	seq=propagateStartseq;
	// changing the door dwell of conditional stop have no effect on the following timing
	while (seq>=0 && timing[car][seq].stopType==STOP_TYPE_CONDITIONAL)
		seq--;
	if (seq<0)
	{
		ret=FALSE;
		seq=0;
	}
	else
		ret=TRUE;

	if (seq<nStop[car])  // change timing (door dwell)
	{
		timing[car][seq].extendedStopDuration+=extendTime;
		timing[car][seq].departureTime+=extendTime;
	}
	seq++;
	// by extension of previous timing, that timing propagate till the end
	for (;seq<nStop[car];seq++)
	{	 
		timing[car][seq].decelTime+=extendTime;
		timing[car][seq].arrivalTime+=extendTime;
		timing[car][seq].departureTime+=extendTime;
	}
	return ret;
}

// need more consideration : shs
float HoistwaySchedule::getTimingScore(void)
{
	int stop,originIndex,destIndex,seq;
	float score,wait1Score,wait2Score,service1Score,service2Score,serviceBoard1Score,serviceBoard2Score;
	float temp;
	float currentTime;
	int car;


	wait1Score=0;wait2Score=0;service1Score=0;service2Score=0;
	serviceBoard1Score=0;serviceBoard2Score=0;
	score=0;
	for (car=0;car<MAX_CARS_IN_HOISTWAY;car++)
	{
		if (carID[car]<0) continue; // shs050810 for 1-D consideration
		currentTime=MAKE_FLOAT_TIME(carState[car].ctCurrent);
		seq=plan.sequences[car].sequence_start;
		for (stop=0;stop<nStop[car];stop++)
		{	
			// Addition of waiting time
			originIndex=plan.sequences[car].sequence[seq].first_origin_call;
			while(originIndex>=0) // check not valid
			{
				plan.de_calls[car].calls[originIndex].origin_position_time=timing[car][stop].arrivalTime;
				if (timing[car][stop].arrivalTime<plan.de_calls[car].calls[originIndex].reg_time)
				{ // after origin arrival, new call is entered
                    temp=0;
				}
				else
				{	
					if (currentTime>plan.de_calls[car].calls[originIndex].origin_position_time)
					{	// current time is after arrival time then exclude past arrival
						temp=0;
					}
					else
					{
						// call entered and arrival at the origin ( waiting time via actual mm position)
						temp=(plan.de_calls[car].calls[originIndex].origin_position_time-plan.de_calls[car].calls[originIndex].reg_time);
					}
				}
				wait1Score+=temp;
				wait2Score+=(temp*temp);
				originIndex=plan.de_calls[car].calls[originIndex].next_same_boarding;
			}
			destIndex=plan.sequences[car].sequence[seq].first_dest_call;
			while(destIndex>=0)
			{
				plan.de_calls[car].calls[destIndex].dest_position_time=timing[car][stop].arrivalTime;
				temp=(plan.de_calls[car].calls[destIndex].dest_position_time-plan.de_calls[car].calls[destIndex].reg_time);
				if (plan.de_calls[car].calls[destIndex].status==PASSENGER_WAITING  // Waiting or
					&& plan.de_calls[car].calls[destIndex].origin_position_time>=currentTime)
				{   // arrive at origin after current time --> WAITING
					// service time for waiting passenger based on Actual position viewpoint
					service1Score+=temp;
					service2Score+=(temp*temp);
				}
				else if (plan.de_calls[car].calls[destIndex].dest_position_time>currentTime)
					//if (plan.de_calls[car].calls[destIndex].status==PASSENGER_BOARDED)
				{	// arrive at destination after current time --> BOARDED
					//    service time for boarded passenger based on actual position viewpoint
					serviceBoard1Score+=temp;
					serviceBoard2Score+=(temp*temp);
				}
				destIndex=plan.de_calls[car].calls[destIndex].next_same_deboarding;
			}
			seq=plan.sequences[car].sequence[seq].next;
		}
		// coefficient multimplicaiton needed here shs tbd
	}
	score=BoardK[0]*(WaitTimeK[0]*wait1Score+WaitTimeK[1]*wait2Score
		+ServiceTimeK[0]*service1Score+ServiceTimeK[1]*service2Score); 
	score+=(BoardK[1]*(ServiceTimeK[0]*serviceBoard1Score+ServiceTimeK[1]*serviceBoard2Score));

	return score;
}

// car is uppercar=1 or lowercar=0
float HoistwaySchedule::DoorTime(int car,int floor,int board,int deboard,int type)
{
	float tDoorTime;

	switch(type)
	{
		case STOP_TYPE_CONDITIONAL:
		case STOP_TYPE_YIELD:
		case STOP_TYPE_PARKING:
		case STOP_TYPE_PIVOT:			// CSW050808: Pivot stop should be handled the same way as Yield stop
			tDoorTime=0;
			break;
		case STOP_TYPE_DEMAND:
			tDoorTime=doorClosingTime[car]+doorOpeningTime[car]+DoorDwellTime(car,floor,board,deboard,type);
			break;
		default:
			tDoorTime=0;
			break;
	}
	return tDoorTime;
}


void HoistwaySchedule::timingPrint(FILE *output)
{
	int stop,car,floor,link,othercar;
	int linktemp;
	int board,deboard;
	char carDesc[2]={'L','U'};

	for (car=0;car<MAX_CARS_IN_HOISTWAY;car++)
	{
		if (carID[car]<0) continue;  // shs080811
		othercar=(car+1)%MAX_CARS_IN_HOISTWAY;
		if (carID[othercar]<0) othercar=car; // shs080811

		fprintf(output,"~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		fprintf(output,"Car: %c, Stop: %d",carDesc[car],nStop[car]);
		fprintf(output,"\tConditional->Decel Time,Others->ArrivalTime\t\t\tplan+car.nBoard\tplan+car.nDeboard\n");
		fprintf(output,"ID\tFloor\tPred(A or D)\tType\tArrival(Decel)Time\tDoorTime\tDepartureTime\tBoard\tDeboard\n");
		fprintf(output,"~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		for (stop=0;stop<nStop[car];stop++)
		{
			link=timing[car][stop].sLink;
			floor=plan.sequences[car].sequence[link].floor;
			board=plan.sequences[car].sequence[link].boarding;
			deboard=plan.sequences[car].sequence[link].deboarding;
			fprintf(output,"[%3d]\t%4d\t",stop,floor);

			// arrival predecessor printing
			if (plan.sequences[car].sequence[link].arrival_predecessor>=0)
			{
				if (othercar ==car) //1-D
				{
					fprintf(output,"False Predirection");
				}
				else
				{
					linktemp=plan.sequences[car].sequence[link].arrival_predecessor;
					fprintf(output,"A<-%3d\t",plan.sequences[othercar].sequence[linktemp].index);
	//				fprintf(output,"[%2d]\t",linktemp);
				}
			}
			else
			{
				fprintf(output,"      \t");
			}
			// departure predecessor printing
			if (plan.sequences[car].sequence[link].departure_predecessor>=0)
			{
				if (othercar==car) //1-D
				{
					fprintf(output,"False Predirection");
				}
				else
				{
					linktemp=plan.sequences[car].sequence[link].departure_predecessor;
					fprintf(output,"D<-%3d\t",plan.sequences[othercar].sequence[linktemp].index);
//					fprintf(output,"[%2d]\t",linktemp);
				}
			}
			else
			{
				fprintf(output,"      \t");
			}

			switch(timing[car][stop].stopType)
			{
			case STOP_TYPE_CONDITIONAL:
				fprintf(output,"Conditional\t");
				fprintf(output,"%8.3f\t%8.3f\t%8.3f\t",
					timing[car][stop].decelTime,
					timing[car][stop].nominalStopDuration+timing[car][stop].extendedStopDuration,
					timing[car][stop].departureTime);
				break;
			case STOP_TYPE_YIELD:
				fprintf(output,"Yield      \t");
				fprintf(output,"%8.3f\t%8.3f\t%8.3f\t",
					timing[car][stop].arrivalTime,
					timing[car][stop].nominalStopDuration+timing[car][stop].extendedStopDuration,
					timing[car][stop].departureTime);
				break;
			case STOP_TYPE_DEMAND:
				fprintf(output,"Demand     \t");
				fprintf(output,"%8.3f\t%8.3f\t%8.3f\t",
					timing[car][stop].arrivalTime,
					timing[car][stop].nominalStopDuration+timing[car][stop].extendedStopDuration,
					timing[car][stop].departureTime);
				break;
			case STOP_TYPE_PARKING:
				fprintf(output,"Parking    \t");
				fprintf(output,"%8.3f\t%8.3f\t%8.3f\t",
					timing[car][stop].arrivalTime,
					timing[car][stop].nominalStopDuration+timing[car][stop].extendedStopDuration,
					timing[car][stop].departureTime);
				break;
			// CSW050808: Added code to print served stop
			case STOP_TYPE_SERVED:
				fprintf(output,"Served      \t");
				fprintf(output,"%8.3f\t%8.3f\t%8.3f\t",
					timing[car][stop].arrivalTime,
					timing[car][stop].nominalStopDuration+timing[car][stop].extendedStopDuration,
					timing[car][stop].departureTime);
				break;
			// CSW050808: Added code to print served stop
			// CSW050808: Added code to print served stop
			case STOP_TYPE_PIVOT:
				fprintf(output,"Pivot      \t");
				fprintf(output,"%8.3f\t%8.3f\t%8.3f\t",
					timing[car][stop].arrivalTime,
					timing[car][stop].nominalStopDuration+timing[car][stop].extendedStopDuration,
					timing[car][stop].departureTime);
				break;
			// CSW050808: Added code to print served stop
			default:
				fprintf(output,"Unknown Stop\t");
				fprintf(output,"XXXXXXXXXXXXXX\t");
				break;
			}
			if (stop==0 && (carState[car].frontDoorState==DOORS_OPENING || carState[car].frontDoorState==DOORS_OPENED))
				fprintf(output,"\t%d+%d\t%d+%d\n",board,carState[car].nBoard,deboard,carState[car].nDeboard);
			else
				fprintf(output,"\t%d\t%d\n",board,deboard);
		}

		fprintf(output,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	}
}

void HoistwaySchedule::print(FILE *output)
{
	int stop,car,floor,link;
	fprintf(output,"/////////////////////////////\n");
	fprintf(output,"Candidate Plan\n");
	plan.print(output);
	
	for (car=0;car<MAX_CARS_IN_HOISTWAY;car++)
	{
		if (carID[car]<0) continue;  // shs080811

		fprintf(output,"Car(L=0,U=1): %d, Stop: %d\n",car,nStop[car]);
		fprintf(output,"Floor\tArrivalTime\tDoorTime\tDepartureTime\n");
		fprintf(output,"---------------------------\n");
		for (stop=0;stop<nStop[car];stop++)
		{
			link=timing[car][stop].sLink;
			floor=plan.sequences[car].sequence[link].floor;
			fprintf(output,"%4d\t%8.3f\t%8.3f\t%8.3f\n",floor,
				timing[car][stop].arrivalTime,
				timing[car][stop].nominalStopDuration+timing[car][stop].extendedStopDuration,
				timing[car][stop].departureTime);
		}
		fprintf(output,"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	}
}

// Timing modification for separation like NSP vs NCP
void HoistwaySchedule::SeparationTiming(void)
{	// TBD shs
}
void HoistwaySchedule::T_ResetDETimes(void)
{
	int car, stop;

	for (car=0;car<MAX_CARS_IN_HOISTWAY;car++)
	{
		if (carID[car]<0) continue; // shs050810 for 1-D consideration
		stop=plan.de_calls[car].sequence_start;
		while(stop>=0)
		{
			plan.de_calls[car].calls[stop].reg_time=0;
			stop=plan.de_calls[car].calls[stop].next;
		}
	}
}

// Consider TSA for the delaying the car, add extendedStopDuration
void HoistwaySchedule::TSAConsideration(void)
{
	int currentRelation[MAX_CARS_IN_HOISTWAY];
	int currentPointer[MAX_CARS_IN_HOISTWAY];
	CoordinationDirector direct;

	int flag;
	flag=0;
	int car;
	
	for (car=0;car<MAX_CARS_IN_HOISTWAY;car++)
	{
		if (carID[car]<0) return;  // shs050810 for 1-D consideration
		currentRelation[car]=0;
		currentPointer[car]=0;
	}
	// TSA delay for first stop of leading car is not reached
	UpdateFirstStopTSA(); // update TSA delay only for first stop of leading car

	// Do while till search completed
	while(flag<MAX_CARS_IN_HOISTWAY)	// till search completed
	{
		// find relationship to consider
		direct=FindFirstRelation(currentPointer,currentRelation);
		
		if (direct.car==-1) // search completed Nothing to do
		{
			break;
		}
		else
		{	// found relation ship adjustment
			TSAConsiderationForOneRun(direct); //timing adjustment by considering TSA
			UpdateCurrentPointer(currentPointer,currentRelation,direct); // one step further to pointer
			//	check complete of sequence
			flag=0;
			for (int i=0;i<MAX_CARS_IN_HOISTWAY;i++)
			{
				if (currentPointer[i]>=nStop[i])
					flag+=1;
			}
		}
	}
}

// From one found relationship, consider TSA delay for run on run
// first check the real origin/dest for trailing and leading car
void HoistwaySchedule::TSAConsiderationForOneRun(CoordinationDirector direct)
{	// needs implementation details // shs
	int leadCar,trailingCar;
	int leadCarSeqIndex;
	int	trailingCarSeqIndex;
	float TSADelay;
	float extendTime;
	float leadCarDepartureTime,trailingCarDepartureTime;
	double extendTimeTSA=0;
	int	leadCarTimingIndex,trailingCarTimingIndex;
	int seqLink;
	int	tpredIndex;
	int iDfloorSearchInTiming;
	int trailingCarDepartureTimingIndex; // timing index for departure from origin for trailing car
	FLOOR_T	leadCarOrigin,leadCarDest;	// leading Car Origin and destination
	FLOOR_T	trailingCarOrigin,trailingCarDest;	// trailing Car Origin and destination
	
	trailingCarDepartureTimingIndex=0;
	trailingCar=direct.car;
	leadCar=(trailingCar+1)%MAX_CARS_IN_HOISTWAY; //two cars in a hoistway
	tpredIndex=plan.sequences[leadCar].sequence[direct.predecessor].index; // get timing index for lead car(predecessor)
	leadCarSeqIndex=direct.predecessor;
	trailingCarSeqIndex=timing[trailingCar][direct.current].sLink;

	if (direct.relation==STOP_PREDECESSOR_ARRIVAL)
	{	
		// if currect stop is conditional stop then we should find the destination and origin
		//        it is not conditional stop, current stop is destination and should find origin

		// find destination of leading car
		if (timing[leadCar][tpredIndex].stopType==STOP_TYPE_CONDITIONAL)
		{	// search for real origin
			iDfloorSearchInTiming=tpredIndex;
			for (;iDfloorSearchInTiming<nStop[leadCar];iDfloorSearchInTiming++)
			{
				if (timing[leadCar][iDfloorSearchInTiming].stopType!=STOP_TYPE_CONDITIONAL)
				{
					break;
				}
			}
			if (iDfloorSearchInTiming>=nStop[leadCar]) // not found in plan sequence -->running
			{
				seqLink=timing[leadCar][nStop[leadCar]-1].sLink;
				leadCarDest=plan.sequences[leadCar].sequence[seqLink].floor;
			}
			else
			{
				seqLink=timing[leadCar][iDfloorSearchInTiming].sLink;
				leadCarDest=plan.sequences[leadCar].sequence[seqLink].floor;
			}
		}
		else
		{
			leadCarDest=plan.sequences[leadCar].sequence[leadCarSeqIndex].floor;
		}
		// find destination of trailingcar
		if (timing[trailingCar][direct.current].stopType==STOP_TYPE_CONDITIONAL)
		{
			iDfloorSearchInTiming=tpredIndex;
			for (;iDfloorSearchInTiming<nStop[trailingCar];iDfloorSearchInTiming++)
			{
				if (timing[trailingCar][iDfloorSearchInTiming].stopType!=STOP_TYPE_CONDITIONAL)
				{
					break;
				}
			}
			if (iDfloorSearchInTiming>=nStop[trailingCar]) // not found in plan sequence -->running
			{
				seqLink=timing[trailingCar][nStop[trailingCar]-1].sLink;
				trailingCarDest=plan.sequences[trailingCar].sequence[seqLink].floor;
			}
			else
			{
				seqLink=timing[trailingCar][iDfloorSearchInTiming].sLink;
				trailingCarDest=plan.sequences[trailingCar].sequence[seqLink].floor;
			}
		}
		else
		{
			trailingCarDest=plan.sequences[trailingCar].sequence[trailingCarSeqIndex].floor;
		}
		
		// find origins and check the TSA (Not conditional stop origin)
		leadCarTimingIndex=tpredIndex-1;
		trailingCarTimingIndex=direct.current-1;
		

		// find lead car origin and departure timing
		for (;leadCarTimingIndex>=0;leadCarTimingIndex--)
		{
			if (timing[leadCar][leadCarTimingIndex].stopType!=STOP_TYPE_CONDITIONAL)
			{
				break;
			}
		}
		// get lead car origin
		if (leadCarTimingIndex<0) // not found in plan sequence -->running
		{
			leadCarOrigin=carState[leadCar].origin;
			leadCarDepartureTime=MAKE_FLOAT_TIME(carState[leadCar].tStartOper);
		}
		else
		{
			seqLink=timing[leadCar][leadCarTimingIndex].sLink;
			leadCarOrigin=plan.sequences[leadCar].sequence[seqLink].floor;
			leadCarDepartureTime=timing[leadCar][leadCarTimingIndex].departureTime;
		}

		for (;trailingCarTimingIndex>=0; trailingCarTimingIndex--)
		{
			if (timing[trailingCar][trailingCarTimingIndex].stopType!=STOP_TYPE_CONDITIONAL)
			{
				break;
			}
		}

		// get trailing car origin
		if (trailingCarTimingIndex<0) // not found in plan sequence -->running
		{
			trailingCarDepartureTimingIndex=0; // shs should be -1
			trailingCarOrigin=carState[trailingCar].origin;
			trailingCarDepartureTime=MAKE_FLOAT_TIME(carState[trailingCar].tStartOper);
		}
		else
		{
			trailingCarDepartureTimingIndex=trailingCarTimingIndex;
			seqLink=timing[trailingCar][trailingCarTimingIndex].sLink;
			trailingCarOrigin=plan.sequences[trailingCar].sequence[seqLink].floor;
			trailingCarDepartureTime=timing[trailingCar][trailingCarTimingIndex].departureTime;
		}

		TSADelay=MAKE_FLOAT_TIME(GetTSADelay(leadCarOrigin,leadCarDest,
			trailingCarOrigin,trailingCarDest,param.MinSeparationDist,AFD[leadCar]));
		
		if (leadCarDepartureTime+TSADelay > trailingCarDepartureTime)
		{	// trailing car starts earlier than lead car by TSADelay 
			// adjust the expected timing by adding extendedTime
			extendTime=leadCarDepartureTime+TSADelay-trailingCarDepartureTime;
			propagateDelay(trailingCar,extendTime,trailingCarDepartureTimingIndex); // propagate TSA delay
		}
	}
	else if (direct.relation==STOP_PREDECESSOR_DEPARTURE) 
	{	
		// current index is for origin if it is not conditional stop
		if (timing[leadCar][tpredIndex].stopType==STOP_TYPE_CONDITIONAL)
		{	// search for real origin
			iDfloorSearchInTiming=tpredIndex;
			for (;iDfloorSearchInTiming>=0;iDfloorSearchInTiming--)
			{
				if (timing[leadCar][iDfloorSearchInTiming].stopType!=STOP_TYPE_CONDITIONAL)
				{
					break;
				}
			}
			if (iDfloorSearchInTiming<0) // not found in plan sequence -->running
			{
				leadCarOrigin=carState[leadCar].origin;
				leadCarDepartureTime=MAKE_FLOAT_TIME(carState[leadCar].tStartOper);
			}
			else
			{
				seqLink=timing[leadCar][iDfloorSearchInTiming].sLink;
				leadCarOrigin=plan.sequences[leadCar].sequence[seqLink].floor;
				leadCarDepartureTime=timing[leadCar][iDfloorSearchInTiming].departureTime;
			}
		}
		else
		{
			leadCarOrigin=plan.sequences[leadCar].sequence[leadCarSeqIndex].floor;
			leadCarDepartureTime=timing[leadCar][tpredIndex].departureTime;
		}

		// find trailing car origin/departure Time
		if (timing[trailingCar][direct.current].stopType==STOP_TYPE_CONDITIONAL)
		{
			// search for destination
			iDfloorSearchInTiming=direct.current;
			for (;iDfloorSearchInTiming>=0;iDfloorSearchInTiming--)
			{
				if (timing[trailingCar][iDfloorSearchInTiming].stopType!=STOP_TYPE_CONDITIONAL) // fix leading car to trailingCar bug
				{
					break;
				}
			}
			if (iDfloorSearchInTiming<0) // not found in plan sequence -->running
			{
				trailingCarDepartureTimingIndex=0;
				trailingCarOrigin=carState[trailingCar].origin;
				trailingCarDepartureTime=MAKE_FLOAT_TIME(carState[trailingCar].tStartOper);
			}
			else
			{
				trailingCarDepartureTimingIndex=iDfloorSearchInTiming;
				seqLink=timing[trailingCar][iDfloorSearchInTiming].sLink;
				trailingCarOrigin=plan.sequences[trailingCar].sequence[seqLink].floor;
				trailingCarDepartureTime=timing[trailingCar][iDfloorSearchInTiming].departureTime;
			}
		}
		else
		{
			trailingCarDepartureTimingIndex=direct.current;
			trailingCarOrigin=plan.sequences[trailingCar].sequence[trailingCarSeqIndex].floor;
			trailingCarDepartureTime=timing[trailingCar][direct.current].departureTime;
		}

		// find dest and check the TSA (Not conditional stop origin)
		leadCarTimingIndex=tpredIndex+1;
		trailingCarTimingIndex=direct.current+1;

		for (;leadCarTimingIndex<nStop[leadCar];leadCarTimingIndex++)
		{	// find real destination floor to stop
			if (timing[leadCar][leadCarTimingIndex].stopType!=STOP_TYPE_CONDITIONAL)
			{
				break;
			}
		}

		// get lead car destination
		if (leadCarTimingIndex>=nStop[leadCar]) // not found in plan sequence -->running
		{
			seqLink=timing[leadCar][nStop[leadCar]-1].sLink; // last stop
		}
		else
		{
			seqLink=timing[leadCar][leadCarTimingIndex].sLink;
		}
		leadCarDest=plan.sequences[leadCar].sequence[seqLink].floor;

		for (;trailingCarTimingIndex<nStop[trailingCar]; trailingCarTimingIndex++)
		{
			if (timing[trailingCar][trailingCarTimingIndex].stopType!=STOP_TYPE_CONDITIONAL)
			{
				break;
			}
		}

		// get trailing car origin
		if (trailingCarTimingIndex>=nStop[trailingCar]) // not found in plan sequence -->running
		{
			seqLink=timing[trailingCar][nStop[trailingCar]-1].sLink;
		}
		else
		{
			seqLink=timing[trailingCar][trailingCarTimingIndex].sLink;
		}
		trailingCarDest=plan.sequences[trailingCar].sequence[seqLink].floor;

		TSADelay=MAKE_FLOAT_TIME(GetTSADelay(leadCarOrigin,leadCarDest,
			trailingCarOrigin,trailingCarDest,param.MinSeparationDist,AFD[leadCar]));
		
		if (leadCarDepartureTime+TSADelay > trailingCarDepartureTime)
		{	// trailing car starts earlier than lead car by TSADelay 
			// adjust the expected timing by adding extendedTime
			extendTime=leadCarDepartureTime+TSADelay-trailingCarDepartureTime;
			propagateDelay(trailingCar,extendTime,trailingCarDepartureTimingIndex); // propagate TSA delay from now
		}
	}
}

LONG HoistwaySchedule::GetTSADelay(int orgLead, int tgtLead, int orgTrail, int tgtTrail, int MSD, int Afd)
{
	double tsaDelay;
	double leadDist;
	double trailingDist;
	double initSeparation;
	

	leadDist=fabs((double)(bdInfo.landingPosition[orgLead])-(double)(bdInfo.landingPosition[tgtLead]));
	trailingDist=fabs((double)(bdInfo.landingPosition[orgTrail])-(double)(bdInfo.landingPosition[(tgtTrail)]));
	initSeparation=fabs((double)(bdInfo.landingPosition[orgTrail])-(double)(bdInfo.landingPosition[orgLead]));

	tsaDelay=ESPNSPDelay( leadDist,			// lead car travel distance
				    trailingDist,		// trailing car travel distance
				   (double)JERK[0]*(INT_SPEED_SCALE*1.0),				// max jerk for both cars
				   (double)ACC[0]*(INT_SPEED_SCALE*1.0),			// max acceleration for both cars
				   (double)VEL[0]*(INT_SPEED_SCALE*1.0),			// max velocity for both cars
				    initSeparation,	// distance between the origin of the two cars
				    (double)MSD,				// minimum separation distance
				   (double)Afd		// forced deceleration
				   );
	return (long)(tsaDelay*(INT_TIME_SCALE*1.0));
}

// return score considering POI violation
//	return value : any violation detected
T_HPSCORE HoistwaySchedule::getPOIScore() 
{
	int poiCount;
	static int poi=2;

	poiCount=getPOICount(poi);
	if (poiCount>0) // any detection of 'poi' POI violation
	{
		return (T_HPSCORE)T_HPSCORE_MAX; // 3.402823466E38F; it is maximum of float but use maximum of long
	}
	else
		return 0;

}

int HoistwaySchedule::getPOICount(int POI) // int poi = 1,2
{
	int nPOIViolation;
	int car,ncar,hwy,othercar;
	int stop,istop;
	int dir,floor1,floor2;
	int callId;
	CallList callList;
	int foundFlag=0;
	// follow the sequence and check it has demand stop with same origin of reverse direction.
	//     except terminal landing
	// for next direction is calculated first
	nPOIViolation=0;
	car=carID[0];
	hwy=carInfo[car].hoistwayID;
	ncar=hwyInfo[hwy].nCars;
	dir=0;

	switch (POI)
	{
	case 2:	// count 2POI violation
		for (car=0; car< ncar ; car++)
		{
			if (car==0) callList=lCallList;
			else callList=uCallList;

			for (stop=0;stop< nStop[car]-1;stop++)
			{
				if (timing[car][stop].stopType==STOP_TYPE_DEMAND ||
					timing[car][stop].stopType==STOP_TYPE_SERVED)
				{  // current stop is demand stop
					floor1=plan.sequences[car].sequence[timing[car][stop].sLink].floor;
					floor2=plan.sequences[car].sequence[timing[car][stop+1].sLink].floor;
					dir =floor2-floor1;  // current direction to move
					// search for the demand stop with hall call with reverse direction after this stop
					for (istop=stop+1;istop<nStop[car]-1;istop++)
					{
						// find demand stop at same floor
						if (timing[car][istop].stopType==STOP_TYPE_DEMAND &&
							floor1==plan.sequences[car].sequence[timing[car][istop].sLink].floor)
						{	// check it has reverse directional origin call
							floor2=plan.sequences[car].sequence[timing[car][istop+1].sLink].floor;
							if (dir*(floor2-floor1)<0) // reverse direction
							{
								callId=plan.sequences[car].sequence[timing[car][istop].sLink].first_origin_call;
							
								while (callId>=0)
								{
									if (dir * 
										(callList.calls[callId].destination-callList.calls[callId].origin)
										<0)
									{ // found reverse direction
										nPOIViolation++;
										foundFlag=1;
#ifdef	WITH_DEBUG
										char msg[150];
										sprintf(msg,"virtual:%d,%d POI violated %dth times at passenger %d assigned for car%d,stop%d vs stop%d\n"
											,virtualAssign,POI,nPOIViolation,d_passengerNo,carID[car],stop,istop);
										writeHPLog(msg);
#endif
										
										break;
									}
									callId=callList.calls[callId].next;
								}
							}
						}
						if (foundFlag==1) // found then exit for
						{
							foundFlag=0;
							break;
						}

					}
				}
			}

		}
		break;
	case 1: // count 1POI violation
		// count the case when the 2 cars in a hoistway has same stop floor with demand 
		//   only if either one car has hall call at there
		double carTime,othercarTime;
		PLAN_INDEX_T seqNo1,seqNo2;

		if (ncar>=2) 
		{	// 2 cars in a hoistway
			car=0;othercar=1;
			for (stop=0;stop<nStop[car];stop++)
			{
				if (timing[car][stop].stopType==STOP_TYPE_DEMAND ||
					timing[car][stop].stopType==STOP_TYPE_SERVED)
				{
					seqNo1=timing[car][stop].sLink;
					floor1=plan.sequences[car].sequence[seqNo1].floor;
					carTime=timing[car][stop].arrivalTime; // arrival time at stop
					/* search for the demand stop of other car at same floor */
					for (istop=0;istop<nStop[othercar];istop++)
					{
						if (timing[othercar][istop].stopType==STOP_TYPE_DEMAND ||
							timing[othercar][istop].stopType==STOP_TYPE_SERVED)
						{
							seqNo2=timing[othercar][istop].sLink;
							if (floor1==plan.sequences[othercar].sequence[seqNo2].floor)
							{	// at same floor
								othercarTime=timing[othercar][istop].arrivalTime;
								if (othercarTime>carTime)
								{  // other car arrives later : other car has hall call-->1POI violation
									if (plan.sequences[othercar].sequence[seqNo2].first_origin_call <0) // no hall call
									{
										// No violation
									}
									else
									{
										nPOIViolation++;
#ifdef	WITH_DEBUG
										char msg[150];
										sprintf(msg,"virtual:%d,%d POI violated %dth at passenger %d assigned for (car%d,stop%d) vs (car%d,stop%d)\n"
											,virtualAssign,POI,nPOIViolation,d_passengerNo,carID[car],stop,carID[othercar],istop);
										writeHPLog(msg);
#endif
										break;
									}
								}
								else
								{	// other car arrives earlier : the car has hall call --> 1POI voilation
									if (plan.sequences[car].sequence[seqNo1].first_origin_call < 0) // no hall call
									{
										// No violation
									}
									else
									{
										nPOIViolation++;
#ifdef	WITH_DEBUG
										char msg[150];
										sprintf(msg,"virtual:%d,%d POI violated %dth at passenger %d assigned for (car%d,stop%d) vs (car%d,stop%d)\n"
											,virtualAssign,POI,nPOIViolation,d_passengerNo,carID[car],stop,carID[othercar],istop);
										writeHPLog(msg);
#endif
										break;
									}
								}
							}
						}
					}
				}
			}
		}
		break;
	default:
		nPOIViolation=0;
		break;
	}
	return nPOIViolation;
}

BOOL HoistwaySchedule::isUnavoidableStopType(int type)
{
	switch(type)
	{
	case STOP_TYPE_DEMAND:
	case STOP_TYPE_PARKING:
	case STOP_TYPE_SERVED:
	case STOP_TYPE_PIVOT:
	case STOP_TYPE_YIELD:
		return true;
	default:
		return false;
	}
}

BOOL HoistwaySchedule::isDoorOpenStopType(int type)
{
	switch(type)
	{
	case STOP_TYPE_DEMAND:
	case STOP_TYPE_SERVED:
		return true;
	default:
		return false;
	}
}

void HoistwaySchedule::UpdateFirstStopTSA(void) // only for first stop of leading car
{
	int stopLink,stopType, i=0;
	int leadCar,trailCar,movingDirection;
	int bNeedTSACheck=0;
	FLOOR_T	leadOrigin,leadTarget;	// leading Car Origin and destination
	FLOOR_T	trailOrigin,trailTarget;	// trailing Car Origin and destination
	float leadCarDepartureTime,trailingCarDepartureTime;
	float TSADelay;
	float extendTime;

	if (carState[UPPER_CAR].motionState==MOVING && carState[UPPER_CAR].movingDirection==UP)
	{	//up directional moving leading car (upper car) and lower car same direction target then calculate it
		if (carState[LOWER_CAR].motionState==STANDING)
		{
			movingDirection=UP;
			leadCar=UPPER_CAR;
			trailCar=LOWER_CAR;
			bNeedTSACheck=1;
		}
	}
	else if (carState[LOWER_CAR].motionState==MOVING && carState[LOWER_CAR].movingDirection==DN)
	{	
		if (carState[UPPER_CAR].motionState==STANDING)
		{
			movingDirection=DN;
			leadCar=LOWER_CAR;
			trailCar=UPPER_CAR;
			bNeedTSACheck=1;
		}
	}

	if (bNeedTSACheck)
	{	
		stopLink=timing[leadCar][0].sLink;	// Target which leading car is going
		leadOrigin=carState[leadCar].origin; // origin
		leadTarget=plan.sequences[leadCar].sequence[stopLink].floor; // dest
		leadCarDepartureTime=MAKE_FLOAT_TIME(carState[leadCar].tStartOper);	// departure time from origin
		// lower car
		i=0;
		trailingCarDepartureTime=timing[trailCar][i].departureTime;
		stopLink=timing[trailCar][i++].sLink;
		trailOrigin=plan.sequences[trailCar].sequence[stopLink].floor;
		trailTarget=trailOrigin; // initialize for safe operation

		while (i<nStop[trailCar])
		{
			// skip conditional stop
			stopLink=timing[trailCar][i].sLink;
			stopType=timing[trailCar][i++].stopType;
			if (stopLink>=0)
			{
				if (stopType!=STOP_TYPE_CONDITIONAL) // valid stop
				{
					trailTarget=plan.sequences[trailCar].sequence[stopLink].floor;
					break;
				}

			}
			else	// invalid stop 
				break;
		}

		if (trailOrigin-trailTarget<0) // up direction plan for lower car
		{
			leadCarDepartureTime=MAKE_FLOAT_TIME(carState[leadCar].tStartOper);
			trailingCarDepartureTime=timing[trailCar][0].departureTime;
			TSADelay=MAKE_FLOAT_TIME(GetTSADelay(leadOrigin, leadTarget,trailOrigin, trailTarget, param.MinSeparationDist,AFD[leadCar]));
			if (leadCarDepartureTime+TSADelay > trailingCarDepartureTime)
			{	// trailing car starts earlier than lead car by TSADelay 
				// adjust the expected timing by adding extendedTime
				extendTime=leadCarDepartureTime+TSADelay-trailingCarDepartureTime;
				propagateDelay(trailCar,extendTime,0); // propagate TSA delay
			}
		}
	}
}

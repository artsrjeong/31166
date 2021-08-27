#include "stdafx.h"
#include "..\common\2dsim_defs.h"						//CSW051213: changed from COGS to common
#include "..\HoistwayPlanner\hoistway_plan.h"
#include "..\common\2DInterfaceFunctionType.h"				//CSW051213: changed from COGS to common
#include "Car.h"
#include <math.h>
#include <crtdbg.h>

extern int LocalSeparationFloorTable[MAXHWY][MAX_CARS_IN_HOISTWAY][MAXFLOOR];
extern Parameter param;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define EPSILON			1.0E-8

//##ModelId=4313F1F602FF
typedef int (*COGS_OCSSCommand)(UCHAR carIndex,CarCommand *oper);
//##ModelId=4313F1F60309
typedef void (*GD_GDCancelHallCalls)(UCHAR carIndex,FLOOR_T origin);
//##ModelId=4313F1F60312
typedef void (*GD_GDAcceptCarCall)(UCHAR carIndex,FLOOR_T destination);
//##ModelId=4313F1F6031C
typedef void (*GD_GDCancelCarCalls)(UCHAR carIndex,FLOOR_T destination);
//##ModelId=4313F1F6031E
typedef void (*HP_RegeneratePlan)(UCHAR hoistway_index,TIME_T ctCurrentTime);


extern "C"
{
	__declspec(dllexport) void DeleteExpectedCarCalls(UCHAR carIndex);
	__declspec(dllexport) UCHAR RemoveCarCalls(UCHAR carIndex, FLOOR_T destination);
	__declspec(dllexport) void ConvertHallCalls(UCHAR carIndex, FLOOR_T origin, CHAR predirection);
	__declspec(dllexport) void AcceptCarCalls(UCHAR carIndex, FLOOR_T destination);
}


extern COGS_OCSSCommand OCSSCommand;
extern GD_GDCancelHallCalls GDCancelHallCalls;
extern GD_GDAcceptCarCall GDAcceptCarCall;
extern GD_GDCancelCarCalls GDCancelCarCalls;
//extern HP_RegeneratePlan RegeneratePlan;
extern BOOL bCogsAttached;

extern double ESPNSPDelay(double leadDist,			// lead car travel distance
				   double trailingDist,		// trailing car travel distance
				   double jerk,				// max jerk for both cars
				   double accel,			// max acceleration for both cars
				   double velocity,			// max velocity for both cars
				   double initSeparation,	// distance between the origin of the two cars
				   double MSD,				// minimum separation distance
				   double accelForced
				   );
extern void updateFloorCall();
//##ModelId=430D774B02F6
CCar::CCar() : data(3)
{
}

//##ModelId=4313F94C01E6
CCar::~CCar()
{

}


//##ModelId=4313F649002F
bool CCar::isEnd(int inx)
{
	if(this->stop[inx].next==-1)
		return true;
	else
		return false;
}

//##ModelId=4313F64803B3
//Copy plan for this car 
void CCar::UpdateSqc(StopSequence paramSqc)
{
	for(int i=0;i<MAX_STOP;i++)
	{
		this->stop[i]=paramSqc.sequence[i];
	}
	this->curInx=paramSqc.sequence_start;
}



//##ModelId=4313F6480056
int CCar::GetNextTarget(int inx)
{
	if(inx!=-1)
		return this->stop[inx].floor;
	else
		return -1;
	/*
	int nextInx=this->stop[inx].next;
	if(nextInx!=-1)
	{
		return this->stop[nextInx].floor;
	}
	else
		return -1;
	*/
}

//##ModelId=4313F64703DA
int CCar::GetStopType(int inx)
{
	return stop[inx].type;
}

void CCar::SetStopType(int inx,unsigned char type)
{
	stop[inx].type=type;
}

//##ModelId=4313F647038A
bool CCar::isConditionAlive(int inx)
{
	if(//stop[inx].arrival_predecessor!=-1 ||
	   //stop[inx].arrival_successor!=-1 ||
	   stop[inx].departure_predecessor!=-1 ||
	   stop[inx].departure_successor!=-1)
		return true;
	else
		return false;
}

//##ModelId=4313F6470127
// Calculate both TSA delay and decelDwelltime
// decelDwellTime is the necessary delay on the trailing car, so lead car will have left
// before the trailing car hit decel point
// This function takes the larger of TSA delay and decelDwellTime
double CCar::GetDelay()
{
	int inx=curInx;
	int nextInx=stop[inx].next;
	int virtualTarget;
	int tsaDelay;
	int orgLead,tgtLead;

	while(GetStopType(nextInx)==STOP_TYPE_CONDITIONAL)
	{
		inx=stop[inx].next;
		nextInx=stop[inx].next;
	}

	if(isConditionAlive(inx))
	{
		//if there is departure precedence
		if(stop[inx].departure_predecessor!=-1)
		{
			int otherInx=stop[inx].departure_predecessor;
			int hwyInx=carId/2;
			double otherCarStartTime=otherCar->GetStartTime(otherInx);
			virtualTarget=(stop[inx].floor<stop[nextInx].floor) ? LocalSeparationFloorTable[hwyInx][LOWER_CAR][stop[nextInx].floor] : LocalSeparationFloorTable[hwyInx][UPPER_CAR][stop[nextInx].floor];
			double otherCarArrivalTime=otherCarStartTime+1000*motion.GetRunTime(otherCar->stop[otherInx].floor,virtualTarget);

			int targetFloor=stop[nextInx].floor;
			double runTime=1000*motion.GetRunTime(this->actual_pos,targetFloor);
			//double decelTime=1000*motion.GetCommittableTime(this->actual_pos,conditionalFloor);
			//decelTime-=ADDITIONAL_DELAY; // delay 100ms more
			double arriveDwellTime=otherCarArrivalTime-prevEventTime-runTime-carInfo.tDoorClosing-carInfo.tDoorOpening;

			orgLead=otherCar->stop[otherInx].floor;
			tgtLead=otherCar->stop[otherCar->stop[otherInx].next].floor;
		
			tsaDelay=this->GetTSADelay(orgLead,tgtLead,this->actual_pos,targetFloor,param.MinSeparationDist);
			double tsaDwellTime=otherCarStartTime+tsaDelay-prevEventTime-carInfo.tDoorClosing-carInfo.tDoorOpening;


			int conditionalFloor=stop[inx].floor;
			double decelTime=1000*motion.GetCommittableTime(this->actual_pos,conditionalFloor);
			decelTime-=ADDITIONAL_DELAY; // delay 100ms more
			double decelDwellTime=(otherCarStartTime-prevEventTime-decelTime-carInfo.tDoorClosing-carInfo.tDoorOpening);

			double doorDwellTime=(tsaDwellTime>decelDwellTime) ? tsaDwellTime : decelDwellTime;
			if(doorDwellTime>0)
				return doorDwellTime+ADDITIONAL_DELAY;
			else
				return -1;
		}
		return -1;
	}
	else
	{
		return -1;
	}
}


//##ModelId=4313F64700E1
// Calculate predicted departure time according to the plan
double CCar::GetStartTime(int inx)
{
	TIME_T startTime=prevEventTime;
	int tmpInx=curInx;
	int nextInx;
	if(oper!=CAS && oper!=CAS_DECEL && oper!=CRS)
	{
		startTime+=carInfo.tDoorOpening+carInfo.tDoorClosing+param.tHallMinDwell;
	}
	else if(oper==CRS)
	{
		
	}
	else
	{
		startTime+=GetRunTime(prevFloor,target_pos);
		if(GetStopType(curInx)==STOP_TYPE_PIVOT)
		{
			
		}
		else
			startTime+=carInfo.tDoorOpening+carInfo.tDoorClosing+param.tHallMinDwell;
	}
	while(tmpInx!=inx && tmpInx!=-1)
	{
		nextInx=stop[tmpInx].next;
		startTime+=GetRunTime(stop[tmpInx].floor,stop[nextInx].floor);
		int doorDwell=DoorDwellTime(stop[nextInx].floor,stop[nextInx].boarding,stop[nextInx].deboarding,0);
		startTime+=carInfo.tDoorOpening+carInfo.tDoorClosing+doorDwell;
		tmpInx=stop[tmpInx].next;
	}
	return (startTime+nTSADelay);
}


//##ModelId=4313F64603E2
// travel time from origin to destination
TIME_T CCar::GetRunTime(int origin, int dest)
{
	double originPos, destPos;
	double dist;
	double j;//maxJerk
	double a;//maxAcceleration
	double v;//maxVelocity
	int kindOfProfile;//1:Car reaches full speed ,2: Car reaches full acceleration ,3: Car does not reach full speed or acceleration
	double temp1, temp2;
	double time;

/*
	if(m_bParamHard)//shin 040914 two kinds of parameter
		return HardRunTime(origin, dest);
*/
	
	j = carInfo.maxJerk;
	a = carInfo.maxAcceleration;
	v = carInfo.maxVelocity;

	originPos = FloorToPos(origin);
	destPos = FloorToPos(dest);
	dist = fabs(originPos - destPos);

	// If distance is zero, than delay time is excluded.
	// So it takes 0 second.
	if(dist < EPSILON)
		return (TIME_T)0.0;

	//check the motion profile kind
	temp1 = (pow(a, 2.0)*v + pow(v, 2.0)*j)/(j*a);
	temp2 = 2.0*pow(a, 3.0)/pow(j, 2.0);
	if(dist >= temp1)
		kindOfProfile = 1;
	else if(dist >= temp2)
		kindOfProfile = 2;
	else
		kindOfProfile = 3;

	switch (kindOfProfile)
	{
	case 1:
		time = (dist/v+a/j+v/a);
		break;
	case 2:
		time = (2.0*(sqrt(pow(a, 3.0)+4*dist*pow(j, 2.0))/(2*j*sqrt(a)) + 0.5*a/j));
		break;
	case 3:
		time = (4.0*(pow(0.5*dist/j, 1.0/3.0)));
		break;
	default:
		time = 0.0;
		break;
	}
	time*=1000;
	time += carInfo.tStartDelay + carInfo.tStopDelay;
	return (TIME_T)(time);
}

//##ModelId=4313F6460374
int CCar::GetDirExec(int f1, int f2)
{
	if(f1<0 || f2<0)
		return NO_PREDIRECTION;
	if (f1 - f2 <0)							// determine the direction from s
		return UP;
	if (f1 == f2)
		return NO_DIR;
	else
		return DN;
}

//The entry time should be recorded in CAS entry action but COGS have start delay time 
//between CRS to CAS about 2~3 sec
// So I record start time in CRS.
void CCar::CRS_EntryAction(int pos,TIME_T time)
{
	prevEventTime=time;
	prevFloor=pos;
}
//##ModelId=4313F64602A2
void CCar::UpdateState(TIME_T nTime, CarState *car,bool bSend)
{
	currentTime=nTime;
	actual_pos=car->currentFloor;
	target_pos=car->target;
	frontDoorState=car->frontDoorState;
	NCF=car->currentNCF;
	motionState=car->motionState;
	mov_dir=car->movingDirection;
	decelDecided=car->decelDecided;
	// Below is only for activating regeneratePlan for testing in debug mode
#ifdef	REGEN_TEST
	int regenTestFlag=0;
	if (regenTestFlag==1)
		RegeneratePlan((UCHAR)(carId/2),currentTime);
#endif

	int nextInx=-1;
	if(curInx!=-1)
		nextInx=stop[curInx].next;
	//change state
	if(curInx==-1)
	{
		oper=CIS;
		prevNCF=NCF;
		prevMotion=motionState;
		prevDoorState=frontDoorState;	
		prevOper=oper;
		return;
	}
	if(oper==CIS && curInx!=-1) //target(T)
	{
		if(actual_pos==stop[curInx].floor && nextInx==-1)
		{}
		else
		{
			oper=CCS;
			CCS_EntryAction();// Record EventTime, SendTarget , Check to transition to next state
		}
	}
	if(oper==CCS && frontDoorState==DOORS_CLOSED && isCHOLDCondition(curInx))
	{
		oper=CHOLD;
	}
	if(oper==CCS && frontDoorState==DOORS_CLOSED && isCDELAYCondition(curInx))
	{
		oper=CDELAY;
	}
	if(oper==CCS && frontDoorState==DOORS_CLOSED
		&& !isCHOLDCondition(curInx))
	{
		oper=CRS;
		CRS_EntryAction(actual_pos,currentTime);
	}
	//put door closed Check routine before check CAS
	if(oper==CDCS && frontDoorState==DOORS_CLOSED)
	{
		if(bSend)SendTarget();
		if(nextInx!=-1)
		{
			oper=CCS;
			CCS_EntryAction();
		}

		if(stop[curInx].type==STOP_TYPE_SERVED 
		&& stop[curInx].floor==actual_pos
		&& nextInx==-1)
		{
			oper=CIS;
			curInx=-1;
		}
	}

	if((oper!=CAS && prevMotion!=MOVING && motionState==MOVING) ||
		(oper==CAS_DECEL && prevDecelDecided==TRUE && decelDecided==FALSE && motionState==MOVING))
	{
		oper=CAS;
		prevDoorCloseCut=-1;
		nPassengers=car->nPassengers; // it is effective during the run
		if(prevMotion!=MOVING && motionState==MOVING)
			prevFloor=actual_pos;

		// precedence cancelation check routine
		if(stop[curInx].departure_successor!=-1)
		{
			int otherInx=stop[curInx].departure_successor;
			// if(prevMotion!=MOVING && motionState==MOVING && otherCar->stop[otherInx].departure_predecessor!=-1)
			// CSW050803: Check whether the car has just left the floor, so the actual position is the current stop
			if(otherCar->stop[otherInx].departure_predecessor!=-1 && actual_pos== stop[curInx].floor)
				// CSW050811: Use the type of the stop to determine whether the precedence relationship can be cancelled
				//if(prevMotion!=MOVING && motionState==MOVING && otherCar->stop[otherInx].departure_predecessor!=-1 && stop[curInx].type != STOP_TYPE_DEMAND)
			{
				stop[curInx].departure_successor=-1;
				otherCar->stop[otherInx].departure_predecessor=-1;
				otherCar->currentTime=currentTime;
				otherCar->SendTarget();
				if(otherCar->oper==CHOLD)
				{
					otherCar->oper=CRS;
					otherCar->CRS_EntryAction(otherCar->actual_pos,currentTime);
				}
			}
		}
		//because if curInx is increased for conditional stopin SendTarget Function during standing, it increase one more time here
		int loopCnt=0;
		int tmpInx=curInx;
		int dir=this->GetDirExec(actual_pos,target_pos);
		int newDir=NO_DIR;
		while(stop[curInx].floor!=target_pos && curInx!=-1 
			&& loopCnt<6 && !isValidTarget(curInx))
		{
			nextInx=stop[curInx].next;
			//check there is next Index
			if(nextInx!=-1)
			{
				newDir=this->GetDirExec(stop[curInx].floor,stop[nextInx].floor);
				if(dir==newDir)
				{
					curInx=this->stop[curInx].next;
					loopCnt++;
				}
				//if direction is changed,don't change curInx
				else
				{
					curInx=tmpInx;
					break;
				}
			}
			else
			{
				curInx=tmpInx;
				break;
			}
		}
		//CAS_EntryAction();
	}

	if(oper==CAS && prevDecelDecided==FALSE && decelDecided==TRUE) // for sending predirection
	{
		oper=CAS_DECEL;
		if(bSend)SendTarget(); //for predirection
	}
	if(oper==CAS_DECEL && motionState==STANDING)
	{
		oper=CSS;
		nTSADelay=0;
		prevEventTime=currentTime;
		if(actual_pos!=stop[curInx].floor)
		{
			RegeneratePlan((UCHAR)(carId/2),currentTime);
		}
		else
		{
			if(GetStopType(curInx)!=STOP_TYPE_DEMAND && nextInx!=-1)
			{
				oper=CCS;
				CCS_EntryAction();
			}
			if(GetStopType(curInx)!=STOP_TYPE_DEMAND && nextInx==-1)
				oper=CIS;
			if(bSend)SendTarget(); // Send dwell time
		}
	}
	if(oper==CSS && GetStopType(curInx)!=STOP_TYPE_DEMAND && nextInx!=-1)
	{
		oper=CCS;
		CCS_EntryAction();
	}
	if(oper==CSS && GetStopType(curInx)!=STOP_TYPE_DEMAND && nextInx==-1)
		oper=CIS;

	if(oper!=CDOS && prevDoorState!=DOORS_OPENING && frontDoorState==DOORS_OPENING)
	{
		oper=CDOS;
	}
	if(prevDoorState!=DOORS_OPENED && frontDoorState==DOORS_OPENED)
	{
		oper=CDOS; //sometimes COGS doesn't send DOORS_OPENING
		if(bSend)SendTarget(); // Send dwell time
		CallProcessing();
		stop[curInx].type=STOP_TYPE_SERVED;
	}
	if(oper==CDOS && frontDoorState==DOORS_CLOSING)
	{
		if(bSend)SendTarget(); // for COGS to prevent motionstate
		oper=CDCS;
	}

	//

	prevNCF=NCF;
	prevDecelDecided=decelDecided;
	prevMotion=motionState;
	prevDoorState=frontDoorState;	
	prevOper=oper;
#ifdef COGS_UNIT_TEST
	SpyCar(carId,-1,0,0);
#endif
}

int CCar::GetTSADelay(int orgLead,int tgtLead,int orgTrail,int tgtTrail,int MSD)
{
	double tsaDelay;
	double leadDist=fabs(FloorToPos(orgLead)-FloorToPos(tgtLead));
	double trailingDist=fabs(FloorToPos(orgTrail)-FloorToPos(tgtTrail));
	double initSeparation=fabs(FloorToPos(orgTrail)-FloorToPos(orgLead));
	tsaDelay=ESPNSPDelay( leadDist,			// lead car travel distance
				    trailingDist,		// trailing car travel distance
				   (double)carInfo.maxJerk,				// max jerk for both cars
				   (double)carInfo.maxAcceleration,			// max acceleration for both cars
				   (double)carInfo.maxVelocity,			// max velocity for both cars
				    initSeparation,	// distance between the origin of the two cars
				    (double)MSD,				// minimum separation distance
				   (double)carInfo.Afd
				   );
	return (int)(tsaDelay*1000);
}

//##ModelId=4313F646028E
void CCar::SendTarget()
{
	int tsaDelay;
	CarCommand cmd;		
	cmd.target=-1;
	cmd.pre_dir=pre_dir;
	int nextInx=-1;
	int tmpCurInx=curInx;
	if(tmpCurInx!=-1)
		nextInx=this->stop[tmpCurInx].next;
	if(tmpCurInx==-1)
		return;

	if(oper==CAS_DECEL && stop[tmpCurInx].floor!=target_pos) // exception handle after decel point
	{
		cmd.target=target_pos;
		cmd.pre_dir=NO_DIR;
		cmd.frontDoorCommand=DOOR_NOT_OPEN;
	}

	else if(nextInx!=-1)
	{
		if(motionState==STANDING || decelDecided)
		{
			pre_dir=cmd.pre_dir=GetDirExec(stop[tmpCurInx].floor,stop[nextInx].floor);
		}
		//if the floor doesn't arrived yet or reopening situation
		if(GetStopType(tmpCurInx)==STOP_TYPE_DEMAND)
		{
			cmd.target=stop[tmpCurInx].floor;
			cmd.frontDoorCommand=DOOR_NORMAL;
		}
		//after servicing floor
		else if(GetStopType(tmpCurInx)==STOP_TYPE_SERVED)
		{
			if(stop[tmpCurInx].departure_predecessor==-1)
			{
				while(GetStopType(nextInx)==STOP_TYPE_CONDITIONAL && !isConditionAlive(nextInx))
				{
					tmpCurInx=nextInx;
					nextInx=this->stop[tmpCurInx].next;
				}
				cmd.target=stop[nextInx].floor;
				if(GetStopType(nextInx)==STOP_TYPE_DEMAND)
				{
					cmd.frontDoorCommand=DOOR_NORMAL;
				}
				else
				{
					cmd.frontDoorCommand=DOOR_NOT_OPEN;
				}
			}
			else if(stop[tmpCurInx].departure_predecessor!=-1)
			{
				cmd.target=stop[tmpCurInx].floor;
				cmd.frontDoorCommand=DOOR_NOT_OPEN;
			}
		}
		else if(GetStopType(tmpCurInx)==STOP_TYPE_CONDITIONAL)
		{
			while(GetStopType(tmpCurInx)==STOP_TYPE_CONDITIONAL && !isConditionAlive(tmpCurInx))
			{
				tmpCurInx=nextInx;
				nextInx=this->stop[tmpCurInx].next;
				if(oper==CAS || oper==CAS_DECEL)
				{
					tsaDelay=GetTSADelay(otherCar->prevFloor,otherCar->target_pos,prevFloor,stop[tmpCurInx].floor,param.MinSeparationDist);
					//don't extend target and set the stop type to STOP_TYPE_PIVOT
					if(tsaDelay>(this->prevEventTime+nTSADelay-otherCar->prevEventTime))
					{
						SetStopType(curInx,STOP_TYPE_PIVOT);
						tmpCurInx=curInx;
					}
					else//extend target 
					{
						curInx=tmpCurInx;
					}
				}
			}
			cmd.target=stop[tmpCurInx].floor;
			if(GetStopType(tmpCurInx)==STOP_TYPE_DEMAND)
			{
				cmd.frontDoorCommand=DOOR_NORMAL;
			}
			else
			{
				cmd.frontDoorCommand=DOOR_NOT_OPEN;
			}
		}
		/* else if(GetStopType(curInx)==STOP_TYPE_YIELD ||
			GetStopType(curInx)==STOP_TYPE_PARKING)
			*/
		// CSW050811: add type pivot to the logic
		else if(GetStopType(tmpCurInx)==STOP_TYPE_YIELD ||	GetStopType(tmpCurInx)==STOP_TYPE_PARKING ||
			GetStopType(tmpCurInx)==STOP_TYPE_PIVOT)
		{
			if(oper==CAS || oper==CAS_DECEL)
			{
				cmd.target=stop[tmpCurInx].floor;
				cmd.frontDoorCommand=DOOR_NOT_OPEN;
			}
			else if(oper==CRS && motionState==1)  // in the case of pivot stop during CRS
			{
				cmd.target=stop[tmpCurInx].floor;
				cmd.frontDoorCommand=DOOR_NOT_OPEN;
			}
			else if(oper!=CAS && oper!=CAS_DECEL
				&& stop[tmpCurInx].departure_predecessor==-1)
			{
				cmd.target=stop[nextInx].floor;
				if(GetStopType(nextInx)==STOP_TYPE_DEMAND)
				{
					cmd.frontDoorCommand=DOOR_NORMAL;
				}
				else
				{
					cmd.frontDoorCommand=DOOR_NOT_OPEN;
				}
			}	
			else if(stop[tmpCurInx].departure_predecessor!=-1)
			{
				cmd.target=stop[tmpCurInx].floor;
				cmd.frontDoorCommand=DOOR_NOT_OPEN;
			}
		}
		if((frontDoorState==DOORS_OPENING || frontDoorState==DOORS_OPENED) &&
			GetStopType(nextInx)==STOP_TYPE_CONDITIONAL)
		{
			TIME_T delay=(TIME_T)GetDelay();
			if(!param.bDwellTimeExtendACS)
				delay=-1;
			if(delay>=0)
			{
				cmd.frontDoorCommand=DOOR_CLOSE_CUT;
				cmd.frontDoorDwell=delay;
				//			this->doorCloseEndTime=time+delay;
			}
		}
	}
	else //this stop is end stop
	{
		cmd.target=stop[tmpCurInx].floor;
		cmd.pre_dir=NO_DIR;
		if(GetStopType(tmpCurInx)==STOP_TYPE_DEMAND)
		{
			cmd.frontDoorCommand=DOOR_NORMAL;
		}
		else
		{
			cmd.frontDoorCommand=DOOR_NOT_OPEN;
		}
	}

	//consider TSA
	cmd.runDelay=0;
	int thisDir,otherDir;

	if(oper!=CAS && oper!=CAS_DECEL)
	{
		nTSADelay=0;
		//this car should move and other car is moving
		if(cmd.target!=actual_pos && (otherCar->oper==CAS || otherCar->oper==CAS_DECEL))
		{
			//if same direction
			thisDir=this->GetDirExec(actual_pos,cmd.target);
			otherDir=otherCar->GetDirExec(otherCar->prevFloor,otherCar->target_pos);
			if(thisDir==otherDir)
			{
				//if this car is trailing car
				if((thisDir==UP && this->actual_pos<otherCar->actual_pos) ||
				   (thisDir==DN && this->actual_pos>otherCar->actual_pos))
				{
					tsaDelay=GetTSADelay(otherCar->prevFloor,otherCar->target_pos,actual_pos,cmd.target,param.MinSeparationDist);
					if(tsaDelay>(currentTime-otherCar->prevEventTime))
					{
						cmd.runDelay=tsaDelay-(currentTime-otherCar->prevEventTime);
						nTSADelay=cmd.runDelay;
					}
				}
			}
		}
	}
	if((oper==CAS || oper==CRS) && (otherCar->oper==CAS || otherCar->oper==CRS || otherCar->oper==CAS_DECEL) && cmd.target!=target_pos && target_pos!=prevFloor)
	{
		// uncomment after TSADelay calculation is finished
		thisDir=this->GetDirExec(actual_pos,cmd.target);
		otherDir=otherCar->GetDirExec(otherCar->prevFloor,otherCar->target_pos);
		if(thisDir==otherDir)
		{
			//if this car is trailing car
			if((thisDir==UP && this->actual_pos<otherCar->actual_pos && cmd.target>=otherCar->prevFloor && cmd.target>target_pos) ||
				(thisDir==DN && this->actual_pos>otherCar->actual_pos && cmd.target<=otherCar->prevFloor && cmd.target<target_pos))
			{
				tsaDelay=GetTSADelay(otherCar->prevFloor,otherCar->target_pos,prevFloor,cmd.target,param.MinSeparationDist);
				if(tsaDelay>(this->prevEventTime+nTSADelay-otherCar->prevEventTime))
				{
					cmd.target=target_pos;
					cmd.pre_dir=NO_DIR;
					cmd.frontDoorCommand=DOOR_NOT_OPEN;
				}
			}
		}
		
		/* Check Arrival Time
		if(!isArrivalTimeValid(cmd.target))
		{
			cmd.target=target_pos;
			cmd.pre_dir=NO_DIR;
			cmd.frontDoorCommand=DOOR_NOT_OPEN;
		}
		*/
	}
	if((oper==CAS || oper==CRS) && cmd.target!=actual_pos)
	{
		cmd.pre_dir=this->GetDirExec(actual_pos,cmd.target);
	}

	if(frontDoorState==DOORS_OPENED && cmd.target!=actual_pos && cmd.frontDoorCommand!=DOOR_CLOSE_CUT)
	{
		cmd.frontDoorCommand=DOOR_NORMAL;
		cmd.target=actual_pos;
	}
	else if((frontDoorState==DOORS_OPENING) && cmd.target != actual_pos && cmd.frontDoorCommand!=DOOR_CLOSE_CUT)
	{
	}
	else if(frontDoorState==DOORS_OPENED && cmd.frontDoorCommand==DOOR_CLOSE_CUT)
	{
		if(prevDoorCloseCut>cmd.frontDoorDwell)
			cmd.frontDoorDwell=currentTime-prevEventTime-carInfo.tDoorOpening;
		prevDoorCloseCut=cmd.frontDoorDwell;
		cmd.target=actual_pos;
		OCSSCommand(carId,&cmd);
	}
	else
		OCSSCommand(carId,&cmd);

#ifdef COGS_UNIT_TEST
	if(cmd.target!=target_pos)
	{
		SpyCar(carId,cmd.target,cmd.frontDoorCommand,cmd.pre_dir);
	}
#endif
}

/*
bool CCar::isArrivalTimeValid(int new_target)
{
	int thisArrivalTime,otherArrivalTime;
	int thisDir,otherDir;
	if(otherCar->oper!=CAS && otherCar->oper!=CAS_DECEL && otherCar->oper!=CRS)
		return TRUE;
	else //moving
	{
		thisDir=GetDirExec(prevFloor,new_target);
		otherDir=otherCar->GetDirExec(otherCar->prevFloor,otherCar->target_pos);
		if(thisDir==otherDir)
		{
			thisArrivalTime=(int)(prevEventTime+1000*motion.GetRunTime(prevFloor,new_target));
			int hwyInx=(int)(carId/2);
			if(thisDir==UP && actual_pos<otherCar->actual_pos)
			{
				otherArrivalTime=(int)(otherCar->prevEventTime+1000*motion.GetRunTime(otherCar->prevFloor,LocalSeparationFloorTable[hwyInx][LOWER_CAR][new_target]));
			}
			else if(thisDir==DN && actual_pos>otherCar->actual_pos)
			{
				otherArrivalTime=(int)(otherCar->prevEventTime+1000*motion.GetRunTime(otherCar->prevFloor,LocalSeparationFloorTable[hwyInx][UPPER_CAR][new_target]));
			}
			else
				return TRUE;
			if(thisArrivalTime>=otherArrivalTime)
				return TRUE;
			else
				return FALSE;
		}
		else
			return TRUE;
	}
}
*/
//##ModelId=4313F6460234
void CCar::CallProcessing()
{
	int index;
	int boarding=0;
	GetCalls(carId,&AssignedCalls);
	UCHAR deBoardCnt=RemoveCarCalls(carId,actual_pos);  // remove car calls at IHC database
	if(bDispatcherAttached)
	{
		index=AssignedCalls.boardedIndex;
		bool bNeedCancelCarCall=false;
		// test whether need to cancel car calls at GD
		while(index>=0)
		{
			if(AssignedCalls.calls[index].destination==actual_pos)
			{
				bNeedCancelCarCall=true;
				break;
			}
			index=AssignedCalls.calls[index].next;
		}
		if(bNeedCancelCarCall)
			GDCancelCarCalls(carId,actual_pos);  // cancel car calls at GD

		// if there is hall calls waiting at the floor
		index=AssignedCalls.waitingAtHead[actual_pos];
		if(index>=0)
			GDCancelHallCalls(carId,actual_pos);  // cancel hall calls at GD
	}
	UCHAR spareCap=carInfo.capacity-this->nPassengers+deBoardCnt;
	DeleteExpectedCarCalls(carId);
	index=AssignedCalls.waitingAtHead[actual_pos];  // shs debug needed
	
	// Check available capacity and convert hall call one at a tme 
	while(index>=0 && spareCap>0)
	{
		dir=GetDirExec(AssignedCalls.calls[index].origin,AssignedCalls.calls[index].destination);
		if(AssignedCalls.calls[index].origin==actual_pos 
			&& dir==pre_dir 
			&& curInx>=0 
			&& GetStopType(curInx)==STOP_TYPE_DEMAND)
		{
			ConvertHallCalls(carId,actual_pos,(CHAR)pre_dir);
			spareCap--;
		}
		index=AssignedCalls.calls[index].next;
	}
	this->nPassengers=carInfo.capacity-spareCap;
	GetCalls(carId,&AssignedCalls);
	index=AssignedCalls.expectedTransferHead;
	bool carCall[MAX_FLOOR];
	for(int i=0;i<MAX_FLOOR;i++)
		carCall[i]=false;
	while(index>=0)
	{
		int destination=AssignedCalls.calls[index].destination;
		carCall[destination]=true;
		index=AssignedCalls.calls[index].next;
	}
	//In the COGS , AccepCarCalls is called directly after ConvertHallCall 
	//without Door Detection
	// in RTS, the code should check whether door sensor detects boarding/deboarding
	// before converting hall calls. 
	for(i=0;i<MAX_FLOOR;i++)
	{
		if(carCall[i])
		{
			AcceptCarCalls(carId,i);
			if(bDispatcherAttached)
				GDAcceptCarCall(carId,i);
		}
	}
#ifdef COGS_UNIT_TEST
	updateFloorCall();
#endif

}

//##ModelId=4313F64601D9
void CCar::Init()
{
	prevOper=CIS;
	oper=CIS;
	curInx=-1;
	pre_dir=NO_PREDIRECTION;
	prevMotion=STANDING;
	prevDoorState=DOORS_CLOSED;
	nPassengers=0;
	prevNCF=-1;
	prevDoorCloseCut=-1;
	nTSADelay=0;
	//motion related initialize
	motion.SetMotionParam(pBuilding->nLandings,carInfo.maxJerk,carInfo.maxAcceleration,carInfo.maxVelocity,carInfo.tStartDelay,carInfo.tStopDelay,carInfo.Afd);
	for(int i=0;i<pBuilding->nLandings;i++)
	{
		motion.SetPos(i,pBuilding->landingPosition[i]);		
	}
}

//##ModelId=4313F64601A7

//##ModelId=4313F646011B
// Using the same logic in COGS to calculate door dwell time.
TIME_T CCar::DoorDwellTime(int floor, int board, int deboard, int type)
{
	int total;
	float min_dwell, dwell, lobbyDwell, stopDwell;
	float NorioriTime;
	// shs consider type
	/*
	if (type==HANDICAPPED_STOP)
		return 0;
	*/
	if(board > carInfo.capacity)
		board = carInfo.capacity;

	total = board + deboard;

	if (floor == this->pBuilding->lobby1Floor || floor == pBuilding->lobby2Floor)
		lobbyDwell = (float)(this->Param.tLobbyDwell);
	else
		lobbyDwell = 0.0;

	if (board == 0)
		stopDwell = (float)(this->Param.tCarMinDwell);
	else if (deboard == 0)
		stopDwell = (float)(this->Param.tHallMinDwell);
	else
		stopDwell = (float)(this->Param.tHallMaxDwell);
	
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
	dwell = (float)(2100 + 1000*total*NorioriTime/2.0); //Later, decide first parameter whether it is 2.1 or 2.0.
//sprintf(DEBUGMsg, "fl=%d nb=%d ndb=%d lob=%4.2lf stop=%4.2lf min=%4.2lf dwel=%4.2lf",
//fl, board, deboard, lobbyDwell, stopDwell, min_dwell, dwell);
//DEBUGWrite(0);
	
	if (dwell < min_dwell)
		return (TIME_T)min_dwell;
	else
		return (TIME_T)dwell;
}

//##ModelId=4313F64600F3
void CCar::CCS_EntryAction()
{
	prevEventTime=currentTime;
	if(frontDoorState==DOORS_CLOSED && isCHOLDCondition(curInx))
	{
		oper=CHOLD;
	}
	if(frontDoorState==DOORS_CLOSED && isCDELAYCondition(curInx))
	{
		oper=CDELAY;
	}
	if(frontDoorState==DOORS_CLOSED
		&& !isCHOLDCondition(curInx))
	{
		oper=CRS;
		CRS_EntryAction(actual_pos,currentTime);
	}
}

//##ModelId=4313F64600C1
bool CCar::isCHOLDCondition(int inx)
{
	if(stop[inx].departure_predecessor!=-1)
		return true;
	else
		return false;

}

//##ModelId=4313F6460099
// We need to think about this more
bool CCar::isCDELAYCondition(int inx)
{
//	if(stop[inx].departure_successor!=-1 && otherCar)
//		return true;
//	else
		return false;
}

// valid target is the stop which executer can not skip that stop in target determination
//##ModelId=4313F6460071
// This function is used when set the next curIndex after car start to move.
// STOP_TYPE_PARKING should be tested later 0603013
bool CCar::isValidTarget(int inx)
{
	bool ret=false;
	if(stop[inx].type==STOP_TYPE_DEMAND || stop[inx].type==STOP_TYPE_PARKING)
		ret=true;
	if(stop[inx].type==STOP_TYPE_CONDITIONAL && stop[inx].departure_predecessor!=-1)
		ret=true;
	return ret;
}

//##ModelId=4313F64901AC
double CCar::FloorToPos(int floor)
{
	return pBuilding->landingPosition[floor];//Change this according to spec table
}


//##ModelId=4315788803DD
//Called by COGS_SPY
void CCar::SetTSACase(int origin, int target, double delay)
{
	motion.t_run_start=delay/1000.0;
	motion.SetOriginTarget(origin,target);
}



//##ModelId=431582F1001D
// Called from COGS spy to retrieve information for TSA delay
int CCar::GetMotionValue(double time, int type)
{
	return (int)motion.GetMotionValue(time,type);
}
//It comes from COGS
int CCar::GetCommittableTime(int s, int d)
{
	return (int)(1000.0*motion.GetCommittableTime(s,d));
}

int CCar::GetNCP_T3(int s,int d)
{
	return (int)(1000.0*motion.GetNCP_T3(s,d));
}

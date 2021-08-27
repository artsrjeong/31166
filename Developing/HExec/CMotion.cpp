#include "stdafx.h"
#include "..\COGS\2dsim_defs.h"
#include "Common.h"
#include "CMotion.h"
#include "math.h"




//##ModelId=430ED7250134
CMotion::CMotion() : origin(3), target(4), MaxVelocity(4), MaxAcceleration(1), MaxJerk(2)
{
	// TODO: Add your specialized code here.
}



//##ModelId=43126C640390
double CMotion::FloorToPos(int floor)
{
	return (double)this->floorPosition[floor];
}

//##ModelId=43126D0400D3
void CMotion::GenerateFullProfile(double dist)
{
	int kindOfProfile;//1:Car reaches full speed ,2: Car reaches full acceleration ,3: Car does not reach full speed or acceleration
	double temp1, temp2;
	if(dist <= (0.0 + EPSILON))
	{
		// dist is too short or minus. Add this to the error.log.
		// dist is 0.0 when the origin floor and target floor are same.
		return;
	}

	//The condition where full speed is reached before full acceleration
	//(a^2>v*j) is discarded as this would be an illogical design.
	//move this to initialization
	if(pow(MaxAcceleration, 2.0) > MaxVelocity*MaxJerk)
	{
		/*
		CString strTemp;
		strTemp.Format("Illogical design! (a^2 > v*j), j=(%3.2f), a=(%3.2f), v=(%3.2f)",maxJerk, maxAcceleration, maxVelocity);
		MessageBox(strTemp);
		*/
	}

	//check the motion profile kind
	temp1 = (pow(MaxAcceleration, 2.0)*MaxVelocity + pow(MaxVelocity, 2.0)*MaxJerk)/(MaxJerk*MaxAcceleration);
	temp2 = 2.0*pow(MaxAcceleration, 3.0)/pow(MaxJerk, 2.0);
	if(dist >= temp1)
		kindOfProfile = 1; // full speed profile
	else if(dist >= temp2)
		kindOfProfile = 2; // full acc profile
	else
		kindOfProfile = 3;
	switch(kindOfProfile)
	{
		//Car reaches full speed
	case 1: MakeBreakPointTableA(MaxJerk, MaxAcceleration, MaxVelocity, dist);
		break;
		//Car reaches full acceleration
	case 2: MakeBreakPointTableB(MaxJerk, MaxAcceleration, MaxVelocity, dist);//FA
		break;
		//Car doesn't reache full speed or acceleration
	case 3: MakeBreakPointTableC(MaxJerk, MaxAcceleration, MaxVelocity, dist);//NO
		break;
	default:
		break;
	}
}



//##ModelId=4312762C0083
void CMotion::MakeBreakPointTableA(double j, double a, double v, double d)
{
	double T1, T2, T3;
	double deltaV1, deltaV2, deltaV3;
	double deltaD1, deltaD2, deltaD3;
	int i;
	double startTime;

	//shin 040819 After start delay time, car start to run.
	startTime = t_run_start + StartDelay;

	mp_n_break = NUM_BREAK_MAX_VELOCITY;

	//T1, T2, T3, deltaV1, deltaV2, deltaV3, deltaD1, deltaD2 and deltaD3
	//can be calculated in the initialization part to speed up for execution time.
	T1 = a/j;
	T2 = v/a;
	T3 = T1+T2;

	deltaV1 = pow(a, 2.0)/(2.0*j);
	deltaV2 = v - pow(a, 2.0)/j;
	deltaV3 = deltaV1;

	deltaD1 = pow(a, 3.0)/(6.0*pow(j, 2.0));
	deltaD2 = pow(v, 2.0)/(2.0*a) - a*v/(2.0*j);
	deltaD3 = a*v/j - deltaD1;


	mp_break[0] = startTime;
	mp_break[1] = startTime + T1;
	mp_break[2] = startTime + T2;
	mp_break[3] = startTime + T3;
	mp_break[4] = startTime + d/v;
	mp_break[5] = startTime + d/v+T1;
	mp_break[6] = startTime + d/v+T2;
	mp_break[7] = startTime + d/v+T3;

	if(target>origin)
	{
		mp_jerk[0] = j;
		mp_jerk[1] = 0.0;
		mp_jerk[2] = -j;
		mp_jerk[3] = 0.0;
		mp_jerk[4] = -j;
		mp_jerk[5] = 0.0;
		mp_jerk[6] = j;
		mp_jerk[7] = 0.0;
	}
	else if(target<origin)
	{
		mp_jerk[0] = -j;
		mp_jerk[1] = 0.0;
		mp_jerk[2] = j;
		mp_jerk[3] = 0.0;
		mp_jerk[4] = j;
		mp_jerk[5] = 0.0;
		mp_jerk[6] = -j;
		mp_jerk[7] = 0.0;
	}
	else
	{
		for(i=0; i<mp_n_break; i++)
		{
			mp_jerk[i] = 0.0;
		}
	}

	//Calculate acceleration, speed and position from break time and jerk
	mp_accel[0] = 0.0;
	mp_speed[0] = 0.0;
	mp_pos[0] = FloorToPos(origin);

	for(i=1; i<mp_n_break; i++)
	{
		mp_accel[i] = mp_accel[i-1]
			+ mp_jerk[i-1]*(mp_break[i]
			- mp_break[i-1]);
		mp_speed[i] = mp_speed[i-1]
			+ mp_accel[i-1]*(mp_break[i] - mp_break[i-1])
			+ mp_jerk[i-1]*pow((mp_break[i] - mp_break[i-1]), 2.0)/2.0;
		mp_pos[i] = mp_pos[i-1]
			+ mp_speed[i-1]*(mp_break[i] - mp_break[i-1])
			+ mp_accel[i-1]*pow((mp_break[i] - mp_break[i-1]), 2.0)/2.0
			+ mp_jerk[i-1]*pow((mp_break[i] - mp_break[i-1]), 3.0)/6.0;
	}
}

//##ModelId=4312768501D6
void CMotion::MakeBreakPointTableB(double j, double a, double v, double d)
{
	double T1, T2, T3;
	double tmpT;
	double deltaV1, deltaV2, deltaV3;
	double deltaD1, deltaD2, deltaD3;
	int i;
	double startTime;

	//shin 040819 After start delay time, car start to run.
	startTime = t_run_start + StartDelay;

	mp_n_break = NUM_BREAK_MAX_ACCELERATION;

	tmpT = sqrt(pow(a, 3.0)+4*d*pow(j, 2.0))/(2*j*sqrt(a));
	T1 = a/j;
	T2 = tmpT - 0.5*T1;
	T3 = tmpT + 0.5*T1;

	deltaV1 = pow(a, 2.0)/(2.0*j);
	deltaV2 = a*(tmpT-1.5*T1);
	deltaV3 = deltaV1;

	deltaD1 = pow(a, 3.0)/(6.0*pow(j, 2.0));
	deltaD2 = 0.5*(tmpT-1.5*T1)*(a*tmpT-deltaV1);
	deltaD3 = 0.5*d-deltaD1-deltaD2;


	mp_break[0] = startTime;
	mp_break[1] = startTime + T1;
	mp_break[2] = startTime + T2;
	mp_break[3] = startTime + T3;
	mp_break[4] = startTime + T3+T1;
	mp_break[5] = startTime + T3+T2;
	mp_break[6] = startTime + 2.0*T3;

	if(target>origin)
	{
		mp_jerk[0] = j;
		mp_jerk[1] = 0.0;
		mp_jerk[2] = -j;
		mp_jerk[3] = -j;
		mp_jerk[4] = 0.0;
		mp_jerk[5] = j;
		mp_jerk[6] = 0.0;
	}
	else if(target<origin)
	{
		mp_jerk[0] = -j;
		mp_jerk[1] = 0.0;
		mp_jerk[2] = j;
		mp_jerk[3] = j;
		mp_jerk[4] = 0.0;
		mp_jerk[5] = -j;
		mp_jerk[6] = 0.0;
	}
	else
	{
		for(i=0; i<mp_n_break; i++)
		{
			mp_jerk[i] = 0.0;
		}
	}

	//Calculate acceleration, speed and position from break time and jerk
	mp_accel[0] = 0.0;
	mp_speed[0] = 0.0;
	mp_pos[0] = FloorToPos(origin);

	for(i=1; i<mp_n_break; i++)
	{
		mp_accel[i] = mp_accel[i-1]
			+ mp_jerk[i-1]*(mp_break[i]
			- mp_break[i-1]);
		mp_speed[i] = mp_speed[i-1]
			+ mp_accel[i-1]*(mp_break[i] - mp_break[i-1])
			+ mp_jerk[i-1]*pow((mp_break[i] - mp_break[i-1]), 2.0)/2.0;
		mp_pos[i] = mp_pos[i-1]
			+ mp_speed[i-1]*(mp_break[i] - mp_break[i-1])
			+ mp_accel[i-1]*pow((mp_break[i] - mp_break[i-1]), 2.0)/2.0
			+ mp_jerk[i-1]*pow((mp_break[i] - mp_break[i-1]), 3.0)/6.0;
	}
}

//##ModelId=4312768B0026
void CMotion::MakeBreakPointTableC(double j, double a, double v, double d)
{
	double tmpT;
	int i;
	double startTime;

	//shin 040819 After start delay time, car start to run.
	startTime = t_run_start + StartDelay;

	mp_n_break = NUM_BREAK_NEITHER;

	tmpT = pow(0.5*d/j, 1.0/3.0);

	mp_break[0] = startTime;
	mp_break[1] = startTime + tmpT;
	mp_break[2] = startTime + 2.0*tmpT;
	mp_break[3] = startTime + 3.0*tmpT;
	mp_break[4] = startTime + 4.0*tmpT;

	if(target>origin)
	{
		mp_jerk[0] = j;
		mp_jerk[1] = -j;
		mp_jerk[2] = -j;
		mp_jerk[3] = j;
		mp_jerk[4] = 0.0;
	}
	else if(target<origin)
	{
		mp_jerk[0] = -j;
		mp_jerk[1] = j;
		mp_jerk[2] = j;
		mp_jerk[3] = -j;
		mp_jerk[4] = 0.0;
	}
	else
	{
		for(i=0; i<mp_n_break; i++)
		{
			mp_jerk[i] = 0.0;
		}
	}

	//Calculate acceleration, speed and position from break time and jerk
	mp_accel[0] = 0.0;
	mp_speed[0] = 0.0;
	mp_pos[0] = FloorToPos(origin);

	for(i=1; i<mp_n_break; i++)
	{
		mp_accel[i] = mp_accel[i-1]
			+ mp_jerk[i-1]*(mp_break[i]
			- mp_break[i-1]);
		mp_speed[i] = mp_speed[i-1]
			+ mp_accel[i-1]*(mp_break[i] - mp_break[i-1])
			+ mp_jerk[i-1]*pow((mp_break[i] - mp_break[i-1]), 2.0)/2.0;
		mp_pos[i] = mp_pos[i-1]
			+ mp_speed[i-1]*(mp_break[i] - mp_break[i-1])
			+ mp_accel[i-1]*pow((mp_break[i] - mp_break[i-1]), 2.0)/2.0
			+ mp_jerk[i-1]*pow((mp_break[i] - mp_break[i-1]), 3.0)/6.0;
	}
}



//##ModelId=4315597201BF
void CMotion::SetMotionParam(int nFloors,double jerk, double acceleration, double velocity, double startDelay, double stopDelay,double Afd)
{
	n_floor=nFloors;
	this->MaxJerk=jerk;
	this->MaxAcceleration=acceleration;
	this->MaxVelocity=velocity;
	this->Afd=Afd;
	this->StartDelay=startDelay/1000;
	this->StopDelay=stopDelay/1000;
}



//##ModelId=431566000395
void CMotion::SetPos(int floor, double pos)
{
	this->floorPosition[floor]=pos;
}



//##ModelId=43157B9000D5
void CMotion::SetOriginTarget(int org, int tgt)
{
	this->origin=org;
	this->target=tgt;
	double dist=(FloorToPos(org)>FloorToPos(tgt)) ? FloorToPos(org)-FloorToPos(tgt) : FloorToPos(tgt)-FloorToPos(org);
	this->GenerateFullProfile(dist);
	// TODO: Add your specialized code here.
}

//##ModelId=4315839E03DD
double CMotion::GetMotionValue(double time, int type)
{
	double pos;
	int into_decel;
	int floor;
	double jerk,accel,vel;
	switch(type)
	{
		case TSA_POSITION:
			ComputeMotionState(time,&jerk,&accel,&vel,&pos);
		break;
		case TSA_NSP:
			pos=GetESPPos(time,&into_decel,0);
			break;
		case TSA_NSF:
			floor=this->GetNextCommittableFloor(time,&into_decel,0);
			pos=this->FloorToPos(floor);
		break;
		case TSA_NCP:
			pos=GetNextCommittablePos(time,&into_decel,0);
		break;

		case TSA_NCF:
			floor=this->GetNextCommittableFloor(time,&into_decel,0);
			pos=this->FloorToPos(floor);
		break;
	}
	return pos;
}

//##ModelId=433BA153008C
bool CMotion::ComputeMotionState(double time, double *jerk, double *accel, double *speed, double *pos)
{

	int index;//index of break point table (motion profile)
	double deltaT;
	double j;//jerk
	double t=time/1000.0; //change to sec

	index = GetBreakPointTableIndex(t);
	if( index > -1)
		deltaT = t - mp_break[index];
	else
	{
		*jerk = *accel = *speed = 0.0;//shin 040812
		*pos = FloorToPos(origin);//shin 040812
		if (index == START_DELAY_PERIOD )
		{
			return TRUE;//No change in the motion state
		}
		if (index == STOP_DELAY_PERIOD)
		{
			*pos=FloorToPos(target);//Change only position
			return TRUE;
		}
		else
		{
			return FALSE;//add err log
		}
	}
	if(deltaT < 0.0)
	{
		int error=1;
	//	MessageBox("deltaT is less than 0!");//add err log
	}
	j = mp_jerk[index];
	*jerk = j;
	*accel = mp_accel[index] + j * deltaT;
	*speed = mp_speed[index]
		+ mp_accel[index] * deltaT
		+ j * pow(deltaT, 2.0)/2.0;
	*pos = mp_pos[index]
		+ mp_speed[index]*deltaT
		+ 0.5 * mp_accel[index] * pow(deltaT, 2.0)
		+ j * pow(deltaT, 3.0)/6.0;
	if(origin==target)
	{
		*pos=FloorToPos(target);
	}
	return TRUE;
}

//##ModelId=433BA15301AE
int CMotion::GetBreakPointTableIndex(double t)
{
	int i;
	int num;

	num = mp_n_break;
	if(num < 1)
		return -4;//no break table

	// check whether t is out of range in the break point table including delay time.
	if(t < mp_break[0]-StartDelay)
		return -3;//add err log
	
	if(t>mp_break[num-1]+StopDelay)
	{
		return STOP_DELAY_PERIOD;
	}
	if (t < mp_break[0] )
	{
		return START_DELAY_PERIOD;
	}
	
	if (t > mp_break[num-1])
	{
		return STOP_DELAY_PERIOD;
	}
	if (StopDelay == 0 && t == mp_break[num-1])
		return STOP_DELAY_PERIOD;
	
	//search index
	for(i=num-1; i>=0 ; i--)  // IN TABLE
	{
		if(mp_break[i] <= t)
			return i;
	}

	return -5;//fail, add err log
}

//##ModelId=433BA1530208
int CMotion::GetNextCommittableFloor(double time,int *into_decel, int should_be_in_flight)
{
	double deltaT;
	double t_into_run;
	double dist, committablePos;
	int committableFloor;
	double posOrg, posDest;//Position
	double maxJ, maxA, maxV;
	double minCommitDist;//miniml committable distance is inter-floor height
	double j, a, v, p;
	double vb; //velocity at the latest break point before current time
	int indexCT;// index of break point table at current time
	double tStep; // time at max accelerlation point from starting to run
	double tStep1;
	double vStep; // velocity at max accelerlation point from starting to run
	BREAK_TABLE b; // break table for getting next committable floor
	int i, ib;
	int carMovingDirection;

	posDest = 0;  // Added by CSW 2/7/2005, initialize the value of posDest
	dist = 0;     // Added by CSW 2/7/2005, initialize the value of posDest
	m_bParamHard=false;
	if(m_bParamHard)//shin 040914 two kinds of parameter
		return NextCommittableFloor(into_decel, should_be_in_flight);
	
	maxJ = this->MaxJerk;
	maxA = this->MaxAcceleration;
	maxV = this->MaxVelocity;

	*into_decel = 0;  // indicates if car is already decelerating into target
	double t=time/1000.0; //change to sec
	int index=GetBreakPointTableIndex(t);
	if(index>-1)
		deltaT=t-mp_break[index];
		
	else
	{
		if(index==START_DELAY_PERIOD)
		{
			return origin; //No change
		}
		if(index==STOP_DELAY_PERIOD)
		{
			return target;
		}
		else
		{
			return origin; //add err log
		}
	}
	{
		if(t - t_run_start > this->StartDelay)
			t_into_run = t -t_run_start - this->StartDelay;
		else if(t > t_run_start)
			t_into_run = t - t_run_start;
		else
			t_into_run = 0.0;

		tStep = maxA/maxJ;
		vStep = tStep * maxA / 2.0;
		indexCT = GetBreakPointTableIndex(t);
		posOrg = FloorToPos(origin);
		posDest = FloorToPos(target);

		//shin050822 Use moving direction to get next committable floor
		if(origin < target)
			carMovingDirection = UP;
		else if(origin > target)
			carMovingDirection = DN;
		//else
		//	carMovingDirection = NO_DIR;
		if(indexCT < mp_n_break && indexCT >= 0)//shin 041006
		{
			//shin050822
			//Current time is the time which motion ends.
			if(indexCT == mp_n_break - 1)
			{
				*into_decel = 1;
				return target;
			}
			b.accel[0] = mp_accel[0];
			b.velocity[0] = mp_speed[0];
			b.position[0] = mp_pos[0];
			for(i=0; i<=indexCT; i++)
			{
				b.time[i] = mp_break[i];
				b.jerk[i] = mp_jerk[i];
			}
			ComputeMotionState(mp_break[indexCT], &j, &a, &vb, &p);
			ComputeMotionState(t, &j, &a, &v, &p);
			if(carMovingDirection == UP)
			{
				if(a > maxA-PRECISION)
				{
					//case 2
					ib = indexCT+1;
					b.time[ib] = t;
					b.jerk[ib++] = -maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = -maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = 0.0;
					
					b.time[ib] = b.time[ib-1] + v/maxA - tStep/2.0;
					b.jerk[ib++] = maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = 0.0;
					
					b.num = ib;
				}
				else if(a > PRECISION)
				{
					//case 1, 13, 3, 12
					if(j > maxJ-PRECISION)
					{
						if(v < vStep)
						{
							// 1
							tStep1 = t - b.time[0];
							ib = indexCT+1;
							b.time[ib] = t;
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1;
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1;
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
						else
						{
							// 13
							tStep1 = t - b.time[indexCT];
							ib = indexCT+1;
							b.time[ib] = t;
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1 - (t-b.time[indexCT]);
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep/2.0;
							b.jerk[ib++] = 0.0;
							
							b.time[ib] = b.time[ib-1] + v/maxA - tStep/2.0;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep/2.0;
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
					}
					else if (j < -maxJ+PRECISION)
					{
						//3, 12
						ib = indexCT+1;
						b.time[ib] = t;
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep - (t-b.time[indexCT]);
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] + vb/maxA - tStep/2.0;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
					else
					{
						//Error
					}
				}
				else if(a > -PRECISION)
				{
					//case 4, 8
					if(v > maxV - PRECISION)
					{
						// 4
						ib = indexCT+1;
						b.time[ib] = t;
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] + v/maxA - tStep;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
					else
					{
						// 8
						ib = indexCT+1;
						b.time[ib] = t;
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] + v/maxA - tStep;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
				}
				else if(a > -maxA+PRECISION)
				{
					//case 5, 9, 11, 7
					if(j > maxJ-PRECISION)
					{
						if(v < vStep)
						{
							// 11
							ib = indexCT+1;
							b.time[ib] = t;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = mp_break[ib-1];
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
						else
						{
							// 7
							ib = indexCT+1;
							b.time[ib] = t;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep - (t-b.time[indexCT]);
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep;
							b.jerk[ib++] = 0.0;
							
							b.time[ib] = b.time[ib-1] + vb/maxA - 3*tStep/2.0;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep;
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
					}
					else if (j < -maxJ+PRECISION)
					{
						//5, 9
						ib = indexCT+1;
						b.time[ib] = t;
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep - (t-b.time[indexCT]);
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] + vb/maxA - tStep;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
					else
					{
						//Error
					}
				}
				else //a < -maxA+PRECISION
				{
					//case 6, 10
					ib = indexCT+1;
					b.time[ib] = t;
					b.jerk[ib++] = 0.0;
					
					//b.time[ib] = b.time[ib-1] + v/maxA - tStep/2.0;
					b.time[ib] = b.time[ib-2] + vb/maxA - tStep/2.0;//shin050822
					b.jerk[ib++] = maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = 0.0;
					
					b.num = ib;
				}
			}
			else if(carMovingDirection == DN)//Car[c].dir == DN
			{
				if(-a > maxA-PRECISION)
				{
					//case 2
					ib = indexCT+1;
					b.time[ib] = t;
					b.jerk[ib++] = maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = 0.0;
					
					b.time[ib] = b.time[ib-1] + -v/maxA - tStep/2.0;//shin051027
					b.jerk[ib++] = -maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = 0.0;
					
					b.num = ib;
				}
				else if(-a > PRECISION)
				{
					//case 1, 13, 3, 12
					if(-j > maxJ-PRECISION)
					{
						if(-v < vStep)
						{
							// 1
							tStep1 = t - b.time[0];
							ib = indexCT+1;
							b.time[ib] = t;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1;
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1;
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
						else
						{
							// 13
							tStep1 = t - b.time[indexCT];
							ib = indexCT+1;
							b.time[ib] = t;
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep1 - (t-b.time[indexCT]);
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep/2.0;
							b.jerk[ib++] = 0.0;
							
							b.time[ib] = b.time[ib-1] + -v/maxA - tStep/2.0;//shin051027
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep/2.0;
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
					}
					else if (-j < -maxJ+PRECISION)
					{
						//3, 12
						ib = indexCT+1;
						b.time[ib] = t;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep - (t-b.time[indexCT]);
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] - vb/maxA - tStep/2.0;//shin051027
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
					else
					{
						//Error
					}
				}
				else if(-a > -PRECISION)
				{
					//case 4, 8
					if(-v > maxV - PRECISION)
					{
						// 4
						ib = indexCT+1;
						b.time[ib] = t;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] + -v/maxA - tStep;//shin051027
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
					else
					{
						// 8
						ib = indexCT+1;
						b.time[ib] = t;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] + -v/maxA - tStep;//shin051027
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
				}
				else if(-a > -maxA+PRECISION)
				{
					//case 5, 9, 11, 7
					if(-j > maxJ-PRECISION)
					{
						if(-v < vStep)
						{
							// 11
							ib = indexCT+1;
							b.time[ib] = t;
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = mp_break[ib-1];
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
						else
						{
							// 7
							ib = indexCT+1;
							b.time[ib] = t;
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep - (t-b.time[indexCT]);
							b.jerk[ib++] = maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep;
							b.jerk[ib++] = 0.0;
							
							b.time[ib] = b.time[ib-1] - vb/maxA - 3*tStep/2.0;//shin051027
							b.jerk[ib++] = -maxJ;
							
							b.time[ib] = b.time[ib-1] + tStep;
							b.jerk[ib++] = 0.0;
							
							b.num = ib;
						}
					}
					else if (-j < -maxJ+PRECISION)
					{
						//5, 9
						ib = indexCT+1;
						b.time[ib] = t;
						b.jerk[ib++] = maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep - (t-b.time[indexCT]);
						b.jerk[ib++] = 0.0;
						
						b.time[ib] = b.time[ib-1] - vb/maxA - tStep;//shin051027
						b.jerk[ib++] = -maxJ;
						
						b.time[ib] = b.time[ib-1] + tStep;
						b.jerk[ib++] = 0.0;
						
						b.num = ib;
					}
					else
					{
						//Error
					}
				}
				else //-a < -maxA+PRECISION
				{
					//case 6, 10
					ib = indexCT+1;
					b.time[ib] = t;
					b.jerk[ib++] = 0.0;
					
					//b.time[ib] = b.time[ib-1] + -v/maxA + tStep/2.0;
					b.time[ib] = b.time[ib-2] + -vb/maxA - tStep/2.0;//shin050822
					b.jerk[ib++] = -maxJ;
					
					b.time[ib] = b.time[ib-1] + tStep;
					b.jerk[ib++] = 0.0;
					
					b.num = ib;
				}
			}
			GenerateProfile(&b);
			dist = fabs(b.position[b.num-1] - posOrg);
		}
		else if(indexCT == START_DELAY_PERIOD)
		{
			dist = 0.0;
		}
		else if(indexCT == STOP_DELAY_PERIOD)
		{
			*into_decel = 1;
			return target;
		}

		//shin 040730
		//correction by minCommitDist
		if(carMovingDirection == UP)
			minCommitDist = FloorToPos(origin+1) - posOrg;
		else if(carMovingDirection == DN)
			minCommitDist = posOrg - FloorToPos(origin-1);
		else
			minCommitDist = 0.0;

		if(dist < minCommitDist)			       // CSW 2/7/2005, dist cause run time error: being used without being defined
			dist = minCommitDist;

		if(carMovingDirection == UP)
			committablePos = posOrg + dist;
		else if(carMovingDirection == DN)
			committablePos = posOrg - dist;
		else
		{
			committablePos = posOrg;
		}

		if (dist >= fabs(posOrg - posDest))  // CSW 2/7/2005, posDest causing runtime error: being used without being defined
		{
			// shin051005 check decel started.
			// ManualTestImpact
				*into_decel = 1;
			return target;
		}
		else
		{
			committableFloor = CommittablePosToNCF(committablePos);// ManualTestImpact
			// ManualTestImpact
			if(committableFloor == target) 
				*into_decel = 1;
			return committableFloor;
		}
	}
}


int CMotion::GetESPPos(double time,int *into_decel,int should_be_in_flight)
{
	int index;//index of break point table (motion profile)
	double deltaT;
	double j;//jerk
	double t=time/1000.0; //change to sec
	if(origin==target)
		return FloorToPos(target);
	index = GetBreakPointTableIndex(t);
	if( index > -1)
		deltaT = t - mp_break[index];
	else
	{
		if (index == START_DELAY_PERIOD )
		{
			return FloorToPos(origin);//No change in the motion state
		}
		if (index == STOP_DELAY_PERIOD)
		{
			return FloorToPos(target);
		}
		else
		{
			return FloorToPos(origin);//add err log
		}
	}
	if(deltaT < 0.0)
	{
		int error=1;
	//	MessageBox("deltaT is less than 0!");//add err log
	}
	j = mp_jerk[index];
	double jerk = j;
	double accel = mp_accel[index] + j * deltaT;
	double speed = mp_speed[index]
		+ mp_accel[index] * deltaT
		+ j * pow(deltaT, 2.0)/2.0;
	double pos = mp_pos[index]
		+ mp_speed[index]*deltaT
		+ 0.5 * mp_accel[index] * pow(deltaT, 2.0)
		+ j * pow(deltaT, 3.0)/6.0;
	double nsp=pos+pow(speed,2.0)/(2*Afd);
	return (int)nsp;
}

//##ModelId=433BA15302C7
int CMotion::GetNextCommittablePos(double time,int *into_decel, int should_be_in_flight)
{
	int kindOfProfile;
	double temp1,temp2;
	double deltaT;
	double t_into_run;
	double dist, committablePos;
	int committableFloor;
	double posOrg, posDest;//Position
	double maxJ, maxA, maxV;
	double minCommitDist;//miniml committable distance is inter-floor height
	double j, a, v, p;
	double vb; //velocity at the latest break point before current time
	int indexCT;// index of break point table at current time
	double tStep; // time at max accelerlation point from starting to run
	double tStep1;
	double vStep; // velocity at max accelerlation point from starting to run
	BREAK_TABLE b; // break table for getting next committable floor
	int i, ib;
	int carMovingDirection;
	posDest = 0;  // Added by CSW 2/7/2005, initialize the value of posDest
	dist = 0;     // Added by CSW 2/7/2005, initialize the value of posDest
	m_bParamHard=false;
	if(m_bParamHard)//shin 040914 two kinds of parameter
		return NextCommittableFloor(into_decel, should_be_in_flight);
	
	maxJ = this->MaxJerk;
	maxA = this->MaxAcceleration;
	maxV = this->MaxVelocity;

	*into_decel = 0;  // indicates if car is already decelerating into target
	double t=time/1000.0; //change to sec

	int index = GetBreakPointTableIndex(t);
	if( index > -1)
		deltaT = t - mp_break[0];
	else
	{
		if (index == START_DELAY_PERIOD )
		{
			return FloorToPos(origin);//No change in the motion state
		}
		if (index == STOP_DELAY_PERIOD)
		{
			return FloorToPos(target);
		}
		else
		{
			return FloorToPos(origin);//add err log
		}
	}
	if(t - t_run_start > this->StartDelay)
		t_into_run = t-t_run_start - StartDelay;
	else if(t > t_run_start)
		t_into_run = t - t_run_start;
	else
		t_into_run = 0.0;

	tStep = maxA/maxJ;
	vStep = tStep * maxA / 2.0;
	indexCT = GetBreakPointTableIndex(t);
	posOrg = FloorToPos(origin);
	posDest = FloorToPos(target);
	dist = fabs(posOrg - posDest);

	//shin050822 Use moving direction to get next committable floor
	if(origin < target)
		carMovingDirection = UP;
	else if(origin > target)
		carMovingDirection = DN;


	//check the motion profile kind
	temp1 = (pow(maxA, 2.0)*maxV + pow(maxV, 2.0)*maxJ)/(maxJ*maxA);
	temp2 = 2.0*pow(maxA, 3.0)/pow(maxJ, 2.0);
	if(dist >= temp1)
		kindOfProfile = 1;
	else if(dist >= temp2)
		kindOfProfile = 2;
	else
		kindOfProfile = 3;

	if(kindOfProfile==3)
	{
		if(index==0)
		{
			committablePos=posOrg+carMovingDirection*2.0*maxJ*pow(deltaT,3.0);
		}
		else
			committablePos=posDest;
	}
	else if(kindOfProfile==2)
	{
		if(index<=0)
		{
			committablePos=posOrg+carMovingDirection*2.0*maxJ*pow(deltaT,3.0);
		}
		else if(index<2)
		{
			committablePos=posOrg+carMovingDirection*(maxA*pow(deltaT,2.0)+pow(maxA,2.0)*deltaT/maxJ);
		}
		else
		{
			committablePos=posDest;		
		}
	}
	else
	{
		if(index<=0)
		{
			committablePos=posOrg+carMovingDirection*2.0*maxJ*pow(deltaT,3.0);
		}
		else if(index<2)
		{
			committablePos=posOrg+carMovingDirection*(maxA*pow(deltaT,2.0)+pow(maxA,2.0)*deltaT/maxJ);
		}
		else if(index<3)
		{
			committablePos=posOrg+carMovingDirection*((pow(maxA,2.0)*maxV+pow(maxV,2.0)*maxJ)/(maxJ*maxA));
		}
		else if(index<4)
		{
			committablePos=posOrg+carMovingDirection*(maxV*deltaT);
		}
		else
			committablePos=posDest;
	}
	return (int)committablePos;
}


//##ModelId=433BA1540160
int CMotion::NextCommittableFloor(int *into_decel, int should_be_in_flight)
{
	return 0;
}

//##ModelId=433BA1530385
void CMotion::GenerateProfile(BREAK_TABLE *b)
{
	int i;
	double diff; //time difference

	for(i=1; i<b->num; i++)
	{
		diff = b->time[i] - b->time[i-1];
		b->accel[i] = b->accel[i-1] + b->jerk[i-1] * diff;
		b->velocity[i] = b->velocity[i-1]
			+ b->accel[i-1] * diff
			+ b->jerk[i-1] * pow(diff, 2.0) / 2.0;
		b->position[i] = b->position[i-1]
			+ b->velocity[i-1] * diff
			+ b->accel[i-1] * pow(diff, 2.0) / 2
			+ b->jerk[i-1] * pow(diff, 3.0) / 6;
	}
}

//##ModelId=433BA1540079
double CMotion::HardRunTime(int origin, int dest)
{
	return 0;
}
//Get the run time from an origin floor position to a destination floor position.
//##ModelId=433BA15303D5
double CMotion::GetRunTime(int origin, int dest) // return second
{
	double originPos, destPos;
	double dist;
	double j;//maxJerk
	double a;//maxAcceleration
	double v;//maxVelocity
	int kindOfProfile;//1:Car reaches full speed ,2: Car reaches full acceleration ,3: Car does not reach full speed or acceleration
	double temp1, temp2;
	double time;
	m_bParamHard=false;
	if(m_bParamHard)//shin 040914 two kinds of parameter
		return HardRunTime(origin, dest);
	
	j = MaxJerk;
	a = MaxAcceleration;
	v = MaxVelocity;

	originPos = FloorToPos(origin);
	destPos = FloorToPos(dest);
	dist = fabs(originPos - destPos);

	// If distance is zero, than delay time is excluded.
	// So it takes 0 second.
	if(dist < EPSILON)
		return 0.0;

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
	time += StartDelay + StopDelay;
	return time;
}

//##ModelId=433BA1540105
int CMotion::CommittablePosToNCF(double pos)
{
	int f;
	int carMovingDirection;
	
	//shin050822 Use moving direction to get next committable floor
	if(origin < target)
		carMovingDirection = UP;
	else if(origin > target)
		carMovingDirection = DN;

	if(carMovingDirection == UP)
	{
		for(f=0; f<n_floor; f++)
		{
			if(pos <=floorPosition[f])
				return f;
		}
	}
	else if(carMovingDirection == DN)
	{
		for(f=0; f<n_floor; f++)
		{
			if(pos < floorPosition[f])
				return (f-1);
		}
	}
	else
	{
		for(f=0; f<n_floor; f++)
		{
			if(pos < (floorPosition[f] + floorPosition[f+1])/2.0)
				return f;
		}
	}
	return -1;//Error
}

//##ModelId=433BA6F7038B
double CMotion::HardCommitTime(int s, int d)
{
	return 0;
}

double CMotion::GetNCP_T3(int s,int d)
{
	double dist = fabs(FloorToPos(s) - FloorToPos(d));
	double a,v,j;
	double temp1;
	j = MaxJerk;
	a = MaxAcceleration;
	v = MaxVelocity;

	temp1 = (pow(a, 2.0) * v + pow(v, 2.0) * j) / (j * a);
	if(dist<temp1)
		return -1;
	else
		return (a/j+v/a);
}

//Get committable time from source floor to destination floor
//##ModelId=433BA6F702FF
double CMotion::GetCommittableTime(int s, int d)
{
	double dist;
	double time;
	double j,a,v;
	double temp1, temp2;
	m_bParamHard=false;	
	if(m_bParamHard)//shin 040914 two kinds of parameter
		return HardCommitTime(s, d);

	dist = fabs(FloorToPos(s) - FloorToPos(d));

	j = MaxJerk;
	a = MaxAcceleration;
	v = MaxVelocity;

	temp1 = (pow(a, 2.0) * v + pow(v, 2.0) * j) / (j * a);
	temp2 = 2.0 * pow(a, 3.0) / pow(j, 2.0);
	if(dist >= temp1)
		time = dist/v;
	else if(dist >= temp2)
		time = -0.5*a/j + 0.5*pow(pow(a, 3.0) + 4*dist*pow(j, 2.0), 0.5)/(pow(a, 0.5)*j);
	else
		time = pow(4.0*dist/j, 1.0/3.0);

	time += StartDelay;
	return time;
}

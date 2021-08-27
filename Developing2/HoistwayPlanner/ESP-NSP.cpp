// ESP-NSP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ESP-NSP.h"
#include <math.h>
#include <stdio.h>

#ifdef WITH_DEBUG
FILE *output;
#endif
// This function return the ESP vs NSP delay based on the parameter
double ESPNSPDelay(double leadDist,			// lead car travel distance
				   double trailingDist,		// trailing car travel distance
				   double jerk,				// max jerk for both cars
				   double accel,			// max acceleration for both cars
				   double velocity,			// max velocity for both cars
				   double initSeparation,	// distance between the origin of the two cars
				   double MSD,				// minimum separation distance
				   double accelForced
				   )
{
#ifdef WITH_DEBUG
output =  fopen("ESP-NSP_debug.txt","w");
if (output == NULL)
	output = stdout;
#endif
	double maxDelay = -1.0 * INFINITE;			// set the delay to a large number
	double MSDTime;							// the time when the two curve separated exactly by MSD
	double MSDPos;							// the position on ESP where MSD occurs
	TrajectoryTableEntry ESPTable[8], NSPTable[8];
	DelayTableEntry delayTable[7];

//  Check conditions that does not need TSA Delay
	// If the trailing car travel distance plus minimum separation is less than initial separation, no need for TSA 
	if (initSeparation > trailingDist + MSD + ESP_NSP_PRECISION)
		return -1.0* INFINITE;

//  Check conditions that the profile comparison is infeasible, return INFINITE
	// if the origins of the  cars already violated MSD, cannot calculate delay
	if (initSeparation + ESP_NSP_PRECISION < MSD)
		return INFINITE;

	// if the destination of the  cars will violate MSD, cannot calculate delay
	if (initSeparation + leadDist - trailingDist + ESP_NSP_PRECISION < MSD)
		return INFINITE;

	CalculateESPTable(leadDist,jerk,accel, velocity, accelForced, ESPTable);
	CalculateNSPTable(trailingDist,jerk,accel, velocity, accelForced, NSPTable);

	// Given critical points on NSP, calculate necessary delays
	int numPoints = 0;
	double accelSQR = accel*accel;
	double velSQR = velocity*velocity;
	double jerkSQR = jerk*jerk;
	// if lift reaching full speed during journey
	if (trailingDist + ESP_NSP_PRECISION  > (accelSQR*velocity + velSQR*jerk)/(jerk*accel))
	{
		delayTable[0].iNSP = 1;
		delayTable[0].pNSP = NSPTable[1].TrajectoryPos;
		delayTable[0].tNSP = NSPTable[1].Time;

		delayTable[1].iNSP = 2;
		delayTable[1].pNSP = NSPTable[2].TrajectoryPos;
		delayTable[1].tNSP = NSPTable[2].Time;

		delayTable[2].iNSP = 4;
		delayTable[2].pNSP = NSPTable[4].TrajectoryPos;
		delayTable[2].tNSP = NSPTable[4].Time;

		numPoints = 3;
	}
	// if lift reaching full acceleration, but not full speed
	else if (trailingDist + ESP_NSP_PRECISION  > 2*accelSQR*accel /jerkSQR)
	{
		delayTable[0].iNSP = 1;
		delayTable[0].pNSP = NSPTable[1].TrajectoryPos;
		delayTable[0].tNSP = NSPTable[1].Time;

		delayTable[1].iNSP = 2;
		delayTable[1].pNSP = NSPTable[2].TrajectoryPos;
		delayTable[1].tNSP = NSPTable[2].Time;

		numPoints = 2;
	}
	// if lift not reaching full acceleration or full speed
	else if (trailingDist + ESP_NSP_PRECISION  <= 2*accelSQR*accel /jerkSQR)
	{
		delayTable[0].iNSP = 1;
		delayTable[0].pNSP = NSPTable[1].TrajectoryPos;
		delayTable[0].tNSP = NSPTable[1].Time;
		numPoints = 1;
		
	}


	for (int i=0; i<numPoints; i++)
	{
		double pos = delayTable[i].pNSP - initSeparation + MSD;

		// if the position is less than 0, than no delay is needed
		if (pos <= 0.0)
		{
			delayTable[i].pESP = 0;
			delayTable[i].tESP = -1.0* INFINITE;
			delayTable[i].iESP = 0;
			delayTable[i].tDelay = -1.0 * INFINITE;
			continue;
		}

		// if the position is larger than lead car travel distance, than the delay is infinite
		if (pos > leadDist + ESP_NSP_PRECISION)
		{
			delayTable[i].pESP = leadDist;
			delayTable[i].tESP = INFINITE;
			delayTable[i].iESP = 8;
			delayTable[i].tDelay = INFINITE;
			continue;
		}
		
		// find which interval of ESP the position is in
		for (int j=1; j<8; j++)
		{
			if (pos < ESPTable[j].TrajectoryPos + ESP_NSP_PRECISION)		// pos <= ESPTable[j].TrajectoryPos
				break;
		}
		delayTable[i].iESP = j;

		double dist = pos - ESPTable[j-1].CarPosition;		// all use car position as reference
		double interval = ESPTable[j].Time - ESPTable[j-1].Time;
		double J = ESPTable[j].Jerk;						// jerk in this period
		double A0 = ESPTable[j-1].Accel;						// acceleration in the previous period
		double V0 = ESPTable[j-1].Velocity;					// velocity in the previous period
		double a = J*J/(8*accelForced);
		double b = J/6 + J*A0/(2*accelForced);
		double c = (A0 + (A0*A0+J*V0)/accelForced)/2;
		double d = V0+ A0*V0/accelForced;
		double e = V0*V0/(2*accelForced) - dist;
		double t[4];
		int numRoots = QuarticEquationSolver(a,b,c,d,e,t);
		for (int k = 0; k<numRoots; k++)
		{
			if ( t[k] + ESP_NSP_PRECISION > 0.0 && t[k] < interval + ESP_NSP_PRECISION)
			// if ( t[k] + ESP_NSP_PRECISION > 0.0)
				break;
		}
		if (k < numRoots)		// if the root is within range of interest
		{
			double tSQR = t[k]*t[k];
			delayTable[i].tESP = ESPTable[j-1].Time + t[k];
			delayTable[i].pESP = a*tSQR*tSQR + b*tSQR*t[k] + c*tSQR + d*t[k] + e + dist + ESPTable[j-1].CarPosition;
			delayTable[i].tDelay = delayTable[i].tESP - delayTable[i].tNSP; 
		}
		else
		{
			delayTable[i].tESP = -1.0* INFINITE;
			delayTable[i].pESP = INFINITE;
			delayTable[i].tDelay = delayTable[i].tESP - delayTable[i].tNSP; 
		}
	}

	// From critical points on ESP, calculate the required delay for these points
	int prevIndex = numPoints;					// remember the previous index of critical point table
	int numESPPoints = 0;
	// if lift reaching full speed during journey
	if (leadDist + ESP_NSP_PRECISION  > (accelSQR*velocity + velSQR*jerk)/(jerk*accel))
	{
		numESPPoints = 3;
	}
	// if lift reaching full acceleration, but not full speed
	else if (leadDist + ESP_NSP_PRECISION  > 2*accelSQR*accel /jerkSQR)
	{
		numESPPoints = 2;
	}
	// if lift not reaching full acceleration or full speed
	else if (leadDist + ESP_NSP_PRECISION  <= 2*accelSQR*accel /jerkSQR)
	{	
		numESPPoints = 1;	
	}

	// check which points should be considered if lead car reach maximum speed
	if (leadDist + ESP_NSP_PRECISION  > (accelSQR*velocity + velSQR*jerk)/(jerk*accel))
	{
		double ll= NSPTable[3].TrajectoryPos + MSD - initSeparation;
		double ul = NSPTable[4].TrajectoryPos + MSD - initSeparation;
		for (i=0; i< numESPPoints; i++)
		{
			// if the ESP section coinside with constant speed region of NSP
			if ((ESPTable[i].TrajectoryPos > ll && ESPTable[i].TrajectoryPos < ul) || 
				(ESPTable[i+1].TrajectoryPos > ll && ESPTable[i+1].TrajectoryPos < ul))
			{
				// calculate the point where the first deritive of ESP equal velocity of trailing car
				double interval = ESPTable[i+1].Time - ESPTable[i].Time;
				double J = ESPTable[i+1].Jerk;						// jerk in this period
				double A0 = ESPTable[i].Accel;						// acceleration in the previous period
				double V0 = ESPTable[i].Velocity;					// velocity in the previous period
				double a = J*J/(2*accelForced);
				double b = J/2 + 3*J*A0/(2*accelForced);
				double c = A0 + (A0*A0+J*V0)/accelForced;
				double d = V0+ A0*V0/accelForced - velocity;
				double t[3];
				int numRoots = CubicEquationSolver(a,b,c,d,t);
				for (int k = 0; k<numRoots; k++)
				{
					if ( t[k] + ESP_NSP_PRECISION > 0.0 && t[k] < interval + ESP_NSP_PRECISION)
					// if ( t[k] + ESP_NSP_PRECISION > 0.0)
						break;
				}
				double tSQR = t[k]*t[k];
				double pESP = a*tSQR*tSQR/4 + b*tSQR*t[k]/3 + c*tSQR/2 + (V0+ A0*V0/accelForced)*t[k] + V0*V0/(2*accelForced) +  ESPTable[i].CarPosition;
				if (k < numRoots && pESP > ll && pESP < ul)				// if the position of ESP in range
				{
					delayTable[numPoints].tESP = ESPTable[i].Time + t[k];
					delayTable[numPoints].pESP = pESP;
					delayTable[numPoints].iESP = i+1;
					numPoints++;
				}
				else
				{
					delayTable[numPoints].tESP = ESPTable[i+1].Time; 
					delayTable[numPoints].pESP = ESPTable[i+1].TrajectoryPos;
					delayTable[numPoints].iESP = i+1;
					numPoints++;
				}
			}
		}
		
		// find correspoinding NSP position and delay
		for (i = prevIndex; i<numPoints; i++)
		{
			double pos = delayTable[i].pESP + initSeparation - MSD;
			delayTable[i].tNSP = pos/velocity;
			delayTable[i].iNSP = 4;
			delayTable[i].pNSP = pos;
			delayTable[i].tDelay = delayTable[i].tESP - delayTable[i].tNSP; 
		}
	}
	

	MSDTime = -1.0* INFINITE;
	for (i = 0; i<numPoints; i++)
	{
		if (delayTable[i].tDelay > maxDelay)
		{
			maxDelay = delayTable[i].tDelay;
			MSDTime = delayTable[i].tESP;
			MSDPos = delayTable[i].pESP;
		}
#ifdef WITH_DEBUG
		fprintf(output,"Critical Point %d, tESP = %.3f, pESP = %.3f, tNSP = %.3f, pNSP = %.3f, Delay %.3f\n", i+1, delayTable[i].tESP,
			delayTable[i].pESP, delayTable[i].tNSP, delayTable[i].pNSP, delayTable[i].tDelay);
#endif
	}

#ifdef WITH_DEBUG
		fprintf(output,"TSA Delay = %.3f", maxDelay);
		fclose(output);
#endif

	return maxDelay;
}

void CalculateESPTable(double dist,				// travel distance
					   double jerk,				// max jerk for both cars
					   double accel,			// max acceleration for both cars
					   double velocity,			// max velocity for both cars
					   double accelForced,		// forced acceleration when brake is dropped
					   TrajectoryTableEntry *table	// table to hold the trajectory entries
					   )
{
	double accelSQR = accel * accel;
	double velSQR = velocity * velocity;
	double jerkSQR = jerk*jerk;
	int i;
	double intervalLength;

	table[0].Accel= table[0].CarPosition = table[0].Jerk = 0.0;
	table[0].Time = table[0].TrajectoryPos = table[0].Velocity = 0.0;

	// if lift reaching full speed during journey
	if (dist + ESP_NSP_PRECISION  > (accelSQR*velocity + velSQR*jerk)/(jerk*accel))
	{
		// 0 <= t <= t1
		i = 1;
		table[i].Time = accel/jerk;
		table[i].Jerk = jerk;
		table[i].Accel = jerk * table[i].Time;
		table[i].Velocity = jerk*table[i].Time*table[i].Time/2;
		table[i].CarPosition = jerk*table[i].Time*table[i].Time *table[i].Time/6;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t1 <= t <= t2
		i = 2;
		table[i].Time = velocity/accel;
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = 0;
		table[i].Accel = accel;
		table[i].Velocity = -accelSQR/(2*jerk)+accel*table[i].Time;
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength + accel*intervalLength*intervalLength/2;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t2 <= t <= t3
		i = 3;
		table[i].Time = table[1].Time + table[2].Time;
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = -jerk;
		table[i].Accel = accel - jerk*intervalLength;
		table[i].Velocity = table[i-1].Velocity + accel*intervalLength - jerk*intervalLength*intervalLength/2;
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength + accel*intervalLength*intervalLength/2
			-jerk*intervalLength*intervalLength*intervalLength/6;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t3 <= t <= t4
		i = 4;
		table[i].Time = dist/velocity;
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = 0;
		table[i].Accel = 0;
		table[i].Velocity = velocity;
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength;
		table[i].TrajectoryPos = table[i].CarPosition + velSQR/(2*accelForced);

		// t4 <= t <= t5
		i = 5;
		table[i].Time = table[4].Time + table[1].Time;
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = -jerk;
		table[i].Accel =  - jerk*intervalLength;
		table[i].Velocity = velocity - jerk*intervalLength*intervalLength/2;
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength 
			-jerk*intervalLength*intervalLength*intervalLength/6;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t5 <= t <= t6
		i = 6;
		table[i].Time = table[4].Time + table[2].Time;
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = 0;
		table[i].Accel = -accel;
		table[i].Velocity = table[i-1].Velocity - accel*intervalLength; 
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength - accel*intervalLength*intervalLength/2;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t6 <= t <= t7
		i = 7;
		table[i].Time = table[5].Time + table[2].Time;
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = jerk;
		table[i].Accel = - accel + jerk*intervalLength;
		table[i].Velocity = table[i-1].Velocity - accel*intervalLength + jerk*intervalLength*intervalLength/2;
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength - accel*intervalLength*intervalLength/2
			+ jerk*intervalLength*intervalLength*intervalLength/6;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);
	}
	// if lift reaching full acceleration, but not full speed
	else if (dist + ESP_NSP_PRECISION  > 2*accelSQR*accel /jerkSQR)
	{
		double accelSQRT = sqrt(accel);
		double a34dj2SQRT = sqrt(accelSQR*accel+4*dist*jerkSQR);
		// 0 <= t <= t1
		i = 1;
		table[i].Time = accel/jerk;
		table[i].Jerk = jerk;
		table[i].Accel = jerk * table[i].Time;
		table[i].Velocity = jerk*table[i].Time*table[i].Time/2;
		table[i].CarPosition = jerk*table[i].Time*table[i].Time *table[i].Time/6;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t1 <= t <= t2
		i = 2;
		table[i].Time = -accel/(2*jerk) + a34dj2SQRT/(2*jerk*accelSQRT);
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = 0;
		table[i].Accel = accel;
		table[i].Velocity = -accelSQR/(2*jerk)+accel*table[i].Time;
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength + accel*intervalLength*intervalLength/2;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t2 <= t <= t3
		i = 3;
		table[i].Time = accel/(2*jerk) + a34dj2SQRT/(2*jerk*accelSQRT);
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = -jerk;
		table[i].Accel = accel - jerk*intervalLength;
		table[i].Velocity = table[i-1].Velocity + accel*intervalLength - jerk*intervalLength*intervalLength/2;
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength + accel*intervalLength*intervalLength/2
			-jerk*intervalLength*intervalLength*intervalLength/6;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t3 <= t <= t4
		i = 4;
		table[i].Time = 3*accel/(2*jerk) + a34dj2SQRT/(2*jerk*accelSQRT);
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = -jerk;
		table[i].Accel =  - jerk*intervalLength;
		table[i].Velocity = table[i-1].Velocity - jerk*intervalLength*intervalLength/2;
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength 
			-jerk*intervalLength*intervalLength*intervalLength/6;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t4 <= t <= t5
		i = 5;
		table[i].Time = a34dj2SQRT/(jerk*accelSQRT);
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = 0;
		table[i].Accel = -accel;
		table[i].Velocity = table[i-1].Velocity - accel*intervalLength; 
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength - accel*intervalLength*intervalLength/2;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t5 <= t <= t6
		i = 6;
		table[i].Time = accel/jerk + a34dj2SQRT/(jerk*accelSQRT);
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = jerk;
		table[i].Accel = - accel + jerk*intervalLength;
		table[i].Velocity = table[i-1].Velocity - accel*intervalLength + jerk*intervalLength*intervalLength/2;
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength - accel*intervalLength*intervalLength/2
			+ jerk*intervalLength*intervalLength*intervalLength/6;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t6 <= t <= t7
		i = 7;
		table[i].Time = table[i-1].Time;
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = 0;
		table[i].Accel = table[i-1].Accel;
		table[i].Velocity = table[i-1].Velocity;
		table[i].CarPosition = table[i-1].CarPosition;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;
	}
	// if lift not reaching full acceleration or full speed
	else if (dist + ESP_NSP_PRECISION  <= 2*accelSQR*accel /jerkSQR)
	{
		// 0 <= t <= t1
		i = 1;
		table[i].Time = pow(dist/(2*jerk),1.0/3);
		table[i].Jerk = jerk;
		table[i].Accel = jerk * table[i].Time;
		table[i].Velocity = jerk*table[i].Time*table[i].Time/2;
		table[i].CarPosition = jerk*table[i].Time*table[i].Time *table[i].Time/6;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t1 <= t <= t2
		i = 2;
		table[i].Time = pow(4*dist/jerk,1.0/3);
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = -jerk;
		table[i].Accel = table[i-1].Accel - jerk*intervalLength;
		table[i].Velocity = table[i-1].Velocity + table[i-1].Accel*intervalLength - jerk*intervalLength*intervalLength/2;
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength + accel*intervalLength*intervalLength/2
			-jerk*intervalLength*intervalLength*intervalLength/6;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t2 <= t <= t3
		i = 3;
		table[i].Time = pow(27*dist/(2*jerk),1.0/3);
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = -jerk;
		table[i].Accel =  - jerk*intervalLength;
		table[i].Velocity = table[i-1].Velocity - jerk*intervalLength*intervalLength/2;
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength 
			-jerk*intervalLength*intervalLength*intervalLength/6;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t3 <= t <= t4
		i = 4;
		table[i].Time = pow(32*dist/jerk,1.0/3);
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = jerk;
		table[i].Accel = - accel + jerk*intervalLength;
		table[i].Velocity = table[i-1].Velocity + table[i-1].Accel*intervalLength + jerk*intervalLength*intervalLength/2;
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength - accel*intervalLength*intervalLength/2
			+ jerk*intervalLength*intervalLength*intervalLength/6;
		table[i].TrajectoryPos = table[i].CarPosition + table[i].Velocity*table[i].Velocity/(2*accelForced);

		// t4 <= t <= t5
		i = 5;
		table[i].Time = table[i-1].Time;
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = 0;
		table[i].Accel = table[i-1].Accel;
		table[i].Velocity = table[i-1].Velocity;
		table[i].CarPosition = table[i-1].CarPosition;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;

		// t5 <= t <= t6
		i = 6;
		table[i].Time = table[i-1].Time;
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = table[i-1].Jerk;
		table[i].Accel = table[i-1].Accel;
		table[i].Velocity = table[i-1].Velocity;
		table[i].CarPosition = table[i-1].CarPosition;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;

		// t6 <= t <= t7
		i = 7;
		table[i].Time = table[i-1].Time;
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Jerk = table[i-1].Jerk;
		table[i].Accel = table[i-1].Accel;
		table[i].Velocity = table[i-1].Velocity;
		table[i].CarPosition = table[i-1].CarPosition;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;
	}


#ifdef WITH_DEBUG
	fprintf(output,"ESP Intervals\n");
	for (i=0; i<8; i++)
	{
		fprintf(output, "i=%d, time= %.3f, j = %.1f, a = %.3f, v = %.3f, p = %.3f, esp = %.3f\n", i, table[i].Time, 
			table[i].Jerk, table[i].Accel, table[i].Velocity, table[i].CarPosition, table[i].TrajectoryPos);
	}
#endif
}

void CalculateNSPTable(double dist,				// travel distance
					   double jerk,				// max jerk for both cars
					   double accel,			// max acceleration for both cars
					   double velocity,			// max velocity for both cars
					   double accelForced,		// forced acceleration when brake is dropped
					   TrajectoryTableEntry *table	// table to hold the trajectory entries
					   )
{
	double accelSQR = accel * accel;
	double velSQR = velocity * velocity;
	double jerkSQR = jerk*jerk;
	int i;
	double intervalLength;

	table[0].Accel= table[0].CarPosition = table[0].Jerk = 0.0;
	table[0].Time = table[0].TrajectoryPos = table[0].Velocity = 0.0;

	// if lift reaching full speed during journey
	if (dist + ESP_NSP_PRECISION  > (accelSQR*velocity + velSQR*jerk)/(jerk*accel))
	{
		// 0 <= t <= t1
		i = 1;
		table[i].Time = accel/jerk;
		table[i].Jerk = jerk;
		table[i].TrajectoryPos = 2*jerk*table[i].Time*table[i].Time*table[i].Time;

		// t1 <= t <= t2
		i = 2;
		table[i].Time = velocity/accel;
		table[i].Jerk = 0;
		table[i].TrajectoryPos = accel*table[i].Time*table[i].Time + accelSQR*table[i].Time/jerk;

		// t2 <= t <= t3
		i = 3;
		table[i].Time = table[1].Time + table[2].Time;
		table[i].Jerk = -jerk;
		table[i].TrajectoryPos = (accelSQR*velocity+velSQR*jerk)/(jerk*accel);

		// t3 <= t <= t4
		i = 4;
		table[i].Time = dist/velocity;
		table[i].Jerk = 0;
		table[i].TrajectoryPos = velocity*table[i].Time;

		// t4 <= t <= t5
		i = 5;
		table[i].Time = table[4].Time + table[1].Time;
		table[i].Jerk = -jerk;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;

		// t5 <= t <= t6
		i = 6;
		table[i].Time = table[4].Time + table[2].Time;
		table[i].Jerk = 0;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;

		// t6 <= t <= t7
		i = 7;
		table[i].Time = table[5].Time + table[2].Time;
		table[i].Jerk = jerk;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;
	}
	// if lift reaching full acceleration, but not full speed
	else if (dist + ESP_NSP_PRECISION  > 2*accelSQR*accel /jerkSQR)
	{
		double accelSQRT = sqrt(accel);
		double a34dj2SQRT = sqrt(accelSQR*accel+4*dist*jerkSQR);
		// 0 <= t <= t1
		i = 1;
		table[i].Time = accel/jerk;
		table[i].Jerk = jerk;
		table[i].TrajectoryPos = 2*jerk*table[i].Time*table[i].Time*table[i].Time;

		// t1 <= t <= t2
		i = 2;
		table[i].Time = -accel/(2*jerk) + a34dj2SQRT/(2*jerk*accelSQRT);
		table[i].Jerk = 0;
		table[i].TrajectoryPos = accel*table[i].Time*table[i].Time + accelSQR*table[i].Time/jerk;

		// t2 <= t <= t3
		i = 3;
		table[i].Time = accel/(2*jerk) + a34dj2SQRT/(2*jerk*accelSQRT);
		table[i].Jerk = -jerk;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;

		// t3 <= t <= t4
		i = 4;
		table[i].Time = 3*accel/(2*jerk) + a34dj2SQRT/(2*jerk*accelSQRT);
		table[i].Jerk = -jerk;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;

		// t4 <= t <= t5
		i = 5;
		table[i].Time = a34dj2SQRT/(jerk*accelSQRT);
		table[i].Jerk = 0;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;

		// t5 <= t <= t6
		i = 6;
		table[i].Time = accel/jerk + a34dj2SQRT/(jerk*accelSQRT);
		table[i].Jerk = jerk;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;

		// t6 <= t <= t7
		i = 7;
		table[i].Time = table[i-1].Time;
		table[i].Jerk = 0;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;
	}
	// if lift not reaching full acceleration or full speed
	else if (dist + ESP_NSP_PRECISION  <= 2*accelSQR*accel /jerkSQR)
	{
		// 0 <= t <= t1
		i = 1;
		table[i].Time = pow(dist/(2*jerk),1.0/3);
		table[i].Jerk = jerk;
		table[i].TrajectoryPos = 2*jerk*table[i].Time*table[i].Time*table[i].Time;

		// t1 <= t <= t2
		i = 2;
		table[i].Time = pow(4*dist/jerk,1.0/3);
		table[i].Jerk = -jerk;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;

		// t2 <= t <= t3
		i = 3;
		table[i].Time = pow(27*dist/(2*jerk),1.0/3);
		table[i].Jerk = -jerk;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;

		// t3 <= t <= t4
		i = 4;
		table[i].Time = pow(32*dist/jerk,1.0/3);
		table[i].Jerk = jerk;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;

		// t4 <= t <= t5
		i = 5;
		table[i].Time = table[i-1].Time;
		table[i].Jerk = 0;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;

		// t5 <= t <= t6
		i = 6;
		table[i].Time = table[i-1].Time;
		table[i].Jerk = table[i-1].Jerk;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;

		// t6 <= t <= t7
		i = 7;
		table[i].Time = table[i-1].Time;
		table[i].Jerk = table[i-1].Jerk;
		table[i].TrajectoryPos = table[i-1].TrajectoryPos;
	}

	for (i=1; i<8; i++)
	{
		intervalLength = table[i].Time - table[i-1].Time;
		table[i].Accel = table[i-1].Accel + table[i].Jerk*intervalLength;
		table[i].Velocity = table[i-1].Velocity + table[i-1].Accel*intervalLength + table[i].Jerk*intervalLength*intervalLength/2;
		table[i].CarPosition = table[i-1].CarPosition + table[i-1].Velocity*intervalLength + 
			table[i-1].Accel*intervalLength*intervalLength/2 + table[i].Jerk*intervalLength*intervalLength*intervalLength/6;
	}


#ifdef WITH_DEBUG
	fprintf(output,"NSP Intervals\n");
	for (i=0; i<8; i++)
	{
		fprintf(output, "i=%d, time= %.3f, j = %.1f, a = %.3f, v = %.3f, p = %.3f, nsp = %.3f\n", i, table[i].Time, 
			table[i].Jerk, table[i].Accel, table[i].Velocity, table[i].CarPosition, table[i].TrajectoryPos);
	}
#endif
}


//****************************************************************************************************
//* Quadratic Equation Solver. Solve the equation ax^2 + bx + c = 0  
//* return the number of real roots. Roots are sored in x1 and x2. If the roots are imaginary number,
//* returns 0. 
//****************************************************************************************************
int QuadraticEquationSolver(double a,
							double b,
							double c,
							double x[])
{
	// return 0 if infeasible
	if ( fabs(a)< ESP_NSP_PRECISION && fabs(b)< ESP_NSP_PRECISION )		// if a == 0 && b = 0, no solution
		return 0;
	// if 1st degree, solve immediately
	else if (fabs(a)< ESP_NSP_PRECISION )		// if a == 0 
	{
		x[0] = -c/b;
		return 1;
	}

	// quadratic equation solver
	double b24ac = b*b-4*a*c;
	
	if (b24ac < 0.0)
		return 0;
	else if (fabs(b24ac) <= ESP_NSP_PRECISION)		// b24ac == 0
	{
		x[0] = x[1] = -b/(2*a);
		return 1;			// the two roots are equal, return 1 the the calling function only process the root once
	}
	else
	{
		x[0] = (-b+sqrt(b24ac))/(2*a);
		x[1] = (-b-sqrt(b24ac))/(2*a);
		return 2;
	}
}

//*******************************************************************************************************
//* Cubic Equation Solver. Solve the equation ax^3 + bx^2 + cx+ d  = 0  
//* return the number of real roots. Roots are sored in x array. x must have at least three entries
//* The funcition returns the number of real roots. 
//*******************************************************************************************************
int CubicEquationSolver(double a,
						double b,
						double c,
						double d,
						double x[])
{
	// if less than 3rd degree, run the guadratic equation solver
	if (fabs(a) <= ESP_NSP_PRECISION)
		return QuadraticEquationSolver(b,c,d,x);

	// if d == 0, solve 2nd degree equation
	if (fabs(d) <= ESP_NSP_PRECISION)
	{
		int numRoots;
		numRoots = QuadraticEquationSolver(a,b,c,x);
		x[numRoots++] = 0.0;
		return numRoots;
	}

	double p = (3*a*c - b*b)/(9*a*a);
	double q = (2*b*b*b - 9*a*b*c+27*a*a*d)/(27*a*a*a);
	double dis = 4*p*p*p+q*q;

	if (dis > 0)
	{		
		double alpha = (-q + sqrt(dis))/2;
		int sgnAlpha = (alpha >= 0 ? 1 : -1);
		// *** The equation on "Algebra" Chpater 2, section 2.2.2 (page 82) is WRONG!!!!!
		// *** the following is correct, not the  beta = (- q - sqrt(dis))/2 printed on the book
		double beta = (q + sqrt(dis))/2;
		int sgnBeta = (beta >= 0 ? 1 : -1);
		x[0] = sgnAlpha*pow(fabs(alpha),1.0/3.0) - sgnBeta*pow(fabs(beta), 1.0/3.0) - b/(3*a);
		return 1;
	}
	else if (p==0)
	{
		x[0] = x[1] = x[2] = -b/(3*a);
		return 1;				// there are three roots, return 1 so the function only need to check 1 of the root
	}
	else 
	{
		double r = sqrt(-p*p*p);
		double theta = acos(-q/(2*r));
		int sgnR = (r >= 0 ? 1 : -1); 
		double rCBT = sgnR*pow(fabs(r), 1.0/3.0);

		x[0] = 2*rCBT*cos(theta/3.0)-b/(3*a);
		x[1] = 2*rCBT*cos((theta+2*PI)/3.0)-b/(3*a);
		x[2] = 2*rCBT*cos((theta+4*PI)/3.0)-b/(3*a);
		return 3;
	}
}


//*******************************************************************************************************
//* Cubic Equation Solver. Solve the equation ax^4 + bx^3 + cx^2+ dx + e  = 0  
//* return the number of real roots. Roots are storred in x array. x must have least 4 entries
//*******************************************************************************************************
int QuarticEquationSolver(double a,	
						  double b,
						  double c,
						  double d,
						  double e,
						  double x[])
{
	int numRoots = 0;

	// if 3rd degree equation, call cubic equation solver
	if (fabs(a) <= ESP_NSP_PRECISION)
	{
		return CubicEquationSolver(b,c,d,e,x);
	}

	if (fabs(e) <= ESP_NSP_PRECISION)
	{
		numRoots = CubicEquationSolver(a,b,c,d,x);
		x[numRoots++] = 0.0;
		return numRoots;
	}

	double aSQR = a*a;
	double bSQR = b*b;
	double p = (8*a*c-3*bSQR)/(8*aSQR);
	double q = (b*bSQR -4*a*b*c + 8*aSQR*d)/(8*a*aSQR);
	double r = (16*a*bSQR*c+256*a*aSQR*e-3*bSQR*bSQR - 64*aSQR*b*d)/(256*aSQR*aSQR);

	double vT[3], y[2];
	int numT;
	int j;

	if (fabs(q) < ESP_NSP_PRECISION)			// if q == 0, solve y^4 + py^2 + r
	{
		int numY = QuadraticEquationSolver(1,p,r,y);
		for (int i=0; i<numY; i++)
		{
			if (y[i] >= 0.0)
			{
				double ySQRT = sqrt(y[i]);
				x[numRoots++] = ySQRT -b/(4*a);
				x[numRoots++] = ySQRT - b/(4*a);
			}
		}
		return numRoots;
	}


	numT = CubicEquationSolver(1,-p,-4*r,4*p*r-q*q,vT); 
	for (int i=0; i<numT; i++)
	{
		double u_p = vT[i] - p;
		if (u_p + ESP_NSP_PRECISION > 0.0)  // if (u_P >= 0)
		{
			double u_pSQRT = sqrt(u_p);
			int numY; 
			if (fabs(u_p) > ESP_NSP_PRECISION)  // if u_p != 0
				numY= QuadraticEquationSolver(1,u_pSQRT, -u_pSQRT*q/(2*u_p)+vT[i]/2, y);
			else
				numY = 0;

			for (j=0; j<numY; j++)
			{
				double xVal = y[j]-b/(4*a);
				int k;
				// Do not record duplicated root
				for (k = 0; k< numRoots; k++)
				{
					if (fabs(xVal - x[k]) < ESP_NSP_PRECISION)
						break;
				}
				if (k >= numRoots)
					x[numRoots++] = xVal;
			}
			if (fabs(u_p) > ESP_NSP_PRECISION)	// if u_p != 0
				numY= QuadraticEquationSolver(1,-u_pSQRT, u_pSQRT*q/(2*u_p)+vT[i]/2, y);
			else
				numY = 0;
			
			for (j=0; j<numY; j++)
			{
				double xVal = y[j]-b/(4*a);
				int k;
				// Do not record duplicated root
				for (k = 0; k< numRoots; k++)
				{
					if (fabs(xVal - x[k]) < ESP_NSP_PRECISION)
						break;
				}
				if (k >= numRoots)
					x[numRoots++] = xVal;
			}
		}
	}

	return numRoots;
}
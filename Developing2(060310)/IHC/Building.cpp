// Building.cpp: implementation of the CBuilding class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\2dsim_defs.h"
#include "..\hoist_exec.h"
#include "Building.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuilding::CBuilding()
{

}

CBuilding::~CBuilding()
{

}

//DEL int CBuilding::GetHoistWayNo(int car)
//DEL {
//DEL 	return 0;
//DEL }

NEXT_STATE_T CBuilding::GetNextState(double currentTime, int car)
{
	int hoistInx=this->GetHoistWayInx(car);
	bool isLowerCar=this->IsLowerCar(car);
	hoistWay[hoistInx].UpdateNext(currentTime);
	return (isLowerCar==TRUE) ? this->hoistWay[noHoistWay].lowerNext : this->hoistWay[noHoistWay].upperNext;
}


int CBuilding::GetHoistWayInx(int car)
{
	return int(car/2);
}

void CBuilding::SetUpHoistWay(int no_hoistway, int no_carOfHoist)
{
	noHoistWay=no_hoistway;
	if(no_carOfHoist==2)
	{
		for(int i=0;i<MAXCAR;i++)
		{
			if(i%2==0)
				this->ocss_info[i].otherCar=(CCar*)&ocss_info[i+1];
			else
				this->ocss_info[i].otherCar=(CCar*)&ocss_info[i-1];
		}
	}
	for(int i=0;i<MAXHWY;i++)
	{
		if(i<noHoistWay)
			this->hoistWay[i].isAvailable=TRUE;
		else
			this->hoistWay[i].isAvailable=FALSE;
	}
}

bool CBuilding::IsLowerCar(int car)
{
	return (car%2==0) ? TRUE : FALSE;
}

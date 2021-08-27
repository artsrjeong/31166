// Building.h: interface for the CBuilding class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUILDING_H__0C33C508_84EE_4182_B9F6_8D14E566DF1C__INCLUDED_)
#define AFX_BUILDING_H__0C33C508_84EE_4182_B9F6_8D14E566DF1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Car.h"

class CBuilding  
{
public:
	CCar ocss_info[MAXCAR];
	bool IsLowerCar(int car);
	int noHoistWay;
	void SetUpHoistWay(int no_hoistway,int no_carOfHoist);
	int GetHoistWayInx(int car);
	CHoistWay hoistWay[MAXHWY];
	NEXT_STATE_T GetNextState(double currentTime,int car);
	CBuilding();
	virtual ~CBuilding();
};

#endif // !defined(AFX_BUILDING_H__0C33C508_84EE_4182_B9F6_8D14E566DF1C__INCLUDED_)

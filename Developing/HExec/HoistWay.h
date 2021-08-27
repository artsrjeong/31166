// HoistWay.h: interface for the CHoistWay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOISTWAY_H__3843958F_6441_4AAC_A71B_FC05F28C615C__INCLUDED_)
#define AFX_HOISTWAY_H__3843958F_6441_4AAC_A71B_FC05F28C615C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHoistWay  
{
public:
	void UpdateNext(double currentTime);
	CAR_STATE upperCar,lowerCar;
	NEXT_STATE_T upperNext,lowerNext;
	int upperCurInx,lowerCurInx;
	int upperNextInx,lowerNextInx;
	StopSequence upperSqc,lowerSqc;	
	StopSequence sequences[MAX_CARS_IN_HOISTWAY];
	bool isAvailable;
	CHoistWay();
	virtual ~CHoistWay();
};

#endif // !defined(AFX_HOISTWAY_H__3843958F_6441_4AAC_A71B_FC05F28C615C__INCLUDED_)

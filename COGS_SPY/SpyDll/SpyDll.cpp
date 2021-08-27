// SpyDll.cpp : DLL 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "..\..\Developing\COGS\2dsim_defs.h"
#include "..\..\Developing\COGS\2DInterfaceData.h"

BuildingInfo	BdInfo;
HoistwayInfo	HwyInfo[MAXHWY];
CarInfo			GCarInfo[MAX_CAR];
Parameter		Param;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

struct SPY_BuildingInfo				// static building information 
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
	FLOOR_T	nLandings;					// number of landings, including pit, overhead, virtual stops in express zone
	int	f_offset;		// floor offset for logical floor to numerical floor(pit exist then -1, else 0)
};

struct SPY_HoistwayInfo				// static hoistway information per hoistway
{
	BOOL	bPit;				// true means pit exists
	BOOL	bOverhead;			// true means overhead exists
	UCHAR	nCars;				// number of cars in this hoistway
	CHAR	idLowerCar;			// index of lower car in the hoistway, -1 represent no lower car
	CHAR	idUpperCar;			// index of upper car in the hoistway, -1 represent no upper car
};
// e.g., HoistwayInfo hoistway_info[MAX_HOISTWAY];		

struct SPY_CarInfo						// static information per car
{
	USHORT  Afd;					// forced deceleration
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
};
// e.g., CarInfo car_info[MAX_CAR];			

struct SPY_Parameter						// static global parameters		
{
	TIME_T tCarMinDwell;				
	TIME_T tHallMinDwell;
	TIME_T tHallMaxDwell;
	TIME_T tLobbyDwell;
	TIME_T tBoardingDeboardingTime;
	BOOL bReopenEnabled;				// true means reopen is enabled
	ULONG A1S,A2S,A1W,A2W,W,V;			// parameter for scoring
	CHAR GDAlgorithmID;					// algorithm ID for group dispatching
};


typedef void (*HE_HPEInitializeDLL)(BuildingInfo *mBdInfo,HoistwayInfo mHwyInfo[],CarInfo mCarInfo[],Parameter *mParam,char* version);

typedef void (*HE_SetTSACase)(int car,int origin,int target,int delay);

typedef void (*HE_MotionCmd)(int car,int origin,int target, int delay, int jerk, int acc, int vel);

typedef int (*HE_GetMotionValue)(int car,int time,int type);

typedef int (*HE_GetCommittableTime)(int car,int s, int d);

typedef int (*HE_GetNCP_T3)(int car,int s, int d);

typedef int (*HE_GetTSADelay)(int orgLead,int tgtLead,int orgTrail,int tgtTrail,int minSep);


HE_HPEInitializeDLL HPEInitializeDLL;
HE_SetTSACase SetTSACase;
HE_GetMotionValue GetMotionValue;
HE_GetCommittableTime GetCommittableTime;
HE_GetNCP_T3 GetNCP_T3;
HE_GetTSADelay GetTSADelay;
HE_MotionCmd MotionCmd;

HINSTANCE hinstCogs=NULL;
HINSTANCE hinstDispatcher=NULL;
HINSTANCE hinstHP=NULL;
HINSTANCE hinstHE=NULL;

bool LinkHE(char* filename)
{
	bool bHEAttached=TRUE;

	if (hinstHE != NULL)
		FreeLibrary(hinstHE);
	hinstHE=LoadLibrary(filename);
	if (hinstHE!=NULL)
	{
		HPEInitializeDLL=(HE_HPEInitializeDLL)GetProcAddress(hinstHE,"HPEInitializeDLL");
		if (HPEInitializeDLL==NULL)
		{
			bHEAttached=FALSE;
		}
		SetTSACase=(HE_SetTSACase)GetProcAddress(hinstHE,"SetTSACase");
		if (SetTSACase==NULL)
		{
			bHEAttached=FALSE;
		}
		GetMotionValue=(HE_GetMotionValue)GetProcAddress(hinstHE,"GetMotionValue");
		if (GetMotionValue==NULL)
		{
			bHEAttached=FALSE;
		}
		GetCommittableTime=(HE_GetCommittableTime)GetProcAddress(hinstHE,"GetCommittableTime");
		if(GetMotionValue==NULL)
		{
			bHEAttached=FALSE;
		}
		GetNCP_T3=(HE_GetNCP_T3)GetProcAddress(hinstHE,"GetNCP_T3");
		if(GetNCP_T3==NULL)
		{
			bHEAttached=FALSE;
		}
		GetTSADelay=(HE_GetTSADelay)GetProcAddress(hinstHE,"GetTSADelay");
		if(GetTSADelay==NULL)
		{
			bHEAttached=FALSE;
		}
		MotionCmd=(HE_MotionCmd)GetProcAddress(hinstHE,"MotionCmd");
		if(MotionCmd==NULL)
		{
			bHEAttached=FALSE;
		}
	}
	else
		bHEAttached=FALSE;

	if (bHEAttached==FALSE)
	{
		if (hinstHE!=NULL)
			FreeLibrary(hinstHE);
	}
	return bHEAttached;
}


extern "C" 
{

__declspec(dllexport) int SPY_GetCommittableTime(int car,int s, int d)
{
	return (int)GetCommittableTime(car,s,d);
}

__declspec(dllexport) int SPY_GetNCP_T3(int car,int s,int d)
{
	return (int)GetNCP_T3(car,s,d);
}

__declspec(dllexport) void SPY_SetBuilding(SPY_BuildingInfo *mBdInfo,SPY_Parameter *mParam)
{
	// shs0712 insert specification copy and initialization of Call DB	
	int hwy,car;
	BdInfo.nLandings=mBdInfo->nLandings;
	hwy=BdInfo.nHoistways=mBdInfo->nHoistways;
	car=BdInfo.nCars=mBdInfo->nCars;
}

__declspec(dllexport) void SPY_SetHoistWay(int hwy,SPY_HoistwayInfo *mHwyInfo)
{
		HwyInfo[hwy].idLowerCar=mHwyInfo->idLowerCar;
		HwyInfo[hwy].idUpperCar=mHwyInfo->idUpperCar;
}

__declspec(dllexport) void SPY_SetCar(int car,SPY_CarInfo *mCarInfo)
{
		GCarInfo[car].Afd=mCarInfo->Afd;
		GCarInfo[car].maxAcceleration=mCarInfo->maxAcceleration;
		GCarInfo[car].maxJerk=mCarInfo->maxJerk;
		GCarInfo[car].maxVelocity=mCarInfo->maxVelocity;
		GCarInfo[car].tStartDelay=mCarInfo->tStartDelay;
		GCarInfo[car].tStopDelay=mCarInfo->tStopDelay;
}

__declspec(dllexport) void SPY_HPEInitialize()
{
	char version[100];
	if(!LinkHE("ihc.dll"))	
		LinkHE("d:\\31166\\developing\\debug\\ihc.dll");
	HPEInitializeDLL(&BdInfo,HwyInfo,GCarInfo,&Param,version);
}

__declspec(dllexport) void SPY_SetPos(int floor,int pos)
{
	BdInfo.landingPosition[floor]=pos;
}

__declspec(dllexport) void SPY_SetTSACase(int car,int origin,int target,int delay)
{
	SetTSACase(car,origin,target,delay);
}

__declspec(dllexport) void SPY_MotionCmd(int car,int origin,int target,int delay, int jerk,int acc,int vel)
{
	MotionCmd(car,origin,target,delay,jerk,acc,vel);
}

__declspec(dllexport) int SPY_GetMotionValue(int car,int time,int type)
{
	return (int)GetMotionValue(car,time,type);
}

__declspec(dllexport) int SPY_GetTSADelay(int orgLead,int tgtLead,int orgTrail,int tgtTrail,int minSep)
{
	return (int)GetTSADelay(orgLead,tgtLead,orgTrail,tgtTrail,minSep);
}

}
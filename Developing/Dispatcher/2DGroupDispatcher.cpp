// 2DGroupDispatcher.cpp: Defines the entry point for the 2DGD DLL application
//
#include "stdafx.h"
#include "..\COGS\all.h"
#include "..\COGS\2DInterfaceData.h"
#include "..\COGS\2DInterfaceFunctionType.h"
#include "..\HoistwayPlanner\HoistwayPlannerDefs.h"  // for T_HPSCORE_MAX
#include <memory.h>
#include <string.h>
#include <windows.h>  // contains references to: HINSTANCE, LoadLibrary, GetProcAddress, FreeLibrary

#define EXTERN extern
#include "NPI.h"
#undef EXTERN

#define GDVERSION		"2DGroupDispatcher.DLL 051103"
//#define COGS_UNIT_TEST


// static parameters
BuildingInfo	bdInfo;
HoistwayInfo	hwyInfo[MAX_HOISTWAY];
CarInfo			carInfo[MAX_CAR];
Parameter		param;

// car state information
CarState		carState[MAX_CAR];

// call information
CallList AssignedCalls[MAX_CAR];

const int MASK16[16] = {0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
					0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000};
// global variable which needs initialization when GDInitialize for consistent running
int JustSequentialCar=0;
// local functions
void InitializeLogic();
CHAR MinimumHoistwayImpactDispatcher(Call new_call[], CarState state[], TIME_T ctCurrentTime);
CHAR SequentialDispatcher(Call new_call[], CarState state[], TIME_T ctCurrentTime);
CHAR JustSequentialDispatcher(Call new_call[],CarState state[],TIME_T ctCurrentTime);
CHAR MinimumHoistwayWorkDispatcher(Call new_call[], CarState state[], TIME_T ctCurrentTime);
CHAR MinimumHoistwayImpact_NoPolicy(Call new_call[], CarState state[], TIME_T ctCurrentTime);
CHAR SimpleUpPeakDispatcher(Call new_call[], CarState state[], TIME_T ctCurrentTime);
CHAR SimpleZoneUpPeakDispatcher(Call new_call[], CarState state[], TIME_T ctCurrentTime);
CHAR StaticUDA(Call new_call[], CarState state[], TIME_T ctCurrentTime);
CHAR StaticU1U2D1D2(Call new_call[], CarState state[], TIME_T ctCurrentTime);
CHAR StaticU1U2D1D2A(Call new_call[], CarState state[], TIME_T ctCurrentTime);
CHAR Static4SectorUpPeak(Call new_call[], CarState state[], TIME_T ctCurrentTime);
void AcceptHallCall(UCHAR carIndex, Call *new_call, TIME_T ctNow);

// external functions
ReturnAssignmentToPassenger_T ReturnAssignmentToPassenger;
AcceptAssignment_T AcceptAssignment;
ReturnHoistwayPlanScore_T ReturnHoistwayPlanScore;
HPPlanParking_T HPPlanParking;

// for debug messages
#ifdef WITH_DEBUG
#include <stdio.h>
FILE *f_ErrorLog;
#define ERRORFILE "GDError.log"
#endif

#ifdef COGS_UNIT_TEST
FILE *f_GDUnitTest;
#endif

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


extern "C" __declspec(dllexport)
void GDInitialize(BuildingInfo *mBdInfo,
				  HoistwayInfo mHwyInfo[MAX_HOISTWAY],
				  CarInfo mCarInfo[MAX_CAR],
				  Parameter *mParam,
				  char *versionName) {
	int h, c, k, f;
	HINSTANCE hinstCOGS, hinstHP;

// Store parameters into local data structure
	memcpy(&bdInfo, mBdInfo, sizeof(BuildingInfo));
	for (h = 0; h < bdInfo.nHoistways; h++)
		memcpy(&(hwyInfo[h]), &(mHwyInfo[h]), sizeof(HoistwayInfo));
	for (c = 0; c < bdInfo.nCars; c++)
		memcpy(&(carInfo[c]), &(mCarInfo[c]), sizeof(CarInfo));
	memcpy(&param, mParam, sizeof(Parameter));

// Other initializations of local data structures
	for (c = 0; c < bdInfo.nCars; c++) {
// empty_list maintained as a singly-linked list
		AssignedCalls[c].empty_list = MAX_CALL - 1;
		for (k = 0; k < MAX_CALL; k++)
			AssignedCalls[c].calls[k].next = k - 1;

		AssignedCalls[c].boardedIndex = -1;
		AssignedCalls[c].nBoardedList = 0;
		for (f = 0; f < MAX_FLOOR; f++) {
			AssignedCalls[c].waitingAtHead[f] = -1;
			AssignedCalls[c].waitingAtTail[f] = -1;
		}
		AssignedCalls[c].expectedTransferHead = -1;
	}

#ifdef WITH_DEBUG
	f_ErrorLog = fopen(ERRORFILE, "w");
	fprintf(f_ErrorLog, "\nGDInitialize called; using algorithm %d\n",
		(int) param.GDAlgorithmID);
#endif
#ifdef COGS_UNIT_TEST
	f_GDUnitTest = fopen("GDUnitTest.log", "w");
#endif

// return version number
	strcpy(versionName, GDVERSION);

// initialization of dispatching logic
	InitializeLogic();

// attach external functions
	hinstCOGS = LoadLibrary(COGS_FILE);
	if (hinstCOGS == NULL) {
#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "GDInitialize 1: cannot load COGS EXE as library\n");
#endif
		return;
	}

	ReturnAssignmentToPassenger = (ReturnAssignmentToPassenger_T)
		GetProcAddress(hinstCOGS, "ReturnAssignmentToPassenger");
	if (ReturnAssignmentToPassenger == NULL) {
#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "GDInitialize 2: cannot get proc address for ReturnAssignmentToPassenger\n");
#endif
		return;
	}

	hinstHP = LoadLibrary(HOISTWAY_PLANNER_FILE);
	if (hinstHP == NULL) {
#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "GDInitialize 3: cannot load HP DLL as library\n");
#endif
		return;
	}

	AcceptAssignment = (AcceptAssignment_T) GetProcAddress(hinstHP, "AcceptAssignment");
	if (AcceptAssignment == NULL) {
#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "GDInitialize 4: cannot get proc address for AcceptAssignment\n");
#endif
		return;
	}
	ReturnHoistwayPlanScore = (ReturnHoistwayPlanScore_T)
		GetProcAddress(hinstHP, "ReturnHoistwayPlanScore");
	if (ReturnHoistwayPlanScore == NULL) {
#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "GDInitialize 5: cannot get proc address for ReturnHoistwayPlanScore\n");
#endif
		return;
	}
	HPPlanParking = (HPPlanParking_T)
		GetProcAddress(hinstHP, "HPPlanParking");
	if (HPPlanParking == NULL) {
#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "GDInitialize 6: cannot get proc address for HPPlanParking\n");
#endif
		return;
	}
}  // GDInitialize


extern "C" __declspec(dllexport)
void GDAlgorithmInitialize(BuildingInfo *mBdInfo,
				  HoistwayInfo mHwyInfo[MAX_HOISTWAY],
				  CarInfo mCarInfo[MAX_CAR],
				  Parameter *mParam,
				  char *versionName) {
	int h, c;
	FLOOR_T parking_location;
	
// Temporary hard-coded setting (AH 051115)
	param.bParkingEnabled = 1;  // TEMPORARY!!!

// Initialization based on dispatching algorithm
	switch (param.GDAlgorithmID) {
		case 2:  // NPI dispatcher
			NPIInitialize();
			break;

		case 6:  // SimpleUpPeakDispatcher
			if (param.bParkingEnabled)
				for (c = 1; c < bdInfo.nCars; c++) {
					parking_location = c % 2;  // upper cars at lobby, lower cars at pit
					HPPlanParking((UCHAR) c, (FLOOR_T) 0, (FLOOR_T) 0, parking_location,
						(TIME_T) 0, (SHORT) 0);
				}
			break;
		case 7:  // SimpleZoneUpPeakDispatcher
			if (param.bParkingEnabled)
				for (c = 0; c < bdInfo.nCars; c++) {
					parking_location = c % 2;  // upper cars at lobby, lower cars at pit
					HPPlanParking((UCHAR) c, (FLOOR_T) 0, (FLOOR_T) 0, parking_location,
						(TIME_T) 0, (SHORT) 0);
				}
			break;
		case 9:  // StaticU1U2D1D2
			if (param.bParkingEnabled)
				for (h = 0; h < bdInfo.nHoistways; h++)
					if (h < bdInfo.nHoistways/2) {
						HPPlanParking((UCHAR) 2*h, (FLOOR_T) 0, (FLOOR_T) 0,
							(FLOOR_T) 0, (TIME_T) 0, (SHORT) 0);
						HPPlanParking((UCHAR) 2*h + 1, (FLOOR_T) 0, (FLOOR_T) 0,
							(FLOOR_T) 1, (TIME_T) 0, (SHORT) 0);
					}
					else {
						HPPlanParking((UCHAR) 2*h, (FLOOR_T) 0, (FLOOR_T) 0,
							(FLOOR_T) 1+(bdInfo.nLandings-1)/4, (TIME_T) 0, (SHORT) 0);
						HPPlanParking((UCHAR) 2*h + 1, (FLOOR_T) 0, (FLOOR_T) 0,
							(FLOOR_T) 3*(bdInfo.nLandings-1)/4, (TIME_T) 0, (SHORT) 0);
					}
			break;
		case 10:  // StaticU1U2D1D2A
			if (param.bParkingEnabled)
				HPPlanParking((UCHAR) 0, (FLOOR_T) 0, (FLOOR_T) 0, (FLOOR_T) 0,
					(TIME_T) 0, (SHORT) 0);
				HPPlanParking((UCHAR) 1, (FLOOR_T) 0, (FLOOR_T) 0, (FLOOR_T) 1,
					(TIME_T) 0, (SHORT) 0);
				HPPlanParking((UCHAR) 2, (FLOOR_T) 0, (FLOOR_T) 0,
					(FLOOR_T) 1+(bdInfo.nLandings-1)/4, (TIME_T) 0, (SHORT) 0);
				HPPlanParking((UCHAR) 3, (FLOOR_T) 0, (FLOOR_T) 0,
					(FLOOR_T) 3*(bdInfo.nLandings-1)/4, (TIME_T) 0, (SHORT) 0);
			break;
		case 11:  // Static4SectorUpPeak
			if (param.bParkingEnabled)
				for (c = 0; c < bdInfo.nCars - 2; c++) {
					parking_location = c % 2;  // upper cars at lobby, lower cars at pit
					HPPlanParking((UCHAR) c, (FLOOR_T) 0, (FLOOR_T) 0, parking_location,
						(TIME_T) 0, (SHORT) 0);
				}
			break;
		default:
			break;
	}
}  // GDAlgorithmInitialize


extern "C" __declspec(dllexport)
void DispatchNewCall(Call new_call,
					 USHORT sequenceNumber,
					 CarState state[],
					 TIME_T ctCurrentTime,
					 CHAR manualAssignment) {
	CHAR assignment;
#ifdef WITH_DEBUG
	UCHAR c;
	HPSCORE_T score;
#endif

	new_call.ctRegistration = ctCurrentTime;
#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, "DispatchNewCall: origin=%d dest=%d manualAssignment=%d\n",
		new_call.origin, new_call.destination, manualAssignment);
#endif

	if (manualAssignment < 0) {
		switch (param.GDAlgorithmID)
		{
			case 1:
			assignment = SequentialDispatcher(&new_call, state, ctCurrentTime);
				break;
			case 2:
			assignment = NPIDispatcher(&new_call, state, ctCurrentTime);
				break;
			case 3:
				assignment = JustSequentialDispatcher(&new_call, state, ctCurrentTime);
				break;
			case 4:  // originally called HPScoring
				assignment = MinimumHoistwayWorkDispatcher(&new_call, state, ctCurrentTime);
				break;
			case 5:
				assignment = MinimumHoistwayImpact_NoPolicy(&new_call, state, ctCurrentTime);
				break;
			case 6:
				assignment = SimpleUpPeakDispatcher(&new_call, state, ctCurrentTime);
				break;
			case 7:
				assignment = SimpleZoneUpPeakDispatcher(&new_call, state, ctCurrentTime);
				break;
			case 8:
				assignment = StaticUDA(&new_call, state, ctCurrentTime);
				break;
			case 9:
				assignment = StaticU1U2D1D2(&new_call, state, ctCurrentTime);
				break;
			case 10:
				assignment = StaticU1U2D1D2A(&new_call, state, ctCurrentTime);
				break;
			case 11:
				assignment = Static4SectorUpPeak(&new_call, state, ctCurrentTime);
				break;
			default:
				assignment = MinimumHoistwayImpactDispatcher(&new_call, state, ctCurrentTime);
				break;
		}

	}
	else {
#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "  [%d -> %d] MANUAL=%d  ( ", new_call.origin, new_call.destination,
			manualAssignment);
		for (c = 0; c < bdInfo.nCars; c++) {
			score = ReturnHoistwayPlanScore(c, new_call, AssignedCalls, state, ctCurrentTime);
			fprintf(f_ErrorLog, " %d:%lu ", c, score);
		}
		fprintf(f_ErrorLog, ")\n");
#endif
		assignment = manualAssignment;
	}

// Store call in local data structure
	if (assignment >= 0) {
		AcceptHallCall((CHAR) assignment, &new_call, ctCurrentTime);
		ReturnAssignmentToPassenger(sequenceNumber, assignment);
		AcceptAssignment(assignment, new_call, state, ctCurrentTime);
	}
}



// FUNCTIONS FOR MANAGING CALL LIST

#ifdef COGS_UNIT_TEST
// For unit testing of call management functions in Group Dispatcher
void PrintCallList(UCHAR c,
				   FLOOR_T lo_floor,
				   FLOOR_T hi_floor,
				   FLOOR_T origin) {
	int f, nDB, nU, nD;
	CALL_INDEX_T p;

	fprintf(f_GDUnitTest, "   floors: up-calls/ down-calls/ deboards :");
	for (f = lo_floor; f <= hi_floor; f++) {
		nDB = 0;  // number deboarding at f
		for (p = AssignedCalls[c].boardedIndex; p >= 0; p = AssignedCalls[c].calls[p].next)
			if (AssignedCalls[c].calls[p].destination == f)
				nDB++;
		nU = 0;   // number of up pax waiting at floor f
		nD = 0;   // number of down pax
		for (p = AssignedCalls[c].waitingAtHead[f]; p >= 0; p = AssignedCalls[c].calls[p].next)
			if (AssignedCalls[c].calls[p].destination > f)
			    nU++;
			else
			    nD++;
		if (nDB || nU || nD)
			fprintf(f_GDUnitTest, " %d:%d/%d/%d", f, nU, nD, nDB);
	}

// Show waiting calls at origin floor
	if (origin >= 0 && origin != INVALID_FLOOR) {
		fprintf(f_GDUnitTest, "\n   Waiting at floor %d (newest first):", origin);
        for (p = AssignedCalls[c].waitingAtHead[origin]; p >= 0;
				p = AssignedCalls[c].calls[p].next)
			fprintf(f_GDUnitTest, " %d", AssignedCalls[c].calls[p].destination);
	}
// Show boarded passengers (car calls)
	if (AssignedCalls[c].boardedIndex >= 0) {
		fprintf(f_GDUnitTest, "\n   Boarded to:");
		for (p = AssignedCalls[c].boardedIndex; p >= 0; p = AssignedCalls[c].calls[p].next)
			fprintf(f_GDUnitTest, " %d", AssignedCalls[c].calls[p].destination);
	}
// Show expected transfer list (expected car calls)
	if (AssignedCalls[c].expectedTransferHead >= 0) {
		fprintf(f_GDUnitTest, "\n   Expected transfers:");
		for (p = AssignedCalls[c].expectedTransferHead; p >= 0;
				p = AssignedCalls[c].calls[p].next)
			fprintf(f_GDUnitTest, " %d", AssignedCalls[c].calls[p].destination);
	}
	fprintf(f_GDUnitTest, "\n");

}  // PrintCallList
#endif


// Accept a new hall call and insert into waiting-at list
void AcceptHallCall(UCHAR carIndex,
					Call *new_call,
					TIME_T ctNow) {
	CallListEntry *current;
	CALL_INDEX_T currentIndex;
//AH:CALL_INDEX_T emptyIndex;

	if (carIndex < 0 || carIndex >= bdInfo.nCars) {
#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "AcceptHallCall 1: car index %d, origin %d, dest %d\n",
			carIndex, new_call->origin, new_call->destination);
#endif
		return;
	}

	if (new_call->origin == new_call->destination ||
		new_call->origin < 0 || new_call->origin >= MAX_FLOOR ||
		new_call->destination < 0 || new_call->destination >= MAX_FLOOR) {
#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "AcceptHallCall 2: car index %d, origin %d, dest %d\n",
			carIndex, new_call->origin, new_call->destination);
#endif
		return;
	}

	if (AssignedCalls[carIndex].empty_list < 0) {
#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "AcceptHallCall 3: empty list, %d\n", MAX_CALL);
#endif
		return;
	}

// Insert new_call into top of call list
	currentIndex = AssignedCalls[carIndex].empty_list;	// new entry index
	current = &(AssignedCalls[carIndex].calls[currentIndex]); // new entry pointer
	AssignedCalls[carIndex].empty_list = current->next; // update the empty list
	
/*AH:	// shs0721 update current empty list's prev as none
	emptyIndex=AssignedCalls[carIndex].empty_list;
	if (emptyIndex>=0)
		AssignedCalls[carIndex].calls[emptyIndex].prev=-1; */

	// update the contents from new call entered
	current->origin = new_call->origin;
	current->destination = new_call->destination;
	current->nPassengers = new_call->nPassengers;
	current->callType = new_call->callType;
	current->ctRegistration = ctNow;

// Insert new call as head of doubly-linked data structure
	current->prev = -1;	                                                    // new call's prev = -1
	current->next = AssignedCalls[carIndex].waitingAtHead[current->origin]; // new call's next is current head
	AssignedCalls[carIndex].waitingAtHead[current->origin] = currentIndex;  // update current waiting head to point new entry

	if (current->next >= 0)	 // current is not the last item
		AssignedCalls[carIndex].calls[current->next].prev = currentIndex;   // next item's prev is current
	else                     // current is only item in list
		AssignedCalls[carIndex].waitingAtTail[current->origin] = currentIndex;

#ifdef COGS_UNIT_TEST
	fprintf(f_GDUnitTest, "\nAfter AcceptHallCall for car %d from %d->%d at t=%6.3lf:\n",
		carIndex, new_call->origin, new_call->destination, (double) 0.001*ctNow);
    PrintCallList(carIndex, 0, 10, new_call->origin);
#endif
}  // AcceptHallCall


// Cancel hall calls from waiting-at list and move them to the expected-transfer list
extern "C" __declspec(dllexport)
void GDCancelHallCalls(UCHAR carIndex,
					   FLOOR_T origin) {
	CALL_INDEX_T p;  // index to traverse hall call list
	SHORT availableCapacity;

#ifdef WITH_DEBUG
	CALL_INDEX_T found_up, found_dn;

	if (carIndex < 0 || carIndex >= bdInfo.nCars || origin < 0 || origin >= bdInfo.nLandings) {
		fprintf(f_ErrorLog, "GDCancelHallCalls 1: car index %d, origin %d\n",
			carIndex, origin);
		return;
	}
#endif
	
	if (AssignedCalls[carIndex].expectedTransferHead >= 0) {
#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "GDCancelHallCalls 2: expectedTransfer list is not empty! %d\n",
			carIndex, origin);
#endif
// Delete any pre-existing expected calls to empty list
		while (AssignedCalls[carIndex].expectedTransferHead >= 0) {
			p = AssignedCalls[carIndex].expectedTransferHead;
			AssignedCalls[carIndex].expectedTransferHead = AssignedCalls[carIndex].calls[p].next;
			AssignedCalls[carIndex].calls[p].next = AssignedCalls[carIndex].empty_list;
			AssignedCalls[carIndex].empty_list = p;
		}
	}

// Shift waiting passengers in queue into expected transfer list within capacity limit
	availableCapacity = carInfo[carIndex].capacity - AssignedCalls[carIndex].nBoardedList;
	while (availableCapacity > 0 && AssignedCalls[carIndex].waitingAtTail[origin] >= 0) {
// Assume first-in, first-out discipline, so the passengers at the tail are transferred first
		p = AssignedCalls[carIndex].waitingAtTail[origin];	// current tail
		AssignedCalls[carIndex].waitingAtTail[origin] = AssignedCalls[carIndex].calls[p].prev; // previous tail becomes current tail
		if (AssignedCalls[carIndex].waitingAtTail[origin] >= 0)  // previous tail exist
			AssignedCalls[carIndex].calls[AssignedCalls[carIndex].waitingAtTail[origin]].next = -1; // cut off the current tail
		else
			AssignedCalls[carIndex].waitingAtHead[origin] = -1; // no waiting at origin

		if (AssignedCalls[carIndex].expectedTransferHead >= 0)  // expected car call exists?
			AssignedCalls[carIndex].calls[AssignedCalls[carIndex].expectedTransferHead].prev = p; // 
		AssignedCalls[carIndex].calls[p].next = AssignedCalls[carIndex].expectedTransferHead;
		//prev check
		AssignedCalls[carIndex].calls[p].prev=-1; // p is inserted into the head of expectedTransferHead
		AssignedCalls[carIndex].expectedTransferHead = p;

		availableCapacity--;
	}

#ifdef WITH_DEBUG
// Check for inconsistencies in the direction
	found_up = 0;
	found_dn = 0;
	for (p = AssignedCalls[carIndex].expectedTransferHead; p >= 0;
			p = AssignedCalls[carIndex].calls[p].next) {
		if (AssignedCalls[carIndex].calls[p].origin < AssignedCalls[carIndex].calls[p].destination)
			found_up++;
		else if (AssignedCalls[carIndex].calls[p].origin > AssignedCalls[carIndex].calls[p].destination)
			found_dn++;
	}
	if (found_up && found_dn)
		fprintf(f_ErrorLog, "GDCancelHallCalls 4: passengers at floor %d for car %d waiting to go up (%d) and down (%d)\n",
			origin, carIndex, found_up, found_dn);
	else if (found_up == 0 && found_dn == 0)
		fprintf(f_ErrorLog, "GDCancelHallCalls 5: no passengers at floor %d for car %d for hall call\n",
			origin, carIndex);
#endif
#ifdef COGS_UNIT_TEST
	fprintf(f_GDUnitTest, "\nAfter GDCancelHallCalls for car %d at origin %d:\n", carIndex, origin);
    PrintCallList(carIndex, 0, 10, -1);
#endif

}  // GDCancelHallCalls


// Accept car calls by converting those calls with a matching destination from the expected
// transfer list
extern "C" __declspec(dllexport)
void GDAcceptCarCall(UCHAR carIndex,
					 FLOOR_T dest) {
	CALL_INDEX_T p,			 // index to traverse expected transfer list
		next_p;				 // index to next element in expected transfer list
	CallListEntry *current;  // pointer to CallListEntry for p
	bool found;

// Scan expected transfer list and board any whose destination match dest (i.e., register car call)
	found = 0;
	p = AssignedCalls[carIndex].expectedTransferHead; 

	while (p >= 0) {
		next_p = AssignedCalls[carIndex].calls[p].next;
		if (AssignedCalls[carIndex].calls[p].destination == dest) {
			found = 1;
			current = &(AssignedCalls[carIndex].calls[p]);
// Delete current from expected transfer list and fix doubly-linked list
			if (current->prev >= 0)
				AssignedCalls[carIndex].calls[current->prev].next = current->next;
			else
				AssignedCalls[carIndex].expectedTransferHead = current->next;
			if (current->next >= 0)
				AssignedCalls[carIndex].calls[current->next].prev = current->prev;

// Now add current to front of boarded list
			if (AssignedCalls[carIndex].boardedIndex >= 0)
				AssignedCalls[carIndex].calls[AssignedCalls[carIndex].boardedIndex].prev = p;
			current->next = AssignedCalls[carIndex].boardedIndex;
			current->prev = -1;
			AssignedCalls[carIndex].boardedIndex = p;
			AssignedCalls[carIndex].nBoardedList++;
		}
		p = next_p;
	}

#ifdef WITH_DEBUG
// Check for inconsistencies in the direction
	if (! found)
		fprintf(f_ErrorLog, "GDAcceptCarCall: no car calls for %d in expected transfer list for car %d\n",
			dest, carIndex);
#endif
#ifdef COGS_UNIT_TEST
	fprintf(f_GDUnitTest, "\nAfter GDAcceptCarCall for car %d for destination %d:\n",
		carIndex, dest);
    PrintCallList(carIndex, 0, 10, -1);
#endif
}  // GDAcceptCarCall


// Cancel all car calls at the destination floor
extern "C" __declspec(dllexport)
void GDCancelCarCalls(UCHAR carIndex,
					  FLOOR_T dest) {
	CALL_INDEX_T p;			 // index to boarded passenger list
	CALL_INDEX_T next_p;	 // index to temporary next passenger
	CallListEntry *current;  // pointer to CallListEntry for p
// Scan expected transfer list and delete any whose destination match dest
	p=AssignedCalls[carIndex].boardedIndex;

	while (p>=0)	// fix infinite loop code
	{
		next_p=AssignedCalls[carIndex].calls[p].next;

		if (AssignedCalls[carIndex].calls[p].destination == dest) 
		{
			current = &(AssignedCalls[carIndex].calls[p]);
// Delete current from boarded list and fix doubly-linked list
			if (current->prev >= 0)
				AssignedCalls[carIndex].calls[current->prev].next = current->next;
			else
				AssignedCalls[carIndex].boardedIndex = current->next;

			if (current->next >= 0)
				AssignedCalls[carIndex].calls[current->next].prev = current->prev;
			current->next = AssignedCalls[carIndex].empty_list;  // deboarded call to be added to empty list
			AssignedCalls[carIndex].empty_list = p;
			AssignedCalls[carIndex].nBoardedList--;
		}
		p=next_p;
	}
#ifdef COGS_UNIT_TEST
	fprintf(f_GDUnitTest, "\nAfter GDCancelCarCall for car %d for destination %d:\n",
		carIndex, dest);
    PrintCallList(carIndex, 0, 10, -1);
#endif
}  // GDCancelCarCalls


// for SequentialDispatcher
CHAR next_up, next_dn;


void InitializeLogic() {
// for SequentialDispatcher
	next_dn = bdInfo.nCars;
	next_up = bdInfo.nCars;
// for JustSequentialDispatcher
	JustSequentialCar=0;
}


// Dispatcher that selects car such that the score impact (i.e., difference between the hoistway
//   score with call assigned to car and hoistway score without call) is minimized
// Simple static policy: even-numbered cars for down traffic and odd-numbered cars for up traffic
CHAR MinimumHoistwayImpactDispatcher(Call *new_call,
								   CarState state[],
								   TIME_T ctCurrentTime) {
	UCHAR c, best_c;
	HPSCORE_T null_score, score, impact, best_impact;
	Call null_call;
	int nHoistways2D;  // number of 2-D hoistways

	nHoistways2D = bdInfo.nCars - bdInfo.nHoistways;

#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, "  [%d -> %d]", new_call->origin, new_call->destination);
#endif
	null_call.origin = INVALID_FLOOR;
	null_call.destination = INVALID_FLOOR;
	null_call.nPassengers = 0;
	best_impact = T_HPSCORE_MAX;
	best_c = -1;

// Dumb static allocation assigns down calls to lower cars and up calls to upper cars
	if (new_call->origin > new_call->destination)  // down traffic uses even-numbered cars
		c = 0;
	else
		c = 1;
	for ( ; c < 2*nHoistways2D; c += 2) {
		null_score = ReturnHoistwayPlanScore(c, null_call, AssignedCalls, state, ctCurrentTime);
		if (null_score >= T_HPSCORE_MAX) {
#ifdef WITH_DEBUG
			fprintf(f_ErrorLog, "  Cannot find plan with car %d at t=%lu\n", c, ctCurrentTime);
#endif
		}
		else {
			score = ReturnHoistwayPlanScore(c, *new_call, AssignedCalls, state, ctCurrentTime);

#ifdef WITH_DEBUG
			fprintf(f_ErrorLog, "  %d:%lu/%lu", c, score, null_score);
			if (score < null_score)
				fprintf(f_ErrorLog, " !");
#endif
			if (score < null_score)
				impact = 0;
			else
				impact = score - null_score;

			if (impact < best_impact) {
				best_impact = impact;
				best_c = c;
			}
		}
	}

// In 1D hoistways, "ad hoc" car is eligible for both up and down calls
	for (c = 2*nHoistways2D ; c < bdInfo.nCars; c++) {
		null_score = ReturnHoistwayPlanScore(c, null_call, AssignedCalls, state, ctCurrentTime);

		if (null_score >= T_HPSCORE_MAX) {
#ifdef WITH_DEBUG
			fprintf(f_ErrorLog, "  Cannot find plan with car %d at t=%lu\n", c, ctCurrentTime);
#endif
		}
		else {
			score = ReturnHoistwayPlanScore(c, *new_call, AssignedCalls, state, ctCurrentTime);

#ifdef WITH_DEBUG
			fprintf(f_ErrorLog, "  %d:%lu/%lu", c, score, null_score);
			if (score < null_score)
				fprintf(f_ErrorLog, " !");
#endif
			if (score < null_score)
				impact = 0;
			else
				impact = score - null_score;

			if (impact < best_impact) {
				best_impact = impact;
				best_c = c;
			}
		}
	}

#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, " ->%d\n", best_c);
#endif

	return best_c;
}  // MinimumHoistwayImpactDispatcher


// dummy dispatcher -- sequential dispatcher with even-numbered cars for down traffic and
// odd-numbered cars for up traffic
CHAR SequentialDispatcher(Call *new_call,
						  CarState state[],
						  TIME_T ctCurrentTime) {

	if (new_call->origin > new_call->destination) {
		next_dn += 2;
		if (next_dn >= bdInfo.nCars)
			next_dn = 0;
		return next_dn;
	}
	else if (new_call->origin < new_call->destination) {
		next_up += 2;
		if (next_up >= bdInfo.nCars)
			next_up = 1;
		return next_up;
	}
	else
		return -1;

}  // SequentialDispatcher


// real sequential dispatcher which assign calls sequentially 		
CHAR JustSequentialDispatcher(Call *new_call,
						  CarState state[],
						  TIME_T ctCurrentTime) 
{

	if (JustSequentialCar<bdInfo.nCars-1 && JustSequentialCar>=0)
	{
		JustSequentialCar++;
	}
	else
		JustSequentialCar=0;

	return JustSequentialCar;

}  // JustSequentialDispatcher		   


// Dispatcher that selects car such that the resulting hoistway plan score is minimized
// Simple static policy: even-numbered cars for down traffic and odd-numbered cars for up traffic
CHAR MinimumHoistwayWorkDispatcher(Call *new_call,
						  CarState state[],
						  TIME_T ctCurrentTime) {

	UCHAR c, best_c;
	HPSCORE_T score, best_score;
	int nHoistways2D;  // number of 2-D hoistways

	nHoistways2D = bdInfo.nCars - bdInfo.nHoistways;

#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, "  [%d -> %d]", new_call->origin, new_call->destination);
#endif

// Dumb static allocation assigns down calls to lower cars and up calls to upper cars
	if (new_call->origin > new_call->destination)  // down traffic uses even-numbered cars
		c = 0;
	else
		c = 1;

	best_score = T_HPSCORE_MAX;
	best_c = -1;
	for ( ; c < 2*nHoistways2D; c += 2) {
		score = ReturnHoistwayPlanScore(c, *new_call, AssignedCalls, state, ctCurrentTime);

#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "  %d:%lu", c, score);
#endif
		if (score < best_score) {
			best_score = score;
			best_c = c;
		}
	}

// In 1D hoistways, "ad hoc" car is eligible for both up and down calls
	for (c = 2*nHoistways2D ; c < bdInfo.nCars; c++) {
		score = ReturnHoistwayPlanScore(c, *new_call, AssignedCalls, state, ctCurrentTime);

#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "  %d:%lu", c, score);
#endif
		if (score < best_score) {
			best_score = score;
			best_c = c;
		}
	}

#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, " ->%d\n", best_c);
#endif

	return best_c;
}  // MinimumHoistwayWorkDispatcher


// Dispatcher that selects car such that the score impact (i.e., difference between the hoistway
//   score with call assigned to car and hoistway score without call) is minimized
// Simple static policy: even-numbered cars for down traffic and odd-numbered cars for up traffic
CHAR MinimumHoistwayImpact_NoPolicy(Call *new_call,
								   CarState state[],
								   TIME_T ctCurrentTime) {
	UCHAR c, best_c;
	HPSCORE_T null_score, score, impact, best_impact;
	Call null_call;

#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, "  [%d -> %d]", new_call->origin, new_call->destination);
#endif
	null_call.origin = INVALID_FLOOR;
	null_call.destination = INVALID_FLOOR;
	null_call.nPassengers = 0;
	best_impact = T_HPSCORE_MAX;
	best_c = -1;

	for (c = 0 ; c < bdInfo.nCars; c++) {
		null_score = ReturnHoistwayPlanScore(c, null_call, AssignedCalls, state, ctCurrentTime);
		score = ReturnHoistwayPlanScore(c, *new_call, AssignedCalls, state, ctCurrentTime);

#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "  %d:%lu/%lu", c, score, null_score);
		if (score < null_score)
			fprintf(f_ErrorLog, " !");
#endif
		if (score < null_score)
			impact = 0;
		else
			impact = score - null_score;

		if (impact < best_impact) {
			best_impact = impact;
			best_c = c;
		}
	}
#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, " ->%d\n", best_c);
#endif

	return best_c;
}  // MinimumHoistwayImpact_NoPolicy


// Dispatcher which reserves only one car (car 0) for down traffic, remaining lower cars for
// up calls whose destination is in the lower partition, and upper cars for up calls whose
// destination is in the upper partition, where the lower partition is floor 1 thru
// floor(bdInfo.nLandings / 2)
CHAR SimpleUpPeakDispatcher(Call *new_call,
							CarState state[],
							TIME_T ctCurrentTime) {
	UCHAR c, best_c;
	HPSCORE_T null_score, score, impact, best_impact;
	Call null_call;

#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, "  [%d -> %d]", new_call->origin, new_call->destination);
#endif

	if (new_call->origin > new_call->destination)
		best_c = 0;
	else {
		if (new_call->destination <= (int) (bdInfo.nLandings/2))
			c = 2;
		else
			c = 1;

		null_call.origin = INVALID_FLOOR;
		null_call.destination = INVALID_FLOOR;
		null_call.nPassengers = 0;
		best_impact = T_HPSCORE_MAX;
		best_c = -1;

		for ( ; c < bdInfo.nCars; c += 2) {
			null_score = ReturnHoistwayPlanScore(c, null_call,
				AssignedCalls, state, ctCurrentTime);
			score = ReturnHoistwayPlanScore(c, *new_call,
				AssignedCalls, state, ctCurrentTime);

#ifdef WITH_DEBUG
			fprintf(f_ErrorLog, "  %d:%lu/%lu", c, score, null_score);
			if (score < null_score)
				fprintf(f_ErrorLog, " !");
#endif
			if (score < null_score)
				impact = 0;
			else
				impact = score - null_score;

			if (impact < best_impact) {
				best_impact = impact;
				best_c = c;
			}
		}
	}
#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, " ->%d\n", best_c);
#endif

	return best_c;
}  // MinimumHoistwayImpact_NoPolicy


// Supposed pure 2-D for 4 hoistway. ( All hoistway have 2 cars)
CHAR SimpleZoneUpPeakDispatcher(Call *new_call,
							CarState state[],
							TIME_T ctCurrentTime)
{
	UCHAR c, best_c;
	HPSCORE_T null_score, score, impact, best_impact;
	Call null_call;
	
#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, "  [%d -> %d]", new_call->origin, new_call->destination);
#endif
	
	if (new_call->origin < new_call->destination)
	{
		if (new_call->destination <= (int) (bdInfo.nLandings/2))
			c = 0;
		else
			c = 1;
	}
	else
	{
		if (new_call->origin <= (int) (bdInfo.nLandings/2))
			c = 0;
		else
			c = 1;
	}
	
	null_call.origin = INVALID_FLOOR;
	null_call.destination = INVALID_FLOOR;
	null_call.nPassengers = 0;
	best_impact = T_HPSCORE_MAX;
	best_c = -1;
	
	for ( ; c < bdInfo.nCars; c += 2) {
		null_score = ReturnHoistwayPlanScore(c, null_call,
			AssignedCalls, state, ctCurrentTime);
		score = ReturnHoistwayPlanScore(c, *new_call,
			AssignedCalls, state, ctCurrentTime);
		
#ifdef WITH_DEBUG
		fprintf(f_ErrorLog, "  %d:%lu/%lu", c, score, null_score);
		if (score < null_score)
			fprintf(f_ErrorLog, " !");
#endif
		if (score < null_score)
			impact = 0;
		else
			impact = score - null_score;
		
		if (impact < best_impact) {
			best_impact = impact;
			best_c = c;
		}
	}
#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, " ->%d\n", best_c);
#endif
	
	return best_c;
}

CHAR StaticUDA(Call new_call[], CarState state[], TIME_T ctCurrentTime)
{
	UCHAR c, best_c;
	HPSCORE_T null_score, score, impact, best_impact;
	Call null_call;
	int candidateUp; // candidate cars for up calls
	int candidateDown; // candidate cars for down calls
	int candidateCars;
	
#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, "  [%d -> %d]", new_call->origin, new_call->destination);
#endif
	
	candidateUp = 0xfe; // all cars except car 0
	candidateDown = 0xfd; // all cars except car 1
	
	if (new_call->origin > new_call->destination)
		candidateCars = candidateDown;
	else
		candidateCars = candidateUp;
	
	null_call.origin = INVALID_FLOOR;
	null_call.destination = INVALID_FLOOR;
	null_call.nPassengers = 0;
	best_impact = T_HPSCORE_MAX;
	best_c = -1;
	for(c=0; c<bdInfo.nCars; c++)
	{
		if(candidateCars & MASK16[c])
		{
			null_score = ReturnHoistwayPlanScore(c, null_call,
				AssignedCalls, state, ctCurrentTime);
			score = ReturnHoistwayPlanScore(c, *new_call,
				AssignedCalls, state, ctCurrentTime);
			
#ifdef WITH_DEBUG
			fprintf(f_ErrorLog, "  %d:%lu/%lu", c, score, null_score);
			if (score < null_score)
				fprintf(f_ErrorLog, " !");
#endif
			if (score < null_score)
				impact = 0;
			else
				impact = score - null_score;
			
			if (impact < best_impact) {
				best_impact = impact;
				best_c = c;
			}
		}
	}
#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, " ->%d\n", best_c);
#endif
	
	return best_c;
}

CHAR StaticU1U2D1D2(Call new_call[], CarState state[], TIME_T ctCurrentTime)
{
	UCHAR c, best_c;
	HPSCORE_T null_score, score, impact, best_impact;
	Call null_call;
	int candidateU1; // candidate cars for U1 partition calls
	int candidateU2; // candidate cars for U2 partition calls
	int candidateD1; // candidate cars for D1 partition calls
	int candidateD2; // candidate cars for D2 partition calls
	int candidateCars;
	
#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, "  [%d -> %d]", new_call->origin, new_call->destination);
#endif
	
	candidateU1 = 0x05;
	candidateU2 = 0x0a;
	candidateD1 = 0x50;
	candidateD2 = 0xa0;
	
	if (new_call->origin < new_call->destination)
	{
		if(new_call->destination <= (int) (bdInfo.nLandings/2))
			candidateCars = candidateU1;
		else
			candidateCars = candidateU2;
	}
	else
	{
		if(new_call->origin <= (int) (bdInfo.nLandings/2))
			candidateCars = candidateD1;
		else
			candidateCars = candidateD2;
	}
	
	null_call.origin = INVALID_FLOOR;
	null_call.destination = INVALID_FLOOR;
	null_call.nPassengers = 0;
	best_impact = T_HPSCORE_MAX;
	best_c = -1;
	for(c=0; c<bdInfo.nCars; c++)
	{
		if(candidateCars & MASK16[c])
		{
			null_score = ReturnHoistwayPlanScore(c, null_call,
				AssignedCalls, state, ctCurrentTime);
			score = ReturnHoistwayPlanScore(c, *new_call,
				AssignedCalls, state, ctCurrentTime);
			
#ifdef WITH_DEBUG
			fprintf(f_ErrorLog, "  %d:%lu/%lu", c, score, null_score);
			if (score < null_score)
				fprintf(f_ErrorLog, " !");
#endif
			if (score < null_score)
				impact = 0;
			else
				impact = score - null_score;
			
			if (impact < best_impact) {
				best_impact = impact;
				best_c = c;
			}
		}
	}
#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, " ->%d\n", best_c);
#endif
	
	return best_c;
}

CHAR StaticU1U2D1D2A(Call new_call[], CarState state[], TIME_T ctCurrentTime)
{
	UCHAR c, best_c;
	HPSCORE_T null_score, score, impact, best_impact;
	Call null_call;
	int candidateU1; // candidate cars for U1 partition calls
	int candidateU2; // candidate cars for U2 partition calls
	int candidateD1; // candidate cars for D1 partition calls
	int candidateD2; // candidate cars for D2 partition calls
	int candidateA;  // candidate cars for A partition calls
	int candidateCars;
	
#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, "  [%d -> %d]", new_call->origin, new_call->destination);
#endif
	
	candidateU1 = 0x01;
	candidateU2 = 0x02;
	candidateD1 = 0x04;
	candidateD2 = 0x08;
	candidateA = 0xf0;
	
	if (new_call->origin < new_call->destination)
	{
		if(new_call->destination <= (int) (bdInfo.nLandings/2))
			candidateCars = candidateU1 | candidateA;
		else
			candidateCars = candidateU2 | candidateA;
	}
	else
	{
		if(new_call->origin <= (int) (bdInfo.nLandings/2))
			candidateCars = candidateD1 | candidateA;
		else
			candidateCars = candidateD2 | candidateA;
	}
	
	null_call.origin = INVALID_FLOOR;
	null_call.destination = INVALID_FLOOR;
	null_call.nPassengers = 0;
	best_impact = T_HPSCORE_MAX;
	best_c = -1;
	for(c=0; c<bdInfo.nCars; c++)
	{
		if(candidateCars & MASK16[c])
		{
			null_score = ReturnHoistwayPlanScore(c, null_call,
				AssignedCalls, state, ctCurrentTime);
			score = ReturnHoistwayPlanScore(c, *new_call,
				AssignedCalls, state, ctCurrentTime);
			
#ifdef WITH_DEBUG
			fprintf(f_ErrorLog, "  %d:%lu/%lu", c, score, null_score);
			if (score < null_score)
				fprintf(f_ErrorLog, " !");
#endif
			if (score < null_score)
				impact = 0;
			else
				impact = score - null_score;
			
			if (impact < best_impact) {
				best_impact = impact;
				best_c = c;
			}
		}
	}
#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, " ->%d\n", best_c);
#endif
	
	return best_c;
}

CHAR Static4SectorUpPeak(Call new_call[], CarState state[], TIME_T ctCurrentTime)
{
	UCHAR c, best_c;
	HPSCORE_T null_score, score, impact, best_impact;
	Call null_call;
	int candidateU1; // candidate cars for U1 partition calls
	int candidateU2; // candidate cars for U2 partition calls
	int candidateU3; // candidate cars for U3 partition calls
	int candidateU4; // candidate cars for U4 partition calls
	int candidateD;  // candidate cars for D partition calls
	int candidateCars;
	
#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, "  [%d -> %d]", new_call->origin, new_call->destination);
#endif
	
	candidateU1 = 0x01;
	candidateU2 = 0x06;
	candidateU3 = 0x18;
	candidateU4 = 0xA0;
	candidateD = 0x40;
	
	if (new_call->origin < new_call->destination)
	{
		if(new_call->destination <= (int) (bdInfo.nLandings/4))
			candidateCars = candidateU1;
		else if(new_call->destination <= (int) (bdInfo.nLandings/2))
			candidateCars = candidateU2;
		else if(new_call->destination <= (int) (bdInfo.nLandings*3/4))
			candidateCars = candidateU3;
		else
			candidateCars = candidateU4;
	}
	else
	{
			candidateCars = candidateD;
	}
	
	null_call.origin = INVALID_FLOOR;
	null_call.destination = INVALID_FLOOR;
	null_call.nPassengers = 0;
	best_impact = T_HPSCORE_MAX;
	best_c = -1;
	for(c=0; c<bdInfo.nCars; c++)
	{
		if(candidateCars & MASK16[c])
		{
			null_score = ReturnHoistwayPlanScore(c, null_call,
				AssignedCalls, state, ctCurrentTime);
			score = ReturnHoistwayPlanScore(c, *new_call,
				AssignedCalls, state, ctCurrentTime);
			
#ifdef WITH_DEBUG
			fprintf(f_ErrorLog, "  %d:%lu/%lu", c, score, null_score);
			if (score < null_score)
				fprintf(f_ErrorLog, " !");
#endif
			if (score < null_score)
				impact = 0;
			else
				impact = score - null_score;
			
			if (impact < best_impact) {
				best_impact = impact;
				best_c = c;
			}
		}
	}
#ifdef WITH_DEBUG
	fprintf(f_ErrorLog, " ->%d\n", best_c);
#endif
	
	return best_c;
}

	
// Algorithm information within this DLL 
static char DLL_NAME[50]		="Dispatcher.DLL";
static char DLL_DESCRIPTION[50]	="with static policy";
static char NO_ALGORITHM = 12;
static char ALGORITHM_NAME[20][50]=
{	   //12345678901234567890123456789012345678901234567890
		"Minimum hoistway impact",
		"Sequential dispatching (050712)",
		"NPI-1D",
		"Just Sequential dispatching (0:ncar-1)",
		"Minimum hoistway work",
		"Minimum hoistway impact - no policy",
		"Up-peak [D+U1-4/U5-9]",
		"Up-peak Zoning [2.5POI]",
		"StaticUDA",
		"StaticU1U2D1D2",
		"StaticU1U2D1D2A",
		"Static4SectorUpPeak"
};


extern "C" __declspec(dllexport)
void GetDispatcherDLLInfo(char info[20][50])
{
	int count,size;
	strncpy(info[0],DLL_NAME,50);
	strncpy(info[1],DLL_DESCRIPTION,50);
	info[2][0]=NO_ALGORITHM;
	size=NO_ALGORITHM;
	if (size>19)
	{
		size=19;
	}
	for(count=0; count<size && count <20;count++)
	{
		strncpy(info[3+count],ALGORITHM_NAME[count],50);
	}

}


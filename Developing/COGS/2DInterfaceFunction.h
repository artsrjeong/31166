#ifndef	_2D_INTERFACE_FUNCTION_
#define	_2D_INTERFACE_FUNCTION_

#include "..\COGS\2DInterfaceData.h"  //jsr 2005,07,07 for sharing same header file among COGS,HE,HP


extern "C" //jsr 2005,07,07 for dll exporting
{
/*******************************************************************************************
 * A function call to Group Dispatcher (GD) module to initialize its local variables 
 ********************************************************************************************/
__declspec(dllexport) void GDInitialize(BuildingInfo *,
										HoistwayInfo [],
										CarInfo [],
										Parameter *,
										char *);
__declspec(dllexport) void GDAlgorithmInitialize();


/*******************************************************************************************
 * A function call to Hoistway Planner (HP) module to initialize its local variables 
 ********************************************************************************************/
__declspec(dllexport) void HPInitialize(BuildingInfo *,
										HoistwayInfo [],
										CarInfo [],
										Parameter *,
										char *);

/*******************************************************************************************
 * A function call to Hoistway Planner Execution(HPE) module to initialize its local variables 
 ********************************************************************************************/
__declspec(dllexport) void HPEInitialize(BuildingInfo *,
										 HoistwayInfo [],
										 CarInfo [],
										 Parameter *,
										 char *);

/*******************************************************************************************
 * A function call to the dispatcher module to dispatch the  new call. 
 * This function will evaluate all feasible assignments and return the best assignment
 ********************************************************************************************/
__declspec(dllexport) void DispatchNewCall(Call new_call,
										   USHORT sequenceNumber,
										   CarState[],
										   TIME_T ctCurrentTime,
										   CHAR manualAssignment);


/*******************************************************************************************
 * A function call from dispatcher to hoistway planner. The call asked the hoistway planner 
 * to return the scoring of the hoistway plan if the new call is assigned to carIndex.
 ********************************************************************************************/
 __declspec(dllexport) HPSCORE_T ReturnHoistwayPlanScore(UCHAR carIndex,
													     Call new_call, 
													     CallList[],
													     CarState[],
														 TIME_T);


/*******************************************************************************************
 * This call asked the hoistway planner to regenerate the hoistway plan for a particular
 * hoistway. 
 ********************************************************************************************/
__declspec(dllexport) void RegeneratePlan(UCHAR hoistway_index, 
										  TIME_T ctCurrentTime);

/*******************************************************************************************
 * A function call Dispatcher to hoistway planner, indicating the new call should 
 * be assigned to carIndex.
 ********************************************************************************************/
__declspec(dllexport) void AcceptAssignment(UCHAR carIndex,
											Call assignedCall,
											CarState [],
											TIME_T ctCurrentTime);

/*******************************************************************************************
 * A function call from COGS to hoistway executer.
 * Update Hoistway Planner about car state
 * Hoistway Executer monitor the progress of hoistway execution, take actions when necessary
 ********************************************************************************************/
__declspec(dllexport) void MonitorCarState(UCHAR carIndex,
										   TIME_T ctCurrentTime);

/*******************************************************************************************
 * A function call from hoistway plan execution to COGS (OCSS)
 * Inform it about the next operation
 ********************************************************************************************/
__declspec(dllexport) void OCSSCommand(UCHAR carIndex,
									   CarCommand *oper);

/*******************************************************************************************
 * A function call to Database for updating car state
 ********************************************************************************************/
__declspec(dllexport) void PutCarState(UCHAR carIndex,
									   CarState *);

/*******************************************************************************************
 * A function call to Database for updating destination entry calls
 ********************************************************************************************/
// shs0712 want return type from void to int to return error code
__declspec(dllexport) UCHAR PutHallCall(UCHAR carIndex, Call *); 

__declspec(dllexport) void DeleteExpectedCarCalls(UCHAR carIndex);

__declspec(dllexport) void ConvertHallCalls(UCHAR carIndex, FLOOR_T origin, CHAR predirection);

__declspec(dllexport) void AcceptCarCalls(UCHAR carIndex, FLOOR_T destination);
 
__declspec(dllexport) UCHAR RemoveCarCalls(UCHAR carIndex, FLOOR_T destination);


/*******************************************************************************************
 * A function call to Database for retrieving car state
 ********************************************************************************************/
__declspec(dllexport) void GetCarState(UCHAR carIndex,
									   CarState *);

/*******************************************************************************************
 * A function call to Database for retrieving destination entry calls
 ********************************************************************************************/
__declspec(dllexport) void GetCalls(UCHAR carIndex,
									CallList *);


/*******************************************************************************************
 * A function call to Database for updating Hoistway Plan
 ********************************************************************************************/
__declspec(dllexport) void PutPlan(UCHAR hwIndex,
								   HoistwayPlan *);

/*******************************************************************************************
 * A function call to hoistway planner, informing it a new plan is ready for execution
 ********************************************************************************************/
__declspec(dllexport) void ExecutePlan(UCHAR hwIndex);


/*******************************************************************************************
 * A function call to Database for retrieving Hoistway Plan
 ********************************************************************************************/
__declspec(dllexport) void GetPlan(UCHAR hwIndex,
								   HoistwayPlan *);


/*******************************************************************************************
 * OTHERS
 ********************************************************************************************/
__declspec(dllexport) void GDCancelHallCalls(UCHAR carIndex,
											 FLOOR_T origin);
__declspec(dllexport) void HPECancelHallCalls(UCHAR carIndex,
											  FLOOR_T origin);
__declspec(dllexport) void ExpectedCarCalls(FLOOR_T destination);
__declspec(dllexport) void GDAcceptCarCall(UCHAR carIndex,
										   FLOOR_T destination);
__declspec(dllexport) void HPEAcceptCarCall(UCHAR carIndex,
										    FLOOR_T destination);
__declspec(dllexport) void GDCancelCarCalls(UCHAR carIndex,
											FLOOR_T destination);
__declspec(dllexport) void HPECancelCarCalls(UCHAR carIndex,
											 FLOOR_T destination);
/*******************************************************************************************
 * A function call from Group Dispatcher (GD) to COGS (DES)
 * simply to store the assignment to a local variable in COGS
 * until the DispatchNewCall function returns
 ********************************************************************************************/
__declspec(dllexport) void ReturnAssignmentToPassenger(USHORT sequenceNumber,
									   CHAR carIndex);

// CSW051112: Add new function
/*******************************************************************************************
 * A function call from Group Dispatcher (GD) to Hoistway Planner
 * Simplay Store the information into a struct for the car
 * When HP wakes up next time, it will use the parameter to make hoistway plan
 *******************************************************************************************/
__declspec(dllexport) void HPPlanParking(UCHAR car, 
										 FLOOR_T low_parking_range, 
										 FLOOR_T high_parking_range, 
										 FLOOR_T parking_landing_index, 
										 TIME_T delay, 
										 SHORT option);

}
#endif
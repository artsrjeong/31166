#ifndef	_2D_INTERFACE_FUNCTION_TYPE_
#define	_2D_INTERFACE_FUNCTION_TYPE_

#include "..\COGS\2DInterfaceData.h"  //jsr 2005,07,07 for sharing same header file among COGS,HE,HP
//////////////////////////////////////////////////////////////////////////////////////////
// Type definition used by COGS
/////////////////////////////////////////////////////////////////////////////////////////
// Initialization Function 
/*******************************************************************************************
 * A function call to Group Dispatcher (GD) module to initialize its local variables 
 ********************************************************************************************/
typedef void (*GDInitialize_T)(BuildingInfo *, HoistwayInfo [], CarInfo [],	Parameter *, char *);
/*******************************************************************************************
 * A function call to Group Dispatcher (GD) module to initialize the selected algorithm 
 ********************************************************************************************/
typedef void (*GDAlgorithmInitialize_T)();
/*******************************************************************************************
 * A function call to Hoistway Planner (HP) module to initialize its local variables 
 ********************************************************************************************/
typedef void (*HPInitialize_T)(BuildingInfo *, HoistwayInfo [],CarInfo [],	Parameter *, char *);
/*******************************************************************************************
 * A function call to Hoistway Planner Execution(HPE) module to initialize its local variables 
 ********************************************************************************************/
typedef void (*HPEInitialize_T)(BuildingInfo *, HoistwayInfo [], CarInfo [], Parameter *, char *);

// Assigning New call related
/*******************************************************************************************
 * A function call to the dispatcher module to dispatch the  new call. 
 * This function will evaluate all feasible assignments and return the best assignment
 ********************************************************************************************/
typedef void (*DispatchNewCall_T)(Call new_call, USHORT sequenceNumber,
								  CarState[], TIME_T ctCurrentTime,
								  CHAR manualAssignment);

// Status Monitoring Related
/*******************************************************************************************
 * A function call from COGS to hoistway executer.
 * Update Hoistway Planner about car state
 * Hoistway Executer monitor the progress of hoistway execution, take actions when necessary
 ********************************************************************************************/
typedef void (*MonitorCarState_T)(UCHAR carIndex,
										   TIME_T ctCurrentTime);

//////////////////////////////////////////////////////////////////////////////////////////
// Type defintion used by Dispatcher
//////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************************
 * Typedef for a function call from dispatcher to hoistway planner. The call asked the hoistway planner 
 * to return the scoring of the hoistway plan if the new call is assigned to carIndex.
 ********************************************************************************************/
typedef HPSCORE_T (*ReturnHoistwayPlanScore_T)(UCHAR carIndex,Call new_call, CallList[], CarState[],
											   TIME_T);

/*******************************************************************************************
 * A function call Dispatcher to hoistway planner, indicating the new call should 
 * be assigned to carIndex.
 ********************************************************************************************/
typedef void (*AcceptAssignment_T)(UCHAR carIndex,
									Call assignedCall,
									CarState [],
									TIME_T ctCurrentTime);

/*******************************************************************************************
 * Typedef for function call from GD to HP to park an idle car at a particular location 
  ********************************************************************************************/
typedef void (*HPPlanParking_T)(UCHAR car, 
								FLOOR_T low_parking_range, 
								FLOOR_T high_parking_range, 
								FLOOR_T parking_landing_index, 
								TIME_T delay, 
								SHORT option);


/////////////////////////////////////////////////////////////////////////////////////////////
// Type definition used by Hoistway Planner
/////////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************************
 * A function call to hoistway plan executorr, informing it a new plan is ready for execution
 ********************************************************************************************/
typedef void (*ExecutePlan_T)(UCHAR hwIndex);



//////////////////////////////////////////////////////////////////////////////////////////
// Type defintion used by Hoistway Executor
//////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************************
 * Typedef for function which request the hoistway planner to regenerate the hoistway plan for a particular
 * hoistway. 
 ********************************************************************************************/
typedef void (*RegeneratePlan_T)(UCHAR hoistway_index,TIME_T ctCurrentTime);
/*******************************************************************************************
 * A function call from hoistway plan execution to COGS (OCSS)
 * Inform it about the next operation
 ********************************************************************************************/
typedef void (*OCSSCommand_T)(UCHAR carIndex,
									   CarCommand *oper);
/*******************************************************************************************
 * A function call from hoistway plan executor to Dispatcher
 * It informs accepted car call
 *******************************************************************************************/
typedef void (*GDAcceptCarCall_T)(UCHAR carIndex,
										   FLOOR_T destination);


/////////////////////////////////////////////////////////////////////////////////////////////
// Data Base
/////////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************************
 * A function call to Database for updating car state
 ********************************************************************************************/
typedef void (*PutCarState_T)(UCHAR carIndex,
									   CarState *);

/*******************************************************************************************
 * A function call to Database for updating destination entry calls
 ********************************************************************************************/
/*********************************************************************************
 * Put Call 'Call*' to car 'carIndex' CallList DB
 *********************************************************************************/
typedef void (*PutHallCall_T)(UCHAR carIndex, Call *); 

/*********************************************************************************
 * Delete expected car call of car 'carIndex' from the list
 * Actually clean all expected car calls in the expected car call list of car 'carIndex'
 *********************************************************************************/
typedef void (*DeleteExpectedCarCalls_T)(UCHAR carIndex);

/*********************************************************************************
 * Convert Hall Calls with 'origin' of car 'carIndex' to Expected Car Call 
 * when arriving at origin floor
 *********************************************************************************/
typedef void (*ConvertHallCalls_T)(UCHAR carIndex, FLOOR_T origin, CHAR predirection);

/*********************************************************************************
 * Convert Expected Car Calls with 'destination' to boarded list 
 * when there is door sensor detection of any boarded person.
 *********************************************************************************/
typedef void (*AcceptCarCalls_T)(UCHAR carIndex, FLOOR_T destination);

/*********************************************************************************
 * Remove Call with 'destination' from the boarded list and make them to entry
 * when arriving at the 'destination' floor
 *********************************************************************************/
typedef UCHAR (*RemoveCarCalls_T)(UCHAR carIndex, FLOOR_T destination);

/*******************************************************************************************
 * A function call to Database for retrieving car state
 ********************************************************************************************/
typedef void (*GetCarState_T)(UCHAR carIndex,
									   CarState *);



/*******************************************************************************************
 * A function call to Database for retrieving destination entry calls
 ********************************************************************************************/
typedef void (*GetCalls_T)(UCHAR carIndex,
									CallList *);

/*******************************************************************************************
 * A function call to Database for updating Hoistway Plan
 ********************************************************************************************/
typedef void (*PutPlan_T)(UCHAR hwIndex,
								   HoistwayPlan *);

/*******************************************************************************************
 * A function call to Database for retrieving Hoistway Plan
 ********************************************************************************************/
typedef void (*GetPlan_T)(UCHAR hwIndex,
								   HoistwayPlan *);


/*******************************************************************************************
 * OTHERS
 ********************************************************************************************/
typedef void (*GDCancelHallCalls_T)(UCHAR carIndex,
											 FLOOR_T origin);
typedef void (*HPECancelHallCalls_T)(UCHAR carIndex,
											  FLOOR_T origin);
typedef void (*ExpectedCarCalls_T)(FLOOR_T destination);

typedef void (*HPEAcceptCarCall_T)(UCHAR carIndex,
										    FLOOR_T destination);
typedef void (*GDCancelCarCalls_T)(UCHAR carIndex,
											FLOOR_T destination);
typedef void (*HPECancelCarCalls_T)(UCHAR carIndex,
											 FLOOR_T destination);
/*******************************************************************************************
 * A function call from Group Dispatcher (GD) to COGS (DES)
 * simply to store the assignment to a local variable in COGS
 * until the DispatchNewCall function returns
 ********************************************************************************************/
typedef void (*ReturnAssignmentToPassenger_T)(USHORT sequenceNumber,
									   CHAR carIndex);

/* A function call to manage timing information for hoistway plan */
typedef void (*ManageStatusTiming_T)(int carid,CarState *currentCarState,int from);

/* A function call to store boarding and deboarding at stop floor for timing */
typedef void (*GetBoardDeboard_T)(int car,int *board,int *deboard);

/* A function call to retrieve timing event information */
typedef void (*GetStatusTiming_T)(int carid,int *oper,double *timing,int *floor,int *board,int *deboard);

#endif

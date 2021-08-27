// ************************Hoistway Planner DLL function prototypes **********************************
#ifndef	_HP_DLL_DECL_
#define	_HP_DLL_DECL_
#include "HoistwayPlannerDefs.h"
extern "C"
{
/******************************************************************************************* 
 * A function call to HoistwayPlanner DLL for initializing specification data like building informaiton
 *   and other information. Information is defined in the HoistwayPlannerDefs.h
 ********************************************************************************************/
__declspec(dllexport) int InitializeHoistwayPlanner(PlannerSpec *spec);

/******************************************************************************************* 
 * A function call to HoistwayPlanner DLL for updating current status
 *******************************************************************************************/
__declspec(dllexport) void updateStatus(double currentTime,HPCarStatus carState);

/******************************************************************************************* 
 * A function call to HoistwayPlanner DLL for assigning new call and get the hoistway plan
 *******************************************************************************************/
__declspec(dllexport) T_HPSCORE assignNewCall(double currentTime,int car,AssignedDestinationEntryCall newAssignedCall,
										HoistwayPlan *generatedPlan);

/*******************************************************************************************
 * A function call to HoistwayPlanner DLL to regenerate Plan
 *******************************************************************************************/
__declspec(dllexport) T_HPSCORE regeneratePlan(double currentTime,int car,HoistwayPlan *generatedPlan);

/*******************************************************************************************
 * A function clal to avoid
 *******************************************************************************************/
__declspec(dllexport) T_HPSCORE cancelExpectedCarCall(double currentTime,int car,int floor,HoistwayPlan *genPlan);

/*******************************************************************************************
 * A function call to the dispatcher module to dispatch the first new call. Assuming the call
 * has been put in GlobalDestinationEntryCalls and unssigned_list point to the call
 * This function will read car status and global
 * desitnation entry call, construct necessary data structures, try various assignments to 
 * find out the best assignment and returns the result
 ********************************************************************************************/
//__declspec(dllexport) int DispatchNewCall();


/*******************************************************************************************
 * A function call from dispatcher to hoistway planner. The call asked the hoistway planner 
 * to return the scoring of the hoistway plan if the new call is assigned to car_index.
 * The parameter plan contains the hoistway plan when the function returns
 * The assumption is that call will be assigned one at a time. Hence the call pointed by 
 * unassigned_list in GlobalDestinationEntry will be the one to assign 
 ********************************************************************************************/
//__declspec(dllexport) int HoistPlanScoreWithAssignment(unsigned char car_index,
//													   HoistwayPlan	&plan);

/*******************************************************************************************
 * A function call from dispatcher to hoistway planner. 
 * This call asked the hoistway planner to regenerate the hostway plan for a particular
 * hoistway. The current state get be read from GlobalDestinationEntryCalls and CarState
 * This paramenter plan contains the regenerated plan when the function returns
 ********************************************************************************************/
//__declspec(dllexport) int RegenerateHoistPlan(unsigned char hoistway_index,
//													   HoistwayPlan	&plan);

/*******************************************************************************************
 * A function call from COGS or Dispatcher hoistway planner, indicating the new call should 
 * be assigned to car_index. The hoistway executer should update the GlobalDestinationEntry
 * accordingly after receive this call. 
 ********************************************************************************************/
//__declspec(dllexport) int AssignCallToCar(unsigned char car_index);
}
#endif
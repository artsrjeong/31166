// ************************Hoistway Planner DLL function type def for caller **********************************
#ifndef	_HP_DLL_DEFS
#define	_HP_DLL_DEFS
#include "HoistwayPlannerDefs.h"

extern "C"
{
// InitializeHoistwayPlanner
typedef	int (*HPInit)(PlannerSpec *spec);
// upateStatus
typedef void (*HPStatusUpdate)(double currentTime,HPCarStatus carState);
// assignNewCall
typedef	T_HPSCORE	(*HPnewAssign)(double currentTime,int car, 
				AssignedDestinationEntryCall newAssignCall, HoistwayPlan *generatedPlan);
// regeneratePlan
typedef	T_HPSCORE	(*HPreplan)(double currentTime,int car,HoistwayPlan *generatedPlan);
// cancel expected car call
typedef T_HPSCORE	(*HPcancalCarCall)(double currentTime,int car, int floor, HoistwayPlan *generatedPlan);
}	

#endif
// HoistwayPlanSelection.H
#ifndef	_HPS_H
#define	_HPS_H
#include <stdio.h>
#include "Hoistway_Plan.h"
#include "HoistwayPlannerDefs.h"

#define	TRUE	1
#define	FALSE	0

#define	PASSENGER_DEBOARDED	2
#define	PASSENGER_BOARDED	1
#define	PASSENGER_WAITING	0
// Select best hoistway plan from the candidate
/*  Return Value:
 *		TRUE : valid plan is selected
 *		FALSE : no valid plan to return
 *	Argument : 
 *		candidateSize : the number of candidate hoistway plan to consider
 *		*candidatePlan : candidate hoistway plan
 *		*bestPlan : selected best plan to return
*/
HPSCORE_T	HoistwayPlanSelection(int upperCarID,int lowerCarID, int candidateSize,
			HoistwayPlan *candidatePlan,HoistwayPlan *bestPlan);

#endif
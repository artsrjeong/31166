#ifndef _COMMON_H_INCLUDED
#define _COMMON_H_INCLUDED
#include "..\COGS\All.h"

//##ModelId=430D77D00152
#define MAXBREAKPOINT     20
#define EPSILON			1.0E-8
#define NUM_BREAK_MAX_VELOCITY		8
#define NUM_BREAK_MAX_ACCELERATION	7
#define NUM_BREAK_NEITHER			5
#define MAXFLOOR   140
#define UP				1
#define DN				-1
#define NO_DIR			0
#define NO_PREDIRECTION	-2
#define MAXDIR				2
#define UP_INDEX			0
#define DN_INDEX			1
#define INVALIDFLOOR	255
#define PRECISION		1.0E-5  // precision for GetTraversalTime computation


#define TSA_POSITION 0
#define TSA_NSP      1
#define TSA_NSF		 2
#define TSA_NCP		 3
#define TSA_NCF      4

#define START_DELAY_PERIOD -1
#define STOP_DELAY_PERIOD -2
#define ADDITIONAL_DELAY 100  // additional delay for car not to stop in conditional stop 

#define COGS_UNIT_TEST

extern void SpyCar(unsigned char car,int cmd_target,int cmd_door,int cmd_preDir) ;

#endif

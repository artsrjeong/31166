/* U10 Project: @(#)infocs.h 1.23/23/95 14:22:52 */
/*********************************************************************
;  File Name:  INFOCS.H - Public Data Definition for INFOCS.C
;
;  File Description:
;  This file describes the INFOCS.C public data.  This file is to be
;  included in any file that uses these services.
;
;
;  This work and the information it contains are the confidential
;  property of Otis Elevator Company ("Otis").  It is delivered on
;  the express condition that it will be used only for, or on
;  behalf of, Otis; that neither it nor the information it contains
;  will be reproduced, distributed or disclosed, in whole or in part,
;  without the written consent of Otis; and that on demand it and any
;  copies will be promptly returned.
;
;  Unpublished work - Copyright 1995 Otis Elevator Company
;  All rights reserved.
;
;
;  Revision History:
;
;  SCN                   Author	                       Date
;  ------------------------------------------------------------------------
;  A1730203BAA           Geoff Mochau                  02 February 1995
;  - SCR 30203-38: Created (renamed) module from "infocs_p.h".
;
;  Transamerica          Geoff Mochau                  17 February 1995
;  - Reduced JICA_FLOORS to limit RAM requirements.
;
;  J1830203AAA           H.Honma(Suzuki)                      29  May  1995
;  30203-5017: Change call mask variables type from BYTE to INTEGER
;
;  J1830203AAA           H.Honma(Suzuki)                      08  June 1995
;  30203-5002: Support Dual Up Peak
;
;  J1830203AAA           H.Honma(Suzuki)                      11  June 1995
;  - SCR 30203-5035: Support Door Re-Open feature for dispatching hall
;  - SCR 30203-5036: Take into account Nudging state of door
;
;  J1930203AAA           H.Honma                              27  June 1995
;  - SCR 30203-5027: Multi Lobby Parking
;
;  J2130203AAA           Hideyuki Honma                     31 Nov 1995
;  - SCR 30203-5029: Dispatch during Channeling
;
;  AAB-30203-3AA         Mark Ross                     21 Dec 1995
;  - SCR 30203-228: OTI SCR 5029.  Dispatch during Channeling
;
;  AAB-30203-4AA         Jannah Stanley              23 Jan 1996
;  - SCR 30203-256:   Add hall lantern active (hl_active) array to OCSS structure
;		        Corrected comment describing assigned_calls variable
;
;  JAB-30203-AAE         Hideyuki Honma              17 Jan 1999
;  - SCR 30203-5xxx:   Add oldFinalLoad,d_passengers,InCarPassengers for V-MIT
;
;  A11-31017-AAA        S. Dridi                        28-Jan-04
;  - SCR 31017-xx change max landings to 63 update modes
;
;  A11-31017-AAA        S. Dridi                        28-Jan-04
;  - SCR 31017-19  Ring ICD 5.1 Update modes
;
********************************************************************/
#ifndef INFOCS_INCLUDED  /* protects from multiple includes */
#define INFOCS_INCLUDED

#include "timer.h"  /* timer type definition */

#include "E2P.H"
/*********************************************************************
;       Car Constants
;********************************************************************/

#define         CAR1    1       /* Car 1 origin id        */
#define         CAR2    2       /* Car 2 origin id        */
#define         CAR3    3       /* Car 3 origin id        */
#define         CAR4    4       /* Car 4 origin id        */
#define         CAR5    5       /* Car 5 origin id        */
#define         CAR6    6       /* Car 6 origin id        */
#define         CAR7    7       /* Car 7 origin id        */
#define         CAR8    8       /* Car 8 origin id        */

#define         ALL_CARS  255   /* All cars mask          */

#define         LOAD0   0       /* Load state 0     */
#define         LOAD1   1       /* Load state 1     */
#define         LOAD2   2       /* Load state 2     */
#define         LOAD3   3       /* Load state 3     */
#define         LOAD4   4       /* Load state 4     */
#define         LOAD5   5       /* Load state 5     */

#define         NAV		0       /* Not available		*/
#define         EPC		1       /* Emergency power correction	*/
#define         COR		2       /* Correction			*/
#define         EFS		3       /* SES phase II			*/
#define         EFO		4       /* SES phase I			*/
#define         EQO		5       /* Earthquake operation		*/
#define         EPR		6       /* Emergency power rescue	*/
#define         EPW		7       /* Emergency power wait for NOR */
#define         OLD		8       /* Overload			*/
#define         ISC		9       /* Independent service		*/
#define         ATT		10      /* Attedent service		*/
#define         DTC		11      /* Door time close protection	*/
#define         DTO		12      /* Door time open protection	*/
#define         CTL		13      /* Car to landing		*/
#define         CHC		14      /* Cutoff hall calls		*/
#define         LNS		15      /* Load non-stop		*/
#define         MIT		16      /* Moderate incoming traffic	*/
#define         DCP		17      /* Delayed car			*/
#define         ANS		18      /* Anti-nuisance		*/
#define         NOR		19      /* Normal			*/
#define         ARD		20      /* Automatic return		*/
#define         PRK		21      /* Park				*/
#define         IDL		22      /* Idle				*/
#define         PKS		23      /* Park switch			*/
#define         GCB		24      /* General control of buttons	*/
#define         EHS		25      /* Emergency hospital service	*/
#define         ROT		26      /* Riot				*/
#define         INI		27      /* Initialization		*/
#define         INS		28      /* Inspection			*/
#define         ESB		29      /* Emergency stop button	*/
#define         DHB		30      /* Door hold button		*/
#define         ACP		31      /* Anti-crime protection	*/
#define         WCO		32      /* Wild car protection		*/
#define         DBF		33      /* Drive brake failure		*/
#define         SAB		34      /* Shabat service		*/
#define         EFP		35      /* SES power interuption 	*/
#define         CRL		36      /* Secure than shut down	*/
#define         CRO		37      /* Card reader operaiton	*/
#define         CES		38      /* Car call express service	*/
#define         DOS		39      /* Door open hold switch	*/
#define         WCS		40      /* Wheel chair service		*/
#define         REC		41      /* Recover operation		*/
#define         OHT		42      /* Overheat operations		*/
#define         ARE		43      /* Automatic rescue operation	*/
#define		EPD		44	/* Manual Emergency power wait  */
#define     GAP     45      /* Motor column abnormal    */
#define     HBP     46      /* Hall button protection   */
#define		OOS		47	/* Out of Service		*/
#define		SCX		48	/* Shuttle car operation	*/
#define		EMT		49	/* Emergency med. car retrevial */
#define		EMK		50	/* Emergency med. car control   */
#define		EPT		51	/* Emergency power transfer     */
#define		SCO		52	/* Swing car operation		*/
#define     CHN     64      /* Channelling mode         */
#define		EPO		65	/* Emergency power operation	*/
#define		ERO		66	/* Emergency rescue operation   */
#define		CBP		67	/* Car button protection	*/
#define		DDO		68	/* Disable door open		*/

// Add New Operation    13May2003
// Especially for Revo,Eco..
#define         PFO             70      /* Pit Flood operation          */
#define         CDO             71      /* Cooler Drain operation       */
#define         SRO             72      /* Separate Riser Operation     */
#define         DLF             73      /* Door Lock Failure            */
#define         LPS             74      /* Low Oil Pressure             */
#define         STP             75      /* Car Stall Protection         */
#define         RCY             76      /* Telescopic Cylinder Recycle  */
#define         REI             77      /* Remote Elevator Inspection   */
#define         KEH             78      /* Kobe Emergency Hospital      */
#define         LOO             79      /* Low Oil operation      */
#define         SRF             80      /* Slacken Rope Function        */
#define         BKF             81      /* Brake Function               */
#define         CAO             82      /* Cylinder Adjustable operation*/
#define         ACO             83      /* Anti Crime operation         */
#define         EPS             84      /* Express Priority Service     */
#define         WRM             85      /* Warm operation               */
#define         OIC             86      /* Outside Inspection Car       */
#define         KIS             87      /* Kobe Independent Service     */
#define         KNO             88      /* Kobe No Operation            */



#define         ARS             89      /* Automatic Rescue Service    */

#define         ARO            103      /* Automatic Rescue Operation  */
#define         DLM            104      /* Door Lock Monitoring        */
#define         EAR            105      /* Emergency Automatic Rescue  */
#define         HAD            106      /* Hoistway Access Detection   */
#define         DAR            107      /* Drive Fault with Automatic Run */
#define         DCS            108      /* Door Check Sequence         */
#define         UFS            109      /* Up Final Limit Switch       */

#define			ONE_UP_ONE_DOWN	110		/* One up One down policy -SeongRak */

#define		NUL		127	/* Null opmode, don't care.     */

#define         RING1           1       /* Ring 1			*/
#define         RING2           2       /* Ring 2			*/
#define         RING_1_2        3       /* Ring 1 and 2			*/

#define		MAX_OCSS	9	/* maximum # of ocss's		*/

#define		TLL_ON		0x20	/* TLL bit on			*/
#define		TLL_OFF		0xdf	/* TLL bit off			*/

#define         MAX_PASSENGERS  31      /* define maximum passenger cnt */
#define         INTERFLOOR_MODE 0       /* mixed traffic mode           */
#define         UP_PEAK_MODE    1       /* up peak traffic mode         */
#define         DOWN_PEAK_MODE  2       /* down peak traffic mode       */
#define         NOON_PEAK_MODE  3       /* up + down peak traffic mode  */

#define         INVALID_POSITION 0x7f   /* invalid MCSS position        */


#ifdef U10
#define     JICA_FLOORS E2P_MAX_LANDINGS   /* number of landings supported */      /* scr31017-2 */
#else
#define 	JICA_FLOORS	22	/* number of landings supported */
#endif

#define		STOP_NOT_IN_PROGRESS 	0 /* No Stop in progress */
#define     STOP_NOT_STARTED        1 /* door cycle not yet started */
#define     STOP_IN_PROGRESS        2 /* Stop in progress, in door cycle */

/*********************************************************************
; OCSS Structure
; This structure is used in the OCSS_S type definition.  This type
; definition will be used by the client module to create an instance
; of an ocss.
;
; This type definition is used by the client module to create an
; instance of a ocss.
;********************************************************************/
struct OCSS_S  {
  INT_T    allowed_mask[JICA_FLOORS];	   /* calls allowed mask   */
  INT_T    assigned_calls[JICA_FLOORS];	   /* calls assigned by central group to car */
  INT_T    future_calls[JICA_FLOORS];      /* calls will be serviced next cycle */
  INT_T    hl_active[JICA_FLOORS];   	   /* hall calls car knows it is assigned to */
  BYTE_T   bottom_pos;                     /* ocss bottom landing          */
  BYTE_T   lobby_pos;                      /* ocss lobby landing           */
  BYTE_T   top_pos;                        /* ocss top landing             */
  BYTE_T   load_state;	                   /* ocss load state		*/
  BYTE_T   pre_dir;		                   /* ocss pre-direction		*/
  BYTE_T   mov_dir;		                   /* ocss moving direction	*/
  BYTE_T   old_mov_dir;		               /* old ocss moving direction	*/
  BYTE_T   actual_pos;		               /* actual ocss floor		*/
  BYTE_T   old_actual_pos;		           /* old actual ocss floor	*/
  BYTE_T   lastStopPos;		               /* previous stop pos		*/
  BYTE_T   NCF;			                   /* next commitable landing      */
  BYTE_T   f_door_state;	               /* front door state		*/
  BYTE_T   r_door_state;	               /* rear door state		*/
  //DBD 17Feb04
  BYTE_T   f_oldDoorState;					/* old front door state		*/
  BYTE_T   r_oldDoorState; 					/* old rear door state		*/
  INT_T    load_wt_counts; 					/* load weight counts		*/
  BYTE_T   d_min;							/* deboard minimum (% load)	*/
  BYTE_T   b_max;							/* boarding maximum (% load)	*/
  BYTE_T   b_passengers;					/* boarding passengers		*/
  BYTE_T   oldFinalLoad;					/* Old Boarding Maximum (% load)*/
  BYTE_T   d_passengers;					/* deboarding passengers	*/
  BYTE_T   inCarPassengers;					/* InCar Passengers     	*/
//Kourakuen
  //jsr
  BYTE_T oper;           // current operation: 0=idle, 1=door opening, 2=dwell, 3=door closing,
                      //   4=running, 5=forced move, 6=forced parking, 7=temporarily after end
                      //   of forced parking, 8=waiting for move initiation, 9=waiting for move
                      //   release
  BYTE_T	car_status;
  BYTE_T	hold_pos;
  FLAG_T	priority;						//main,sub,none
  BYTE_T	next_up;						//nearest up stop
  BYTE_T	next_down;						//nearest down stop
  //jsr
  BYTE_T   incar_p;							/* InCar Passengers(RealTime:Type7Sub7) */
  TIMER_T  ring1_T0_tout;					/* ring1 msg. type 3 timeout 	*/
  TIMER_T  ring2_T0_tout;					/* ring2 msg. type 3 timeout    */
  TIMER_T  ring12_T0_tout;					/* ring1 & 2 msg. type 0 tout   */
  TIMER_T  tll_value;						/* traffic leaving lobby value  */
  TIMER_T  tll_tout;						/* traffic leaving lobby timer  */
  FLAG_T   tll_status;						/* traffic leaving lobby status */
  FLAG_T   old_tll_status;					/* traffic leaving lobby status */
  INT_T    car_capacity;					/* car rated load capacity	*/
  BYTE_T   opmode;							/* ocss mode			*/
  BYTE_T   source_ring;						/* source ring for this ocss    */
  BYTE_T   ez_bottom_pos;					/* express zone bottom landing  */
  BYTE_T   ez_top_pos;						/* express zone top landing     */
  BYTE_T   ez_runtime;						/* express zone run time   	*/
  BYTE_T   ez_type;							/* 1 if ez has dummy floors     */
  FLAG_T   car_in_ez_flag;         			/* express zone status          */
  BYTE_T   call_allowed_mask;      			/* call types allowed for car   */
  BYTE_T   mgset_status;					/* MG set status   		*/
/* 30203-506 +START+ Change call mask variables type from BYTE to INTEGER */
  FLAG_T   dnhall_flag;
  FLAG_T   uphall_flag;
  INT_T    atsummary;              			/* pending call type at car     */
  INT_T    upsummary;              			/* pending call types above car */
  INT_T    dnsummary;              			/* pending call types below car */
  INT_T    allsummary;             			/* all pending call types       */
  INT_T    stop_type;						/* type of call being served    */
/* 30203-506 +END+ Change call mask variables type from BYTE to INTEGER */
  BYTE_T   stop_pos;						/* position of call served	*/
  BYTE_T   stop_status;						/* status of car stop		*/
  FLAG_T   decel_flag;             			/* status of car decelerating   */
  FLAG_T   motion_status;          			/* motion status                */
  FLAG_T   target_pos;             			/* target ocss floor            */
  BYTE_T   crowd_last_pos;         			/* Last landing of crowd        */
  BYTE_T   crowd_last_direction;   			/* Last crowd direction         */
  BYTE_T   reopen_cnt;
  BYTE_T   nudging_detect;
  BYTE_T   second_lobby_pos;
//kagaya
  BYTE_T   oldNextCom;
  BYTE_T   oldOpMode;
//DBD-honma 20Mar04
  BYTE_T   carMaxPassenger;	        		/* car maximum passenger	*/
  BYTE_T   carEffPassenger;	        		/* car effective passenger	*/
  FLAG_T   upper_car_flag;					/* upper_car or lower_car   */
};
typedef struct OCSS_S OCSS_T;

/*****************************************************************
; Global OCSS Information - Declare all instance of ocss (1..8)
******************************************************************/
EXTERN OCSS_T ocss_info[MAX_OCSS];

EXTERN FLAG_T ocss_mit_clock_flag;
EXTERN FLAG_T ocss_mit_status_flag;
EXTERN FLAG_T ocss_dup_clock_flag;
EXTERN FLAG_T ocss_mot_status_flag;
//Shizuoka, at 00/03/14
EXTERN FLAG_T ocss_mitSp1_clock_flag;
EXTERN FLAG_T ocss_mitSp2_clock_flag;
EXTERN FLAG_T ocss_adf1_flag;

//SGO,Shindongah
EXTERN FLAG_T ocss_sgo1_clock_flag;
EXTERN FLAG_T ocss_sgo2_clock_flag;

//SRO,Shindongah
EXTERN FLAG_T ocss_sro1_clock_flag;

//Fixed-Park,Tohoku
EXTERN FLAG_T ocss_parkFixed_clock_flag;

EXTERN FLAG_T ocss_2nd_lobby_flag;
EXTERN FLAG_T ocss_2nd_lobby_type;
EXTERN FLAG_T ocss_chn_status_flag;

//OsakaObp
EXTERN FLAG_T ocss_dpc_clock_flag,ocss_dpc2_clock_flag;

EXTERN FLAG_T ocss_eso_clock_flag,ocss_lunch_clock_flag;
EXTERN FLAG_T ocss_crowd_clock_flag,ocss_sps_clock_flag;

EXTERN FLAG_T ocss_allParkLobbyClockFlag;


EXTERN BYTE_T ocss_MitPriority[MAX_OCSS];
EXTERN BYTE_T gcss_MitPriority[MAX_OCSS];

EXTERN BYTE_T ocss_IdlTimer[MAX_OCSS];
EXTERN FLAG_T ocss_ParkEnable[MAX_OCSS];
EXTERN FLAG_T ocss_AssignEnable[MAX_OCSS];


/*********************************************************************
;  Function Prototypes
;********************************************************************/
void   O_init     (OCSS_T *O_ptr);	/* initialization function  */
BYTE_T cars_available_count(void);
BYTE_T cars_available_mask(void);

#endif  /* end of include file definition */




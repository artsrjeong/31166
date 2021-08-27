/* U10 Project: @(#)infbld.h	1.2	3/23/95	14:22:46 */
/*********************************************************************
;  File Name:  INFBLD.H - Public Data Definition for INFBLD.C
;
;  File Description:
;  This file describes the INFBLD.C public data.  This file is to be
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
;  - SCR 30203-38: Created (renamed) module from "infbld_p.h".
;
;  Transamerica          Geoff Mochau                  16 February 1995
;  - Reduced MAX_LANDINGS to limit RAM requirements.
;
;  J1830203AAA           H.Honma(Suzuki)                     29 May 1995
;  - SCR 30203-5017
;   Change call mask vaiables type from BYTE to INTEGER
;
;  J1830203AAA           H.Honma(Suzuki)                     08 June 1995
;  - SCR 30203-5002
;   Support Dual Up Peak
;
;  J1930203AAA           H.Honma                             23 June 1995
;  - SCR 30203-5027: Multi Lobby Parking
;
;  J2130203AAA           Hideyuki Honma               22 Sep 1995
;  - SCR 30203-5055: Crowd Floor Service with EMS
;  - SCR 30203-5057: WCS operation
;
;  AAB302033AA        Mark Ross                          17 Oct 1995
;  - SCR 30203-243: OTI SCR 5055. Crowd floor service by EMS.
;  - SCR 30203-244: OTI SCR 5057. WCS operation.
;
;  J2130203AAA           Hideyuki Honma                     31 Nov 1995
;  - SCR 30203-5029: Dispatch during Channeling
;
;  AAB-30203-3AA         Mark Ross                     21 Dec 1995
;  - SCR 30203-228: OTI SCR 5029.  Dispatch during Channeling
;
;  A11-31017-AAA        S. Dridi                        28-Jan-04
;  - SCR 31017-002 ADD DBD parameters, change max landings to 64
;
;********************************************************************/
#ifndef INFBLD_INCLUDED  /* protects from multiple includes */
#define INFBLD_INCLUDED


#include "infocs.h"
#include "E2P.H"
//#import "E2P.H" // shs
/*********************************************************************
; Building Constants
;********************************************************************/

#define         BOTTOM          0   /* Bottom landing       */
#define         MAX_LANDINGS    E2P_MAX_LANDINGS  /* Maximum number of landings   */      /* scr31017-2 */


#define         MAX_FLOOR_SEG   7       /* maximum segments (landings/10) */              /* scr31017-2 */
#define     ALL_LANDINGS    127 /* all landings             */

#define     RSR_MAX         0x3ff0  /* Maximum RSR value        */
#define     RSR_WAIT        0x3fc0  /* Wait RSR value       */
#define     RSR_MIN         0x0001  /* Minimum RSR value        */
#define     RSR_MAX_PLUS_1  0x3ff1  /* Maximum RSR value plus one   */

#define         RSR_IMMEDIATE       0x0000  /* Immediate RSR value          */
#define         RRT_MAX             0xfff /* maximum RRT value          */
#define         RRT_WAIT            0xfe  /* wait RRT value             */
#define         RRT_HALL_STOP       0x0B  /* Hall stop value = 11       */
#define         RRT_CAR_STOP        0x0A  /* car stop value = 10        */
#define         RRT_WCS_STOP        0x0F  /* car stop value = 15        */
#define         RRT_MG_START        0x14  /* mg start value = 20        */
#define         RRT_DOORS_OPENING   0x06  /* door opening time          */
#define         RRT_DOORS_OPENED    0x04  /* door opened time           */
#define         RRT_DOORS_CLOSING   0x02  /* door closing time          */
#define         RRT_SHORT_RUN       0x05  /* car stopping period        */
#define         RRT_FULL_RUN        0x01  /* car full speed run, 1 ldg  */
#define         RRT_REVERSE         0x03  /*                            */
#define         RRT_UNSURE_DISTANCE 0x02  /* unsure distance            */

/* The following bit mask positions MUST be the same as the OCSS        */
/* for proper translation of ring message contents.                     */
#define         S_FCC             0x01    /* front up car call          */
#define         S_FUHC            0x02    /* front up hall call		*/
#define         S_FDHC            0x04	  /* front down hall call       */
#define         S_FEHC            0x08    /* front emergency hall call  */
#define         S_RCC             0x10    /* rear car call             */
#define         S_RUHC            0x20	  /* rear up hall call		*/
#define         S_RDHC            0x40	  /* rear down hall call	*/
#define         S_REHC            0x80    /* rear emergency hall call   */

#define         E_FCC             0x0100  /* front up car call          */
#define         E_FUHC            0x0200  /* front up hall call		*/
#define         E_FDHC            0x0400  /* front down hall call	*/
#define         E_FEHC            0x0800  /* front emergency hall call  */
#define         E_RCC             0x1000  /* rear car call              */
#define         E_RUHC            0x2000  /* rear up hall call		*/
#define         E_RDHC            0x4000  /* rear down hall call        */
#define         E_REHC            0x8000  /* rear emergency hall call   */

#define         S_UHC           (S_FUHC + S_RUHC)
#define         S_DHC           (S_FDHC + S_RDHC)
#define         S_HC	        (S_FUHC + S_RUHC + S_FDHC + S_RDHC)
#define         S_CC            (S_FCC  + S_RCC)
#define         S_EHC           (S_FEHC + S_REHC)

#define         E_UHC           (E_FUHC + E_RUHC)
#define         E_DHC           (E_FDHC + E_RDHC)
#define         E_HC            (E_FUHC + E_RUHC + E_FDHC + E_RDHC)
#define         E_CC            (E_FCC  + E_RCC)
#define         E_EHC           (E_FEHC + E_REHC)

// a_ prefix used for "any" hall call std or extended  for front doors only
#define         A_FUHC           (S_FUHC + E_FUHC)
#define         A_FDHC           (S_FDHC + E_FDHC)
#define         A_RUHC           (S_RUHC + E_RUHC)
#define         A_RDHC           (S_RDHC + E_RDHC)
#define         A_FCC            (S_FCC  + E_FCC)
#define         A_RCC            (S_RCC  + E_RCC)

#define         A_FHC            (A_FUHC + A_FDHC)
#define         A_RHC            (A_RUHC + A_RDHC)
#define         A_UHC            (A_FUHC + A_RUHC)
#define         A_DHC            (A_FDHC + A_RDHC)
#define         A_HC             (A_FUHC + A_FDHC + A_RUHC + A_RDHC)
#define         A_CC             (A_FCC  + A_RCC)

#define         MAX_RISER       3
#define         RISER_ONE       0
#define         RISER_TWO       1

/*********************************************************************
; BUILDING Structure
; This structure is used in the BUILDING_S definition.  This type
; definition will be used by the client module to create an instance
; of a BUILDING.
;
; This type definition is used by the client module to create an
; instance of a BUILDING. Each occurance of a building contains the
; number of landings, the lobby landing, the status of hall calls
; (UHC, DHC, RUHC, RDHC) and the lowest RSR value associated with
; that call.
;********************************************************************/
struct HALL_CALL_S {
	INT_T           low_rsr;
	INT_T           low_rrt;
	FLAG_T         	dispatch;
	BYTE_T		reg_elapsed;
	BYTE_T		reassigned_count;
	INT_T           last_boarding_weight;
	BYTE_T          crowd_interval;
	BYTE_T          crowd_current_estimate;
	LONG_T          crowd_departure_time;
	LONG_T          crowd_registration_time;
	LONG_T          crowd_previous_interval;
	BYTE_T          reg_elapsed_car[MAX_OCSS]; // DBD-honma 05Apr04  For MultiAssignment
	LONG_T			reg_car[MAX_OCSS];  //jsr 050818
};
typedef struct HALL_CALL_S HALL_CALL_T;

struct BUILDING_S  {
				INT_T	 nCars;
                INT_T    ocss_lobby_pos;
                INT_T    second_lobby_pos;
                INT_T    ads_lobby_pos;
                INT_T    dup_common_pos;
                INT_T    top_pos;
                INT_T    bottom_pos;
				BYTE_T   lz_top_pos;						/* lower zone top landing       */
				BYTE_T   uz_bottom_pos;					/* upper zone bottom landing	*/

                INT_T		up_reg_time;
                INT_T		dn_reg_time;
                INT_T		up_ave_reg;
                INT_T		dn_ave_reg;
                INT_T		total_ave_reg;
                INT_T		up_hall_calls;
                INT_T		dn_hall_calls;
                INT_T		total_hall_calls;
                HALL_CALL_T     fuhc[MAX_RISER][MAX_LANDINGS];    /* info about fuhc */
                HALL_CALL_T     fdhc[MAX_RISER][MAX_LANDINGS];	   /* info about fdhc */
                HALL_CALL_T     ruhc[MAX_RISER][MAX_LANDINGS];	   /* info about ruhc */
                HALL_CALL_T     rdhc[MAX_RISER][MAX_LANDINGS];	   /* info about rdhc */
//dojima
                HALL_CALL_T     e_fuhc[MAX_RISER][MAX_LANDINGS];  /* info about extended fuhc */
                HALL_CALL_T     e_fdhc[MAX_RISER][MAX_LANDINGS];  /* info about extended fdhc */
                HALL_CALL_T     e_ruhc[MAX_RISER][MAX_LANDINGS];  /* info about extended ruhc */
                HALL_CALL_T     e_rdhc[MAX_RISER][MAX_LANDINGS];  /* info about extended rdhc */

                INT_T          new_hc_mask[MAX_RISER][MAX_LANDINGS]; /* new HC mask */
                INT_T          old_hc_mask[MAX_RISER][MAX_LANDINGS]; /* old HC mask */
};
typedef struct BUILDING_S BUILDING_T;

/******************************************
; Global Building Information
*******************************************/
EXTERN BUILDING_T bld_info;

struct GROUP_S  {
		INT_T          new_hc_mask[3][MAX_LANDINGS];
		INT_T          old_hc_mask[3][MAX_LANDINGS];
};
typedef struct GROUP_S GROUP_T;
EXTERN GROUP_T grp_info;

struct SECTOR_S  {
		BYTE_T          start;
		BYTE_T          end;
};
typedef struct SECTOR_S SECTOR_T;
EXTERN SECTOR_T Sector_Info[MAX_OCSS];

EXTERN BYTE_T Sector_Assign[MAX_OCSS];
EXTERN BYTE_T No_of_Sector;
EXTERN BYTE_T Group_No[MAX_OCSS];
EXTERN BYTE_T Group_old_No[MAX_OCSS];
EXTERN FLAG_T dup_enable_flag;
EXTERN BYTE_T OCSS_Crowd_Floor[6];

//Toshimaku Job with EMS
EXTERN BYTE_T infbld_EmsDoubleAssignSignal;
EXTERN BYTE_T infbld_EmsPreCrowdSignal;
EXTERN BYTE_T infbld_EmsUTurnSignal;
EXTERN BYTE_T infbld_EmsAutoLobbyHallSignal;
EXTERN BYTE_T infbld_EmsDoorOpenParkSignal;

EXTERN BYTE_T infbld_SgoGroupCar[MAX_OCSS];

//SGO
EXTERN FLAG_T infbld_Sgo1EnableFlag,infbld_Sgo2EnableFlag;

//SRO  Cyuou Mitsui 00/10/17
EXTERN FLAG_T infbld_Sro1EnableFlag;
EXTERN BYTE_T infbld_SroLowPos,infbld_SroHighPos;

EXTERN FLAG_T ems_mit_clock_flag;
EXTERN BYTE_T rrt_hdcp_hall_stop;
EXTERN BYTE_T rrt_hdcp_car_stop;


/*********************************************************************
;  Function Prototypes - initialization of building structure.
;********************************************************************/
void    B_init (BUILDING_T *B_prt);
void    G_init (GROUP_T *G_prt);
void    S_init (void);


#endif  /* end of include file definition */



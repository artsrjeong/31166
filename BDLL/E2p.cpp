#include "stdafx.h"
#include "common.h"

#define EXTERN extern
#include "Inferr.h"
#include "density.h"
#undef EXTERN

#define EXTERN
#include "e2p.h"
#undef EXTERN

/**************************************************************************
;  Name: e2p_init
;
;  Description: Initialize EEPROM pointers on POR.
;
;  Parameters: none
;*************************************************************************/
void e2p_init(void)

{
	LONG_T e2pBackTmp1,e2pBackTmp2;
	FLAG_T BackupLoadedFlag = FALSE;

	
   /* D E F I N E   A D S S   P A R A M E T E R S */
   /* ------------------------------------------- */
   e2p_ads =   		(BYTE_T *)E2P_ADSS_LOC;
   e2p_ads_call_pred =  (BYTE_T *)E2P_ADS_CALL_PRED;


   /* D E F I N E   D B D   P A R A M E T E R S         scr31017-2*/
   /* ------------------------------------------- */
   e2p_DbdEnable                  = (BYTE_T *)e2p_DbdEnablePtr;
   e2p_DbdNackMax                 = (BYTE_T *)e2p_DbdNackMaxPtr;
   e2p_DbdPassStatTimeout         = (BYTE_T *)e2p_DbdPassStatTimeoutPtr;

   e2p_DbdDistanceType            = (BYTE_T *)e2p_DbdDistanceTypePtr;

   e2p_DbdAssignEnable            = (BYTE_T *)e2p_DbdAssignEnablePtr;
   e2p_DbdAssignType              = (BYTE_T *)e2p_DbdAssignTypePtr;
   e2p_DbdReassignEnable          = (BYTE_T *)e2p_DbdReassignEnablePtr;
   e2p_DbdReassignType            = (BYTE_T *)e2p_DbdReassignTypePtr;

   e2p_DbdDecDevicesCfg           = (DECDeviceRecordT *)e2p_DbdDecDevicesCfgPtr;

   e2p_DbdFloor2Building          = (BYTE_T *)e2p_DbdFloor2BuildingPtr;

   //DBD-honma 05Mar04
   e2p_DbdRrtTerminalType         = (BYTE_T *)e2p_DbdRrtTerminalTypePtr;
   e2p_DbdPwtTerminalType         = (BYTE_T *)e2p_DbdPwtTerminalTypePtr;
   e2p_DbdImmediateType           = (BYTE_T *)e2p_DbdImmediateTypePtr;
   e2p_DbdSendExpectCall          = (BYTE_T *)e2p_DbdSendExpectCallPtr;
   e2p_DbdSendCarCall             = (BYTE_T *)e2p_DbdSendCarCallPtr;
   e2p_DbdUseSeqNum               = (BYTE_T *)e2p_DbdUseSeqNumPtr;
   e2p_DbdInvChnPureRrt           = (BYTE_T *)e2p_DbdInvChnPureRrtPtr;
   e2p_DbdInvChnRrtMin            = (BYTE_T *)e2p_DbdInvChnRrtMinPtr;
   e2p_DbdInvChnRrtDif            = (BYTE_T *)e2p_DbdInvChnRrtDifPtr;
//05-may temp tests..
   e2p_DbdInvChnImmDwell          = (BYTE_T *)e2p_DbdInvChnImmDwellPtr;
   e2p_DbdInvChnExtDwell          = (BYTE_T *)e2p_DbdInvChnExtDwellPtr;
   e2p_DbdInvChnSectMax           = (BYTE_T *)e2p_DbdInvChnSectMaxPtr;
// end 05-may temp tests

   e2p_DbdMixedCfgEnable          = (BYTE_T *)e2p_DbdMixedCfgEnablePtr;
   e2p_DbdMaxDwellAccept          = (BYTE_T *)e2p_DbdMaxDwellAcceptPtr;
   e2p_DbdMaxAcceptRRT            = (BYTE_T *)e2p_DbdMaxAcceptRRTPtr;
   e2p_DbdStdWalkDist             = (BYTE_T *)e2p_DbdStdWalkDistPtr;
   e2p_DbdHcTravelDist            = (BYTE_T *)e2p_DbdHcTravelDistPtr;
   e2p_DbdStandardDwellTime       = (BYTE_T *)e2p_DbdStandardDwellTimePtr;
   e2p_DbdHandicapDwellTime       = (BYTE_T *)e2p_DbdHandicapDwellTimePtr;

   /* Initialize dbd spare 1 assignment matrix pointers */
   e2p_dbd_s_low_none     = (BYTE_T *)E2P_DBD_S_LOW_NONE;
   e2p_dbd_s_low_elderly  = (BYTE_T *)E2P_DBD_S_LOW_ELDERLY;
   e2p_dbd_s_med_none     = (BYTE_T *)E2P_DBD_S_MED_NONE;
   e2p_dbd_s_med_elderly  = (BYTE_T *)E2P_DBD_S_MED_ELDERLY;
   e2p_dbd_s_high_none    = (BYTE_T *)E2P_DBD_S_HIGH_NONE;
   e2p_dbd_s_high_elderly = (BYTE_T *)E2P_DBD_S_HIGH_ELDERLY;

   /* Initialize dbd spare rule2 assignment matrix pointers */
   e2p_dbd_c_low_none     = (BYTE_T *)E2P_DBD_C_LOW_NONE;
   e2p_dbd_c_low_elderly  = (BYTE_T *)E2P_DBD_C_LOW_ELDERLY;
   e2p_dbd_c_med_none     = (BYTE_T *)E2P_DBD_C_MED_NONE;
   e2p_dbd_c_med_elderly  = (BYTE_T *)E2P_DBD_C_MED_ELDERLY;
   e2p_dbd_c_high_none    = (BYTE_T *)E2P_DBD_C_HIGH_NONE;
   e2p_dbd_c_high_elderly = (BYTE_T *)E2P_DBD_C_HIGH_ELDERLY;

   /* Initialize dbd coincident src assignment matrix pointers */
   e2p_dbd_csrc_low_none     = (BYTE_T *)E2P_DBD_CSRC_LOW_NONE;
   e2p_dbd_csrc_low_elderly  = (BYTE_T *)E2P_DBD_CSRC_LOW_ELDERLY;
   e2p_dbd_csrc_med_none     = (BYTE_T *)E2P_DBD_CSRC_MED_NONE;
   e2p_dbd_csrc_med_elderly  = (BYTE_T *)E2P_DBD_CSRC_MED_ELDERLY;
   e2p_dbd_csrc_high_none    = (BYTE_T *)E2P_DBD_CSRC_HIGH_NONE;
   e2p_dbd_csrc_high_elderly = (BYTE_T *)E2P_DBD_CSRC_HIGH_ELDERLY;

   /* Initialize dbd coincident destination assignment matrix pointers */
   e2p_dbd_cdst_low_none     = (BYTE_T *)E2P_DBD_CDST_LOW_NONE;
   e2p_dbd_cdst_low_elderly  = (BYTE_T *)E2P_DBD_CDST_LOW_ELDERLY;
   e2p_dbd_cdst_med_none     = (BYTE_T *)E2P_DBD_CDST_MED_NONE;
   e2p_dbd_cdst_med_elderly  = (BYTE_T *)E2P_DBD_CDST_MED_ELDERLY;
   e2p_dbd_cdst_high_none    = (BYTE_T *)E2P_DBD_CDST_HIGH_NONE;
   e2p_dbd_cdst_high_elderly = (BYTE_T *)E2P_DBD_CDST_HIGH_ELDERLY;

   /* Initialize dbd coincident source and destination assignment matrix pointers */
   e2p_dbd_csd_low_none     = (BYTE_T *)E2P_DBD_CSD_LOW_NONE;
   e2p_dbd_csd_low_elderly  = (BYTE_T *)E2P_DBD_CSD_LOW_ELDERLY;
   e2p_dbd_csd_med_none     = (BYTE_T *)E2P_DBD_CSD_MED_NONE;
   e2p_dbd_csd_med_elderly  = (BYTE_T *)E2P_DBD_CSD_MED_ELDERLY;
   e2p_dbd_csd_high_none    = (BYTE_T *)E2P_DBD_CSD_HIGH_NONE;
   e2p_dbd_csd_high_elderly = (BYTE_T *)E2P_DBD_CSD_HIGH_ELDERLY;


   /* D E F I N E   F U Z Z Y   P A R A M E T E R S */
   /* --------------------------------------------- */
   e2p_fuzzy_enable = 	(BYTE_T *)E2P_FUZZY_ENABLE;
   e2p_fuzzylw_enable = (BYTE_T *)E2P_FUZZYLW_ENABLE;
   e2p_lnsold_enable = (BYTE_T *)E2P_LNSOLD_ENABLE;

   /* Fuzzy load weighing parameters */
   e2p_male_percent =	(BYTE_T *)E2P_FUZ_MALE;
   e2p_lw_sets =	(BYTE_T *)E2P_FUZ_SET;


   /* Fuzzy Mode Identification & Variable MIT parameters */
   e2p_mit_variable =	(BYTE_T *)E2P_FUZ_MIT_VARIABLE;
   e2p_mit_hysteresis =	(BYTE_T *)E2P_FUZ_MIT_HYSTER;
   e2p_mit_rlr_period =	(BYTE_T *)E2P_FUZ_MIT_ASSESS;


   /* Fuzzy Estimating passengers behind hall calls */
   e2p_crowd_cutoff =	(BYTE_T *)E2P_FUZ_MAX_CUTOFF;
   e2p_crowd_threshold =(BYTE_T *)E2P_FUZ_CROWD_THRESH;
   e2p_crowd_number =	(BYTE_T *)E2P_FUZ_CROWD_NUM;
   e2p_crowd_enable =   (BYTE_T *)E2P_FUZ_CROWD_ENABLE;


   /* fuzzy set variables */
   e2p_heavily_loaded_set = (BYTE_T *)E2P_FUZ_HEAVILY_LOAD;
   e2p_short_period_set =   (BYTE_T *)E2P_FUZ_SHORT_PERIOD;
   e2p_several_cars_set =   (BYTE_T *)E2P_FUZ_SEVERAL_CARS;
   e2p_many_int_pass_set =  (BYTE_T *)E2P_FUZ_INTRFLR_PSNGR;


   /* Fuzzy ICA variables */
   e2p_ica_enable =         (BYTE_T *)E2P_ICA_ENABLE;
   e2p_ica_reassn_reg_t =   (BYTE_T *)E2P_ICA_REASSN_REG_T;
   e2p_ica_pm_reassn =      (BYTE_T *)E2P_ICA_PM_REASSN;
   e2p_ica_reassn_cnt =     (BYTE_T *)E2P_ICA_REASSN_CNT;
   e2p_ica_pm_unex_car =    (BYTE_T *)E2P_ICA_PM_UNEX_CAR;
   e2p_ica_pm_elderly  =    (BYTE_T *)E2P_ICA_PM_ELDERLY;

   /* Fuzzy NO ICA variables */
   e2p_no_ica_reassn_reg_t =   (BYTE_T *)E2P_NO_ICA_REASSN_REG_T;
   e2p_no_ica_pm_reassn =      (BYTE_T *)E2P_NO_ICA_PM_REASSN;
   e2p_no_ica_reassn_cnt =     (BYTE_T *)E2P_NO_ICA_REASSN_CNT;
   e2p_no_ica_pm_elderly  =    (BYTE_T *)E2P_NO_ICA_PM_ELDERLY;

   e2p_IcaReassignCccTime            = (BYTE_T *)e2p_IcaReassignCccTimePtr;
   e2p_NoIcaReassignCccTime          = (BYTE_T *)e2p_NoIcaReassignCccTimePtr;
   e2p_IcaReassignPassImprove        = (BYTE_T *)e2p_IcaReassignPassImprovePtr;
   e2p_IcaReassignPassIdlImprove     = (BYTE_T *)e2p_IcaReassignPassIdlImprovePtr;
   e2p_IcaReassignPassLong           = (BYTE_T *)e2p_IcaReassignPassLongPtr;
   e2p_IcaReassignPassPwt            = (BYTE_T *)e2p_IcaReassignPassPwtPtr;
   e2p_IcaReassignPassRrt            = (BYTE_T *)e2p_IcaReassignPassRrtPtr;
   e2p_IcaReassignPassLoad           = (BYTE_T *)e2p_IcaReassignPassLoadPtr;
   e2p_PassingReassignEnable         = (BYTE_T *)e2p_PassingReassignEnablePtr;

   e2p_IcaReassignCccImprove         = (BYTE_T *)e2p_IcaReassignCccImprovePtr;
   e2p_IcaReassignCccOldRrt          = (BYTE_T *)e2p_IcaReassignCccOldRrtPtr;

   e2p_PassingReassignFloorType    = (BYTE_T *)e2p_PassingReassignFloorTypePtr;
   e2p_PassingReassignFloor1       = (BYTE_T *)e2p_PassingReassignFloor1Ptr;


   e2p_WcsReassignType               = (BYTE_T *)e2p_WcsReassignTypePtr;

   //toshimaku
   e2p_EmsUserPreferenceEnable       = (BYTE_T *)e2p_EmsUserPreferenceEnablePtr;
   e2p_EmsPreCrowdFloor              = (BYTE_T *)e2p_EmsPreCrowdFloorPtr;
   e2p_EmsUturnDir                   = (BYTE_T *)e2p_EmsUturnDirPtr;

   /* Fuzzy density variables */
   e2p_adj_ica_enable =              	(BYTE_T *)E2P_ADJ_ICA_ENABLE;
   e2p_density_debug_enable =    	(BYTE_T *)E2P_DENSITY_DEBUG_ENABLE;
   e2p_density_interval_len =    	(BYTE_T *)E2P_DENSITY_INTERVAL_LEN;
   e2p_density_intervals =      	(BYTE_T *)E2P_DENSITY_INTERVALS;
   e2p_density_30sec_weight =    	(BYTE_T *)E2P_DENSITY_30SEC_WEIGHT;
   e2p_density_avereg_weight =   	(BYTE_T *)E2P_DENSITY_AVREG_WEIGHT;
   e2p_density_hcecar_weight =   	(BYTE_T *)E2P_DENSITY_HCECAR_WEIGHT;
   e2p_density_hcpcar_weight =   	(BYTE_T *)E2P_DENSITY_HCPCAR_WEIGHT;
   e2p_density_ccecar_weight =   	(BYTE_T *)E2P_DENSITY_CCECAR_WEIGHT;
   e2p_density_ccpcar_weight =   	(BYTE_T *)E2P_DENSITY_CCPCAR_WEIGHT;
   e2p_density_long_pwt_weight = 	(BYTE_T *)E2P_DENSITY_LONG_PWT_WEIGHT;
   e2p_density_default_traffic = 	(BYTE_T *)E2P_DENSITY_DEFAULT_TRAFFIC;
   e2p_density_increasing_hc_traffic = 	(BYTE_T *)E2P_DENSITY_INCREASING_HC_TRAFFIC;
   e2p_density_increasing_cc_traffic = 	(BYTE_T *)E2P_DENSITY_INCREASING_CC_TRAFFIC;
   e2p_adj_contig_enable =              (BYTE_T *)E2P_ADJ_CONTIG_ENABLE;
   e2p_adj_demand_enable =              (BYTE_T *)E2P_ADJ_DEMAND_ENABLE;
   e2p_adj_park_enable =             	(BYTE_T *)E2P_ADJ_PARK_ENABLE;
   e2p_adj_load_enable =             	(BYTE_T *)E2P_ADJ_LOAD_ENABLE;
   e2p_adj_dnpk_enable =             	(BYTE_T *)E2P_ADJ_DNPK_ENABLE;

   /* Predirection Recommendation variables */
   e2p_predir_enable = (BYTE_T *)E2P_PREDIR_ENABLE;
   e2p_predir_min_rec_ratio = (BYTE_T *)E2P_PREDIR_MIN_REC_RATIO;


   /* Omacs enabling variable */
   e2p_omacs_enable = (BYTE_T *)E2P_OMACS_ENABLE;


   /* Debug message enabling variable */
   e2p_debug_enable = (BYTE_T *)E2P_DEBUG_ENABLE;
   e2p_nn_training_enable = (BYTE_T *)E2P_NN_TRAINING_ENABLE;


   /* Neural RRT prediction enabling variable */
   e2p_neural_enable = (BYTE_T *)E2P_NEURAL_ENABLE;

   /* Car to Call Distance Unsure variables */
   e2p_DistanceUnsureEnable = (BYTE_T *)e2p_DistanceUnsureEnablePtr;
   e2p_UnsureDistance = (BYTE_T *)e2p_UnsureDistancePtr;
   e2p_CarToCallUnsureFactor = (BYTE_T *)e2p_CarToCallUnsureFactorPtr;

   e2p_AdsCallPredictionEnable = (BYTE_T *)e2p_AdsCallPredictionEnablePtr;

   e2p_dup_enable = (BYTE_T *)E2P_DUP_ENABLE;
   e2p_mit_type = (BYTE_T *)E2P_MIT_TYPE;
   e2p_crowd_heavy_type = (BYTE_T *)E2P_CROWD_HEAVY_TYPE;

   e2p_affect_pass_enable = (BYTE_T *)E2P_AFFECT_PASS_ENABLE;
   e2p_affect_rrt_improvement = (BYTE_T *)E2P_AFFECT_RRT_IMPROVEMENT;
   e2p_affect_pwt_elderly = (BYTE_T *)E2P_AFFECT_PWT_ELDERLY;

   e2p_double_penalty   = (BYTE_T *)E2P_DOUBLE_PENALTY;

   e2p_parking_enable   = (BYTE_T *)E2P_PARKING_ENABLE;

   /* Neural Network Configurable Inputs */
   e2p_busy_floor1 = (BYTE_T *)E2P_BUSY_FLOOR1;
   e2p_busy_floor2 = (BYTE_T *)E2P_BUSY_FLOOR2;
   e2p_busy_floor3 = (BYTE_T *)E2P_BUSY_FLOOR3;
   e2p_traffic_density = (BYTE_T *)E2P_TRAFFIC_DENSITY;

   e2p_pref_ring_array =    (BYTE_T *)E2P_CROWD_PREF_RING;


   /* Additional Assignment Rules related parameters */
   e2p_Contiguous_Stops_Priority    =  (BYTE_T *)E2P_AARULE_CONTIGUOUS_STOPS_PRIORITY;
   e2p_Demand_Beyond_HC_Priority    =  (BYTE_T *)E2P_AARULE_DEMAND_BEYOND_HC_PRIORITY;
   e2p_Downpeak_Clustering_Priority =  (BYTE_T *)E2P_AARULE_DOWNPEAK_CLUSTERING_PRIORITY;
   e2p_Car_Load_Priority            =  (BYTE_T *)E2P_AARULE_CAR_LOAD_PRIORITY;
   e2p_Parking_Priority             =  (BYTE_T *)E2P_AARULE_PARKING_PRIORITY;

   e2p_Contiguous_Stops_RRT_Low     =  (BYTE_T *)E2P_AARULE_CONTIGUOUS_STOPS_RRT_LOW;
   e2p_Contiguous_Stops_RRT_Hi      =  (BYTE_T *)E2P_AARULE_CONTIGUOUS_STOPS_RRT_HI;
   e2p_Contiguous_Stops_PWT_Low     =  (BYTE_T *)E2P_AARULE_CONTIGUOUS_STOPS_PWT_LOW;
   e2p_Contiguous_Stops_PWT_Hi      =  (BYTE_T *)E2P_AARULE_CONTIGUOUS_STOPS_PWT_HI;
   e2p_Contiguous_Stops_Before      =  (BYTE_T *)E2P_AARULE_CONTIGUOUS_STOPS_BEFORE;
   e2p_Contiguous_Stops_Beyond      =  (BYTE_T *)E2P_AARULE_CONTIGUOUS_STOPS_BEYOND;

   e2p_Demand_Beyond_RRT_Low        =  (BYTE_T *)E2P_AARULE_DEMAND_BEYOND_HC_RRT_LOW;
   e2p_Demand_Beyond_RRT_Hi         =  (BYTE_T *)E2P_AARULE_DEMAND_BEYOND_HC_RRT_HI;
   e2p_Demand_Beyond_PWT_Low        =  (BYTE_T *)E2P_AARULE_DEMAND_BEYOND_HC_PWT_LOW;
   e2p_Demand_Beyond_PWT_Hi         =  (BYTE_T *)E2P_AARULE_DEMAND_BEYOND_HC_PWT_HI;

   e2p_Downpeak_Clustering_RRT_Low  =  (BYTE_T *)E2P_AARULE_DOWNPEAK_CLUSTERING_RRT_LOW;
   e2p_Downpeak_Clustering_RRT_Hi   =  (BYTE_T *)E2P_AARULE_DOWNPEAK_CLUSTERING_RRT_HI;
   e2p_Downpeak_Clustering_PWT_Low  =  (BYTE_T *)E2P_AARULE_DOWNPEAK_CLUSTERING_PWT_LOW;
   e2p_Downpeak_Clustering_PWT_Hi   =  (BYTE_T *)E2P_AARULE_DOWNPEAK_CLUSTERING_PWT_HI;
   e2p_Dpk_Cluster_Top_1_of_2       =  (BYTE_T *)E2P_AARULE_DOWNPEAK_CLUSTER_TOP_1_OF_2;

   e2p_Car_Load_RRT_Low             =  (BYTE_T *)E2P_AARULE_CAR_LOAD_RRT_LOW;
   e2p_Car_Load_RRT_Hi              =  (BYTE_T *)E2P_AARULE_CAR_LOAD_RRT_HI;
   e2p_Car_Load_PWT_Low             =  (BYTE_T *)E2P_AARULE_CAR_LOAD_PWT_LOW;
   e2p_Car_Load_PWT_Hi              =  (BYTE_T *)E2P_AARULE_CAR_LOAD_PWT_HI;

   e2p_Parking_RRT_Low              =  (BYTE_T *)E2P_AARULE_PARKING_RRT_LOW;
   e2p_Parking_RRT_Hi               =  (BYTE_T *)E2P_AARULE_PARKING_RRT_HI;
   e2p_Parking_PWT_Low              =  (BYTE_T *)E2P_AARULE_PARKING_PWT_LOW;
   e2p_Parking_PWT_Hi               =  (BYTE_T *)E2P_AARULE_PARKING_PWT_HI;

   /* Initialize miscellaneous variables */
   e2p_ica_nlb_enable = (BYTE_T *)E2P_ICA_NLB_ENABLE;

   /* Initialize RRT Medium fuzzy set coordinate pointers */
   e2p_light_med_x0 =  (BYTE_T *)E2P_LIGHT_MED_X0;

   /* Initialize RRT Short fuzzy set coordinate pointers */
   e2p_short_x0 =  (BYTE_T *)E2P_SHORT_X0;

   /* Initialize RRT Improv fuzzy set coordinate pointers */
   e2p_improv_x0 =  (BYTE_T *)E2P_IMPROV_X0;

   /* Initialize PWT Elderly fuzzy set coordinate pointers */
   e2p_light_elderly_x0 =  (BYTE_T *)E2P_LIGHT_ELDERLY_X0;

   /* Initialize PWT Long fuzzy set coordinate pointers */
   e2p_light_long_x0 =  (BYTE_T *)E2P_LIGHT_LONG_X0;

  /* Initialize RRT Medium fuzzy set coordinate pointers */
   e2p_heavy_med_x0 =  (BYTE_T *)E2P_HEAVY_MED_X0;

   /* Initialize PWT Elderly fuzzy set coordinate pointers */
   e2p_heavy_elderly_x0 =  (BYTE_T *)E2P_HEAVY_ELDERLY_X0;

   /* Initialize PWT Long fuzzy set coordinate pointers */
   e2p_heavy_long_x0 =  (BYTE_T *)E2P_HEAVY_LONG_X0;

   /* Initialize DENSITY >30 sec fuzzy set coordinate pointers */
   e2p_30sec_x0 =  (BYTE_T *)E2P_30SEC_X0;

   /* Initialize DENSITY cc entered per car fuzzy set coordinate pointers */
   e2p_cc_entered_percar_x0 =  (BYTE_T *)E2P_CC_E_PERCAR_X0;

   /* Initialize DENSITY cc present per car fuzzy set coordinate pointers */
   e2p_cc_present_percar_x0 =  (BYTE_T *)E2P_CC_P_PERCAR_X0;

   /* Initialize DENSITY hc entered per car fuzzy set coordinate pointers */
   e2p_hc_entered_percar_x0 =  (BYTE_T *)E2P_HC_E_PERCAR_X0;

   /* Initialize DENSITY hc present per car fuzzy set coordinate pointers */
   e2p_hc_present_percar_x0 =  (BYTE_T *)E2P_HC_P_PERCAR_X0;

   /* Initialize DENSITY avg reg T fuzzy set coordinate pointers */
   e2p_avgreg_x0 =  (BYTE_T *)E2P_AVGREG_T_X0;

   /* Initialize Longest PWT fuzzy set coordinate pointers */
   e2p_longest_pwt_x0 =  (BYTE_T *)E2P_LONG_PWT_X0;

   /* Initialize standard assignment matrix pointers */
   e2p_s_low_none     = (BYTE_T *)E2P_S_LOW_NONE;
   e2p_s_low_elderly  = (BYTE_T *)E2P_S_LOW_ELDERLY;
   e2p_s_med_none     = (BYTE_T *)E2P_S_MED_NONE;
   e2p_s_med_elderly  = (BYTE_T *)E2P_S_MED_ELDERLY;
   e2p_s_high_none    = (BYTE_T *)E2P_S_HIGH_NONE;
   e2p_s_high_elderly = (BYTE_T *)E2P_S_HIGH_ELDERLY;

   /* Initialize coincident call assignment matrix pointers */
   e2p_c_low_none     = (BYTE_T *)E2P_C_LOW_NONE;
   e2p_c_low_elderly  = (BYTE_T *)E2P_C_LOW_ELDERLY;
   e2p_c_med_none     = (BYTE_T *)E2P_C_MED_NONE;
   e2p_c_med_elderly  = (BYTE_T *)E2P_C_MED_ELDERLY;
   e2p_c_high_none    = (BYTE_T *)E2P_C_HIGH_NONE;
   e2p_c_high_elderly = (BYTE_T *)E2P_C_HIGH_ELDERLY;


   //dojima
   e2p_trafdatSecondLobbyPos      = (BYTE_T *)e2p_trafdatSecondLobbyPosPtr;
// e2p_trafdatBuildingPopulation  = (INT_T *)e2p_trafdatBuildingPopulationPtr;
   e2p_trafdatBuildingPopulation  = (BYTE_T *)e2p_trafdatBuildingPopulationPtr;
   e2p_trafdatPassengerWeight     = (BYTE_T *)e2p_trafdatPassengerWeightPtr;

   e2p_VmitType			= (BYTE_T *)e2p_VmitTypePtr;
   e2p_VmitTrigger		= (BYTE_T *)e2p_VmitTriggerPtr;
   e2p_CdbsEnable		= (BYTE_T *)e2p_CdbsEnablePtr;
   e2p_CdbsLobbyBoarding	= (BYTE_T *)e2p_CdbsLobbyBoardingPtr;
   e2p_CdbsLobbyDeboarding	= (BYTE_T *)e2p_CdbsLobbyDeboardingPtr;
   e2p_NorSecondLobbyParkEnable = (BYTE_T *)e2p_NorSecondLobbyParkEnablePtr;

   e2p_VmitEnable		= (BYTE_T *)e2p_VmitEnablePtr;
   e2p_EnergySaveEnable		= (BYTE_T *)e2p_EnergySaveEnablePtr;
   e2p_VmitSelectCarType        = (BYTE_T *)e2p_VmitSelectCarTypePtr;
   e2p_VmitPriorityCar1         = (BYTE_T *)e2p_VmitPriorityCar1Ptr;

   e2p_EsoVelocityEnable        = (BYTE_T *)e2p_EsoVelocityEnablePtr;
   e2p_EsoReduceCarEnable       = (BYTE_T *)e2p_EsoReduceCarEnablePtr;

   e2p_EsoSelectCarType         = (BYTE_T *)e2p_EsoSelectCarTypePtr;
// e2p_EsoBuildingPopulation    = (INT_T *)e2p_EsoBuildingPopulationPtr;
   e2p_EsoBuildingPopulation    = (BYTE_T *)e2p_EsoBuildingPopulationPtr;
   e2p_Eso5minConsecutiveTimes  = (BYTE_T *)e2p_Eso5minConsecutiveTimesPtr;

   e2p_EsoPriorityCar1          = (BYTE_T *)e2p_EsoPriorityCar1Ptr;

   e2p_EsoMaxCars               = (BYTE_T *)e2p_EsoMaxCarsPtr;

   e2p_MultiLobbyHallPenalty    = (BYTE_T *)e2p_MultiLobbyHallPenaltyPtr;

   e2p_MitStartHour             = (BYTE_T *)e2p_MitStartHourPtr;
   e2p_MitStartMin              = (BYTE_T *)e2p_MitStartMinPtr;
   e2p_MitStopHour             = (BYTE_T *)e2p_MitStopHourPtr;
   e2p_MitStopMin              = (BYTE_T *)e2p_MitStopMinPtr;

   e2p_ParkSpecialFloor        = (BYTE_T *)e2p_ParkSpecialFloorPtr;

   e2p_WcsCar                  = (BYTE_T *)e2p_WcsCarPtr;
   e2p_WcsPenalty              = (BYTE_T *)e2p_WcsPenaltyPtr;

   e2p_WcsElderlyType          = (BYTE_T *)e2p_WcsElderlyTypePtr;

   e2p_EzUnsureType            = (BYTE_T *)e2p_EzUnsureTypePtr;
   e2p_LnsReassignCntType      = (BYTE_T *)e2p_LnsReassignCntTypePtr;
   e2p_ParkNorLobbyZoneTop     = (BYTE_T *)e2p_ParkNorLobbyZoneTopPtr;

  //Shizuoka
   e2p_MitSpTrigger            = (BYTE_T *)e2p_MitSpTriggerPtr;
   //   e2p_MitSp2CarsEnable        = (BYTE_T *)e2p_MitSp2CarsEnablePtr;
   e2p_MitSp2EnableCar1        = (BYTE_T *)e2p_MitSp2EnableCar1Ptr;
   e2p_MitGroupCar1            = (BYTE_T *)e2p_MitGroupCar1Ptr;

   //dojima
   e2p_AssPeriodTskDisable     = (BYTE_T *)e2p_AssPeriodTskDisablePtr;
   //australia
   e2p_IdlCheckEnable          = (BYTE_T *)e2p_IdlCheckEnablePtr;


   e2p_ParkFixedZoneType       = (BYTE_T *)e2p_ParkFixedZoneTypePtr;
   e2p_ParkFixedZoneTrigger    = (BYTE_T *)e2p_ParkFixedZoneTriggerPtr;
   e2p_ParkMzpType             = (BYTE_T *)e2p_ParkMzpTypePtr;
   e2p_FixedZone1Pos           = (BYTE_T *)e2p_FixedZone1PosPtr;
   e2p_FixedZone1Bot           = (BYTE_T *)e2p_FixedZone1BotPtr;
   e2p_FixedZone1Top           = (BYTE_T *)e2p_FixedZone1TopPtr;

   e2p_JobToshimakuEnable      = (BYTE_T *)e2p_JobToshimakuEnablePtr;
   e2p_JobShizuokaEnable       = (BYTE_T *)e2p_JobShizuokaEnablePtr;
   e2p_JobTohokuEnable         = (BYTE_T *)e2p_JobTohokuEnablePtr;

   e2p_JobShindongahEnable     = (BYTE_T *)e2p_JobShindongahEnablePtr;
   e2p_JobCyuouMitsuiEnable    = (BYTE_T *)e2p_JobCyuouMitsuiEnablePtr;

   e2p_JobOSangyoEnable        = (BYTE_T *)e2p_JobOSangyoEnablePtr;
   e2p_CrowdInterval           = (BYTE_T *)e2p_CrowdIntervalPtr;

   e2p_DhbReassignType         = (BYTE_T *)e2p_DhbReassignTypePtr;

   e2p_ParkCarsInMainLobby     = (BYTE_T *)e2p_ParkCarsInMainLobbyPtr;
   e2p_ParkLimitationHallCallType = (BYTE_T *)e2p_ParkLimitationHallCallTypePtr;
   e2p_ParkLimitationRrt       = (BYTE_T *)e2p_ParkLimitationRrtPtr;
   e2p_ParkSpecialFloorEnable    = (BYTE_T *)e2p_ParkSpecialFloorEnablePtr;


   //Osaka OBP
   e2p_DpcEnable                 = (BYTE_T *)e2p_DpcEnablePtr;
   e2p_DpcTriggerType            = (BYTE_T *)e2p_DpcTriggerTypePtr;
   e2p_DpcType                   = (BYTE_T *)e2p_DpcTypePtr;
   e2p_DpcSectorNumber           = (BYTE_T *)e2p_DpcSectorNumberPtr;
   e2p_DpcLobbyPos               = (BYTE_T *)e2p_DpcLobbyPosPtr;

   e2p_DpcSector1Penalty         = (BYTE_T *)e2p_DpcSector1PenaltyPtr;

   e2p_DpcSector1Bot             = (BYTE_T *)e2p_DpcSector1BotPtr;

   e2p_DpcSector1Top             = (BYTE_T *)e2p_DpcSector1TopPtr;

   e2p_JobObpEnable              = (BYTE_T *)e2p_JobObpEnablePtr;

   e2p_Dpc2Enable                 = (BYTE_T *)e2p_Dpc2EnablePtr;
   e2p_Dpc2TriggerType            = (BYTE_T *)e2p_Dpc2TriggerTypePtr;
   e2p_Dpc2Type                   = (BYTE_T *)e2p_Dpc2TypePtr;
   e2p_Dpc2SectorNumber           = (BYTE_T *)e2p_Dpc2SectorNumberPtr;
   e2p_Dpc2LobbyPos               = (BYTE_T *)e2p_Dpc2LobbyPosPtr;

   e2p_Dpc2Sector1Penalty         = (BYTE_T *)e2p_Dpc2Sector1PenaltyPtr;

   e2p_Dpc2Sector1Bot             = (BYTE_T *)e2p_Dpc2Sector1BotPtr;

   e2p_Dpc2Sector1Top             = (BYTE_T *)e2p_Dpc2Sector1TopPtr;

   e2p_CrowdFloorType             = (BYTE_T *)e2p_CrowdFloorTypePtr;
   e2p_CrowdFloor1                = (BYTE_T *)e2p_CrowdFloor1Ptr;
   e2p_CrowdFloor2                = (BYTE_T *)e2p_CrowdFloor2Ptr;
   e2p_CrowdFloor3                = (BYTE_T *)e2p_CrowdFloor3Ptr;

   e2p_SpsFloor1                  = (BYTE_T *)e2p_SpsFloor1Ptr;
   e2p_SpsFloor2                  = (BYTE_T *)e2p_SpsFloor2Ptr;
   e2p_SpsFloor3                  = (BYTE_T *)e2p_SpsFloor3Ptr;

   e2p_JobDentsuEnable            = (BYTE_T *)e2p_JobDentsuEnablePtr;

   e2p_DupTopPos                  = (BYTE_T *)e2p_DupTopPosPtr;


	e2p_default_value();
}

/**************************************************************************
;  Name: e2p_default_value
;
;  Description: This task include e2p default value
;
;  Parameters: None
;  Returns: None
;*************************************************************************/

void  e2p_default_value(void)
{
	e2p_fixed_value();
	*e2p_DbdEnable=1;
	*e2p_DbdAssignEnable=1;
	*e2p_DbdPwtTerminalType=1;
	*e2p_DistanceUnsureEnable=0;
	*e2p_UnsureDistance=0;
	*e2p_DbdAssignType = 0x01; //AssignTypeImmediate
}//void  e2p_default_value(void)

 /**************************************************************************
;  Name: e2p_fixed_value
;
;  Description: This task includes e2p fixed value

;  Parameters: None
;  Returns: None
;*************************************************************************/

void  e2p_fixed_value(void)
  {

   /* Fuzzy ICA variables */
   *e2p_ica_enable =         1;
   *e2p_ica_reassn_reg_t =   20;
   *e2p_ica_pm_reassn =      75;
   *e2p_ica_reassn_cnt =     1;
   *e2p_ica_pm_unex_car =    100;
   *e2p_ica_pm_elderly  =   40;

   *e2p_fuzzy_enable   = 1;

   /* Initialize RRT Medium fuzzy set coordinate pointers */
   *(e2p_light_med_x0+0) = 0;
   *(e2p_light_med_x0+1) = 0;
   *(e2p_light_med_x0+2) = 30;
   *(e2p_light_med_x0+3) = 100;
   *(e2p_light_med_x0+4) = 120;
   *(e2p_light_med_x0+5) = 0;

   /* Initialize RRT Short fuzzy set coordinate pointers */
   *(e2p_short_x0+0) = 0;
   *(e2p_short_x0+1) = 100;
   *(e2p_short_x0+2) = 5;
   *(e2p_short_x0+3) = 100;
   *(e2p_short_x0+4) = 25;
   *(e2p_short_x0+5) = 0;
   *(e2p_short_x0+6) = 120;
   *(e2p_short_x0+7) = 0;

   /* Initialize RRT Improv fuzzy set coordinate pointers */
   *(e2p_improv_x0+0) = 5;
   *(e2p_improv_x0+1) = 0;
   *(e2p_improv_x0+2) = 25;
   *(e2p_improv_x0+3) = 100;
   *(e2p_improv_x0+4) = 120;
   *(e2p_improv_x0+5) = 100;

   /* Initialize PWT Elderly fuzzy set coordinate pointers */
   *(e2p_light_elderly_x0+0) = 0;
   *(e2p_light_elderly_x0+1) = 0;
   *(e2p_light_elderly_x0+2) = 40;
   *(e2p_light_elderly_x0+3) = 0;
   *(e2p_light_elderly_x0+4) = 120;
   *(e2p_light_elderly_x0+5) = 100;
   *(e2p_light_elderly_x0+6) = 150;
   *(e2p_light_elderly_x0+7) = 100;

   /* Initialize PWT Long fuzzy set coordinate pointers */
   *(e2p_light_long_x0+0) = 0;
   *(e2p_light_long_x0+1) = 0;
   *(e2p_light_long_x0+2) = 40;
   *(e2p_light_long_x0+3) = 0;
   *(e2p_light_long_x0+4) = 120;
   *(e2p_light_long_x0+5) = 100;
   *(e2p_light_long_x0+6) = 150;
   *(e2p_light_long_x0+7) = 100;

   /* Initialize standard assignment matrix pointers */
   *e2p_s_low_none     = 80;
   *e2p_s_low_elderly  = 35;
   *e2p_s_med_none     = 60;
   *e2p_s_med_elderly  = 25;
   *e2p_s_high_none    = 40;
   *e2p_s_high_elderly = 15;

   *e2p_c_low_none     = 90;
   *e2p_c_low_elderly  = 35;
   *e2p_c_med_none     = 70;
   *e2p_c_med_elderly  = 25;
   *e2p_c_high_none    = 30;
   *e2p_c_high_elderly = 15;

   /* Initialize dbd standard assignment matrix pointers */
    E2P_DbdAssignMatrixDefault();

  }//void  e2p_fixed_value(void)

  void  E2P_DbdAssignMatrixDefault(void)
{
   /* Initialize dbd spare rule 1 assignment matrix pointers */
   *e2p_dbd_s_low_none     = 80;
   *e2p_dbd_s_low_elderly  = 35;
   *e2p_dbd_s_med_none     = 60;
   *e2p_dbd_s_med_elderly  = 25;
   *e2p_dbd_s_high_none    = 40;
   *e2p_dbd_s_high_elderly = 15;

   /* Initialize dbd spare rule 2 assignment matrix pointers */
   *e2p_dbd_c_low_none     = 90;
   *e2p_dbd_c_low_elderly  = 90;
   *e2p_dbd_c_med_none     = 70;
   *e2p_dbd_c_med_elderly  = 70;
   *e2p_dbd_c_high_none    = 30;
   *e2p_dbd_c_high_elderly = 30;

   /* Initialize dbd coincident src assignment matrix pointers */
   *e2p_dbd_csrc_low_none     = 100;
   *e2p_dbd_csrc_low_elderly  = 35;
   *e2p_dbd_csrc_med_none     = 85;
   *e2p_dbd_csrc_med_elderly  = 25;
   *e2p_dbd_csrc_high_none    = 40;
   *e2p_dbd_csrc_high_elderly = 15;

   /* Initialize dbd coincident destination assignment matrix pointers */
   *e2p_dbd_cdst_low_none     = 90;
   *e2p_dbd_cdst_low_elderly  = 35;
   *e2p_dbd_cdst_med_none     = 70;
   *e2p_dbd_cdst_med_elderly  = 25;
   *e2p_dbd_cdst_high_none    = 30;
   *e2p_dbd_cdst_high_elderly = 15;

   /* Initialize dbd coincident source and destination assignment matrix pointers */
   *e2p_dbd_csd_low_none     = 100;
   *e2p_dbd_csd_low_elderly  = 100;
   *e2p_dbd_csd_med_none     = 85;
   *e2p_dbd_csd_med_elderly  = 85;
   *e2p_dbd_csd_high_none    = 40;
   *e2p_dbd_csd_high_elderly = 40;


}

  

/**************************************************************************
;  Name: E2P_Generate_Fuzzy_Set
;
;  Description: Given a coordinate set, set pointer, and set type, generate
;		appropriate fuzzy set.
;
;  NOTE:  The local variables in this module MUST be signed integers
;  for the calculations to work properly.
;
;  Parameters:
;  coordinate_list (input)	- list of coordinates (8).
;  set_type (input)		- inidicates number of elements in set.
;  fuzzy_set (input)		- pointer to fuzzy set.
;*************************************************************************/
void E2P_Generate_Fuzzy_Set(BYTE_T *coordinate_list, BYTE_T set_type,
			    S_INT_T *fuzzy_set)

{
	S_INT_T y_intercept;
	S_INT_T slope;
	S_INT_T set_upper_limit;
	S_INT_T x_upper_limit;
	S_INT_T i;
	S_INT_T x;
	S_INT_T y;
	S_INT_T next_x;
	S_INT_T next_y;
	
	/*------------------------------------------------*/
	/* Perform initialization for this fuzzy set gen. */
	/*   1. Determine type of fuzzy set.              */
	/*   2. Zero out the fuzzy set.                   */
	/*------------------------------------------------*/
	
	/* S T E P   #1 */
	/*--------------*/
	if (set_type == RRT_TYPE)
		set_upper_limit = RRT_X_UPPER;
	else if (set_type == PWT_TYPE)
		set_upper_limit = PWT_X_UPPER;
	else
		set_upper_limit = DENSITY_X_UPPER;
	
	
	/* S T E P   #2 */
	/*--------------*/
	for (i=0; i<set_upper_limit; i++)
	{
		*(fuzzy_set+i) = 0;
	}
	
	next_x = 0;
	while (next_x < set_upper_limit)
	{
		/*-----------------------------------------------------*/
		/* For each coordinate pair, perform the following:    */
		/*	 1. Initialize x and y from coordinate set.  */
		/*	 2. Place intial y value in fuzzy set.	     */
		/*	 3. Determine slope and y-intercept 	     */
		/*-----------------------------------------------------*/
		
		/* S T E P   #1 */
		/*--------------*/
		x = *coordinate_list;
		coordinate_list++;
		y = *coordinate_list * 10;
		coordinate_list++;
		next_x = (*coordinate_list);
		next_y = (*(coordinate_list+1)) * 10;
		
		/* S T E P   #2 */
		/*--------------*/
		*(fuzzy_set+x) = min(100,(y+5)/10);
		
		/* S T E P   #3 */
		/*--------------*/
		if (next_x == x)
			next_x++;
		slope = (next_y - y)/(next_x - x);
		y_intercept = y - slope*x;
		
		while ((x < next_x) && (x < set_upper_limit))
		{
			/*--------------------------------------------------*/
			/* For this line segment, do the following:         */
			/*    1. Determine y value.       	    	     */
			/*    2. Store value to fuzzy set.	    	     */
			/*--------------------------------------------------*/
			
			/* S T E P  #1 */
			/*-------------*/
			x++;
			y = (slope*x) + y_intercept;
			
			/* S T E P  #2 */
			/*-------------*/
			*(fuzzy_set+x) = min(100,(y+5)/10);
			
		}
	}
}
//*********************************************************************************************


//void  E2P_DbdAssignMatrixDefault(void)

//*********************************************************************************************
//  Name: E2P_Generate_RRT_Low_Fuzzy_Set
// 
//  Description: Generate RRT Low Fuzzy set. This is dependent on the RRT 
// 		Meduium fuzzy set. RRT_Low = 100 - RRT_Medium.
//
//  Parameters:
//  None
//*********************************************************************************************
void E2P_Generate_RRT_Low_Fuzzy_Set(void) 
{
	BYTE_T i;
	S_INT_T *fuzzy_set;
   
	//--------------------------------------------------//
	// Steps needed to generate the RRT_Low fuzzy set:  //
	//    1. Initialize fuzzy set to zero.	            //
	//    2. Determine set values based on RRT Medium   //
	//--------------------------------------------------//
   
	// S T E P   #1	---------------------------------------------------------------------------
	fuzzy_set = (S_INT_T *)&RRT_Light_Low[0];

	for (i=0; i<=RRT_X_UPPER; i++) {
		*(fuzzy_set+i) = 0;
	}
   
	// S T E P   #2	---------------------------------------------------------------------------
	i = 0;
	
	while ( (RRT_Light_Medium[i+1] >= RRT_Light_Medium[i]) && (i <= RRT_X_UPPER) ) {
		
		RRT_Light_Low[i] = 100 - RRT_Light_Medium[i];
		i++;
	}
   
	//---------------------------------------------//
	//     Do the same for the heavy RRT sets      //
	// --------------------------------------------//

	// S T E P   #1	---------------------------------------------------------------------------
	fuzzy_set = (S_INT_T *)&RRT_Heavy_Low[0];
	
	for (i=0; i<=RRT_X_UPPER; i++) {
		*(fuzzy_set+i) = 0;
	}
   
	// S T E P   #2	---------------------------------------------------------------------------
	i = 0;
	
	while ( (RRT_Heavy_Medium[i+1] >= RRT_Heavy_Medium[i]) && (i <= RRT_X_UPPER) ) {

		RRT_Heavy_Low[i] = 100 - RRT_Heavy_Medium[i];
		i++;
	}
}

//*********************************************************************************************
//  Name: E2P_Generate_RRT_High_Fuzzy_Set
// 
//  Description: Generate RRT High Fuzzy set. This is dependent on the RRT 
// 		Meduium fuzzy set. RRT_High = 100 - RRT_Medium.
//
//  Parameters:
//  None
//*********************************************************************************************
void E2P_Generate_RRT_High_Fuzzy_Set(void) 
{
	BYTE_T i;
	S_INT_T	*fuzzy_set;
   
	//---------------------------------------------------//
	// Steps needed to generate the RRT_High fuzzy set:  //
	//	   1. Initialize fuzzy set to zero.	             //
	//    2. Determine set values based on RRT Medium    //
	//---------------------------------------------------//
   
	// S T E P   #1	---------------------------------------------------------------------------
	fuzzy_set=(S_INT_T *)&RRT_Light_High[0];

	for(i=0;i<=RRT_X_UPPER;i++) {

		*(fuzzy_set+i) = 0;
	}

	// S T E P   #2	---------------------------------------------------------------------------
	i = 0;

	while( (RRT_Light_Medium[i+1] >= RRT_Light_Medium[i]) && (i <= RRT_X_UPPER) ) {
		i++;
	}
	
	while(i <= RRT_X_UPPER) {
		
		RRT_Light_High[i] = 100 - RRT_Light_Medium[i];
		i++;
	}
   
	//-------------------------------------------//
	//     Do the same for the heavy RRT sets    //
	//-------------------------------------------//

	// S T E P   #1	---------------------------------------------------------------------------
	fuzzy_set = (S_INT_T *)&RRT_Heavy_High[0];

	for (i=0; i<=RRT_X_UPPER; i++) {

		*(fuzzy_set+i) = 0;
	}
   
	// S T E P   #2	---------------------------------------------------------------------------
	i = 0;

	while ( (RRT_Heavy_Medium[i+1] >= RRT_Heavy_Medium[i]) && (i <= RRT_X_UPPER) ) {
		i++;
	}

	while (i <= RRT_X_UPPER) {
		
		RRT_Heavy_High[i] = 100 - RRT_Heavy_Medium[i];
		i++;
	}
}
//*********************************************************************************************

//*********************************************************************************************
//  Name: E2P_Generate_PWT_None_Fuzzy_Set
// 
//  Description: Generate PWT None Fuzzy set. This is dependent on the PWT 
// 		elderly fuzzy set.
//
//  Parameters:
//  None
//*********************************************************************************************
void E2P_Generate_PWT_None_Fuzzy_Set(void) 
{
	BYTE_T i;
	S_INT_T *fuzzy_set;
   
	//----------------------------------------------------//
	// Steps needed to generate the PWT_None fuzzy set:   //
	//	   1. Initialize fuzzy set to zero.	              //
	//    2. Determine set values based on PWT Elderly    //
	//----------------------------------------------------//
   
	// S T E P   #1	---------------------------------------------------------------------------
	fuzzy_set = (S_INT_T *)&PWT_Light_None[0];
	
	for (i=0; i<PWT_X_UPPER; i++) *(fuzzy_set+i) = 0;
   
	// S T E P   #2 ---------------------------------------------------------------------------
	i = 0;
	while ( (PWT_Light_Elderly[i+1] >= PWT_Light_Elderly[i]) && (i <= PWT_X_UPPER) ) {
		
		PWT_Light_None[i] = 100 - PWT_Light_Elderly[i];
		i++;
	}
     
	//------------------------------------------//
	//    Do the same for the heavy PWT sets    //
	//------------------------------------------//

	// S T E P   #1	---------------------------------------------------------------------------
	fuzzy_set = (S_INT_T *)&PWT_Heavy_None[0];
	
	for (i=0; i<PWT_X_UPPER; i++) *(fuzzy_set+i) = 0;
	   
	// S T E P   #2	---------------------------------------------------------------------------
	i = 0;
	while ( (PWT_Heavy_Elderly[i+1] >= PWT_Heavy_Elderly[i]) && (i <= PWT_X_UPPER) ) {
		
		PWT_Heavy_None[i] = 100 - PWT_Heavy_Elderly[i];
		i++;
	}
}
//*********************************************************************************************

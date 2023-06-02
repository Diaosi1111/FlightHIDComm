#pragma once
#ifndef _SIM_CLIENT_
#define _SIM_CLIENT_
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#include "SimConnect.h"


struct A32NXFCUPanel
{
    double ap_master;
    double ap1_master;//FCU AP1
    double ap2_master;//FCU AP2
    double ap_throttle;//FCU A/THR
    double loc_mode_active;//FCU LOC
    double exped_mode_active;//FCU EXPED
    double appr_mode_active;//FCU APPR

    double spd_dashes;//FCU SPD DASHES
    double spd_dot;//FCU SPD dot
    double hdg_dashes;//---
    double hdg_dot;//点
    double alt_dot;//ALT显示点
    double vs_dashes;//VS显示----
    double trk_fpa_mode;// HDG-TRK / V/S-FPA
    double spd_mach;



    //double ap_airspeed_hold;
    //bool ap_altitiude_arm;
    //bool ap_altitiude_lock;
    //double ap_altitiude_var;
    //AUTOPILOT ALTITUDE MANUALLY TUNABLE


    double ap_airspeed_hold_var;
    double fpa_selected;
    INT32 heading_selected;
    INT32 alt_selected;
    INT32 alt_inc;//100|1000
    INT32 vs_selected;
};

typedef struct AIRCRAFT_VAR {
    double metric_mode_active;
}aircraft_var_t;

//typedef struct PANEL_STRUCT_HID {
//    UINT16 data1;
//    UINT16 hdg_selected;
//    float spd_selected;
//    INT8 fpa_selected;
//    INT8 vs_selected;
//    UINT16 alt_selected;
//}panel_hid_t;
typedef struct PANEL_STRUCT_HID {
    struct {
        UINT16 ap_master : 1;
        UINT16 ap1_active : 1;
        UINT16 ap2_active : 1;
        UINT16 ap_throttle_active : 1;
        UINT16 loc_mode_active : 1;
        UINT16 exped_mode_active : 1;
        UINT16 appr_mode_active : 1;
        UINT16 spd_mach_mode : 1;
        UINT16 trk_fpa_mode : 1;

        UINT16 spd_dashes : 1;
        UINT16 spd_dot : 1;
        UINT16 hdg_dashes : 1;
        UINT16 hdg_dot : 1;
        UINT16 alt_dot : 1;
        UINT16 alt_increment : 1; // 0|1=>100|1000
        UINT16 vs_dashes : 1;
    };
    UINT16 hdg_selected;
    float spd_selected;
    INT8 fpa_selected;
    INT8 vs_selected;
    UINT16 alt_selected;
} fcu_panel_hid_t;

extern fcu_panel_hid_t panel_state;

typedef enum  {
    GROUP_FCU_PANEL,
    GROUP_AIRCRAFT_VAR,
}GROUP_ID;

typedef enum {
    EVENT_SIM_START,
    EVENT_A32X_FCU_AP1_PUSH,
    EVENT_A32X_FCU_AP2_PUSH,
    EVENT_A32X_FCU_ATHR_PUSH,
    EVENT_A32X_FCU_LOC_PUSH,
    EVENT_A32X_FCU_EXPED_PUSH,
    EVENT_A32X_FCU_APPR_PUSH,
    EVENT_A32X_FCU_SPD_MACH_PUSH,
    EVENT_A32X_FCU_TRK_FPA_PUSH,
    EVENT_A32X_FCU_METRIC_ALT_PUSH,

    EVENT_A32X_FCU_SPD_PUSH,
    EVENT_A32X_FCU_SPD_PULL,
    EVENT_A32X_FCU_SPD_INC,
    EVENT_A32X_FCU_SPD_DEC,

    EVENT_A32X_FCU_HDG_PUSH,
    EVENT_A32X_FCU_HDG_PULL,
    EVENT_A32X_FCU_HDG_INC,
    EVENT_A32X_FCU_HDG_DEC,

    EVENT_A32X_FCU_ALT_PUSH,
    EVENT_A32X_FCU_ALT_PULL,
    EVENT_A32X_FCU_ALT_INC,
    EVENT_A32X_FCU_ALT_DEC,

    EVENT_A32X_FCU_VS_PUSH,
    EVENT_A32X_FCU_VS_PULL,
    EVENT_A32X_FCU_VS_INC,
    EVENT_A32X_FCU_VS_DEC,
}EVENT_ID;
/*数据格式定义*/
typedef enum  {
    DEFINITION_1,
    DEFINITION_FCU_PANEL,
    DEFINITION_AIRCRAFT_VAR
}DATA_DEFINE_ID;

typedef enum  {
    REQUEST_1,
    REQUEST_FCU_PANEL,
    REQUEST_AIRCRAFT_VAR,
}DATA_REQUEST_ID;

void testDataRequest();

int init_SimConnect();

void deinit_SimConnect();

void aircraft_event_send(EVENT_ID event_id);

void simconnect_call();

#endif // !_SIM_CLIENT_

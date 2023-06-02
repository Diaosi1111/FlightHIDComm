#include "sim_client.hpp"
#include "main.h"
#include "log.h"

int     quit = 0;
HANDLE  hSimConnect = NULL;
HRESULT hr;

fcu_panel_hid_t panel_state;
aircraft_var_t aircraft_var;

void CALLBACK MyDispatchProcRD(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext)
{
    switch (pData->dwID)
    {
    case SIMCONNECT_RECV_ID_EVENT://接收事件
    {
        SIMCONNECT_RECV_EVENT* evt = (SIMCONNECT_RECV_EVENT*)pData;

        switch (evt->uEventID)
        {
        case EVENT_SIM_START:

            // Now the sim is running, request information on the user aircraft
            //hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_FCU_PANEL, DEFINITION_FCU_PANEL, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND);

            break;

        default:
            break;
        }
        break;
    }

    case SIMCONNECT_RECV_ID_SIMOBJECT_DATA://接收数据
    {
        SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;

        switch (pObjData->dwRequestID)
        {
        case REQUEST_FCU_PANEL:
        {
            DWORD ObjectID = pObjData->dwObjectID;
            A32NXFCUPanel* pS = (A32NXFCUPanel*)&pObjData->dwData;
            {
                //printf("\nObjectID=%d   Value:%lf\n", ObjectID, pS->ap_airspeed_hold_var);
                //printf("AP Master:%d AP1:%d AP2:%d A/THR:%d \n", pS->ap_master != 0, pS->ap1_master != 0, pS->ap2_master != 0, (int)pS->ap_throttle);
                //printf("LOC Mode:%d EXPED Mode:%d APPR Mode:%d\n", pS->loc_mode_active != 0, pS->exped_mode_active != 0, pS->appr_mode_active != 0);
                //printf("SPD Dashes:%d SPD Dot:%d HDG Dashes:%d HDG Dot:%d ALT:%d VS:%d\n", pS->spd_dashes != 0, pS->spd_dot != 0, pS->hdg_dashes != 0, pS->hdg_dot != 0, pS->alt_dot != 0, pS->vs_dashes != 0);
                //printf("FPA:%f Heading:%d Alt selected:%d VS Selected:%d\n", pS->fpa_selected, pS->heading_selected, pS->alt_selected, pS->vs_selected);
                //printf("SPD_MACH:%d V/S-FPA:%d Alt INC:%d\n", pS->spd_mach != 0, pS->trk_fpa_mode != 0, pS->alt_inc);
                //uint8_t* data = (uint8_t*)&panel_state;
                panel_state.ap_master = pS->ap_master != 0;
                panel_state.ap1_active = pS->ap1_master != 0;
                panel_state.ap2_active = pS->ap2_master != 0;
                panel_state.ap_throttle_active = pS->ap_throttle != 0;
                panel_state.loc_mode_active = pS->loc_mode_active != 0;
                panel_state.exped_mode_active = pS->exped_mode_active != 0;
                panel_state.appr_mode_active = pS->appr_mode_active != 0;
                panel_state.spd_dashes = pS->spd_dashes != 0;
                panel_state.spd_dot = pS->spd_dot != 0;
                panel_state.hdg_dashes = pS->hdg_dashes != 0;
                panel_state.hdg_dot = pS->hdg_dot != 0;
                panel_state.alt_dot = pS->alt_dot != 0;
                panel_state.spd_mach_mode = pS->spd_mach != 0;
                panel_state.trk_fpa_mode = pS->trk_fpa_mode != 0;
                panel_state.alt_increment = pS->alt_inc != 0;
                panel_state.vs_dashes = pS->vs_dashes != 0;
                //(UIN16)*data=(pS->ap_master != 0) << 15 | (pS->ap1_master != 0) << 14 | (pS->ap2_master != 0) << 13 | 
                //    () << 12 | () << 11 | () << 10 | () << 9 |
                //    () << 8 | () << 7 | () << 6 | () << 5 |
                //    () << 4 | () << 3 | () << 2 | () << 1 | ();

                panel_state.fpa_selected = (INT8)(pS->fpa_selected * 10);
                panel_state.hdg_selected = pS->heading_selected;
                
                //if (panel_state.spd_mach_mode)
                panel_state.spd_selected = (float)pS->ap_airspeed_hold_var;
                panel_state.alt_selected = pS->alt_selected;
                panel_state.vs_selected = pS->vs_selected/100;
                
                hid_data_send(&panel_state);

            }
            break;
        }
        case REQUEST_AIRCRAFT_VAR:
        {
            DWORD ObjectID = pObjData->dwObjectID;
            aircraft_var_t* pS = (aircraft_var_t*)&pObjData->dwData;
            {
                aircraft_var.metric_mode_active = pS->metric_mode_active != 0;
            }
            break;
        }

        default:
            break;
        }
        break;
    }
    case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE:
    {
        SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE*)pData;

        switch (pObjData->dwRequestID)
        {
        case REQUEST_1:
        {
            //DWORD ObjectID = pObjData->dwObjectID;
            //Struct1* pS = (Struct1*)&pObjData->dwData;
            //if (SUCCEEDED(StringCbLengthA(&pS->title[0], sizeof(pS->title), NULL))) // security check
            //{
            //    printf("\nObjectID=%d  Title=\"%s\"\nLat=%f  Lon=%f  Alt=%f  Kohlsman=%.2f", ObjectID, pS->title, pS->latitude, pS->longitude, pS->altitude, pS->kohlsmann);
            //}
            //break;
        }

        default:
            break;
        }
        break;
    }


    case SIMCONNECT_RECV_ID_QUIT:
    {
        quit = 1;
        break;
    }

    default:
        //printf("\nReceived:%d", pData->dwID);
        break;
    }
}

void testDataRequest()
{
    HRESULT hr;

    if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Request Data", NULL, 0, 0, 0)))
    {
        printf("\nConnected to Flight Simulator!");

        // Set up the data definition, but do not yet do anything with it
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Title", NULL, SIMCONNECT_DATATYPE_STRING256);
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Kohlsman setting hg", "inHg");
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Plane Altitude", "feet");
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Plane Latitude", "degrees");
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "Plane Longitude", "degrees");

        // Set up the data definition, but do not yet do anything with it
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOPILOT_ACTIVE", "Bool");//FCU AP Master
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOPILOT_1_ACTIVE", "Bool");//FCU AP1
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOPILOT_2_ACTIVE", "Bool");//FCU AP2
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOTHRUST_STATUS", NULL);//FCU A/THR 0=Disengaged, 1=Armed, 2=Active
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_LOC_MODE_ACTIVE", "Bool");//指示 FCU 上的 LOC 按钮是否亮起
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FMA_EXPEDITE_MODE", "Bool");//指示是否开启加速模式 //指示 FCU 上的 LOC 按钮是否亮起
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_APPR_MODE_ACTIVE", "Bool");//指示 FCU 上的 APPR 按钮是否亮起
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_SPD_MANAGED_DASHES", "Bool");//指示管理速度/马赫模式是否处于活动状态且不显示数值
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_SPD_MANAGED_DOT", "Bool");//指示管理速度/马赫模式是否处于活动状态
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_HDG_MANAGED_DASHES", "Bool");//指示托管Heading模式是否处于活动状态且不显示数值
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_HDG_MANAGED_DOT", "Bool");//指示管理航向模式是否处于活动状态
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_ALT_MANAGED", "Bool");//指示管理高度模式是否处于活动状态（点）
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_VS_MANAGED", "Bool");//指示托管 VS/FPA 模式是否处于活动状态
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_TRK_FPA_MODE_ACTIVE", "Bool");//如果 TRK/FPA 模式处于活动状态，则为 true
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "AUTOPILOT MANAGED SPEED IN MACH", "Bool");//SPD/MACH 0=knots 1=mach

        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOPILOT_SPEED_SELECTED", NULL);
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOPILOT_FPA_SELECTED", "degrees");//指示 FCU 上选定的 FPA，即时更新
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOPILOT_HEADING_SELECTED", "degrees", SIMCONNECT_DATATYPE_INT32);//指示 FCU 上的选定heading，在托管标题模式下，值为 - 1
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "AUTOPILOT ALTITUDE LOCK VAR:3", "feet", SIMCONNECT_DATATYPE_INT32);// 用于 DES 模式下的垂直引导,指示目标高度
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:XMLVAR_AUTOPILOT_ALTITUDE_INCREMENT", "feet", SIMCONNECT_DATATYPE_INT32);// 高度增量 100|1000
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOPILOT_VS_SELECTED", "feet per minute", SIMCONNECT_DATATYPE_INT32);// V/S显示


        // Request an event when the simulation starts
        hr = SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_START, "SimStart");
        hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_FCU_PANEL, DEFINITION_FCU_PANEL, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND);


        // Registe Client Event
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_AP1_PUSH, "A32NX.FCU_AP_1_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_AP2_PUSH, "A32NX.FCU_AP_2_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_ATHR_PUSH, "A32NX.FCU_ATHR_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_LOC_PUSH, "A32NX.FCU_LOC_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_EXPED_PUSH, "A32NX.FCU_EXPED_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_APPR_PUSH, "A32NX.FCU_APPR_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_SPD_MACH_PUSH, "A32NX.FCU_SPD_MACH_TOGGLE_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_TRK_FPA_PUSH, "A32NX.FCU_TRK_FPA_TOGGLE_PUSH");
        //hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_METRIC_ALT_PUSH, ""); //该事件在switch中直接处理变量

        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_SPD_PUSH, "A32NX.FCU_SPD_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_SPD_PULL, "A32NX.FCU_SPD_PULL");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_SPD_INC, "A32NX.FCU_SPD_INC");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_SPD_DEC, "A32NX.FCU_SPD_DEC");

        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_HDG_PUSH, "A32NX.FCU_HDG_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_HDG_PULL, "A32NX.FCU_HDG_PULL");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_HDG_INC, "A32NX.FCU_HDG_INC");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_HDG_DEC, "A32NX.FCU_HDG_DEC");

        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_ALT_PUSH, "A32NX.FCU_ALT_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_ALT_PULL, "A32NX.FCU_ALT_PULL");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_ALT_INC, "A32NX.FCU_ALT_INC");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_ALT_DEC, "A32NX.FCU_ALT_DEC");

        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_VS_PUSH, "A32NX.FCU_VS_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_VS_PULL, "A32NX.FCU_VS_PULL");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_VS_INC, "A32NX.FCU_VS_INC");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_VS_DEC, "A32NX.FCU_VS_DEC");


        hr = SimConnect_SetNotificationGroupPriority(hSimConnect, GROUP_FCU_PANEL, SIMCONNECT_GROUP_PRIORITY_HIGHEST);

        //SimConnect_TransmitClientEvent(hSimConnect, 0, EVENT_A32X_FCU_AP1_PUSH, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

        printf("\nLaunch a flight.");

        while (0 == quit)
        {
            SimConnect_CallDispatch(hSimConnect, MyDispatchProcRD, NULL);
            Sleep(1);
        }
        //while (true)
        //{
        //    SimConnect_CallDispatch(hSimConnect, MyDispatchProcRD, NULL);
        //    Sleep(1);
        //}
        hr = SimConnect_Close(hSimConnect);
    }
    else
    {
        printf("Connect Failed!\n");
    }
}

int init_SimConnect() {
    HRESULT hr;

    if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Request Data", NULL, 0, 0, 0)))
    {
        FCU_DEBUG("Connected to Flight Simulator!");

        // Set up the data definition, but do not yet do anything with it 需要请求的数据
        // ------------------------------------------------ DEFINITION_FCU_PANEL-------------------------------------------------------
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOPILOT_ACTIVE", "Bool");//FCU AP Master
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOPILOT_1_ACTIVE", "Bool");//FCU AP1
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOPILOT_2_ACTIVE", "Bool");//FCU AP2
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOTHRUST_STATUS", NULL);//FCU A/THR 0=Disengaged, 1=Armed, 2=Active
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_LOC_MODE_ACTIVE", "Bool");//指示 FCU 上的 LOC 按钮是否亮起
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FMA_EXPEDITE_MODE", "Bool");//指示是否开启加速模式 //指示 FCU 上的 LOC 按钮是否亮起
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_APPR_MODE_ACTIVE", "Bool");//指示 FCU 上的 APPR 按钮是否亮起
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_SPD_MANAGED_DASHES", "Bool");//指示管理速度/马赫模式是否处于活动状态且不显示数值
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_SPD_MANAGED_DOT", "Bool");//指示管理速度/马赫模式是否处于活动状态
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_HDG_MANAGED_DASHES", "Bool");//指示托管Heading模式是否处于活动状态且不显示数值
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_HDG_MANAGED_DOT", "Bool");//指示管理航向模式是否处于活动状态
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_ALT_MANAGED", "Bool");//指示管理高度模式是否处于活动状态（点）
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_FCU_VS_MANAGED", "Bool");//指示托管 VS/FPA 模式是否处于活动状态
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_TRK_FPA_MODE_ACTIVE", "Bool");//如果 TRK/FPA 模式处于活动状态，则为 true
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "AUTOPILOT MANAGED SPEED IN MACH", "Bool");//SPD/MACH 0=knots 1=mach
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOPILOT_SPEED_SELECTED", NULL);
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOPILOT_FPA_SELECTED", "degrees");//指示 FCU 上选定的 FPA，即时更新
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOPILOT_HEADING_SELECTED", "degrees", SIMCONNECT_DATATYPE_INT32);//指示 FCU 上的选定heading，在托管标题模式下，值为 - 1
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "AUTOPILOT ALTITUDE LOCK VAR:3", "feet", SIMCONNECT_DATATYPE_INT32);// 用于 DES 模式下的垂直引导,指示目标高度
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:XMLVAR_AUTOPILOT_ALTITUDE_INCREMENT", "feet", SIMCONNECT_DATATYPE_INT32);// 高度增量 100|1000
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_FCU_PANEL, "L:A32NX_AUTOPILOT_VS_SELECTED", "feet per minute", SIMCONNECT_DATATYPE_INT32);// V/S显示

        hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_FCU_PANEL, DEFINITION_FCU_PANEL, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);
        // ------------------------------------------------ DEFINITION_FCU_PANEL-------------------------------------------------------
        hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_AIRCRAFT_VAR, "L:A32NX_METRIC_ALT_TOGGLE", "Bool");//是否开启PFD米制单位显示

        hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_AIRCRAFT_VAR, DEFINITION_AIRCRAFT_VAR, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);

        // Request an event when the simulation starts
        hr = SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_START, "SimStart");


        // Registe Client Event 注册客户端事件
        // ------------------------------------------------ Client Event Map -------------------------------------------------------
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_AP1_PUSH, "A32NX.FCU_AP_1_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_AP2_PUSH, "A32NX.FCU_AP_2_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_ATHR_PUSH, "A32NX.FCU_ATHR_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_LOC_PUSH, "A32NX.FCU_LOC_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_EXPED_PUSH, "A32NX.FCU_EXPED_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_APPR_PUSH, "A32NX.FCU_APPR_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_SPD_MACH_PUSH, "A32NX.FCU_SPD_MACH_TOGGLE_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_TRK_FPA_PUSH, "A32NX.FCU_TRK_FPA_TOGGLE_PUSH");
        //hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_METRIC_ALT_PUSH, ""); //该事件在switch中直接处理变量

        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_SPD_PUSH, "A32NX.FCU_SPD_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_SPD_PULL, "A32NX.FCU_SPD_PULL");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_SPD_INC, "A32NX.FCU_SPD_INC");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_SPD_DEC, "A32NX.FCU_SPD_DEC");

        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_HDG_PUSH, "A32NX.FCU_HDG_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_HDG_PULL, "A32NX.FCU_HDG_PULL");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_HDG_INC, "A32NX.FCU_HDG_INC");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_HDG_DEC, "A32NX.FCU_HDG_DEC");

        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_ALT_PUSH, "A32NX.FCU_ALT_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_ALT_PULL, "A32NX.FCU_ALT_PULL");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_ALT_INC, "A32NX.FCU_ALT_INC");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_ALT_DEC, "A32NX.FCU_ALT_DEC");

        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_VS_PUSH, "A32NX.FCU_VS_PUSH");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_VS_PULL, "A32NX.FCU_VS_PULL");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_VS_INC, "A32NX.FCU_VS_INC");
        hr = SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_A32X_FCU_VS_DEC, "A32NX.FCU_VS_DEC");

        // ------------------------------------------------ Notification Priority -------------------------------------------------------
        //hr = SimConnect_SetNotificationGroupPriority(hSimConnect, GROUP_FCU_PANEL, SIMCONNECT_GROUP_PRIORITY_HIGHEST);

        //hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_FCU_PANEL, DEFINITION_FCU_PANEL, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);

        FCU_INFO("Simconnect 初始化完毕");
    }
    else
    {
        FCU_CRITICAL("请先启动游戏!");
        return 1;
    }
    return 0;
}

void deinit_SimConnect() {
    if (hSimConnect!=NULL)
    {
        hr = SimConnect_Close(hSimConnect);
        FCU_TRACE("SimConnect_Close(hSimConnect)");
    }
    else
    {
        FCU_DEBUG("SimConnect 未初始化");
    }
}

void aircraft_event_send(EVENT_ID event){
    FCU_DEBUG("Transmit event {}",(UINT8)event);
    switch (event)
    {
    case EVENT_A32X_FCU_METRIC_ALT_PUSH:
        aircraft_var.metric_mode_active = 1 - aircraft_var.metric_mode_active;
        hr = SimConnect_SetDataOnSimObject(hSimConnect, DEFINITION_AIRCRAFT_VAR, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(aircraft_var), &aircraft_var);
        break;

    default:
        SimConnect_TransmitClientEvent(hSimConnect, 0, event, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
        break;
    }
}

void simconnect_call() {
    SimConnect_CallDispatch(hSimConnect, MyDispatchProcRD, NULL);
}
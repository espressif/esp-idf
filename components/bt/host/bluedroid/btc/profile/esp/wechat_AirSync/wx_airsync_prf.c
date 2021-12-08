#include "wx_airsync_prf.h"
/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#if (WX_AIRSYNC_CFG)

#include <stdint.h>

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "common/bt_target.h"
#include "common/bt_trace.h"
#include "stack/bt_types.h"
#include "stack/gatt_api.h"
#include "bta/bta_api.h"
#include "bta/bta_gatt_api.h"
#include "bta_gatts_int.h"



tAIRSYNC_CB_ENV airsync_cb_env;


/*****************************************************************************
**  Constants
*****************************************************************************/
static void airsync_profile_cb(esp_gatts_evt_t event,  esp_gatts_t *p_data);


/*******************************************************************************
**
** Function         airsync_profile_cb
**
** Description      the callback function after the profile has been register to the BTA manager module
**
** Returns          NULL
**
*******************************************************************************/
static void airsync_profile_cb(esp_gatts_evt_t event, esp_gatts_t *p_data)
{
    esp_gatts_rsp_t rsp;
    esp_bt_uuid_t uuid = {LEN_UUID_16, {ATT_SVC_AIRSYNC}};
    tAirSync_INST  *p_inst = &airsync_cb_env.airsync_inst;


    BTC_TRACE_ERROR("airsync profile cb event = %x\n", event);
    switch (event) {
    case ESP_GATTS_REG_EVT:

        if (p_data->reg_oper.status != BTA_GATT_OK) {
            BTC_TRACE_ERROR("button profile register failed\n");
        }
        airsync_cb_env.gatt_if = p_data->reg_oper.server_if;
        airsync_cb_env.enabled = true;

        if (p_data->reg_oper.uuid.uu.uuid16 == ATT_SVC_AIRSYNC) {
            AirSync_CreateService();
        }
        break;
    case ESP_GATTS_READ_EVT:

        if (airsync_cb_env.clcb.connected && airsync_cb_env.enabled) {
            //tBTA_GATTS_RSP rsp;
            memset(&rsp, 0, sizeof(tBTA_GATTS_API_RSP));
            rsp.attr_value.handle = p_data->req_data.p_data->read_req.handle;
            rsp.attr_value.len = 2;
            esp_ble_gatts_send_rsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                                   p_data->req_data.status, &rsp);
        }
        break;
    case ESP_GATTS_WRITE_EVT:
        if (airsync_cb_env.clcb.connected && airsync_cb_env.enabled) {
            esp_ble_gatts_send_rsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                                   p_data->req_data.status, NULL);

        }
        break;
    case ESP_GATTS_CFM_EVT:

        break;
    case ESP_GATTS_CREATE_EVT:
        uuid.uu.uuid16 = ATT_CHAR_AIRSYNC_WIT;

        airsync_cb_env.clcb.cur_srvc_id =  p_data->create.service_id;
        airsync_cb_env.is_primery =  p_data->create.is_primary;
        //start the airsync service after created
        esp_ble_gatts_start_srvc(p_data->create.service_id);
        //add the frist airsync characteristic --> write characteristic
        esp_ble_gatts_add_char(airsync_cb_env.clcb.cur_srvc_id, &uuid,
                               (GATT_PERM_WRITE | GATT_PERM_READ),
                               (GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_WRITE));
        break;

    case ESP_GATTS_ADD_CHAR_EVT:
        if (p_data->add_result.char_uuid.uu.uuid16 == ATT_CHAR_AIRSYNC_WIT) {
            uuid.uu.uuid16 = ATT_CHAR_AIRSYBC_NTF;
            //tBTA_GATT_PERM perm = GATT_PERM_READ;
            //tBTA_GATT_CHAR_PROP prop = (GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_NOTIFY);
            //save the att handle to the env
            airsync_cb_env.airsync_inst.airsync_wirt_hdl = p_data->add_result.attr_id;
            //add the second airsync characteristic --> Notify characteristic
            esp_ble_gatts_add_char(airsync_cb_env.clcb.cur_srvc_id, &uuid,
                                   GATT_PERM_READ, (GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_INDICATE));
        } else if (p_data->add_result.char_uuid.uu.uuid16 == ATT_CHAR_AIRSYBC_NTF) {
            //tBTA_GATT_PERM perm = (GATT_PERM_WRITE|GATT_PERM_WRITE);
            uuid.uu.uuid16 = GATT_UUID_CHAR_CLIENT_CONFIG;
            airsync_cb_env.airsync_inst.airsync_ntf_hdl = p_data->add_result.attr_id;
            esp_ble_gatts_add_char_descr (airsync_cb_env.clcb.cur_srvc_id,
                                          (GATT_PERM_WRITE | GATT_PERM_WRITE),
                                          &uuid);

            uuid.uu.uuid16 = ATT_CHAR_AIRSYNC_READ;
            //add the third airsync characteristic --> Read characteristic
            esp_ble_gatts_add_char(airsync_cb_env.clcb.cur_srvc_id, &uuid,
                                   GATT_PERM_READ,
                                   GATT_CHAR_PROP_BIT_READ);
        } else if (p_data->add_result.char_uuid.uu.uuid16 == ATT_CHAR_AIRSYNC_READ) {
            airsync_cb_env.airsync_inst.airsync_read_hdl = p_data->add_result.attr_id;
        }

        break;
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        if (p_data->add_result.char_uuid.uu.uuid16 == GATT_UUID_CHAR_CLIENT_CONFIG) {
            airsync_cb_env.airsync_inst.airsync_cfg_hdl = p_data->add_result.attr_id;
        }
        break;
    case ESP_GATTS_CONNECT_EVT:
        //set the connection flag to true
        airsync_env_clcb_alloc(p_data->conn.conn_id, p_data->conn.remote_bda);
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        //set the connection flag to true
        airsync_cb_env.clcb.connected = false;
        break;
    case ESP_GATTS_OPEN_EVT:
        break;
    case ESP_GATTS_CLOSE_EVT:
        if (airsync_cb_env.clcb.connected && (airsync_cb_env.clcb.conn_id == p_data->conn.conn_id)) {
            //set the connection channal congested flag to true
            airsync_cb_env.clcb.congest =  p_data->congest.congested;
        }
        break;
    case ESP_GATTS_CONGEST_EVT:
        //set the congest flag
        airsync_cb_env.clcb.congest = p_data->congest.congested;
        break;
    default:
        break;
    }
}


/*******************************************************************************
**
** Function         AirSync_CreateService
**
** Description      Create a Service for the airsync profile
**
** Returns          NULL
**
*******************************************************************************/
void AirSync_CreateService(void)
{
    esp_gatts_if_t server_if ;
    esp_bt_uuid_t uuid = {LEN_UUID_16, {ATT_SVC_AIRSYNC}};
    UINT16 num_handle = WX_IDX_NB;
    UINT8 inst = 0x00;
    server_if = airsync_cb_env.gatt_if;
    airsync_cb_env.inst_id = inst;

    esp_ble_gatts_create_srvc(server_if, &uuid, inst, num_handle, true);

}

/*******************************************************************************
**
** Function         airsync_env_clcb_alloc
**
** Description      The function allocates a GATT profile  connection link control block
**
** Returns          NULL if not found. Otherwise pointer to the connection link block.
**
*******************************************************************************/
tAirSync_CLCB *airsync_env_clcb_alloc (UINT16 conn_id, BD_ADDR remote_bda)
{
    tAirSync_CLCB *p_clcb = NULL;
    p_clcb = &airsync_cb_env.clcb;

    if (!p_clcb->in_use) {
        p_clcb->in_use = TRUE;
        p_clcb->conn_id = conn_id;
        BTC_TRACE_ERROR("p_clcb->conn_id = %x\n", conn_id);
        p_clcb->connected = TRUE;
        memcpy(p_clcb->remote_bda, remote_bda, BD_ADDR_LEN);
    }

    return p_clcb;
}

/*******************************************************************************
**
** Function         airsync_env_find_conn_id_by_bd_adddr
**
** Description      The function searches all LCB with macthing bd address
**
** Returns          total number of clcb found.
**
*******************************************************************************/
UINT16 airsync_env_find_conn_id_by_bd_adddr(BD_ADDR remote_bda)
{
    UINT8 i_clcb;
    tAirSync_CLCB *p_clcb = NULL;

    for (i_clcb = 0, p_clcb = &airsync_cb_env.clcb; i_clcb < 1; i_clcb++, p_clcb++) {
        if (p_clcb->in_use && p_clcb->connected && memcmp(p_clcb->remote_bda, remote_bda, BD_ADDR_LEN)) {
            return p_clcb->conn_id;
        }
    }

    return GATT_INVALID_CONN_ID;
}


/*******************************************************************************
**
** Function         airsync_init
**
** Description      Initializa the GATT Service for airsync profiles.
**
*******************************************************************************/
tGATT_STATUS AirSync_Init(tAIRSYNC_CBACK *call_back)
{
    esp_bt_uuid_t app_uuid = {LEN_UUID_16, {ATT_SVC_AIRSYNC}};


    if (airsync_cb_env.enabled) {
        BTC_TRACE_ERROR("airsync svc already initaliezd\n");
        return ESP_GATT_ERROR;
    } else {
        memset(&airsync_cb_env, 0, sizeof(tAIRSYNC_CB_ENV));
    }


    if (call_back != NULL) {
        airsync_cb_env.airsync_inst.p_cback = call_back;
    }


    /* register the airsync profile to the BTA_GATTS module*/
    esp_ble_gatts_app_register(&app_uuid, airsync_profile_cb);

    airsync_cb_env.enabled = TRUE;

    return ESP_GATT_OK;
}

#endif  ///WX_AIRSYNC_CFG

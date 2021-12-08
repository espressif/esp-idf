/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
#include "button_pro.h"

#include "prf_defs.h"

#if (BUT_PROFILE_CFG)


#define ARRAY_SIZE(x)   (sizeof(x)/sizeof((x)[0]))

button_env_cb_t button_cb_env;




/*****************************************************************************
**  Constants
*****************************************************************************/
static void button_profile_cb(esp_gatts_evt_t event,  esp_gatts_t *p_data);


/*******************************************************************************
**
** Function         button_profile_cb
**
** Description      the callback function after the profile has been register to the BTA manager module
**
** Returns          NULL
**
*******************************************************************************/
static void button_profile_cb(esp_gatts_evt_t event, esp_gatts_t *p_data)
{
    esp_gatts_rsp_t rsp;
    esp_bt_uuid_t uuid = {LEN_UUID_16, {ATT_SVC_BUTTON}};
    but_inst_t  *p_inst = &button_cb_env.button_inst;
    uint8_t net_event = 0xff;
    uint8_t len = 0;
    uint8_t *p_rec_data = NULL;
    //BTC_TRACE_ERROR("p_data->status = %x\n",p_data->status);
    //if(p_data->status != BTA_GATT_OK){
    //  BTC_TRACE_ERROR("button profile register failed\n");
    //  return;
    //}
    BTC_TRACE_ERROR("button profile cb event = %x\n", event);
    switch (event) {
    case ESP_GATTS_REG_EVT:

        BTC_TRACE_ERROR("p_data->reg_oper.status = %x\n", p_data->reg_oper.status);
        BTC_TRACE_ERROR("(p_data->reg_oper.uuid.uu.uuid16=%x\n", p_data->reg_oper.uuid.uu.uuid16);
        if (p_data->reg_oper.status != BTA_GATT_OK) {
            BTC_TRACE_ERROR("button profile register failed\n");
        }
        button_cb_env.gatt_if = p_data->reg_oper.server_if;
        button_cb_env.enabled = true;
        //button_cb_env.button_inst.app_id = p_data->reg_oper.uuid;
        //create the button service to the service data base.
        if (p_data->reg_oper.uuid.uu.uuid16 == ATT_SVC_BUTTON) {
            Button_CreateService();
        }
        break;
    case ESP_GATTS_READ_EVT:
        //tBTA_GATTS_RSP rsp;
        memset(&rsp, 0, sizeof(tBTA_GATTS_API_RSP));
        rsp.attr_value.handle = p_data->req_data.p_data->read_req.handle;
        rsp.attr_value.len = 2;
        esp_ble_gatts_send_rsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                               p_data->req_data.status, &rsp);
        break;
    case ESP_GATTS_WRITE_EVT:
        esp_ble_gatts_send_rsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                               p_data->req_data.status, NULL);
        BTC_TRACE_ERROR("Received button data:");
        for (int i = 0; i < p_data->req_data.p_data->write_req.len; i++) {
            BTC_TRACE_ERROR("%x", p_data->req_data.p_data->write_req.value[i]);
        }
        BTC_TRACE_ERROR("\n");
        if (p_data->req_data.p_data->write_req.handle == button_cb_env.button_inst.but_wirt_hdl) {

            p_rec_data = &p_data->req_data.p_data->write_req.value[0];
            //  button_msg_notify(len,p_rec_data);
            (*p_inst->p_cback)(button_cb_env.button_inst.app_id, net_event, len, p_rec_data);

        }
        break;
    case ESP_GATTS_CFM_EVT:

        break;
    case ESP_GATTS_CREATE_EVT:
        //tBT_UUID uuid_butt_write;
        uuid.uu.uuid16 = ATT_CHAR_BUTTON_WIT;
        //tBTA_GATT_PERM perm = (GATT_PERM_WRITE|GATT_PERM_READ);
        //tBTA_GATT_CHAR_PROP prop = (GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_WRITE);
        //uuid = {LEN_UUID_16, {ATT_SVC_BUTTON}};
        button_cb_env.clcb.cur_srvc_id =  p_data->create.service_id;
        button_cb_env.is_primery =  p_data->create.is_primary;
        //uuid = {LEN_UUID_16, {ATT_CHAR_BUTTON_WIT}};
        //start the button service after created
        esp_ble_gatts_start_srvc(p_data->create.service_id);
        //add the frist button characteristic --> write characteristic
        esp_ble_gatts_add_char(button_cb_env.clcb.cur_srvc_id, &uuid,
                               (GATT_PERM_WRITE | GATT_PERM_READ),
                               (GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_WRITE));
        break;

    case ESP_GATTS_ADD_CHAR_EVT:
        if (p_data->add_result.char_uuid.uu.uuid16 == ATT_CHAR_BUTTON_WIT) {
            uuid.uu.uuid16 = ATT_CHAR_BUTTON_NTF;
            //tBTA_GATT_PERM perm = GATT_PERM_READ;
            tBTA_GATT_CHAR_PROP prop = (GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_NOTIFY);
            //save the att handle to the env
            button_cb_env.button_inst.but_wirt_hdl = p_data->add_result.attr_id;
            //add the frist button characteristic --> Notify characteristic
            esp_ble_gatts_add_char(button_cb_env.clcb.cur_srvc_id, &uuid,
                                   GATT_PERM_READ, (GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_NOTIFY));
        } else if (p_data->add_result.char_uuid.uu.uuid16 == ATT_CHAR_BUTTON_NTF) { // add the gattc config descriptor to the notify charateristic
            //tBTA_GATT_PERM perm = (GATT_PERM_WRITE|GATT_PERM_WRITE);
            uuid.uu.uuid16 = GATT_UUID_CHAR_CLIENT_CONFIG;
            button_cb_env.button_inst.but_ntf_hdl = p_data->add_result.attr_id;
            esp_ble_gatts_add_char_descr (button_cb_env.clcb.cur_srvc_id,
                                          (GATT_PERM_WRITE | GATT_PERM_WRITE),
                                          &uuid);
        }

        break;
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        if (p_data->add_result.char_uuid.uu.uuid16 == GATT_UUID_CHAR_CLIENT_CONFIG) {
            button_cb_env.button_inst.but_cfg_hdl = p_data->add_result.attr_id;
        }
        ///Start advertising
        BTC_TRACE_ERROR("\n*******Start sent the ADV.*************\n");
        //esp_ble_start_advertising (&adv_params);
        //BTA_GATTS_Listen(button_cb_env.gatt_if, true, NULL);
        break;
    case ESP_GATTS_CONNECT_EVT:
        BTC_TRACE_ERROR("############BUTTON CONNCET EVT################\n");
        //esp_ble_stop_advertising();
        //set the connection flag to true
        button_env_clcb_alloc(p_data->conn.conn_id, p_data->conn.remote_bda);
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        //set the connection flag to true
        button_cb_env.clcb.connected = false;
        break;
    case ESP_GATTS_OPEN_EVT:
        ///stop the advertising after connected

        break;
    case ESP_GATTS_CLOSE_EVT:
        if (button_cb_env.clcb.connected && (button_cb_env.clcb.conn_id == p_data->conn.conn_id)) {
            //set the connection channal congested flag to true
            button_cb_env.clcb.congest =  p_data->congest.congested;
        }
        break;
    case ESP_GATTS_CONGEST_EVT:
        break;
    default:
        break;
    }
}


/*******************************************************************************
**
** Function         Button_CreateService
**
** Description      Create a Service for the button profile
**
** Returns          NULL
**
*******************************************************************************/
void Button_CreateService(void)
{
    esp_gatts_if_t server_if ;
    esp_bt_uuid_t uuid = {LEN_UUID_16, {ATT_SVC_BUTTON}};
    uint16_t num_handle = KEY_IDX_NB;
    uint8_t inst = 0x00;
    server_if = button_cb_env.gatt_if;
    button_cb_env.inst_id = inst;
    //if(!button_cb_env.enabled)
    //{
    //  BTC_TRACE_ERROR("button service added error.");
    //}
    esp_ble_gatts_create_srvc(server_if, &uuid, inst, num_handle, true);

}

/*******************************************************************************
**
** Function         button_env_clcb_alloc
**
** Description      The function allocates a GATT profile  connection link control block
**
** Returns          NULL if not found. Otherwise pointer to the connection link block.
**
*******************************************************************************/
but_clcb_t *button_env_clcb_alloc (uint16_t conn_id, BD_ADDR remote_bda)
{
    but_clcb_t *p_clcb = NULL;
    p_clcb = &button_cb_env.clcb;

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
** Function         button_env_find_conn_id_by_bd_adddr
**
** Description      The function searches all LCB with macthing bd address
**
** Returns          total number of clcb found.
**
*******************************************************************************/
uint16_t button_env_find_conn_id_by_bd_adddr(BD_ADDR remote_bda)
{
    uint8_t i_clcb;
    but_clcb_t *p_clcb = NULL;

    for (i_clcb = 0, p_clcb = &button_cb_env.clcb; i_clcb < BUTT_MAX_APPS; i_clcb++, p_clcb++) {
        if (p_clcb->in_use && p_clcb->connected && memcmp(p_clcb->remote_bda, remote_bda, BD_ADDR_LEN)) {
            return p_clcb->conn_id;
        }
    }

    return GATT_INVALID_CONN_ID;
}

/*******************************************************************************
**
** Function         button_env_clcb_dealloc
**
** Description      The function deallocates a GATT profile  connection link control block
**
** Returns          True the deallocation is successful
**
*******************************************************************************/

BOOLEAN button_env_clcb_dealloc(uint16_t conn_id)
{
    uint16_t i_clcb = 0;
    but_clcb_t *p_clcb = NULL;

    for (i_clcb = 0, p_clcb = &button_cb_env.clcb; i_clcb < 1; i_clcb++, p_clcb++) {
        if (p_clcb->in_use && p_clcb->connected && (p_clcb->conn_id == conn_id)) {
            memset(p_clcb, 0, sizeof(but_clcb_t));
            return TRUE;
        }
    }

    return FALSE;
}

/*******************************************************************************
**
** Function         button_init
**
** Description      Initializa the GATT Service for button profiles.
**
*******************************************************************************/
esp_gatt_status_t button_init (but_prf_cb_t call_back)
{
    tBT_UUID app_uuid = {LEN_UUID_16, {ATT_SVC_BUTTON}};

    BTC_TRACE_ERROR("\n=============================button_init==============================================\n");
    if (button_cb_env.enabled) {
        BTC_TRACE_ERROR("button svc already initaliezd\n");
        return ESP_GATT_ERROR;
    } else {
        memset(&button_cb_env, 0, sizeof(button_env_cb_t));
    }


    if (call_back != NULL) {
        button_cb_env.button_inst.p_cback = call_back;
    }


    /* register the button profile to the BTA_GATTS module*/
    esp_ble_gatts_app_register(&app_uuid, button_profile_cb);

    button_cb_env.enabled = TRUE;

    return ESP_GATT_OK;
}

void button_disable(uint16_t connid)
{
    button_env_clcb_dealloc(connid);
}


void button_msg_notify(uint16_t len, uint8_t *button_msg)
{
    BOOLEAN conn_status = button_cb_env.clcb.connected;
    uint16_t conn_id = button_cb_env.clcb.conn_id;
    uint16_t attr_id = button_cb_env.button_inst.but_ntf_hdl;
    //notify rsp==false; indicate rsp==true.
    BOOLEAN rsp = false;
    if (!conn_status && button_cb_env.clcb.congest) {
        BTC_TRACE_ERROR("the conneciton for button profile has been loss\n");
        return;
    }

    esp_ble_gatts_hdl_val_indica (conn_id, attr_id, len,
                                  button_msg, rsp);
}

#endif  ///BUT_PROFILE_CFG

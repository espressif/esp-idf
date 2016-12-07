// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>

#include <string.h>
#include <stdbool.h>
#include <stdio.h>


#include "bt_target.h"
#include "bt_trace.h"
#include "bt_types.h"
#include "gatt_api.h"
#include "bta_api.h"
#include "bta_gatt_api.h"
#include "bta_gatts_int.h"

#include "btc_blufi_prf.h"
#include "btc_task.h"
#include "btc_manage.h"

#include "blufi_adv.h"
#include "blufi_int.h"

const char success_msg[] = "BLUFI_CONFIG_OK";
const char failed_msg[] = "BLUFI_CONFIG_FAILED";

#define BTC_BLUFI_CB_TO_APP(_event, _param) ((esp_profile_cb_t)btc_profile_cb_get(BTC_PID_BLUFI))(_event, _param)

#define BT_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]


UINT16 esp32_uuid = SVC_BLUFI_UUID;
UINT8 esp32_manu[17] = {0xff, 0x20, 0x14, 0x07, 0x22, 0x00, 0x02, 0x5B, 0x00, 0x33, 0x49, 0x31, 0x30, 0x4a, 0x30, 0x30, 0x31};
tBTA_BLE_MANU   p_esp32_manu = {sizeof(esp32_manu), esp32_manu};        /* manufacturer data */

tBTA_BLE_SERVICE esp32_service = {
    0x01,       //only one service in the ijiazu button profile
    false,
    &esp32_uuid
};        /* 16 bits services */


tBLUFI_BLE_ADV_DATA esp32_adv_data[ADV_SCAN_IDX_MAX] = {
    [BLE_ADV_DATA_IDX] = {
        .adv_name = "Espressif_008",
        {
            {0, 0},
            NULL,           //no manufature data to be setting in the esp32 adervetisiing datas
            &esp32_service,
            NULL,                   //the  128 bits service uuid set to null(not used)
            NULL,                   //the 32 bits Service UUID set to null(not used)
            NULL,                   //16 bits services Solicitation UUIDs set to null(not used)
            NULL,                   //List of 32 bit Service Solicitation UUIDs set to null(not used)
            NULL,                   //List of 128 bit Service Solicitation UUIDs set to null(not used)
            NULL,                   //proprietary data set to null(not used)
            NULL,                   //service data set not null(no service data to be sent)
            0x0200,                 //device type : generic display
            BTA_DM_GENERAL_DISC,    // General discoverable.
            0xFE                    //the tx power value,defult value is 0
        },
    },

    [BLE_SCAN_RSP_DATA_IDX] = {
        .adv_name = NULL,
        {
            {0, 0},
            &p_esp32_manu,
            NULL,
            NULL,                   //the  128 bits service uuid set to null(not used)
            NULL,                   //the 32 bits Service UUID set to null(not used)
            NULL,                   //16 bits services Solicitation UUIDs set to null(not used)
            NULL,                   //List of 32 bit Service Solicitation UUIDs set to null(not used)
            NULL,                   //List of 128 bit Service Solicitation UUIDs set to null(not used)
            NULL,                   //proprietary data set to null(not used)
            NULL,                   //service data set not null(no service data to be sent)
            0x0000,                 //device type : generic display
            0x00,                   // General discoverable.
            0x00
        },                  //the tx power value,defult value is 0
    }
};



static tBLUFI_CB_ENV blufi_cb_env;



/*****************************************************************************
**  Constants
*****************************************************************************/
static void blufi_profile_cb(tBTA_GATTS_EVT event,  tBTA_GATTS *p_data);


/*******************************************************************************
**
** Function         blufi_create_service
**
** Description      Create a Service for the blufi profile
**
** Returns          NULL
**
*******************************************************************************/
static void blufi_create_service(void)
{
    tBTA_GATTS_IF server_if ;
    tBT_UUID uuid = {LEN_UUID_16, {SVC_BLUFI_UUID}};
    UINT16 num_handle = BLUFI_HDL_NUM;
    UINT8 inst = 0x00;
    server_if = blufi_cb_env.gatt_if;
    blufi_cb_env.inst_id = inst;
    if (!blufi_cb_env.enabled) {
        LOG_ERROR("blufi service added error.");
        return;
    }
    BTA_GATTS_CreateService(server_if, &uuid, inst, num_handle, true);

}


/*******************************************************************************
**
** Function         blufi_profile_cb
**
** Description      the callback function after the profile has been register to the BTA manager module
**
** Returns          NULL
**
*******************************************************************************/
static void blufi_profile_cb(tBTA_GATTS_EVT event, tBTA_GATTS *p_data)
{
    tBTA_GATTS_RSP rsp;
    tBT_UUID uuid = {LEN_UUID_16, {SVC_BLUFI_UUID}};
    UINT8 *p_rec_data = NULL;
    tBTA_GATT_STATUS  status;

    LOG_DEBUG("blufi profile cb event = %x\n", event);
    switch (event) {
    case BTA_GATTS_REG_EVT:
        status = p_data->reg_oper.status;

        LOG_DEBUG("p_data->reg_oper.status = %x\n", p_data->reg_oper.status);
        LOG_DEBUG("(p_data->reg_oper.uuid.uu.uuid16=%x\n", p_data->reg_oper.uuid.uu.uuid16);
        if (p_data->reg_oper.status != BTA_GATT_OK) {
            LOG_ERROR("blufi profile register failed\n");
            return;
        }

        blufi_cb_env.gatt_if = p_data->reg_oper.server_if;
        blufi_cb_env.enabled = true;
        LOG_DEBUG("register complete: event=%d, status=%d, server_if=%d\n",
                  event, status, blufi_cb_env.gatt_if);

        LOG_DEBUG("set advertising parameters\n");
        //set the advertising data to the btm layer
        BlufiBleConfigadvData(&esp32_adv_data[BLE_ADV_DATA_IDX], NULL);
        //set the adversting data to the btm layer
        BlufiBleSetScanRsp(&esp32_adv_data[BLE_SCAN_RSP_DATA_IDX], NULL);
        BTA_GATTS_Listen(blufi_cb_env.gatt_if, true, NULL);

        //create the blufi service to the service data base.
        if (p_data->reg_oper.uuid.uu.uuid16 == SVC_BLUFI_UUID) {
            blufi_create_service();
        }
        break;
    case BTA_GATTS_READ_EVT:
        memset(&rsp, 0, sizeof(tBTA_GATTS_API_RSP));
        rsp.attr_value.handle = p_data->req_data.p_data->read_req.handle;
        rsp.attr_value.len = 2;
        //rsp.attr_value.value[0] = 0xde;
        //rsp.attr_value.value[1] = 0xed;
        //rsp.attr_value.value[2] = 0xbe;
        //rsp.attr_value.value[3] = 0xef;
        BTA_GATTS_SendRsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                          p_data->req_data.status, &rsp);
        break;
    case BTA_GATTS_WRITE_EVT:
        BTA_GATTS_SendRsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                          p_data->req_data.status, NULL);

        LOG_DEBUG("Received blufi data:");
        for (int i = 0; i < p_data->req_data.p_data->write_req.len; i++) {
            LOG_DEBUG("%x", p_data->req_data.p_data->write_req.value[i]);
        }
        LOG_DEBUG("\n");

        if (p_data->req_data.p_data->write_req.handle == blufi_cb_env.blufi_inst.blufi_hdl) {
            btc_msg_t msg;
            struct blufi_recv_evt_param recv_data;

            memset(&recv_data, 0x00, sizeof(struct blufi_recv_evt_param));

            p_rec_data = &p_data->req_data.p_data->write_req.value[0];
            recv_data.data_len = p_data->req_data.p_data->write_req.len;
            memcpy(recv_data.data, p_rec_data, recv_data.data_len);

            msg.sig = BTC_SIG_API_CB;
            msg.pid = BTC_PID_BLUFI;
            msg.act = BTC_BLUFI_CB_ACT_RECV_DATA;

            btc_transfer_context(&msg, &recv_data, sizeof(struct blufi_recv_evt_param), NULL);
        }
        break;
    case BTA_GATTS_CONF_EVT:
        /* Nothing */
        break;
    case BTA_GATTS_CREATE_EVT:
        uuid.uu.uuid16 = CHAR_BLUFI_UUID;
        blufi_cb_env.cur_srvc_id = p_data->create.service_id;
        blufi_cb_env.is_primery =  p_data->create.is_primary;
        //start the blufi service after created
        BTA_GATTS_StartService(p_data->create.service_id, BTA_GATT_TRANSPORT_LE);
        //add the frist blufi characteristic --> write characteristic
        BTA_GATTS_AddCharacteristic(blufi_cb_env.cur_srvc_id, &uuid,
                                    (GATT_PERM_WRITE | GATT_PERM_READ),
                                    (GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_WRITE | GATT_CHAR_PROP_BIT_NOTIFY));
        break;
    case BTA_GATTS_ADD_CHAR_EVT:
        if (p_data->add_result.char_uuid.uu.uuid16 == CHAR_BLUFI_UUID) {
            //save the att handle to the env
            blufi_cb_env.blufi_inst.blufi_hdl = p_data->add_result.attr_id;

            uuid.uu.uuid16 = GATT_UUID_CHAR_CLIENT_CONFIG;
            BTA_GATTS_AddCharDescriptor (blufi_cb_env.cur_srvc_id,
                                         (GATT_PERM_WRITE | GATT_PERM_WRITE),
                                         &uuid);
        }
        break;
    case BTA_GATTS_ADD_CHAR_DESCR_EVT: {
        /* call init finish */
        btc_msg_t msg;

        msg.sig = BTC_SIG_API_CB;
        msg.pid = BTC_PID_BLUFI;
        msg.act = BTC_BLUFI_CB_ACT_INIT_FINISH;
        btc_transfer_context(&msg, NULL, 0, NULL);
        break;
    }
    case BTA_GATTS_CONNECT_EVT:
        //set the connection flag to true
        LOG_ERROR("\ndevice is connected "BT_BD_ADDR_STR", server_if=%d,reason=0x%x,connect_id=%d\n",
                  BT_BD_ADDR_HEX(p_data->conn.remote_bda), p_data->conn.server_if,
                  p_data->conn.reason, p_data->conn.conn_id);

        blufi_cb_env.conn_id = p_data->conn.conn_id;

        /*return whether the remote device is currently connected*/
        int is_connected = BTA_DmGetConnectionState(p_data->conn.remote_bda);
        LOG_DEBUG("is_connected=%d\n", is_connected);
        BTA_DmBleBroadcast(0); //stop adv
        break;
    case BTA_GATTS_DISCONNECT_EVT:
        //set the connection flag to true
        blufi_cb_env.connected = false;
        break;
    case BTA_GATTS_OPEN_EVT:
        break;
    case BTA_GATTS_CLOSE_EVT:
        if (blufi_cb_env.connected && (blufi_cb_env.conn_id == p_data->conn.conn_id)) {
            //set the connection channal congested flag to true
            blufi_cb_env.congest =  p_data->congest.congested;
        }
        break;
    case BTA_GATTS_LISTEN_EVT:
        break;
    case BTA_GATTS_CONGEST_EVT:
        break;
    default:
        break;
    }
}

static tGATT_STATUS btc_blufi_profile_init(void)
{
    tBT_UUID app_uuid = {LEN_UUID_16, {SVC_BLUFI_UUID}};


    if (blufi_cb_env.enabled) {
        LOG_ERROR("blufi svc already initaliezd");
        return GATT_ERROR;
    } else {
        memset(&blufi_cb_env, 0, sizeof(tBLUFI_CB_ENV));
    }


    /* register the blufi profile to the BTA_GATTS module*/
    BTA_GATTS_AppRegister(&app_uuid, blufi_profile_cb);

    return GATT_SUCCESS;
}

static void blufi_msg_notify(UINT8 *blufi_msg, UINT8 len)
{
    BOOLEAN conn_status = blufi_cb_env.connected;
    UINT16 conn_id = blufi_cb_env.conn_id;
    UINT16 attr_id = blufi_cb_env.blufi_inst.blufi_hdl;
    //notify rsp==false; indicate rsp==true.
    BOOLEAN rsp = false;
    if (!conn_status && blufi_cb_env.congest) {
        LOG_ERROR("the conneciton for blufi profile has been loss");
        return;
    }

    BTA_GATTS_HandleValueIndication (conn_id, attr_id, len,
                                     blufi_msg, rsp);
}

static void btc_blufi_config_success(void)
{
    LOG_DEBUG("config success\n");
    blufi_msg_notify((uint8_t *)success_msg, strlen(success_msg));
}

static void btc_blufi_config_failed(void)
{
    LOG_DEBUG("config faield\n");
    blufi_msg_notify((uint8_t *)failed_msg, strlen(failed_msg));
}

void btc_blufi_cb_handler(btc_msg_t *msg)
{
    esp_blufi_cb_param_t param;

    switch (msg->act) {
    case BTC_BLUFI_CB_ACT_INIT_FINISH:
        param.init_finish.state = ESP_BLUFI_INIT_OK;
        BTC_BLUFI_CB_TO_APP(ESP_BLUFI_EVENT_INIT_FINISH, &param);
        break;
    case BTC_BLUFI_CB_ACT_DEINIT_FINISH:
        /* TODO: but now nothing */
        break;
    case BTC_BLUFI_CB_ACT_RECV_DATA:
        memcpy(&param.recv_data, msg->arg, sizeof(struct blufi_recv_evt_param));

        BTC_BLUFI_CB_TO_APP(ESP_BLUFI_EVENT_RECV_DATA, &param);
        break;
    default:
        LOG_ERROR("%s UNKNOWN %d\n", __func__, msg->act);
        break;
    }
}

void btc_blufi_call_handler(btc_msg_t *msg)
{
    btc_blufi_args_t *arg = (btc_blufi_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLUFI_ACT_INIT:
        btc_blufi_profile_init();
        break;
    case BTC_BLUFI_ACT_DEINIT:
        /* TODO: but now nothing */
        break;
    case BTC_BLUFI_ACT_SEND_CFG_STATE:
        if (arg->cfg_state.state == ESP_BLUFI_CONFIG_OK) {
            btc_blufi_config_success();
        } else {
            btc_blufi_config_failed();
        }
        break;
    default:
        LOG_ERROR("%s UNKNOWN %d\n", __func__, msg->act);
        break;
    }
}

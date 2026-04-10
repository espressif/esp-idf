/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "osi/allocator.h"
#include "btc/btc_task.h"
#include "btc/btc_manage.h"
#include "blufi_int.h"
#include "btc_blufi_prf.h"
#include "esp_log.h"
#include "esp_blufi_api.h"

#include "common/bt_target.h"
#include "common/bt_trace.h"
#include "stack/bt_types.h"
#include "stack/gatt_api.h"
#include "bta/bta_api.h"
#include "bta/bta_gatt_api.h"
#include "bta_gatts_int.h"
#include "btc_gatt_util.h"
#include "btc_gatts.h"

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_err.h"
#include "esp_blufi.h"
#include <esp_gap_ble_api.h>

#if (BLUFI_INCLUDED == TRUE)

static uint8_t server_if;
static uint16_t conn_id;
static uint8_t blufi_service_uuid128[32] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
};

static esp_ble_adv_data_t blufi_adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 16,
    .p_service_uuid = blufi_service_uuid128,
    .flag = 0x6,
};

static esp_ble_adv_params_t blufi_adv_params = {
    .adv_int_min        = 0x100,
    .adv_int_max        = 0x100,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    //.peer_addr            =
    //.peer_addr_type       =
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

#ifdef CONFIG_BT_BLUFI_BLE_SMP_ENABLE
static char *esp_auth_req_to_str(esp_ble_auth_req_t auth_req)
{
   char *auth_str = NULL;
   switch(auth_req) {
    case ESP_LE_AUTH_NO_BOND:
        auth_str = "ESP_LE_AUTH_NO_BOND";
        break;
    case ESP_LE_AUTH_BOND:
        auth_str = "ESP_LE_AUTH_BOND";
        break;
    case ESP_LE_AUTH_REQ_MITM:
        auth_str = "ESP_LE_AUTH_REQ_MITM";
        break;
    case ESP_LE_AUTH_REQ_BOND_MITM:
        auth_str = "ESP_LE_AUTH_REQ_BOND_MITM";
        break;
    case ESP_LE_AUTH_REQ_SC_ONLY:
        auth_str = "ESP_LE_AUTH_REQ_SC_ONLY";
        break;
    case ESP_LE_AUTH_REQ_SC_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_BOND";
        break;
    case ESP_LE_AUTH_REQ_SC_MITM:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM";
        break;
    case ESP_LE_AUTH_REQ_SC_MITM_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM_BOND";
        break;
    default:
        auth_str = "INVALID BLE AUTH REQ";
        break;
   }

   return auth_str;
}

static char *esp_key_type_to_str(esp_ble_key_type_t key_type)
{
   char *key_str = NULL;
   switch(key_type) {
    case ESP_LE_KEY_NONE:
        key_str = "ESP_LE_KEY_NONE";
        break;
    case ESP_LE_KEY_PENC:
        key_str = "ESP_LE_KEY_PENC";
        break;
    case ESP_LE_KEY_PID:
        key_str = "ESP_LE_KEY_PID";
        break;
    case ESP_LE_KEY_PCSRK:
        key_str = "ESP_LE_KEY_PCSRK";
        break;
    case ESP_LE_KEY_PLK:
        key_str = "ESP_LE_KEY_PLK";
        break;
    case ESP_LE_KEY_LLK:
        key_str = "ESP_LE_KEY_LLK";
        break;
    case ESP_LE_KEY_LENC:
        key_str = "ESP_LE_KEY_LENC";
        break;
    case ESP_LE_KEY_LID:
        key_str = "ESP_LE_KEY_LID";
        break;
    case ESP_LE_KEY_LCSRK:
        key_str = "ESP_LE_KEY_LCSRK";
        break;
    default:
        key_str = "INVALID BLE KEY TYPE";
        break;
   }
   return key_str;
}
#endif

void esp_blufi_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    BLUFI_TRACE_DEBUG("GAP_EVT, event %d", event);
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        esp_ble_gap_start_advertising(&blufi_adv_params);
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        //advertising start complete event to indicate advertising start successfully or failed
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            BTC_TRACE_ERROR("Advertising start failed, status %x", param->adv_start_cmpl.status);
            break;
        }
        BLUFI_TRACE_API("Advertising start successfully");
        break;
#ifdef CONFIG_BT_BLUFI_BLE_SMP_ENABLE
    case ESP_GAP_BLE_PASSKEY_REQ_EVT:                           /* passkey request event */
        BLUFI_TRACE_API("Passkey request");
        break;
    case ESP_GAP_BLE_OOB_REQ_EVT: {
        BLUFI_TRACE_API("OOB request");
        uint8_t tk[16] = {1}; //If you paired with OOB, both devices need to use the same tk
        esp_ble_oob_req_reply(param->ble_security.ble_req.bd_addr, tk, sizeof(tk));
        break;
    }
    case ESP_GAP_BLE_LOCAL_IR_EVT:                               /* BLE local IR event */
        BLUFI_TRACE_API("Local identity root");
        break;
    case ESP_GAP_BLE_LOCAL_ER_EVT:                               /* BLE local ER event */
        BLUFI_TRACE_API("Local encryption root");
        break;
    case ESP_GAP_BLE_NC_REQ_EVT:
        /* The app will receive this evt when the IO has DisplayYesNO capability and the peer device IO also has DisplayYesNo capability.
        show the passkey number to the user to confirm it with the number displayed by peer device. */
        esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
        BLUFI_TRACE_WARNING("Numeric Comparison request, passkey %" PRIu32, param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_SEC_REQ_EVT:
        /* send the positive(true) security response to the peer device to accept the security request.
        If not accept the security request, should send the security response with negative(false) accept value*/
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;
    case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:  ///the app will receive this evt when the IO  has Output capability and the peer device IO has Input capability.
        ///show the passkey number to the user to input it in the peer device.
        BLUFI_TRACE_WARNING("Passkey notify, passkey %06" PRIu32, param->ble_security.key_notif.passkey);
        break;
    case ESP_GAP_BLE_KEY_EVT:
        //shows the ble key info share with peer device to the user.
        BLUFI_TRACE_API("Key exchanged, key_type %s", esp_key_type_to_str(param->ble_security.ble_key.key_type));
        break;
    case ESP_GAP_BLE_AUTH_CMPL_EVT: {
        esp_bd_addr_t bd_addr;
        memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
        BLUFI_TRACE_API("Authentication complete, addr_type %u, addr "ESP_BD_ADDR_STR"",
                 param->ble_security.auth_cmpl.addr_type, ESP_BD_ADDR_HEX(bd_addr));
        if(!param->ble_security.auth_cmpl.success) {
            BLUFI_TRACE_WARNING("Pairing failed, reason 0x%x",param->ble_security.auth_cmpl.fail_reason);
        } else {
            BLUFI_TRACE_WARNING("Pairing successfully, auth_mode %s",esp_auth_req_to_str(param->ble_security.auth_cmpl.auth_mode));
        }
        break;
    }
    case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT: {
        BLUFI_TRACE_DEBUG("Bond device remove, status %d, device "ESP_BD_ADDR_STR"",
                 param->remove_bond_dev_cmpl.status, ESP_BD_ADDR_HEX(param->remove_bond_dev_cmpl.bd_addr));
        break;
    }
    case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
        if (param->local_privacy_cmpl.status != ESP_BT_STATUS_SUCCESS){
            BLUFI_TRACE_WARNING("Local privacy config failed, status %x", param->local_privacy_cmpl.status);
        }
        break;
#endif // CONFIG_BT_BLUFI_BLE_SMP_ENABLE
    default:
        break;
    }
}

// static functions declare
static void blufi_profile_cb(tBTA_GATTS_EVT event,  tBTA_GATTS *p_data);

void blufi_create_service(void)
{
    if (!blufi_env.enabled) {
        BTC_TRACE_ERROR("blufi service added error.");
        return;
    }

    blufi_env.srvc_inst = 0x00;
    BTA_GATTS_CreateService(blufi_env.gatt_if, &blufi_srvc_uuid, blufi_env.srvc_inst, BLUFI_HDL_NUM, true);
}

uint8_t esp_blufi_init(void)
{

    /* register the BLUFI profile to the BTA_GATTS module*/
    BTA_GATTS_AppRegister(&blufi_app_uuid, blufi_profile_cb);
    return GATT_SUCCESS;
}

static void blufi_profile_cb(tBTA_GATTS_EVT event, tBTA_GATTS *p_data)
{
    tBTA_GATTS_RSP rsp;
    BLUFI_TRACE_DEBUG("blufi profile cb event = %x\n", event);

    switch (event) {
    case BTA_GATTS_REG_EVT:
        BLUFI_TRACE_DEBUG("REG: status %d, app_uuid %04x, gatt_if %d\n", p_data->reg_oper.status, p_data->reg_oper.uuid.uu.uuid16, p_data->reg_oper.server_if);

        if (p_data->reg_oper.status != BTA_GATT_OK) {
            BLUFI_TRACE_ERROR("BLUFI profile register failed\n");
            return;
        }

        blufi_env.gatt_if = p_data->reg_oper.server_if;
        blufi_env.enabled = true;

        //create the blufi service to the service data base.
        if (p_data->reg_oper.uuid.uu.uuid16 == BLUFI_APP_UUID) {
            BLUFI_TRACE_DEBUG("%s %d\n", __func__, __LINE__);
            blufi_create_service();
        }
        break;
    case BTA_GATTS_DEREG_EVT: {
        esp_blufi_cb_param_t param;
        btc_msg_t msg;

        BLUFI_TRACE_DEBUG("DEREG: status %d, gatt_if %d\n", p_data->reg_oper.status, p_data->reg_oper.server_if);

        if (p_data->reg_oper.status != BTA_GATT_OK) {
            BLUFI_TRACE_ERROR("BLUFI profile unregister failed\n");
            return;
        }

        blufi_env.enabled = false;

        msg.sig = BTC_SIG_API_CB;
        msg.pid = BTC_PID_BLUFI;
        msg.act = ESP_BLUFI_EVENT_DEINIT_FINISH;
        param.deinit_finish.state = ESP_BLUFI_DEINIT_OK;

        btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL, NULL);

        break;
    }
    case BTA_GATTS_READ_EVT:
        memset(&rsp, 0, sizeof(tBTA_GATTS_API_RSP));
        rsp.attr_value.handle = p_data->req_data.p_data->read_req.handle;
        rsp.attr_value.len = 1;
        rsp.attr_value.value[0] = 0x00;
        BTA_GATTS_SendRsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                          p_data->req_data.status, &rsp);
        break;
    case BTA_GATTS_WRITE_EVT: {
        if (p_data->req_data.p_data->write_req.is_prep) {
            tBTA_GATT_STATUS status = GATT_SUCCESS;

            do {
                if (p_data->req_data.p_data->write_req.offset > BLUFI_PREPARE_BUF_MAX_SIZE) {
                   status = ESP_GATT_INVALID_OFFSET;
                   break;
                }

                if ((p_data->req_data.p_data->write_req.offset + p_data->req_data.p_data->write_req.len) > BLUFI_PREPARE_BUF_MAX_SIZE) {
                   status = ESP_GATT_INVALID_ATTR_LEN;
                   break;
                }

                if (blufi_env.prepare_buf == NULL) {
                    if (p_data->req_data.p_data->write_req.offset != 0) {
                        status = GATT_INVALID_OFFSET;
                        break;
                    }
                    blufi_env.prepare_buf = osi_malloc(BLUFI_PREPARE_BUF_MAX_SIZE);
                    blufi_env.prepare_len = 0;
                    if (blufi_env.prepare_buf == NULL) {
                        BLUFI_TRACE_ERROR("Blufi prep no mem\n");
                        status = GATT_NO_RESOURCES;
                        break;
                    }
                }
            } while (0);

            memset(&rsp, 0, sizeof(tGATTS_RSP));
            rsp.attr_value.handle = p_data->req_data.p_data->write_req.handle;
            rsp.attr_value.len = p_data->req_data.p_data->write_req.len;
            rsp.attr_value.offset = p_data->req_data.p_data->write_req.offset;
            memcpy(rsp.attr_value.value, p_data->req_data.p_data->write_req.value, p_data->req_data.p_data->write_req.len);

            BLUFI_TRACE_DEBUG("prep write, len=%d, offset=%d\n", p_data->req_data.p_data->write_req.len, p_data->req_data.p_data->write_req.offset);

            BTA_GATTS_SendRsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                              status, &rsp);

            if (status != GATT_SUCCESS) {
                if (blufi_env.prepare_buf) {
                    osi_free(blufi_env.prepare_buf);
                    blufi_env.prepare_buf = NULL;
                    blufi_env.prepare_len = 0;
                }
                BLUFI_TRACE_ERROR("write data error , error code 0x%x\n", status);
                return;
            }
            memcpy(blufi_env.prepare_buf + p_data->req_data.p_data->write_req.offset,
                   p_data->req_data.p_data->write_req.value,
                   p_data->req_data.p_data->write_req.len);
            blufi_env.prepare_len += p_data->req_data.p_data->write_req.len;

            return;
        } else {
            BLUFI_TRACE_DEBUG("norm write, len=%d, offset=%d\n", p_data->req_data.p_data->write_req.len, p_data->req_data.p_data->write_req.offset);
            BTA_GATTS_SendRsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                              p_data->req_data.status, NULL);
        }

        if (p_data->req_data.p_data->write_req.handle == blufi_env.handle_char_p2e) {
            btc_blufi_recv_handler(&p_data->req_data.p_data->write_req.value[0],
                                   p_data->req_data.p_data->write_req.len);
        }
        break;
    }
    case BTA_GATTS_EXEC_WRITE_EVT:
        BLUFI_TRACE_DEBUG("exec write exec %d\n", p_data->req_data.p_data->exec_write);

        BTA_GATTS_SendRsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                          GATT_SUCCESS, NULL);

        if (blufi_env.prepare_buf && p_data->req_data.p_data->exec_write == GATT_PREP_WRITE_EXEC) {
            btc_blufi_recv_handler(blufi_env.prepare_buf, blufi_env.prepare_len);
        }

        if (blufi_env.prepare_buf) {
            osi_free(blufi_env.prepare_buf);
            blufi_env.prepare_buf = NULL;
            blufi_env.prepare_len = 0;
        }

        break;
    case BTA_GATTS_MTU_EVT:
        BLUFI_TRACE_DEBUG("MTU size %d\n", p_data->req_data.p_data->mtu);
        blufi_env.frag_size = (p_data->req_data.p_data->mtu < BLUFI_MAX_DATA_LEN ? p_data->req_data.p_data->mtu : BLUFI_MAX_DATA_LEN) - BLUFI_MTU_RESERVED_SIZE;
        break;
    case BTA_GATTS_CONF_EVT:
        BLUFI_TRACE_DEBUG("CONFIRM EVT\n");
        /* Nothing */
        break;
    case BTA_GATTS_CREATE_EVT:
        blufi_env.handle_srvc = p_data->create.service_id;
        #if CONFIG_BT_BLUFI_BLE_SMP_ENABLE
        BLUFI_TRACE_WARNING("BLE SMP support in BLUFI is ENABLED!");
        #endif // CONFIG_BT_BLUFI_BLE_SMP_ENABLE
        //add the first blufi characteristic --> write characteristic
        BTA_GATTS_AddCharacteristic(blufi_env.handle_srvc, &blufi_char_uuid_p2e,
                                    #if CONFIG_BT_BLUFI_BLE_SMP_ENABLE
                                    GATT_PERM_WRITE_ENC_MITM,
                                    #else
                                    GATT_PERM_WRITE,
                                    #endif
                                    (GATT_CHAR_PROP_BIT_WRITE),
                                    NULL, NULL);
        break;
    case BTA_GATTS_ADD_CHAR_EVT:
        switch (p_data->add_result.char_uuid.uu.uuid16) {
        case BLUFI_CHAR_P2E_UUID:  /* Phone to ESP32 */
            blufi_env.handle_char_p2e = p_data->add_result.attr_id;

            BTA_GATTS_AddCharacteristic(blufi_env.handle_srvc, &blufi_char_uuid_e2p,
                                        (GATT_PERM_READ),
                                        (GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_NOTIFY),
                                        NULL, NULL);
            break;
        case BLUFI_CHAR_E2P_UUID:  /* ESP32 to Phone */
            blufi_env.handle_char_e2p = p_data->add_result.attr_id;

            BTA_GATTS_AddCharDescriptor (blufi_env.handle_srvc,
                                         (GATT_PERM_READ | GATT_PERM_WRITE),
                                         &blufi_descr_uuid_e2p,
                                         NULL, NULL);
            break;
        default:
            break;
        }
        break;
    case BTA_GATTS_ADD_CHAR_DESCR_EVT: {
        /* call init finish */
        esp_blufi_cb_param_t param;
        btc_msg_t msg;

        blufi_env.handle_descr_e2p = p_data->add_result.attr_id;
        //start the blufi service after created
        BTA_GATTS_StartService(blufi_env.handle_srvc, BTA_GATT_TRANSPORT_LE);

        msg.sig = BTC_SIG_API_CB;
        msg.pid = BTC_PID_BLUFI;
        msg.act = ESP_BLUFI_EVENT_INIT_FINISH;
        param.init_finish.state = ESP_BLUFI_INIT_OK;

        btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL, NULL);
        break;
    }
    case BTA_GATTS_CONNECT_EVT: {
        btc_msg_t msg;
        esp_blufi_cb_param_t param;

        //set the connection flag to true
        BLUFI_TRACE_API("\ndevice is connected "BT_BD_ADDR_STR", server_if=%d,reason=0x%x,connect_id=%d\n",
                        BT_BD_ADDR_HEX(p_data->conn.remote_bda), p_data->conn.server_if,
                        p_data->conn.reason, p_data->conn.conn_id);

        memcpy(blufi_env.remote_bda, p_data->conn.remote_bda, ESP_BLUFI_BD_ADDR_LEN);
        blufi_env.conn_id = p_data->conn.conn_id;
        blufi_env.is_connected = true;
        blufi_env.recv_seq = blufi_env.send_seq = 0;

        msg.sig = BTC_SIG_API_CB;
        msg.pid = BTC_PID_BLUFI;
        msg.act = ESP_BLUFI_EVENT_BLE_CONNECT;
        memcpy(param.connect.remote_bda, p_data->conn.remote_bda, ESP_BLUFI_BD_ADDR_LEN);
        param.connect.conn_id = BTC_GATT_GET_CONN_ID(p_data->conn.conn_id);
        conn_id = param.connect.conn_id;
        server_if = p_data->conn.server_if;
        btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL, NULL);
        break;
    }
    case BTA_GATTS_DISCONNECT_EVT: {
        btc_msg_t msg;
        esp_blufi_cb_param_t param;

        blufi_env.is_connected = false;
        //set the connection flag to true
        BLUFI_TRACE_API("\ndevice is disconnected "BT_BD_ADDR_STR", server_if=%d,reason=0x%x,connect_id=%d\n",
                        BT_BD_ADDR_HEX(p_data->conn.remote_bda), p_data->conn.server_if,
                        p_data->conn.reason, p_data->conn.conn_id);

        memcpy(blufi_env.remote_bda, p_data->conn.remote_bda, ESP_BLUFI_BD_ADDR_LEN);
        blufi_env.conn_id = p_data->conn.conn_id;
        blufi_env.recv_seq = blufi_env.send_seq = 0;
        blufi_env.sec_mode = 0x0;
        blufi_env.offset = 0;

        if (blufi_env.aggr_buf != NULL) {
            osi_free(blufi_env.aggr_buf);
            blufi_env.aggr_buf = NULL;
        }

        msg.sig = BTC_SIG_API_CB;
        msg.pid = BTC_PID_BLUFI;
        msg.act = ESP_BLUFI_EVENT_BLE_DISCONNECT;
        memcpy(param.disconnect.remote_bda, p_data->conn.remote_bda, ESP_BLUFI_BD_ADDR_LEN);
        btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL, NULL);
        break;
    }
    case BTA_GATTS_OPEN_EVT:
        break;
    case BTA_GATTS_CLOSE_EVT:
        break;
    case BTA_GATTS_CONGEST_EVT:
        break;
    default:
        break;
    }
}

void esp_blufi_send_notify(void *arg)
{
    struct pkt_info *pkts = (struct pkt_info *) arg;
    uint16_t conn_id = blufi_env.conn_id;
    uint16_t attr_id = blufi_env.handle_char_e2p;
    bool rsp = false;
    BTA_GATTS_HandleValueIndication(conn_id, attr_id, pkts->pkt_len,
                                    pkts->pkt, rsp);

}

void esp_blufi_deinit(void)
{
    BTA_GATTS_StopService(blufi_env.handle_srvc);
    BTA_GATTS_DeleteService(blufi_env.handle_srvc);
    /* register the BLUFI profile to the BTA_GATTS module*/
    BTA_GATTS_AppDeregister(blufi_env.gatt_if);
}

void esp_blufi_adv_start(void)
{
    esp_ble_gap_config_adv_data(&blufi_adv_data);
}

void esp_blufi_adv_start_with_name(const char *name)
{
    if (name != NULL) {
        esp_ble_gap_set_device_name(name);
    }

    esp_ble_gap_config_adv_data(&blufi_adv_data);
}

void esp_blufi_adv_stop(void)
{
    esp_ble_gap_stop_advertising();
}


esp_err_t esp_blufi_start_security_request(esp_blufi_bd_addr_t remote_bda)
{
    #ifdef CONFIG_BT_BLUFI_BLE_SMP_ENABLE
    return esp_ble_set_encryption(remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);
    #else
    return ESP_ERR_INVALID_STATE;
    #endif // CONFIG_BT_BLUFI_BLE_SMP_ENABLE
}

void esp_blufi_send_encap(void *arg)
{
    struct blufi_hdr *hdr = (struct blufi_hdr *)arg;
retry:
    if (blufi_env.is_connected == false) {
        BTC_TRACE_WARNING("%s ble connection is broken\n", __func__);
        return;
    }
    if (esp_ble_get_cur_sendable_packets_num(BTC_GATT_GET_CONN_ID(blufi_env.conn_id)) > 0) {
        btc_blufi_send_notify((uint8_t *)hdr,
                              ((hdr->fc & BLUFI_FC_CHECK) ?
                               hdr->data_len + sizeof(struct blufi_hdr) + 2 :
                               hdr->data_len + sizeof(struct blufi_hdr)));
    } else {
        BTC_TRACE_WARNING("%s wait to send blufi custom data\n", __func__);
        vTaskDelay(pdMS_TO_TICKS(10));
        goto retry;
    }
}

esp_err_t esp_blufi_close(esp_gatt_if_t gatts_if, uint16_t conn_id)
{
    ESP_BLE_HOST_STATUS_CHECK(ESP_BLE_HOST_STATUS_ENABLED);
    btc_msg_t msg;
    btc_ble_gatts_args_t arg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATTS;
    msg.act = BTC_GATTS_ACT_CLOSE;
    arg.close.conn_id = BTC_GATT_CREATE_CONN_ID(gatts_if, conn_id);
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatts_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

void  esp_blufi_disconnect()
{
    int rc;
    rc = esp_blufi_close(server_if, conn_id);
    assert (rc == 0);
}

#endif

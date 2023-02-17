/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
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

void esp_blufi_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        esp_ble_gap_start_advertising(&blufi_adv_params);
        break;
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

            if (blufi_env.prepare_buf == NULL) {
                blufi_env.prepare_buf = osi_malloc(BLUFI_PREPAIR_BUF_MAX_SIZE);
                blufi_env.prepare_len = 0;
                if (blufi_env.prepare_buf == NULL) {
                    BLUFI_TRACE_ERROR("Blufi prep no mem\n");
                    status = GATT_NO_RESOURCES;
                }
            } else {
                if (p_data->req_data.p_data->write_req.offset > BLUFI_PREPAIR_BUF_MAX_SIZE) {
                    status = GATT_INVALID_OFFSET;
                } else if ((p_data->req_data.p_data->write_req.offset + p_data->req_data.p_data->write_req.len) > BLUFI_PREPAIR_BUF_MAX_SIZE) {
                    status = GATT_INVALID_ATTR_LEN;
                }
            }

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

        //add the frist blufi characteristic --> write characteristic
        BTA_GATTS_AddCharacteristic(blufi_env.handle_srvc, &blufi_char_uuid_p2e,
                                    (GATT_PERM_WRITE),
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
    esp_ble_gap_set_device_name(BLUFI_DEVICE_NAME);
    esp_ble_gap_config_adv_data(&blufi_adv_data);
}

void esp_blufi_adv_stop(void)
{
    esp_ble_gap_stop_advertising();
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

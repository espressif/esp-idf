// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>

#include "bta_gatt_api.h"

#include "btc_task.h"
#include "btc_manage.h"
#include "btc_gatts.h"
#include "btc_gatt_util.h"

#include "esp_gatts_api.h"

#define BTC_GATTS_CB_TO_APP(_event, _param) ((esp_profile_cb_t)btc_profile_cb_get(BTC_PID_GATTS))(_event, _param)

#define A2C_GATTS_EVT(_bta_event) (_bta_event) //BTA TO BTC EVT
#define C2A_GATTS_EVT(_btc_event) (_btc_event) //BTC TO BTA EVT

void btc_gatts_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_gatts_args_t *dst = (btc_ble_gatts_args_t *) p_dest;
    btc_ble_gatts_args_t *src = (btc_ble_gatts_args_t *)p_src;

    switch (msg->act) {
    case BTC_GATTS_ACT_SEND_INDICATE: {
        dst->send_ind.value = (uint8_t *)GKI_getbuf(src->send_ind.value_len);
        if (dst->send_ind.value) {
            memcpy(dst->send_ind.value, src->send_ind.value, src->send_ind.value_len);
        } else {
            LOG_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        break;
    }
    case BTC_GATTS_ACT_SEND_RESPONSE: {
        if (src->send_rsp.rsp) {
            dst->send_rsp.rsp = (esp_gatt_rsp_t *)GKI_getbuf(sizeof(esp_gatt_rsp_t));
            if (dst->send_rsp.rsp) {
                memcpy(dst->send_rsp.rsp, src->send_rsp.rsp, sizeof(esp_gatt_rsp_t));
            } else {
                LOG_ERROR("%s %d no mem\n", __func__, msg->act);
            }
        }
        break;
    }
    default:
        LOG_DEBUG("%s Unhandled deep copy %d\n", __func__, msg->act);
        break;
    }

}

void btc_gatts_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_gatts_args_t *arg = (btc_ble_gatts_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_GATTS_ACT_SEND_INDICATE: {
        if (arg->send_ind.value) {
            GKI_freebuf(arg->send_ind.value);
        }
        break;
    }
    case BTC_GATTS_ACT_SEND_RESPONSE: {
        if (arg->send_rsp.rsp) {
            GKI_freebuf(arg->send_rsp.rsp);
        }
        break;
    }
    default:
        LOG_DEBUG("%s Unhandled deep free %d\n", __func__, msg->act);
        break;
    }

}


static void btc_gatts_cb_param_copy_req(btc_msg_t *msg, void *p_dest, void *p_src)
{
    uint16_t event = msg->act;

    tBTA_GATTS *p_dest_data = (tBTA_GATTS *) p_dest;
    tBTA_GATTS *p_src_data = (tBTA_GATTS *) p_src;

    if (!p_src_data || !p_dest_data) {
        return;
    }

    // Copy basic structure first
    memcpy(p_dest_data, p_src_data, sizeof(tBTA_GATTS));

    // Allocate buffer for request data if necessary
    switch (event) {
    case BTA_GATTS_READ_EVT:
    case BTA_GATTS_WRITE_EVT:
    case BTA_GATTS_EXEC_WRITE_EVT:
    case BTA_GATTS_MTU_EVT:
        p_dest_data->req_data.p_data = GKI_getbuf(sizeof(tBTA_GATTS_REQ_DATA));
        if (p_dest_data->req_data.p_data != NULL) {
            memcpy(p_dest_data->req_data.p_data, p_src_data->req_data.p_data,
                   sizeof(tBTA_GATTS_REQ_DATA));
        }
        break;

    default:
        break;
    }
}

static void btc_gatts_cb_param_copy_free(btc_msg_t *msg, tBTA_GATTS *p_data)
{
    uint16_t event = msg->act;

    switch (event) {
    case BTA_GATTS_READ_EVT:
    case BTA_GATTS_WRITE_EVT:
    case BTA_GATTS_EXEC_WRITE_EVT:
    case BTA_GATTS_MTU_EVT:
        if (p_data && p_data->req_data.p_data) {
            GKI_freebuf(p_data->req_data.p_data);
        }
        break;

    default:
        break;
    }
}


static void btc_gatts_inter_cb(tBTA_GATTS_EVT event, tBTA_GATTS *p_data)
{
    bt_status_t status;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GATTS;
    msg.act = event;

    status = btc_transfer_context(&msg, p_data,
                                  sizeof(tBTA_GATTS), btc_gatts_cb_param_copy_req);

    if (status != BT_STATUS_SUCCESS) {
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

void btc_gatts_call_handler(btc_msg_t *msg)
{
    btc_ble_gatts_args_t *arg = (btc_ble_gatts_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_GATTS_ACT_APP_REGISTER: {
        tBT_UUID uuid;

        uuid.len = LEN_UUID_16;
        uuid.uu.uuid16 = arg->app_reg.app_id;

        BTA_GATTS_AppRegister(&uuid, btc_gatts_inter_cb);

        break;
    }
    case BTC_GATTS_ACT_APP_UNREGISTER:
        BTA_GATTS_AppDeregister(arg->app_unreg.gatt_if);
        break;
    case BTC_GATTS_ACT_CREATE_SERVICE: {
        tBTA_GATT_SRVC_ID srvc_id;
        btc_to_bta_srvc_id(&srvc_id, &arg->create_srvc.service_id);
        BTA_GATTS_CreateService(arg->create_srvc.gatt_if, &srvc_id.id.uuid,
                                srvc_id.id.inst_id, arg->create_srvc.num_handle,
                                srvc_id.is_primary);
        break;
    }
    case BTC_GATTS_ACT_DELETE_SERVICE:
        BTA_GATTS_DeleteService(arg->delete_srvc.service_handle);
        break;
    case BTC_GATTS_ACT_START_SERVICE:
        BTA_GATTS_StartService(arg->start_srvc.service_handle, BTA_GATT_TRANSPORT_LE);
        break;
    case BTC_GATTS_ACT_STOP_SERVICE:
        BTA_GATTS_StopService(arg->stop_srvc.service_handle);
        break;
    case BTC_GATTS_ACT_ADD_INCLUDE_SERVICE:
        BTA_GATTS_AddIncludeService(arg->add_incl_srvc.service_handle, arg->add_incl_srvc.included_service_handle);
        break;
    case BTC_GATTS_ACT_ADD_CHAR: {
        tBT_UUID uuid;
        btc_to_bta_uuid(&uuid, &arg->add_char.char_uuid);

        BTA_GATTS_AddCharacteristic(arg->add_char.service_handle, &uuid,
                                    arg->add_char.perm, arg->add_char.property);
        break;
    }
    case BTC_GATTS_ACT_ADD_CHAR_DESCR: {
        tBT_UUID uuid;
        btc_to_bta_uuid(&uuid, &arg->add_descr.descr_uuid);
        BTA_GATTS_AddCharDescriptor(arg->add_descr.service_handle, arg->add_descr.perm, &uuid);
        break;
    }
    case BTC_GATTS_ACT_SEND_INDICATE:
        BTA_GATTS_HandleValueIndication(arg->send_ind.conn_id, arg->send_ind.attr_handle,
                                        arg->send_ind.value_len, arg->send_ind.value, arg->send_ind.need_confirm);
        break;
    case BTC_GATTS_ACT_SEND_RESPONSE: {
        esp_ble_gatts_cb_param_t param;
        esp_gatt_rsp_t *p_rsp = arg->send_rsp.rsp;

        if (p_rsp) {
            tBTA_GATTS_RSP rsp_struct;
            btc_to_bta_response(&rsp_struct, p_rsp);
            BTA_GATTS_SendRsp(arg->send_rsp.conn_id, arg->send_rsp.trans_id,
                              arg->send_rsp.status, &rsp_struct);
            param.rsp.handle = rsp_struct.attr_value.handle;
        } else {
            BTA_GATTS_SendRsp(arg->send_rsp.conn_id, arg->send_rsp.trans_id,
                              arg->send_rsp.status, NULL);
        }

        param.rsp.status = 0;
        BTC_GATTS_CB_TO_APP(ESP_GATTS_RESPONSE_EVT, &param);
        break;
    }
    case BTC_GATTS_ACT_OPEN: {
        // Ensure device is in inquiry database
        tBTA_GATT_TRANSPORT transport = BTA_GATT_TRANSPORT_LE;

        //TODO : implement address type and device type
#if  0
        if (_get_address_type(arg->remote_bda, &addr_type) &&
                btif_get_device_type(arg->remote_bda, &device_type) &&
                device_type != BT_DEVICE_TYPE_BREDR) {
            BTA_DmAddBleDevice(p_cb->bd_addr.address, addr_type, device_type);
        }
#else
        //BTA_DmAddBleDevice(p_cb->bd_addr.address, addr_type, device_type);
#endif
        // Mark background connections
        if (!arg->open.is_direct) {
            BTA_DmBleSetBgConnType(BTM_BLE_CONN_AUTO, NULL);
        }

        transport = BTA_GATT_TRANSPORT_LE;

        // Connect!
        BTA_GATTS_Open(arg->open.gatt_if, arg->open.remote_bda,
                       arg->open.is_direct, transport);
        break;
    }
    case BTC_GATTS_ACT_CLOSE:
        // TODO : implement cancel open
        // Cancel pending foreground/background connections
        //BTA_GATTS_CancelOpen(p_cb->server_if, p_cb->bd_addr.address, TRUE);
        //BTA_GATTS_CancelOpen(p_cb->server_if, p_cb->bd_addr.address, FALSE);

        // Close active connection
        if (arg->close.conn_id != 0) {
            BTA_GATTS_Close(arg->close.conn_id);
        }

        break;
    default:
        break;
    }
    btc_gatts_arg_deep_free(msg);
}

void btc_gatts_cb_handler(btc_msg_t *msg)
{
    esp_ble_gatts_cb_param_t param;

    tBTA_GATTS *p_data = (tBTA_GATTS *)msg->arg;

    switch (msg->act) {
    case BTA_GATTS_REG_EVT: {
        param.reg.status = p_data->reg_oper.status;
        param.reg.gatt_if = p_data->reg_oper.server_if;
        param.reg.app_id = p_data->reg_oper.uuid.uu.uuid16;

        BTC_GATTS_CB_TO_APP(ESP_GATTS_REG_EVT, &param);
        break;
    }
    case BTA_GATTS_DEREG_EVT:
        // do nothing
        break;
    case BTA_GATTS_READ_EVT: {
        param.read.conn_id = p_data->req_data.conn_id;
        param.read.trans_id = p_data->req_data.trans_id;
        memcpy(param.read.bda, p_data->req_data.remote_bda, ESP_BD_ADDR_LEN);
        param.read.handle = p_data->req_data.p_data->read_req.handle,
                   param.read.offset = p_data->req_data.p_data->read_req.offset,
                              param.read.is_long = p_data->req_data.p_data->read_req.is_long,

                                         BTC_GATTS_CB_TO_APP(ESP_GATTS_READ_EVT, &param);
        break;
    }
    case BTA_GATTS_WRITE_EVT: {
        param.write.conn_id = p_data->req_data.conn_id;
        param.write.trans_id = p_data->req_data.trans_id;
        memcpy(param.write.bda, p_data->req_data.remote_bda, ESP_BD_ADDR_LEN);
        param.write.handle = p_data->req_data.p_data->write_req.handle;
        param.write.offset = p_data->req_data.p_data->write_req.offset;
        param.write.need_rsp = p_data->req_data.p_data->write_req.need_rsp;
        param.write.is_prep = p_data->req_data.p_data->write_req.is_prep;
        param.write.len = p_data->req_data.p_data->write_req.len;
        param.write.value = p_data->req_data.p_data->write_req.value;

        BTC_GATTS_CB_TO_APP(ESP_GATTS_WRITE_EVT, &param);

        break;
    }
    case BTA_GATTS_EXEC_WRITE_EVT: {
        param.exec_write.conn_id = p_data->req_data.conn_id;
        param.exec_write.trans_id = p_data->req_data.trans_id;
        memcpy(param.exec_write.bda, p_data->req_data.remote_bda, ESP_BD_ADDR_LEN);
        param.exec_write.exec_write_flag = p_data->req_data.p_data->exec_write;

        BTC_GATTS_CB_TO_APP(ESP_GATTS_EXEC_WRITE_EVT, &param);

        break;
    }
    case BTA_GATTS_MTU_EVT:
        param.mtu.conn_id = p_data->req_data.conn_id;
        param.mtu.mtu = p_data->req_data.p_data->mtu;

        BTC_GATTS_CB_TO_APP(ESP_GATTS_MTU_EVT, &param);
        break;
    case BTA_GATTS_CONF_EVT:
        param.conf.conn_id = p_data->req_data.conn_id;
        param.conf.status = p_data->req_data.status;

        BTC_GATTS_CB_TO_APP(ESP_GATTS_CONF_EVT, &param);
        break;
    case BTA_GATTS_CREATE_EVT:
        param.create.status = p_data->create.status;
        param.create.gatt_if = p_data->create.server_if;
        param.create.service_handle = p_data->create.service_id;
        param.create.service_id.is_primary = p_data->create.is_primary;
        param.create.service_id.id.inst_id = p_data->create.svc_instance;
        bta_to_btc_uuid(&param.create.service_id.id.uuid, &p_data->create.uuid);
        BTC_GATTS_CB_TO_APP(ESP_GATTS_CREATE_EVT, &param);
        break;
    case BTA_GATTS_ADD_INCL_SRVC_EVT:
        param.add_incl_srvc.status = p_data->add_result.status;
        param.add_incl_srvc.gatt_if = p_data->add_result.server_if;
        param.add_incl_srvc.attr_handle = p_data->add_result.attr_id;
        param.add_incl_srvc.service_handle = p_data->add_result.service_id;

        BTC_GATTS_CB_TO_APP(ESP_GATTS_ADD_INCL_SRVC_EVT, &param);
        break;
    case BTA_GATTS_ADD_CHAR_EVT:
        param.add_char.status = p_data->add_result.status;
        param.add_char.gatt_if = p_data->add_result.server_if;
        param.add_char.attr_handle = p_data->add_result.attr_id;
        param.add_char.service_handle = p_data->add_result.service_id;
        bta_to_btc_uuid(&param.add_char.char_uuid, &p_data->add_result.char_uuid);

        BTC_GATTS_CB_TO_APP(ESP_GATTS_ADD_CHAR_EVT, &param);
        break;
    case BTA_GATTS_ADD_CHAR_DESCR_EVT:
        param.add_char_descr.status = p_data->add_result.status;
        param.add_char_descr.gatt_if = p_data->add_result.server_if;
        param.add_char_descr.attr_handle = p_data->add_result.attr_id;
        param.add_char_descr.service_handle = p_data->add_result.service_id;
        bta_to_btc_uuid(&param.add_char_descr.char_uuid, &p_data->add_result.char_uuid);

        BTC_GATTS_CB_TO_APP(ESP_GATTS_ADD_CHAR_DESCR_EVT, &param);
        break;
    case BTA_GATTS_DELELTE_EVT:
        param.del.status = p_data->srvc_oper.status;
        param.del.gatt_if = p_data->srvc_oper.server_if;
        param.del.service_handle = p_data->srvc_oper.service_id;

        BTC_GATTS_CB_TO_APP(ESP_GATTS_DELETE_EVT, &param);
        break;
    case BTA_GATTS_START_EVT:
        param.start.status = p_data->srvc_oper.status;
        param.start.gatt_if = p_data->srvc_oper.server_if;
        param.start.service_handle = p_data->srvc_oper.service_id;

        BTC_GATTS_CB_TO_APP(ESP_GATTS_START_EVT, &param);
        break;
    case BTA_GATTS_STOP_EVT:
        param.stop.status = p_data->srvc_oper.status;
        param.stop.gatt_if = p_data->srvc_oper.server_if;
        param.stop.service_handle = p_data->srvc_oper.service_id;

        BTC_GATTS_CB_TO_APP(ESP_GATTS_STOP_EVT, &param);
        break;
    case BTA_GATTS_CONNECT_EVT:
        param.connect.conn_id = p_data->conn.conn_id;
        param.connect.gatt_if = p_data->conn.server_if;
        param.connect.is_connected = true;
        memcpy(param.connect.remote_bda, p_data->conn.remote_bda, ESP_BD_ADDR_LEN);

        BTC_GATTS_CB_TO_APP(ESP_GATTS_CONNECT_EVT, &param);
        break;
    case BTA_GATTS_DISCONNECT_EVT:
        param.connect.conn_id = p_data->conn.conn_id;
        param.connect.gatt_if = p_data->conn.server_if;
        param.connect.is_connected = false;
        memcpy(param.connect.remote_bda, p_data->conn.remote_bda, ESP_BD_ADDR_LEN);

        BTC_GATTS_CB_TO_APP(ESP_GATTS_DISCONNECT_EVT, &param);
        break;
    case BTA_GATTS_OPEN_EVT:
    // do nothing
    case BTA_GATTS_CANCEL_OPEN_EVT:
    // do nothing
    case BTA_GATTS_CLOSE_EVT:
    // do nothing
    case BTA_GATTS_LISTEN_EVT:
        // do nothing
        break;
    case BTA_GATTS_CONGEST_EVT:
        param.congest.conn_id = p_data->congest.conn_id;
        param.congest.congested = p_data->congest.congested;
        break;
    default:
        // do nothing
        break;
    }

    btc_gatts_cb_param_copy_free(msg, p_data);
}

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

#include "blufi_int.h"

#include "esp_blufi_api.h"

#define BT_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define BT_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

//define the blufi serivce uuid
#define BLUFI_SERVICE_UUID  0xFFFF
//define the blufi Char uuid (PHONE to ESP32)
#define BLUFI_CHAR_P2E_UUID 0xFF01
//define the blufi Char uuid (ESP32 to PHONE)
#define BLUFI_CHAR_E2P_UUID 0xFF02
//define the blufi Descriptor uuid (ESP32 to PHONE)
#define BLUFI_DESCR_E2P_UUID GATT_UUID_CHAR_CLIENT_CONFIG
//define the blufi APP ID
#define BLUFI_APP_UUID      0xFFFF

#define BLUFI_HDL_NUM   6

tBLUFI_ENV blufi_env;

static /* const */ tBT_UUID blufi_srvc_uuid = {LEN_UUID_16, {BLUFI_SERVICE_UUID}};
static /* const */ tBT_UUID blufi_char_uuid_p2e = {LEN_UUID_16, {BLUFI_CHAR_P2E_UUID}};
static /* const */ tBT_UUID blufi_char_uuid_e2p = {LEN_UUID_16, {BLUFI_CHAR_E2P_UUID}};
static /* const */ tBT_UUID blufi_descr_uuid_e2p = {LEN_UUID_16, {BLUFI_DESCR_E2P_UUID}};
static /* const */ tBT_UUID blufi_app_uuid = {LEN_UUID_16, {BLUFI_APP_UUID}};

// static functions declare
static void blufi_profile_cb(tBTA_GATTS_EVT event,  tBTA_GATTS *p_data);
static void btc_blufi_recv_handler(uint8_t *data, int len);
static void btc_blufi_send_ack(uint8_t seq);

static inline void btc_blufi_cb_to_app(esp_blufi_cb_event_t event, esp_blufi_cb_param_t *param)
{
    esp_blufi_event_cb_t btc_blufi_cb = (esp_blufi_event_cb_t)btc_profile_cb_get(BTC_PID_BLUFI);
    if (btc_blufi_cb) {
	btc_blufi_cb(event, param);
    }
}

static void blufi_create_service(void)
{
    if (!blufi_env.enabled) {
        LOG_ERROR("blufi service added error.");
        return;
    }

    blufi_env.srvc_inst = 0x00;
    BTA_GATTS_CreateService(blufi_env.gatt_if, &blufi_srvc_uuid, blufi_env.srvc_inst, BLUFI_HDL_NUM, true);
}

static void blufi_profile_cb(tBTA_GATTS_EVT event, tBTA_GATTS *p_data)
{
    tBTA_GATTS_RSP rsp;

    LOG_DEBUG("blufi profile cb event = %x\n", event);

    switch (event) {
    case BTA_GATTS_REG_EVT:
        LOG_DEBUG("REG: status %d, app_uuid %04x, gatt_if %d\n", p_data->reg_oper.status, p_data->reg_oper.uuid.uu.uuid16, p_data->reg_oper.server_if);

        if (p_data->reg_oper.status != BTA_GATT_OK) {
            LOG_ERROR("BLUFI profile register failed\n");
            return;
        }

        blufi_env.gatt_if = p_data->reg_oper.server_if;
        blufi_env.enabled = true;

        //create the blufi service to the service data base.
        if (p_data->reg_oper.uuid.uu.uuid16 == BLUFI_APP_UUID) {
            LOG_DEBUG("%s %d\n", __func__, __LINE__);
            blufi_create_service();
        }
        break;
    case BTA_GATTS_READ_EVT:
        memset(&rsp, 0, sizeof(tBTA_GATTS_API_RSP));
        rsp.attr_value.handle = p_data->req_data.p_data->read_req.handle;
        rsp.attr_value.len = 1;
        rsp.attr_value.value[0] = 0x00;
        BTA_GATTS_SendRsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                          p_data->req_data.status, &rsp);
        break;
    case BTA_GATTS_WRITE_EVT: {
        if(p_data->req_data.p_data->write_req.is_prep) {
            tBTA_GATT_STATUS status = GATT_SUCCESS;

            if (blufi_env.prepare_buf == NULL) {
                blufi_env.prepare_buf = GKI_getbuf(BLUFI_PREPAIR_BUF_MAX_SIZE);
                if (blufi_env.prepare_buf == NULL) {
                    LOG_ERROR("Blufi prep no mem\n");
                    status = GATT_NO_RESOURCES;
                }
            } else {
                if(p_data->req_data.p_data->write_req.offset > BLUFI_PREPAIR_BUF_MAX_SIZE) {
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

            LOG_DEBUG("prep write, len=%d, offset=%d\n", p_data->req_data.p_data->write_req.len, p_data->req_data.p_data->write_req.offset);

            BTA_GATTS_SendRsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                          status, &rsp);

            memcpy(blufi_env.prepare_buf + p_data->req_data.p_data->write_req.offset,
                   p_data->req_data.p_data->write_req.value,
                   p_data->req_data.p_data->write_req.len);
            blufi_env.prepare_len += p_data->req_data.p_data->write_req.len;

            return;
        } else {
            LOG_DEBUG("norm write, len=%d, offset=%d\n", p_data->req_data.p_data->write_req.len, p_data->req_data.p_data->write_req.offset);
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
        LOG_DEBUG("exec write exec %d\n", p_data->req_data.p_data->exec_write);

        BTA_GATTS_SendRsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                    GATT_SUCCESS, NULL);

        if (p_data->req_data.p_data->exec_write == GATT_PREP_WRITE_EXEC) {
            btc_blufi_recv_handler(blufi_env.prepare_buf, blufi_env.prepare_len);
        }

        if (blufi_env.prepare_buf) {
            GKI_freebuf(blufi_env.prepare_buf);
            blufi_env.prepare_buf = NULL;
        }

        break;
    case BTA_GATTS_MTU_EVT:
        LOG_DEBUG("MTU size %d\n", p_data->req_data.p_data->mtu);
        blufi_env.frag_size = p_data->req_data.p_data->mtu - BLUFI_MTU_RESERVED_SIZE;
        break;
    case BTA_GATTS_CONF_EVT:
        LOG_DEBUG("CONIRM EVT\n");
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

        btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL);
        break;
    }
    case BTA_GATTS_CONNECT_EVT: {
        btc_msg_t msg;
        esp_blufi_cb_param_t param;

        //set the connection flag to true
        LOG_INFO("\ndevice is connected "BT_BD_ADDR_STR", server_if=%d,reason=0x%x,connect_id=%d\n",
                  BT_BD_ADDR_HEX(p_data->conn.remote_bda), p_data->conn.server_if,
                  p_data->conn.reason, p_data->conn.conn_id);

        memcpy(blufi_env.remote_bda, p_data->conn.remote_bda, sizeof(esp_bd_addr_t));
        blufi_env.conn_id = p_data->conn.conn_id;
        blufi_env.is_connected = true;

        msg.sig = BTC_SIG_API_CB;
        msg.pid = BTC_PID_BLUFI;
        msg.act = ESP_BLUFI_EVENT_BLE_CONNECT;
        memcpy(param.connect.remote_bda, p_data->conn.remote_bda, sizeof(esp_bd_addr_t));
        btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL);
        break;
    }
    case BTA_GATTS_DISCONNECT_EVT: {
        btc_msg_t msg;
        esp_blufi_cb_param_t param;

        blufi_env.is_connected = false;
        //set the connection flag to true
        LOG_INFO("\ndevice is disconnected "BT_BD_ADDR_STR", server_if=%d,reason=0x%x,connect_id=%d\n",
                  BT_BD_ADDR_HEX(p_data->conn.remote_bda), p_data->conn.server_if,
                  p_data->conn.reason, p_data->conn.conn_id);

        memcpy(blufi_env.remote_bda, p_data->conn.remote_bda, sizeof(esp_bd_addr_t));
        blufi_env.conn_id = p_data->conn.conn_id;
        blufi_env.is_connected = false;
        blufi_env.recv_seq = blufi_env.send_seq = 0;
        blufi_env.sec_mode = 0x0;

        msg.sig = BTC_SIG_API_CB;
        msg.pid = BTC_PID_BLUFI;
        msg.act = ESP_BLUFI_EVENT_BLE_DISCONNECT;
        memcpy(param.disconnect.remote_bda, p_data->conn.remote_bda, sizeof(esp_bd_addr_t));
        btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL);
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

static tGATT_STATUS btc_blufi_profile_init(void)
{
    esp_blufi_callbacks_t *store_p = blufi_env.cbs;

    if (blufi_env.enabled) {
        LOG_ERROR("BLUFI already initialized");
        return GATT_ERROR;
    }

    memset(&blufi_env, 0x0, sizeof(blufi_env));
    blufi_env.cbs = store_p;        /* if set callback prior, restore the point */
    blufi_env.frag_size = BLUFI_FRAG_DATA_DEFAULT_LEN;

    /* register the BLUFI profile to the BTA_GATTS module*/
    BTA_GATTS_AppRegister(&blufi_app_uuid, blufi_profile_cb);

    return GATT_SUCCESS;
}

static tGATT_STATUS btc_blufi_profile_deinit(void)
{
    esp_blufi_cb_param_t param;
    btc_msg_t msg;

    if (!blufi_env.enabled) {
        LOG_ERROR("BLUFI already initialized");
        return GATT_ERROR;
    }

    BTA_GATTS_StopService(blufi_env.handle_srvc);
    BTA_GATTS_DeleteService(blufi_env.handle_srvc);
    /* register the BLUFI profile to the BTA_GATTS module*/
    BTA_GATTS_AppDeregister(blufi_env.gatt_if);

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_BLUFI;
    msg.act = ESP_BLUFI_EVENT_DEINIT_FINISH;
    param.deinit_finish.state = ESP_BLUFI_DEINIT_OK;

    btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL);

    return GATT_SUCCESS;
}

static void btc_blufi_send_notify(uint8_t *pkt, int pkt_len)
{
    UINT16 conn_id = blufi_env.conn_id;
    UINT16 attr_id = blufi_env.handle_char_e2p;
    bool rsp = false;
    
    BTA_GATTS_HandleValueIndication(conn_id, attr_id, pkt_len,
                                     pkt, rsp);
}

static void btc_blufi_recv_handler(uint8_t *data, int len)
{
    struct blufi_hdr *hdr = (struct blufi_hdr *)data;
    uint16_t checksum, checksum_pkt;
    int ret;

    if (hdr->seq != blufi_env.recv_seq) {
        LOG_ERROR("%s seq %d is not expect %d\n", __func__, hdr->seq, blufi_env.recv_seq + 1);
        return;
    }

    blufi_env.recv_seq++;

    // first step, decrypt
    if (BLUFI_FC_IS_ENC(hdr->fc)
            && (blufi_env.cbs && blufi_env.cbs->decrypt_func)) {
        ret = blufi_env.cbs->decrypt_func(hdr->seq, hdr->data, hdr->data_len);
        if (ret != hdr->data_len) { /* enc must be success and enc len must equal to plain len */
            LOG_ERROR("%s decrypt error %d\n", __func__, ret);
            return;
        }
    }

    // second step, check sum
    if (BLUFI_FC_IS_CHECK(hdr->fc)
            && (blufi_env.cbs && blufi_env.cbs->checksum_func)) {
        checksum = blufi_env.cbs->checksum_func(hdr->seq, &hdr->seq, hdr->data_len + 2);
        checksum_pkt = hdr->data[hdr->data_len] | (((uint16_t) hdr->data[hdr->data_len + 1]) << 8);
        if (checksum != checksum_pkt) {
            LOG_ERROR("%s checksum error %04x, pkt %04x\n", __func__, checksum, checksum_pkt);
            return;
        }
    }

    if (BLUFI_FC_IS_REQ_ACK(hdr->fc)) {
        btc_blufi_send_ack(hdr->seq);
    }

    if (BLUFI_FC_IS_FRAG(hdr->fc)) {
        if (blufi_env.offset == 0) {
            blufi_env.total_len = hdr->data[0] | (((uint16_t) hdr->data[1]) << 8);
            blufi_env.aggr_buf = GKI_getbuf(blufi_env.total_len);
            if (blufi_env.aggr_buf == NULL) {
                LOG_ERROR("%s no mem, len %d\n", __func__, blufi_env.total_len);
                return;
            }
        }
        memcpy(blufi_env.aggr_buf + blufi_env.offset, hdr->data + 2, hdr->data_len  - 2);
        blufi_env.offset += (hdr->data_len - 2);
    } else {
        if (blufi_env.offset > 0) {   /* if previous pkt is frag */
            memcpy(blufi_env.aggr_buf + blufi_env.offset, hdr->data, hdr->data_len);

            btc_blufi_protocol_handler(hdr->type, blufi_env.aggr_buf, blufi_env.total_len);
            blufi_env.offset = 0;
            GKI_freebuf(blufi_env.aggr_buf);
            blufi_env.aggr_buf = NULL;
        } else {
            btc_blufi_protocol_handler(hdr->type, hdr->data, hdr->data_len);
            blufi_env.offset = 0;
        }
    }
}
void btc_blufi_send_encap(uint8_t type, uint8_t *data, int total_data_len)
{
    struct blufi_hdr *hdr = NULL;
    int remain_len = total_data_len;
    uint16_t checksum;
    int ret;

    while (remain_len > 0) {
        if (remain_len > blufi_env.frag_size) {
            hdr = GKI_getbuf(sizeof(struct blufi_hdr) + 2 + blufi_env.frag_size + 2);
            if (hdr == NULL) {
                LOG_ERROR("%s no mem\n", __func__);
                return;
            }
            hdr->fc = 0x0;
            hdr->data_len = blufi_env.frag_size + 2;
            hdr->data[0] = remain_len & 0xff;
            hdr->data[1] = (remain_len >> 8) & 0xff;
            memcpy(hdr->data + 2, &data[total_data_len - remain_len], blufi_env.frag_size); //copy first, easy for check sum
            hdr->fc |= BLUFI_FC_FRAG;
        } else {
            hdr = GKI_getbuf(sizeof(struct blufi_hdr) + remain_len + 2);
            if (hdr == NULL) {
                LOG_ERROR("%s no mem\n", __func__);
                return;
            }
            hdr->fc = 0x0;
            hdr->data_len = remain_len;
            memcpy(hdr->data, &data[total_data_len - remain_len], hdr->data_len); //copy first, easy for check sum
        }

        hdr->type = type;
        hdr->fc |= BLUFI_FC_DIR_E2P;
        hdr->seq = blufi_env.send_seq++;
 
        if (BLUFI_TYPE_IS_CTRL(hdr->type)) {
            if ((blufi_env.sec_mode & BLUFI_CTRL_SEC_MODE_CHECK_MASK)
                    && (blufi_env.cbs && blufi_env.cbs->checksum_func)) {
                hdr->fc |= BLUFI_FC_CHECK;
                checksum = blufi_env.cbs->checksum_func(hdr->seq, &hdr->seq, hdr->data_len + 2);
                memcpy(&hdr->data[hdr->data_len], &checksum, 2);
            }
        } else if (!BLUFI_TYPE_IS_DATA_NEG(hdr->type)) {
            if ((blufi_env.sec_mode & BLUFI_DATA_SEC_MODE_CHECK_MASK)
                    && (blufi_env.cbs && blufi_env.cbs->checksum_func)) {
                hdr->fc |= BLUFI_FC_CHECK;
                checksum = blufi_env.cbs->checksum_func(hdr->seq, &hdr->seq, hdr->data_len + 2);
                memcpy(&hdr->data[hdr->data_len], &checksum, 2);
            }

            if ((blufi_env.sec_mode & BLUFI_DATA_SEC_MODE_ENC_MASK)
                    && (blufi_env.cbs && blufi_env.cbs->encrypt_func)) {
                ret = blufi_env.cbs->encrypt_func(hdr->seq, hdr->data, hdr->data_len);
                if (ret == hdr->data_len) { /* enc must be success and enc len must equal to plain len */
                    hdr->fc |= BLUFI_FC_ENC;
                } else {
                    LOG_ERROR("%s encrypt error %d\n", __func__, ret);
                    GKI_freebuf(hdr);
                    return;
                }
            }
        }

        if (hdr->fc & BLUFI_FC_FRAG) {
            remain_len -= (hdr->data_len - 2);
        } else {
            remain_len -= hdr->data_len;
        }

        btc_blufi_send_notify((uint8_t *)hdr,
                ((hdr->fc & BLUFI_FC_CHECK) ?
                 hdr->data_len + sizeof(struct blufi_hdr) + 2 :
                 hdr->data_len + sizeof(struct blufi_hdr)));

        GKI_freebuf(hdr);
        hdr =  NULL;
    }
}

static void btc_blufi_wifi_conn_report(uint8_t opmode, uint8_t sta_conn_state, uint8_t softap_conn_num, esp_blufi_extra_info_t *info, int info_len)
{
    uint8_t type;
    uint8_t *data;
    int data_len;
    uint8_t *p;

    data_len = info_len + 3;
    p = data = GKI_getbuf(data_len);
    if (data == NULL) {
        return;
    }

    type = BLUFI_BUILD_TYPE(BLUFI_TYPE_DATA, BLUFI_TYPE_DATA_SUBTYPE_WIFI_REP);
    *p++ = opmode;
    *p++ = sta_conn_state;
    *p++ = softap_conn_num;

    if (info) {
        if (info->sta_bssid_set) {
            *p++ = BLUFI_TYPE_DATA_SUBTYPE_STA_BSSID;
            *p++ = 6;
            memcpy(p, info->sta_bssid, 6);
            p += 6;
        }
        if (info->sta_ssid) {
            *p++ = BLUFI_TYPE_DATA_SUBTYPE_STA_SSID;
            *p++ = info->sta_ssid_len;
            memcpy(p, info->sta_ssid, info->sta_ssid_len);
            p += info->sta_ssid_len;
        }
        if (info->sta_passwd) {
            *p++ = BLUFI_TYPE_DATA_SUBTYPE_STA_PASSWD;
            *p++ = info->sta_passwd_len;
            memcpy(p, info->sta_passwd, info->sta_passwd_len);
            p += info->sta_passwd_len;
        }
        if (info->softap_ssid) {
            *p++ = BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_SSID;
            *p++ = info->softap_ssid_len;
            memcpy(p, info->softap_ssid, info->softap_ssid_len);
            p += info->softap_ssid_len;
        }
        if (info->softap_passwd) {
            *p++ = BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_PASSWD;
            *p++ = info->softap_passwd_len;
            memcpy(p, info->softap_passwd, info->softap_passwd_len);
            p += info->softap_passwd_len;
        }
        if (info->softap_authmode_set) {
            *p++ = BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_AUTH_MODE;
            *p++ = 1;
            *p++ = info->softap_authmode;
        }
        if (info->softap_max_conn_num_set) {
            *p++ = BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_MAX_CONN_NUM;
            *p++ = 1;
            *p++ = info->softap_max_conn_num;
        }
        if (info->softap_channel_set) {
            *p++ = BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_MAX_CONN_NUM;
            *p++ = 1;
            *p++ = info->softap_channel;
        }
    }
    if (p - data > data_len) {
        LOG_ERROR("%s len error %d %d\n", __func__, (int)(p - data), data_len);
    }

    btc_blufi_send_encap(type, data, data_len);
    GKI_freebuf(data);
}

static void btc_blufi_send_ack(uint8_t seq)
{
    uint8_t type;
    uint8_t data;

    type = BLUFI_BUILD_TYPE(BLUFI_TYPE_CTRL, BLUFI_TYPE_CTRL_SUBTYPE_ACK);
    data = seq;

    btc_blufi_send_encap(type, &data, 1);
}

void btc_blufi_cb_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_blufi_cb_param_t *dst = (esp_blufi_cb_param_t *) p_dest;
    esp_blufi_cb_param_t *src = (esp_blufi_cb_param_t *) p_src;

    switch (msg->act) {
    case ESP_BLUFI_EVENT_RECV_STA_SSID:
        dst->sta_ssid.ssid = GKI_getbuf(src->sta_ssid.ssid_len);
        if (dst->sta_ssid.ssid == NULL) {
            LOG_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->sta_ssid.ssid, src->sta_ssid.ssid, src->sta_ssid.ssid_len);
        break;
    case ESP_BLUFI_EVENT_RECV_STA_PASSWD:
        dst->sta_passwd.passwd = GKI_getbuf(src->sta_passwd.passwd_len);
        if (dst->sta_passwd.passwd == NULL) {
            LOG_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->sta_passwd.passwd, src->sta_passwd.passwd, src->sta_passwd.passwd_len);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_SSID:
        dst->softap_ssid.ssid = GKI_getbuf(src->softap_ssid.ssid_len);
        if (dst->softap_ssid.ssid == NULL) {
            LOG_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->softap_ssid.ssid, src->softap_ssid.ssid, src->softap_ssid.ssid_len);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD:
        dst->softap_passwd.passwd = GKI_getbuf(src->softap_passwd.passwd_len);
        if (dst->softap_passwd.passwd == NULL) {
            LOG_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->softap_passwd.passwd, src->softap_passwd.passwd, src->softap_passwd.passwd_len);
        break;
    case ESP_BLUFI_EVENT_RECV_USERNAME:
        dst->username.name = GKI_getbuf(src->username.name_len);
        if (dst->username.name == NULL) {
            LOG_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->username.name, src->username.name, src->username.name_len);
        break;
    case ESP_BLUFI_EVENT_RECV_CA_CERT:
        dst->ca.cert = GKI_getbuf(src->ca.cert_len);
        if (dst->ca.cert == NULL) {
            LOG_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->ca.cert, src->ca.cert, src->ca.cert_len);
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_CERT:
        dst->client_cert.cert = GKI_getbuf(src->client_cert.cert_len);
        if (dst->client_cert.cert == NULL) {
            LOG_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->client_cert.cert, src->client_cert.cert, src->client_cert.cert_len);
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_CERT:
        dst->server_cert.cert = GKI_getbuf(src->server_cert.cert_len);
        if (dst->server_cert.cert == NULL) {
            LOG_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->server_cert.cert, src->server_cert.cert, src->server_cert.cert_len);
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY:
         dst->client_pkey.pkey = GKI_getbuf(src->client_pkey.pkey_len);
        if (dst->client_pkey.pkey == NULL) {
            LOG_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->client_pkey.pkey, src->client_pkey.pkey, src->client_pkey.pkey_len);
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY:
         dst->server_pkey.pkey = GKI_getbuf(src->server_pkey.pkey_len);
        if (dst->server_pkey.pkey == NULL) {
            LOG_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->server_pkey.pkey, src->server_pkey.pkey, src->server_pkey.pkey_len);
        break;
    default:
        break;
    }
}

void btc_blufi_cb_deep_free(btc_msg_t *msg)
{
    esp_blufi_cb_param_t *param = (esp_blufi_cb_param_t *)msg->arg;

    switch (msg->act) {
    case ESP_BLUFI_EVENT_RECV_STA_SSID:
        GKI_freebuf(param->sta_ssid.ssid);
        break;
    case ESP_BLUFI_EVENT_RECV_STA_PASSWD:
        GKI_freebuf(param->sta_passwd.passwd);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_SSID:
        GKI_freebuf(param->softap_ssid.ssid);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD:
        GKI_freebuf(param->softap_passwd.passwd);
        break;
    case ESP_BLUFI_EVENT_RECV_USERNAME:
        GKI_freebuf(param->username.name);
        break;
    case ESP_BLUFI_EVENT_RECV_CA_CERT:
        GKI_freebuf(param->ca.cert);
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_CERT:
        GKI_freebuf(param->client_cert.cert);
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_CERT:
        GKI_freebuf(param->server_cert.cert);
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY:
        GKI_freebuf(param->client_pkey.pkey);
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY:
        GKI_freebuf(param->server_pkey.pkey);
        break;
    default:
        break;
    }
}

void btc_blufi_cb_handler(btc_msg_t *msg)
{
    esp_blufi_cb_param_t *param = (esp_blufi_cb_param_t *)msg->arg;

    switch (msg->act) {
    case ESP_BLUFI_EVENT_INIT_FINISH: {
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_INIT_FINISH, param);
        break;
    }
    case ESP_BLUFI_EVENT_DEINIT_FINISH: {
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_DEINIT_FINISH, param);
        break;
    }
    case ESP_BLUFI_EVENT_BLE_CONNECT:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_BLE_CONNECT, param);
        break;
    case ESP_BLUFI_EVENT_BLE_DISCONNECT:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_BLE_DISCONNECT, param);
        break;
    case ESP_BLUFI_EVENT_SET_WIFI_OPMODE:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_SET_WIFI_OPMODE, param);
        break;
    case ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP, NULL);
        break;
    case ESP_BLUFI_EVENT_REQ_DISCONNECT_FROM_AP:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_REQ_DISCONNECT_FROM_AP, NULL);
        break;
    case ESP_BLUFI_EVENT_GET_WIFI_STATUS:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_GET_WIFI_STATUS, NULL);
        break;
    case ESP_BLUFI_EVENT_DEAUTHENTICATE_STA:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_DEAUTHENTICATE_STA, NULL);
        break;
    case ESP_BLUFI_EVENT_RECV_STA_BSSID:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_STA_BSSID, param);
        break;
    case ESP_BLUFI_EVENT_RECV_STA_SSID:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_STA_SSID, param);
        break;
    case ESP_BLUFI_EVENT_RECV_STA_PASSWD:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_STA_PASSWD, param);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_SSID:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_SOFTAP_SSID, param);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD, param);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_MAX_CONN_NUM:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_SOFTAP_MAX_CONN_NUM, param);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_AUTH_MODE:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_SOFTAP_AUTH_MODE, param);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_CHANNEL:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_SOFTAP_CHANNEL, param);
        break;
    case ESP_BLUFI_EVENT_RECV_USERNAME:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_USERNAME, param);
        break;
    case ESP_BLUFI_EVENT_RECV_CA_CERT:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_CA_CERT, param);
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_CERT:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_CLIENT_CERT, param);
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_CERT:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_SERVER_CERT, param);
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY, param);
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY, param);
        break;
    default:
        LOG_ERROR("%s UNKNOWN %d\n", __func__, msg->act);
        break;
    }

    btc_blufi_cb_deep_free(msg);
}

void btc_blufi_call_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_blufi_args_t *dst = (btc_blufi_args_t *) p_dest;
    btc_blufi_args_t *src = (btc_blufi_args_t *) p_src;

    switch (msg->act) {
    case BTC_BLUFI_ACT_SEND_CFG_REPORT: {
        esp_blufi_extra_info_t *src_info = src->wifi_conn_report.extra_info;
        dst->wifi_conn_report.extra_info_len = 0;
        dst->wifi_conn_report.extra_info = NULL;

        if (src_info == NULL) {
            return;
        }

        dst->wifi_conn_report.extra_info = GKI_getbuf(sizeof(esp_blufi_extra_info_t));
        if (dst->wifi_conn_report.extra_info == NULL) {
            return;
        }

        if (src_info->sta_bssid_set) {
            memcpy(dst->wifi_conn_report.extra_info->sta_bssid, src_info->sta_bssid, 6);
            dst->wifi_conn_report.extra_info->sta_bssid_set = src_info->sta_bssid_set;
            dst->wifi_conn_report.extra_info_len += (6 + 2);
        }
        if (src_info->sta_ssid) {
            dst->wifi_conn_report.extra_info->sta_ssid = GKI_getbuf(src_info->sta_ssid_len);
            if (dst->wifi_conn_report.extra_info->sta_ssid) {
                memcpy(dst->wifi_conn_report.extra_info->sta_ssid, src_info->sta_ssid, src_info->sta_ssid_len);
                dst->wifi_conn_report.extra_info->sta_ssid_len = src_info->sta_ssid_len;
                dst->wifi_conn_report.extra_info_len += (dst->wifi_conn_report.extra_info->sta_ssid_len + 2);
            }
        }
        if (src_info->sta_passwd) {
            dst->wifi_conn_report.extra_info->sta_passwd = GKI_getbuf(src_info->sta_passwd_len);
            if (dst->wifi_conn_report.extra_info->sta_passwd) {
                memcpy(dst->wifi_conn_report.extra_info->sta_passwd, src_info->sta_passwd, src_info->sta_passwd_len);
                dst->wifi_conn_report.extra_info->sta_passwd_len = src_info->sta_passwd_len;
                dst->wifi_conn_report.extra_info_len += (dst->wifi_conn_report.extra_info->sta_passwd_len + 2);
            }
        }
        if (src_info->softap_ssid) {
            dst->wifi_conn_report.extra_info->softap_ssid = GKI_getbuf(src_info->softap_ssid_len);
            if (dst->wifi_conn_report.extra_info->softap_ssid) {
                memcpy(dst->wifi_conn_report.extra_info->softap_ssid, src_info->softap_ssid, src_info->softap_ssid_len);
                dst->wifi_conn_report.extra_info->softap_ssid_len = src_info->softap_ssid_len;
                dst->wifi_conn_report.extra_info_len += (dst->wifi_conn_report.extra_info->softap_ssid_len + 2);
            }
        }
        if (src_info->softap_passwd) {
            dst->wifi_conn_report.extra_info->softap_passwd = GKI_getbuf(src_info->softap_passwd_len);
            if (dst->wifi_conn_report.extra_info->softap_passwd) {
                memcpy(dst->wifi_conn_report.extra_info->softap_passwd, src_info->softap_passwd, src_info->softap_passwd_len);
                dst->wifi_conn_report.extra_info->softap_passwd_len = src_info->softap_passwd_len;
                dst->wifi_conn_report.extra_info_len += (dst->wifi_conn_report.extra_info->softap_passwd_len + 2);
            }
        }
        if (src_info->softap_authmode_set) {
            dst->wifi_conn_report.extra_info->softap_authmode_set = src_info->softap_authmode_set;
            dst->wifi_conn_report.extra_info->softap_authmode = src_info->softap_authmode;
            dst->wifi_conn_report.extra_info_len += (1 + 2);
        }
        if (src_info->softap_max_conn_num_set) {
            dst->wifi_conn_report.extra_info->softap_max_conn_num_set = src_info->softap_max_conn_num_set;
            dst->wifi_conn_report.extra_info->softap_max_conn_num = src_info->softap_max_conn_num;
            dst->wifi_conn_report.extra_info_len += (1 + 2);
        }
        if (src_info->softap_channel_set) {
            dst->wifi_conn_report.extra_info->softap_channel_set = src_info->softap_channel_set;
            dst->wifi_conn_report.extra_info->softap_channel = src_info->softap_channel;
            dst->wifi_conn_report.extra_info_len += (1 + 2);
        }
        break;
    }
    default:
        break;
    }
}

void btc_blufi_call_deep_free(btc_msg_t *msg)
{
    btc_blufi_args_t *arg = (btc_blufi_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_BLUFI_ACT_SEND_CFG_REPORT: {
        esp_blufi_extra_info_t *info = (esp_blufi_extra_info_t *)arg->wifi_conn_report.extra_info;

        if (info == NULL) {
            return;
        }
        if (info->sta_ssid) {
            GKI_freebuf(info->sta_ssid);
        }
        if (info->sta_passwd) {
            GKI_freebuf(info->sta_passwd);
        }
        if (info->softap_ssid) {
            GKI_freebuf(info->softap_ssid);
        }
        if (info->softap_passwd) {
            GKI_freebuf(info->softap_passwd);
        }
        GKI_freebuf(info);
        break;
    }
    default:
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
        btc_blufi_profile_deinit();
        break;
    case BTC_BLUFI_ACT_SEND_CFG_REPORT:
        btc_blufi_wifi_conn_report(arg->wifi_conn_report.opmode,
                                   arg->wifi_conn_report.sta_conn_state,
                                   arg->wifi_conn_report.softap_conn_num,
                                   arg->wifi_conn_report.extra_info,
                                   arg->wifi_conn_report.extra_info_len);
        break;
    default:
        LOG_ERROR("%s UNKNOWN %d\n", __func__, msg->act);
        break;
    }
    btc_blufi_call_deep_free(msg);
}

void btc_blufi_set_callbacks(esp_blufi_callbacks_t *callbacks)
{
    blufi_env.cbs = callbacks;
}

uint16_t btc_blufi_get_version(void)
{
    return BTC_BLUFI_VERSION;
}

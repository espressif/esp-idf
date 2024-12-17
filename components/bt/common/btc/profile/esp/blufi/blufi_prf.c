/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include <string.h>
#include <stdbool.h>
#include <stdio.h>


#include "osi/allocator.h"

#include "btc_blufi_prf.h"
#include "btc/btc_task.h"
#include "btc/btc_manage.h"

#include "blufi_int.h"

#include "esp_log.h"
#include "esp_blufi_api.h"
#include "esp_blufi.h"

#if (BLUFI_INCLUDED == TRUE)

#if GATT_DYNAMIC_MEMORY == FALSE
tBLUFI_ENV blufi_env;
#else
tBLUFI_ENV *blufi_env_ptr;
#endif

// static functions declare
static void btc_blufi_send_ack(uint8_t seq);

inline void btc_blufi_cb_to_app(esp_blufi_cb_event_t event, esp_blufi_cb_param_t *param)
{
    esp_blufi_event_cb_t btc_blufi_cb = (esp_blufi_event_cb_t)btc_profile_cb_get(BTC_PID_BLUFI);
    if (btc_blufi_cb) {
	btc_blufi_cb(event, param);
    }
}

static uint8_t btc_blufi_profile_init(void)
{
    esp_blufi_callbacks_t *store_p = blufi_env.cbs;

    uint8_t rc;
    if (blufi_env.enabled) {
        BLUFI_TRACE_ERROR("BLUFI already initialized");
        return ESP_BLUFI_ERROR;
    }

    memset(&blufi_env, 0x0, sizeof(blufi_env));
    blufi_env.cbs = store_p;        /* if set callback prior, restore the point */
    blufi_env.frag_size = BLUFI_FRAG_DATA_DEFAULT_LEN;
    rc = esp_blufi_init();
    if(rc != 0 ){
       return rc;
    }

    return ESP_BLUFI_SUCCESS;
}

static uint8_t btc_blufi_profile_deinit(void)
{
    if (!blufi_env.enabled) {
        BTC_TRACE_ERROR("BLUFI already de-initialized");
        return ESP_BLUFI_ERROR;
    }

    esp_blufi_deinit();
    return ESP_BLUFI_SUCCESS;
}

void btc_blufi_send_notify(uint8_t *pkt, int pkt_len)
{
   struct pkt_info pkts;
   pkts.pkt = pkt;
   pkts.pkt_len = pkt_len;
   esp_blufi_send_notify(&pkts);
}

void btc_blufi_report_error(esp_blufi_error_state_t state)
{
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_BLUFI;
    msg.act = ESP_BLUFI_EVENT_REPORT_ERROR;
    esp_blufi_cb_param_t param;
    param.report_error.state = state;
    btc_transfer_context(&msg, &param, sizeof(esp_blufi_cb_param_t), NULL, NULL);
}

void btc_blufi_recv_handler(uint8_t *data, int len)
{
    if (len < sizeof(struct blufi_hdr)) {
        BTC_TRACE_ERROR("%s invalid data length: %d", __func__, len);
        btc_blufi_report_error(ESP_BLUFI_DATA_FORMAT_ERROR);
        return;
    }

    struct blufi_hdr *hdr = (struct blufi_hdr *)data;

    // Verify if the received data length matches the expected length based on the BLUFI protocol
    int target_data_len;

    if (BLUFI_FC_IS_CHECK(hdr->fc)) {
        target_data_len = hdr->data_len + 4 + 2; // Data + (Type + Frame Control + Sequence Number + Data Length) + Checksum
    } else {
        target_data_len = hdr->data_len + 4; // Data + (Type + Frame Control + Sequence Number + Data Length)
    }

    if (len != target_data_len) {
        BTC_TRACE_ERROR("%s: Invalid data length: %d, expected: %d", __func__, len, target_data_len);
        btc_blufi_report_error(ESP_BLUFI_DATA_FORMAT_ERROR);
        return;
    }

    uint16_t checksum, checksum_pkt;
    int ret;

    if (hdr->seq != blufi_env.recv_seq) {
        BTC_TRACE_ERROR("%s seq %d is not expect %d\n", __func__, hdr->seq, blufi_env.recv_seq + 1);
        btc_blufi_report_error(ESP_BLUFI_SEQUENCE_ERROR);
        return;
    }

    blufi_env.recv_seq++;

    // first step, decrypt
    if (BLUFI_FC_IS_ENC(hdr->fc)
            && (blufi_env.cbs && blufi_env.cbs->decrypt_func)) {
        ret = blufi_env.cbs->decrypt_func(hdr->seq, hdr->data, hdr->data_len);
        if (ret != hdr->data_len) { /* enc must be success and enc len must equal to plain len */
            BTC_TRACE_ERROR("%s decrypt error %d\n", __func__, ret);
            btc_blufi_report_error(ESP_BLUFI_DECRYPT_ERROR);
            return;
        }
    }

    // second step, check sum
    if (BLUFI_FC_IS_CHECK(hdr->fc)
            && (blufi_env.cbs && blufi_env.cbs->checksum_func)) {
        checksum = blufi_env.cbs->checksum_func(hdr->seq, &hdr->seq, hdr->data_len + 2);
        checksum_pkt = hdr->data[hdr->data_len] | (((uint16_t) hdr->data[hdr->data_len + 1]) << 8);
        if (checksum != checksum_pkt) {
            BTC_TRACE_ERROR("%s checksum error %04x, pkt %04x\n", __func__, checksum, checksum_pkt);
            btc_blufi_report_error(ESP_BLUFI_CHECKSUM_ERROR);
            return;
        }
    }

    if (BLUFI_FC_IS_REQ_ACK(hdr->fc)) {
        btc_blufi_send_ack(hdr->seq);
    }

    if (BLUFI_FC_IS_FRAG(hdr->fc)) {
        if (blufi_env.offset == 0) {
            /*
            blufi_env.aggr_buf should be NULL if blufi_env.offset is 0.
            It is possible that the process of sending fragment packet
            has not been completed
            */
            if (blufi_env.aggr_buf) {
                BTC_TRACE_ERROR("%s msg error, blufi_env.aggr_buf is not freed\n", __func__);
                btc_blufi_report_error(ESP_BLUFI_MSG_STATE_ERROR);
                return;
            }
            blufi_env.total_len = hdr->data[0] | (((uint16_t) hdr->data[1]) << 8);
            blufi_env.aggr_buf = osi_malloc(blufi_env.total_len);
            if (blufi_env.aggr_buf == NULL) {
                BTC_TRACE_ERROR("%s no mem, len %d\n", __func__, blufi_env.total_len);
                btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
                return;
            }
        }
        if (blufi_env.offset + hdr->data_len  - 2 <= blufi_env.total_len){
            memcpy(blufi_env.aggr_buf + blufi_env.offset, hdr->data + 2, hdr->data_len  - 2);
            blufi_env.offset += (hdr->data_len - 2);
        } else {
            BTC_TRACE_ERROR("%s payload is longer than packet length, len %d \n", __func__, blufi_env.total_len);
            btc_blufi_report_error(ESP_BLUFI_DATA_FORMAT_ERROR);
            return;
        }

    } else {
        if (blufi_env.offset > 0) {   /* if previous pkt is frag */
            /* blufi_env.aggr_buf should not be NULL */
            if (blufi_env.aggr_buf == NULL) {
                BTC_TRACE_ERROR("%s buffer is NULL\n", __func__);
                btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
                return;
            }
            /* payload length should be equal to total_len */
            if ((blufi_env.offset + hdr->data_len) != blufi_env.total_len) {
                BTC_TRACE_ERROR("%s payload is longer than packet length, len %d \n", __func__, blufi_env.total_len);
                btc_blufi_report_error(ESP_BLUFI_DATA_FORMAT_ERROR);
                return;
            }
            memcpy(blufi_env.aggr_buf + blufi_env.offset, hdr->data, hdr->data_len);

            btc_blufi_protocol_handler(hdr->type, blufi_env.aggr_buf, blufi_env.total_len);
            blufi_env.offset = 0;
            osi_free(blufi_env.aggr_buf);
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

    if (blufi_env.is_connected == false) {
        BTC_TRACE_ERROR("blufi connection has been disconnected \n");
        return;
    }

    while (remain_len > 0) {
        if (remain_len > blufi_env.frag_size) {
            hdr = osi_malloc(sizeof(struct blufi_hdr) + 2 + blufi_env.frag_size + 2);
            if (hdr == NULL) {
                BTC_TRACE_ERROR("%s no mem\n", __func__);
                return;
            }
            hdr->fc = 0x0;
            hdr->data_len = blufi_env.frag_size + 2;
            hdr->data[0] = remain_len & 0xff;
            hdr->data[1] = (remain_len >> 8) & 0xff;
            memcpy(hdr->data + 2, &data[total_data_len - remain_len], blufi_env.frag_size); //copy first, easy for check sum
            hdr->fc |= BLUFI_FC_FRAG;
        } else {
            hdr = osi_malloc(sizeof(struct blufi_hdr) + remain_len + 2);
            if (hdr == NULL) {
                BTC_TRACE_ERROR("%s no mem\n", __func__);
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
        } else if (!BLUFI_TYPE_IS_DATA_NEG(hdr->type) && !BLUFI_TYPE_IS_DATA_ERROR_INFO(hdr->type)) {
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
                    BTC_TRACE_ERROR("%s encrypt error %d\n", __func__, ret);
                    btc_blufi_report_error(ESP_BLUFI_ENCRYPT_ERROR);
                    osi_free(hdr);
                    return;
                }
            }
        }

        if (hdr->fc & BLUFI_FC_FRAG) {
            remain_len -= (hdr->data_len - 2);
        } else {
            remain_len -= hdr->data_len;
        }

       esp_blufi_send_encap(hdr);

        osi_free(hdr);
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
    p = data = osi_malloc(data_len);
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
            *p++ = BLUFI_TYPE_DATA_SUBTYPE_SOFTAP_CHANNEL;
            *p++ = 1;
            *p++ = info->softap_channel;
        }
        if (info->sta_max_conn_retry_set) {
            *p++ = BLUFI_TYPE_DATA_SUBTYPE_STA_MAX_CONN_RETRY;
            *p++ = 1;
            *p++ = info->sta_max_conn_retry;
        }
        if (info->sta_conn_end_reason_set) {
            *p++ = BLUFI_TYPE_DATA_SUBTYPE_STA_CONN_END_REASON;
            *p++ = 1;
            *p++ = info->sta_conn_end_reason;
        }
        if (info->sta_conn_rssi_set) {
            *p++ = BLUFI_TYPE_DATA_SUBTYPE_STA_CONN_RSSI;
            *p++ = 1;
            *p++ = info->sta_conn_rssi;
        }
    }
    if (p - data > data_len) {
        BTC_TRACE_ERROR("%s len error %d %d\n", __func__, (int)(p - data), data_len);
    }

    btc_blufi_send_encap(type, data, data_len);
    osi_free(data);
}

void btc_blufi_send_wifi_list(uint16_t apCount, esp_blufi_ap_record_t *list)
{
    uint8_t type;
    uint8_t *data;
    int data_len;
    uint8_t *p;
    // malloc size: (len + RSSI + ssid buffer) * apCount;
    uint32_t malloc_size = (1 + 1 + sizeof(list->ssid)) * apCount;
    p = data = osi_malloc(malloc_size);
    if (data == NULL) {
        BTC_TRACE_ERROR("malloc error\n");
        return;
    }
    type = BLUFI_BUILD_TYPE(BLUFI_TYPE_DATA, BLUFI_TYPE_DATA_SUBTYPE_WIFI_LIST);
    for (int i = 0; i < apCount; ++i)
    {
        uint32_t len = strlen((const char *)list[i].ssid);
        data_len = (p - data);
        //current_len + ssid + rssi + total_len_value
        if((data_len + len + 1 + 1) >  malloc_size) {
            BTC_TRACE_ERROR("%s len error", __func__);
            osi_free(data);
            return;
        }
        *p++ = len + 1; // length of ssid + rssi
        *p++ = list[i].rssi;
        memcpy(p, list[i].ssid, len);
        p = p + len;
    }
    data_len = (p - data);
    btc_blufi_send_encap(type, data, data_len);
    osi_free(data);
}

static void btc_blufi_send_ack(uint8_t seq)
{
    uint8_t type;
    uint8_t data;

    type = BLUFI_BUILD_TYPE(BLUFI_TYPE_CTRL, BLUFI_TYPE_CTRL_SUBTYPE_ACK);
    data = seq;

    btc_blufi_send_encap(type, &data, 1);
}
static void btc_blufi_send_error_info(uint8_t state)
{
    uint8_t type;
    uint8_t *data;
    int data_len;
    uint8_t *p;

    data_len = 1;
    p = data = osi_malloc(data_len);
    if (data == NULL) {
        return;
    }

    type = BLUFI_BUILD_TYPE(BLUFI_TYPE_DATA, BLUFI_TYPE_DATA_SUBTYPE_ERROR_INFO);
    *p++ = state;
    if (p - data > data_len) {
        BTC_TRACE_ERROR("%s len error %d %d\n", __func__, (int)(p - data), data_len);
    }

    btc_blufi_send_encap(type, data, data_len);
    osi_free(data);
}

static void btc_blufi_send_custom_data(uint8_t *value, uint32_t value_len)
{
    if(value == NULL || value_len == 0) {
        BTC_TRACE_ERROR("%s value or value len error", __func__);
        return;
    }
    uint8_t *data = osi_malloc(value_len);
    if (data == NULL) {
        BTC_TRACE_ERROR("%s mem malloc error", __func__);
        return;
    }
    uint8_t type = BLUFI_BUILD_TYPE(BLUFI_TYPE_DATA, BLUFI_TYPE_DATA_SUBTYPE_CUSTOM_DATA);
    memcpy(data, value, value_len);
    btc_blufi_send_encap(type, data, value_len);
    osi_free(data);
}

void btc_blufi_cb_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_blufi_cb_param_t *dst = (esp_blufi_cb_param_t *) p_dest;
    esp_blufi_cb_param_t *src = (esp_blufi_cb_param_t *) p_src;

    switch (msg->act) {
    case ESP_BLUFI_EVENT_RECV_STA_SSID:
        dst->sta_ssid.ssid = osi_malloc(src->sta_ssid.ssid_len);
        if (dst->sta_ssid.ssid == NULL) {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->sta_ssid.ssid, src->sta_ssid.ssid, src->sta_ssid.ssid_len);
        break;
    case ESP_BLUFI_EVENT_RECV_STA_PASSWD:
        dst->sta_passwd.passwd = osi_malloc(src->sta_passwd.passwd_len);
        if (dst->sta_passwd.passwd == NULL) {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->sta_passwd.passwd, src->sta_passwd.passwd, src->sta_passwd.passwd_len);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_SSID:
        dst->softap_ssid.ssid = osi_malloc(src->softap_ssid.ssid_len);
        if (dst->softap_ssid.ssid == NULL) {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->softap_ssid.ssid, src->softap_ssid.ssid, src->softap_ssid.ssid_len);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD:
        dst->softap_passwd.passwd = osi_malloc(src->softap_passwd.passwd_len);
        if (dst->softap_passwd.passwd == NULL) {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->softap_passwd.passwd, src->softap_passwd.passwd, src->softap_passwd.passwd_len);
        break;
    case ESP_BLUFI_EVENT_RECV_USERNAME:
        dst->username.name = osi_malloc(src->username.name_len);
        if (dst->username.name == NULL) {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->username.name, src->username.name, src->username.name_len);
        break;
    case ESP_BLUFI_EVENT_RECV_CA_CERT:
        dst->ca.cert = osi_malloc(src->ca.cert_len);
        if (dst->ca.cert == NULL) {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->ca.cert, src->ca.cert, src->ca.cert_len);
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_CERT:
        dst->client_cert.cert = osi_malloc(src->client_cert.cert_len);
        if (dst->client_cert.cert == NULL) {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->client_cert.cert, src->client_cert.cert, src->client_cert.cert_len);
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_CERT:
        dst->server_cert.cert = osi_malloc(src->server_cert.cert_len);
        if (dst->server_cert.cert == NULL) {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->server_cert.cert, src->server_cert.cert, src->server_cert.cert_len);
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY:
         dst->client_pkey.pkey = osi_malloc(src->client_pkey.pkey_len);
        if (dst->client_pkey.pkey == NULL) {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->client_pkey.pkey, src->client_pkey.pkey, src->client_pkey.pkey_len);
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY:
         dst->server_pkey.pkey = osi_malloc(src->server_pkey.pkey_len);
        if (dst->server_pkey.pkey == NULL) {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        memcpy(dst->server_pkey.pkey, src->server_pkey.pkey, src->server_pkey.pkey_len);
        break;
    case ESP_BLUFI_EVENT_RECV_CUSTOM_DATA:
         dst->custom_data.data = osi_malloc(src->custom_data.data_len);
        if (dst->custom_data.data == NULL) {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
            break;
        }
        memcpy(dst->custom_data.data, src->custom_data.data, src->custom_data.data_len);
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
        osi_free(param->sta_ssid.ssid);
        break;
    case ESP_BLUFI_EVENT_RECV_STA_PASSWD:
        osi_free(param->sta_passwd.passwd);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_SSID:
        osi_free(param->softap_ssid.ssid);
        break;
    case ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD:
        osi_free(param->softap_passwd.passwd);
        break;
    case ESP_BLUFI_EVENT_RECV_USERNAME:
        osi_free(param->username.name);
        break;
    case ESP_BLUFI_EVENT_RECV_CA_CERT:
        osi_free(param->ca.cert);
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_CERT:
        osi_free(param->client_cert.cert);
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_CERT:
        osi_free(param->server_cert.cert);
        break;
    case ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY:
        osi_free(param->client_pkey.pkey);
        break;
    case ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY:
        osi_free(param->server_pkey.pkey);
        break;
    case ESP_BLUFI_EVENT_RECV_CUSTOM_DATA:
        osi_free(param->custom_data.data);
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
    case ESP_BLUFI_EVENT_GET_WIFI_LIST:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_GET_WIFI_LIST, NULL);
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
    case ESP_BLUFI_EVENT_RECV_SLAVE_DISCONNECT_BLE:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_SLAVE_DISCONNECT_BLE, param);
        break;
    case ESP_BLUFI_EVENT_REPORT_ERROR:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_REPORT_ERROR, param);
        break;
    case ESP_BLUFI_EVENT_RECV_CUSTOM_DATA:
        btc_blufi_cb_to_app(ESP_BLUFI_EVENT_RECV_CUSTOM_DATA, param);
        break;
    default:
        BTC_TRACE_ERROR("%s UNKNOWN %d\n", __func__, msg->act);
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

        dst->wifi_conn_report.extra_info = osi_calloc(sizeof(esp_blufi_extra_info_t));
        if (dst->wifi_conn_report.extra_info == NULL) {
            return;
        }

        if (src_info->sta_bssid_set) {
            memcpy(dst->wifi_conn_report.extra_info->sta_bssid, src_info->sta_bssid, 6);
            dst->wifi_conn_report.extra_info->sta_bssid_set = src_info->sta_bssid_set;
            dst->wifi_conn_report.extra_info_len += (6 + 2);
        }
        if (src_info->sta_ssid) {
            dst->wifi_conn_report.extra_info->sta_ssid = osi_malloc(src_info->sta_ssid_len);
            if (dst->wifi_conn_report.extra_info->sta_ssid) {
                memcpy(dst->wifi_conn_report.extra_info->sta_ssid, src_info->sta_ssid, src_info->sta_ssid_len);
                dst->wifi_conn_report.extra_info->sta_ssid_len = src_info->sta_ssid_len;
                dst->wifi_conn_report.extra_info_len += (dst->wifi_conn_report.extra_info->sta_ssid_len + 2);
            }
        }
        if (src_info->sta_passwd) {
            dst->wifi_conn_report.extra_info->sta_passwd = osi_malloc(src_info->sta_passwd_len);
            if (dst->wifi_conn_report.extra_info->sta_passwd) {
                memcpy(dst->wifi_conn_report.extra_info->sta_passwd, src_info->sta_passwd, src_info->sta_passwd_len);
                dst->wifi_conn_report.extra_info->sta_passwd_len = src_info->sta_passwd_len;
                dst->wifi_conn_report.extra_info_len += (dst->wifi_conn_report.extra_info->sta_passwd_len + 2);
            }
        }
        if (src_info->softap_ssid) {
            dst->wifi_conn_report.extra_info->softap_ssid = osi_malloc(src_info->softap_ssid_len);
            if (dst->wifi_conn_report.extra_info->softap_ssid) {
                memcpy(dst->wifi_conn_report.extra_info->softap_ssid, src_info->softap_ssid, src_info->softap_ssid_len);
                dst->wifi_conn_report.extra_info->softap_ssid_len = src_info->softap_ssid_len;
                dst->wifi_conn_report.extra_info_len += (dst->wifi_conn_report.extra_info->softap_ssid_len + 2);
            }
        }
        if (src_info->softap_passwd) {
            dst->wifi_conn_report.extra_info->softap_passwd = osi_malloc(src_info->softap_passwd_len);
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
        if (src_info->sta_max_conn_retry_set) {
            dst->wifi_conn_report.extra_info->sta_max_conn_retry_set = src_info->sta_max_conn_retry_set;
            dst->wifi_conn_report.extra_info->sta_max_conn_retry = src_info->sta_max_conn_retry;
            dst->wifi_conn_report.extra_info_len += (1 + 2);
        }
        if (src_info->sta_conn_end_reason_set) {
            dst->wifi_conn_report.extra_info->sta_conn_end_reason_set = src_info->sta_conn_end_reason_set;
            dst->wifi_conn_report.extra_info->sta_conn_end_reason = src_info->sta_conn_end_reason;
            dst->wifi_conn_report.extra_info_len += (1 + 2);
        }
        if (src_info->sta_conn_rssi_set) {
            dst->wifi_conn_report.extra_info->sta_conn_rssi_set = src_info->sta_conn_rssi_set;
            dst->wifi_conn_report.extra_info->sta_conn_rssi = src_info->sta_conn_rssi;
            dst->wifi_conn_report.extra_info_len += (1 + 2);
        }
        break;
    }
    case BTC_BLUFI_ACT_SEND_WIFI_LIST:{
        esp_blufi_ap_record_t *list = src->wifi_list.list;
        src->wifi_list.list = NULL;
        if (list == NULL || src->wifi_list.apCount <= 0) {
            break;
        }
        dst->wifi_list.list = (esp_blufi_ap_record_t *)osi_malloc(sizeof(esp_blufi_ap_record_t) * src->wifi_list.apCount);
        if (dst->wifi_list.list == NULL) {
            break;
        }
        memcpy(dst->wifi_list.list, list, sizeof(esp_blufi_ap_record_t) * src->wifi_list.apCount);
        break;
    }
    case BTC_BLUFI_ACT_SEND_CUSTOM_DATA:{
        uint8_t *data = src->custom_data.data;
        if(data == NULL) {
            BTC_TRACE_ERROR("custom data is NULL\n");
            break;
        }
        dst->custom_data.data = osi_malloc(src->custom_data.data_len);
        if(dst->custom_data.data == NULL) {
            BTC_TRACE_ERROR("custom data malloc error\n");
            break;
        }
        memcpy(dst->custom_data.data, src->custom_data.data, src->custom_data.data_len);
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
            osi_free(info->sta_ssid);
        }
        if (info->sta_passwd) {
            osi_free(info->sta_passwd);
        }
        if (info->softap_ssid) {
            osi_free(info->softap_ssid);
        }
        if (info->softap_passwd) {
            osi_free(info->softap_passwd);
        }
        osi_free(info);
        break;
    }
    case BTC_BLUFI_ACT_SEND_WIFI_LIST:{
        esp_blufi_ap_record_t *list = (esp_blufi_ap_record_t *)arg->wifi_list.list;
        if (list){
            osi_free(list);
        }
        break;
    }
    case BTC_BLUFI_ACT_SEND_CUSTOM_DATA:{
        uint8_t *data = arg->custom_data.data;
        if(data) {
            osi_free(data);
        }
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
    case BTC_BLUFI_ACT_SEND_WIFI_LIST:{
        btc_blufi_send_wifi_list(arg->wifi_list.apCount, arg->wifi_list.list);
        break;
    }
    case BTC_BLUFI_ACT_SEND_ERR_INFO:
        btc_blufi_send_error_info(arg->blufi_err_infor.state);
        break;
    case BTC_BLUFI_ACT_SEND_CUSTOM_DATA:
        btc_blufi_send_custom_data(arg->custom_data.data, arg->custom_data.data_len);
        break;
    default:
        BTC_TRACE_ERROR("%s UNKNOWN %d\n", __func__, msg->act);
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

#endif  ///BLUFI_INCLUDED == TRUE

/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "osi/allocator.h"

#include "bta/bta_api.h"
#include "bta/bta_dm_co.h"
#include "btc/btc_task.h"
#include "btc/btc_util.h"
#if (BLE_FEAT_CTE_EN == TRUE)
#include "btc_ble_cte.h"

static inline void btc_cte_ble_cb_to_app(esp_ble_cte_cb_event_t event, esp_ble_cte_cb_param_t *param)
{
    esp_ble_cte_cb_t btc_cte_ble_cb = (esp_ble_cte_cb_t)btc_profile_cb_get(BTC_PID_BLE_CTE);
    if (btc_cte_ble_cb) {
        btc_cte_ble_cb(event, param);
    }
}

static void btc_ble_cte_callback(tBTM_BLE_CTE_EVENT event,
                                                tBTM_BLE_CTE_CB_PARAMS *params)
{
    esp_ble_cte_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_BLE_CTE;

    BTC_TRACE_DEBUG("%s event %d\n", __func__, event);

    switch(event) {
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
        case BTA_BLE_CTE_SET_TRANS_PARAMS_EVT:
            msg.act = ESP_BLE_CTE_SET_CONNLESS_TRANS_PARAMS_CMPL_EVT;
            param.set_trans_params_cmpl.status = btc_btm_status_to_esp_status(params->cte_trans_params_cmpl.status);
            break;
        case BTA_BLE_CTE_SET_TRANS_ENABLE_EVT:
            msg.act = ESP_BLE_CTE_SET_CONNLESS_TRANS_ENABLE_CMPL_EVT;
            param.set_trans_enable_cmpl.status = btc_btm_status_to_esp_status(params->cte_trans_params_cmpl.status);
            break;
        case BTA_BLE_CTE_SET_IQ_SAMP_ENABLE_EVT:
            msg.act = ESP_BLE_CTE_SET_CONNLESS_IQ_SAMPLING_ENABLE_CMPL_EVT;
            param.iq_sampling_enable_cmpl.status = btc_btm_status_to_esp_status(params->cte_iq_samp_en_cmpl.status);
            param.iq_sampling_enable_cmpl.sync_handle = params->cte_iq_samp_en_cmpl.sync_handle;
            break;
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
        case BTA_BLE_CTE_SET_CONN_RECV_PARAMS_EVT:
            msg.act = ESP_BLE_CTE_SET_CONN_RECV_PARAMS_CMPL_EVT;
            param.conn_recv_params_cmpl.status = btc_btm_status_to_esp_status(params->cte_recv_params_cmpl.status);
            param.conn_recv_params_cmpl.conn_handle = params->cte_recv_params_cmpl.conn_handle;
            break;
        case BTA_BLE_CTE_SET_CONN_TRANS_PARAMS_EVT:
            msg.act = ESP_BLE_CTE_SET_CONN_TRANS_PARAMS_CMPL_EVT;
            param.conn_trans_params_cmpl.status = btc_btm_status_to_esp_status(params->cte_conn_trans_params_cmpl.status);
            param.conn_trans_params_cmpl.conn_handle = params->cte_conn_trans_params_cmpl.conn_handle;
            break;
        case BTA_BLE_CTE_SET_CONN_REQ_ENABLE_EVT:
            msg.act = ESP_BLE_CTE_SET_CONN_REQ_ENABLE_CMPL_EVT;
            param.conn_req_en_cmpl.status = btc_btm_status_to_esp_status(params->cte_conn_req_en_cmpl.status);
            param.conn_req_en_cmpl.conn_handle = params->cte_conn_req_en_cmpl.conn_handle;
            break;
        case BTA_BLE_CTE_SET_CONN_RSP_ENABLE_EVT:
            msg.act = ESP_BLE_CTE_SET_CONN_RSP_ENABLE_CMPL_EVT;
            param.conn_rsp_en_cmpl.status = btc_btm_status_to_esp_status(params->cte_conn_rsp_en_cmpl.status);
            param.conn_rsp_en_cmpl.conn_handle = params->cte_conn_rsp_en_cmpl.conn_handle;
            break;
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
        case BTA_BLE_CTE_READ_ANT_INFOR_EVT:
            msg.act = ESP_BLE_CTE_READ_ANT_INFOR_CMPL_EVT;
            param.read_ant_infor_cmpl.status = btc_btm_status_to_esp_status(params->cte_read_ant_infor_cmpl.status);
            param.read_ant_infor_cmpl.supported_switching_sampling_rates = params->cte_read_ant_infor_cmpl.supported_switching_sampling_rates;
            param.read_ant_infor_cmpl.num_antennae = params->cte_read_ant_infor_cmpl.num_ant;
            param.read_ant_infor_cmpl.max_switching_pattern_len = params->cte_read_ant_infor_cmpl.max_switching_pattern_len;
            param.read_ant_infor_cmpl.max_cte_len = params->cte_read_ant_infor_cmpl.max_cte_len;
            break;
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
        case BTA_BLE_CTE_CONNLESS_IQ_REPORT_EVT:
            msg.act = ESP_BLE_CTE_CONNLESS_IQ_REPORT_EVT;
            param.connless_iq_rpt.sync_handle = params->cte_connless_iq_rpt.sync_handle;
            param.connless_iq_rpt.channel_idx = params->cte_connless_iq_rpt.channel_idx;
            param.connless_iq_rpt.rssi = params->cte_connless_iq_rpt.rssi;
            param.connless_iq_rpt.rssi_ant_id = params->cte_connless_iq_rpt.rssi_ant_id;
            param.connless_iq_rpt.cte_type = params->cte_connless_iq_rpt.cte_type;
            param.connless_iq_rpt.slot_dur = params->cte_connless_iq_rpt.slot_dur;
            param.connless_iq_rpt.pkt_status = params->cte_connless_iq_rpt.pkt_status;
            param.connless_iq_rpt.periodic_evt_counter = params->cte_connless_iq_rpt.periodic_evt_counter;
            param.connless_iq_rpt.sample_count = params->cte_connless_iq_rpt.sample_count;
            param.connless_iq_rpt.i_sample = &(params->cte_connless_iq_rpt.i_sample[0]);
            param.connless_iq_rpt.q_sample = &(params->cte_connless_iq_rpt.q_sample[0]);
            break;
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
        case BTA_BLE_CTE_CONN_IQ_REPORT_EVT:
            msg.act = ESP_BLE_CTE_CONN_IQ_REPORT_EVT;
            param.conn_iq_rpt.conn_handle = params->cte_conn_iq_rpt.conn_handle;
            param.conn_iq_rpt.data_channel_idx = params->cte_conn_iq_rpt.data_channel_idx;
            param.conn_iq_rpt.rssi = params->cte_conn_iq_rpt.rssi;
            param.conn_iq_rpt.rssi_ant_id = params->cte_conn_iq_rpt.rssi_ant_id;
            param.conn_iq_rpt.cte_type = params->cte_conn_iq_rpt.cte_type;
            param.conn_iq_rpt.slot_dur = params->cte_conn_iq_rpt.slot_dur;
            param.conn_iq_rpt.pkt_status = params->cte_conn_iq_rpt.pkt_status;
            param.conn_iq_rpt.conn_evt_counter = params->cte_conn_iq_rpt.conn_evt_counter;
            param.conn_iq_rpt.sample_count = params->cte_conn_iq_rpt.sample_count;
            param.conn_iq_rpt.i_sample = &params->cte_conn_iq_rpt.i_sample[0];
            param.conn_iq_rpt.q_sample = &params->cte_conn_iq_rpt.q_sample[0];
            break;
        case BTA_BLE_CTE_REQUEST_FAILED_EVT:
            msg.act = ESP_BLE_CTE_REQUEST_FAILED_EVT;
            param.req_failed_evt.reason = btc_btm_status_to_esp_status(params->cte_req_failed.status);
            param.req_failed_evt.conn_handle = params->cte_req_failed.conn_handle;
            break;
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
        default:
            break;
    }

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_cte_cb_param_t), btc_ble_cte_cb_deep_copy, btc_ble_cte_cb_deep_free);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

void btc_ble_cte_cb_handler(btc_msg_t *msg)
{
    BTC_TRACE_DEBUG("%s act %d \n", __func__, msg->act);
    esp_ble_cte_cb_param_t *param = (esp_ble_cte_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_CTE_EVT_MAX) {
        btc_cte_ble_cb_to_app(msg->act, param);
    } else {
        BTC_TRACE_ERROR("%s, unknown msg->act = %d", __func__, msg->act);
    }

    btc_ble_cte_cb_deep_free(msg);

}

void btc_ble_cte_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
#if ((BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE) || (BLE_FEAT_CTE_CONNECTION_EN == TRUE))
    btc_ble_cte_args_t *src = (btc_ble_cte_args_t *)p_src;
    btc_ble_cte_args_t *dst = (btc_ble_cte_args_t *)p_dest;
#endif // #if ((BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE) || (BLE_FEAT_CTE_CONNECTION_EN == TRUE))
    switch (msg->act) {
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
    case BTC_CTE_ACT_SET_TRANS_PARAMS:
        if (src->cte_trans_params.switching_pattern_len && src->cte_trans_params.antenna_ids) {
            dst->cte_trans_params.antenna_ids = osi_malloc(src->cte_trans_params.switching_pattern_len);
            if (dst->cte_trans_params.antenna_ids) {
                memcpy(dst->cte_trans_params.antenna_ids, src->cte_trans_params.antenna_ids, src->cte_trans_params.switching_pattern_len);
            } else {
                BTC_TRACE_ERROR("%s %d no mem\n",__func__, msg->act);
            }
        }
        break;
    case BTC_CTE_ACT_SET_IQ_SAMPLING_EN:
        if (src->cte_iq_sampling_en.switching_pattern_len && src->cte_iq_sampling_en.antenna_ids) {
            dst->cte_iq_sampling_en.antenna_ids = osi_malloc(src->cte_iq_sampling_en.switching_pattern_len);
            if (dst->cte_iq_sampling_en.antenna_ids) {
                memcpy(dst->cte_iq_sampling_en.antenna_ids, src->cte_iq_sampling_en.antenna_ids, src->cte_iq_sampling_en.switching_pattern_len);
            } else {
                BTC_TRACE_ERROR("%s %d no mem\n",__func__, msg->act);
            }
        }
        break;
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)

#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    case BTC_CTE_ACT_SET_CONN_CTE_RECV_PARAMS:
        if (src->cte_recv_params.switching_pattern_len && src->cte_recv_params.antenna_ids) {
            dst->cte_recv_params.antenna_ids = osi_malloc(src->cte_recv_params.switching_pattern_len);
            if (dst->cte_recv_params.antenna_ids) {
                memcpy(dst->cte_recv_params.antenna_ids, src->cte_recv_params.antenna_ids, src->cte_recv_params.switching_pattern_len);
            } else {
                BTC_TRACE_ERROR("%s %d no mem\n",__func__, msg->act);
            }
        }
        break;
    case BTC_CTE_ACT_SET_CONN_CTE_TRANS_PARAMS:
        if (src->cte_conn_trans_params.switching_pattern_len && src->cte_conn_trans_params.antenna_ids) {
            dst->cte_conn_trans_params.antenna_ids = osi_malloc(src->cte_conn_trans_params.switching_pattern_len);
            if (dst->cte_conn_trans_params.antenna_ids) {
                memcpy(dst->cte_conn_trans_params.antenna_ids, src->cte_conn_trans_params.antenna_ids, src->cte_conn_trans_params.switching_pattern_len);
            } else {
                BTC_TRACE_ERROR("%s %d no mem\n",__func__, msg->act);
            }
        }
        break;
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    default:
        BTC_TRACE_ERROR("Unhandled deep copy %d\n", msg->act);
        break;
    }
}

void btc_ble_cte_arg_deep_free(btc_msg_t *msg)
{
    BTC_TRACE_DEBUG("%s \n", __func__);
    switch (msg->act) {
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
    case BTC_CTE_ACT_SET_TRANS_PARAMS: {
        uint8_t *antenna_ids = ((btc_ble_cte_args_t *)msg->arg)->cte_trans_params.antenna_ids;
        if (antenna_ids) {
            osi_free(antenna_ids);
        }
        break;
    }
    case BTC_CTE_ACT_SET_IQ_SAMPLING_EN: {
        uint8_t *antenna_ids = ((btc_ble_cte_args_t *)msg->arg)->cte_iq_sampling_en.antenna_ids;
        if (antenna_ids) {
            osi_free(antenna_ids);
        }
        break;
    }
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    case BTC_CTE_ACT_SET_CONN_CTE_RECV_PARAMS: {
        uint8_t *antenna_ids = ((btc_ble_cte_args_t *)msg->arg)->cte_recv_params.antenna_ids;
        if (antenna_ids) {
            osi_free(antenna_ids);
        }
        break;
    }
    case BTC_CTE_ACT_SET_CONN_CTE_TRANS_PARAMS: {
        uint8_t *antenna_ids = ((btc_ble_cte_args_t *)msg->arg)->cte_conn_trans_params.antenna_ids;
        if (antenna_ids) {
            osi_free(antenna_ids);
        }
        break;
    }
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    default:
        BTC_TRACE_DEBUG("Unhandled deep free %d\n", msg->act);
        break;
    }
}

void btc_ble_cte_cb_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_cte_cb_param_t *src = (esp_ble_cte_cb_param_t *)p_src;
    esp_ble_cte_cb_param_t  *dst = (esp_ble_cte_cb_param_t *) p_dest;

    switch (msg->act) {
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
    case ESP_BLE_CTE_CONNLESS_IQ_REPORT_EVT:
        if (src->connless_iq_rpt.sample_count) {
            dst->connless_iq_rpt.i_sample = osi_malloc(src->connless_iq_rpt.sample_count);
            if (dst->connless_iq_rpt.i_sample) {
                memcpy(dst->connless_iq_rpt.i_sample, &(src->connless_iq_rpt.i_sample[0]),
                   src->connless_iq_rpt.sample_count);
            } else {
                BTC_TRACE_ERROR("%s, i_sample malloc failed\n", __func__);
            }
            dst->connless_iq_rpt.q_sample = osi_malloc(src->connless_iq_rpt.sample_count);
            if (dst->connless_iq_rpt.q_sample) {
                memcpy(dst->connless_iq_rpt.q_sample, &(src->connless_iq_rpt.q_sample[0]),
                   src->connless_iq_rpt.sample_count);
            } else {
                BTC_TRACE_ERROR("%s, q_sample malloc failed\n", __func__);
            }
        }
        break;
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    case ESP_BLE_CTE_CONN_IQ_REPORT_EVT:
        if (src->conn_iq_rpt.sample_count) {
            dst->conn_iq_rpt.i_sample = osi_malloc(src->conn_iq_rpt.sample_count);
            if (dst->conn_iq_rpt.i_sample) {
                memcpy(dst->conn_iq_rpt.i_sample, src->conn_iq_rpt.i_sample,
                   src->conn_iq_rpt.sample_count);
            } else {
                BTC_TRACE_ERROR("%s, i_sample malloc failed\n", __func__);
            }
            dst->conn_iq_rpt.q_sample = osi_malloc(src->conn_iq_rpt.sample_count);
            if (dst->conn_iq_rpt.q_sample) {
                memcpy(dst->conn_iq_rpt.q_sample, src->conn_iq_rpt.q_sample,
                   src->conn_iq_rpt.sample_count);
            } else {
                BTC_TRACE_ERROR("%s, q_sample malloc failed\n", __func__);
            }
        }
        break;
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    default:
       BTC_TRACE_DEBUG("%s, Unhandled deep copy %d\n", __func__, msg->act);
       break;
    }
}

void btc_ble_cte_cb_deep_free(btc_msg_t *msg)
{
    BTC_TRACE_DEBUG("%s", __func__);
    switch (msg->act) {
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
        case ESP_BLE_CTE_CONNLESS_IQ_REPORT_EVT: {
            esp_ble_cte_cb_param_t *params = (esp_ble_cte_cb_param_t *)msg->arg;
            uint8_t *i_sample = &(params->connless_iq_rpt.i_sample[0]);
            uint8_t *q_sample = &(params->connless_iq_rpt.q_sample[0]);
            if (i_sample) {
                osi_free(i_sample);
            }
            if (q_sample) {
                osi_free(q_sample);
            }
            break;
        }
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
        case ESP_BLE_CTE_CONN_IQ_REPORT_EVT: {
             esp_ble_cte_cb_param_t *params = (esp_ble_cte_cb_param_t *)msg->arg;
            uint8_t *i_sample = params->conn_iq_rpt.i_sample;
            uint8_t *q_sample = params->conn_iq_rpt.q_sample;
            if (i_sample) {
                osi_free(i_sample);
            }
            if (q_sample) {
                osi_free(q_sample);
            }
            break;
        }
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
        default:
            BTC_TRACE_DEBUG("Unhandled deep free %d", msg->act);
            break;
    }
}

void btc_ble_cte_call_handler(btc_msg_t *msg)
{
#if ((BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE) || (BLE_FEAT_CTE_CONNECTION_EN == TRUE))
    btc_ble_cte_args_t *arg = (btc_ble_cte_args_t *)msg->arg;
#endif // #if ((BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE) || (BLE_FEAT_CTE_CONNECTION_EN == TRUE))
    BTC_TRACE_DEBUG("%s act %d\n", __FUNCTION__, msg->act);

    switch (msg->act) {
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
    case BTC_CTE_ACT_SET_TRANS_PARAMS:
        struct cte_set_trans_params_arg *cte_trans_params = &arg->cte_trans_params;
        BTA_DmBleCteSetConnectionlessTransParams(cte_trans_params->adv_handle, cte_trans_params->cte_len, cte_trans_params->cte_type,
                                                    cte_trans_params->cte_count, cte_trans_params->switching_pattern_len, cte_trans_params->antenna_ids);
        break;
    case BTC_CTE_ACT_SET_TRANS_ENABLE:
        BTA_DmBleCteSetConnectionlessTransEnable(arg->cte_trans_enable.adv_handle, arg->cte_trans_enable.cte_enable);
        break;
    case BTC_CTE_ACT_SET_IQ_SAMPLING_EN:
        struct cte_iq_sampling_en_arg *iq_sampling_en = &arg->cte_iq_sampling_en;
        BTA_DmBleCteSetConnectionlessIqSamplingEnable(iq_sampling_en->sync_handle, iq_sampling_en->sampling_en, iq_sampling_en->slot_dur,
                                                    iq_sampling_en->max_sampled_ctes, iq_sampling_en->switching_pattern_len, iq_sampling_en->antenna_ids);
        break;
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    case BTC_CTE_ACT_SET_CONN_CTE_RECV_PARAMS:
        struct cte_recv_params_arg *recv_params = &arg->cte_recv_params;
        BTA_DmBleCteSetConnectionReceiveParams(recv_params->conn_handle, recv_params->sampling_en, recv_params->slot_dur,
                                            recv_params->switching_pattern_len, recv_params->antenna_ids);
        break;
    case BTC_CTE_ACT_SET_CONN_CTE_TRANS_PARAMS:
        struct cte_set_conn_trans_params_arg *trans_params = &arg->cte_conn_trans_params;
        BTA_DmBleCteSetConnectionTransParams(trans_params->conn_handle, trans_params->cte_types, trans_params->switching_pattern_len, trans_params->antenna_ids);
        break;
    case BTC_CTE_ACT_SET_CONN_CTE_REQUEST_EN:
        struct cte_req_en_arg *cte_req_en = &arg->cte_req_en;
        BTA_DmBleCteSetConnectionRequestEnable(cte_req_en->conn_handle, cte_req_en->enable, cte_req_en->cte_req_interval,
                                            cte_req_en->req_cte_len, cte_req_en->req_cte_Type);
        break;
    case BTC_CTE_ACT_SET_CONN_CTE_RESPONSE_EN:
        BTA_DmBleCteSetConnectionRspEnable(arg->cte_rsp_en.conn_handle, arg->cte_rsp_en.enable);
        break;
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
    case BTC_CTE_ACT_READ_ANTENNA_INFOR:
        BTA_DmBleCteReadAntInfor();
        break;
    default:
        break;
    }

    btc_ble_cte_arg_deep_free(msg);
}

//register connection parameter update callback
void btc_cte_callback_init(void)
{
    BTM_BleCteRegisterCallback(btc_ble_cte_callback);
}

#endif  ///BLE_FEAT_CTE_EN == TRUE

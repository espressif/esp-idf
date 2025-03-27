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
#if (BLE_FEAT_ISO_EN == TRUE)
#include "btc_iso_ble.h"
#include "esp_ble_iso_api.h"

static inline void btc_iso_ble_cb_to_app(esp_ble_iso_cb_event_t event, esp_ble_iso_cb_param_t *param)
{
    esp_ble_iso_cb_t btc_iso_ble_cb = (esp_ble_iso_cb_t)btc_profile_cb_get(BTC_PID_ISO_BLE);
    if (btc_iso_ble_cb) {
        btc_iso_ble_cb(event, param);
    }
}

static void btc_ble_iso_callback(tBTM_BLE_ISO_EVENT event,
                                                tBTM_BLE_ISO_CB_PARAMS *params)
{
    esp_ble_iso_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_ISO_BLE;

    switch(event) {
#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
        case BTA_BLE_ISO_BIG_CREATE_COMPLETE_EVT:
            msg.act = ESP_BLE_ISO_BIG_CREATE_CMPL_EVT;
            param.create_big_cmpl.status = btc_btm_status_to_esp_status(params->btm_big_cmpl.status);
            param.create_big_cmpl.big_handle = params->btm_big_cmpl.big_handle;
            param.create_big_cmpl.big_sync_delay = params->btm_big_cmpl.big_sync_delay;
            param.create_big_cmpl.transport_latency = params->btm_big_cmpl.transport_latency;
            param.create_big_cmpl.phy = params->btm_big_cmpl.phy;
            param.create_big_cmpl.nse = params->btm_big_cmpl.nse;
            param.create_big_cmpl.bn = params->btm_big_cmpl.bn;
            param.create_big_cmpl.pto = params->btm_big_cmpl.pto;
            param.create_big_cmpl.irc = params->btm_big_cmpl.irc;
            param.create_big_cmpl.max_pdu = params->btm_big_cmpl.max_pdu;
            param.create_big_cmpl.iso_interval = params->btm_big_cmpl.iso_interval;
            param.create_big_cmpl.num_bis = params->btm_big_cmpl.num_bis;
            for (uint8_t i = 0; i <  params->btm_big_cmpl.num_bis; i++)
            {
                param.create_big_cmpl.bis_handle[i] = params->btm_big_cmpl.bis_handle[i];
            }
            break;
        case BTA_BLE_ISO_BIG_TERMINATE_COMPLETE_EVT:
            msg.act = ESP_BLE_ISO_BIG_TERMINATE_CMPL_EVT;
            param.term_big_cmpl.status = btc_btm_status_to_esp_status(params->btm_big_term.status);
            param.term_big_cmpl.big_handle = params->btm_big_term.big_handle;
            param.term_big_cmpl.reason = params->btm_big_term.reason;
            break;
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
        case BTA_BLE_ISO_BIG_SYNC_ESTABLISHED_EVT:
            msg.act = ESP_BLE_ISO_BIG_SYNC_ESTABLISHED_EVT;
            param.big_sync_estab.status = btc_btm_status_to_esp_status(params->btm_big_sync_estab.status);
            param.big_sync_estab.big_handle = params->btm_big_sync_estab.big_handle;
            param.big_sync_estab.transport_latency = params->btm_big_sync_estab.transport_latency_big;
            param.big_sync_estab.nse = params->btm_big_sync_estab.nse;
            param.big_sync_estab.bn = params->btm_big_sync_estab.bn;
            param.big_sync_estab.pto = params->btm_big_sync_estab.pto;
            param.big_sync_estab.irc = params->btm_big_sync_estab.irc;
            param.big_sync_estab.max_pdu = params->btm_big_sync_estab.max_pdu;
            param.big_sync_estab.iso_interval = params->btm_big_sync_estab.iso_interval;
            param.big_sync_estab.num_bis = params->btm_big_sync_estab.num_bis;
            for (uint8_t  i = 0; i < params->btm_big_sync_estab.num_bis; i++)
            {
                param.big_sync_estab.bis_handle[i] = params->btm_big_sync_estab.bis_handle[i];
            }
            break;
        case BTA_BLE_ISO_BIG_SYNC_LOST_EVT:
            msg.act = ESP_BLE_ISO_BIG_SYNC_LOST_EVT;
            param.big_sync_lost.big_handle = params->btm_big_sync_lost.big_handle;
            param.big_sync_lost.reason = params->btm_big_sync_lost.reason;
            break;
        case BTA_BLE_ISO_BIG_SYNC_TERMINATE_COMPLETE_EVT:
            msg.act = ESP_BLE_ISO_BIG_SYNC_TERMINATE_CMPL_EVT;
            param.term_big_sync.status = btc_btm_status_to_esp_status(params->btm_big_sync_ter.status);
            param.term_big_sync.big_handle = params->btm_big_sync_ter.big_hdl;
            break;
        case BTA_BLE_ISO_BIGINFO_ADV_REPORT_EVT:
            msg.act = ESP_BLE_ISO_BIGINFO_ADV_REPORT_EVT;
            param.biginfo_report.sync_handle = params->btm_biginfo_report.sync_handle;
            param.biginfo_report.num_bis = params->btm_biginfo_report.num_bis;
            param.biginfo_report.nse = params->btm_biginfo_report.nse;
            param.biginfo_report.iso_interval = params->btm_biginfo_report.iso_interval;
            param.biginfo_report.bn = params->btm_biginfo_report.bn;
            param.biginfo_report.pto = params->btm_biginfo_report.pto;
            param.biginfo_report.irc = params->btm_biginfo_report.irc;
            param.biginfo_report.max_pdu = params->btm_biginfo_report.max_pdu;
            param.biginfo_report.sdu_interval = params->btm_biginfo_report.sdu_interval;
            param.biginfo_report.max_sdu = params->btm_biginfo_report.max_sdu;
            param.biginfo_report.phy = params->btm_biginfo_report.phy;
            param.biginfo_report.framing = params->btm_biginfo_report.framing;
            param.biginfo_report.encryption = params->btm_biginfo_report.encryption;
            break;
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
        case BTA_BLE_ISO_DATA_PATH_UPFATE_EVT:
            msg.act = ESP_BLE_ISO_ISO_DATA_PATH_UPDATE_EVT;
            param.data_path.op_type = params->btm_data_path_update.op_type;
            param.data_path.status = btc_btm_status_to_esp_status(params->btm_data_path_update.status);
            param.data_path.iso_hdl = params->btm_data_path_update.conn_hdl;
            break;
        case BTA_BLE_ISO_READ_TX_SYNC_EVT:
            msg.act = ESP_BLE_ISO_READ_ISO_TX_SYNC_CMPL_EVT;
            param.read_tx_sync.status = btc_btm_status_to_esp_status(params->btm_read_tx_sync.status);
            param.read_tx_sync.iso_hdl = params->btm_read_tx_sync.conn_hdl;
            param.read_tx_sync.pkt_seq_num = params->btm_read_tx_sync.pkt_seq_num;
            param.read_tx_sync.tx_time_stamp = params->btm_read_tx_sync.tx_time_stamp;
            param.read_tx_sync.time_offset = params->btm_read_tx_sync.time_offset;
            break;
        case BTA_BLE_ISO_READ_LINK_QUALITY_EVT:
            msg.act = ESP_BLE_ISO_READ_LINK_QUALITY_CMPL_EVT;
            param.read_link_quality.status = btc_btm_status_to_esp_status(params->btm_read_link_quality.status);
            param.read_link_quality.iso_hdl = params->btm_read_link_quality.conn_hdl;
            param.read_link_quality.tx_unacked_pkts = params->btm_read_link_quality.tx_unacked_pkts;
            param.read_link_quality.tx_flushed_pkts = params->btm_read_link_quality.tx_flushed_pkts;
            param.read_link_quality.tx_last_subevt_pkts = params->btm_read_link_quality.tx_last_subevt_pkts;
            param.read_link_quality.retransmitted_pkts = params->btm_read_link_quality.retransmitted_pkts;
            param.read_link_quality.crc_error_pkts = params->btm_read_link_quality.crc_error_pkts;
            param.read_link_quality.rx_unreceived_pkts = params->btm_read_link_quality.rx_unreceived_pkts;
            param.read_link_quality.duplicate_pkts = params->btm_read_link_quality.duplicate_pkts;
            break;
#if (BLE_FEAT_ISO_CIG_EN == TRUE)
        case BTA_BLE_ISO_CIS_ESTABLISHED_EVT:
            msg.act = ESP_BLE_ISO_CIS_ESTABLISHED_EVT;
            param.cis_estab.status = btc_btm_status_to_esp_status(params->btm_cis_established_evt.status);
            param.cis_estab.cis_handle = params->btm_cis_established_evt.conn_handle;
            param.cis_estab.cig_sync_delay = params->btm_cis_established_evt.cig_sync_delay;
            param.cis_estab.cis_sync_delay = params->btm_cis_established_evt.cis_sync_delay;
            param.cis_estab.trans_lat_c_to_p = params->btm_cis_established_evt.trans_lat_c_to_p;
            param.cis_estab.trans_lat_p_to_c = params->btm_cis_established_evt.trans_lat_p_to_c;
            param.cis_estab.phy_c_to_p = params->btm_cis_established_evt.phy_c_to_p;
            param.cis_estab.phy_p_to_c = params->btm_cis_established_evt.phy_p_to_c;
            param.cis_estab.nse = params->btm_cis_established_evt.nse;
            param.cis_estab.bn_c_to_p = params->btm_cis_established_evt.bn_c_to_p;
            param.cis_estab.bn_p_to_c = params->btm_cis_established_evt.bn_p_to_c;
            param.cis_estab.ft_c_to_p = params->btm_cis_established_evt.ft_c_to_p;
            param.cis_estab.ft_p_to_c = params->btm_cis_established_evt.ft_p_to_c;
            param.cis_estab.max_pdu_c_to_p = params->btm_cis_established_evt.max_pdu_c_to_p;
            param.cis_estab.max_pdu_p_to_c = params->btm_cis_established_evt.max_pdu_p_to_c;
            param.cis_estab.iso_interval = params->btm_cis_established_evt.iso_interval;
#if (BLE_FEAT_ISO_60_EN == TRUE)
            param.cis_estab.sub_interval = params->btm_cis_established_evt.sub_interval;
            param.cis_estab.max_sdu_c_to_p = params->btm_cis_established_evt.max_sdu_c_to_p;
            param.cis_estab.max_sdu_p_to_c = params->btm_cis_established_evt.max_sdu_p_to_c;
            param.cis_estab.sdu_int_c_to_p = params->btm_cis_established_evt.sdu_int_c_to_p;
            param.cis_estab.sdu_int_p_to_c = params->btm_cis_established_evt.sdu_int_p_to_c;
            param.cis_estab.framing = params->btm_cis_established_evt.framing;
#endif // #if (BLE_FEAT_ISO_60_EN == TRUE)
            break;
        case BTA_BLE_ISO_CIS_DISCONNECTED_EVT:
            msg.act = ESP_BLE_ISO_CIS_DISCONNECTED_EVT;
            param.cis_disconnected.cis_handle = params->btm_cis_disconnectd_evt.cis_handle;
            param.cis_disconnected.reason = params->btm_cis_disconnectd_evt.reason;
            break;
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
        case BTM_BLE_ISO_CIS_REQUEST_EVT:
            msg.act = ESP_BLE_ISO_CIS_REQUEST_EVT;
            param.cis_request.acl_handle = params->btm_cis_request_evt.acl_handle;
            param.cis_request.cis_handle = params->btm_cis_request_evt.cis_handle;
            param.cis_request.cig_id = params->btm_cis_request_evt.cig_id;
            param.cis_request.cis_id = params->btm_cis_request_evt.cis_id;
            break;
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
        case BTA_BLE_ISO_SET_CIG_PARAMS_EVT:
            msg.act = ESP_BLE_ISO_SET_CIG_PARAMS_CMPL_EVT;
            param.set_cig_params.status = btc_btm_status_to_esp_status(params->btm_set_cig_params.status);
            param.set_cig_params.cig_id = params->btm_set_cig_params.cig_id;
            param.set_cig_params.cis_count = params->btm_set_cig_params.cis_count;
            memcpy(&param.set_cig_params.cis_hdl[0], &params->btm_set_cig_params.conn_hdl[0], params->btm_set_cig_params.cis_count * 2);
            break;
        case BTA_BLE_ISO_CREATE_CIS_EVT:
            msg.act = ESP_BLE_ISO_CREATE_CIS_CMPL_EVT;
            param.create_cis.status = btc_btm_status_to_esp_status(params->status);
            break;
        case BTA_BLE_ISO_REMOVE_CIG_EVT:
            msg.act = ESP_BLE_ISO_REMOVE_CIG_CMPL_EVT;
            param.remove_cig.status = btc_btm_status_to_esp_status(params->btm_remove_cig.status);
            param.remove_cig.cig_id = params->btm_remove_cig.cig_id;
            break;
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
        case BTA_BLE_ISO_ACCEPT_CIS_REQ_EVT:
            msg.act = ESP_BLE_ISO_ACCEPT_CIS_REQ_CMPL_EVT;
            param.accept_req.status = btc_btm_status_to_esp_status(params->status);
            break;
        case BTA_BLE_ISO_REJECT_CIS_REQ_EVT:
            msg.act = ESP_BLE_ISO_REJECT_CIS_REQ_CMPL_EVT;
            param.reject_req.status = btc_btm_status_to_esp_status(params->btm_reject_cis_req.status);
            param.reject_req.cis_handle = params->btm_reject_cis_req.cis_handle;
            break;
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
        default:
            break;
    }

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_iso_cb_param_t), NULL, NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

void btc_iso_ble_cb_handler(btc_msg_t *msg)
{
    BTC_TRACE_DEBUG("%s act %d \n", __func__, msg->act);
    esp_ble_iso_cb_param_t *param = (esp_ble_iso_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_ISO_EVT_MAX) {
        btc_iso_ble_cb_to_app(msg->act, param);
    } else {
        BTC_TRACE_ERROR("%s, unknown msg->act = %d", __func__, msg->act);
    }

    btc_iso_ble_cb_deep_free(msg);

}

void btc_iso_ble_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_iso_args_t *src = (btc_ble_iso_args_t *)p_src;
    btc_ble_iso_args_t *dst = (btc_ble_iso_args_t *)p_dest;

    switch (msg->act) {
    case BTC_ISO_ACT_SET_DATA_PATH: {
        if (src->iso_set_data_path_params.codec_cfg_len && src->iso_set_data_path_params.codec_cfg) {
            dst->iso_set_data_path_params.codec_cfg = osi_malloc(src->iso_set_data_path_params.codec_cfg_len);
            if (dst->iso_set_data_path_params.codec_cfg) {
                memcpy(dst->iso_set_data_path_params.codec_cfg, src->iso_set_data_path_params.codec_cfg, src->iso_set_data_path_params.codec_cfg_len);
            } else {
                BTC_TRACE_ERROR("%s %d no mem\n",__func__, msg->act);
            }
        }
        break;
    }
    default:
        BTC_TRACE_ERROR("Unhandled deep copy %d\n", msg->act);
        break;
    }
}

void btc_iso_ble_cb_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    // esp_ble_iso_cb_param_t *src = (esp_ble_iso_cb_param_t *)p_src;
    // esp_ble_iso_cb_param_t  *dst = (esp_ble_iso_cb_param_t *) p_dest;

    switch (msg->act) {
    default:
       BTC_TRACE_ERROR("%s, Unhandled deep copy %d\n", __func__, msg->act);
       break;
    }
}

void btc_iso_ble_arg_deep_free(btc_msg_t *msg)
{
    BTC_TRACE_DEBUG("%s \n", __func__);
    switch (msg->act) {

    case BTC_ISO_ACT_SET_DATA_PATH: {
        uint8_t *p_codec_cfg = ((btc_ble_iso_args_t *)msg->arg)->iso_set_data_path_params.codec_cfg;
        if (p_codec_cfg) {
            osi_free(p_codec_cfg);
        }
        break;
    }
    default:
        BTC_TRACE_DEBUG("Unhandled deep free %d\n", msg->act);
        break;
    }
}

void btc_iso_ble_cb_deep_free(btc_msg_t *msg)
{
    BTC_TRACE_DEBUG("%s", __func__);
    switch (msg->act) {
        default:
            BTC_TRACE_DEBUG("Unhandled deep free %d", msg->act);
            break;
    }
}

void btc_iso_ble_call_handler(btc_msg_t *msg)
{
    btc_ble_iso_args_t *arg = (btc_ble_iso_args_t *)msg->arg;
    BTC_TRACE_DEBUG("%s act %d\n", __FUNCTION__, msg->act);

    switch (msg->act) {
#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
    case BTC_ISO_ACT_BIG_CREATE:
        BTA_DmBleGapIsoBigCreate((tBTA_DM_BLE_BIG_CREATE_PARAMS *)&arg->iso_big_creat_params);
        break;
    case BTC_ISO_ACT_BIG_CREATE_TEST:
        BTA_DmBleGapIsoBigCreateTest((tBTA_DM_BLE_BIG_CREATE_TEST_PARAMS *)&arg->iso_big_creat_test_params);
        break;
    case BTC_ISO_ACT_BIG_TERMINATE:
        BTA_DmBleGapIsoBigTerminate((tBTA_DM_BLE_BIG_TERMINATE_PARAMS *)&arg->iso_big_terminate_params);
        break;
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    case BTC_ISO_ACT_BIG_SYNC_CREATE:
        BTA_DmBleGapIsoBigSyncCreate((tBTA_DM_BLE_BIG_SYNC_CREATE_PARAMS *)&arg->iso_big_sync_creat_params);
        break;
    case BTC_ISO_ACT_BIG_SYNC_TERMINATE:
        BTA_DmBleGapIsoBigSyncTerminate((tBTA_DM_BLE_BIG_SYNC_TERMINATE_PARAMS *)&arg->iso_big_sync_terminate_params);
        break;
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
    case BTC_ISO_ACT_SET_DATA_PATH:
        BTA_DmBleGapIsoDataPathSet((tBTA_DM_BLE_ISO_SET_DATA_PATH_PARAMS *)&arg->iso_set_data_path_params);
        break;
    case BTC_ISO_ACT_REMOVE_DATA_PATH:
        BTA_DmBleGapIsoDataPathRemove((tBTA_DM_BLE_ISO_REMOVE_DATA_PATH_PARAMS *)&arg->iso_remove_data_path_params);
        break;
    case BTC_ISO_ACT_READ_ISO_TX_SYNC:
        BTA_DmBleGapIsoReadTxSync(arg->iso_read_tx_sync_params.iso_handle);
        break;
    case BTC_ISO_ACT_READ_ISO_LINK_QUALITY:
        break;
#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
    case BTC_ISO_ACT_SET_CIG_PARAMS: {
        struct set_cig_params_arg *set_cig_params = (struct set_cig_params_arg *)arg;
        BTA_DmBleIsoSetCigParams(set_cig_params->cig_id, set_cig_params->sdu_int_c_to_p, set_cig_params->sdu_int_p_to_c,
                                set_cig_params->worse_case_SCA, set_cig_params->packing, set_cig_params->framing,
                                set_cig_params->mtl_c_to_p, set_cig_params->mtl_p_to_c, set_cig_params->cis_cnt,
                                (uint8_t *)&set_cig_params->cis_params[0]);
        break;
    }
    case BTC_ISO_ACT_SET_CIG_PARAMS_TEST:
        struct set_cig_params_test_arg *set_cig_params_test = (struct set_cig_params_test_arg *)arg;
        BTA_DmBleIsoSetCigParamsTest(set_cig_params_test->cig_id, set_cig_params_test->sdu_int_c_to_p, set_cig_params_test->sdu_int_p_to_c,
                                set_cig_params_test->ft_c_to_p, set_cig_params_test->ft_p_to_c, set_cig_params_test->iso_interval,
                                set_cig_params_test->worse_case_SCA, set_cig_params_test->packing, set_cig_params_test->framing,
                                set_cig_params_test->cis_cnt, (uint8_t *)&set_cig_params_test->cis_params_test[0]);
        break;
    case BTC_ISO_ACT_CREATE_CIS: {
        struct creat_cis_arg * create_cis = (struct creat_cis_arg *)arg;
        BTA_DmBleIsoCreateCis(create_cis->cis_count, (uint8_t *)&create_cis->cis_hdls[0]);
        break;
    }
    case BTC_ISO_ACT_REMOVE_CIG:
        BTA_DmBleIsoRemoveCig(arg->remove_cig_params.cig_id);
        break;
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
    case BTC_ISO_ACT_ACCEPT_CIS_REQ:
        BTA_DmBleIsoAcceptCisReq(arg->accept_cis_req_params.cis_handle);
        break;
    case BTC_ISO_ACT_REJECT_CIS_REQ:
        BTA_DmBleIsoRejectCisReq(arg->reject_cis_req_params.cis_handle, arg->reject_cis_req_params.reason);
        break;
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
#if (BLE_FEAT_ISO_CIG_EN == TRUE)
    case BTC_ISO_ACT_DISCON_CIS:
        BTA_DmBleIsoDisconCis(arg->discon_cis_params.cis_handle, arg->discon_cis_params.reason);
        break;
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)
    default:
        break;
    }

    btc_iso_ble_arg_deep_free(msg);
}

//register connection parameter update callback
void btc_iso_callback_init(void)
{
    BTM_BleIsoRegisterCallback(btc_ble_iso_callback);
}

#endif  ///BLE_FEAT_ISO_EN == TRUE

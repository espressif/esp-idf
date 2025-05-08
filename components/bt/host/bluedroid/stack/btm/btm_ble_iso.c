/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "btm_int.h"
#include "stack/hcimsgs.h"
#include "osi/allocator.h"
#include "device/controller.h"
#include <string.h>
#include "l2c_int.h"

tBTM_BLE_ISO_CBACK ble_iso_cb;

extern void btm_ble_inter_set(bool extble_inter);

void BTM_BleIsoRegisterCallback(tBTM_BLE_ISO_CBACK cb)
{
    if (cb) {
        ble_iso_cb = cb;
    } else {
        BTM_TRACE_ERROR("%s, register fail, the cb function is NULL.", __func__);
    }
}

void BTM_IsoBleCallbackTrigger(tBTM_BLE_5_GAP_EVENT event, tBTM_BLE_ISO_CB_PARAMS *params)
{
    if(params && params->status == BTM_SUCCESS) {
        btm_ble_inter_set(true);
    }
    if (ble_iso_cb) {
        ble_iso_cb(event, params);
    }
}

void btm_ble_iso_read_iso_tx_sync_complete(UINT8 *p)
{
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};
    STREAM_TO_UINT8(cb_params.btm_read_tx_sync.status, p);
    STREAM_TO_UINT16(cb_params.btm_read_tx_sync.conn_hdl, p);
    STREAM_TO_UINT16(cb_params.btm_read_tx_sync.pkt_seq_num, p);
    STREAM_TO_UINT32(cb_params.btm_read_tx_sync.tx_time_stamp, p);
    STREAM_TO_UINT24(cb_params.btm_read_tx_sync.time_offset, p);

    if (cb_params.btm_read_tx_sync.status != HCI_SUCCESS) {
        cb_params.btm_read_tx_sync.status = (BTM_HCI_ERROR | cb_params.btm_read_tx_sync.status);
    }
    cb_params.btm_read_tx_sync.conn_hdl = (cb_params.btm_read_tx_sync.conn_hdl & 0xEFF);
    cb_params.btm_read_tx_sync.time_offset = (cb_params.btm_read_tx_sync.time_offset & 0xFFFFFF);
    BTM_TRACE_DEBUG("read tx sync cmpl, status 0x%x conn_hdl 0x%x pkt_seq_num %d tx_time_stamp %ld time_offset %ld\n",
                    cb_params.btm_read_tx_sync.status, cb_params.btm_read_tx_sync.conn_hdl, cb_params.btm_read_tx_sync.pkt_seq_num,
                    cb_params.btm_read_tx_sync.tx_time_stamp, cb_params.btm_read_tx_sync.time_offset);

    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_READ_TX_SYNC_EVT, &cb_params);
}

void btm_ble_iso_read_iso_link_quality_complete(UINT8 *p)
{
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};
    STREAM_TO_UINT8(cb_params.btm_read_link_quality.status, p);
    STREAM_TO_UINT16(cb_params.btm_read_link_quality.conn_hdl, p);
    STREAM_TO_UINT32(cb_params.btm_read_link_quality.tx_unacked_pkts, p);
    STREAM_TO_UINT32(cb_params.btm_read_link_quality.tx_flushed_pkts, p);
    STREAM_TO_UINT32(cb_params.btm_read_link_quality.tx_last_subevt_pkts, p);
    STREAM_TO_UINT32(cb_params.btm_read_link_quality.retransmitted_pkts, p);
    STREAM_TO_UINT32(cb_params.btm_read_link_quality.crc_error_pkts, p);
    STREAM_TO_UINT32(cb_params.btm_read_link_quality.rx_unreceived_pkts, p);
    STREAM_TO_UINT32(cb_params.btm_read_link_quality.duplicate_pkts, p);

    if (cb_params.btm_read_link_quality.status != HCI_SUCCESS) {
        cb_params.btm_read_link_quality.status = (BTM_HCI_ERROR | cb_params.btm_read_link_quality.status);
    }

    BTM_TRACE_DEBUG("read link quality cmpl, status 0x%x conn_hdl 0x%x tx_unacked_pkts %d tx_flushed_pkts %ld tx_last_subevt_pkts %ld retransmitted_pkts %d\
                    crc_error_pkts %d rx_unreceived_pkts %d duplicate_pkts %d\n",
                    cb_params.btm_read_link_quality.status, cb_params.btm_read_link_quality.conn_hdl, cb_params.btm_read_link_quality.tx_unacked_pkts,
                    cb_params.btm_read_link_quality.tx_flushed_pkts, cb_params.btm_read_link_quality.tx_last_subevt_pkts, cb_params.btm_read_link_quality.retransmitted_pkts,
                    cb_params.btm_read_link_quality.crc_error_pkts, cb_params.btm_read_link_quality.rx_unreceived_pkts, cb_params.btm_read_link_quality.duplicate_pkts);

    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_READ_LINK_QUALITY_EVT, &cb_params);
}

#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
void btm_ble_iso_set_cig_params_complete(UINT8 *p)
{
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};

    STREAM_TO_UINT8(cb_params.btm_set_cig_params.status, p);
    STREAM_TO_UINT8(cb_params.btm_set_cig_params.cig_id, p);
    STREAM_TO_UINT8(cb_params.btm_set_cig_params.cis_count, p);
    for (uint8_t i = 0; i < cb_params.btm_set_cig_params.cis_count; i++)
    {
        STREAM_TO_UINT16(cb_params.btm_set_cig_params.conn_hdl[i], p);
        BTM_TRACE_DEBUG("i = %d, conn_hdl = %d", i, cb_params.btm_set_cig_params.conn_hdl[i]);
    }

    if (cb_params.btm_set_cig_params.status != HCI_SUCCESS) {
        cb_params.btm_set_cig_params.status = (BTM_HCI_ERROR | cb_params.btm_set_cig_params.status);
    }

    BTM_TRACE_DEBUG("set cig params, status 0x%x cig_id %d cis_count %d\n",
                    cb_params.btm_set_cig_params.status, cb_params.btm_set_cig_params.cig_id, cb_params.btm_set_cig_params.cis_count);

    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_SET_CIG_PARAMS_EVT, &cb_params);
}
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_EN == TRUE)
void btm_ble_cis_disconnected_evt(tBTM_BLE_CIS_DISCON_CMPL *params)
{
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};
    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }
    cb_params.btm_cis_disconnectd_evt.cis_handle = params->cis_handle;
    cb_params.btm_cis_disconnectd_evt.reason = params->reason;

    BTM_TRACE_DEBUG("btm cis disconnect, cis_handle %d reason 0x%x\n", params->cis_handle, params->reason);
    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_CIS_DISCONNECTED_EVT, &cb_params);
}

void btm_ble_cis_established_evt(tBTM_BLE_CIS_ESTABLISHED_CMPL *params)
{
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};
    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    if (params->status != HCI_SUCCESS) {
        params->status = (params->status | BTM_HCI_ERROR);
    }

    memcpy(&cb_params.btm_cis_established_evt, params, sizeof(tBTM_BLE_CIS_ESTABLISHED_CMPL));

    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_CIS_ESTABLISHED_EVT, &cb_params);
}
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
void btm_ble_cis_request_evt(tBTM_BLE_CIS_REQUEST_CMPL *params)
{
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};
    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }
    memcpy(&cb_params.btm_cis_request_evt, params, sizeof(tBTM_BLE_CIS_REQUEST_CMPL));

    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_CIS_REQUEST_EVT, &cb_params);
}
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)

#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
void btm_ble_big_create_cmpl_evt(tBTM_BLE_BIG_CREATE_CMPL *params)
{
    BTM_TRACE_DEBUG("%s", __func__);
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};
    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    if (params->status != HCI_SUCCESS) {
        params->status = (params->status | BTM_HCI_ERROR);
    }

    cb_params.btm_big_cmpl.status = params->status;
    cb_params.btm_big_cmpl.big_handle = params->big_handle;
    cb_params.btm_big_cmpl.big_sync_delay = params->big_sync_delay;
    cb_params.btm_big_cmpl.transport_latency = params->transport_latency;
    cb_params.btm_big_cmpl.phy = params->phy;
    cb_params.btm_big_cmpl.nse = params->nse;
    cb_params.btm_big_cmpl.bn = params->bn;
    cb_params.btm_big_cmpl.pto = params->pto;
    cb_params.btm_big_cmpl.irc = params->irc;
    cb_params.btm_big_cmpl.max_pdu = params->max_pdu;
    cb_params.btm_big_cmpl.iso_interval = params->iso_interval;
    cb_params.btm_big_cmpl.num_bis = params->num_bis;
    // for (uint8_t i = 0; i < params->num_bis; i++)
    // {
    //     cb_params.btm_big_cmpl.bis_handle[i] = params->bis_handle[i];
    // }
    memcpy(&cb_params.btm_big_cmpl.bis_handle[0], &params->bis_handle[0], params->num_bis * 2);

    //memcpy(&cb_params.btm_big_cmpl, params, sizeof(tBTM_BLE_BIG_CREATE_CMPL));

    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_BIG_CREATE_COMPLETE_EVT, &cb_params);
}

void btm_ble_big_terminate_cmpl_evt(tBTM_BLE_BIG_TERMINATE_CMPL *params)
{
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};
    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }
    memcpy(&cb_params.btm_big_term, params, sizeof(tBTM_BLE_BIG_TERMINATE_CMPL));

    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_BIG_TERMINATE_COMPLETE_EVT, &cb_params);
}
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)

#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
void btm_ble_big_sync_estab_evt(tBTM_BLE_BIG_SYNC_ESTAB_CMPL *params)
{
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};
    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    if (params->status != HCI_SUCCESS) {
        params->status = (params->status | BTM_HCI_ERROR);
    }

    memcpy(&cb_params.btm_big_sync_estab, params, sizeof(tBTM_BLE_BIG_SYNC_ESTAB_CMPL));

    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_BIG_SYNC_ESTABLISHED_EVT, &cb_params);
}

void btm_ble_big_sync_terminate_complete(UINT8 hci_status, UINT8 big_handle)
{
    BTM_TRACE_DEBUG("%s hci_status 0x%x big_handle %d\n", __func__, hci_status, big_handle);
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};

    if (hci_status != HCI_SUCCESS) {
        hci_status = (hci_status | BTM_HCI_ERROR);
    }

    cb_params.status = hci_status;
    cb_params.btm_big_sync_ter.big_hdl = big_handle;
    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_BIG_SYNC_TERMINATE_COMPLETE_EVT, &cb_params);
}

void btm_ble_big_sync_lost_evt(tBTM_BLE_BIG_SYNC_LOST_EVT *params)
{
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};
    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    memcpy(&cb_params.btm_big_sync_lost, params, sizeof(tBTM_BLE_BIG_SYNC_LOST_EVT));

    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_BIG_SYNC_LOST_EVT, &cb_params);
}

void btm_ble_biginfo_adv_report_evt(tBTM_BLE_BIGINFO_ADV_REPORT_EVT *params)
{
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};
    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    memcpy(&cb_params.btm_biginfo_report, params, sizeof(tBTM_BLE_BIGINFO_ADV_REPORT_EVT));

    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_BIGINFO_ADV_REPORT_EVT, &cb_params);
}
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)

void btm_ble_iso_data_path_update_complete(UINT16 opcode, UINT8 hci_status, UINT16 conn_handle)
{
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};
    BTM_TRACE_DEBUG("data path update cmpl, opcode 0x%x status 0x%x conn_hdl %d\n", opcode, hci_status, conn_handle);
    if (hci_status != HCI_SUCCESS) {
        hci_status = (BTM_HCI_ERROR | hci_status);
    }
    cb_params.status = hci_status;
    cb_params.btm_data_path_update.op_type = ((opcode == HCI_BLE_ISO_SET_DATA_PATH) ? BTM_BLE_ISO_DATA_PATH_SETUP : BTM_BLE_ISO_DATA_PATH_REMOVE);
    cb_params.btm_data_path_update.conn_hdl = conn_handle;

    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_DATA_PATH_UPFATE_EVT, &cb_params);
}

#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
tBTM_STATUS BTM_BleBigCreate(uint8_t big_handle, uint8_t adv_handle, uint8_t num_bis,
                            uint32_t sdu_interval, uint16_t max_sdu, uint16_t max_transport_latency,
                            uint8_t rtn, uint8_t phy, uint8_t packing, uint8_t framing,
                            uint8_t encryption, uint8_t *broadcast_code)
{
    BTM_TRACE_API("big_handle %d adv_handle %d num_bis %d sdu_interval %d max_sdu %d max_transport_latency %d \
    rtn %d phy %d packing %d framing %d encryption %d broadcast_code %d", big_handle, adv_handle, num_bis, sdu_interval, max_sdu, max_transport_latency,\
    rtn, phy, packing, framing, encryption, broadcast_code);

    btsnd_hcic_ble_big_create(big_handle, adv_handle, num_bis, sdu_interval, max_sdu, max_transport_latency,
                              rtn, phy, packing, framing, encryption, broadcast_code);


    return BTM_SUCCESS;
}

tBTM_STATUS BTM_BleBigCreateTest(uint8_t big_handle, uint8_t adv_handle, uint8_t num_bis,
                                uint32_t sdu_interval, uint16_t iso_interval, uint8_t nse,
                                uint16_t max_sdu, uint16_t max_pdu, uint8_t phy,
                                uint8_t packing, uint8_t framing, uint8_t bn, uint8_t irc,
                                uint8_t pto, uint8_t encryption, uint8_t *broadcast_code)
{
    btsnd_hcic_ble_big_create_test(big_handle, adv_handle, num_bis, sdu_interval, iso_interval, nse,
                                    max_sdu, max_pdu, phy, packing, framing, bn, irc, pto, encryption,
                                    broadcast_code);
    return BTM_SUCCESS;
}

tBTM_STATUS BTM_BleBigTerminate(UINT8 big_handle, UINT8 reason)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_big_terminate(big_handle, reason)) != TRUE) {
        BTM_TRACE_ERROR("LE PA SyncCancel, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    if (status != BTM_SUCCESS) {
        cb_params.status = status;
        BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_BIG_TERMINATE_COMPLETE_EVT, &cb_params);
    }

    return status;
}
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)

#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
tBTM_STATUS BTM_BleBigSyncCreate(uint8_t big_handle, uint16_t sync_handle,
                                uint8_t encryption, uint8_t *bc_code,
                                uint8_t mse, uint16_t big_sync_timeout,
                                uint8_t num_bis, uint8_t *bis)
{
    btsnd_hcic_ble_big_sync_create(big_handle, sync_handle, encryption, bc_code,
                                    mse, big_sync_timeout, num_bis, bis);
    return BTM_SUCCESS;
}

tBTM_STATUS BTM_BleBigSyncTerminate(uint8_t big_handle)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;

    if ((err = btsnd_hcic_ble_big_sync_terminate(big_handle)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("LE big sync terminate, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    return status;
}
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)

tBTM_STATUS BTM_BleIsoSetDataPath(uint16_t conn_handle, uint8_t data_path_dir, uint8_t data_path_id, uint8_t coding_fmt,
                                    uint16_t company_id, uint16_t vs_codec_id, uint32_t controller_delay, uint8_t codec_len,
                                    uint8_t *codec_cfg)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;

    if (codec_len && (codec_cfg == NULL)) {
        tBTM_BLE_ISO_CB_PARAMS cb_params = {0};
        status = BTM_ILLEGAL_VALUE;
        cb_params.btm_data_path_update.op_type = BTM_BLE_ISO_DATA_PATH_SETUP;
        cb_params.status = status;
        cb_params.btm_data_path_update.conn_hdl = conn_handle;
        BTM_TRACE_ERROR("codec_cfg is NULL, set data path error");
        BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_DATA_PATH_UPFATE_EVT, &cb_params);
        return status;
    }

    BTM_TRACE_DEBUG("BTM_BleIsoSetDataPath conn_handle %d data_path_dir %d\n", conn_handle, data_path_dir);

    if ((err = btsnd_hcic_ble_iso_set_data_path(conn_handle, data_path_dir, data_path_id, coding_fmt, company_id, vs_codec_id,
                                                controller_delay, codec_len, codec_cfg)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("iso data path update, cmd err=0x%x", err);
        return BTM_HCI_ERROR | err;
    }

    return status;
}

tBTM_STATUS BTM_BleIsoRemoveDataPath(uint16_t conn_handle, uint8_t data_path_dir)
{
    tHCI_STATUS err = HCI_SUCCESS;

    BTM_TRACE_DEBUG("BTM_BleIsoRemoveDataPath conn_handle %d data_path_dir %d\n", conn_handle, data_path_dir);

    if ((err = btsnd_hcic_ble_iso_remove_data_path(conn_handle, data_path_dir)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("iso data path remove, cmd err=0x%x", err);
        return BTM_HCI_ERROR | err;
    }

    return BTM_SUCCESS;
}

tBTM_STATUS BTM_BleIsoReadTxSync(uint16_t iso_hdl)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;

    BTM_TRACE_DEBUG("BTM_BleIsoReadTxSync iso_hdl %d\n", iso_hdl);

    if ((err = btsnd_hcic_ble_iso_read_tx_sync(iso_hdl)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("iso read tx sync, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    return status;
}

tBTM_STATUS BTM_BleIsoReadLinkQuality(uint16_t iso_hdl)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;

    BTM_TRACE_DEBUG("BTM_BleIsoReadLinkQuality iso_hdl %d\n", iso_hdl);

    if ((err = btsnd_hcic_ble_iso_read_iso_link_quality(iso_hdl)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("iso read link quality, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    return status;
}

#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
tBTM_STATUS BTM_BleSetCigParams(uint8_t cig_id, uint32_t sdu_int_c_to_p, uint32_t sdu_int_p_to_c, uint8_t worse_case_SCA, uint8_t packing,
                                uint8_t framing, uint16_t mtl_c_to_p, uint16_t mtl_p_to_c, uint8_t cis_cnt, uint8_t *cis_params)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;

    if ((err = btsnd_hcic_ble_iso_set_cig_params(cig_id, sdu_int_c_to_p, sdu_int_p_to_c, worse_case_SCA, packing,
                                            framing, mtl_c_to_p, mtl_p_to_c, cis_cnt, (struct ble_hci_le_cis_params *)cis_params)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("iso set cig params, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    return status;
}

tBTM_STATUS BTM_BleSetCigParamsTest(uint8_t cig_id, uint32_t sdu_int_c_to_p, uint32_t sdu_int_p_to_c, uint8_t ft_c_to_p, uint8_t ft_p_to_c, uint16_t iso_interval,
                                uint8_t worse_case_SCA, uint8_t packing, uint8_t framing, uint8_t cis_cnt, uint8_t *cis_params)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;

    if ((err = btsnd_hcic_ble_iso_set_cig_params_test(cig_id, sdu_int_c_to_p, sdu_int_p_to_c,ft_c_to_p, ft_p_to_c, iso_interval,
                                                        worse_case_SCA, packing, framing, cis_cnt, (struct ble_hci_le_cis_params_test *)cis_params)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("iso set cig params test, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    return status;
}

void btm_ble_create_cis_cmd_status(tBTM_BLE_ISO_CB_PARAMS *cb_params)
{
    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_CREATE_CIS_EVT, cb_params);
}

tBTM_STATUS BTM_BleCreateCis(uint8_t cis_count, uint8_t *cis_hdls)
{
    btsnd_hcic_ble_iso_create_cis(cis_count, (struct ble_hci_cis_hdls *)cis_hdls);

    return BTM_SUCCESS;
}

tBTM_STATUS BTM_BleRemoveCig(uint8_t cig_id)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_iso_remove_cig(cig_id)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("iso remove cig, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.btm_remove_cig.status = status;
    cb_params.btm_remove_cig.cig_id = cig_id;
    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_REMOVE_CIG_EVT, &cb_params);

    return status;
}
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
void btm_ble_accept_cis_req_cmd_status(tBTM_BLE_ISO_CB_PARAMS *cb_params)
{
    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_ACCEPT_CIS_REQ_EVT, cb_params);
}

tBTM_STATUS BTM_BleAcceptCisReq(uint16_t cis_handle)
{
    btsnd_hcic_ble_iso_accept_cis_req(cis_handle);
    return BTM_SUCCESS;
}

tBTM_STATUS BTM_BleRejectCisReq(uint16_t cis_handle, uint8_t reason)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_ISO_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_iso_reject_cis_req(cis_handle, reason)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("iso reject cis req, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.btm_reject_cis_req.status = status;
    cb_params.btm_reject_cis_req.cis_handle = cis_handle;
    BTM_IsoBleCallbackTrigger(BTM_BLE_ISO_REJECT_CIS_REQ_EVT, &cb_params);
    return status;
}
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_EN == TRUE)
tBTM_STATUS BTM_BleDisconCis(uint16_t cis_handle, uint8_t reason)
{
    btsnd_hcic_disconnect(cis_handle, reason);

    return BTM_SUCCESS;
}
#endif // #if (BLE_FEAT_ISO_CIG_EN == TRUE)

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

tBTM_BLE_CTE_CBACK ble_cte_cb;

extern void btm_ble_inter_set(bool extble_inter);

void BTM_BleCteRegisterCallback(tBTM_BLE_CTE_CBACK cb)
{
    if (cb) {
        ble_cte_cb = cb;
    } else {
        BTM_TRACE_ERROR("%s, register fail, the cb function is NULL.", __func__);
    }
}

void BTM_CteBleCallbackTrigger(tBTM_BLE_5_GAP_EVENT event, tBTM_BLE_CTE_CB_PARAMS *params)
{
    BTM_TRACE_DEBUG("%s event %x", __func__, event);

    if(params && params->status == BTM_SUCCESS) {
        btm_ble_inter_set(true);
    }
    if (ble_cte_cb) {
        ble_cte_cb(event, params);
    }
}
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
tBTM_STATUS BTM_BleSetCteTransParams(uint8_t adv_handle, uint8_t cte_len, uint8_t cte_type, uint8_t cte_count, uint8_t switching_pattern_len, uint8_t *antenna_ids)
{


    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_CTE_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_set_connless_cte_trans_params(adv_handle, cte_len, cte_type, cte_count, switching_pattern_len, antenna_ids)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("cte set trans params, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.cte_trans_params_cmpl.status = status;
    BTM_CteBleCallbackTrigger(BTM_BLE_CTE_SET_TRANS_PARAMS_EVT, &cb_params);

    return status;
}

tBTM_STATUS BTM_BleCteSetConnectionlessTransEnable(uint8_t adv_handle, uint8_t cte_en)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_CTE_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_set_connless_cte_enable(adv_handle, cte_en)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("cte set trans enable, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.cte_trans_en_cmpl.status = status;
    BTM_CteBleCallbackTrigger(BTM_BLE_CTE_SET_TRANS_ENABLE_EVT, &cb_params);

    return status;
}

tBTM_STATUS BTM_BleCteSetConnectionlessIqSamplingEnable(uint16_t sync_handle, uint8_t sampling_en, uint8_t slot_dur,
                                                    uint8_t max_sampled_ctes, uint8_t switching_pattern_len, uint8_t *ant_ids)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_CTE_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_set_connless_iq_sampling_enable(sync_handle, sampling_en, slot_dur, max_sampled_ctes, switching_pattern_len, ant_ids)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("cte set trans enable, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.cte_iq_samp_en_cmpl.status = status;
    cb_params.cte_iq_samp_en_cmpl.sync_handle = sync_handle;
    BTM_CteBleCallbackTrigger(BTM_BLE_CTE_SET_IQ_SAMP_ENABLE_EVT, &cb_params);

    return status;
}
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)

#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
tBTM_STATUS BTM_BleCteSetConnectionReceiveParams(uint16_t conn_handle, uint8_t sampling_en, uint8_t slot_dur,
                                            uint8_t switching_pattern_len, uint8_t *ant_ids)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_CTE_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_set_conn_cte_receive_params(conn_handle, sampling_en, slot_dur, switching_pattern_len, ant_ids)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("cte set conn recv params, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.cte_recv_params_cmpl.status = status;
    cb_params.cte_recv_params_cmpl.conn_handle = conn_handle;
    BTM_CteBleCallbackTrigger(BTM_BLE_CTE_SET_CONN_RECV_PARAMS_EVT, &cb_params);

    return status;
}

tBTM_STATUS BTM_BleCteSetConnectionTransParams(uint16_t conn_handle, uint8_t cte_types, uint8_t switching_pattern_len, uint8_t *ant_ids)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_CTE_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_set_conn_cte_trans_params(conn_handle, cte_types, switching_pattern_len, ant_ids)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("cte set conn trans params, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.cte_conn_trans_params_cmpl.status = status;
    cb_params.cte_conn_trans_params_cmpl.conn_handle = conn_handle;
    BTM_CteBleCallbackTrigger(BTM_BLE_CTE_SET_CONN_TRANS_PARAMS_EVT, &cb_params);

    return status;
}

tBTM_STATUS BTM_BleCteSetConnectionRequestEnable(uint16_t conn_handle, uint8_t enable, uint16_t cte_req_int,
                                            uint8_t req_cte_len, uint8_t req_cte_type)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_CTE_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_conn_cte_req_enable(conn_handle, enable, cte_req_int, req_cte_len, req_cte_type)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("cte set conn req en, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.cte_conn_req_en_cmpl.status = status;
    cb_params.cte_conn_req_en_cmpl.conn_handle = conn_handle;
    BTM_CteBleCallbackTrigger(BTM_BLE_CTE_SET_CONN_REQ_ENABLE_EVT, &cb_params);

    return status;
}

tBTM_STATUS BTM_BleCteSetConnectionRspEnable(uint16_t conn_handle, uint8_t enable)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;
    tBTM_BLE_CTE_CB_PARAMS cb_params = {0};

    if ((err = btsnd_hcic_ble_conn_cte_rsp_enable(conn_handle, enable)) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("cte set conn rsp en, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    cb_params.cte_conn_rsp_en_cmpl.status = status;
    cb_params.cte_conn_rsp_en_cmpl.conn_handle = conn_handle;
    BTM_CteBleCallbackTrigger(BTM_BLE_CTE_SET_CONN_RSP_ENABLE_EVT, &cb_params);

    return status;
}
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)

tBTM_STATUS BTM_BleCteReadAntInfor(void)
{
    tHCI_STATUS err = HCI_SUCCESS;
    tBTM_STATUS status = BTM_SUCCESS;

    if ((err = btsnd_hcic_ble_read_antenna_info()) != HCI_SUCCESS) {
        BTM_TRACE_ERROR("cte read ant information, cmd err=0x%x", err);
        status = BTM_HCI_ERROR | err;
    }

    return status;
}

void btm_ble_cte_read_ant_infor_complete(UINT8 *p)
{
    tBTM_BLE_CTE_CB_PARAMS cb_params = {0};

    STREAM_TO_UINT8(cb_params.cte_read_ant_infor_cmpl.status, p);
    STREAM_TO_UINT8(cb_params.cte_read_ant_infor_cmpl.supported_switching_sampling_rates, p);
    STREAM_TO_UINT8(cb_params.cte_read_ant_infor_cmpl.num_ant, p);
    STREAM_TO_UINT8(cb_params.cte_read_ant_infor_cmpl.max_switching_pattern_len, p);
    STREAM_TO_UINT8(cb_params.cte_read_ant_infor_cmpl.max_cte_len, p);

    if (cb_params.cte_read_ant_infor_cmpl.status != HCI_SUCCESS) {
        cb_params.cte_read_ant_infor_cmpl.status = (BTM_HCI_ERROR | cb_params.cte_read_ant_infor_cmpl.status);
    }

    BTM_CteBleCallbackTrigger(BTM_BLE_CTE_READ_ANT_INFOR_EVT, &cb_params);
}

#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
void btm_ble_connless_iq_report_evt(tBTM_BLE_CTE_CONNLESS_IQ_REPORT_EVT *params)
{
    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    BTM_CteBleCallbackTrigger(BTM_BLE_CTE_CONNLESS_IQ_REPORT_EVT, (tBTM_BLE_CTE_CB_PARAMS *)params);
}
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)

#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
void btm_ble_conn_iq_report_evt(tBTM_BLE_CTE_CONN_IQ_REPORT_EVT *params)
{
    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    BTM_CteBleCallbackTrigger(BTM_BLE_CTE_CONN_IQ_REPORT_EVT, (tBTM_BLE_CTE_CB_PARAMS *)params);
}

void btm_ble_cte_req_failed_evt(tBTM_BLE_CTE_REQ_FAILED_EVT *params)
{
    if (!params) {
        BTM_TRACE_ERROR("%s, Invalid params.", __func__);
        return;
    }

    if (params->status != HCI_SUCCESS) {
        params->status = (params->status | BTM_HCI_ERROR);
    }

    BTM_CteBleCallbackTrigger(BTM_BLE_CTE_REQUEST_FAILED_EVT, (tBTM_BLE_CTE_CB_PARAMS *)params);
}
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)

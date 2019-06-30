/******************************************************************************
 *
 *  Copyright (C) 2014 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#include <string.h>
//#include <stdio.h>
#include <stddef.h>
#include "common/bt_target.h"

#include "stack/btm_ble_api.h"
#include "stack/bt_types.h"
//#include "bt_utils.h"
#include "stack/btu.h"
#include "btm_int.h"
#include "device/controller.h"
#include "stack/hcimsgs.h"

#if (BLE_INCLUDED == TRUE)

#if BTM_DYNAMIC_MEMORY == FALSE
tBTM_BLE_BATCH_SCAN_CB      ble_batchscan_cb;
tBTM_BLE_ADV_TRACK_CB       ble_advtrack_cb;
#else
tBTM_BLE_BATCH_SCAN_CB      *ble_batchscan_cb_ptr;
tBTM_BLE_ADV_TRACK_CB       *ble_advtrack_cb_ptr;
#define ble_batchscan_cb    (*ble_batchscan_cb_ptr)
#define ble_advtrack_cb     (*ble_advtrack_cb_ptr)
#endif

/* length of each batch scan command */
#define BTM_BLE_BATCH_SCAN_STORAGE_CFG_LEN      4
#define BTM_BLE_BATCH_SCAN_PARAM_CONFIG_LEN    12
#define BTM_BLE_BATCH_SCAN_ENB_DISB_LEN         2
#define BTM_BLE_BATCH_SCAN_READ_RESULTS_LEN     2

#define BTM_BLE_BATCH_SCAN_CB_EVT_MASK       0xF0
#define BTM_BLE_BATCH_SCAN_SUBCODE_MASK      0x0F

/*******************************************************************************
**  Local functions
*******************************************************************************/
void btm_ble_batchscan_vsc_cmpl_cback (tBTM_VSC_CMPL *p_params);
void btm_ble_batchscan_cleanup(void);

/*******************************************************************************
**
** Function         btm_ble_batchscan_filter_track_adv_vse_cback
**
** Description      VSE callback for batch scan, filter, and tracking events.
**
** Returns          None
**
*******************************************************************************/
void btm_ble_batchscan_filter_track_adv_vse_cback(UINT8 len, UINT8 *p)
{
    tBTM_BLE_TRACK_ADV_DATA adv_data;

    UINT8   sub_event = 0;
    tBTM_BLE_VSC_CB cmn_ble_vsc_cb;
    STREAM_TO_UINT8(sub_event, p);

    BTM_TRACE_EVENT("btm_ble_batchscan_filter_track_adv_vse_cback called with event:%x", sub_event);
    if (HCI_VSE_SUBCODE_BLE_THRESHOLD_SUB_EVT == sub_event &&
            NULL != ble_batchscan_cb.p_thres_cback) {
        ble_batchscan_cb.p_thres_cback(ble_batchscan_cb.ref_value);
        return;
    }

    if (HCI_VSE_SUBCODE_BLE_TRACKING_SUB_EVT == sub_event && NULL != ble_advtrack_cb.p_track_cback) {
        if (len < 10) {
            return;
        }

        memset(&adv_data, 0 , sizeof(tBTM_BLE_TRACK_ADV_DATA));
        BTM_BleGetVendorCapabilities(&cmn_ble_vsc_cb);
        adv_data.client_if = (UINT8)ble_advtrack_cb.ref_value;
        if (cmn_ble_vsc_cb.version_supported > BTM_VSC_CHIP_CAPABILITY_L_VERSION) {
            STREAM_TO_UINT8(adv_data.filt_index, p);
            STREAM_TO_UINT8(adv_data.advertiser_state, p);
            STREAM_TO_UINT8(adv_data.advertiser_info_present, p);
            STREAM_TO_BDADDR(adv_data.bd_addr.address, p);
            STREAM_TO_UINT8(adv_data.addr_type, p);

            /* Extract the adv info details */
            if (ADV_INFO_PRESENT == adv_data.advertiser_info_present) {
                STREAM_TO_UINT8(adv_data.tx_power, p);
                STREAM_TO_UINT8(adv_data.rssi_value, p);
                STREAM_TO_UINT16(adv_data.time_stamp, p);

                STREAM_TO_UINT8(adv_data.adv_pkt_len, p);
                if (adv_data.adv_pkt_len > 0) {
                    adv_data.p_adv_pkt_data = osi_malloc(adv_data.adv_pkt_len);
                    memcpy(adv_data.p_adv_pkt_data, p, adv_data.adv_pkt_len);
                }

                STREAM_TO_UINT8(adv_data.scan_rsp_len, p);
                if (adv_data.scan_rsp_len > 0) {
                    adv_data.p_scan_rsp_data = osi_malloc(adv_data.scan_rsp_len);
                    memcpy(adv_data.p_scan_rsp_data, p, adv_data.scan_rsp_len);
                }
            }
        } else {
            /* Based on L-release version */
            STREAM_TO_UINT8(adv_data.filt_index, p);
            STREAM_TO_UINT8(adv_data.addr_type, p);
            STREAM_TO_BDADDR(adv_data.bd_addr.address, p);
            STREAM_TO_UINT8(adv_data.advertiser_state, p);
        }

        BTM_TRACE_EVENT("track_adv_vse_cback called: %d, %d, %d", adv_data.filt_index,
                        adv_data.addr_type, adv_data.advertiser_state);
        ble_advtrack_cb.p_track_cback(&adv_data);
        return;
    }
}

/*******************************************************************************
**
** Function         btm_ble_batchscan_enq_op_q
**
** Description      enqueue a batchscan operation in q to check command complete
**                  status
**
** Returns          void
**
*******************************************************************************/
void btm_ble_batchscan_enq_op_q(UINT8 opcode, tBTM_BLE_BATCH_SCAN_STATE cur_state,
                                UINT8 cb_evt, tBTM_BLE_REF_VALUE ref_value)
{
    ble_batchscan_cb.op_q.sub_code[ble_batchscan_cb.op_q.next_idx] = (opcode | (cb_evt << 4));
    ble_batchscan_cb.op_q.cur_state[ble_batchscan_cb.op_q.next_idx] = cur_state;
    ble_batchscan_cb.op_q.ref_value[ble_batchscan_cb.op_q.next_idx] = ref_value;
    BTM_TRACE_DEBUG("btm_ble_batchscan_enq_op_q: subcode:%d, Cur_state:%d, ref_value:%d",
                    ble_batchscan_cb.op_q.sub_code[ble_batchscan_cb.op_q.next_idx],
                    ble_batchscan_cb.op_q.cur_state[ble_batchscan_cb.op_q.next_idx],
                    ble_batchscan_cb.op_q.ref_value[ble_batchscan_cb.op_q.next_idx]);
    ble_batchscan_cb.op_q.next_idx = (ble_batchscan_cb.op_q.next_idx + 1)
                                     % BTM_BLE_BATCH_SCAN_MAX;
}

/*******************************************************************************
**
** Function         btm_ble_batchscan_enq_rep_q
**
** Description      enqueue a batchscan report operation in q to check command complete
**                  status
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS btm_ble_batchscan_enq_rep_q(UINT8 report_format, tBTM_BLE_REF_VALUE ref_value)
{
    int i = 0;
    for (i = 0; i < BTM_BLE_BATCH_REP_MAIN_Q_SIZE; i++) {
        if (report_format == ble_batchscan_cb.main_rep_q.rep_mode[i]) {
            return BTM_ILLEGAL_VALUE;
        }
    }

    ble_batchscan_cb.main_rep_q.rep_mode[ble_batchscan_cb.main_rep_q.next_idx] = report_format;
    ble_batchscan_cb.main_rep_q.ref_value[ble_batchscan_cb.main_rep_q.next_idx] = ref_value;
    ble_batchscan_cb.main_rep_q.num_records[ble_batchscan_cb.main_rep_q.next_idx] = 0;
    ble_batchscan_cb.main_rep_q.data_len[ble_batchscan_cb.main_rep_q.next_idx] = 0;
    ble_batchscan_cb.main_rep_q.p_data[ble_batchscan_cb.main_rep_q.next_idx] = NULL;
    BTM_TRACE_DEBUG("btm_ble_batchscan_enq_rep_q: index:%d, rep %d, ref %d",
                    ble_batchscan_cb.main_rep_q.next_idx, report_format, ref_value);

    ble_batchscan_cb.main_rep_q.next_idx = (ble_batchscan_cb.main_rep_q.next_idx + 1)
                                           % BTM_BLE_BATCH_REP_MAIN_Q_SIZE;
    return BTM_SUCCESS;
}

/*******************************************************************************
**
** Function         btm_ble_batchscan_enq_rep_data
**
** Description      setup the data in the main report queue
**
** Returns          void
**
*******************************************************************************/
void btm_ble_batchscan_enq_rep_data(UINT8 report_format, UINT8 num_records, UINT8 *p_data,
                                    UINT8 data_len)
{
    int index = 0, len = 0;
    UINT8 *p_orig_data = NULL, *p_app_data = NULL;

    for (index = 0; index < BTM_BLE_BATCH_REP_MAIN_Q_SIZE; index++) {
        if (report_format == ble_batchscan_cb.main_rep_q.rep_mode[index]) {
            break;
        }
    }

    BTM_TRACE_DEBUG("btm_ble_batchscan_enq_rep_data: index:%d, rep %d, num %d len : %d",
                    index, report_format, num_records, data_len);

    if (index < BTM_BLE_BATCH_REP_MAIN_Q_SIZE && data_len > 0 && num_records > 0) {
        len = ble_batchscan_cb.main_rep_q.data_len[index];
        p_orig_data = ble_batchscan_cb.main_rep_q.p_data[index];
        if (NULL != p_orig_data) {
            p_app_data = osi_malloc(len + data_len);
            memcpy(p_app_data, p_orig_data, len);
            memcpy(p_app_data + len, p_data, data_len);
            osi_free(p_orig_data);
            ble_batchscan_cb.main_rep_q.p_data[index] = p_app_data;
            ble_batchscan_cb.main_rep_q.num_records[index] += num_records;
            ble_batchscan_cb.main_rep_q.data_len[index] += data_len;
        } else {
            p_app_data = osi_malloc(data_len);
            memcpy(p_app_data, p_data, data_len);
            ble_batchscan_cb.main_rep_q.p_data[index] = p_app_data;
            ble_batchscan_cb.main_rep_q.num_records[index] = num_records;
            ble_batchscan_cb.main_rep_q.data_len[index] = data_len;
        }
    }
}

/*******************************************************************************
**
** Function         btm_ble_batchscan_deq_rep_q
**
** Description      dequeue a batchscan report  in q when command complete
**                  is received
**
** Returns          void
**
*******************************************************************************/
void btm_ble_batchscan_deq_rep_data(UINT8 report_format, tBTM_BLE_REF_VALUE *p_ref_value,
                                    UINT8 *p_num_records, UINT8 **p_data, UINT16 *p_data_len)
{
    int index = 0;

    for (index = 0; index < BTM_BLE_BATCH_REP_MAIN_Q_SIZE; index++) {
        if (report_format == ble_batchscan_cb.main_rep_q.rep_mode[index]) {
            break;
        }
    }

    if (BTM_BLE_BATCH_REP_MAIN_Q_SIZE == index) {
        BTM_TRACE_ERROR("btm_ble_batchscan_deq_rep_data: rep_format:%d not found", report_format);
        return;
    }

    *p_num_records = ble_batchscan_cb.main_rep_q.num_records[index];
    *p_ref_value = ble_batchscan_cb.main_rep_q.ref_value[index];
    *p_data = ble_batchscan_cb.main_rep_q.p_data[index];
    *p_data_len = ble_batchscan_cb.main_rep_q.data_len[index];

    ble_batchscan_cb.main_rep_q.p_data[index] = NULL;
    ble_batchscan_cb.main_rep_q.data_len[index] = 0;
    ble_batchscan_cb.main_rep_q.rep_mode[index] = 0;
    ble_batchscan_cb.main_rep_q.ref_value[index] = 0;
    ble_batchscan_cb.main_rep_q.num_records[index] = 0;

    BTM_TRACE_DEBUG("btm_ble_batchscan_deq_rep_data: index:%d, rep %d, num %d, data_len %d",
                    index, report_format, *p_num_records, *p_data_len);

    ble_batchscan_cb.main_rep_q.pending_idx = (ble_batchscan_cb.main_rep_q.pending_idx + 1)
            % BTM_BLE_BATCH_SCAN_MAX;
}

/*******************************************************************************
**
** Function         btm_ble_batchscan_deq_op_q
**
** Description      dequeue a batch scan operation from q when command complete
**                  is received
**
** Returns          void
**
*******************************************************************************/
void btm_ble_batchscan_deq_op_q(UINT8 *p_opcode, tBTM_BLE_BATCH_SCAN_STATE *cur_state,
                                UINT8 *p_cb_evt, tBTM_BLE_REF_VALUE *p_ref)
{
    *p_cb_evt = (ble_batchscan_cb.op_q.sub_code[ble_batchscan_cb.op_q.pending_idx] >> 4);
    *p_opcode = (ble_batchscan_cb.op_q.sub_code[ble_batchscan_cb.op_q.pending_idx]
                 & BTM_BLE_BATCH_SCAN_SUBCODE_MASK);
    *p_ref = ble_batchscan_cb.op_q.ref_value[ble_batchscan_cb.op_q.pending_idx];
    *cur_state = (ble_batchscan_cb.op_q.cur_state[ble_batchscan_cb.op_q.pending_idx]);
    ble_batchscan_cb.op_q.pending_idx = (ble_batchscan_cb.op_q.pending_idx + 1)
                                        % BTM_BLE_BATCH_SCAN_MAX;
}

/*******************************************************************************
**
** Function         btm_ble_read_batchscan_reports
**
** Description      This function reads the reports from controller
**
** Parameters       scan_mode - The mode for which the reports are to be read out from the controller
**                  ref_value - Reference value
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS btm_ble_read_batchscan_reports(tBTM_BLE_BATCH_SCAN_MODE scan_mode,
        tBTM_BLE_REF_VALUE ref_value)
{
    tBTM_STATUS     status = BTM_NO_RESOURCES;
    UINT8 param[BTM_BLE_BATCH_SCAN_READ_RESULTS_LEN], *pp;
    pp = param;

    memset(param, 0, BTM_BLE_BATCH_SCAN_READ_RESULTS_LEN);

    UINT8_TO_STREAM (pp, BTM_BLE_BATCH_SCAN_READ_RESULTS);
    UINT8_TO_STREAM (pp, scan_mode);

    if ((status = BTM_VendorSpecificCommand (HCI_BLE_BATCH_SCAN_OCF,
                  BTM_BLE_BATCH_SCAN_READ_RESULTS_LEN, param, btm_ble_batchscan_vsc_cmpl_cback))
            != BTM_CMD_STARTED) {
        BTM_TRACE_ERROR("btm_ble_read_batchscan_reports %d", status);
        return BTM_ILLEGAL_VALUE;
    }

    if (BTM_CMD_STARTED == status) {
        /* The user needs to be provided scan read reports event */
        btm_ble_batchscan_enq_op_q(BTM_BLE_BATCH_SCAN_READ_RESULTS, ble_batchscan_cb.cur_state,
                                   BTM_BLE_BATCH_SCAN_READ_REPTS_EVT, ref_value);
    }

    return status;
}

/*******************************************************************************
**
** Function         btm_ble_batchscan_vsc_cmpl_cback
**
** Description      Batch scan VSC complete callback
**
** Parameters       p_params - VSC completed callback parameters
**
** Returns          void
**
*******************************************************************************/
void btm_ble_batchscan_vsc_cmpl_cback (tBTM_VSC_CMPL *p_params)
{
    UINT8  *p = p_params->p_param_buf;
    UINT16  len = p_params->param_len;
    tBTM_BLE_REF_VALUE ref_value = 0;

    UINT8  status = 0, subcode = 0, opcode = 0;
    UINT8 report_format = 0, num_records = 0, cb_evt = 0;
    UINT16 data_len = 0;
    tBTM_BLE_BATCH_SCAN_STATE cur_state = 0;
    tBTM_STATUS btm_status = 0;
    UINT8 *p_data = NULL;

    if (len < 2) {
        BTM_TRACE_ERROR("wrong length for btm_ble_batch_scan_vsc_cmpl_cback");
        btm_ble_batchscan_deq_op_q(&opcode, &cur_state, &cb_evt, &ref_value);
        return;
    }

    STREAM_TO_UINT8(status, p);
    STREAM_TO_UINT8(subcode, p);

    btm_ble_batchscan_deq_op_q(&opcode, &cur_state, &cb_evt, &ref_value);

    BTM_TRACE_DEBUG("btm_ble_batchscan op_code = %02x state = %02x cb_evt = %02x,ref_value=%d",
                    opcode, cur_state, cb_evt, ref_value);

    if (opcode != subcode) {
        BTM_TRACE_ERROR("Got unexpected VSC cmpl, expected: %d got: %d", subcode, opcode);
        return;
    }

    switch (subcode) {
    case BTM_BLE_BATCH_SCAN_ENB_DISAB_CUST_FEATURE: {
        if (BTM_SUCCESS == status && BTM_BLE_SCAN_ENABLE_CALLED == cur_state) {
            ble_batchscan_cb.cur_state = BTM_BLE_SCAN_ENABLED_STATE;
        } else if (BTM_BLE_SCAN_ENABLE_CALLED == cur_state) {
            BTM_TRACE_ERROR("SCAN_ENB_DISAB_CUST_FEATURE - Invalid state after enb");
            ble_batchscan_cb.cur_state = BTM_BLE_SCAN_INVALID_STATE;
        }

        BTM_TRACE_DEBUG("BTM_BLE_BATCH_SCAN_ENB_DISAB_CUST_FEAT status = %d, state: %d,evt=%d",
                        status, ble_batchscan_cb.cur_state, cb_evt);

        if (cb_evt != 0 && NULL != ble_batchscan_cb.p_setup_cback) {
            ble_batchscan_cb.p_setup_cback(cb_evt, ref_value, status);
        }
        break;
    }

    case BTM_BLE_BATCH_SCAN_SET_STORAGE_PARAM: {
        BTM_TRACE_DEBUG("BTM_BLE_BATCH_SCAN_SET_STORAGE_PARAM status = %d, evt=%d",
                        status, cb_evt);
        if (cb_evt != 0 && NULL != ble_batchscan_cb.p_setup_cback) {
            ble_batchscan_cb.p_setup_cback(cb_evt, ref_value, status);
        }
        break;
    }

    case BTM_BLE_BATCH_SCAN_SET_PARAMS: {
        BTM_TRACE_DEBUG("BTM_BLE_BATCH_SCAN_SET_PARAMS status = %d,evt=%d", status, cb_evt);

        if (BTM_BLE_SCAN_DISABLE_CALLED == cur_state) {
            if (BTM_SUCCESS == status) {
                ble_batchscan_cb.cur_state = BTM_BLE_SCAN_DISABLED_STATE;
            } else {
                BTM_TRACE_ERROR("BTM_BLE_BATCH_SCAN_SET_PARAMS - Invalid state after disabled");
                ble_batchscan_cb.cur_state = BTM_BLE_SCAN_INVALID_STATE;
            }
        }

        if (cb_evt != 0 && NULL != ble_batchscan_cb.p_setup_cback) {
            ble_batchscan_cb.p_setup_cback(cb_evt, ref_value, status);
        }
        break;
    }

    case BTM_BLE_BATCH_SCAN_READ_RESULTS: {
        if (cb_evt != 0 && NULL != ble_batchscan_cb.p_scan_rep_cback) {
            STREAM_TO_UINT8(report_format, p);
            STREAM_TO_UINT8(num_records, p);
            p = (uint8_t *)(p_params->p_param_buf + 4);
            BTM_TRACE_DEBUG("BTM_BLE_BATCH_SCAN_READ_RESULTS status=%d,len=%d,rec=%d",
                            status, len - 4, num_records);

            if (0 == num_records) {
                btm_ble_batchscan_deq_rep_data(report_format, &ref_value, &num_records,
                                               &p_data, &data_len);
                if (NULL != ble_batchscan_cb.p_scan_rep_cback) {
                    ble_batchscan_cb.p_scan_rep_cback(ref_value, report_format, num_records,
                                                      data_len, p_data, status);
                }
            } else {
                if ((len - 4) > 0) {
                    btm_ble_batchscan_enq_rep_data(report_format, num_records, p, len - 4);
                    /* More records could be in the buffer and needs to be pulled out */
                    btm_status = btm_ble_read_batchscan_reports(report_format, ref_value);
                    if (BTM_CMD_STARTED != btm_status) {
                        btm_ble_batchscan_deq_rep_data(report_format, &ref_value, &num_records,
                                                       &p_data, &data_len);
                        /* Send whatever is available, in case of a command failure */
                        if (NULL != ble_batchscan_cb.p_scan_rep_cback && NULL != p_data) {
                            ble_batchscan_cb.p_scan_rep_cback(ref_value, report_format,
                                                              num_records, data_len, p_data, status);
                        }
                    }
                }
            }
        }
        break;
    }

    default:
        break;
    }

    return;
}

/*******************************************************************************
**
** Function         btm_ble_set_storage_config
**
** Description      This function writes the storage configuration in controller
**
** Parameters       batch_scan_full_max -Max storage space (in %) allocated to full scanning
**                  batch_scan_trunc_max -Max storage space (in %) allocated to truncated scanning
**                  batch_scan_notify_threshold - Setup notification level based on total space
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS btm_ble_set_storage_config(UINT8 batch_scan_full_max, UINT8 batch_scan_trunc_max,
                                       UINT8 batch_scan_notify_threshold)
{
    tBTM_STATUS     status = BTM_NO_RESOURCES;
    UINT8 param[BTM_BLE_BATCH_SCAN_STORAGE_CFG_LEN], *pp;

    pp = param;
    memset(param, 0, BTM_BLE_BATCH_SCAN_STORAGE_CFG_LEN);

    UINT8_TO_STREAM (pp, BTM_BLE_BATCH_SCAN_SET_STORAGE_PARAM);
    UINT8_TO_STREAM (pp, batch_scan_full_max);
    UINT8_TO_STREAM (pp, batch_scan_trunc_max);
    UINT8_TO_STREAM (pp, batch_scan_notify_threshold);

    if ((status = BTM_VendorSpecificCommand (HCI_BLE_BATCH_SCAN_OCF,
                  BTM_BLE_BATCH_SCAN_STORAGE_CFG_LEN, param,
                  btm_ble_batchscan_vsc_cmpl_cback)) != BTM_CMD_STARTED) {
        BTM_TRACE_ERROR("btm_ble_set_storage_config %d", status);
        return BTM_ILLEGAL_VALUE;
    }

    return status;
}

/*******************************************************************************
**
** Function         btm_ble_set_batchscan_param
**
** Description      This function writes the batch scan params in controller
**
** Parameters       scan_mode -Batch scan mode
**                  scan_interval - Scan interval
**                  scan_window  - Scan window
**                  discard_rule -Discard rules
**                  addr_type - Address type
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS btm_ble_set_batchscan_param(tBTM_BLE_BATCH_SCAN_MODE scan_mode,
                                        UINT32 scan_interval, UINT32 scan_window, tBLE_ADDR_TYPE addr_type,
                                        tBTM_BLE_DISCARD_RULE discard_rule)
{
    tBTM_STATUS     status = BTM_NO_RESOURCES;
    UINT8 scan_param[BTM_BLE_BATCH_SCAN_PARAM_CONFIG_LEN], *pp_scan;

    pp_scan = scan_param;
    memset(scan_param, 0, BTM_BLE_BATCH_SCAN_PARAM_CONFIG_LEN);

    // Override param and decide addr_type based on own addr type
    // TODO: Remove upper layer parameter?
    addr_type = btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type;

    UINT8_TO_STREAM (pp_scan, BTM_BLE_BATCH_SCAN_SET_PARAMS);
    UINT8_TO_STREAM (pp_scan, scan_mode);
    UINT32_TO_STREAM (pp_scan, scan_window);
    UINT32_TO_STREAM (pp_scan, scan_interval);
    UINT8_TO_STREAM (pp_scan, addr_type);
    UINT8_TO_STREAM (pp_scan, discard_rule);

    if ((status = BTM_VendorSpecificCommand (HCI_BLE_BATCH_SCAN_OCF,
                  BTM_BLE_BATCH_SCAN_PARAM_CONFIG_LEN,
                  scan_param, btm_ble_batchscan_vsc_cmpl_cback)) != BTM_CMD_STARTED) {
        BTM_TRACE_ERROR("btm_ble_set_batchscan_param %d", status);
        return BTM_ILLEGAL_VALUE;
    }

    return status;
}

/*******************************************************************************
**
** Function         btm_ble_enable_disable_batchscan
**
** Description      This function enables the customer specific feature in controller
**
** Parameters       enable_disable: true - enable, false - disable
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS btm_ble_enable_disable_batchscan(BOOLEAN should_enable)
{
    tBTM_STATUS     status = BTM_NO_RESOURCES;
    UINT8 shld_enable = 0x01;
    UINT8 enable_param[BTM_BLE_BATCH_SCAN_ENB_DISB_LEN], *pp_enable;

    if (!should_enable) {
        shld_enable = 0x00;
    }

    if (should_enable) {
        pp_enable = enable_param;
        memset(enable_param, 0, BTM_BLE_BATCH_SCAN_ENB_DISB_LEN);

        UINT8_TO_STREAM (pp_enable, BTM_BLE_BATCH_SCAN_ENB_DISAB_CUST_FEATURE);
        UINT8_TO_STREAM (pp_enable, shld_enable);

        if ((status = BTM_VendorSpecificCommand(HCI_BLE_BATCH_SCAN_OCF,
                                                BTM_BLE_BATCH_SCAN_ENB_DISB_LEN, enable_param,
                                                btm_ble_batchscan_vsc_cmpl_cback)) != BTM_CMD_STARTED) {
            status = BTM_MODE_UNSUPPORTED;
            BTM_TRACE_ERROR("btm_ble_enable_disable_batchscan %d", status);
            return BTM_ILLEGAL_VALUE;
        }
    } else if ((status = btm_ble_set_batchscan_param(BTM_BLE_BATCH_SCAN_MODE_DISABLE,
                         ble_batchscan_cb.scan_interval, ble_batchscan_cb.scan_window,
                         ble_batchscan_cb.addr_type, ble_batchscan_cb.discard_rule)) != BTM_CMD_STARTED) {
        status = BTM_MODE_UNSUPPORTED;
        BTM_TRACE_ERROR("btm_ble_enable_disable_batchscan %d", status);
        return BTM_ILLEGAL_VALUE;
    }

    if (should_enable) {
        ble_batchscan_cb.cur_state = BTM_BLE_SCAN_ENABLE_CALLED;
    } else {
        ble_batchscan_cb.cur_state = BTM_BLE_SCAN_DISABLE_CALLED;
    }
    return status;
}

/*******************************************************************************
**
** Function         BTM_BleSetStorageConfig
**
** Description      This function is called to write storage config params.
**
** Parameters:      batch_scan_full_max - Max storage space (in %) allocated to full style
**                  batch_scan_trunc_max - Max storage space (in %) allocated to trunc style
**                  batch_scan_notify_threshold - Setup notification level based on total space
**                  p_setup_cback - Setup callback pointer
**                  p_thres_cback - Threshold callback pointer
**                  p_rep_cback - Reports callback pointer
**                  ref_value - Reference value
**
** Returns          tBTM_STATUS
**
*******************************************************************************/
tBTM_STATUS BTM_BleSetStorageConfig(UINT8 batch_scan_full_max, UINT8 batch_scan_trunc_max,
                                    UINT8 batch_scan_notify_threshold,
                                    tBTM_BLE_SCAN_SETUP_CBACK *p_setup_cback,
                                    tBTM_BLE_SCAN_THRESHOLD_CBACK *p_thres_cback,
                                    tBTM_BLE_SCAN_REP_CBACK *p_rep_cback,
                                    tBTM_BLE_REF_VALUE ref_value)
{
    tBTM_STATUS     status = BTM_NO_RESOURCES;
    tBTM_BLE_VSC_CB cmn_ble_vsc_cb;

    BTM_TRACE_EVENT (" BTM_BleSetStorageConfig: %d, %d, %d, %d, %d",
                     ble_batchscan_cb.cur_state, ref_value, batch_scan_full_max, batch_scan_trunc_max,
                     batch_scan_notify_threshold);

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

    BTM_BleGetVendorCapabilities(&cmn_ble_vsc_cb);

    if (0 == cmn_ble_vsc_cb.tot_scan_results_strg) {
        BTM_TRACE_ERROR("Controller does not support batch scan");
        return BTM_ERR_PROCESSING;
    }

    ble_batchscan_cb.p_setup_cback = p_setup_cback;
    ble_batchscan_cb.p_thres_cback = p_thres_cback;
    ble_batchscan_cb.p_scan_rep_cback = p_rep_cback;
    ble_batchscan_cb.ref_value = ref_value;

    if (batch_scan_full_max > BTM_BLE_ADV_SCAN_FULL_MAX ||
            batch_scan_trunc_max > BTM_BLE_ADV_SCAN_TRUNC_MAX ||
            batch_scan_notify_threshold > BTM_BLE_ADV_SCAN_THR_MAX) {
        BTM_TRACE_ERROR("Illegal set storage config params");
        return BTM_ILLEGAL_VALUE;
    }

    if (BTM_BLE_SCAN_INVALID_STATE == ble_batchscan_cb.cur_state ||
            BTM_BLE_SCAN_DISABLED_STATE == ble_batchscan_cb.cur_state ||
            BTM_BLE_SCAN_DISABLE_CALLED == ble_batchscan_cb.cur_state) {
        status = btm_ble_enable_disable_batchscan(TRUE);
        if (BTM_CMD_STARTED != status) {
            return status;
        }

        ble_batchscan_cb.cur_state = BTM_BLE_SCAN_ENABLE_CALLED;
        btm_ble_batchscan_enq_op_q(BTM_BLE_BATCH_SCAN_ENB_DISAB_CUST_FEATURE,
                                   BTM_BLE_SCAN_ENABLE_CALLED, 0, ref_value);
    }

    status = btm_ble_set_storage_config(batch_scan_full_max, batch_scan_trunc_max,
                                        batch_scan_notify_threshold);
    if (BTM_CMD_STARTED != status) {
        return status;
    }
    /* The user needs to be provided scan config storage event */
    btm_ble_batchscan_enq_op_q(BTM_BLE_BATCH_SCAN_SET_STORAGE_PARAM, ble_batchscan_cb.cur_state,
                               BTM_BLE_BATCH_SCAN_CFG_STRG_EVT, ref_value);

    return status;
}


/*******************************************************************************
**
** Function         BTM_BleEnableBatchScan
**
** Description      This function is called to configure and enable batch scanning
**
** Parameters:      scan_mode -Batch scan mode
**                  scan_interval - Scan interval value
**                  scan_window - Scan window value
**                  discard_rule - Data discard rule
**                  ref_value - Reference value
**
** Returns          tBTM_STATUS
**
*******************************************************************************/
tBTM_STATUS BTM_BleEnableBatchScan(tBTM_BLE_BATCH_SCAN_MODE scan_mode,
                                   UINT32 scan_interval, UINT32 scan_window, tBLE_ADDR_TYPE addr_type,
                                   tBTM_BLE_DISCARD_RULE discard_rule, tBTM_BLE_REF_VALUE ref_value)
{
    tBTM_STATUS     status = BTM_NO_RESOURCES;
    tBTM_BLE_VSC_CB cmn_ble_vsc_cb;
    BTM_TRACE_EVENT (" BTM_BleEnableBatchScan: %d, %d, %d, %d, %d, %d",
                     scan_mode, scan_interval, scan_window, addr_type, discard_rule, ref_value);

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

    BTM_BleGetVendorCapabilities(&cmn_ble_vsc_cb);

    if (0 == cmn_ble_vsc_cb.tot_scan_results_strg) {
        BTM_TRACE_ERROR("Controller does not support batch scan");
        return BTM_ERR_PROCESSING;
    }

    BTM_TRACE_DEBUG("BTM_BleEnableBatchScan: %d, %x, %x, %d, %d", scan_mode, scan_interval,
                    scan_window, discard_rule, ble_batchscan_cb.cur_state);

    /* Only 16 bits will be used for scan interval and scan window as per agreement with Google */
    /* So the standard LE range would suffice for scan interval and scan window */
    if ((BTM_BLE_ISVALID_PARAM(scan_interval, BTM_BLE_SCAN_INT_MIN, BTM_BLE_SCAN_INT_MAX) ||
            BTM_BLE_ISVALID_PARAM(scan_window, BTM_BLE_SCAN_WIN_MIN, BTM_BLE_SCAN_WIN_MAX))
            && (BTM_BLE_BATCH_SCAN_MODE_PASS == scan_mode || BTM_BLE_BATCH_SCAN_MODE_ACTI == scan_mode
                || BTM_BLE_BATCH_SCAN_MODE_PASS_ACTI == scan_mode)
            && (BTM_BLE_DISCARD_OLD_ITEMS == discard_rule ||
                BTM_BLE_DISCARD_LOWER_RSSI_ITEMS == discard_rule)) {
        if (BTM_BLE_SCAN_INVALID_STATE == ble_batchscan_cb.cur_state ||
                BTM_BLE_SCAN_DISABLED_STATE == ble_batchscan_cb.cur_state ||
                BTM_BLE_SCAN_DISABLE_CALLED == ble_batchscan_cb.cur_state) {
            status = btm_ble_enable_disable_batchscan(TRUE);
            if (BTM_CMD_STARTED != status) {
                return status;
            }
            btm_ble_batchscan_enq_op_q(BTM_BLE_BATCH_SCAN_ENB_DISAB_CUST_FEATURE,
                                       BTM_BLE_SCAN_ENABLE_CALLED, 0, ref_value);
        }

        ble_batchscan_cb.scan_mode = scan_mode;
        ble_batchscan_cb.scan_interval = scan_interval;
        ble_batchscan_cb.scan_window = scan_window;
        ble_batchscan_cb.addr_type = addr_type;
        ble_batchscan_cb.discard_rule = discard_rule;
        /* This command starts batch scanning, if enabled */
        status = btm_ble_set_batchscan_param(scan_mode, scan_interval, scan_window, addr_type,
                                             discard_rule);
        if (BTM_CMD_STARTED != status) {
            return status;
        }

        /* The user needs to be provided scan enable event */
        btm_ble_batchscan_enq_op_q(BTM_BLE_BATCH_SCAN_SET_PARAMS, ble_batchscan_cb.cur_state,
                                   BTM_BLE_BATCH_SCAN_ENABLE_EVT, ref_value);
    } else {
        BTM_TRACE_ERROR("Illegal enable scan params");
        return BTM_ILLEGAL_VALUE;
    }
    return status;
}

/*******************************************************************************
**
** Function         BTM_BleDisableBatchScan
**
** Description      This function is called to disable batch scanning
**
** Parameters:      ref_value - Reference value
**
** Returns          tBTM_STATUS
**
*******************************************************************************/
tBTM_STATUS BTM_BleDisableBatchScan(tBTM_BLE_REF_VALUE ref_value)
{
    tBTM_STATUS     status = BTM_NO_RESOURCES;
    tBTM_BLE_VSC_CB cmn_ble_vsc_cb;
    BTM_TRACE_EVENT (" BTM_BleDisableBatchScan");

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

    BTM_BleGetVendorCapabilities(&cmn_ble_vsc_cb);

    if (0 == cmn_ble_vsc_cb.tot_scan_results_strg) {
        BTM_TRACE_ERROR("Controller does not support batch scan");
        return BTM_ERR_PROCESSING;
    }

    status = btm_ble_enable_disable_batchscan(FALSE);
    if (BTM_CMD_STARTED == status) {
        /* The user needs to be provided scan disable event */
        btm_ble_batchscan_enq_op_q(BTM_BLE_BATCH_SCAN_SET_PARAMS,
                                   BTM_BLE_SCAN_DISABLE_CALLED, BTM_BLE_BATCH_SCAN_DISABLE_EVT,
                                   ref_value);
    }

    return status;
}

/*******************************************************************************
**
** Function         BTM_BleReadScanReports
**
** Description      This function is called to start reading batch scan reports
**
** Parameters:      scan_mode - Batch scan mode
**                  ref_value - Reference value
**
** Returns          tBTM_STATUS
**
*******************************************************************************/
tBTM_STATUS BTM_BleReadScanReports(tBTM_BLE_BATCH_SCAN_MODE scan_mode,
                                   tBTM_BLE_REF_VALUE ref_value)
{
    tBTM_STATUS     status = BTM_NO_RESOURCES;
    tBTM_BLE_VSC_CB cmn_ble_vsc_cb;
    UINT8 read_scan_mode = 0;
    UINT8  *p_data = NULL, num_records = 0;
    UINT16 data_len = 0;

    BTM_TRACE_EVENT (" BTM_BleReadScanReports; %d, %d", scan_mode, ref_value);

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

    BTM_BleGetVendorCapabilities(&cmn_ble_vsc_cb);

    if (0 == cmn_ble_vsc_cb.tot_scan_results_strg) {
        BTM_TRACE_ERROR("Controller does not support batch scan");
        return BTM_ERR_PROCESSING;
    }

    /*  Check if the requested scan mode has already been setup by the user */
    read_scan_mode = ble_batchscan_cb.scan_mode & BTM_BLE_BATCH_SCAN_MODE_ACTI;
    if (0 == read_scan_mode) {
        read_scan_mode = ble_batchscan_cb.scan_mode & BTM_BLE_BATCH_SCAN_MODE_PASS;
    }

    /* Check only for modes, as scan reports can be called after disabling batch scan */
    if (read_scan_mode > 0 && (BTM_BLE_BATCH_SCAN_MODE_PASS == scan_mode ||
                               BTM_BLE_BATCH_SCAN_MODE_ACTI == scan_mode)) {
        status = btm_ble_batchscan_enq_rep_q(scan_mode, ref_value);
        if (BTM_SUCCESS == status) {
            status = btm_ble_read_batchscan_reports(scan_mode, ref_value);
            if (BTM_CMD_STARTED != status) {
                btm_ble_batchscan_deq_rep_data(scan_mode, &ref_value,
                                               &num_records, &p_data, &data_len);
            }
        }
    } else {
        BTM_TRACE_ERROR("Illegal read scan params: %d, %d, %d", read_scan_mode, scan_mode,
                        ble_batchscan_cb.cur_state);
        return BTM_ILLEGAL_VALUE;
    }
    return status;
}


/*******************************************************************************
**
** Function         BTM_BleTrackAdvertiser
**
** Description      This function is called to setup the callback for tracking advertisers
**
** Parameters:      p_track_cback - Tracking callback pointer
**                  ref_value - Reference value
**
** Returns          tBTM_STATUS
**
*******************************************************************************/
tBTM_STATUS BTM_BleTrackAdvertiser(tBTM_BLE_TRACK_ADV_CBACK *p_track_cback,
                                   tBTM_BLE_REF_VALUE ref_value)
{
    tBTM_BLE_VSC_CB cmn_ble_vsc_cb;
    BTM_TRACE_EVENT (" BTM_BleTrackAdvertiser");
    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

    BTM_BleGetVendorCapabilities(&cmn_ble_vsc_cb);

    if (0 == cmn_ble_vsc_cb.tot_scan_results_strg) {
        BTM_TRACE_ERROR("Controller does not support scan storage");
        return BTM_ERR_PROCESSING;
    }

    ble_advtrack_cb.p_track_cback = p_track_cback;
    ble_advtrack_cb.ref_value = ref_value;
    return BTM_CMD_STARTED;
}

/*******************************************************************************
**
** Function         btm_ble_batchscan_init
**
** Description      This function initialize the batch scan control block.
**
** Parameters       None
**
** Returns          status
**
*******************************************************************************/
void btm_ble_batchscan_init(void)
{
#if BTM_DYNAMIC_MEMORY == TRUE
    ble_batchscan_cb_ptr = (tBTM_BLE_BATCH_SCAN_CB *)osi_malloc(sizeof(tBTM_BLE_BATCH_SCAN_CB));
    ble_advtrack_cb_ptr = (tBTM_BLE_ADV_TRACK_CB *)osi_malloc(sizeof(tBTM_BLE_ADV_TRACK_CB));
    if (ble_batchscan_cb_ptr == NULL || ble_advtrack_cb_ptr == NULL) {
        BTM_TRACE_ERROR("%s malloc failed", __func__);
        return;
    }
#endif
    BTM_TRACE_EVENT (" btm_ble_batchscan_init");
    memset(&ble_batchscan_cb, 0, sizeof(tBTM_BLE_BATCH_SCAN_CB));
    memset(&ble_advtrack_cb, 0, sizeof(tBTM_BLE_ADV_TRACK_CB));
    BTM_RegisterForVSEvents(btm_ble_batchscan_filter_track_adv_vse_cback, TRUE);
}

/*******************************************************************************
**
** Function         btm_ble_batchscan_cleanup
**
** Description      This function cleans the batch scan control block.
**
** Parameters       None
**
** Returns          void
**
*******************************************************************************/
void btm_ble_batchscan_cleanup(void)
{
    int index = 0;
    BTM_TRACE_EVENT (" btm_ble_batchscan_cleanup");

    for (index = 0; index < BTM_BLE_BATCH_REP_MAIN_Q_SIZE; index++) {
        if (NULL != ble_batchscan_cb.main_rep_q.p_data[index]) {
            osi_free(ble_batchscan_cb.main_rep_q.p_data[index]);
            ble_batchscan_cb.main_rep_q.p_data[index] = NULL;
        }
    }

    memset(&ble_batchscan_cb, 0, sizeof(tBTM_BLE_BATCH_SCAN_CB));
    memset(&ble_advtrack_cb, 0, sizeof(tBTM_BLE_ADV_TRACK_CB));

#if BTM_DYNAMIC_MEMORY == TRUE
    osi_free(ble_batchscan_cb_ptr);
    osi_free(ble_advtrack_cb_ptr);
    ble_batchscan_cb_ptr = NULL;
    ble_advtrack_cb_ptr = NULL;
#endif
}

#endif

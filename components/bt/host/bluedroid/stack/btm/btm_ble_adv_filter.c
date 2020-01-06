/******************************************************************************
 *
 *  Copyright (C) 2014  Broadcom Corporation
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

//#define LOG_TAG "bt_btm_ble"

#include <string.h>
#include "common/bt_target.h"

#if (BLE_INCLUDED == TRUE)
#include "stack/bt_types.h"
#include "stack/hcimsgs.h"
#include "stack/btu.h"
#include "btm_int.h"
#include "osi/allocator.h"
#include "stack/hcidefs.h"
#include "stack/btm_ble_api.h"
#include "device/controller.h"

#define BTM_BLE_ADV_FILT_META_HDR_LENGTH 3
#define BTM_BLE_ADV_FILT_FEAT_SELN_LEN  13
#define BTM_BLE_ADV_FILT_TRACK_NUM       2

#define BTM_BLE_PF_SELECT_NONE              0

/* BLE meta vsc header: 1 bytes of sub_code, 1 byte of PCF action */
#define BTM_BLE_META_HDR_LENGTH     3
#define BTM_BLE_PF_FEAT_SEL_LEN     18
#define BTM_BLE_PCF_ENABLE_LEN      2

#define BTM_BLE_META_ADDR_LEN       7
#define BTM_BLE_META_UUID_LEN       40

#define BTM_BLE_PF_BIT_TO_MASK(x)          (UINT16)(1 << (x))


#if BTM_DYNAMIC_MEMORY == FALSE
tBTM_BLE_ADV_FILTER_CB btm_ble_adv_filt_cb;
tBTM_BLE_VSC_CB cmn_ble_adv_vsc_cb;
#else
tBTM_BLE_ADV_FILTER_CB *btm_ble_adv_filt_cb_ptr;
tBTM_BLE_VSC_CB *cmn_ble_adv_vsc_cb_ptr;
#define btm_ble_adv_filt_cb (*btm_ble_adv_filt_cb_ptr)
#define cmn_ble_adv_vsc_cb  (*cmn_ble_adv_vsc_cb_ptr)
#endif

static const BD_ADDR     na_bda = {0};

static UINT8 btm_ble_cs_update_pf_counter(tBTM_BLE_SCAN_COND_OP action,
        UINT8 cond_type, tBLE_BD_ADDR *p_bd_addr, UINT8 num_available);

#define BTM_BLE_SET_SCAN_PF_OPCODE(x, y) (((x)<<4)|y)
#define BTM_BLE_GET_SCAN_PF_SUBCODE(x)    ((x) >> 4)
#define BTM_BLE_GET_SCAN_PF_ACTION(x)    ((x) & 0x0f)
#define BTM_BLE_INVALID_COUNTER     0xff


/* length of each multi adv sub command */
#define BTM_BLE_ADV_FILTER_ENB_LEN                       3

/* length of each batch scan command */
#define BTM_BLE_ADV_FILTER_CLEAR_LEN            3
#define BTM_BLE_ADV_FILTER_LEN     2

#define BTM_BLE_ADV_FILT_CB_EVT_MASK       0xF0
#define BTM_BLE_ADV_FILT_SUBCODE_MASK      0x0F

/*******************************************************************************
**
** Function         btm_ble_obtain_vsc_details
**
** Description      This function obtains the VSC details
**
** Parameters
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS btm_ble_obtain_vsc_details(void)
{
    tBTM_STATUS st = BTM_SUCCESS;

#if BLE_VND_INCLUDED == TRUE
    BTM_BleGetVendorCapabilities(&cmn_ble_adv_vsc_cb);
    if (0 == cmn_ble_adv_vsc_cb.max_filter) {
        st = BTM_MODE_UNSUPPORTED;
        return st;
    }
#else
    cmn_ble_adv_vsc_cb.max_filter = BTM_BLE_MAX_FILTER_COUNTER;
#endif
    return st;
}

/*******************************************************************************
**
** Function         btm_ble_advfilt_enq_op_q
**
** Description      enqueue an adv filter operation in q to check command complete
**                  status
**
** Returns          void
**
*******************************************************************************/
void btm_ble_advfilt_enq_op_q(UINT8 action, UINT8 ocf, tBTM_BLE_FILT_CB_EVT cb_evt,
                              tBTM_BLE_REF_VALUE ref, tBTM_BLE_PF_CFG_CBACK *p_cmpl_cback,
                              tBTM_BLE_PF_PARAM_CBACK  *p_filt_param_cback)
{
    btm_ble_adv_filt_cb.op_q.action_ocf[btm_ble_adv_filt_cb.op_q.next_idx] = (action | (ocf << 4));
    btm_ble_adv_filt_cb.op_q.ref_value[btm_ble_adv_filt_cb.op_q.next_idx] = ref;
    btm_ble_adv_filt_cb.op_q.cb_evt[btm_ble_adv_filt_cb.op_q.next_idx] = cb_evt;
    btm_ble_adv_filt_cb.op_q.p_scan_cfg_cback[btm_ble_adv_filt_cb.op_q.next_idx] = p_cmpl_cback;
    btm_ble_adv_filt_cb.op_q.p_filt_param_cback[btm_ble_adv_filt_cb.op_q.next_idx]
        = p_filt_param_cback;
    BTM_TRACE_DEBUG("btm_ble_advfilt_enq_op_q: act_ocf:%d, action:%d, ocf:%d,cb_evt;%d, cback:%p",
                    btm_ble_adv_filt_cb.op_q.action_ocf[btm_ble_adv_filt_cb.op_q.next_idx], action,
                    ocf, cb_evt, p_cmpl_cback);
    btm_ble_adv_filt_cb.op_q.next_idx = (btm_ble_adv_filt_cb.op_q.next_idx + 1)
                                        % BTM_BLE_PF_TYPE_MAX;
}

/*******************************************************************************
**
** Function         btm_ble_advfilt_deq_op_q
**
** Description      dequeue an adv filter operation from q when command complete
**                  is received
**
** Returns          void
**
*******************************************************************************/
void btm_ble_advfilt_deq_op_q(UINT8 *p_action, UINT8 *p_ocf, tBTM_BLE_FILT_CB_EVT *p_cb_evt,
                              tBTM_BLE_REF_VALUE *p_ref, tBTM_BLE_PF_CFG_CBACK **p_cmpl_cback,
                              tBTM_BLE_PF_PARAM_CBACK  **p_filt_param_cback)
{
    *p_ocf = (btm_ble_adv_filt_cb.op_q.action_ocf[btm_ble_adv_filt_cb.op_q.pending_idx] >> 4);
    *p_action = (btm_ble_adv_filt_cb.op_q.action_ocf[btm_ble_adv_filt_cb.op_q.pending_idx]
                 & BTM_BLE_ADV_FILT_SUBCODE_MASK);
    *p_ref = btm_ble_adv_filt_cb.op_q.ref_value[btm_ble_adv_filt_cb.op_q.pending_idx];
    *p_cb_evt = btm_ble_adv_filt_cb.op_q.cb_evt[btm_ble_adv_filt_cb.op_q.pending_idx];
    *p_cmpl_cback = btm_ble_adv_filt_cb.op_q.p_scan_cfg_cback[btm_ble_adv_filt_cb.op_q.pending_idx];
    *p_filt_param_cback =
        btm_ble_adv_filt_cb.op_q.p_filt_param_cback[btm_ble_adv_filt_cb.op_q.pending_idx];

    btm_ble_adv_filt_cb.op_q.pending_idx = (btm_ble_adv_filt_cb.op_q.pending_idx + 1)
                                           % BTM_BLE_PF_TYPE_MAX;
    BTM_TRACE_DEBUG("btm_ble_advfilt_deq_op_q: ocf:%d, action:%d, ref_value:%d, cb_evt:%x",
                    *p_ocf, *p_action, *p_ref, *p_cb_evt);
}

/*******************************************************************************
**
** Function         btm_ble_condtype_to_ocf
**
** Description      Convert cond_type to OCF
**
** Returns          Returns ocf value
**
*******************************************************************************/
UINT8 btm_ble_condtype_to_ocf(UINT8 cond_type)
{
    UINT8 ocf = 0;

    switch (cond_type) {
    case BTM_BLE_PF_ADDR_FILTER:
        ocf = BTM_BLE_META_PF_ADDR;
        break;
    case BTM_BLE_PF_SRVC_UUID:
        ocf = BTM_BLE_META_PF_UUID;
        break;
    case BTM_BLE_PF_SRVC_SOL_UUID:
        ocf = BTM_BLE_META_PF_SOL_UUID;
        break;
    case BTM_BLE_PF_LOCAL_NAME:
        ocf = BTM_BLE_META_PF_LOCAL_NAME;
        break;
    case BTM_BLE_PF_MANU_DATA:
        ocf = BTM_BLE_META_PF_MANU_DATA;
        break;
    case BTM_BLE_PF_SRVC_DATA_PATTERN:
        ocf = BTM_BLE_META_PF_SRVC_DATA;
        break;
    case BTM_BLE_PF_TYPE_ALL:
        ocf = BTM_BLE_META_PF_ALL;
        break;
    default:
        ocf = BTM_BLE_PF_TYPE_MAX;
        break;
    }
    return ocf;
}

/*******************************************************************************
**
** Function         btm_ble_ocf_to_condtype
**
** Description      Convert OCF to cond type
**
** Returns          Returns condtype value
**
*******************************************************************************/
UINT8 btm_ble_ocf_to_condtype(UINT8 ocf)
{
    UINT8 cond_type = 0;

    switch (ocf) {
    case BTM_BLE_META_PF_FEAT_SEL:
        cond_type = BTM_BLE_META_PF_FEAT_SEL;
        break;
    case BTM_BLE_META_PF_ADDR:
        cond_type = BTM_BLE_PF_ADDR_FILTER;
        break;
    case BTM_BLE_META_PF_UUID:
        cond_type = BTM_BLE_PF_SRVC_UUID;
        break;
    case BTM_BLE_META_PF_SOL_UUID:
        cond_type = BTM_BLE_PF_SRVC_SOL_UUID;
        break;
    case BTM_BLE_META_PF_LOCAL_NAME:
        cond_type = BTM_BLE_PF_LOCAL_NAME;
        break;
    case BTM_BLE_META_PF_MANU_DATA:
        cond_type = BTM_BLE_PF_MANU_DATA;
        break;
    case BTM_BLE_META_PF_SRVC_DATA:
        cond_type = BTM_BLE_PF_SRVC_DATA_PATTERN;
        break;
    case BTM_BLE_META_PF_ALL:
        cond_type = BTM_BLE_PF_TYPE_ALL;
        break;
    default:
        cond_type = BTM_BLE_PF_TYPE_MAX;
        break;
    }
    return cond_type;
}

/*******************************************************************************
**
** Function         btm_ble_scan_pf_cmpl_cback
**
** Description      the BTM BLE customer feature VSC complete callback for ADV PF filtering
**
** Returns          pointer to the counter if found; NULL otherwise.
**
*******************************************************************************/
void btm_ble_scan_pf_cmpl_cback(tBTM_VSC_CMPL *p_params)
{
    UINT8  status = 0;
    UINT8  *p = p_params->p_param_buf, op_subcode = 0, action = 0xff;
    UINT16  evt_len = p_params->param_len;
    UINT8   ocf = BTM_BLE_META_PF_ALL, cond_type = 0;
    UINT8   num_avail = 0, cb_evt = 0;
    tBTM_BLE_REF_VALUE ref_value = 0;
    tBTM_BLE_PF_CFG_CBACK *p_scan_cfg_cback = NULL;
    tBTM_BLE_PF_PARAM_CBACK *p_filt_param_cback = NULL;

    if (evt_len < 3 || evt_len > 4) {
        BTM_TRACE_ERROR("%s cannot interpret APCF callback status = %d, length = %d",
                        __func__, status, evt_len);
        btm_ble_advfilt_deq_op_q(&action, &ocf, &cb_evt, &ref_value, &p_scan_cfg_cback,
                                 &p_filt_param_cback);
        return;
    }

    btm_ble_advfilt_deq_op_q(&action, &ocf, &cb_evt, &ref_value, &p_scan_cfg_cback,
                             &p_filt_param_cback);

    STREAM_TO_UINT8(status, p);
    STREAM_TO_UINT8(op_subcode, p);
    STREAM_TO_UINT8(action, p);

    /* Ignore the event, if it is not the same one expected */
    if (3 == evt_len) {
        if (ocf != op_subcode) {
            BTM_TRACE_ERROR("btm_ble_scan_pf_cmpl_cback:3-Incorrect opcode :%d, %d, %d, %d, %d, %d",
                            ocf, op_subcode, action, evt_len, ref_value, status);
            return;
        } else {
            if (NULL != btm_ble_adv_filt_cb.p_filt_stat_cback) {
                btm_ble_adv_filt_cb.p_filt_stat_cback(action, status, ref_value);
            }
            BTM_TRACE_DEBUG("btm_ble_scan_pf_cmpl_cback enabled/disabled, %d, %d, %d, %d",
                            ocf, action, status, ref_value);
            return;
        }
    }

    if (4 == evt_len && ocf != op_subcode) {
        BTM_TRACE_ERROR("btm_ble_scan_pf_cmpl_cback:4-Incorrect opcode: %d, %d, %d, %d, %d",
                        ocf, op_subcode, action, status, ref_value);
        return;
    }

    STREAM_TO_UINT8(num_avail, p);
    switch (op_subcode) {
    case BTM_BLE_META_PF_ADDR:
    case BTM_BLE_META_PF_UUID:
    case BTM_BLE_META_PF_SOL_UUID:
    case BTM_BLE_META_PF_LOCAL_NAME:
    case BTM_BLE_META_PF_MANU_DATA:
    case BTM_BLE_META_PF_SRVC_DATA:
        cond_type = btm_ble_ocf_to_condtype(ocf);
        BTM_TRACE_DEBUG("btm_ble_scan_pf_cmpl_cback Recd: %d, %d, %d, %d, %d, %d", op_subcode,
                        ocf, action, status, ref_value, num_avail);
        if (HCI_SUCCESS == status) {
            if (memcmp(&btm_ble_adv_filt_cb.cur_filter_target.bda, &na_bda, BD_ADDR_LEN) == 0) {
                btm_ble_cs_update_pf_counter(action, cond_type, NULL, num_avail);
            } else {
                btm_ble_cs_update_pf_counter(action, cond_type,
                                             &btm_ble_adv_filt_cb.cur_filter_target, num_avail);
            }
        }

        /* send ADV PF operation complete */
        btm_ble_adv_filt_cb.op_type = 0;
        break;

    case BTM_BLE_META_PF_FEAT_SEL:
        BTM_TRACE_DEBUG("btm_ble_scan_pf_cmpl_cback-Feat sel event: %d, %d, %d, %d",
                        action, status, ref_value, num_avail);
        break;

    default:
        BTM_TRACE_ERROR("btm_ble_scan_pf_cmpl_cback: unknown operation: %d", op_subcode);
        break;
    }

    BTM_TRACE_DEBUG("btm_ble_scan_pf_cmpl_cback: calling the cback: %d", cb_evt);
    switch (cb_evt) {
    case BTM_BLE_FILT_CFG:
        if (NULL != p_scan_cfg_cback) {
            p_scan_cfg_cback(action, cond_type, num_avail, status, ref_value);
        }
        break;
    case BTM_BLE_FILT_ADV_PARAM:
        if (NULL != p_filt_param_cback) {
            p_filt_param_cback(action, num_avail, ref_value, status);
        }
        break;
    default:
        break;
    }
}

/*******************************************************************************
**
** Function         btm_ble_find_addr_filter_counter
**
** Description      find the per bd address ADV payload filter counter by BD_ADDR.
**
** Returns          pointer to the counter if found; NULL otherwise.
**
*******************************************************************************/
tBTM_BLE_PF_COUNT *btm_ble_find_addr_filter_counter(tBLE_BD_ADDR *p_le_bda)
{
    UINT8               i;
    tBTM_BLE_PF_COUNT   *p_addr_filter = &btm_ble_adv_filt_cb.p_addr_filter_count[1];

    if (p_le_bda == NULL) {
        return &btm_ble_adv_filt_cb.p_addr_filter_count[0];
    }

    for (i = 0; i < cmn_ble_adv_vsc_cb.max_filter; i ++, p_addr_filter ++) {
        if (p_addr_filter->in_use &&
                memcmp(p_le_bda->bda, p_addr_filter->bd_addr, BD_ADDR_LEN) == 0) {
            return p_addr_filter;
        }
    }
    return NULL;
}

/*******************************************************************************
**
** Function         btm_ble_alloc_addr_filter_counter
**
** Description      allocate the per device adv payload filter counter.
**
** Returns          pointer to the counter if allocation succeed; NULL otherwise.
**
*******************************************************************************/
tBTM_BLE_PF_COUNT *btm_ble_alloc_addr_filter_counter(BD_ADDR bd_addr)
{
    UINT8               i;
    tBTM_BLE_PF_COUNT   *p_addr_filter = &btm_ble_adv_filt_cb.p_addr_filter_count[1];

    for (i = 0; i < cmn_ble_adv_vsc_cb.max_filter; i ++, p_addr_filter ++) {
        if (memcmp(na_bda, p_addr_filter->bd_addr, BD_ADDR_LEN) == 0) {
            memcpy(p_addr_filter->bd_addr, bd_addr, BD_ADDR_LEN);
            p_addr_filter->in_use = TRUE;
            return p_addr_filter;
        }
    }
    return NULL;
}
/*******************************************************************************
**
** Function         btm_ble_dealloc_addr_filter_counter
**
** Description      de-allocate the per device adv payload filter counter.
**
** Returns          TRUE if deallocation succeed; FALSE otherwise.
**
*******************************************************************************/
BOOLEAN btm_ble_dealloc_addr_filter_counter(tBLE_BD_ADDR *p_bd_addr, UINT8 filter_type)
{
    UINT8               i;
    tBTM_BLE_PF_COUNT   *p_addr_filter = &btm_ble_adv_filt_cb.p_addr_filter_count[1];
    BOOLEAN             found = FALSE;

    if (BTM_BLE_PF_TYPE_ALL == filter_type && NULL == p_bd_addr) {
        memset(&btm_ble_adv_filt_cb.p_addr_filter_count[0], 0, sizeof(tBTM_BLE_PF_COUNT));
    }

    for (i = 0; i < cmn_ble_adv_vsc_cb.max_filter; i ++, p_addr_filter ++) {
        if ((p_addr_filter->in_use) && (NULL == p_bd_addr ||
                                        (NULL != p_bd_addr &&
                                         memcmp(p_bd_addr->bda, p_addr_filter->bd_addr, BD_ADDR_LEN) == 0))) {
            found = TRUE;
            memset(p_addr_filter, 0, sizeof(tBTM_BLE_PF_COUNT));

            if (NULL != p_bd_addr) {
                break;
            }
        }
    }
    return found;
}

/*******************************************************************************
**
** Function         btm_ble_update_pf_local_name
**
** Description      this function update(add,delete or clear) the adv lcoal name filtering condition.
**
**
** Returns          BTM_SUCCESS if sucessful,
**                  BTM_ILLEGAL_VALUE if paramter is not valid.
**
*******************************************************************************/
tBTM_STATUS btm_ble_update_pf_local_name(tBTM_BLE_SCAN_COND_OP action,
        tBTM_BLE_PF_FILT_INDEX filt_index,
        tBTM_BLE_PF_COND_PARAM *p_cond)
{
    tBTM_BLE_PF_LOCAL_NAME_COND *p_local_name = (p_cond == NULL) ? NULL : &p_cond->local_name;
    UINT8       param[BTM_BLE_PF_STR_LEN_MAX + BTM_BLE_ADV_FILT_META_HDR_LENGTH],
                *p = param,
                 len = BTM_BLE_ADV_FILT_META_HDR_LENGTH;
    tBTM_STATUS st = BTM_ILLEGAL_VALUE;

    memset(param, 0, BTM_BLE_PF_STR_LEN_MAX + BTM_BLE_ADV_FILT_META_HDR_LENGTH);

    UINT8_TO_STREAM(p, BTM_BLE_META_PF_LOCAL_NAME);
    UINT8_TO_STREAM(p, action);

    /* Filter index */
    UINT8_TO_STREAM(p, filt_index);

    if (BTM_BLE_SCAN_COND_ADD == action ||
            BTM_BLE_SCAN_COND_DELETE == action) {
        if (NULL == p_local_name) {
            return st;
        }

        if (p_local_name->data_len > BTM_BLE_PF_STR_LEN_MAX) {
            p_local_name->data_len = BTM_BLE_PF_STR_LEN_MAX;
        }

        ARRAY_TO_STREAM(p, p_local_name->p_data, p_local_name->data_len);
        len += p_local_name->data_len;
    }

    /* send local name filter */
    if ((st = BTM_VendorSpecificCommand (HCI_BLE_ADV_FILTER_OCF,
                                         len,
                                         param,
                                         btm_ble_scan_pf_cmpl_cback))
            != BTM_NO_RESOURCES) {
        memset(&btm_ble_adv_filt_cb.cur_filter_target, 0, sizeof(tBLE_BD_ADDR));
    } else {
        BTM_TRACE_ERROR("Local Name PF filter update failed");
    }

    return st;
}


/*******************************************************************************
**
** Function         btm_ble_update_srvc_data_change
**
** Description      this function update(add/remove) service data change filter.
**
**
** Returns          BTM_SUCCESS if sucessful,
**                  BTM_ILLEGAL_VALUE if paramter is not valid.
**
*******************************************************************************/
tBTM_STATUS btm_ble_update_srvc_data_change(tBTM_BLE_SCAN_COND_OP action,
        tBTM_BLE_PF_FILT_INDEX filt_index,
        tBTM_BLE_PF_COND_PARAM *p_cond)
{
    tBTM_STATUS st = BTM_ILLEGAL_VALUE;
    tBLE_BD_ADDR   *p_bd_addr = p_cond ? &p_cond->target_addr : NULL;
    UINT8           num_avail = (action == BTM_BLE_SCAN_COND_ADD) ? 0 : 1;

    if (btm_ble_cs_update_pf_counter (action, BTM_BLE_PF_SRVC_DATA, p_bd_addr, num_avail)
            != BTM_BLE_INVALID_COUNTER) {
        st = BTM_SUCCESS;
    }

    return st;
}

/*******************************************************************************
**
** Function         btm_ble_update_pf_manu_data
**
** Description      this function update(add,delete or clear) the adv manufacturer
**                  data filtering condition.
**
**
** Returns          BTM_SUCCESS if sucessful,
**                  BTM_ILLEGAL_VALUE if paramter is not valid.
**
*******************************************************************************/
tBTM_STATUS btm_ble_update_pf_manu_data(tBTM_BLE_SCAN_COND_OP action,
                                        tBTM_BLE_PF_FILT_INDEX filt_index,
                                        tBTM_BLE_PF_COND_PARAM *p_data,
                                        tBTM_BLE_PF_COND_TYPE cond_type,
                                        tBTM_BLE_FILT_CB_EVT cb_evt,
                                        tBTM_BLE_REF_VALUE ref_value)
{
    tBTM_BLE_PF_MANU_COND *p_manu_data = (p_data == NULL) ? NULL : &p_data->manu_data;
    tBTM_BLE_PF_SRVC_PATTERN_COND *p_srvc_data = (p_data == NULL) ? NULL : &p_data->srvc_data;

    UINT8 param[BTM_BLE_PF_STR_LEN_MAX + BTM_BLE_PF_STR_LEN_MAX + BTM_BLE_ADV_FILT_META_HDR_LENGTH],
          *p = param,
           len = BTM_BLE_ADV_FILT_META_HDR_LENGTH;
    tBTM_STATUS st = BTM_ILLEGAL_VALUE;

    if (NULL == p_data) {
        return st;
    }

    memset(param, 0, BTM_BLE_PF_STR_LEN_MAX + BTM_BLE_PF_STR_LEN_MAX
           + BTM_BLE_ADV_FILT_META_HDR_LENGTH);

    if (BTM_BLE_PF_SRVC_DATA_PATTERN == cond_type) {
        UINT8_TO_STREAM(p, BTM_BLE_META_PF_SRVC_DATA);
    } else {
        UINT8_TO_STREAM(p, BTM_BLE_META_PF_MANU_DATA);
    }

    UINT8_TO_STREAM(p, action);

    /* Filter index */
    UINT8_TO_STREAM(p, filt_index);

    if (BTM_BLE_SCAN_COND_ADD == action || BTM_BLE_SCAN_COND_DELETE == action) {
        if (BTM_BLE_PF_SRVC_DATA_PATTERN == cond_type) {
            if (NULL == p_srvc_data) {
                return st;
            }
            if (p_srvc_data->data_len > (BTM_BLE_PF_STR_LEN_MAX - 2)) {
                p_srvc_data->data_len = (BTM_BLE_PF_STR_LEN_MAX - 2);
            }

            if (p_srvc_data->data_len > 0) {
                ARRAY_TO_STREAM(p, p_srvc_data->p_pattern, p_srvc_data->data_len);
                len += (p_srvc_data->data_len);
                ARRAY_TO_STREAM(p, p_srvc_data->p_pattern_mask, p_srvc_data->data_len);
            }

            len += (p_srvc_data->data_len);
            BTM_TRACE_DEBUG("Service data length: %d", len);
        } else {
            if (NULL == p_manu_data) {
                BTM_TRACE_ERROR("btm_ble_update_pf_manu_data - No manuf data");
                return st;
            }
            BTM_TRACE_EVENT("btm_ble_update_pf_manu_data length: %d",
                            p_manu_data->data_len);
            if (p_manu_data->data_len > (BTM_BLE_PF_STR_LEN_MAX - 2)) {
                p_manu_data->data_len = (BTM_BLE_PF_STR_LEN_MAX - 2);
            }

            UINT16_TO_STREAM(p, p_manu_data->company_id);
            if (p_manu_data->data_len > 0 && p_manu_data->p_pattern_mask != NULL) {
                ARRAY_TO_STREAM(p, p_manu_data->p_pattern, p_manu_data->data_len);
                len += (p_manu_data->data_len + 2);
            } else {
                len += 2;
            }

            if (p_manu_data->company_id_mask != 0) {
                UINT16_TO_STREAM (p, p_manu_data->company_id_mask);
            } else {
                memset(p, 0xff, 2);
                p += 2;
            }
            len += 2;

            if (p_manu_data->data_len > 0 && p_manu_data->p_pattern_mask != NULL) {
                ARRAY_TO_STREAM(p, p_manu_data->p_pattern_mask, p_manu_data->data_len);
                len += (p_manu_data->data_len);
            }

            BTM_TRACE_DEBUG("Manuf data length: %d", len);
        }
    }

    /* send manufacturer*/
    if ((st = BTM_VendorSpecificCommand (HCI_BLE_ADV_FILTER_OCF,
                                         len,
                                         param,
                                         btm_ble_scan_pf_cmpl_cback)) != BTM_NO_RESOURCES) {
        memset(&btm_ble_adv_filt_cb.cur_filter_target, 0, sizeof(tBLE_BD_ADDR));
    } else {
        BTM_TRACE_ERROR("manufacturer data PF filter update failed");
    }

    return st;
}

/*******************************************************************************
**
** Function         btm_ble_cs_update_pf_counter
**
** Description      this function is to update the adv data payload filter counter
**
** Returns          current number of the counter; BTM_BLE_INVALID_COUNTER if
**                  counter update failed.
**
*******************************************************************************/
UINT8 btm_ble_cs_update_pf_counter(tBTM_BLE_SCAN_COND_OP action,
                                   UINT8 cond_type, tBLE_BD_ADDR *p_bd_addr,
                                   UINT8 num_available)
{
    tBTM_BLE_PF_COUNT   *p_addr_filter = NULL;
    UINT8               *p_counter = NULL;

    btm_ble_obtain_vsc_details();

    if (cond_type > BTM_BLE_PF_TYPE_ALL) {
        BTM_TRACE_ERROR("unknown PF filter condition type %d", cond_type);
        return BTM_BLE_INVALID_COUNTER;
    }

    /* for these three types of filter, always generic */
    if (BTM_BLE_PF_ADDR_FILTER == cond_type ||
            BTM_BLE_PF_MANU_DATA == cond_type ||
            BTM_BLE_PF_LOCAL_NAME == cond_type ||
            BTM_BLE_PF_SRVC_DATA_PATTERN == cond_type) {
        p_bd_addr = NULL;
    }

    if ((p_addr_filter = btm_ble_find_addr_filter_counter(p_bd_addr)) == NULL &&
            BTM_BLE_SCAN_COND_ADD == action) {
        p_addr_filter = btm_ble_alloc_addr_filter_counter(p_bd_addr->bda);
    }

    if (NULL != p_addr_filter) {
        /* all filter just cleared */
        if ((BTM_BLE_PF_TYPE_ALL == cond_type && BTM_BLE_SCAN_COND_CLEAR == action) ||
                /* or bd address filter been deleted */
                (BTM_BLE_PF_ADDR_FILTER == cond_type &&
                 (BTM_BLE_SCAN_COND_DELETE == action || BTM_BLE_SCAN_COND_CLEAR == action))) {
            btm_ble_dealloc_addr_filter_counter(p_bd_addr, cond_type);
        }
        /* if not feature selection, update new addition/reduction of the filter counter */
        else if (cond_type != BTM_BLE_PF_TYPE_ALL) {
            p_counter = p_addr_filter->pf_counter;
            if (num_available > 0) {
                p_counter[cond_type] += 1;
            }

            BTM_TRACE_DEBUG("counter = %d, maxfilt = %d, num_avbl=%d",
                            p_counter[cond_type], cmn_ble_adv_vsc_cb.max_filter, num_available);
            return p_counter[cond_type];
        }
    } else {
        BTM_TRACE_ERROR("no matching filter counter found");
    }
    /* no matching filter located and updated */
    return BTM_BLE_INVALID_COUNTER;
}


/*******************************************************************************
**
** Function         btm_ble_update_addr_filter
**
** Description      this function update(add,delete or clear) the address filter of adv.
**
**
** Returns          BTM_SUCCESS if sucessful,
**                  BTM_ILLEGAL_VALUE if paramter is not valid.
**
*******************************************************************************/
tBTM_STATUS btm_ble_update_addr_filter(tBTM_BLE_SCAN_COND_OP action,
                                       tBTM_BLE_PF_FILT_INDEX filt_index,
                                       tBTM_BLE_PF_COND_PARAM *p_cond)
{
    UINT8       param[BTM_BLE_META_ADDR_LEN + BTM_BLE_ADV_FILT_META_HDR_LENGTH],
                * p = param;
    tBTM_STATUS st = BTM_ILLEGAL_VALUE;
    tBLE_BD_ADDR *p_addr = (p_cond == NULL) ? NULL : &p_cond->target_addr;

    memset(param, 0, BTM_BLE_META_ADDR_LEN + BTM_BLE_ADV_FILT_META_HDR_LENGTH);

    UINT8_TO_STREAM(p, BTM_BLE_META_PF_ADDR);
    UINT8_TO_STREAM(p, action);

    /* Filter index */
    UINT8_TO_STREAM(p, filt_index);

    if (BTM_BLE_SCAN_COND_ADD == action ||
            BTM_BLE_SCAN_COND_DELETE == action) {
        if (NULL == p_addr) {
            return st;
        }

        BDADDR_TO_STREAM(p, p_addr->bda);
        UINT8_TO_STREAM(p, p_addr->type);
    }
    /* send address filter */
    if ((st = BTM_VendorSpecificCommand (HCI_BLE_ADV_FILTER_OCF,
                                         (UINT8)(BTM_BLE_ADV_FILT_META_HDR_LENGTH + BTM_BLE_META_ADDR_LEN),
                                         param,
                                         btm_ble_scan_pf_cmpl_cback)) != BTM_NO_RESOURCES) {
        memset(&btm_ble_adv_filt_cb.cur_filter_target, 0, sizeof(tBLE_BD_ADDR));
    } else {
        BTM_TRACE_ERROR("Broadcaster Address Filter Update failed");
    }
    return st;
}

/*******************************************************************************
**
** Function         btm_ble_update_uuid_filter
**
** Description      this function update(add,delete or clear) service UUID filter.
**
**
** Returns          BTM_SUCCESS if sucessful,
**                  BTM_ILLEGAL_VALUE if paramter is not valid.
**
*******************************************************************************/
tBTM_STATUS btm_ble_update_uuid_filter(tBTM_BLE_SCAN_COND_OP action,
                                       tBTM_BLE_PF_FILT_INDEX filt_index,
                                       tBTM_BLE_PF_COND_TYPE filter_type,
                                       tBTM_BLE_PF_COND_PARAM *p_cond,
                                       tBTM_BLE_FILT_CB_EVT cb_evt,
                                       tBTM_BLE_REF_VALUE ref_value)
{
    UINT8       param[BTM_BLE_META_UUID_LEN + BTM_BLE_ADV_FILT_META_HDR_LENGTH],
                * p = param,
                  len = BTM_BLE_ADV_FILT_META_HDR_LENGTH;
    tBTM_STATUS st = BTM_ILLEGAL_VALUE;
    tBTM_BLE_PF_UUID_COND *p_uuid_cond;
    UINT8           evt_type;

    memset(param, 0, BTM_BLE_META_UUID_LEN + BTM_BLE_ADV_FILT_META_HDR_LENGTH);

    if (BTM_BLE_PF_SRVC_UUID == filter_type) {
        evt_type = BTM_BLE_META_PF_UUID;
        p_uuid_cond = p_cond ? &p_cond->srvc_uuid : NULL;
    } else {
        evt_type = BTM_BLE_META_PF_SOL_UUID;
        p_uuid_cond = p_cond ? &p_cond->solicitate_uuid : NULL;
    }

    if (NULL == p_uuid_cond && action != BTM_BLE_SCAN_COND_CLEAR) {
        BTM_TRACE_ERROR("Illegal param for add/delete UUID filter");
        return st;
    }

    /* need to add address filter first, if adding per bda UUID filter without address filter */
    if (BTM_BLE_SCAN_COND_ADD == action && NULL != p_uuid_cond &&
            p_uuid_cond->p_target_addr &&
            btm_ble_find_addr_filter_counter(p_uuid_cond->p_target_addr) == NULL) {
        UINT8_TO_STREAM(p, BTM_BLE_META_PF_ADDR);
        UINT8_TO_STREAM(p, action);

        /* Filter index */
        UINT8_TO_STREAM(p, filt_index);

        BDADDR_TO_STREAM(p, p_uuid_cond->p_target_addr->bda);
        UINT8_TO_STREAM(p, p_uuid_cond->p_target_addr->type);

        /* send address filter */
        if ((st = BTM_VendorSpecificCommand (HCI_BLE_ADV_FILTER_OCF,
                                             (UINT8)(BTM_BLE_ADV_FILT_META_HDR_LENGTH + BTM_BLE_META_ADDR_LEN),
                                             param,
                                             btm_ble_scan_pf_cmpl_cback)) == BTM_NO_RESOURCES) {
            BTM_TRACE_ERROR("Update Address filter into controller failed.");
            return st;
        }

        btm_ble_advfilt_enq_op_q(action, BTM_BLE_META_PF_ADDR, cb_evt, ref_value, NULL, NULL);
        BTM_TRACE_DEBUG("Updated Address filter");
    }

    p = param;
    UINT8_TO_STREAM(p, evt_type);
    UINT8_TO_STREAM(p, action);

    /* Filter index */
    UINT8_TO_STREAM(p, filt_index);

    if ((BTM_BLE_SCAN_COND_ADD == action ||
            BTM_BLE_SCAN_COND_DELETE == action) &&
            NULL != p_uuid_cond) {
        if (p_uuid_cond->uuid.len == LEN_UUID_16) {
            UINT16_TO_STREAM(p, p_uuid_cond->uuid.uu.uuid16);
            len += LEN_UUID_16;
        } else if (p_uuid_cond->uuid.len == LEN_UUID_32) { /*4 bytes */
            UINT32_TO_STREAM(p, p_uuid_cond->uuid.uu.uuid32);
            len += LEN_UUID_32;
        } else if (p_uuid_cond->uuid.len == LEN_UUID_128) {
            ARRAY_TO_STREAM (p, p_uuid_cond->uuid.uu.uuid128, LEN_UUID_128);
            len += LEN_UUID_128;
        } else {
            BTM_TRACE_ERROR("illegal UUID length: %d", p_uuid_cond->uuid.len);
            return BTM_ILLEGAL_VALUE;
        }

        if (NULL != p_uuid_cond->p_uuid_mask) {
            if (p_uuid_cond->uuid.len == LEN_UUID_16) {
                UINT16_TO_STREAM(p, p_uuid_cond->p_uuid_mask->uuid16_mask);
                len += LEN_UUID_16;
            } else if (p_uuid_cond->uuid.len == LEN_UUID_32) { /*4 bytes */
                UINT32_TO_STREAM(p, p_uuid_cond->p_uuid_mask->uuid32_mask);
                len += LEN_UUID_32;
            } else if (p_uuid_cond->uuid.len == LEN_UUID_128) {
                ARRAY_TO_STREAM (p, p_uuid_cond->p_uuid_mask->uuid128_mask, LEN_UUID_128);
                len += LEN_UUID_128;
            }
        } else {
            memset(p, 0xff, p_uuid_cond->uuid.len);
            len += p_uuid_cond->uuid.len;
        }
        BTM_TRACE_DEBUG("btm_ble_update_uuid_filter : %d, %d, %d, %d", filter_type, evt_type,
                        p_uuid_cond->uuid.len, len);
    }

    /* send UUID filter update */
    if ((st = BTM_VendorSpecificCommand (HCI_BLE_ADV_FILTER_OCF,
                                         len,
                                         param,
                                         btm_ble_scan_pf_cmpl_cback)) != BTM_NO_RESOURCES) {
        if (p_uuid_cond && p_uuid_cond->p_target_addr) {
            memcpy(&btm_ble_adv_filt_cb.cur_filter_target, p_uuid_cond->p_target_addr,
                   sizeof(tBLE_BD_ADDR));
        }
        else {
            memset(&btm_ble_adv_filt_cb.cur_filter_target, 0, sizeof(tBLE_BD_ADDR));
        }
    } else {
        BTM_TRACE_ERROR("UUID filter udpating failed");
    }

    return st;
}


/*******************************************************************************
**
** Function         btm_ble_clear_scan_pf_filter
**
** Description      clear all adv payload filter by de-select all the adv pf feature bits
**
**
** Returns          BTM_SUCCESS if sucessful,
**                  BTM_ILLEGAL_VALUE if paramter is not valid.
**
*******************************************************************************/
tBTM_STATUS btm_ble_clear_scan_pf_filter(tBTM_BLE_SCAN_COND_OP action,
        tBTM_BLE_PF_FILT_INDEX filt_index,
        tBTM_BLE_PF_COND_PARAM *p_cond,
        tBTM_BLE_PF_CFG_CBACK *p_cmpl_cback,
        tBTM_BLE_FILT_CB_EVT cb_evt,
        tBTM_BLE_REF_VALUE ref_value)
{
    tBLE_BD_ADDR *p_target = (p_cond == NULL) ? NULL : &p_cond->target_addr;
    tBTM_BLE_PF_COUNT *p_bda_filter;
    tBTM_STATUS     st = BTM_WRONG_MODE;
    UINT8           param[20], *p;

    if (BTM_BLE_SCAN_COND_CLEAR != action) {
        BTM_TRACE_ERROR("unable to perform action:%d for generic adv filter type", action);
        return BTM_ILLEGAL_VALUE;
    }

    p = param;
    memset(param, 0, 20);

    p_bda_filter = btm_ble_find_addr_filter_counter(p_target);

    if (NULL == p_bda_filter ||
            /* not a generic filter */
            (p_target != NULL && p_bda_filter)) {
        BTM_TRACE_ERROR("Error: Can not clear filter, No PF filter has been configured!");
        return st;
    }

    /* clear the general filter entry */
    if (NULL == p_target) {
        /* clear manufacturer data filter */
        st = btm_ble_update_pf_manu_data(BTM_BLE_SCAN_COND_CLEAR, filt_index, NULL,
                                         BTM_BLE_PF_MANU_DATA, cb_evt, ref_value);
        if (BTM_CMD_STARTED == st) {
            btm_ble_advfilt_enq_op_q(action, BTM_BLE_META_PF_MANU_DATA, cb_evt,
                                     ref_value, NULL, NULL);
        }

        /* clear local name filter */
        st = btm_ble_update_pf_local_name(BTM_BLE_SCAN_COND_CLEAR, filt_index, NULL);
        if (BTM_CMD_STARTED == st) {
            btm_ble_advfilt_enq_op_q(action, BTM_BLE_META_PF_LOCAL_NAME, cb_evt,
                                     ref_value, NULL, NULL);
        }

        /* update the counter for service data */
        st = btm_ble_update_srvc_data_change(BTM_BLE_SCAN_COND_CLEAR, filt_index, NULL);

        /* clear UUID filter */
        st = btm_ble_update_uuid_filter(BTM_BLE_SCAN_COND_CLEAR, filt_index,
                                        BTM_BLE_PF_SRVC_UUID, NULL, cb_evt, ref_value);
        if (BTM_CMD_STARTED == st) {
            btm_ble_advfilt_enq_op_q(action, BTM_BLE_META_PF_UUID, cb_evt, ref_value, NULL, NULL);
        }

        st = btm_ble_update_uuid_filter(BTM_BLE_SCAN_COND_CLEAR, filt_index,
                                        BTM_BLE_PF_SRVC_SOL_UUID, NULL, cb_evt, ref_value);
        if (BTM_CMD_STARTED == st) {
            btm_ble_advfilt_enq_op_q(action, BTM_BLE_META_PF_SOL_UUID, cb_evt,
                                     ref_value, NULL, NULL);
        }

        /* clear service data filter */
        st = btm_ble_update_pf_manu_data(BTM_BLE_SCAN_COND_CLEAR, filt_index, NULL,
                                         BTM_BLE_PF_SRVC_DATA_PATTERN, cb_evt, ref_value);
        if (BTM_CMD_STARTED == st) {
            btm_ble_advfilt_enq_op_q(action, BTM_BLE_META_PF_SRVC_DATA, cb_evt,
                                     ref_value, NULL, NULL);
        }
    }

    /* select feature based on control block settings */
    UINT8_TO_STREAM(p, BTM_BLE_META_PF_FEAT_SEL);
    UINT8_TO_STREAM(p, BTM_BLE_SCAN_COND_CLEAR);

    /* Filter index */
    UINT8_TO_STREAM(p, filt_index);

    /* set PCF selection */
    UINT32_TO_STREAM(p, BTM_BLE_PF_SELECT_NONE);
    /* set logic condition as OR as default */
    UINT8_TO_STREAM(p, BTM_BLE_PF_LOGIC_OR);

    if ((st = BTM_VendorSpecificCommand (HCI_BLE_ADV_FILTER_OCF,
                                         (UINT8)(BTM_BLE_ADV_FILT_META_HDR_LENGTH + BTM_BLE_PF_FEAT_SEL_LEN),
                                         param,
                                         btm_ble_scan_pf_cmpl_cback))
            != BTM_NO_RESOURCES) {
        if (p_target) {
            memcpy(&btm_ble_adv_filt_cb.cur_filter_target, p_target, sizeof(tBLE_BD_ADDR));
        } else {
            memset(&btm_ble_adv_filt_cb.cur_filter_target, 0, sizeof(tBLE_BD_ADDR));
        }
    }
    return st;
}

/*******************************************************************************
**
** Function         BTM_BleAdvFilterParamSetup
**
** Description      This function is called to setup the adv data payload filter
**                  condition.
**
** Parameters       action - Type of action to be performed
**                       filt_index - Filter index
**                       p_filt_params - Filter parameters
**                       p_target - Target device
**                       p_cmpl_back - Callback pointer
**                       ref_value - reference value
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS BTM_BleAdvFilterParamSetup(int action, tBTM_BLE_PF_FILT_INDEX filt_index,
                                       tBTM_BLE_PF_FILT_PARAMS *p_filt_params,
                                       tBLE_BD_ADDR *p_target, tBTM_BLE_PF_PARAM_CBACK *p_cmpl_cback,
                                       tBTM_BLE_REF_VALUE ref_value)
{
    tBTM_STATUS st = BTM_WRONG_MODE;
    tBTM_BLE_PF_COUNT *p_bda_filter = NULL;
    UINT8 len = BTM_BLE_ADV_FILT_META_HDR_LENGTH + BTM_BLE_ADV_FILT_FEAT_SELN_LEN +
                BTM_BLE_ADV_FILT_TRACK_NUM;
    UINT8 param[len], *p;

    if (BTM_SUCCESS  != btm_ble_obtain_vsc_details()) {
        return st;
    }

    p = param;
    memset(param, 0, len);
    BTM_TRACE_EVENT (" BTM_BleAdvFilterParamSetup");

    if (BTM_BLE_SCAN_COND_ADD == action) {
        p_bda_filter = btm_ble_find_addr_filter_counter(p_target);
        if (NULL == p_bda_filter) {
            BTM_TRACE_ERROR("BD Address not found!");
            return st;
        }

        BTM_TRACE_DEBUG("BTM_BleAdvFilterParamSetup : Feat mask:%d", p_filt_params->feat_seln);
        /* select feature based on control block settings */
        UINT8_TO_STREAM(p, BTM_BLE_META_PF_FEAT_SEL);
        UINT8_TO_STREAM(p, BTM_BLE_SCAN_COND_ADD);

        /* Filter index */
        UINT8_TO_STREAM(p, filt_index);

        /* set PCF selection */
        UINT16_TO_STREAM(p, p_filt_params->feat_seln);
        /* set logic type */
        UINT16_TO_STREAM(p, p_filt_params->logic_type);
        /* set logic condition */
        UINT8_TO_STREAM(p, p_filt_params->filt_logic_type);
        /* set RSSI high threshold */
        UINT8_TO_STREAM(p, p_filt_params->rssi_high_thres);
        /* set delivery mode */
        UINT8_TO_STREAM(p, p_filt_params->dely_mode);

        if (0x01 == p_filt_params->dely_mode) {
            /* set onfound timeout */
            UINT16_TO_STREAM(p, p_filt_params->found_timeout);
            /* set onfound timeout count*/
            UINT8_TO_STREAM(p, p_filt_params->found_timeout_cnt);
            /* set RSSI low threshold */
            UINT8_TO_STREAM(p, p_filt_params->rssi_low_thres);
            /* set onlost timeout */
            UINT16_TO_STREAM(p, p_filt_params->lost_timeout);
            /* set num_of_track_entries for firmware greater than L-release version */
            if (cmn_ble_adv_vsc_cb.version_supported > BTM_VSC_CHIP_CAPABILITY_L_VERSION) {
                UINT16_TO_STREAM(p, p_filt_params->num_of_tracking_entries);
            }
        }

        if (cmn_ble_adv_vsc_cb.version_supported == BTM_VSC_CHIP_CAPABILITY_L_VERSION) {
            len = BTM_BLE_ADV_FILT_META_HDR_LENGTH + BTM_BLE_ADV_FILT_FEAT_SELN_LEN;
        } else {
            len = BTM_BLE_ADV_FILT_META_HDR_LENGTH + BTM_BLE_ADV_FILT_FEAT_SELN_LEN +
                  BTM_BLE_ADV_FILT_TRACK_NUM;
        }

        if ((st = BTM_VendorSpecificCommand (HCI_BLE_ADV_FILTER_OCF,
                                             (UINT8)len,
                                             param,
                                             btm_ble_scan_pf_cmpl_cback))
                == BTM_NO_RESOURCES) {
            return st;
        }
        btm_ble_advfilt_enq_op_q(action, BTM_BLE_META_PF_FEAT_SEL, BTM_BLE_FILT_ADV_PARAM,
                                 ref_value, NULL, p_cmpl_cback);
    } else if (BTM_BLE_SCAN_COND_DELETE == action) {
        /* select feature based on control block settings */
        UINT8_TO_STREAM(p, BTM_BLE_META_PF_FEAT_SEL);
        UINT8_TO_STREAM(p, BTM_BLE_SCAN_COND_DELETE);
        /* Filter index */
        UINT8_TO_STREAM(p, filt_index);

        if ((st = BTM_VendorSpecificCommand (HCI_BLE_ADV_FILTER_OCF,
                                             (UINT8)(BTM_BLE_ADV_FILT_META_HDR_LENGTH),
                                             param,
                                             btm_ble_scan_pf_cmpl_cback))
                == BTM_NO_RESOURCES) {
            return st;
        }
        btm_ble_advfilt_enq_op_q(action, BTM_BLE_META_PF_FEAT_SEL,  BTM_BLE_FILT_ADV_PARAM,
                                 ref_value, NULL, p_cmpl_cback);
    } else if (BTM_BLE_SCAN_COND_CLEAR == action) {
        /* Deallocate all filters here */
        btm_ble_dealloc_addr_filter_counter(NULL, BTM_BLE_PF_TYPE_ALL);

        /* select feature based on control block settings */
        UINT8_TO_STREAM(p, BTM_BLE_META_PF_FEAT_SEL);
        UINT8_TO_STREAM(p, BTM_BLE_SCAN_COND_CLEAR);

        if ((st = BTM_VendorSpecificCommand (HCI_BLE_ADV_FILTER_OCF,
                                             (UINT8)(BTM_BLE_ADV_FILT_META_HDR_LENGTH - 1),
                                             param,
                                             btm_ble_scan_pf_cmpl_cback))
                == BTM_NO_RESOURCES) {
            return st;
        }
        btm_ble_advfilt_enq_op_q(action, BTM_BLE_META_PF_FEAT_SEL,  BTM_BLE_FILT_ADV_PARAM,
                                 ref_value, NULL, p_cmpl_cback);
    }

    return st;
}

/*******************************************************************************
**
** Function         BTM_BleEnableDisableFilterFeature
**
** Description      This function is called to enable / disable the APCF feature
**
** Parameters  enable the generic scan condition.
**                  enable: enable or disable the filter condition
**                  p_stat_cback - Status callback pointer
**                  ref_value   - Ref value
** Returns          void
**
*******************************************************************************/
tBTM_STATUS BTM_BleEnableDisableFilterFeature(UINT8 enable,
        tBTM_BLE_PF_STATUS_CBACK *p_stat_cback,
        tBTM_BLE_REF_VALUE ref_value)
{
    UINT8           param[20], *p;
    tBTM_STATUS     st = BTM_WRONG_MODE;

    if (BTM_SUCCESS  != btm_ble_obtain_vsc_details()) {
        return st;
    }

    p = param;
    memset(param, 0, 20);

    /* enable the content filter in controller */
    p = param;
    UINT8_TO_STREAM(p, BTM_BLE_META_PF_ENABLE);
    /* enable adv data payload filtering */
    UINT8_TO_STREAM(p, enable);

    if ((st = BTM_VendorSpecificCommand (HCI_BLE_ADV_FILTER_OCF,
                                         BTM_BLE_PCF_ENABLE_LEN, param,
                                         btm_ble_scan_pf_cmpl_cback)) == BTM_CMD_STARTED) {
        btm_ble_adv_filt_cb.p_filt_stat_cback = p_stat_cback;
        btm_ble_advfilt_enq_op_q(enable, BTM_BLE_META_PF_ENABLE, BTM_BLE_FILT_ENABLE_DISABLE,
                                 ref_value, NULL, NULL);
    }
    return st;
}

/*******************************************************************************
**
** Function         BTM_BleCfgFilterCondition
**
** Description      This function is called to configure the adv data payload filter
**                  condition.
**
** Parameters       action: to read/write/clear
**                  cond_type: filter condition type.
**                  filt_index - Filter index
**                  p_cond: filter condition parameter
**                  p_cmpl_cback  - Config callback pointer
**                  ref_value - Reference value
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS BTM_BleCfgFilterCondition(tBTM_BLE_SCAN_COND_OP action,
                                      tBTM_BLE_PF_COND_TYPE cond_type,
                                      tBTM_BLE_PF_FILT_INDEX filt_index,
                                      tBTM_BLE_PF_COND_PARAM *p_cond,
                                      tBTM_BLE_PF_CFG_CBACK *p_cmpl_cback,
                                      tBTM_BLE_REF_VALUE ref_value)
{
    tBTM_STATUS     st = BTM_ILLEGAL_VALUE;
    UINT8 ocf = 0;
    BTM_TRACE_EVENT (" BTM_BleCfgFilterCondition action:%d, cond_type:%d, index:%d", action,
                     cond_type, filt_index);

    if (BTM_SUCCESS  != btm_ble_obtain_vsc_details()) {
        return st;
    }

    switch (cond_type) {
    /* write service data filter */
    case BTM_BLE_PF_SRVC_DATA_PATTERN:
    /* write manufacturer data filter */
    case BTM_BLE_PF_MANU_DATA:
        st = btm_ble_update_pf_manu_data(action, filt_index, p_cond, cond_type, 0, ref_value);
        break;

    /* write local name filter */
    case BTM_BLE_PF_LOCAL_NAME:
        st = btm_ble_update_pf_local_name(action, filt_index, p_cond);
        break;

    /* filter on advertiser address */
    case BTM_BLE_PF_ADDR_FILTER:
        st = btm_ble_update_addr_filter(action, filt_index, p_cond);
        break;

    /* filter on service/solicited UUID */
    case BTM_BLE_PF_SRVC_UUID:
    case BTM_BLE_PF_SRVC_SOL_UUID:
        st = btm_ble_update_uuid_filter(action, filt_index, cond_type, p_cond, 0, ref_value);
        break;

    case BTM_BLE_PF_SRVC_DATA:
        st = btm_ble_update_srvc_data_change(action, filt_index, p_cond);
        break;

    case BTM_BLE_PF_TYPE_ALL: /* only used to clear filter */
        st = btm_ble_clear_scan_pf_filter(action, filt_index, p_cond, p_cmpl_cback,
                                          0, ref_value);
        break;

    default:
        BTM_TRACE_WARNING("condition type [%d] not supported currently.", cond_type);
        break;
    }

    if (BTM_CMD_STARTED == st && cond_type != BTM_BLE_PF_TYPE_ALL) {
        ocf = btm_ble_condtype_to_ocf(cond_type);
        btm_ble_advfilt_enq_op_q(action, ocf, BTM_BLE_FILT_CFG, ref_value, p_cmpl_cback, NULL);
    } else if (BTM_CMD_STARTED == st && BTM_BLE_PF_TYPE_ALL == cond_type) {
        btm_ble_advfilt_enq_op_q(action, BTM_BLE_META_PF_FEAT_SEL, BTM_BLE_FILT_CFG,
                                 ref_value, p_cmpl_cback, NULL);
    }
    return st;
}

/*******************************************************************************
**
** Function         btm_ble_adv_filter_init
**
** Description      This function initializes the adv filter control block
**
** Parameters
**
** Returns          status
**
*******************************************************************************/
void btm_ble_adv_filter_init(void)
{
#if BTM_DYNAMIC_MEMORY == TRUE
    btm_ble_adv_filt_cb_ptr = (tBTM_BLE_ADV_FILTER_CB *)osi_malloc(sizeof(tBTM_BLE_ADV_FILTER_CB));
    cmn_ble_adv_vsc_cb_ptr = (tBTM_BLE_VSC_CB *)osi_malloc(sizeof(tBTM_BLE_VSC_CB));
    if (btm_ble_adv_filt_cb_ptr == NULL || cmn_ble_adv_vsc_cb_ptr == NULL) {
        BTM_TRACE_ERROR("%s malloc failed", __func__);
        return;
    }
    memset((void *)btm_ble_adv_filt_cb_ptr, 0, sizeof(tBTM_BLE_ADV_FILTER_CB));
    memset((void *)cmn_ble_adv_vsc_cb_ptr, 0, sizeof(tBTM_BLE_VSC_CB));
#endif
    memset(&btm_ble_adv_filt_cb, 0, sizeof(tBTM_BLE_ADV_FILTER_CB));
    if (BTM_SUCCESS != btm_ble_obtain_vsc_details()) {
        return;
    }

    if (cmn_ble_adv_vsc_cb.max_filter > 0) {
        btm_ble_adv_filt_cb.p_addr_filter_count =
            (tBTM_BLE_PF_COUNT *) osi_malloc( sizeof(tBTM_BLE_PF_COUNT) * cmn_ble_adv_vsc_cb.max_filter);
    }
}

/*******************************************************************************
**
** Function         btm_ble_adv_filter_cleanup
**
** Description      This function de-initializes the adv filter control block
**
** Parameters
**
** Returns          status
**
*******************************************************************************/
void btm_ble_adv_filter_cleanup(void)
{
    if (btm_ble_adv_filt_cb.p_addr_filter_count) {
        osi_free(btm_ble_adv_filt_cb.p_addr_filter_count);
        btm_ble_adv_filt_cb.p_addr_filter_count = NULL;
    }

#if BTM_DYNAMIC_MEMORY == TRUE
    osi_free(btm_ble_adv_filt_cb_ptr);
    btm_ble_adv_filt_cb_ptr = NULL;
    osi_free(cmn_ble_adv_vsc_cb_ptr);
    cmn_ble_adv_vsc_cb_ptr = NULL;
#endif
}

#endif

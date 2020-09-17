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

#include <string.h>

#include "common/bt_target.h"
#include "device/controller.h"

#if (BLE_INCLUDED == TRUE)
#include "stack/bt_types.h"
#include "stack/hcimsgs.h"
#include "stack/btu.h"
#include "btm_int.h"
//#include "bt_utils.h"
#include "stack/hcidefs.h"
#include "stack/btm_ble_api.h"

/************************************************************************************
**  Constants & Macros
************************************************************************************/
/* length of each multi adv sub command */
#define BTM_BLE_MULTI_ADV_ENB_LEN                       3
#define BTM_BLE_MULTI_ADV_SET_PARAM_LEN                 24
#define BTM_BLE_MULTI_ADV_WRITE_DATA_LEN                (BTM_BLE_AD_DATA_LEN + 3)
#define BTM_BLE_MULTI_ADV_SET_RANDOM_ADDR_LEN           8

#define BTM_BLE_MULTI_ADV_CB_EVT_MASK   0xF0
#define BTM_BLE_MULTI_ADV_SUBCODE_MASK  0x0F

/************************************************************************************
**  Static variables
************************************************************************************/
#if BTM_DYNAMIC_MEMORY == FALSE
tBTM_BLE_MULTI_ADV_CB           btm_multi_adv_cb;
tBTM_BLE_MULTI_ADV_INST_IDX_Q   btm_multi_adv_idx_q;
#else
tBTM_BLE_MULTI_ADV_CB           *btm_multi_adv_cb_ptr;
tBTM_BLE_MULTI_ADV_INST_IDX_Q   *btm_multi_adv_idx_q_ptr;
#define btm_multi_adv_cb        (*btm_multi_adv_cb_ptr)
#define btm_multi_adv_idx_q     (*btm_multi_adv_idx_q_ptr)
#endif

/************************************************************************************
**  Externs
************************************************************************************/
extern void btm_ble_update_dmt_flag_bits(UINT8 *flag_value,
        const UINT16 connect_mode, const UINT16 disc_mode);

/*******************************************************************************
**
** Function         btm_ble_multi_adv_enq_op_q
**
** Description      enqueue a multi adv operation in q to check command complete
**                  status.
**
** Returns          void
**
*******************************************************************************/
void btm_ble_multi_adv_enq_op_q(UINT8 opcode, UINT8 inst_id, UINT8 cb_evt)
{
    tBTM_BLE_MULTI_ADV_OPQ  *p_op_q = &btm_multi_adv_cb.op_q;

    p_op_q->p_inst_id[p_op_q->next_idx] = inst_id;

    p_op_q->p_sub_code[p_op_q->next_idx] = (opcode | (cb_evt << 4));

    p_op_q->next_idx = (p_op_q->next_idx + 1) %  BTM_BleMaxMultiAdvInstanceCount();
}

/*******************************************************************************
**
** Function         btm_ble_multi_adv_deq_op_q
**
** Description      dequeue a multi adv operation from q when command complete
**                  is received.
**
** Returns          void
**
*******************************************************************************/
void btm_ble_multi_adv_deq_op_q(UINT8 *p_opcode, UINT8 *p_inst_id, UINT8 *p_cb_evt)
{
    tBTM_BLE_MULTI_ADV_OPQ  *p_op_q = &btm_multi_adv_cb.op_q;

    *p_inst_id = p_op_q->p_inst_id[p_op_q->pending_idx] & 0x7F;
    *p_cb_evt = (p_op_q->p_sub_code[p_op_q->pending_idx] >> 4);
    *p_opcode = (p_op_q->p_sub_code[p_op_q->pending_idx] & BTM_BLE_MULTI_ADV_SUBCODE_MASK);

    p_op_q->pending_idx = (p_op_q->pending_idx + 1) %  BTM_BleMaxMultiAdvInstanceCount();
}

/*******************************************************************************
**
** Function         btm_ble_multi_adv_vsc_cmpl_cback
**
** Description      Multi adv VSC complete callback
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
void btm_ble_multi_adv_vsc_cmpl_cback (tBTM_VSC_CMPL *p_params)
{
    UINT8  status, subcode;
    UINT8  *p = p_params->p_param_buf, inst_id;
    UINT16  len = p_params->param_len;
    tBTM_BLE_MULTI_ADV_INST *p_inst ;
    UINT8   cb_evt = 0, opcode;

    if (len  < 2) {
        BTM_TRACE_ERROR("wrong length for btm_ble_multi_adv_vsc_cmpl_cback");
        return;
    }

    STREAM_TO_UINT8(status, p);
    STREAM_TO_UINT8(subcode, p);

    btm_ble_multi_adv_deq_op_q(&opcode, &inst_id, &cb_evt);

    BTM_TRACE_DEBUG("op_code = %02x inst_id = %d cb_evt = %02x", opcode, inst_id, cb_evt);

    if (opcode != subcode || inst_id == 0) {
        BTM_TRACE_ERROR("get unexpected VSC cmpl, expect: %d get: %d", subcode, opcode);
        return;
    }

    p_inst = &btm_multi_adv_cb.p_adv_inst[inst_id - 1];

    switch (subcode) {
    case BTM_BLE_MULTI_ADV_ENB: {
        BTM_TRACE_DEBUG("BTM_BLE_MULTI_ADV_ENB status = %d", status);

        /* Mark as not in use here, if instance cannot be enabled */
        if (HCI_SUCCESS != status && BTM_BLE_MULTI_ADV_ENB_EVT == cb_evt) {
            btm_multi_adv_cb.p_adv_inst[inst_id - 1].in_use = FALSE;
        }
        break;
    }

    case BTM_BLE_MULTI_ADV_SET_PARAM: {
        BTM_TRACE_DEBUG("BTM_BLE_MULTI_ADV_SET_PARAM status = %d", status);
        break;
    }

    case BTM_BLE_MULTI_ADV_WRITE_ADV_DATA: {
        BTM_TRACE_DEBUG("BTM_BLE_MULTI_ADV_WRITE_ADV_DATA status = %d", status);
        break;
    }

    case BTM_BLE_MULTI_ADV_WRITE_SCAN_RSP_DATA: {
        BTM_TRACE_DEBUG("BTM_BLE_MULTI_ADV_WRITE_SCAN_RSP_DATA status = %d", status);
        break;
    }

    case BTM_BLE_MULTI_ADV_SET_RANDOM_ADDR: {
        BTM_TRACE_DEBUG("BTM_BLE_MULTI_ADV_SET_RANDOM_ADDR status = %d", status);
        break;
    }

    default:
        break;
    }

    if (cb_evt != 0 && p_inst->p_cback != NULL) {
        (p_inst->p_cback)(cb_evt, inst_id, p_inst->p_ref, status);
    }
    return;
}

/*******************************************************************************
**
** Function         btm_ble_enable_multi_adv
**
** Description      This function enable the customer specific feature in controller
**
** Parameters       enable: enable or disable
**                  inst_id:    adv instance ID, can not be 0
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS btm_ble_enable_multi_adv (BOOLEAN enable, UINT8 inst_id, UINT8 cb_evt)
{
    UINT8           param[BTM_BLE_MULTI_ADV_ENB_LEN], *pp;
    UINT8           enb = enable ? 1 : 0;
    tBTM_STATUS     rt;

    pp = param;
    memset(param, 0, BTM_BLE_MULTI_ADV_ENB_LEN);

    UINT8_TO_STREAM (pp, BTM_BLE_MULTI_ADV_ENB);
    UINT8_TO_STREAM (pp, enb);
    UINT8_TO_STREAM (pp, inst_id);

    BTM_TRACE_EVENT (" btm_ble_enable_multi_adv: enb %d, Inst ID %d", enb, inst_id);

    if ((rt = BTM_VendorSpecificCommand (HCI_BLE_MULTI_ADV_OCF,
                                         BTM_BLE_MULTI_ADV_ENB_LEN,
                                         param,
                                         btm_ble_multi_adv_vsc_cmpl_cback))
            == BTM_CMD_STARTED) {
        btm_ble_multi_adv_enq_op_q(BTM_BLE_MULTI_ADV_ENB, inst_id, cb_evt);
    }
    return rt;
}
/*******************************************************************************
**
** Function         btm_ble_map_adv_tx_power
**
** Description      return the actual power in dBm based on the mapping in config file
**
** Parameters       advertise parameters used for this instance.
**
** Returns          tx power in dBm
**
*******************************************************************************/
static const int btm_ble_tx_power[BTM_BLE_ADV_TX_POWER_MAX + 1] = BTM_BLE_ADV_TX_POWER;
char btm_ble_map_adv_tx_power(int tx_power_index)
{
    if (0 <= tx_power_index && tx_power_index <= BTM_BLE_ADV_TX_POWER_MAX) {
        return (char)btm_ble_tx_power[tx_power_index];
    }
    return 0;
}
/*******************************************************************************
**
** Function         btm_ble_multi_adv_set_params
**
** Description      This function enable the customer specific feature in controller
**
** Parameters       advertise parameters used for this instance.
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS btm_ble_multi_adv_set_params (tBTM_BLE_MULTI_ADV_INST *p_inst,
        tBTM_BLE_ADV_PARAMS *p_params,
        UINT8 cb_evt)
{
    UINT8           param[BTM_BLE_MULTI_ADV_SET_PARAM_LEN], *pp;
    tBTM_STATUS     rt;
    BD_ADDR         dummy = {0, 0, 0, 0, 0, 0};

    pp = param;
    memset(param, 0, BTM_BLE_MULTI_ADV_SET_PARAM_LEN);

    UINT8_TO_STREAM(pp, BTM_BLE_MULTI_ADV_SET_PARAM);

    UINT16_TO_STREAM (pp, p_params->adv_int_min);
    UINT16_TO_STREAM (pp, p_params->adv_int_max);
    UINT8_TO_STREAM  (pp, p_params->adv_type);

#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
    if (btm_cb.ble_ctr_cb.privacy_mode != BTM_PRIVACY_NONE) {
        UINT8_TO_STREAM  (pp, BLE_ADDR_RANDOM);
        BDADDR_TO_STREAM (pp, p_inst->rpa);
    } else
#endif
    {
        UINT8_TO_STREAM  (pp, BLE_ADDR_PUBLIC);
        BDADDR_TO_STREAM (pp, controller_get_interface()->get_address()->address);
    }

    BTM_TRACE_EVENT (" btm_ble_multi_adv_set_params,Min %d, Max %d,adv_type %d",
                     p_params->adv_int_min, p_params->adv_int_max, p_params->adv_type);

    UINT8_TO_STREAM  (pp, 0);
    BDADDR_TO_STREAM (pp, dummy);

    if (p_params->channel_map == 0 || p_params->channel_map > BTM_BLE_DEFAULT_ADV_CHNL_MAP) {
        p_params->channel_map = BTM_BLE_DEFAULT_ADV_CHNL_MAP;
    }
    UINT8_TO_STREAM (pp, p_params->channel_map);

    if (p_params->adv_filter_policy >= AP_SCAN_CONN_POLICY_MAX) {
        p_params->adv_filter_policy = AP_SCAN_CONN_ALL;
    }
    UINT8_TO_STREAM (pp, p_params->adv_filter_policy);

    UINT8_TO_STREAM (pp, p_inst->inst_id);

    if (p_params->tx_power > BTM_BLE_ADV_TX_POWER_MAX) {
        p_params->tx_power = BTM_BLE_ADV_TX_POWER_MAX;
    }
    UINT8_TO_STREAM (pp, btm_ble_map_adv_tx_power(p_params->tx_power));

    BTM_TRACE_EVENT("set_params:Chnl Map %d,adv_fltr policy %d,ID:%d, TX Power%d",
                    p_params->channel_map, p_params->adv_filter_policy, p_inst->inst_id, p_params->tx_power);

    if ((rt = BTM_VendorSpecificCommand (HCI_BLE_MULTI_ADV_OCF,
                                         BTM_BLE_MULTI_ADV_SET_PARAM_LEN,
                                         param,
                                         btm_ble_multi_adv_vsc_cmpl_cback))
            == BTM_CMD_STARTED) {
        p_inst->adv_evt = p_params->adv_type;

#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
        if (btm_cb.ble_ctr_cb.privacy_mode != BTM_PRIVACY_NONE) {
            /* start timer */
            p_inst->raddr_timer_ent.param = (TIMER_PARAM_TYPE) p_inst;
            btu_start_timer_oneshot(&p_inst->raddr_timer_ent, BTU_TTYPE_BLE_RANDOM_ADDR,
                                    BTM_BLE_PRIVATE_ADDR_INT);
        }
#endif
        btm_ble_multi_adv_enq_op_q(BTM_BLE_MULTI_ADV_SET_PARAM, p_inst->inst_id, cb_evt);
    }
    return rt;
}

/*******************************************************************************
**
** Function         btm_ble_multi_adv_write_rpa
**
** Description      This function write the random address for the adv instance into
**                  controller
**
** Parameters
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS btm_ble_multi_adv_write_rpa (tBTM_BLE_MULTI_ADV_INST *p_inst, BD_ADDR random_addr)
{
    UINT8           param[BTM_BLE_MULTI_ADV_SET_RANDOM_ADDR_LEN], *pp = param;
    tBTM_STATUS     rt;

    BTM_TRACE_EVENT ("%s-BD_ADDR:%02x-%02x-%02x-%02x-%02x-%02x,inst_id:%d",
                     __FUNCTION__, random_addr[5], random_addr[4], random_addr[3], random_addr[2],
                     random_addr[1], random_addr[0], p_inst->inst_id);

    memset(param, 0, BTM_BLE_MULTI_ADV_SET_RANDOM_ADDR_LEN);

    UINT8_TO_STREAM (pp, BTM_BLE_MULTI_ADV_SET_RANDOM_ADDR);
    BDADDR_TO_STREAM(pp, random_addr);
    UINT8_TO_STREAM(pp,  p_inst->inst_id);

    if ((rt = BTM_VendorSpecificCommand (HCI_BLE_MULTI_ADV_OCF,
                                         BTM_BLE_MULTI_ADV_SET_RANDOM_ADDR_LEN,
                                         param,
                                         btm_ble_multi_adv_vsc_cmpl_cback)) == BTM_CMD_STARTED) {
        /* start a periodical timer to refresh random addr */
        btu_stop_timer_oneshot(&p_inst->raddr_timer_ent);
        p_inst->raddr_timer_ent.param = (TIMER_PARAM_TYPE) p_inst;
        btu_start_timer_oneshot(&p_inst->raddr_timer_ent, BTU_TTYPE_BLE_RANDOM_ADDR,
                                BTM_BLE_PRIVATE_ADDR_INT);

        btm_ble_multi_adv_enq_op_q(BTM_BLE_MULTI_ADV_SET_RANDOM_ADDR, p_inst->inst_id, 0);
    }
    return rt;
}

/*******************************************************************************
**
** Function         btm_ble_multi_adv_gen_rpa_cmpl
**
** Description      RPA generation completion callback for each adv instance. Will
**                  continue write the new RPA into controller.
**
** Returns          none.
**
*******************************************************************************/
void btm_ble_multi_adv_gen_rpa_cmpl(tBTM_RAND_ENC *p)
{
#if (SMP_INCLUDED == TRUE)
    tSMP_ENC    output;
    UINT8 index = 0;
    tBTM_BLE_MULTI_ADV_INST *p_inst = NULL;

    /* Retrieve the index of adv instance from stored Q */
    if (btm_multi_adv_idx_q.front == -1) {
        BTM_TRACE_ERROR(" %s can't locate advertise instance", __FUNCTION__);
        return;
    } else {
        index = btm_multi_adv_idx_q.inst_index_queue[btm_multi_adv_idx_q.front];
        if (btm_multi_adv_idx_q.front == btm_multi_adv_idx_q.rear) {
            btm_multi_adv_idx_q.front = -1;
            btm_multi_adv_idx_q.rear = -1;
        } else {
            btm_multi_adv_idx_q.front = (btm_multi_adv_idx_q.front + 1) % BTM_BLE_MULTI_ADV_MAX;
        }
    }

    p_inst = &(btm_multi_adv_cb.p_adv_inst[index]);

    BTM_TRACE_EVENT ("btm_ble_multi_adv_gen_rpa_cmpl inst_id = %d", p_inst->inst_id);
    if (p) {
        p->param_buf[2] &= (~BLE_RESOLVE_ADDR_MASK);
        p->param_buf[2] |= BLE_RESOLVE_ADDR_MSB;

        p_inst->rpa[2] = p->param_buf[0];
        p_inst->rpa[1] = p->param_buf[1];
        p_inst->rpa[0] = p->param_buf[2];

        if (!SMP_Encrypt(btm_cb.devcb.id_keys.irk, BT_OCTET16_LEN, p->param_buf, 3, &output)) {
            BTM_TRACE_DEBUG("generate random address failed");
        } else {
            /* set hash to be LSB of rpAddress */
            p_inst->rpa[5] = output.param_buf[0];
            p_inst->rpa[4] = output.param_buf[1];
            p_inst->rpa[3] = output.param_buf[2];
        }

        if (p_inst->inst_id != BTM_BLE_MULTI_ADV_DEFAULT_STD &&
                p_inst->inst_id < BTM_BleMaxMultiAdvInstanceCount()) {
            /* set it to controller */
            btm_ble_multi_adv_write_rpa(p_inst, p_inst->rpa);
        }
    }
#endif
}

/*******************************************************************************
**
** Function         btm_ble_multi_adv_configure_rpa
**
** Description      This function set the random address for the adv instance
**
** Parameters       advertise parameters used for this instance.
**
** Returns          none
**
*******************************************************************************/
void btm_ble_multi_adv_configure_rpa (tBTM_BLE_MULTI_ADV_INST *p_inst)
{
    if (btm_multi_adv_idx_q.front == (btm_multi_adv_idx_q.rear + 1) % BTM_BLE_MULTI_ADV_MAX) {
        BTM_TRACE_ERROR("outstanding rand generation exceeded max allowed ");
        return;
    } else {
        if (btm_multi_adv_idx_q.front == -1) {
            btm_multi_adv_idx_q.front = 0;
            btm_multi_adv_idx_q.rear = 0;
        } else {
            btm_multi_adv_idx_q.rear = (btm_multi_adv_idx_q.rear + 1) % BTM_BLE_MULTI_ADV_MAX;
        }
        btm_multi_adv_idx_q.inst_index_queue[btm_multi_adv_idx_q.rear] = p_inst->index;
    }
    btm_gen_resolvable_private_addr((void *)btm_ble_multi_adv_gen_rpa_cmpl);
}

/*******************************************************************************
**
** Function         btm_ble_multi_adv_reenable
**
** Description      This function re-enable adv instance upon a connection establishment.
**
** Parameters       advertise parameters used for this instance.
**
** Returns          none.
**
*******************************************************************************/
void btm_ble_multi_adv_reenable(UINT8 inst_id)
{
    tBTM_BLE_MULTI_ADV_INST *p_inst = &btm_multi_adv_cb.p_adv_inst[inst_id - 1];

    if (TRUE == p_inst->in_use) {
        if (p_inst->adv_evt != BTM_BLE_CONNECT_DIR_EVT) {
            btm_ble_enable_multi_adv (TRUE, p_inst->inst_id, 0);
        } else
            /* mark directed adv as disabled if adv has been stopped */
        {
            (p_inst->p_cback)(BTM_BLE_MULTI_ADV_DISABLE_EVT, p_inst->inst_id, p_inst->p_ref, 0);
            p_inst->in_use = FALSE;
        }
    }
}

/*******************************************************************************
**
** Function         btm_ble_multi_adv_enb_privacy
**
** Description      This function enable/disable privacy setting in multi adv
**
** Parameters       enable: enable or disable the adv instance.
**
** Returns          none.
**
*******************************************************************************/
void btm_ble_multi_adv_enb_privacy(BOOLEAN enable)
{
    UINT8 i;
    tBTM_BLE_MULTI_ADV_INST *p_inst = &btm_multi_adv_cb.p_adv_inst[0];

    for (i = 0; i <  BTM_BleMaxMultiAdvInstanceCount() - 1; i ++, p_inst++) {
        p_inst->in_use = FALSE;
        if (enable) {
            btm_ble_multi_adv_configure_rpa (p_inst);
        } else {
            btu_stop_timer_oneshot(&p_inst->raddr_timer_ent);
        }
    }
}

/*******************************************************************************
**
** Function         BTM_BleEnableAdvInstance
**
** Description      This function enable a Multi-ADV instance with the specified
**                  adv parameters
**
** Parameters       p_params: pointer to the adv parameter structure, set as default
**                            adv parameter when the instance is enabled.
**                  p_cback: callback function for the adv instance.
**                  p_ref:  reference data attach to the adv instance to be enabled.
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS BTM_BleEnableAdvInstance (tBTM_BLE_ADV_PARAMS *p_params,
                                      tBTM_BLE_MULTI_ADV_CBACK *p_cback, void *p_ref)
{
    UINT8 i;
    tBTM_STATUS rt = BTM_NO_RESOURCES;
    tBTM_BLE_MULTI_ADV_INST *p_inst = &btm_multi_adv_cb.p_adv_inst[0];

    BTM_TRACE_EVENT("BTM_BleEnableAdvInstance called");

    if (0 == btm_cb.cmn_ble_vsc_cb.adv_inst_max) {
        BTM_TRACE_ERROR("Controller does not support Multi ADV");
        return BTM_ERR_PROCESSING;
    }

    if (NULL == p_inst) {
        BTM_TRACE_ERROR("Invalid instance in BTM_BleEnableAdvInstance");
        return BTM_ERR_PROCESSING;
    }

    for (i = 0; i <  BTM_BleMaxMultiAdvInstanceCount() - 1; i ++, p_inst++) {
        if (FALSE == p_inst->in_use) {
            p_inst->in_use = TRUE;
            /* configure adv parameter */
            if (p_params) {
                rt = btm_ble_multi_adv_set_params(p_inst, p_params, 0);
            } else {
                rt = BTM_CMD_STARTED;
            }

            /* enable adv */
            BTM_TRACE_EVENT("btm_ble_enable_multi_adv being called with inst_id:%d",
                            p_inst->inst_id);

            if (BTM_CMD_STARTED == rt) {
                if ((rt = btm_ble_enable_multi_adv (TRUE, p_inst->inst_id,
                                                    BTM_BLE_MULTI_ADV_ENB_EVT)) == BTM_CMD_STARTED) {
                    p_inst->p_cback = p_cback;
                    p_inst->p_ref   = p_ref;
                }
            }

            if (BTM_CMD_STARTED != rt) {
                p_inst->in_use = FALSE;
                BTM_TRACE_ERROR("BTM_BleEnableAdvInstance failed");
            }
            break;
        }
    }
    return rt;
}

/*******************************************************************************
**
** Function         BTM_BleUpdateAdvInstParam
**
** Description      This function update a Multi-ADV instance with the specified
**                  adv parameters.
**
** Parameters       inst_id: adv instance ID
**                  p_params: pointer to the adv parameter structure.
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS BTM_BleUpdateAdvInstParam (UINT8 inst_id, tBTM_BLE_ADV_PARAMS *p_params)
{
    tBTM_STATUS rt = BTM_ILLEGAL_VALUE;
    tBTM_BLE_MULTI_ADV_INST *p_inst = &btm_multi_adv_cb.p_adv_inst[inst_id - 1];

    BTM_TRACE_EVENT("BTM_BleUpdateAdvInstParam called with inst_id:%d", inst_id);

    if (0 == btm_cb.cmn_ble_vsc_cb.adv_inst_max) {
        BTM_TRACE_ERROR("Controller does not support Multi ADV");
        return BTM_ERR_PROCESSING;
    }

    if (inst_id <  BTM_BleMaxMultiAdvInstanceCount() &&
            inst_id != BTM_BLE_MULTI_ADV_DEFAULT_STD &&
            p_params != NULL) {
        if (FALSE == p_inst->in_use) {
            BTM_TRACE_DEBUG("adv instance %d is not active", inst_id);
            return BTM_WRONG_MODE;
        } else {
            btm_ble_enable_multi_adv(FALSE, inst_id, 0);
        }

        if (BTM_CMD_STARTED == btm_ble_multi_adv_set_params(p_inst, p_params, 0)) {
            rt = btm_ble_enable_multi_adv(TRUE, inst_id, BTM_BLE_MULTI_ADV_PARAM_EVT);
        }
    }
    return rt;
}

/*******************************************************************************
**
** Function         BTM_BleCfgAdvInstData
**
** Description      This function configure a Multi-ADV instance with the specified
**                  adv data or scan response data.
**
** Parameters       inst_id: adv instance ID
**                  is_scan_rsp: is this scan response. if no, set as adv data.
**                  data_mask: adv data mask.
**                  p_data: pointer to the adv data structure.
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS BTM_BleCfgAdvInstData (UINT8 inst_id, BOOLEAN is_scan_rsp,
                                   tBTM_BLE_AD_MASK data_mask,
                                   tBTM_BLE_ADV_DATA *p_data)
{
    UINT8       param[BTM_BLE_MULTI_ADV_WRITE_DATA_LEN], *pp = param;
    UINT8       sub_code = (is_scan_rsp) ?
                           BTM_BLE_MULTI_ADV_WRITE_SCAN_RSP_DATA : BTM_BLE_MULTI_ADV_WRITE_ADV_DATA;
    UINT8       *p_len;
    tBTM_STATUS rt;
    UINT8 *pp_temp = (UINT8 *)(param + BTM_BLE_MULTI_ADV_WRITE_DATA_LEN - 1);
    tBTM_BLE_VSC_CB cmn_ble_vsc_cb;

    BTM_BleGetVendorCapabilities(&cmn_ble_vsc_cb);
    if (0 == cmn_ble_vsc_cb.adv_inst_max) {
        BTM_TRACE_ERROR("Controller does not support Multi ADV");
        return BTM_ERR_PROCESSING;
    }

    btm_ble_update_dmt_flag_bits(&p_data->flag, btm_cb.btm_inq_vars.connectable_mode,
                                 btm_cb.btm_inq_vars.discoverable_mode);

    BTM_TRACE_EVENT("BTM_BleCfgAdvInstData called with inst_id:%d", inst_id);
    if (inst_id > BTM_BLE_MULTI_ADV_MAX || inst_id == BTM_BLE_MULTI_ADV_DEFAULT_STD) {
        return BTM_ILLEGAL_VALUE;
    }

    memset(param, 0, BTM_BLE_MULTI_ADV_WRITE_DATA_LEN);

    UINT8_TO_STREAM(pp, sub_code);
    p_len = pp ++;
    btm_ble_build_adv_data(&data_mask, &pp, p_data);
    *p_len = (UINT8)(pp - param - 2);
    UINT8_TO_STREAM(pp_temp, inst_id);

    if ((rt = BTM_VendorSpecificCommand (HCI_BLE_MULTI_ADV_OCF,
                                         (UINT8)BTM_BLE_MULTI_ADV_WRITE_DATA_LEN,
                                         param,
                                         btm_ble_multi_adv_vsc_cmpl_cback))
            == BTM_CMD_STARTED) {
        btm_ble_multi_adv_enq_op_q(sub_code, inst_id, BTM_BLE_MULTI_ADV_DATA_EVT);
    }
    return rt;
}

/*******************************************************************************
**
** Function         BTM_BleDisableAdvInstance
**
** Description      This function disables a Multi-ADV instance.
**
** Parameters       inst_id: adv instance ID
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS BTM_BleDisableAdvInstance (UINT8 inst_id)
{
    tBTM_STATUS rt = BTM_ILLEGAL_VALUE;
    tBTM_BLE_VSC_CB cmn_ble_vsc_cb;

    BTM_TRACE_EVENT("BTM_BleDisableAdvInstance with inst_id:%d", inst_id);

    BTM_BleGetVendorCapabilities(&cmn_ble_vsc_cb);

    if (0 == cmn_ble_vsc_cb.adv_inst_max) {
        BTM_TRACE_ERROR("Controller does not support Multi ADV");
        return BTM_ERR_PROCESSING;
    }

    if (inst_id < BTM_BleMaxMultiAdvInstanceCount() &&
            inst_id != BTM_BLE_MULTI_ADV_DEFAULT_STD) {
        if ((rt = btm_ble_enable_multi_adv(FALSE, inst_id, BTM_BLE_MULTI_ADV_DISABLE_EVT))
                == BTM_CMD_STARTED) {
            btm_ble_multi_adv_configure_rpa(&btm_multi_adv_cb.p_adv_inst[inst_id - 1]);
            btu_stop_timer_oneshot(&btm_multi_adv_cb.p_adv_inst[inst_id - 1].raddr_timer_ent);
            btm_multi_adv_cb.p_adv_inst[inst_id - 1].in_use = FALSE;
        }
    }
    return rt;
}
/*******************************************************************************
**
** Function         btm_ble_multi_adv_vse_cback
**
** Description      VSE callback for multi adv events.
**
** Returns
**
*******************************************************************************/
void btm_ble_multi_adv_vse_cback(UINT8 len, UINT8 *p)
{
    UINT8   sub_event;
    UINT8   adv_inst, idx;
    UINT16  conn_handle;

    /* Check if this is a BLE RSSI vendor specific event */
    STREAM_TO_UINT8(sub_event, p);
    len--;

    BTM_TRACE_EVENT("btm_ble_multi_adv_vse_cback called with event:%d", sub_event);
    if ((sub_event == HCI_VSE_SUBCODE_BLE_MULTI_ADV_ST_CHG) && (len >= 4)) {
        STREAM_TO_UINT8(adv_inst, p);
        ++p;
        STREAM_TO_UINT16(conn_handle, p);

        if ((idx = btm_handle_to_acl_index(conn_handle)) != MAX_L2CAP_LINKS) {
#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
            if (btm_cb.ble_ctr_cb.privacy_mode != BTM_PRIVACY_NONE &&
                    adv_inst <= BTM_BLE_MULTI_ADV_MAX && adv_inst !=  BTM_BLE_MULTI_ADV_DEFAULT_STD) {
                memcpy(btm_cb.acl_db[idx].conn_addr, btm_multi_adv_cb.p_adv_inst[adv_inst - 1].rpa,
                       BD_ADDR_LEN);
            }
#endif
        }

        if (adv_inst < BTM_BleMaxMultiAdvInstanceCount() &&
                adv_inst !=  BTM_BLE_MULTI_ADV_DEFAULT_STD) {
            BTM_TRACE_EVENT("btm_ble_multi_adv_reenable called");
            btm_ble_multi_adv_reenable(adv_inst);
        }
        /* re-enable connectibility */
        else if (adv_inst == BTM_BLE_MULTI_ADV_DEFAULT_STD) {
            if (btm_cb.ble_ctr_cb.inq_var.connectable_mode == BTM_BLE_CONNECTABLE) {
                btm_ble_set_connectability ( btm_cb.ble_ctr_cb.inq_var.connectable_mode );
            }
        }

    }

}
/*******************************************************************************
**
** Function         btm_ble_multi_adv_init
**
** Description      This function initialize the multi adv control block.
**
** Parameters       None
**
** Returns          void
**
*******************************************************************************/
void btm_ble_multi_adv_init(void)
{
#if BTM_DYNAMIC_MEMORY == TRUE
    btm_multi_adv_cb_ptr = (tBTM_BLE_MULTI_ADV_CB *)osi_malloc(sizeof(tBTM_BLE_MULTI_ADV_CB));
    btm_multi_adv_idx_q_ptr = (tBTM_BLE_MULTI_ADV_INST_IDX_Q *)osi_malloc(sizeof(tBTM_BLE_MULTI_ADV_INST_IDX_Q));
    if (btm_multi_adv_cb_ptr == NULL || btm_multi_adv_idx_q_ptr == NULL) {
        BTM_TRACE_ERROR("%s malloc failed", __func__);
        return;
    }
#endif

    UINT8 i = 0;
    memset(&btm_multi_adv_cb, 0, sizeof(tBTM_BLE_MULTI_ADV_CB));
    memset (&btm_multi_adv_idx_q, 0, sizeof (tBTM_BLE_MULTI_ADV_INST_IDX_Q));
    btm_multi_adv_idx_q.front = -1;
    btm_multi_adv_idx_q.rear = -1;

    if (btm_cb.cmn_ble_vsc_cb.adv_inst_max > 0) {
        btm_multi_adv_cb.p_adv_inst = osi_malloc( sizeof(tBTM_BLE_MULTI_ADV_INST) *
                                      (btm_cb.cmn_ble_vsc_cb.adv_inst_max));
        memset(btm_multi_adv_cb.p_adv_inst, 0, sizeof(tBTM_BLE_MULTI_ADV_INST) *
               (btm_cb.cmn_ble_vsc_cb.adv_inst_max));

        btm_multi_adv_cb.op_q.p_sub_code = osi_malloc( sizeof(UINT8) *
                                           (btm_cb.cmn_ble_vsc_cb.adv_inst_max));
        memset(btm_multi_adv_cb.op_q.p_sub_code, 0,
               sizeof(UINT8) * (btm_cb.cmn_ble_vsc_cb.adv_inst_max));

        btm_multi_adv_cb.op_q.p_inst_id = osi_malloc( sizeof(UINT8) *
                                          (btm_cb.cmn_ble_vsc_cb.adv_inst_max));
        memset(btm_multi_adv_cb.op_q.p_inst_id, 0,
               sizeof(UINT8) * (btm_cb.cmn_ble_vsc_cb.adv_inst_max));
    }

    /* Initialize adv instance indices and IDs. */
    for (i = 0; i < btm_cb.cmn_ble_vsc_cb.adv_inst_max; i++) {
        btm_multi_adv_cb.p_adv_inst[i].index = i;
        btm_multi_adv_cb.p_adv_inst[i].inst_id = i + 1;
    }

    BTM_RegisterForVSEvents(btm_ble_multi_adv_vse_cback, TRUE);
}

/*******************************************************************************
**
** Function         btm_ble_multi_adv_cleanup
**
** Description      This function cleans up multi adv control block.
**
** Parameters
** Returns          void
**
*******************************************************************************/
void btm_ble_multi_adv_cleanup(void)
{
    if (btm_multi_adv_cb.p_adv_inst) {
        osi_free(btm_multi_adv_cb.p_adv_inst);
        btm_multi_adv_cb.p_adv_inst = NULL;
    }

    if (btm_multi_adv_cb.op_q.p_sub_code) {
        osi_free(btm_multi_adv_cb.op_q.p_sub_code);
        btm_multi_adv_cb.op_q.p_sub_code = NULL;
    }

    if (btm_multi_adv_cb.op_q.p_inst_id) {
        osi_free(btm_multi_adv_cb.op_q.p_inst_id);
        btm_multi_adv_cb.op_q.p_inst_id = NULL;
    }

#if BTM_DYNAMIC_MEMORY == TRUE
    if(btm_multi_adv_cb_ptr) {
       osi_free(btm_multi_adv_cb_ptr);
       btm_multi_adv_cb_ptr = NULL;
    }
    if(btm_multi_adv_idx_q_ptr) {
       osi_free(btm_multi_adv_idx_q_ptr);
       btm_multi_adv_idx_q_ptr = NULL;
    }
#endif
}

/*******************************************************************************
**
** Function         btm_ble_multi_adv_get_ref
**
** Description      This function obtains the reference pointer for the instance ID provided
**
** Parameters       inst_id - Instance ID
**
** Returns          void*
**
*******************************************************************************/
void *btm_ble_multi_adv_get_ref(UINT8 inst_id)
{
    tBTM_BLE_MULTI_ADV_INST *p_inst = NULL;

    if (inst_id < BTM_BleMaxMultiAdvInstanceCount()) {
        p_inst = &btm_multi_adv_cb.p_adv_inst[inst_id - 1];
        if (NULL != p_inst) {
            return p_inst->p_ref;
        }
    }

    return NULL;
}
#endif


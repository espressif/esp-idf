/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
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

/******************************************************************************
 *
 *  This file contains functions for BLE controller based privacy.
 *
 ******************************************************************************/
#include <string.h>
#include "bt_target.h"

#if (BLE_INCLUDED == TRUE && BLE_PRIVACY_SPT == TRUE)
#include "bt_types.h"
#include "hcimsgs.h"
#include "btu.h"
//#include "vendor_hcidefs.h"
#include "btm_int.h"
#include "controller.h"
#include "hcidefs.h"

#define HCI_VENDOR_BLE_RPA_VSC          (0x0155 | HCI_GRP_VENDOR_SPECIFIC)

/* RPA offload VSC specifics */
#define BTM_BLE_META_IRK_ENABLE         0x01
#define BTM_BLE_META_ADD_IRK_ENTRY      0x02
#define BTM_BLE_META_REMOVE_IRK_ENTRY   0x03
#define BTM_BLE_META_CLEAR_IRK_LIST     0x04
#define BTM_BLE_META_READ_IRK_ENTRY     0x05
#define BTM_BLE_META_CS_RESOLVE_ADDR    0x00000001
#define BTM_BLE_IRK_ENABLE_LEN          2

#define BTM_BLE_META_ADD_IRK_LEN        24
#define BTM_BLE_META_REMOVE_IRK_LEN     8
#define BTM_BLE_META_CLEAR_IRK_LEN      1
#define BTM_BLE_META_READ_IRK_LEN       2
#define BTM_BLE_META_ADD_WL_ATTR_LEN    9

/*******************************************************************************
**         Functions implemented controller based privacy using Resolving List
*******************************************************************************/
/*******************************************************************************
**
** Function         btm_ble_enq_resolving_list_pending
**
** Description      add target address into resolving pending operation queue
**
** Parameters       target_bda: target device address
**                  add_entry: TRUE for add entry, FALSE for remove entry
**
** Returns          void
**
*******************************************************************************/
void btm_ble_enq_resolving_list_pending(BD_ADDR pseudo_bda, UINT8 op_code)
{
    tBTM_BLE_RESOLVE_Q *p_q = &btm_cb.ble_ctr_cb.resolving_list_pend_q;

    memcpy(p_q->resolve_q_random_pseudo[p_q->q_next], pseudo_bda, BD_ADDR_LEN);
    p_q->resolve_q_action[p_q->q_next] = op_code;
    p_q->q_next ++;
    p_q->q_next %= controller_get_interface()->get_ble_resolving_list_max_size();
}

/*******************************************************************************
**
** Function         btm_ble_brcm_find_resolving_pending_entry
**
** Description      check to see if the action is in pending list
**
** Parameters       TRUE: action pending;
**                  FALSE: new action
**
** Returns          void
**
*******************************************************************************/
BOOLEAN btm_ble_brcm_find_resolving_pending_entry(BD_ADDR pseudo_addr, UINT8 action)
{
    tBTM_BLE_RESOLVE_Q *p_q = &btm_cb.ble_ctr_cb.resolving_list_pend_q;

    for (UINT8 i = p_q->q_pending; i != p_q->q_next;) {
        if (memcmp(p_q->resolve_q_random_pseudo[i], pseudo_addr, BD_ADDR_LEN) == 0 &&
                action == p_q->resolve_q_action[i]) {
            return TRUE;
        }

        i ++;
        i %= controller_get_interface()->get_ble_resolving_list_max_size();
    }
    return FALSE;
}

/*******************************************************************************
**
** Function         btm_ble_deq_resolving_pending
**
** Description      dequeue target address from resolving pending operation queue
**
** Parameters       pseudo_addr: pseudo_addr device address
**
** Returns          void
**
*******************************************************************************/
BOOLEAN btm_ble_deq_resolving_pending(BD_ADDR pseudo_addr)
{
    tBTM_BLE_RESOLVE_Q *p_q = &btm_cb.ble_ctr_cb.resolving_list_pend_q;

    if (p_q->q_next != p_q->q_pending) {
        memcpy(pseudo_addr, p_q->resolve_q_random_pseudo[p_q->q_pending], BD_ADDR_LEN);
        memset(p_q->resolve_q_random_pseudo[p_q->q_pending], 0, BD_ADDR_LEN);
        p_q->q_pending ++;
        p_q->q_pending %= controller_get_interface()->get_ble_resolving_list_max_size();
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
**
** Function         btm_ble_clear_irk_index
**
** Description      clear IRK list index mask for availability
**
** Returns          none
**
*******************************************************************************/
void btm_ble_clear_irk_index(UINT8 index)
{
    UINT8 byte;
    UINT8 bit;

    if (index < controller_get_interface()->get_ble_resolving_list_max_size()) {
        byte = index / 8;
        bit = index % 8;
        btm_cb.ble_ctr_cb.irk_list_mask[byte] &= (~(1 << bit));
    }
}

/*******************************************************************************
**
** Function         btm_ble_find_irk_index
**
** Description      find the first available IRK list index
**
** Returns          index from 0 ~ max (127 default)
**
*******************************************************************************/
UINT8 btm_ble_find_irk_index(void)
{
    UINT8 i = 0;
    UINT8 byte;
    UINT8 bit;

    while (i < controller_get_interface()->get_ble_resolving_list_max_size()) {
        byte = i / 8;
        bit = i % 8;

        if ((btm_cb.ble_ctr_cb.irk_list_mask[byte] & (1 << bit)) == 0) {
            btm_cb.ble_ctr_cb.irk_list_mask[byte] |= (1 << bit);
            return i;
        }
        i++;
    }

    BTM_TRACE_ERROR ("%s failed, list full", __func__);
    return i;
}

/*******************************************************************************
**
** Function         btm_ble_update_resolving_list
**
** Description      update resolving list entry in host maintained record
**
** Returns          void
**
*******************************************************************************/
void btm_ble_update_resolving_list(BD_ADDR pseudo_bda, BOOLEAN add)
{
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev(pseudo_bda);
    if (p_dev_rec == NULL) {
        return;
    }

    if (add) {
        p_dev_rec->ble.in_controller_list |= BTM_RESOLVING_LIST_BIT;
        if (!controller_get_interface()->supports_ble_privacy()) {
            p_dev_rec->ble.resolving_list_index = btm_ble_find_irk_index();
        }
    } else {
        p_dev_rec->ble.in_controller_list &= ~BTM_RESOLVING_LIST_BIT;
        if (!controller_get_interface()->supports_ble_privacy()) {
            /* clear IRK list index mask */
            btm_ble_clear_irk_index(p_dev_rec->ble.resolving_list_index);
            p_dev_rec->ble.resolving_list_index = 0;
        }
    }
}

/*******************************************************************************
**
** Function         btm_ble_clear_resolving_list_complete
**
** Description      This function is called when command complete for
**                  clear resolving list
**
** Returns          void
**
*******************************************************************************/
void btm_ble_clear_resolving_list_complete(UINT8 *p, UINT16 evt_len)
{
    UINT8 status = 0;
    STREAM_TO_UINT8(status, p);

    BTM_TRACE_DEBUG("%s status=%d", __func__, status);

    if (status == HCI_SUCCESS) {
        if (evt_len >= 3) {
            /* VSC complete has one extra byte for op code and list size, skip it here */
            p ++;

            /* updated the available list size, and current list size */
            uint8_t irk_list_sz_max = 0;
            STREAM_TO_UINT8(irk_list_sz_max, p);

            if (controller_get_interface()->get_ble_resolving_list_max_size() == 0) {
                btm_ble_resolving_list_init(irk_list_sz_max);
            }

            uint8_t irk_mask_size = (irk_list_sz_max % 8) ?
                                    (irk_list_sz_max / 8 + 1) : (irk_list_sz_max / 8);
            memset(btm_cb.ble_ctr_cb.irk_list_mask, 0, irk_mask_size);
        }

        btm_cb.ble_ctr_cb.resolving_list_avail_size =
            controller_get_interface()->get_ble_resolving_list_max_size();

        BTM_TRACE_DEBUG("%s resolving_list_avail_size=%d",
                        __func__, btm_cb.ble_ctr_cb.resolving_list_avail_size);

        for (UINT8 i = 0; i < BTM_SEC_MAX_DEVICE_RECORDS; ++i) {
            btm_cb.sec_dev_rec[i].ble.in_controller_list &= ~BTM_RESOLVING_LIST_BIT;
        }
    }
}

/*******************************************************************************
**
** Function         btm_ble_add_resolving_list_entry_complete
**
** Description      This function is called when command complete for
**                  add resolving list entry
**
** Returns          void
**
*******************************************************************************/
void btm_ble_add_resolving_list_entry_complete(UINT8 *p, UINT16 evt_len)
{
    UINT8 status;
    STREAM_TO_UINT8(status, p);

    BTM_TRACE_DEBUG("%s status = %d", __func__, status);

    BD_ADDR pseudo_bda;
    if (!btm_ble_deq_resolving_pending(pseudo_bda)) {
        BTM_TRACE_DEBUG("no pending resolving list operation");
        return;
    }

    if (status == HCI_SUCCESS) {
        /* privacy 1.2 command complete does not have these extra byte */
        if (evt_len > 2) {
            /* VSC complete has one extra byte for op code, skip it here */
            p ++;
            STREAM_TO_UINT8(btm_cb.ble_ctr_cb.resolving_list_avail_size, p);
        } else {
            btm_cb.ble_ctr_cb.resolving_list_avail_size --;
        }
    } else if (status == HCI_ERR_MEMORY_FULL) { /* BT_ERROR_CODE_MEMORY_CAPACITY_EXCEEDED  */
        btm_cb.ble_ctr_cb.resolving_list_avail_size = 0;
        BTM_TRACE_DEBUG("%s Resolving list Full ", __func__);
    }
}

/*******************************************************************************
**
** Function         btm_ble_remove_resolving_list_entry_complete
**
** Description      This function is called when command complete for
**                  remove resolving list entry
**
** Returns          void
**
*******************************************************************************/
void btm_ble_remove_resolving_list_entry_complete(UINT8 *p, UINT16 evt_len)
{
    BD_ADDR pseudo_bda;
    UINT8 status;

    STREAM_TO_UINT8(status, p);

    BTM_TRACE_DEBUG("%s status = %d", __func__, status);

    if (!btm_ble_deq_resolving_pending(pseudo_bda)) {
        BTM_TRACE_ERROR("%s no pending resolving list operation", __func__);
        return;
    }

    if (status == HCI_SUCCESS) {
        /* proprietary: spec does not have these extra bytes */
        if (evt_len > 2) {
            p ++; /* skip opcode */
            STREAM_TO_UINT8(btm_cb.ble_ctr_cb.resolving_list_avail_size, p);
        } else {
            btm_cb.ble_ctr_cb.resolving_list_avail_size++;
        }
    }
}

/*******************************************************************************
**
** Function         btm_ble_read_resolving_list_entry_complete
**
** Description      This function is called when command complete for
**                  remove resolving list entry
**
** Returns          void
**
*******************************************************************************/
void btm_ble_read_resolving_list_entry_complete(UINT8 *p, UINT16 evt_len)
{
    UINT8           status, rra_type = BTM_BLE_ADDR_PSEUDO;
    BD_ADDR         rra, pseudo_bda;

    STREAM_TO_UINT8  (status, p);

    BTM_TRACE_DEBUG("%s status = %d", __func__, status);

    if (!btm_ble_deq_resolving_pending(pseudo_bda)) {
        BTM_TRACE_ERROR("no pending resolving list operation");
        return;
    }

    if (status == HCI_SUCCESS) {
        /* proprietary spec has extra bytes */
        if (evt_len > 8) {
            p += (2 + 16 + 1 + 6); /* skip subcode, index, IRK value, address type, identity addr type */
            STREAM_TO_BDADDR(rra, p);

            BTM_TRACE_ERROR("%s peer_addr: %02x:%02x:%02x:%02x:%02x:%02x",
                            __func__, rra[0], rra[1], rra[2], rra[3], rra[4], rra[5]);
        } else {
            STREAM_TO_BDADDR(rra, p);
        }
        btm_ble_refresh_peer_resolvable_private_addr(pseudo_bda, rra, rra_type);
    }
}
/*******************************************************************************
                VSC that implement controller based privacy
********************************************************************************/
/*******************************************************************************
**
** Function         btm_ble_resolving_list_vsc_op_cmpl
**
** Description      IRK operation VSC complete handler
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
void btm_ble_resolving_list_vsc_op_cmpl (tBTM_VSC_CMPL *p_params)
{
    UINT8  *p = p_params->p_param_buf, op_subcode;
    UINT16  evt_len = p_params->param_len;

    op_subcode   = *(p + 1);

    BTM_TRACE_DEBUG("%s op_subcode = %d", __func__, op_subcode);

    if (op_subcode == BTM_BLE_META_CLEAR_IRK_LIST) {
        btm_ble_clear_resolving_list_complete(p, evt_len);
    } else if (op_subcode == BTM_BLE_META_ADD_IRK_ENTRY) {
        btm_ble_add_resolving_list_entry_complete(p, evt_len);
    } else if (op_subcode == BTM_BLE_META_REMOVE_IRK_ENTRY) {
        btm_ble_remove_resolving_list_entry_complete(p, evt_len);
    } else if (op_subcode == BTM_BLE_META_READ_IRK_ENTRY) {
        btm_ble_read_resolving_list_entry_complete(p, evt_len);
    } else if (op_subcode == BTM_BLE_META_IRK_ENABLE) {
        /* RPA offloading enable/disabled */
    }
}

/*******************************************************************************
**
** Function         btm_ble_remove_resolving_list_entry
**
** Description      This function to remove an IRK entry from the list
**
** Parameters       ble_addr_type: address type
**                  ble_addr: LE adddress
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS btm_ble_remove_resolving_list_entry(tBTM_SEC_DEV_REC *p_dev_rec)
{
    /* if controller does not support RPA offloading or privacy 1.2, skip */
    if (controller_get_interface()->get_ble_resolving_list_max_size() == 0) {
        return BTM_WRONG_MODE;
    }

    tBTM_STATUS st = BTM_NO_RESOURCES;
    if (controller_get_interface()->supports_ble_privacy()) {
        if (btsnd_hcic_ble_rm_device_resolving_list(p_dev_rec->ble.static_addr_type,
                p_dev_rec->ble.static_addr)) {
            st =  BTM_CMD_STARTED;
        }
    } else {
        UINT8 param[20] = {0};
        UINT8 *p = param;

        UINT8_TO_STREAM(p, BTM_BLE_META_REMOVE_IRK_ENTRY);
        UINT8_TO_STREAM(p, p_dev_rec->ble.static_addr_type);
        BDADDR_TO_STREAM(p, p_dev_rec->ble.static_addr);

        st = BTM_VendorSpecificCommand(HCI_VENDOR_BLE_RPA_VSC,
                                       BTM_BLE_META_REMOVE_IRK_LEN,
                                       param,
                                       btm_ble_resolving_list_vsc_op_cmpl);
    }

    if (st == BTM_CMD_STARTED) {
        btm_ble_enq_resolving_list_pending( p_dev_rec->bd_addr, BTM_BLE_META_REMOVE_IRK_ENTRY);
    }

    return st;
}

/*******************************************************************************
**
** Function         btm_ble_clear_resolving_list
**
** Description      This function clears the resolving  list
**
** Parameters       None.
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS btm_ble_clear_resolving_list(void)
{
    tBTM_STATUS st = BTM_NO_RESOURCES;

    if (controller_get_interface()->supports_ble_privacy()) {
        if (btsnd_hcic_ble_clear_resolving_list()) {
            st =  BTM_SUCCESS;
        }
    } else {
        UINT8 param[20] = {0};
        UINT8 *p = param;

        UINT8_TO_STREAM(p, BTM_BLE_META_CLEAR_IRK_LIST);
        st = BTM_VendorSpecificCommand (HCI_VENDOR_BLE_RPA_VSC,
                                        BTM_BLE_META_CLEAR_IRK_LEN,
                                        param,
                                        btm_ble_resolving_list_vsc_op_cmpl);
    }

    return st;
}

/*******************************************************************************
**
** Function         btm_ble_read_resolving_list_entry
**
** Description      This function read an IRK entry by index
**
** Parameters       entry index.
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS btm_ble_read_resolving_list_entry(tBTM_SEC_DEV_REC *p_dev_rec)
{
    tBTM_STATUS st = BTM_NO_RESOURCES;

    if (!(p_dev_rec->ble.in_controller_list & BTM_RESOLVING_LIST_BIT)) {
        return BTM_WRONG_MODE;
    }

    if (controller_get_interface()->supports_ble_privacy()) {
        if (btsnd_hcic_ble_read_resolvable_addr_peer(p_dev_rec->ble.static_addr_type,
                p_dev_rec->ble.static_addr)) {
            st =  BTM_CMD_STARTED;
        }
    } else {
        UINT8 param[20] = {0};
        UINT8 *p = param;

        UINT8_TO_STREAM(p, BTM_BLE_META_READ_IRK_ENTRY);
        UINT8_TO_STREAM(p, p_dev_rec->ble.resolving_list_index);

        st = BTM_VendorSpecificCommand (HCI_VENDOR_BLE_RPA_VSC,
                                        BTM_BLE_META_READ_IRK_LEN,
                                        param,
                                        btm_ble_resolving_list_vsc_op_cmpl);
    }

    if (st == BTM_CMD_STARTED)
        btm_ble_enq_resolving_list_pending(p_dev_rec->bd_addr,
                                           BTM_BLE_META_READ_IRK_ENTRY);

    return st;
}


/*******************************************************************************
**
** Function         btm_ble_suspend_resolving_list_activity
**
** Description      This function suspends all resolving list activity, including
**                  scan, initiating, and advertising, if resolving list is being
**                  enabled.
**
** Parameters
**
** Returns          TRUE if suspended; FALSE otherwise
**
*******************************************************************************/
BOOLEAN btm_ble_suspend_resolving_list_activity(void)
{
    tBTM_BLE_CB *p_ble_cb = &btm_cb.ble_ctr_cb;

    /* if resolving list is not enabled, do not need to terminate any activity */
    /* if asking for stop all activity */
    /* if already suspended */
    if (p_ble_cb->suspended_rl_state != BTM_BLE_RL_IDLE) {
        return TRUE;
    }

    /* direct connection active, wait until it completed */
    if (btm_ble_get_conn_st() == BLE_DIR_CONN) {
        BTM_TRACE_ERROR("resolving list can not be edited, EnQ now");
        return FALSE;
    }

    p_ble_cb->suspended_rl_state = BTM_BLE_RL_IDLE;

    if (p_ble_cb->inq_var.adv_mode == BTM_BLE_ADV_ENABLE) {
        btm_ble_stop_adv();
        p_ble_cb->suspended_rl_state |= BTM_BLE_RL_ADV;
    }

    if (BTM_BLE_IS_SCAN_ACTIVE(p_ble_cb->scan_activity)) {
        btm_ble_stop_scan();
        p_ble_cb->suspended_rl_state |= BTM_BLE_RL_SCAN;
    }

    if (btm_ble_suspend_bg_conn()) {
        p_ble_cb->suspended_rl_state |= BTM_BLE_RL_INIT;
    }

    return TRUE;
}

/*******************************************************************************
**
** Function         btm_ble_resume_resolving_list_activity
**
** Description      This function resumes the resolving list activity, including
**                  scanning, initiating, and advertising, if any of these
**                  activities has been suspended earlier.
**
** Returns          none
**
*******************************************************************************/
void btm_ble_resume_resolving_list_activity(void)
{
    tBTM_BLE_CB *p_ble_cb = &btm_cb.ble_ctr_cb;

    if (p_ble_cb->suspended_rl_state & BTM_BLE_RL_ADV) {
        btm_ble_start_adv();
    }

    if (p_ble_cb->suspended_rl_state & BTM_BLE_RL_SCAN) {
        btm_ble_start_scan();
    }

    if  (p_ble_cb->suspended_rl_state & BTM_BLE_RL_INIT) {
        btm_ble_resume_bg_conn();
    }

    p_ble_cb->suspended_rl_state = BTM_BLE_RL_IDLE;
}

/*******************************************************************************
**
** Function         btm_ble_vendor_enable_irk_feature
**
** Description      This function is called to enable or disable the RRA
**                  offloading feature.
**
** Parameters       enable: enable or disable the RRA offloading feature
**
** Returns          BTM_SUCCESS if successful
**
*******************************************************************************/
tBTM_STATUS btm_ble_vendor_enable_irk_feature(BOOLEAN enable)
{
    UINT8           param[20], *p;
    tBTM_STATUS     st = BTM_MODE_UNSUPPORTED;

    p = param;
    memset(param, 0, 20);

    /* select feature based on control block settings */
    UINT8_TO_STREAM(p, BTM_BLE_META_IRK_ENABLE);
    UINT8_TO_STREAM(p, enable ? 0x01 : 0x00);

    st = BTM_VendorSpecificCommand (HCI_VENDOR_BLE_RPA_VSC, BTM_BLE_IRK_ENABLE_LEN,
                                    param, btm_ble_resolving_list_vsc_op_cmpl);

    return st;
}

/*******************************************************************************
**
** Function         btm_ble_exe_disable_resolving_list
**
** Description      execute resolving list disable
**
** Returns          none
**
*******************************************************************************/
BOOLEAN btm_ble_exe_disable_resolving_list(void)
{
    if (!btm_ble_suspend_resolving_list_activity()) {
        return FALSE;
    }

    if (!controller_get_interface()->supports_ble_privacy()) {
        btm_ble_vendor_enable_irk_feature(FALSE);
    } else {
        btsnd_hcic_ble_set_addr_resolution_enable(FALSE);
    }

    return TRUE;
}

/*******************************************************************************
**
** Function         btm_ble_exe_enable_resolving_list
**
** Description      enable LE resolve address list
**
** Returns          none
**
*******************************************************************************/
void btm_ble_exe_enable_resolving_list(void)
{
    if (!btm_ble_suspend_resolving_list_activity()) {
        return;
    }

    if (!controller_get_interface()->supports_ble_privacy()) {
        btm_ble_vendor_enable_irk_feature(TRUE);
    } else {
        btsnd_hcic_ble_set_addr_resolution_enable(TRUE);
    }
}

/*******************************************************************************
**
** Function         btm_ble_disable_resolving_list
**
** Description      Disable LE Address resolution
**
** Returns          none
**
*******************************************************************************/
BOOLEAN btm_ble_disable_resolving_list(UINT8 rl_mask, BOOLEAN to_resume )
{
    UINT8 rl_state = btm_cb.ble_ctr_cb.rl_state;

    /* if controller does not support RPA offloading or privacy 1.2, skip */
    if (controller_get_interface()->get_ble_resolving_list_max_size() == 0) {
        return FALSE;
    }

    btm_cb.ble_ctr_cb.rl_state &= ~rl_mask;

    if (rl_state != BTM_BLE_RL_IDLE && btm_cb.ble_ctr_cb.rl_state == BTM_BLE_RL_IDLE) {
        if (btm_ble_exe_disable_resolving_list()) {
            if (to_resume) {
                btm_ble_resume_resolving_list_activity();
            }

            return TRUE;
        } else {
            return FALSE;
        }
    }

    return TRUE;
}

/*******************************************************************************
**
** Function         btm_ble_resolving_list_load_dev
**
** Description      This function add a device which is using RPA into white list
**
** Parameters       pointer to device security record
**
** Returns          TRUE if device added, otherwise falase.
**
*******************************************************************************/
BOOLEAN btm_ble_resolving_list_load_dev(tBTM_SEC_DEV_REC *p_dev_rec)
{
    BOOLEAN rt = FALSE;
#if (SMP_INCLUDED == TRUE)
    UINT8 rl_mask = btm_cb.ble_ctr_cb.rl_state;

    BTM_TRACE_DEBUG("%s btm_cb.ble_ctr_cb.privacy_mode = %d\n", __func__,
                    btm_cb.ble_ctr_cb.privacy_mode);

    /* if controller does not support RPA offloading or privacy 1.2, skip */
    if (controller_get_interface()->get_ble_resolving_list_max_size() == 0) {
        return FALSE;
    }

    BTM_TRACE_DEBUG("%s btm_cb.ble_ctr_cb.privacy_mode = %d\n",
                    __func__, btm_cb.ble_ctr_cb.privacy_mode);

    /* only add RPA enabled device into resolving list */
    if (p_dev_rec != NULL && /* RPA is being used and PID is known */
            (p_dev_rec->sec_flags & BTM_SEC_IN_USE) != 0 &&
            ((p_dev_rec->ble.key_type & BTM_LE_KEY_PID) != 0 ||
             (p_dev_rec->ble.key_type & BTM_LE_KEY_LID) != 0)) {
        if (!(p_dev_rec->ble.in_controller_list & BTM_RESOLVING_LIST_BIT) &&
                btm_ble_brcm_find_resolving_pending_entry(p_dev_rec->bd_addr,
                        BTM_BLE_META_ADD_IRK_ENTRY) == FALSE) {
            if (btm_cb.ble_ctr_cb.resolving_list_avail_size > 0) {
                if (rl_mask) {
                    if (!btm_ble_disable_resolving_list (rl_mask, FALSE)) {
                        return FALSE;
                    }
                }

                btm_ble_update_resolving_list(p_dev_rec->bd_addr, TRUE);
                if (controller_get_interface()->supports_ble_privacy()) {
                    BD_ADDR dummy_bda = {0};
                    UINT8 *peer_irk = p_dev_rec->ble.keys.irk;
                    UINT8 *local_irk = btm_cb.devcb.id_keys.irk;

                    if (memcmp(p_dev_rec->ble.static_addr, dummy_bda, BD_ADDR_LEN) == 0) {
                        memcpy(p_dev_rec->ble.static_addr, p_dev_rec->bd_addr, BD_ADDR_LEN);
                        p_dev_rec->ble.static_addr_type = p_dev_rec->ble.ble_addr_type;
                    }

                    BTM_TRACE_DEBUG("%s:adding device to controller resolving list\n", __func__);
                    // use identical IRK for now
                    rt = btsnd_hcic_ble_add_device_resolving_list(p_dev_rec->ble.static_addr_type,
                            p_dev_rec->ble.static_addr, peer_irk, local_irk);
                } else {
                    UINT8 param[40] = {0};
                    UINT8 *p = param;

                    UINT8_TO_STREAM(p, BTM_BLE_META_ADD_IRK_ENTRY);
                    ARRAY_TO_STREAM(p, p_dev_rec->ble.keys.irk, BT_OCTET16_LEN);
                    UINT8_TO_STREAM(p, p_dev_rec->ble.static_addr_type);
                    BDADDR_TO_STREAM(p, p_dev_rec->ble.static_addr);

                    if (BTM_VendorSpecificCommand (HCI_VENDOR_BLE_RPA_VSC,
                                                   BTM_BLE_META_ADD_IRK_LEN,
                                                   param,
                                                   btm_ble_resolving_list_vsc_op_cmpl)
                            == BTM_CMD_STARTED) {
                        rt = TRUE;
                    }
                }

                if (rt)
                    btm_ble_enq_resolving_list_pending(p_dev_rec->bd_addr,
                                                       BTM_BLE_META_ADD_IRK_ENTRY);

                /* if resolving list has been turned on, re-enable it */
                if (rl_mask) {
                    btm_ble_enable_resolving_list(rl_mask);
                } else {
                    btm_ble_enable_resolving_list(BTM_BLE_RL_INIT);
                }
            }
        } else {
            BTM_TRACE_ERROR("Device already in Resolving list\n");
            rt = TRUE;
        }
    } else {
        BTM_TRACE_DEBUG("Device not a RPA enabled device\n");
    }
#endif  ///SMP_INCLUDED == TRUE
    return rt;
}

/*******************************************************************************
**
** Function         btm_ble_resolving_list_remove_dev
**
** Description      This function removes the device from resolving list
**
** Parameters
**
** Returns          status
**
*******************************************************************************/
void btm_ble_resolving_list_remove_dev(tBTM_SEC_DEV_REC *p_dev_rec)
{
    UINT8 rl_mask = btm_cb.ble_ctr_cb.rl_state;

    BTM_TRACE_EVENT ("%s\n", __func__);
    if (rl_mask) {
        if (!btm_ble_disable_resolving_list (rl_mask, FALSE)) {
            return;
        }
    }

    if ((p_dev_rec->ble.in_controller_list & BTM_RESOLVING_LIST_BIT) &&
            btm_ble_brcm_find_resolving_pending_entry(p_dev_rec->bd_addr,
                    BTM_BLE_META_REMOVE_IRK_ENTRY) == FALSE) {
        btm_ble_update_resolving_list( p_dev_rec->bd_addr, FALSE);
        btm_ble_remove_resolving_list_entry(p_dev_rec);
    } else {
        BTM_TRACE_DEBUG("Device not in resolving list\n");
    }

    /* if resolving list has been turned on, re-enable it */
    if (rl_mask) {
        btm_ble_enable_resolving_list(rl_mask);
    }
}

/*******************************************************************************
**
** Function         btm_ble_enable_resolving_list
**
** Description      enable LE resolve address list
**
** Returns          none
**
*******************************************************************************/
void btm_ble_enable_resolving_list(UINT8 rl_mask)
{
    UINT8 rl_state = btm_cb.ble_ctr_cb.rl_state;

    btm_cb.ble_ctr_cb.rl_state |= rl_mask;
    if (rl_state == BTM_BLE_RL_IDLE &&
            btm_cb.ble_ctr_cb.rl_state != BTM_BLE_RL_IDLE &&
            controller_get_interface()->get_ble_resolving_list_max_size() != 0) {
        btm_ble_exe_enable_resolving_list();
        btm_ble_resume_resolving_list_activity();
    }
}

/*******************************************************************************
**
** Function         btm_ble_resolving_list_empty
**
** Description      check to see if resoving list is empty or not
**
** Returns          TRUE: empty; FALSE non-empty
**
*******************************************************************************/
BOOLEAN btm_ble_resolving_list_empty(void)
{
    return (controller_get_interface()->get_ble_resolving_list_max_size() ==
            btm_cb.ble_ctr_cb.resolving_list_avail_size);
}

/*******************************************************************************
**
** Function         btm_ble_enable_resolving_list_for_platform
**
** Description      enable/disable resolving list feature depending on if any
**                  resolving list is empty and whitelist is involoved in the
**                  operation.
**
** Returns          none
**
*******************************************************************************/
void btm_ble_enable_resolving_list_for_platform (UINT8 rl_mask)
{
    /* if controller does not support, skip */
    if (controller_get_interface()->get_ble_resolving_list_max_size() == 0) {
        return;
    }

    if (btm_cb.ble_ctr_cb.wl_state == BTM_BLE_WL_IDLE) {
        if (controller_get_interface()->get_ble_resolving_list_max_size() >
                btm_cb.ble_ctr_cb.resolving_list_avail_size) {
            btm_ble_enable_resolving_list(rl_mask);
        } else {
            btm_ble_disable_resolving_list(rl_mask, TRUE);
        }
        return;
    }

    tBTM_SEC_DEV_REC *p_dev = &btm_cb.sec_dev_rec[0];
    for (UINT8 i = 0; i < BTM_SEC_MAX_DEVICE_RECORDS; i ++, p_dev ++) {
        if ((p_dev->ble.in_controller_list & BTM_RESOLVING_LIST_BIT) &&
                (p_dev->ble.in_controller_list & BTM_WHITE_LIST_BIT)) {
            btm_ble_enable_resolving_list(rl_mask);
            return;
        }
    }
    btm_ble_disable_resolving_list(rl_mask, TRUE);
}

/*******************************************************************************
**
** Function         btm_ble_resolving_list_init
**
** Description      Initialize resolving list in host stack
**
** Parameters       Max resolving list size
**
** Returns          void
**
*******************************************************************************/
void btm_ble_resolving_list_init(UINT8 max_irk_list_sz)
{
    tBTM_BLE_RESOLVE_Q *p_q = &btm_cb.ble_ctr_cb.resolving_list_pend_q;
    UINT8 irk_mask_size =  (max_irk_list_sz % 8) ?
                           (max_irk_list_sz / 8 + 1) : (max_irk_list_sz / 8);

    if (max_irk_list_sz > 0) {
        p_q->resolve_q_random_pseudo = (BD_ADDR *)GKI_getbuf(sizeof(BD_ADDR) * max_irk_list_sz);
        p_q->resolve_q_action = (UINT8 *)GKI_getbuf(max_irk_list_sz);

        /* RPA offloading feature */
        if (btm_cb.ble_ctr_cb.irk_list_mask == NULL) {
            btm_cb.ble_ctr_cb.irk_list_mask = (UINT8 *)GKI_getbuf(irk_mask_size);
        }

        BTM_TRACE_DEBUG ("%s max_irk_list_sz = %d", __func__, max_irk_list_sz);
    }

    controller_get_interface()->set_ble_resolving_list_max_size(max_irk_list_sz);
    btm_ble_clear_resolving_list();
    btm_cb.ble_ctr_cb.resolving_list_avail_size = max_irk_list_sz;
}

/*******************************************************************************
**
** Function         btm_ble_resolving_list_cleanup
**
** Description      Cleanup resolving list dynamic memory
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
void btm_ble_resolving_list_cleanup(void)
{
    tBTM_BLE_RESOLVE_Q *p_q = &btm_cb.ble_ctr_cb.resolving_list_pend_q;

    if (p_q->resolve_q_random_pseudo) {
        GKI_freebuf(p_q->resolve_q_random_pseudo);
    }

    if (p_q->resolve_q_action) {
        GKI_freebuf(p_q->resolve_q_action);
    }

    controller_get_interface()->set_ble_resolving_list_max_size(0);
    if (btm_cb.ble_ctr_cb.irk_list_mask) {
        GKI_freebuf(btm_cb.ble_ctr_cb.irk_list_mask);
    }

    btm_cb.ble_ctr_cb.irk_list_mask = NULL;
}
#endif

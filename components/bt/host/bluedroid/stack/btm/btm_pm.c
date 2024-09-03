/******************************************************************************
 *
 *  Copyright (C) 2000-2012 Broadcom Corporation
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

/*****************************************************************************
 *
 *  This file contains functions that manages ACL link modes.
 *  This includes operations such as active, hold,
 *  park and sniff modes.
 *
 *  This module contains both internal and external (API)
 *  functions. External (API) functions are distinguishable
 *  by their names beginning with uppercase BTM.
 *
 *****************************************************************************/

//#define LOG_TAG "bt_btm_pm"

#include <stdlib.h>
#include <string.h>
//#include <stdio.h>
#include <stddef.h>

#include "stack/bt_types.h"
#include "stack/hcimsgs.h"
#include "stack/btu.h"
#include "stack/btm_api.h"
#include "btm_int.h"
#include "l2c_int.h"
#include "stack/hcidefs.h"
//#include "bt_utils.h"
//#include "osi/include/log.h"
#include "osi/allocator.h"
/*****************************************************************************/
/*      to handle different modes                                            */
/*****************************************************************************/
#define BTM_PM_STORED_MASK      0x80 /* set this mask if the command is stored */
#define BTM_PM_NUM_SET_MODES    3 /* only hold, sniff & park */

/* Usage:  (ptr_features[ offset ] & mask )?TRUE:FALSE */
/* offset to supported feature */
const UINT8 btm_pm_mode_off[BTM_PM_NUM_SET_MODES] = {0,    0,    1};
/* mask to supported feature */
const UINT8 btm_pm_mode_msk[BTM_PM_NUM_SET_MODES] = {0x40, 0x80, 0x01};

#define BTM_PM_GET_MD1      1
#define BTM_PM_GET_MD2      2
#define BTM_PM_GET_COMP     3

const UINT8 btm_pm_md_comp_matrix[BTM_PM_NUM_SET_MODES * BTM_PM_NUM_SET_MODES] = {
    BTM_PM_GET_COMP,
    BTM_PM_GET_MD2,
    BTM_PM_GET_MD2,

    BTM_PM_GET_MD1,
    BTM_PM_GET_COMP,
    BTM_PM_GET_MD1,

    BTM_PM_GET_MD1,
    BTM_PM_GET_MD2,
    BTM_PM_GET_COMP
};

/* function prototype */
static tBTM_STATUS btm_pm_snd_md_req( UINT8 pm_id, UINT16 link_hdl, tBTM_PM_PWR_MD *p_mode );
#if (!CONFIG_BT_STACK_NO_LOG)
static const char *mode_to_string(tBTM_PM_MODE mode);
#endif

/*
#ifdef BTM_PM_DEBUG
#undef BTM_PM_DEBUG
#define BTM_PM_DEBUG    TRUE
#endif
*/

#if BTM_PM_DEBUG == TRUE
const char *btm_pm_state_str[] = {
    "pm_active_state",
    "pm_hold_state",
    "pm_sniff_state",
    "pm_park_state",
    "pm_pend_state"
};

const char *btm_pm_event_str[] = {
    "pm_set_mode_event",
    "pm_hci_sts_event",
    "pm_mod_chg_event",
    "pm_update_event"
};

const char *btm_pm_action_str[] = {
    "pm_set_mode_action",
    "pm_update_db_action",
    "pm_mod_chg_action",
    "pm_hci_sts_action",
    "pm_update_action"
};
#endif  // BTM_PM_DEBUG

/*****************************************************************************/
/*                     P U B L I C  F U N C T I O N S                        */
/*****************************************************************************/
/*******************************************************************************
**
** Function         BTM_PmRegister
**
** Description      register or deregister with power manager
**
** Returns          BTM_SUCCESS if successful,
**                  BTM_NO_RESOURCES if no room to hold registration
**                  BTM_ILLEGAL_VALUE
**
*******************************************************************************/
tBTM_STATUS BTM_PmRegister (UINT8 mask, UINT8 *p_pm_id, tBTM_PM_STATUS_CBACK *p_cb)
{
    int xx;

    /* de-register */
    if (mask & BTM_PM_DEREG) {
        if (*p_pm_id >= BTM_MAX_PM_RECORDS) {
            return BTM_ILLEGAL_VALUE;
        }
        btm_cb.pm_reg_db[*p_pm_id].mask = BTM_PM_REC_NOT_USED;
        return BTM_SUCCESS;
    }

    for (xx = 0; xx < BTM_MAX_PM_RECORDS; xx++) {
        /* find an unused entry */
        if (btm_cb.pm_reg_db[xx].mask == BTM_PM_REC_NOT_USED) {
            /* if register for notification, should provide callback routine */
            if (mask & BTM_PM_REG_NOTIF) {
                if (p_cb == NULL) {
                    return BTM_ILLEGAL_VALUE;
                }
                btm_cb.pm_reg_db[xx].cback = p_cb;
            }
            btm_cb.pm_reg_db[xx].mask = mask;
            *p_pm_id = xx;
            return BTM_SUCCESS;
        }
    }

    return BTM_NO_RESOURCES;
}

/*******************************************************************************
**
** Function         BTM_SetPowerMode
**
** Description      store the mode in control block or
**                  alter ACL connection behavior.
**
** Returns          BTM_SUCCESS if successful,
**                  BTM_UNKNOWN_ADDR if bd addr is not active or bad
**
*******************************************************************************/
tBTM_STATUS BTM_SetPowerMode (UINT8 pm_id, BD_ADDR remote_bda, tBTM_PM_PWR_MD *p_mode)
{
    UINT8               *p_features;
    int                 ind;
    tBTM_PM_MCB *p_cb = NULL;   /* per ACL link */
    tBTM_PM_MODE        mode;
    int                 temp_pm_id;
    tACL_CONN           *p_acl_cb;

    if (pm_id >= BTM_MAX_PM_RECORDS) {
        pm_id = BTM_PM_SET_ONLY_ID;
    }

    if (p_mode == NULL) {
        return BTM_ILLEGAL_VALUE;
    }

    BTM_TRACE_API( "BTM_SetPowerMode: pm_id %d BDA: %08x mode:0x%x", pm_id,
                   (remote_bda[2] << 24) + (remote_bda[3] << 16) + (remote_bda[4] << 8) + remote_bda[5], p_mode->mode);

    /* take out the force bit */
    mode = p_mode->mode & ~BTM_PM_MD_FORCE;

    p_acl_cb = btm_bda_to_acl(remote_bda, BT_TRANSPORT_BR_EDR);
    if (p_acl_cb == NULL){
        return BTM_UNKNOWN_ADDR;
    }

    p_cb = p_acl_cb->p_pm_mode_db;
    if (mode != BTM_PM_MD_ACTIVE) {
        /* check if the requested mode is supported */
        ind = mode - BTM_PM_MD_HOLD; /* make it base 0 */
        p_features = BTM_ReadLocalFeatures();
        if ( !(p_features[ btm_pm_mode_off[ind] ] & btm_pm_mode_msk[ind] ) ) {
            return BTM_MODE_UNSUPPORTED;
        }
    }

    if (mode == p_cb->state) { /* the requested mode is current mode */
        /* already in the requested mode and the current interval has less latency than the max */
        if ( (mode == BTM_PM_MD_ACTIVE) ||
                ((p_mode->mode & BTM_PM_MD_FORCE) && (p_mode->max >= p_cb->interval) && (p_mode->min <= p_cb->interval)) ||
                ((p_mode->mode & BTM_PM_MD_FORCE) == 0 && (p_mode->max >= p_cb->interval)) ) {
            BTM_TRACE_DEBUG( "BTM_SetPowerMode: mode:0x%x interval %d max:%d, min:%d", p_mode->mode, p_cb->interval, p_mode->max, p_mode->min);
            return BTM_SUCCESS;
        }
    }

    temp_pm_id = pm_id;
    if (pm_id == BTM_PM_SET_ONLY_ID) {
        temp_pm_id = BTM_MAX_PM_RECORDS;
    }

    /* update mode database */
    if ( ((pm_id != BTM_PM_SET_ONLY_ID) &&
            (btm_cb.pm_reg_db[pm_id].mask & BTM_PM_REG_SET))
            || ((pm_id == BTM_PM_SET_ONLY_ID)
		&& (btm_cb.pm_pend_link_hdl != BTM_INVALID_HANDLE)) ) {
#if BTM_PM_DEBUG == TRUE
        BTM_TRACE_DEBUG( "BTM_SetPowerMode: Saving cmd acl handle %d temp_pm_id %d", p_acl_cb->hci_handle, temp_pm_id);
#endif  // BTM_PM_DEBUG
        /* Make sure mask is set to BTM_PM_REG_SET */
        btm_cb.pm_reg_db[temp_pm_id].mask |= BTM_PM_REG_SET;
        *(&p_cb->req_mode[temp_pm_id]) = *((tBTM_PM_PWR_MD *)p_mode);
        p_cb->chg_ind = TRUE;
    }

#if BTM_PM_DEBUG == TRUE
    BTM_TRACE_DEBUG( "btm_pm state:0x%x, pm_pend_link_hdl: %d", p_cb->state, btm_cb.pm_pend_link_hdl);
#endif  // BTM_PM_DEBUG
    /* if mode == hold or pending, return */
    if ( (p_cb->state == BTM_PM_STS_HOLD) ||
            (p_cb->state ==  BTM_PM_STS_PENDING) ||
            (btm_cb.pm_pend_link_hdl != BTM_INVALID_HANDLE) ||
            (p_cb->state & BTM_PM_STORED_MASK) ) { /* command pending */
        if (p_acl_cb->hci_handle != btm_cb.pm_pend_link_hdl) {
            /* set the stored mask */
            p_cb->state |= BTM_PM_STORED_MASK;
            BTM_TRACE_DEBUG( "btm_pm state stored:%d", p_acl_cb->hci_handle);
        }
        return BTM_CMD_STORED;
    }



    return btm_pm_snd_md_req(pm_id, p_acl_cb->hci_handle, p_mode);
}

/*******************************************************************************
**
** Function         BTM_ReadPowerMode
**
** Description      This returns the current mode for a specific
**                  ACL connection.
**
** Input Param      remote_bda - device address of desired ACL connection
**
** Output Param     p_mode - address where the current mode is copied into.
**                          BTM_ACL_MODE_NORMAL
**                          BTM_ACL_MODE_HOLD
**                          BTM_ACL_MODE_SNIFF
**                          BTM_ACL_MODE_PARK
**                          (valid only if return code is BTM_SUCCESS)
**
** Returns          BTM_SUCCESS if successful,
**                  BTM_UNKNOWN_ADDR if bd addr is not active or bad
**
*******************************************************************************/
tBTM_STATUS BTM_ReadPowerMode (BD_ADDR remote_bda, tBTM_PM_MODE *p_mode)
{
    tACL_CONN *p_acl_cb = btm_bda_to_acl(remote_bda, BT_TRANSPORT_BR_EDR);
    if (!p_acl_cb) {
        return (BTM_UNKNOWN_ADDR);
    }

    *p_mode = p_acl_cb->p_pm_mode_db->state;
    return BTM_SUCCESS;
}

/*******************************************************************************
**
** Function         BTM_SetSsrParams
**
** Description      This sends the given SSR parameters for the given ACL
**                  connection if it is in ACTIVE mode.
**
** Input Param      remote_bda - device address of desired ACL connection
**                  max_lat    - maximum latency (in 0.625ms)(0-0xFFFE)
**                  min_rmt_to - minimum remote timeout
**                  min_loc_to - minimum local timeout
**
**
** Returns          BTM_SUCCESS if the HCI command is issued successful,
**                  BTM_UNKNOWN_ADDR if bd addr is not active or bad
**                  BTM_CMD_STORED if the command is stored
**
*******************************************************************************/
tBTM_STATUS BTM_SetSsrParams (BD_ADDR remote_bda, UINT16 max_lat,
                              UINT16 min_rmt_to, UINT16 min_loc_to)
{
#if (BTM_SSR_INCLUDED == TRUE)
    tBTM_PM_MCB *p_cb;
    tACL_CONN *p_acl_cb = NULL;

    p_acl_cb = btm_bda_to_acl(remote_bda, BT_TRANSPORT_BR_EDR);
    if (!p_acl_cb) {
        return (BTM_UNKNOWN_ADDR);
    }
    p_cb = p_acl_cb->p_pm_mode_db;

    if (BTM_PM_STS_ACTIVE == p_cb->state ||
            BTM_PM_STS_SNIFF == p_cb->state) {
        if (btsnd_hcic_sniff_sub_rate(p_acl_cb->hci_handle, max_lat,
                                      min_rmt_to, min_loc_to)) {
            return BTM_SUCCESS;
        } else {
            return BTM_NO_RESOURCES;
        }
    }
    p_cb->max_lat       = max_lat;
    p_cb->min_rmt_to    = min_rmt_to;
    p_cb->min_loc_to    = min_loc_to;
    return BTM_CMD_STORED;
#else
    return BTM_ILLEGAL_ACTION;
#endif  // BTM_SSR_INCLUDED
}

/*******************************************************************************
**
** Function         btm_pm_reset
**
** Description      as a part of the BTM reset process.
**
** Returns          void
**
*******************************************************************************/
void btm_pm_reset(void)
{
    int xx;
    tBTM_PM_STATUS_CBACK *cb = NULL;

    /* clear the pending request for application */
    if ( (btm_cb.pm_pend_id != BTM_PM_SET_ONLY_ID) &&
            (btm_cb.pm_reg_db[btm_cb.pm_pend_id].mask & BTM_PM_REG_NOTIF) ) {
        cb = btm_cb.pm_reg_db[btm_cb.pm_pend_id].cback;
    }


    /* clear the register record */
    for (xx = 0; xx < BTM_MAX_PM_RECORDS; xx++) {
        btm_cb.pm_reg_db[xx].mask = BTM_PM_REC_NOT_USED;
    }

    if (cb != NULL && btm_cb.pm_pend_link_hdl != BTM_INVALID_HANDLE) {
        (*cb)((btm_handle_to_acl(btm_cb.pm_pend_link_hdl))->remote_addr, BTM_PM_STS_ERROR, BTM_DEV_RESET, 0);
    }

    /* no command pending */
    btm_cb.pm_pend_link_hdl = BTM_INVALID_HANDLE;
}

/*******************************************************************************
**
** Function         btm_pm_sm_alloc
**
** Description      This function initializes the control block of an ACL link.
**                  It is called when an ACL connection is created.
**
** Returns          void
**
*******************************************************************************/
tBTM_PM_MCB *btm_pm_sm_alloc(void)
{
    tBTM_PM_MCB *p_db = (tBTM_PM_MCB *) osi_malloc(sizeof(tBTM_PM_MCB));   /* per ACL link */
    if (p_db) {
        memset (p_db, 0, sizeof(tBTM_PM_MCB));
        p_db->state = BTM_PM_ST_ACTIVE;
        if (list_length(btm_cb.p_pm_mode_db_list) >= MAX_L2CAP_LINKS) {
    	    osi_free(p_db);
            p_db = NULL;
        }
        if (!list_append(btm_cb.p_pm_mode_db_list, p_db)) {
    	    osi_free(p_db);
            p_db = NULL;
        }
    }
    return p_db;
}
/*******************************************************************************
**
** Function         btm_pm_find_acl_ind
**
** Description      This function initializes the control block of an ACL link.
**                  It is called when an ACL connection is created.
**
** Returns          void
**
*******************************************************************************/

/*******************************************************************************
**
** Function     btm_pm_compare_modes
** Description  get the "more active" mode of the 2
** Returns      void
**
*******************************************************************************/
static tBTM_PM_PWR_MD *btm_pm_compare_modes(tBTM_PM_PWR_MD *p_md1, tBTM_PM_PWR_MD *p_md2, tBTM_PM_PWR_MD *p_res)
{
    UINT8 res;

    if (p_md1 == NULL) {
        *p_res = *p_md2;
        p_res->mode &= ~BTM_PM_MD_FORCE;

        return p_md2;
    }

    if (p_md2->mode == BTM_PM_MD_ACTIVE || p_md1->mode == BTM_PM_MD_ACTIVE) {
        return NULL;
    }

    /* check if force bit is involved */
    if (p_md1->mode & BTM_PM_MD_FORCE) {
        *p_res = *p_md1;
        p_res->mode &= ~BTM_PM_MD_FORCE;
        return p_res;
    }

    if (p_md2->mode & BTM_PM_MD_FORCE) {
        *p_res = *p_md2;
        p_res->mode &= ~BTM_PM_MD_FORCE;
        return p_res;
    }

    res = (p_md1->mode - 1) * BTM_PM_NUM_SET_MODES + (p_md2->mode - 1);
    res = btm_pm_md_comp_matrix[res];
    switch (res) {
    case BTM_PM_GET_MD1:
        *p_res = *p_md1;
        return p_md1;

    case BTM_PM_GET_MD2:
        *p_res = *p_md2;
        return p_md2;

    case BTM_PM_GET_COMP:
        p_res->mode = p_md1->mode;
        /* min of the two */
        p_res->max  = (p_md1->max < p_md2->max) ? (p_md1->max) : (p_md2->max);
        /* max of the two */
        p_res->min  = (p_md1->min > p_md2->min) ? (p_md1->min) : (p_md2->min);

        /* the intersection is NULL */
        if ( p_res->max < p_res->min) {
            return NULL;
        }

        if (p_res->mode == BTM_PM_MD_SNIFF) {
            /* max of the two */
            p_res->attempt  = (p_md1->attempt > p_md2->attempt) ? (p_md1->attempt) : (p_md2->attempt);
            p_res->timeout  = (p_md1->timeout > p_md2->timeout) ? (p_md1->timeout) : (p_md2->timeout);
        }
        return p_res;
    }
    return NULL;
}

/*******************************************************************************
**
** Function     btm_pm_get_set_mode
** Description  get the resulting mode from the registered parties, then compare it
**              with the requested mode, if the command is from an unregistered party.
** Returns      void
**
*******************************************************************************/
static tBTM_PM_MODE btm_pm_get_set_mode(UINT8 pm_id, tBTM_PM_MCB *p_cb, tBTM_PM_PWR_MD *p_mode, tBTM_PM_PWR_MD *p_res)
{
    int   xx, loop_max;
    tBTM_PM_PWR_MD *p_md = NULL;

    if (p_mode != NULL && p_mode->mode & BTM_PM_MD_FORCE) {
        *p_res = *p_mode;
        p_res->mode &= ~BTM_PM_MD_FORCE;
        return p_res->mode;
    }

    if (!p_mode) {
        loop_max = BTM_MAX_PM_RECORDS + 1;
    } else {
        loop_max = BTM_MAX_PM_RECORDS;
    }

    for ( xx = 0; xx < loop_max; xx++) {
        /* g through all the registered "set" parties */
        if (btm_cb.pm_reg_db[xx].mask & BTM_PM_REG_SET) {
            if (p_cb->req_mode[xx].mode == BTM_PM_MD_ACTIVE) {
                /* if at least one registered (SET) party says ACTIVE, stay active */
                return BTM_PM_MD_ACTIVE;
            } else {
                /* if registered parties give conflicting information, stay active */
                if ( (btm_pm_compare_modes(p_md, &p_cb->req_mode[xx], p_res)) == NULL) {
                    return BTM_PM_MD_ACTIVE;
                }
                p_md = p_res;
            }
        }
    }

    /* if the resulting mode is NULL(nobody registers SET), use the requested mode */
    if (p_md == NULL) {
        if (p_mode) {
            *p_res = *((tBTM_PM_PWR_MD *)p_mode);
        } else { /* p_mode is NULL when btm_pm_snd_md_req is called from btm_pm_proc_mode_change */
            return BTM_PM_MD_ACTIVE;
        }
    } else {
        /* if the command is from unregistered party,
           compare the resulting mode from registered party*/
        if ( (pm_id == BTM_PM_SET_ONLY_ID) &&
                ((btm_pm_compare_modes(p_mode, p_md, p_res)) == NULL) ) {
            return BTM_PM_MD_ACTIVE;
        }
    }

    return p_res->mode;
}

/*******************************************************************************
**
** Function     btm_pm_snd_md_req
** Description  get the resulting mode and send the resuest to host controller
** Returns      tBTM_STATUS
**
*******************************************************************************/
static tBTM_STATUS btm_pm_snd_md_req(UINT8 pm_id, UINT16 link_hdl, tBTM_PM_PWR_MD *p_mode)
{
    tBTM_PM_PWR_MD  md_res;
    tBTM_PM_MODE    mode;
    tACL_CONN   *p_acl_cb = btm_handle_to_acl(link_hdl);
    tBTM_PM_MCB *p_cb = p_acl_cb->p_pm_mode_db;
    BOOLEAN      chg_ind = FALSE;

    mode = btm_pm_get_set_mode(pm_id, p_cb, p_mode, &md_res);
    md_res.mode = mode;

#if BTM_PM_DEBUG == TRUE
    BTM_TRACE_DEBUG( "btm_pm_snd_md_req link_hdl:%d, mode: %d",
                     link_hdl, mode);
#endif  // BTM_PM_DEBUG

    if ( p_cb->state == mode) {
        /* already in the resulting mode */
        if ( (mode == BTM_PM_MD_ACTIVE) ||
                ((md_res.max >= p_cb->interval) && (md_res.min <= p_cb->interval)) ) {
            // Clear request change indication because already in result mode
            p_cb->chg_ind = FALSE;
            return BTM_CMD_STORED;
        }
        /* Otherwise, needs to wake, then sleep */
        chg_ind = TRUE;
    }
    p_cb->chg_ind = chg_ind;

    /* cannot go directly from current mode to resulting mode. */
    if ( mode != BTM_PM_MD_ACTIVE && p_cb->state != BTM_PM_MD_ACTIVE) {
        p_cb->chg_ind = TRUE;    /* needs to wake, then sleep */
    }

    if (p_cb->chg_ind == TRUE) { /* needs to wake first */
        md_res.mode = BTM_PM_MD_ACTIVE;
    }
#if (BTM_SSR_INCLUDED == TRUE)
    else if (BTM_PM_MD_SNIFF == md_res.mode && p_cb->max_lat) {
        btsnd_hcic_sniff_sub_rate(link_hdl, p_cb->max_lat,
                                  p_cb->min_rmt_to, p_cb->min_loc_to);
        p_cb->max_lat = 0;
    }
#endif  // BTM_SSR_INCLUDED
    /* Default is failure */
    btm_cb.pm_pend_link_hdl = BTM_INVALID_HANDLE;

    /* send the appropriate HCI command */
    btm_cb.pm_pend_id   = pm_id;

#if BTM_PM_DEBUG == TRUE
    BTM_TRACE_DEBUG("btm_pm_snd_md_req state:0x%x, link_hdl: %d", p_cb->state, link_hdl);
#endif  // BTM_PM_DEBUG

    BTM_TRACE_DEBUG("%s switching from %s to %s.", __func__, mode_to_string(p_cb->state), mode_to_string(md_res.mode));
    switch (md_res.mode) {
    case BTM_PM_MD_ACTIVE:
        switch (p_cb->state) {
        case BTM_PM_MD_SNIFF:
            if (btsnd_hcic_exit_sniff_mode(link_hdl)) {
                btm_cb.pm_pend_link_hdl = link_hdl;
            }
            break;
        case BTM_PM_MD_PARK:
            if (btsnd_hcic_exit_park_mode(link_hdl)) {
                btm_cb.pm_pend_link_hdl = link_hdl;
            }
            break;
        default:
            /* Failure btm_cb.pm_pend_link = MAX_L2CAP_LINKS */
            break;
        }
        break;

    case BTM_PM_MD_HOLD:
        if (btsnd_hcic_hold_mode (link_hdl,
                                  md_res.max, md_res.min)) {
            btm_cb.pm_pend_link_hdl = link_hdl;
        }
        break;

    case BTM_PM_MD_SNIFF:
        if (btsnd_hcic_sniff_mode (link_hdl,
                                   md_res.max, md_res.min, md_res.attempt,
                                   md_res.timeout)) {
            btm_cb.pm_pend_link_hdl = link_hdl;
        }
        break;

    case BTM_PM_MD_PARK:
        if (btsnd_hcic_park_mode (link_hdl,
                                  md_res.max, md_res.min)) {
            btm_cb.pm_pend_link_hdl = link_hdl;
        }
        break;
    default:
        /* Failure btm_cb.pm_pend_link = MAX_L2CAP_LINKS */
        break;
    }

    if (btm_cb.pm_pend_link_hdl == BTM_INVALID_HANDLE) {
        /* the command was not sent */
#if BTM_PM_DEBUG == TRUE
        BTM_TRACE_DEBUG( "pm_pend_link_hdl: %d", btm_cb.pm_pend_link_hdl);
#endif  // BTM_PM_DEBUG
        return (BTM_NO_RESOURCES);
    }

    return BTM_CMD_STARTED;
}

/*******************************************************************************
**
** Function         btm_pm_check_stored
**
** Description      This function is called when an HCI command status event occurs
**                  to check if there's any PM command issued while waiting for
**                  HCI command status.
**
** Returns          none.
**
*******************************************************************************/
static void btm_pm_check_stored(void)
{
    tACL_CONN   *p_acl_cb = NULL;
    list_node_t *p_node   = NULL;
    for (p_node = list_begin(btm_cb.p_acl_db_list); p_node; p_node = list_next(p_node)) {
	p_acl_cb = list_node(p_node);
        if (p_acl_cb->p_pm_mode_db->state & BTM_PM_STORED_MASK) {
            p_acl_cb->p_pm_mode_db->state &= ~BTM_PM_STORED_MASK;
            BTM_TRACE_DEBUG( "btm_pm_check_stored :%d", p_acl_cb->hci_handle);
            btm_pm_snd_md_req(BTM_PM_SET_ONLY_ID, p_acl_cb->hci_handle, NULL);
            break;
        }
    }

}


/*******************************************************************************
**
** Function         btm_pm_proc_cmd_status
**
** Description      This function is called when an HCI command status event occurs
**                  for power manager related commands.
**
** Input Params     status - status of the event (HCI_SUCCESS if no errors)
**
** Returns          none.
**
*******************************************************************************/
void btm_pm_proc_cmd_status(UINT8 status)
{
    tBTM_PM_MCB     *p_cb;
    tBTM_PM_STATUS  pm_status;
    tACL_CONN       *p_acl_cb;

    if (btm_cb.pm_pend_link_hdl == BTM_INVALID_HANDLE) {
        return;
    }


    p_acl_cb = btm_handle_to_acl(btm_cb.pm_pend_link_hdl);
    if (p_acl_cb == NULL) {
        return;
    }
    p_cb = p_acl_cb->p_pm_mode_db;

    if (status == HCI_SUCCESS) {
        p_cb->state = BTM_PM_ST_PENDING;
        pm_status = BTM_PM_STS_PENDING;
#if BTM_PM_DEBUG == TRUE
        BTM_TRACE_DEBUG( "btm_pm_proc_cmd_status new state:0x%x", p_cb->state);
#endif // BTM_PM_DEBUG
    } else { /* the command was not successful. Stay in the same state */
        pm_status = BTM_PM_STS_ERROR;
    }

    /* notify the caller is appropriate */
    if ( (btm_cb.pm_pend_id != BTM_PM_SET_ONLY_ID) &&
            (btm_cb.pm_reg_db[btm_cb.pm_pend_id].mask & BTM_PM_REG_NOTIF) ) {
        (*btm_cb.pm_reg_db[btm_cb.pm_pend_id].cback)(p_acl_cb->remote_addr, pm_status, 0, status);
    }

    /* no pending cmd now */
#if BTM_PM_DEBUG == TRUE
    BTM_TRACE_DEBUG( "btm_pm_proc_cmd_status state:0x%x, pm_pend_link: %d(new: %d)",
                     p_cb->state, btm_cb.pm_pend_link_hdl, MAX_L2CAP_LINKS);
#endif  // BTM_PM_DEBUG
    btm_cb.pm_pend_link_hdl = BTM_INVALID_HANDLE;

    btm_pm_check_stored();
}

/*******************************************************************************
**
** Function         btm_process_mode_change
**
** Description      This function is called when an HCI mode change event occurs.
**
** Input Params     hci_status - status of the event (HCI_SUCCESS if no errors)
**                  hci_handle - connection handle associated with the change
**                  mode - HCI_MODE_ACTIVE, HCI_MODE_HOLD, HCI_MODE_SNIFF, or HCI_MODE_PARK
**                  interval - number of baseband slots (meaning depends on mode)
**
** Returns          none.
**
*******************************************************************************/
void btm_pm_proc_mode_change (UINT8 hci_status, UINT16 hci_handle, UINT8 mode, UINT16 interval)
{
    tACL_CONN   *p;
    tBTM_PM_MCB *p_cb = NULL;
    int yy;
    tBTM_PM_STATE  old_state;
    tL2C_LCB        *p_lcb;

    /* get the index to acl_db */
    p = btm_handle_to_acl(hci_handle);
    if (!p) {
        return;
    }

    /* update control block */
    p_cb = p->p_pm_mode_db;
    old_state       = p_cb->state;
    p_cb->state     = mode;
    p_cb->interval  = interval;

    BTM_TRACE_DEBUG("%s switched from %s to %s.", __func__, mode_to_string(old_state), mode_to_string(p_cb->state));

    if ((p_lcb = l2cu_find_lcb_by_bd_addr(p->remote_addr, BT_TRANSPORT_BR_EDR)) != NULL) {
        if ((p_cb->state == BTM_PM_ST_ACTIVE) || (p_cb->state == BTM_PM_ST_SNIFF)) {
            /* There might be any pending packets due to SNIFF or PENDING state */
            /* Trigger L2C to start transmission of the pending packets. */
            BTM_TRACE_DEBUG("btm mode change to active; check l2c_link for outgoing packets");
            l2c_link_check_send_pkts(p_lcb, NULL, NULL);
        }
    }

    /* notify registered parties */
    for (yy = 0; yy <= BTM_MAX_PM_RECORDS; yy++) {
        /* set req_mode  HOLD mode->ACTIVE */
        if ( (mode == BTM_PM_MD_ACTIVE) && (p_cb->req_mode[yy].mode == BTM_PM_MD_HOLD) ) {
            p_cb->req_mode[yy].mode = BTM_PM_MD_ACTIVE;
        }
    }

    /* new request has been made. - post a message to BTU task */
    if (old_state & BTM_PM_STORED_MASK) {
#if BTM_PM_DEBUG == TRUE
        BTM_TRACE_DEBUG( "btm_pm_proc_mode_change: Sending stored req:%d", xx);
#endif  // BTM_PM_DEBUG
        btm_pm_snd_md_req(BTM_PM_SET_ONLY_ID, hci_handle, NULL);
    } else {
        list_node_t *p_node = NULL;

        for (p_node =(list_begin(btm_cb.p_pm_mode_db_list)); p_node; p_node = (list_next(p_node))) {
	    p_cb = (tBTM_PM_MCB *)list_node(p_node);
	    if (p_cb->chg_ind == TRUE) {
#if BTM_PM_DEBUG == TRUE
                BTM_TRACE_DEBUG( "btm_pm_proc_mode_change: Sending PM req :%d", zz);
#endif   // BTM_PM_DEBUG
                btm_pm_snd_md_req(BTM_PM_SET_ONLY_ID, hci_handle, NULL);
                break;
            }
        }
    }


    /* notify registered parties */
    for (yy = 0; yy < BTM_MAX_PM_RECORDS; yy++) {
        if (btm_cb.pm_reg_db[yy].mask & BTM_PM_REG_NOTIF) {
            (*btm_cb.pm_reg_db[yy].cback)( p->remote_addr, mode, interval, hci_status);
        }
    }

    /* If mode change was because of an active role switch or change link key */
    btm_cont_rswitch(p, btm_find_dev(p->remote_addr), hci_status);
}

/*******************************************************************************
**
** Function         btm_pm_proc_ssr_evt
**
** Description      This function is called when an HCI sniff subrating event occurs.
**
** Returns          none.
**
*******************************************************************************/
#if (BTM_SSR_INCLUDED == TRUE)
void btm_pm_proc_ssr_evt (UINT8 *p, UINT16 evt_len)
{
    UINT8       status;
    UINT16      handle;
    UINT16      max_rx_lat;
    int         xx;
    tBTM_PM_MCB *p_cb;
    tACL_CONN   *p_acl = NULL;
    UINT16      use_ssr = TRUE;
    UNUSED(evt_len);

    STREAM_TO_UINT8 (status, p);

    STREAM_TO_UINT16 (handle, p);
    /* get the index to acl_db */

    p += 2;
    STREAM_TO_UINT16 (max_rx_lat, p);
    p_acl = btm_handle_to_acl(handle);
    if (!p_acl) {
        return;
    }
    p_cb = p_acl->p_pm_mode_db;
    if (p_cb->interval == max_rx_lat) {
        /* using legacy sniff */
        use_ssr = FALSE;
    }

    /* notify registered parties */
    for (xx = 0; xx < BTM_MAX_PM_RECORDS; xx++) {
        if (btm_cb.pm_reg_db[xx].mask & BTM_PM_REG_NOTIF) {
            if ( p_acl) {
                (*btm_cb.pm_reg_db[xx].cback)( p_acl->remote_addr, BTM_PM_STS_SSR, use_ssr, status);
            }
        }
    }
}
#endif  // BTM_SSR_INCLUDED

/*******************************************************************************
**
** Function         btm_pm_device_in_active_or_sniff_mode
**
** Description      This function is called to check if in active or sniff mode
**
** Returns          TRUE, if in active or sniff mode
**
*******************************************************************************/
BOOLEAN btm_pm_device_in_active_or_sniff_mode(void)
{
    /* The active state is the highest state-includes connected device and sniff mode*/

    /* Covers active and sniff modes */
    if (BTM_GetNumAclLinks() > 0) {
        BTM_TRACE_DEBUG("%s - ACL links: %d", __func__, BTM_GetNumAclLinks());
        return TRUE;
    }

#if ((defined BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
    /* Check BLE states */
    if (btm_ble_get_conn_st() != BLE_CONN_IDLE) {
        BTM_TRACE_DEBUG("%s - BLE state: %x", __func__, btm_ble_get_conn_st());
        return TRUE;
    }
#endif

    return FALSE;
}

/*******************************************************************************
**
** Function         btm_pm_device_in_scan_state
**
** Description      This function is called to check if in paging, inquiry or connecting mode
**
** Returns          TRUE, if in paging, inquiry or connecting mode
**
*******************************************************************************/
BOOLEAN btm_pm_device_in_scan_state(void)
{
    /* Scan state-paging, inquiry, and trying to connect */

    /* Check for paging */
    if (btm_cb.is_paging || (!fixed_queue_is_empty(btm_cb.page_queue)) ||
            BTM_BL_PAGING_STARTED == btm_cb.busy_level) {
        BTM_TRACE_DEBUG("btm_pm_device_in_scan_state- paging");
        return TRUE;
    }

    /* Check for inquiry */
    if ((btm_cb.btm_inq_vars.inq_active & (BTM_BR_INQ_ACTIVE_MASK | BTM_BLE_INQ_ACTIVE_MASK)) != 0) {
        BTM_TRACE_DEBUG("btm_pm_device_in_scan_state- Inq active");
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
**
** Function         BTM_PM_ReadControllerState
**
** Description      This function is called to obtain the controller state
**
** Returns          Controller State-BTM_CONTRL_ACTIVE, BTM_CONTRL_SCAN, and BTM_CONTRL_IDLE
**
*******************************************************************************/
tBTM_CONTRL_STATE BTM_PM_ReadControllerState(void)
{
    if (TRUE == btm_pm_device_in_active_or_sniff_mode()) {
        return BTM_CONTRL_ACTIVE;
    } else if (TRUE == btm_pm_device_in_scan_state()) {
        return BTM_CONTRL_SCAN;
    } else {
        return BTM_CONTRL_IDLE;
    }
}

#if (!CONFIG_BT_STACK_NO_LOG)
static const char *mode_to_string(tBTM_PM_MODE mode)
{
    switch (mode) {
    case BTM_PM_MD_ACTIVE: return "ACTIVE";
    case BTM_PM_MD_SNIFF:  return "SNIFF";
    case BTM_PM_MD_PARK:   return "PARK";
    case BTM_PM_MD_HOLD:   return "HOLD";
    default:               return "UNKNOWN";
    }
}
#endif

/******************************************************************************
 *
 *  Copyright (C) 2008-2014 Broadcom Corporation
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
 *  This file contains functions for BLE GAP.
 *
 ******************************************************************************/

#include <string.h>
//#include <stdio.h>
#include <stddef.h>

#include "bt_types.h"
//#include "bt_utils.h"
#include "btm_int.h"
#include "btm_ble_api.h"
#include "btu.h"
#include "controller.h"
#include "hcimsgs.h"
#include "gap_api.h"

#if BLE_INCLUDED == TRUE
#include "l2c_int.h"

#include "gattdefs.h"
#include "gatt_int.h"

#include "btm_ble_int.h"
//#define LOG_TAG "bt_btm_ble"
//#include "osi/include/log.h"

#define BTM_BLE_NAME_SHORT                  0x01
#define BTM_BLE_NAME_CMPL                   0x02

#define BTM_BLE_FILTER_TARGET_UNKNOWN       0xff
#define BTM_BLE_POLICY_UNKNOWN              0xff

#define BTM_EXT_BLE_RMT_NAME_TIMEOUT        30
#define MIN_ADV_LENGTH                       2
#define BTM_VSC_CHIP_CAPABILITY_RSP_LEN_L_RELEASE 9

static tBTM_BLE_VSC_CB cmn_ble_vsc_cb;

#if BLE_VND_INCLUDED == TRUE
static tBTM_BLE_CTRL_FEATURES_CBACK    *p_ctrl_le_feature_rd_cmpl_cback = NULL;
#endif

/*******************************************************************************
**  Local functions
*******************************************************************************/
static void btm_ble_update_adv_flag(UINT8 flag);
static void btm_ble_process_adv_pkt_cont(BD_ADDR bda, UINT8 addr_type, UINT8 evt_type, UINT8 *p);
UINT8 *btm_ble_build_adv_data(tBTM_BLE_AD_MASK *p_data_mask, UINT8 **p_dst,
                              tBTM_BLE_ADV_DATA *p_data);
static UINT8 btm_set_conn_mode_adv_init_addr(tBTM_BLE_INQ_CB *p_cb,
        BD_ADDR_PTR p_peer_addr_ptr,
        tBLE_ADDR_TYPE *p_peer_addr_type,
        tBLE_ADDR_TYPE *p_own_addr_type);
static void btm_ble_stop_observe(void);

#define BTM_BLE_INQ_RESULT          0x01
#define BTM_BLE_OBS_RESULT          0x02
#define BTM_BLE_SEL_CONN_RESULT     0x04

/* LE states combo bit to check */
const UINT8 btm_le_state_combo_tbl[BTM_BLE_STATE_MAX][BTM_BLE_STATE_MAX][2] = {
    {/* single state support */
        {HCI_SUPP_LE_STATES_CONN_ADV_MASK, HCI_SUPP_LE_STATES_CONN_ADV_OFF},  /* conn_adv */
        {HCI_SUPP_LE_STATES_INIT_MASK, HCI_SUPP_LE_STATES_INIT_OFF}, /* init */
        {HCI_SUPP_LE_STATES_INIT_MASK, HCI_SUPP_LE_STATES_INIT_OFF}, /* master */
        {HCI_SUPP_LE_STATES_SLAVE_MASK, HCI_SUPP_LE_STATES_SLAVE_OFF}, /* slave */
        {0, 0},                   /* todo: lo du dir adv, not covered ? */
        {HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_MASK, HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_OFF}, /* hi duty dir adv */
        {HCI_SUPP_LE_STATES_NON_CONN_ADV_MASK, HCI_SUPP_LE_STATES_NON_CONN_ADV_OFF},  /* non connectable adv */
        {HCI_SUPP_LE_STATES_PASS_SCAN_MASK, HCI_SUPP_LE_STATES_PASS_SCAN_OFF},   /*  passive scan */
        {HCI_SUPP_LE_STATES_ACTIVE_SCAN_MASK, HCI_SUPP_LE_STATES_ACTIVE_SCAN_OFF},  /*   active scan */
        {HCI_SUPP_LE_STATES_SCAN_ADV_MASK, HCI_SUPP_LE_STATESSCAN_ADV_OFF}   /* scanable adv */
    },
    {    /* conn_adv =0 */
        {0, 0},                                                                           /* conn_adv */
        {HCI_SUPP_LE_STATES_CONN_ADV_INIT_MASK, HCI_SUPP_LE_STATES_CONN_ADV_INIT_OFF},      /* init: 32 */
        {HCI_SUPP_LE_STATES_CONN_ADV_MASTER_MASK, HCI_SUPP_LE_STATES_CONN_ADV_MASTER_OFF},  /* master: 35 */
        {HCI_SUPP_LE_STATES_CONN_ADV_SLAVE_MASK, HCI_SUPP_LE_STATES_CONN_ADV_SLAVE_OFF}, /* slave: 38,*/
        {0, 0},                                                                           /* lo du dir adv */
        {0, 0},                                                                            /* hi duty dir adv */
        {0, 0},  /* non connectable adv */
        {HCI_SUPP_LE_STATES_CONN_ADV_PASS_SCAN_MASK, HCI_SUPP_LE_STATES_CONN_ADV_PASS_SCAN_OFF},   /*  passive scan */
        {HCI_SUPP_LE_STATES_CONN_ADV_ACTIVE_SCAN_MASK, HCI_SUPP_LE_STATES_CONN_ADV_ACTIVE_SCAN_OFF},  /*   active scan */
        {0, 0}   /* scanable adv */
    },
    {   /* init */
        {HCI_SUPP_LE_STATES_CONN_ADV_INIT_MASK, HCI_SUPP_LE_STATES_CONN_ADV_INIT_OFF},      /* conn_adv: 32 */
        {0, 0},                                                                             /* init */
        {HCI_SUPP_LE_STATES_INIT_MASTER_MASK, HCI_SUPP_LE_STATES_INIT_MASTER_OFF},          /* master 28 */
        {HCI_SUPP_LE_STATES_INIT_MASTER_SLAVE_MASK, HCI_SUPP_LE_STATES_INIT_MASTER_SLAVE_OFF}, /* slave 41 */
        {HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_INIT_MASK, HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_INIT_OFF} ,/* lo du dir adv 34 */
        {HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_INIT_MASK, HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_INIT_OFF},     /* hi duty dir adv 33 */
        {HCI_SUPP_LE_STATES_NON_CONN_INIT_MASK, HCI_SUPP_LE_STATES_NON_CONN_INIT_OFF},  /*  non connectable adv */
        {HCI_SUPP_LE_STATES_PASS_SCAN_INIT_MASK, HCI_SUPP_LE_STATES_PASS_SCAN_INIT_OFF},   /* passive scan */
        {HCI_SUPP_LE_STATES_ACTIVE_SCAN_INIT_MASK, HCI_SUPP_LE_STATES_ACTIVE_SCAN_INIT_OFF},  /*  active scan */
        {HCI_SUPP_LE_STATES_SCAN_ADV_INIT_MASK, HCI_SUPP_LE_STATES_SCAN_ADV_INIT_OFF}   /* scanable adv */

    },
    {   /* master */
        {HCI_SUPP_LE_STATES_CONN_ADV_MASTER_MASK, HCI_SUPP_LE_STATES_CONN_ADV_MASTER_OFF},  /* conn_adv: 35 */
        {HCI_SUPP_LE_STATES_INIT_MASTER_MASK, HCI_SUPP_LE_STATES_INIT_MASTER_OFF},          /* init 28 */
        {HCI_SUPP_LE_STATES_INIT_MASTER_MASK, HCI_SUPP_LE_STATES_INIT_MASTER_OFF},          /* master 28 */
        {HCI_SUPP_LE_STATES_CONN_ADV_INIT_MASK, HCI_SUPP_LE_STATES_CONN_ADV_INIT_OFF},      /* slave: 32 */
        {HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_MASTER_MASK, HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_MASTER_OFF},  /* lo duty cycle adv 37 */
        {HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_MASTER_MASK, HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_MASTER_OFF},   /* hi duty cycle adv 36 */
        {HCI_SUPP_LE_STATES_NON_CONN_ADV_MASTER_MASK, HCI_SUPP_LE_STATES_NON_CONN_ADV_MASTER_OFF},  /*  non connectable adv */
        {HCI_SUPP_LE_STATES_PASS_SCAN_MASTER_MASK, HCI_SUPP_LE_STATES_PASS_SCAN_MASTER_OFF},   /*  passive scan */
        {HCI_SUPP_LE_STATES_ACTIVE_SCAN_MASTER_MASK, HCI_SUPP_LE_STATES_ACTIVE_SCAN_MASTER_OFF},  /*   active scan */
        {HCI_SUPP_LE_STATES_SCAN_ADV_MASTER_MASK, HCI_SUPP_LE_STATES_SCAN_ADV_MASTER_OFF}   /*  scanable adv */

    },
    { /* slave */
        {HCI_SUPP_LE_STATES_CONN_ADV_SLAVE_MASK, HCI_SUPP_LE_STATES_CONN_ADV_SLAVE_OFF}, /* conn_adv: 38,*/
        {HCI_SUPP_LE_STATES_INIT_MASTER_SLAVE_MASK, HCI_SUPP_LE_STATES_INIT_MASTER_SLAVE_OFF}, /* init 41 */
        {HCI_SUPP_LE_STATES_INIT_MASTER_SLAVE_MASK, HCI_SUPP_LE_STATES_INIT_MASTER_SLAVE_OFF}, /* master 41 */
        {HCI_SUPP_LE_STATES_CONN_ADV_SLAVE_MASK, HCI_SUPP_LE_STATES_CONN_ADV_SLAVE_OFF},        /* slave: 38,*/
        {HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_SLAVE_MASK, HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_SLAVE_OFF},  /* lo duty cycle adv 40 */
        {HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_SLAVE_MASK, HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_SLAVE_OFF},   /* hi duty cycle adv 39 */
        {HCI_SUPP_LE_STATES_NON_CONN_ADV_SLAVE_MASK, HCI_SUPP_LE_STATES_NON_CONN_ADV_SLAVE_OFF},  /* non connectable adv */
        {HCI_SUPP_LE_STATES_PASS_SCAN_SLAVE_MASK, HCI_SUPP_LE_STATES_PASS_SCAN_SLAVE_OFF},   /* passive scan */
        {HCI_SUPP_LE_STATES_ACTIVE_SCAN_SLAVE_MASK, HCI_SUPP_LE_STATES_ACTIVE_SCAN_SLAVE_OFF},  /*  active scan */
        {HCI_SUPP_LE_STATES_SCAN_ADV_SLAVE_MASK, HCI_SUPP_LE_STATES_SCAN_ADV_SLAVE_OFF}   /* scanable adv */

    },
    { /* lo duty cycle adv */
        {0, 0}, /* conn_adv: 38,*/
        {HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_INIT_MASK, HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_INIT_OFF} ,/* init 34 */
        {HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_MASTER_MASK, HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_MASTER_OFF}, /* master 37 */
        {HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_SLAVE_MASK, HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_SLAVE_OFF}, /* slave: 40 */
        {0, 0},  /* lo duty cycle adv 40 */
        {0, 0},   /* hi duty cycle adv 39 */
        {0, 0},  /*  non connectable adv */
        {0, 0},   /* TODO: passive scan, not covered? */
        {0, 0},  /* TODO:  active scan, not covered? */
        {0, 0}   /*  scanable adv */
    },
    { /* hi duty cycle adv */
        {0, 0}, /* conn_adv: 38,*/
        {HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_INIT_MASK, HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_INIT_OFF}, /* init 33 */
        {HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_MASTER_MASK, HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_MASTER_OFF}, /* master 36 */
        {HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_SLAVE_MASK, HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_SLAVE_OFF},   /* slave: 39*/
        {0, 0},  /* lo duty cycle adv 40 */
        {0, 0},   /* hi duty cycle adv 39 */
        {0, 0},  /* non connectable adv */
        {HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_PASS_SCAN_MASK, HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_PASS_SCAN_OFF},   /* passive scan */
        {HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_ACTIVE_SCAN_MASK, HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_ACTIVE_SCAN_OFF},  /* active scan */
        {0, 0}   /* scanable adv */
    },
    { /* non connectable adv */
        {0, 0}, /* conn_adv: */
        {HCI_SUPP_LE_STATES_NON_CONN_INIT_MASK, HCI_SUPP_LE_STATES_NON_CONN_INIT_OFF}, /* init  */
        {HCI_SUPP_LE_STATES_NON_CONN_ADV_MASTER_MASK, HCI_SUPP_LE_STATES_NON_CONN_ADV_MASTER_OFF}, /* master  */
        {HCI_SUPP_LE_STATES_NON_CONN_ADV_SLAVE_MASK, HCI_SUPP_LE_STATES_NON_CONN_ADV_SLAVE_OFF},   /* slave: */
        {0, 0},  /* lo duty cycle adv */
        {0, 0},   /* hi duty cycle adv */
        {0, 0},  /* non connectable adv */
        {HCI_SUPP_LE_STATES_NON_CONN_ADV_PASS_SCAN_MASK, HCI_SUPP_LE_STATES_NON_CONN_ADV_PASS_SCAN_OFF},   /* passive scan */
        {HCI_SUPP_LE_STATES_NON_CONN_ADV_ACTIVE_SCAN_MASK, HCI_SUPP_LE_STATES_NON_CONN_ADV_ACTIVE_SCAN_OFF},  /*  active scan */
        {0, 0}   /* scanable adv */
    },
    { /* passive scan */
        {HCI_SUPP_LE_STATES_CONN_ADV_PASS_SCAN_MASK, HCI_SUPP_LE_STATES_CONN_ADV_PASS_SCAN_OFF}, /* conn_adv: */
        {HCI_SUPP_LE_STATES_PASS_SCAN_INIT_MASK, HCI_SUPP_LE_STATES_PASS_SCAN_INIT_OFF}, /* init  */
        {HCI_SUPP_LE_STATES_PASS_SCAN_MASTER_MASK, HCI_SUPP_LE_STATES_PASS_SCAN_MASTER_OFF}, /* master  */
        {HCI_SUPP_LE_STATES_PASS_SCAN_SLAVE_MASK, HCI_SUPP_LE_STATES_PASS_SCAN_SLAVE_OFF},   /* slave: */
        {0, 0},  /* lo duty cycle adv */
        {HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_PASS_SCAN_MASK, HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_PASS_SCAN_OFF},   /* hi duty cycle adv */
        {HCI_SUPP_LE_STATES_NON_CONN_ADV_PASS_SCAN_MASK, HCI_SUPP_LE_STATES_NON_CONN_ADV_PASS_SCAN_OFF},  /*  non connectable adv */
        {0, 0},   /* passive scan */
        {0, 0},  /* active scan */
        {HCI_SUPP_LE_STATES_SCAN_ADV_PASS_SCAN_MASK, HCI_SUPP_LE_STATES_SCAN_ADV_PASS_SCAN_OFF}   /* scanable adv */
    },
    { /* active scan */
        {HCI_SUPP_LE_STATES_CONN_ADV_ACTIVE_SCAN_MASK, HCI_SUPP_LE_STATES_CONN_ADV_ACTIVE_SCAN_OFF}, /* conn_adv: */
        {HCI_SUPP_LE_STATES_ACTIVE_SCAN_INIT_MASK, HCI_SUPP_LE_STATES_ACTIVE_SCAN_INIT_OFF}, /* init  */
        {HCI_SUPP_LE_STATES_ACTIVE_SCAN_MASTER_MASK, HCI_SUPP_LE_STATES_ACTIVE_SCAN_MASTER_OFF}, /* master  */
        {HCI_SUPP_LE_STATES_ACTIVE_SCAN_SLAVE_MASK, HCI_SUPP_LE_STATES_ACTIVE_SCAN_SLAVE_OFF},   /* slave: */
        {0, 0},  /* lo duty cycle adv */
        {HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_ACTIVE_SCAN_MASK, HCI_SUPP_LE_STATES_HI_DUTY_DIR_ADV_ACTIVE_SCAN_OFF},   /* hi duty cycle adv */
        {HCI_SUPP_LE_STATES_NON_CONN_ADV_ACTIVE_SCAN_MASK, HCI_SUPP_LE_STATES_NON_CONN_ADV_ACTIVE_SCAN_OFF},  /*  non connectable adv */
        {0, 0},   /* TODO: passive scan */
        {0, 0},  /* TODO:  active scan */
        {HCI_SUPP_LE_STATES_SCAN_ADV_ACTIVE_SCAN_MASK, HCI_SUPP_LE_STATES_SCAN_ADV_ACTIVE_SCAN_OFF}   /*  scanable adv */
    },
    { /* scanable adv */
        {0, 0}, /* conn_adv: */
        {HCI_SUPP_LE_STATES_SCAN_ADV_INIT_MASK, HCI_SUPP_LE_STATES_SCAN_ADV_INIT_OFF}, /* init  */
        {HCI_SUPP_LE_STATES_SCAN_ADV_MASTER_MASK, HCI_SUPP_LE_STATES_SCAN_ADV_MASTER_OFF}, /* master  */
        {HCI_SUPP_LE_STATES_SCAN_ADV_SLAVE_MASK, HCI_SUPP_LE_STATES_SCAN_ADV_SLAVE_OFF},   /* slave: */
        {0, 0},  /* lo duty cycle adv */
        {0, 0},   /* hi duty cycle adv */
        {0, 0},  /* non connectable adv */
        {HCI_SUPP_LE_STATES_SCAN_ADV_PASS_SCAN_MASK, HCI_SUPP_LE_STATES_SCAN_ADV_PASS_SCAN_OFF},   /*  passive scan */
        {HCI_SUPP_LE_STATES_SCAN_ADV_ACTIVE_SCAN_MASK, HCI_SUPP_LE_STATES_SCAN_ADV_ACTIVE_SCAN_OFF},  /*  active scan */
        {0, 0}   /* scanable adv */
    }

};
/* check LE combo state supported */
#define BTM_LE_STATES_SUPPORTED(x, y, z)      ((x)[(z)] & (y))


/*******************************************************************************
**
** Function         BTM_BleUpdateAdvWhitelist
**
** Description      Add or remove device from advertising white list
**
** Returns          void
**
*******************************************************************************/
BOOLEAN BTM_BleUpdateAdvWhitelist(BOOLEAN add_remove, BD_ADDR remote_bda)
{
    UNUSED(add_remove);
    UNUSED(remote_bda);

    return FALSE;
}

/*******************************************************************************
**
** Function         BTM_BleUpdateAdvFilterPolicy
**
** Description      This function update the filter policy of advertiser.
**
** Parameter        adv_policy: advertising filter policy
**
** Return           void
*******************************************************************************/
void BTM_BleUpdateAdvFilterPolicy(tBTM_BLE_AFP adv_policy)
{
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;
    tBLE_ADDR_TYPE   init_addr_type = BLE_ADDR_PUBLIC;
    BD_ADDR          p_addr_ptr = {0};
    UINT8            adv_mode = p_cb->adv_mode;

    BTM_TRACE_EVENT ("BTM_BleUpdateAdvFilterPolicy\n");

    if (!controller_get_interface()->supports_ble()) {
        return;
    }

    if (p_cb->afp != adv_policy) {
        p_cb->afp = adv_policy;

        /* if adv active, stop and restart */
        btm_ble_stop_adv ();

        if (p_cb->connectable_mode & BTM_BLE_CONNECTABLE)
            p_cb->evt_type = btm_set_conn_mode_adv_init_addr(p_cb, p_addr_ptr, &init_addr_type,
                             &p_cb->adv_addr_type);

        btsnd_hcic_ble_write_adv_params ((UINT16)(p_cb->adv_interval_min ? p_cb->adv_interval_min :
                                         BTM_BLE_GAP_ADV_SLOW_INT),
                                         (UINT16)(p_cb->adv_interval_max ? p_cb->adv_interval_max :
                                                 BTM_BLE_GAP_ADV_SLOW_INT),
                                         p_cb->evt_type,
                                         p_cb->adv_addr_type,
                                         init_addr_type,
                                         p_addr_ptr,
                                         p_cb->adv_chnl_map,
                                         p_cb->afp);

        if (adv_mode == BTM_BLE_ADV_ENABLE) {
            btm_ble_start_adv ();
        }

    }
}

/*******************************************************************************
**
** Function         btm_ble_send_extended_scan_params
**
** Description      This function sends out the extended scan parameters command to the controller
**
** Parameters       scan_type - Scan type
**                  scan_int - Scan interval
**                  scan_win - Scan window
**                  addr_type_own - Own address type
**                  scan_filter_policy - Scan filter policy
**
** Returns          TRUE or FALSE
**
*******************************************************************************/
BOOLEAN btm_ble_send_extended_scan_params(UINT8 scan_type, UINT32 scan_int,
        UINT32 scan_win, UINT8 addr_type_own,
        UINT8 scan_filter_policy)
{
    UINT8 scan_param[HCIC_PARAM_SIZE_BLE_WRITE_EXTENDED_SCAN_PARAM];
    UINT8 *pp_scan = scan_param;

    memset(scan_param, 0, HCIC_PARAM_SIZE_BLE_WRITE_EXTENDED_SCAN_PARAM);

    UINT8_TO_STREAM(pp_scan, scan_type);
    UINT32_TO_STREAM(pp_scan, scan_int);
    UINT32_TO_STREAM(pp_scan, scan_win);
    UINT8_TO_STREAM(pp_scan, addr_type_own);
    UINT8_TO_STREAM(pp_scan, scan_filter_policy);

    BTM_TRACE_DEBUG("%s, %d, %d", __func__, scan_int, scan_win);
    if ((BTM_VendorSpecificCommand(HCI_BLE_EXTENDED_SCAN_PARAMS_OCF,
                                   HCIC_PARAM_SIZE_BLE_WRITE_EXTENDED_SCAN_PARAM, scan_param, NULL)) != BTM_SUCCESS) {
        BTM_TRACE_ERROR("%s error sending extended scan parameters", __func__);
        return FALSE;
    }
    return TRUE;
}

/*******************************************************************************
**
** Function         BTM_BleObserve
**
** Description      This procedure keep the device listening for advertising
**                  events from a broadcast device.
**
** Parameters       start: start or stop observe.
**                  white_list: use white list in observer mode or not.
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS BTM_BleObserve(BOOLEAN start, UINT8 duration,
                           tBTM_INQ_RESULTS_CB *p_results_cb, tBTM_CMPL_CB *p_cmpl_cb)
{
    tBTM_BLE_INQ_CB *p_inq = &btm_cb.ble_ctr_cb.inq_var;
    tBTM_STATUS status = BTM_WRONG_MODE;

    UINT32 scan_interval = !p_inq->scan_interval ? BTM_BLE_GAP_DISC_SCAN_INT : p_inq->scan_interval;
    UINT32 scan_window = !p_inq->scan_window ? BTM_BLE_GAP_DISC_SCAN_WIN : p_inq->scan_window;

    BTM_TRACE_EVENT ("%s : scan_type:%d, %d, %d\n", __func__, btm_cb.btm_inq_vars.scan_type,
                     p_inq->scan_interval, p_inq->scan_window);

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

    if (start) {
        /* shared inquiry database, do not allow observe if any inquiry is active */
        if (BTM_BLE_IS_OBS_ACTIVE(btm_cb.ble_ctr_cb.scan_activity)) {
            BTM_TRACE_ERROR("%s Observe Already Active", __func__);
            return status;
        }

        btm_cb.ble_ctr_cb.p_obs_results_cb = p_results_cb;
        btm_cb.ble_ctr_cb.p_obs_cmpl_cb = p_cmpl_cb;
        status = BTM_CMD_STARTED;

        /* scan is not started */
        if (!BTM_BLE_IS_SCAN_ACTIVE(btm_cb.ble_ctr_cb.scan_activity)) {
            /* allow config of scan type */
            p_inq->scan_type = (p_inq->scan_type == BTM_BLE_SCAN_MODE_NONE) ?
                               BTM_BLE_SCAN_MODE_ACTI : p_inq->scan_type;
            /* assume observe always not using white list */
#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
            /* enable resolving list */
            btm_ble_enable_resolving_list_for_platform(BTM_BLE_RL_SCAN);
#endif

            if (cmn_ble_vsc_cb.extended_scan_support == 0) {
                btsnd_hcic_ble_set_scan_params(p_inq->scan_type, (UINT16)scan_interval,
                                               (UINT16)scan_window,
                                               btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type,
                                               BTM_BLE_DEFAULT_SFP);
            } else {
                btm_ble_send_extended_scan_params(p_inq->scan_type, scan_interval, scan_window,
                                                  btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type,
                                                  BTM_BLE_DEFAULT_SFP);
            }

            p_inq->scan_duplicate_filter = BTM_BLE_DUPLICATE_DISABLE;
            status = btm_ble_start_scan();
        }

        if (status == BTM_CMD_STARTED) {
            btm_cb.ble_ctr_cb.scan_activity |= BTM_LE_OBSERVE_ACTIVE;
            if (duration != 0)
                /* start observer timer */
            {
                btu_start_timer (&btm_cb.ble_ctr_cb.obs_timer_ent, BTU_TTYPE_BLE_OBSERVE, duration);
            }
        }
    } else if (BTM_BLE_IS_OBS_ACTIVE(btm_cb.ble_ctr_cb.scan_activity)) {
        status = BTM_CMD_STARTED;
        btm_ble_stop_observe();
    } else {
        BTM_TRACE_ERROR("%s Observe not active\n", __func__);
    }

    return status;

}

/*******************************************************************************
**
** Function         BTM_BleBroadcast
**
** Description      This function is to start or stop broadcasting.
**
** Parameters       start: start or stop broadcasting.
**
** Returns          status.
**
*******************************************************************************/
tBTM_STATUS BTM_BleBroadcast(BOOLEAN start)
{
    tBTM_STATUS status = BTM_NO_RESOURCES;
    tBTM_LE_RANDOM_CB *p_addr_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;
    UINT8 evt_type = p_cb->scan_rsp ? BTM_BLE_DISCOVER_EVT : BTM_BLE_NON_CONNECT_EVT;

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

#ifdef  BTM_BLE_PC_ADV_TEST_MODE
    if (BTM_BLE_PC_ADV_TEST_MODE) {
        evt_type = p_cb->scan_rsp ? BTM_BLE_CONNECT_EVT : BTM_BLE_NON_CONNECT_EVT;
    }
#endif

    if (start && p_cb->adv_mode == BTM_BLE_ADV_DISABLE) {
        /* update adv params */
        if (!btsnd_hcic_ble_write_adv_params ((UINT16)(p_cb->adv_interval_min ? p_cb->adv_interval_min :
                                              BTM_BLE_GAP_ADV_INT),
                                              (UINT16)(p_cb->adv_interval_max ? p_cb->adv_interval_max :
                                                      BTM_BLE_GAP_ADV_INT),
                                              evt_type,
                                              p_addr_cb->own_addr_type,
                                              p_cb->direct_bda.type,
                                              p_cb->direct_bda.bda,
                                              p_cb->adv_chnl_map,
                                              p_cb->afp))

        {
            status = BTM_NO_RESOURCES;
        } else {
            p_cb->evt_type = evt_type;
        }

        status = btm_ble_start_adv ();
    } else if (!start) {
        status = btm_ble_stop_adv();
#if BLE_PRIVACY_SPT == TRUE
        btm_ble_disable_resolving_list(BTM_BLE_RL_ADV, TRUE);
#endif
    } else {
        status = BTM_WRONG_MODE;
        BTM_TRACE_ERROR("Can not %s Broadcast, device %s in Broadcast mode",
                        (start ? "Start" : "Stop"), (start ? "already" : "not"));
    }
    return status;
}

#if BLE_VND_INCLUDED == TRUE
/*******************************************************************************
**
** Function         btm_vsc_brcm_features_complete
**
** Description      Command Complete callback for HCI_BLE_VENDOR_CAP_OCF
**
** Returns          void
**
*******************************************************************************/
static void btm_ble_vendor_capability_vsc_cmpl_cback (tBTM_VSC_CMPL *p_vcs_cplt_params)
{
    UINT8 status = 0xFF;
    UINT8 *p;

    BTM_TRACE_DEBUG("%s", __func__);

    /* Check status of command complete event */
    if ((p_vcs_cplt_params->opcode == HCI_BLE_VENDOR_CAP_OCF) &&
            (p_vcs_cplt_params->param_len > 0)) {
        p = p_vcs_cplt_params->p_param_buf;
        STREAM_TO_UINT8(status, p);
    }

    if (status == HCI_SUCCESS) {
        STREAM_TO_UINT8(btm_cb.cmn_ble_vsc_cb.adv_inst_max, p);
        STREAM_TO_UINT8(btm_cb.cmn_ble_vsc_cb.rpa_offloading, p);
        STREAM_TO_UINT16(btm_cb.cmn_ble_vsc_cb.tot_scan_results_strg, p);
        STREAM_TO_UINT8(btm_cb.cmn_ble_vsc_cb.max_irk_list_sz, p);
        STREAM_TO_UINT8(btm_cb.cmn_ble_vsc_cb.filter_support, p);
        STREAM_TO_UINT8(btm_cb.cmn_ble_vsc_cb.max_filter, p);
        STREAM_TO_UINT8(btm_cb.cmn_ble_vsc_cb.energy_support, p);

        if (p_vcs_cplt_params->param_len > BTM_VSC_CHIP_CAPABILITY_RSP_LEN_L_RELEASE) {
            STREAM_TO_UINT16(btm_cb.cmn_ble_vsc_cb.version_supported, p);
        } else {
            btm_cb.cmn_ble_vsc_cb.version_supported = BTM_VSC_CHIP_CAPABILITY_L_VERSION;
        }

        if (btm_cb.cmn_ble_vsc_cb.version_supported >= BTM_VSC_CHIP_CAPABILITY_M_VERSION) {
            STREAM_TO_UINT16(btm_cb.cmn_ble_vsc_cb.total_trackable_advertisers, p);
            STREAM_TO_UINT16(btm_cb.cmn_ble_vsc_cb.extended_scan_support, p);
            STREAM_TO_UINT16(btm_cb.cmn_ble_vsc_cb.debug_logging_supported, p);
        }
        btm_cb.cmn_ble_vsc_cb.values_read = TRUE;
    }

    BTM_TRACE_DEBUG("%s: stat=%d, irk=%d, ADV ins:%d, rpa=%d, ener=%d, ext_scan=%d",
                    __func__, status, btm_cb.cmn_ble_vsc_cb.max_irk_list_sz,
                    btm_cb.cmn_ble_vsc_cb.adv_inst_max, btm_cb.cmn_ble_vsc_cb.rpa_offloading,
                    btm_cb.cmn_ble_vsc_cb.energy_support, btm_cb.cmn_ble_vsc_cb.extended_scan_support);

    if (BTM_BleMaxMultiAdvInstanceCount() > 0) {
        btm_ble_multi_adv_init();
    }

    if (btm_cb.cmn_ble_vsc_cb.max_filter > 0) {
        btm_ble_adv_filter_init();
    }

#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
    /* VS capability included and non-4.2 device */
    if (btm_cb.cmn_ble_vsc_cb.max_irk_list_sz > 0 &&
            controller_get_interface()->get_ble_resolving_list_max_size() == 0) {
        btm_ble_resolving_list_init(btm_cb.cmn_ble_vsc_cb.max_irk_list_sz);
    }
#endif

    if (btm_cb.cmn_ble_vsc_cb.tot_scan_results_strg > 0) {
        btm_ble_batchscan_init();
    }

    if (p_ctrl_le_feature_rd_cmpl_cback != NULL) {
        p_ctrl_le_feature_rd_cmpl_cback(status);
    }
}
#endif

/*******************************************************************************
**
** Function         BTM_BleGetVendorCapabilities
**
** Description      This function reads local LE features
**
** Parameters       p_cmn_vsc_cb : Locala LE capability structure
**
** Returns          void
**
*******************************************************************************/
extern void BTM_BleGetVendorCapabilities(tBTM_BLE_VSC_CB *p_cmn_vsc_cb)
{
    BTM_TRACE_DEBUG("BTM_BleGetVendorCapabilities");

    if (NULL != p_cmn_vsc_cb) {
        *p_cmn_vsc_cb = btm_cb.cmn_ble_vsc_cb;
    }
}

/******************************************************************************
**
** Function         BTM_BleReadControllerFeatures
**
** Description      Reads BLE specific controller features
**
** Parameters:      tBTM_BLE_CTRL_FEATURES_CBACK : Callback to notify when features are read
**
** Returns          void
**
*******************************************************************************/
extern void BTM_BleReadControllerFeatures(tBTM_BLE_CTRL_FEATURES_CBACK  *p_vsc_cback)
{
    if (TRUE == btm_cb.cmn_ble_vsc_cb.values_read) {
        return;
    }

#if BLE_VND_INCLUDED == TRUE
    BTM_TRACE_DEBUG("BTM_BleReadControllerFeatures");

    p_ctrl_le_feature_rd_cmpl_cback = p_vsc_cback;
    if ( BTM_VendorSpecificCommand (HCI_BLE_VENDOR_CAP_OCF,
                                    0,
                                    NULL,
                                    btm_ble_vendor_capability_vsc_cmpl_cback)
            != BTM_CMD_STARTED) {
        BTM_TRACE_ERROR("LE Get_Vendor Capabilities Command Failed.");
    }
#else
    UNUSED(p_vsc_cback);
#endif
    return ;
}

/*******************************************************************************
**
** Function         BTM_BleEnableMixedPrivacyMode
**
** Description      This function is called to enabled Mixed mode if privacy 1.2
**                  is applicable in controller.
**
** Parameters       mixed_on:  mixed mode to be used or not.
**
** Returns          void
**
*******************************************************************************/
void BTM_BleEnableMixedPrivacyMode(BOOLEAN mixed_on)
{

#if BLE_PRIVACY_SPT == TRUE
    btm_cb.ble_ctr_cb.mixed_mode = mixed_on;

    /* TODO: send VSC to enabled mixed mode */
#endif
}

/*******************************************************************************
**
** Function         BTM_BleConfigPrivacy
**
** Description      This function is called to enable or disable the privacy in
**                   LE channel of the local device.
**
** Parameters       privacy_mode:  privacy mode on or off.
**
** Returns          BOOLEAN privacy mode set success; otherwise failed.
**
*******************************************************************************/
BOOLEAN BTM_BleConfigPrivacy(BOOLEAN privacy_mode)
{
#if BLE_PRIVACY_SPT == TRUE
    tBTM_BLE_CB  *p_cb = &btm_cb.ble_ctr_cb;

    BTM_TRACE_EVENT ("%s\n", __func__);

    /* if LE is not supported, return error */
    if (!controller_get_interface()->supports_ble()) {
        return FALSE;
    }
#if (defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE && GATTS_INCLUDED == TRUE)
    uint8_t addr_resolution = 0;
#endif  /* defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE && GATTS_INCLUDED == TRUE */
    if (!privacy_mode) { /* if privacy disabled, always use public address */
        p_cb->addr_mgnt_cb.own_addr_type = BLE_ADDR_PUBLIC;
        p_cb->privacy_mode = BTM_PRIVACY_NONE;
    } else { /* privacy is turned on*/
        /* always set host random address, used when privacy 1.1 or priavcy 1.2 is disabled */
        p_cb->addr_mgnt_cb.own_addr_type = BLE_ADDR_RANDOM;
        btm_gen_resolvable_private_addr((void *)btm_gen_resolve_paddr_low);

        if (BTM_BleMaxMultiAdvInstanceCount() > 0) {
            btm_ble_multi_adv_enb_privacy(privacy_mode);
        }

        /* 4.2 controller only allow privacy 1.2 or mixed mode, resolvable private address in controller */
        if (controller_get_interface()->supports_ble_privacy()) {
#if (defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE && GATTS_INCLUDED == TRUE)
            addr_resolution = 1;
#endif  /* defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE && GATTS_INCLUDED == TRUE */
            /* check vendor specific capability */
            p_cb->privacy_mode = btm_cb.ble_ctr_cb.mixed_mode ? BTM_PRIVACY_MIXED : BTM_PRIVACY_1_2;
        } else { /* 4.1/4.0 controller */
            p_cb->privacy_mode = BTM_PRIVACY_1_1;
        }
    }

#if (defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE && GATTS_INCLUDED == TRUE)
    GAP_BleAttrDBUpdate (GATT_UUID_GAP_CENTRAL_ADDR_RESOL, (tGAP_BLE_ATTR_VALUE *)&addr_resolution);
#endif

    return TRUE;
#else
    return FALSE;
#endif
}

/*******************************************************************************
**
** Function          BTM_BleMaxMultiAdvInstanceCount
**
** Description        Returns max number of multi adv instances supported by controller
**
** Returns          Max multi adv instance count
**
*******************************************************************************/
extern UINT8  BTM_BleMaxMultiAdvInstanceCount(void)
{
    return btm_cb.cmn_ble_vsc_cb.adv_inst_max < BTM_BLE_MULTI_ADV_MAX ?
           btm_cb.cmn_ble_vsc_cb.adv_inst_max : BTM_BLE_MULTI_ADV_MAX;
}

#if BLE_PRIVACY_SPT == TRUE
/*******************************************************************************
**
** Function         btm_ble_resolve_random_addr_on_adv
**
** Description      resolve random address complete callback.
**
** Returns          void
**
*******************************************************************************/
static void btm_ble_resolve_random_addr_on_adv(void *p_rec, void *p)
{
    tBTM_SEC_DEV_REC    *match_rec = (tBTM_SEC_DEV_REC *) p_rec;
    UINT8       addr_type = BLE_ADDR_RANDOM;
    BD_ADDR     bda;
    UINT8       *pp = (UINT8 *)p + 1;
    UINT8           evt_type;

    BTM_TRACE_EVENT ("btm_ble_resolve_random_addr_on_adv ");

    STREAM_TO_UINT8    (evt_type, pp);
    STREAM_TO_UINT8    (addr_type, pp);
    STREAM_TO_BDADDR   (bda, pp);

    if (match_rec) {
        BTM_TRACE_DEBUG("Random match");
        match_rec->ble.active_addr_type = BTM_BLE_ADDR_RRA;
        memcpy(match_rec->ble.cur_rand_addr, bda, BD_ADDR_LEN);

        if (btm_ble_init_pseudo_addr(match_rec, bda)) {
            memcpy(bda, match_rec->bd_addr, BD_ADDR_LEN);
        } else {
            // Assign the original address to be the current report address
            memcpy(bda, match_rec->ble.pseudo_addr, BD_ADDR_LEN);
        }
    }

    btm_ble_process_adv_pkt_cont(bda, addr_type, evt_type, pp);

    return;
}
#endif

/*******************************************************************************
**
** Function         BTM_BleLocalPrivacyEnabled
**
** Description        Checks if local device supports private address
**
** Returns          Return TRUE if local privacy is enabled else FALSE
**
*******************************************************************************/
BOOLEAN BTM_BleLocalPrivacyEnabled(void)
{
#if BLE_PRIVACY_SPT == TRUE
    return (btm_cb.ble_ctr_cb.privacy_mode != BTM_PRIVACY_NONE);
#else
    return false;
#endif
}

/*******************************************************************************
**
** Function         BTM_BleSetBgConnType
**
** Description      This function is called to set BLE connectable mode for a
**                  peripheral device.
**
** Parameters       bg_conn_type: it can be auto connection, or selective connection.
**                  p_select_cback: callback function when selective connection procedure
**                              is being used.
**
** Returns          void
**
*******************************************************************************/
BOOLEAN BTM_BleSetBgConnType(tBTM_BLE_CONN_TYPE   bg_conn_type,
                             tBTM_BLE_SEL_CBACK   *p_select_cback)
{
    BOOLEAN started = TRUE;

    BTM_TRACE_EVENT ("BTM_BleSetBgConnType ");
    if (!controller_get_interface()->supports_ble()) {
        return FALSE;
    }

    if (btm_cb.ble_ctr_cb.bg_conn_type != bg_conn_type) {
        switch (bg_conn_type) {
        case BTM_BLE_CONN_AUTO:
            btm_ble_start_auto_conn(TRUE);
            break;

        case BTM_BLE_CONN_SELECTIVE:
            if (btm_cb.ble_ctr_cb.bg_conn_type == BTM_BLE_CONN_AUTO) {
                btm_ble_start_auto_conn(FALSE);
            }
            btm_ble_start_select_conn(TRUE, p_select_cback);
            break;

        case BTM_BLE_CONN_NONE:
            if (btm_cb.ble_ctr_cb.bg_conn_type == BTM_BLE_CONN_AUTO) {
                btm_ble_start_auto_conn(FALSE);
            } else if (btm_cb.ble_ctr_cb.bg_conn_type == BTM_BLE_CONN_SELECTIVE) {
                btm_ble_start_select_conn(FALSE, NULL);
            }
            started = TRUE;
            break;

        default:
            BTM_TRACE_ERROR("invalid bg connection type : %d ", bg_conn_type);
            started = FALSE;
            break;
        }

        if (started) {
            btm_cb.ble_ctr_cb.bg_conn_type = bg_conn_type;
        }
    }
    return started;
}

/*******************************************************************************
**
** Function         BTM_BleClearBgConnDev
**
** Description      This function is called to clear the whitelist,
**                  end any pending whitelist connections,
*                   and reset the local bg device list.
**
** Parameters       void
**
** Returns          void
**
*******************************************************************************/
void BTM_BleClearBgConnDev(void)
{
    btm_ble_start_auto_conn(FALSE);
    btm_ble_clear_white_list();
    gatt_reset_bgdev_list();
}

/*******************************************************************************
**
** Function         BTM_BleUpdateBgConnDev
**
** Description      This function is called to add or remove a device into/from
**                  background connection procedure. The background connection
*                   procedure is decided by the background connection type, it can be
*                   auto connection, or selective connection.
**
** Parameters       add_remove: TRUE to add; FALSE to remove.
**                  remote_bda: device address to add/remove.
**
** Returns          void
**
*******************************************************************************/
BOOLEAN BTM_BleUpdateBgConnDev(BOOLEAN add_remove, BD_ADDR   remote_bda)
{
    BTM_TRACE_EVENT("%s() add=%d", __func__, add_remove);
    return btm_update_dev_to_white_list(add_remove, remote_bda);
}

/*******************************************************************************
**
** Function         BTM_BleSetConnectableMode
**
** Description      This function is called to set BLE connectable mode for a
**                  peripheral device.
**
** Parameters       conn_mode:  directed connectable mode, or non-directed.It can
**                              be BTM_BLE_CONNECT_EVT, BTM_BLE_CONNECT_DIR_EVT or
**                              BTM_BLE_CONNECT_LO_DUTY_DIR_EVT
**
** Returns          BTM_ILLEGAL_VALUE if controller does not support BLE.
**                  BTM_SUCCESS is status set successfully; otherwise failure.
**
*******************************************************************************/
tBTM_STATUS BTM_BleSetConnectableMode(tBTM_BLE_CONN_MODE connectable_mode)
{
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;

    BTM_TRACE_EVENT ("%s connectable_mode = %d ", __func__, connectable_mode);
    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

    p_cb->directed_conn = connectable_mode;
    return btm_ble_set_connectability( p_cb->connectable_mode);
}

/*******************************************************************************
**
** Function         btm_set_conn_mode_adv_init_addr
**
** Description      set initator address type and local address type based on adv
**                  mode.
**
**
*******************************************************************************/
static UINT8 btm_set_conn_mode_adv_init_addr(tBTM_BLE_INQ_CB *p_cb,
        BD_ADDR_PTR p_peer_addr_ptr,
        tBLE_ADDR_TYPE *p_peer_addr_type,
        tBLE_ADDR_TYPE *p_own_addr_type)
{
    UINT8 evt_type;
#if BLE_PRIVACY_SPT == TRUE
    UINT8 i = BTM_SEC_MAX_DEVICE_RECORDS;
    tBTM_SEC_DEV_REC    *p_dev_rec;
#endif  ///BLE_PRIVACY_SPT == TRUE
    evt_type = (p_cb->connectable_mode == BTM_BLE_NON_CONNECTABLE) ? \
               ((p_cb->scan_rsp) ? BTM_BLE_DISCOVER_EVT : BTM_BLE_NON_CONNECT_EVT )\
               : BTM_BLE_CONNECT_EVT;

    if (evt_type == BTM_BLE_CONNECT_EVT) {
        evt_type = p_cb->directed_conn;

        if ( p_cb->directed_conn == BTM_BLE_CONNECT_DIR_EVT ||
                p_cb->directed_conn == BTM_BLE_CONNECT_LO_DUTY_DIR_EVT) {

#if BLE_PRIVACY_SPT == TRUE
            /* for privacy 1.2, convert peer address as static, own address set as ID addr */
            if (btm_cb.ble_ctr_cb.privacy_mode ==  BTM_PRIVACY_1_2 ||
                    btm_cb.ble_ctr_cb.privacy_mode ==  BTM_PRIVACY_MIXED) {
                /* only do so for bonded device */
                if ((p_dev_rec = btm_find_or_alloc_dev (p_cb->direct_bda.bda)) != NULL &&
                        p_dev_rec->ble.in_controller_list & BTM_RESOLVING_LIST_BIT) {
                    btm_ble_enable_resolving_list(BTM_BLE_RL_ADV);
                    memcpy(p_peer_addr_ptr, p_dev_rec->ble.static_addr, BD_ADDR_LEN);
                    *p_peer_addr_type = p_dev_rec->ble.static_addr_type;
                    *p_own_addr_type = BLE_ADDR_RANDOM_ID;
                    return evt_type;
                }
                /* otherwise fall though as normal directed adv */
                else {
                    btm_ble_disable_resolving_list(BTM_BLE_RL_ADV, TRUE);
                }
            }
#endif
            /* direct adv mode does not have privacy, if privacy is not enabled  */
            *p_peer_addr_type  = p_cb->direct_bda.type;
            memcpy(p_peer_addr_ptr, p_cb->direct_bda.bda, BD_ADDR_LEN);
            return evt_type;
        }
    }

    /* undirect adv mode or non-connectable mode*/
#if BLE_PRIVACY_SPT == TRUE
    /* when privacy 1.2 privacy only mode is used, or mixed mode */
    if ((btm_cb.ble_ctr_cb.privacy_mode ==  BTM_PRIVACY_1_2 && p_cb->afp != AP_SCAN_CONN_ALL) ||
            btm_cb.ble_ctr_cb.privacy_mode ==  BTM_PRIVACY_MIXED) {
        /* if enhanced privacy is required, set Identity address and matching IRK peer */
        for (i = 0; i < BTM_SEC_MAX_DEVICE_RECORDS; i ++) {
            if ((btm_cb.sec_dev_rec[i].sec_flags & BTM_SEC_IN_USE) != 0 &&
                    (btm_cb.sec_dev_rec[i].ble.in_controller_list & BTM_RESOLVING_LIST_BIT) != 0) {
                memcpy(p_peer_addr_ptr, btm_cb.sec_dev_rec[i].ble.static_addr, BD_ADDR_LEN);
                *p_peer_addr_type = btm_cb.sec_dev_rec[i].ble.static_addr_type;
                break;
            }
        }

        if (i != BTM_SEC_MAX_DEVICE_RECORDS) {
            *p_own_addr_type = BLE_ADDR_RANDOM_ID;
        } else
            /* resolving list is empty, not enabled */
        {
            *p_own_addr_type = BLE_ADDR_RANDOM;
        }
    }
    /* privacy 1.1, or privacy 1.2, general discoverable/connectable mode, disable privacy in */
    /* controller fall back to host based privacy */
    else if (btm_cb.ble_ctr_cb.privacy_mode !=  BTM_PRIVACY_NONE) {
        *p_own_addr_type = BLE_ADDR_RANDOM;
    }
#endif

    /* if no privacy,do not set any peer address,*/
    /* local address type go by global privacy setting */
    return evt_type;
}

/*******************************************************************************
**
** Function         BTM_BleSetAdvParams
**
** Description      This function is called to set advertising parameters.
**
** Parameters       adv_int_min: minimum advertising interval
**                  adv_int_max: maximum advertising interval
**                  p_dir_bda: connectable direct initiator's LE device address
**                  chnl_map: advertising channel map.
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS BTM_BleSetAdvParams(UINT16 adv_int_min, UINT16 adv_int_max,
                                tBLE_BD_ADDR *p_dir_bda,
                                tBTM_BLE_ADV_CHNL_MAP chnl_map)
{
    tBTM_LE_RANDOM_CB *p_addr_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;
    tBTM_STATUS status = BTM_SUCCESS;
    BD_ADDR     p_addr_ptr =  {0};
    tBLE_ADDR_TYPE   init_addr_type = BLE_ADDR_PUBLIC;
    tBLE_ADDR_TYPE   own_addr_type = p_addr_cb->own_addr_type;
    UINT8            adv_mode = p_cb->adv_mode;

    BTM_TRACE_EVENT ("BTM_BleSetAdvParams");

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

    if (!BTM_BLE_ISVALID_PARAM(adv_int_min, BTM_BLE_ADV_INT_MIN, BTM_BLE_ADV_INT_MAX) ||
            !BTM_BLE_ISVALID_PARAM(adv_int_max, BTM_BLE_ADV_INT_MIN, BTM_BLE_ADV_INT_MAX)) {
        return BTM_ILLEGAL_VALUE;
    }

    p_cb->adv_interval_min = adv_int_min;
    p_cb->adv_interval_max = adv_int_max;
    p_cb->adv_chnl_map = chnl_map;

    if (p_dir_bda) {
        memcpy(&p_cb->direct_bda, p_dir_bda, sizeof(tBLE_BD_ADDR));
    }

    BTM_TRACE_EVENT ("update params for an active adv\n");

    btm_ble_stop_adv();

    p_cb->evt_type = btm_set_conn_mode_adv_init_addr(p_cb, p_addr_ptr, &init_addr_type,
                     &own_addr_type);

    /* update adv params */
    btsnd_hcic_ble_write_adv_params (p_cb->adv_interval_min,
                                     p_cb->adv_interval_max,
                                     p_cb->evt_type,
                                     own_addr_type,
                                     init_addr_type,
                                     p_addr_ptr,
                                     p_cb->adv_chnl_map,
                                     p_cb->afp);

    if (adv_mode == BTM_BLE_ADV_ENABLE) {
        btm_ble_start_adv();
    }

    return status;
}


/*******************************************************************************
**
** Function         BTM_BleSetAdvParamsStartAdv
**
** Description      This function is called to set all of the advertising parameters.
**
** Parameters:       None.
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS BTM_BleSetAdvParamsStartAdv(UINT16 adv_int_min, UINT16 adv_int_max, UINT8 adv_type,
                                        tBLE_ADDR_TYPE own_bda_type, tBLE_BD_ADDR *p_dir_bda,
                                        tBTM_BLE_ADV_CHNL_MAP chnl_map, tBTM_BLE_AFP afp)
{
    tBTM_LE_RANDOM_CB *p_addr_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;

    BTM_TRACE_EVENT ("BTM_BleSetAdvParamsStartAdv\n");

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

    if (!BTM_BLE_ISVALID_PARAM(adv_int_min, BTM_BLE_ADV_INT_MIN, BTM_BLE_ADV_INT_MAX) ||
            !BTM_BLE_ISVALID_PARAM(adv_int_max, BTM_BLE_ADV_INT_MIN, BTM_BLE_ADV_INT_MAX)) {
        return BTM_ILLEGAL_VALUE;
    }

    if(adv_type == BTM_BLE_CONNECT_DIR_EVT){
        btm_ble_set_topology_mask(BTM_BLE_STATE_HI_DUTY_DIR_ADV_BIT);
    }else if(adv_type == BTM_BLE_CONNECT_LO_DUTY_DIR_EVT){
        btm_ble_set_topology_mask(BTM_BLE_STATE_LO_DUTY_DIR_ADV_BIT);
    }else if(adv_type == BTM_BLE_NON_CONNECT_EVT){
        btm_ble_set_topology_mask(BTM_BLE_STATE_NON_CONN_ADV_BIT);
    }

    p_cb->adv_interval_min = adv_int_min;
    p_cb->adv_interval_max = adv_int_max;
    p_cb->adv_chnl_map = chnl_map;
    p_addr_cb->own_addr_type = own_bda_type;
    p_cb->evt_type = adv_type;
    p_cb->afp = afp;

    if (p_dir_bda) {
        memcpy(&p_cb->direct_bda, p_dir_bda, sizeof(tBLE_BD_ADDR));
    }

    BTM_TRACE_EVENT ("update params for an active adv\n");

    btm_ble_stop_adv();

    /* update adv params */
    btsnd_hcic_ble_write_adv_params (adv_int_min,
                                     adv_int_max,
                                     adv_type,
                                     own_bda_type,
                                     p_dir_bda->type,
                                     p_dir_bda->bda,
                                     chnl_map,
                                     p_cb->afp);

    return btm_ble_start_adv();
}




/*******************************************************************************
**
** Function         BTM_BleReadAdvParams
**
** Description      This function is called to set advertising parameters.
**
** Parameters       adv_int_min: minimum advertising interval
**                  adv_int_max: maximum advertising interval
**                  p_dir_bda: connectable direct initiator's LE device address
**                  chnl_map: advertising channel map.
**
** Returns          void
**
*******************************************************************************/
void BTM_BleReadAdvParams (UINT16 *adv_int_min, UINT16 *adv_int_max,
                           tBLE_BD_ADDR *p_dir_bda, tBTM_BLE_ADV_CHNL_MAP *p_chnl_map)
{
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;

    BTM_TRACE_EVENT ("BTM_BleReadAdvParams ");
    if (!controller_get_interface()->supports_ble()) {
        return ;
    }

    *adv_int_min = p_cb->adv_interval_min;
    *adv_int_max = p_cb->adv_interval_max;
    *p_chnl_map = p_cb->adv_chnl_map;

    if (p_dir_bda != NULL) {
        memcpy(p_dir_bda, &p_cb->direct_bda, sizeof(tBLE_BD_ADDR));
    }
}

/*******************************************************************************
**
** Function         BTM_BleSetScanParams
**
** Description      This function is called to set scan parameters.
**
** Parameters       client_if - Client IF
**                  scan_interval - Scan interval
**                  scan_window - Scan window
**                  scan_mode -    Scan mode
**                  scan_setup_status_cback - Scan param setup status callback
**
** Returns          void
**
*******************************************************************************/
void BTM_BleSetScanParams(tGATT_IF client_if, UINT32 scan_interval, UINT32 scan_window,
                          tBLE_SCAN_MODE scan_mode,
                          tBLE_SCAN_PARAM_SETUP_CBACK scan_setup_status_cback)
{
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;
    UINT32 max_scan_interval;
    UINT32 max_scan_window;

    BTM_TRACE_EVENT ("%s\n", __func__);
    if (!controller_get_interface()->supports_ble()) {
        return;
    }

    /* If not supporting extended scan support, use the older range for checking */
    if (btm_cb.cmn_ble_vsc_cb.extended_scan_support == 0) {
        max_scan_interval = BTM_BLE_SCAN_INT_MAX;
        max_scan_window = BTM_BLE_SCAN_WIN_MAX;
    } else {
        /* If supporting extended scan support, use the new extended range for checking */
        max_scan_interval = BTM_BLE_EXT_SCAN_INT_MAX;
        max_scan_window = BTM_BLE_EXT_SCAN_WIN_MAX;
    }

    if (BTM_BLE_ISVALID_PARAM(scan_interval, BTM_BLE_SCAN_INT_MIN, max_scan_interval) &&
            BTM_BLE_ISVALID_PARAM(scan_window, BTM_BLE_SCAN_WIN_MIN, max_scan_window) &&
            (scan_mode == BTM_BLE_SCAN_MODE_ACTI || scan_mode == BTM_BLE_SCAN_MODE_PASS)) {
        p_cb->scan_type = scan_mode;
        p_cb->scan_interval = scan_interval;
        p_cb->scan_window = scan_window;

        if (scan_setup_status_cback != NULL) {
            scan_setup_status_cback(client_if, BTM_SUCCESS);
        }
    } else {
        if (scan_setup_status_cback != NULL) {
            scan_setup_status_cback(client_if, BTM_ILLEGAL_VALUE);
        }

        BTM_TRACE_ERROR("Illegal params: scan_interval = %d scan_window = %d\n",
                        scan_interval, scan_window);
    }

}

void BTM_BleSetScanFilterParams(tGATT_IF client_if, UINT32 scan_interval, UINT32 scan_window,
                                tBLE_SCAN_MODE scan_mode, UINT8 addr_type_own, tBTM_BLE_SFP scan_filter_policy,
                                tBLE_SCAN_PARAM_SETUP_CBACK scan_setup_status_cback)
{
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;
    UINT32 max_scan_interval;
    UINT32 max_scan_window;

    BTM_TRACE_EVENT ("%s\n", __func__);
    if (!controller_get_interface()->supports_ble()) {
        return;
    }

    /* If not supporting extended scan support, use the older range for checking */
    if (btm_cb.cmn_ble_vsc_cb.extended_scan_support == 0) {
        max_scan_interval = BTM_BLE_SCAN_INT_MAX;
        max_scan_window = BTM_BLE_SCAN_WIN_MAX;
    } else {
        /* If supporting extended scan support, use the new extended range for checking */
        max_scan_interval = BTM_BLE_EXT_SCAN_INT_MAX;
        max_scan_window = BTM_BLE_EXT_SCAN_WIN_MAX;
    }

    if (BTM_BLE_ISVALID_PARAM(scan_interval, BTM_BLE_SCAN_INT_MIN, max_scan_interval) &&
            BTM_BLE_ISVALID_PARAM(scan_window, BTM_BLE_SCAN_WIN_MIN, max_scan_window) &&
            (scan_mode == BTM_BLE_SCAN_MODE_ACTI || scan_mode == BTM_BLE_SCAN_MODE_PASS)) {
        p_cb->scan_type = scan_mode;
        p_cb->scan_interval = scan_interval;
        p_cb->scan_window = scan_window;
        p_cb->sfp = scan_filter_policy;

        btsnd_hcic_ble_set_scan_params(p_cb->scan_type, (UINT16)scan_interval,
                                       (UINT16)scan_window,
                                       addr_type_own,
                                       scan_filter_policy);

        if (scan_setup_status_cback != NULL) {
            scan_setup_status_cback(client_if, BTM_SUCCESS);
        }
    } else {
        if (scan_setup_status_cback != NULL) {
            scan_setup_status_cback(client_if, BTM_ILLEGAL_VALUE);
        }

        BTM_TRACE_ERROR("Illegal params: scan_interval = %d scan_window = %d\n",
                        scan_interval, scan_window);
    }
}


/*******************************************************************************
**
** Function         BTM_BleWriteScanRsp
**
** Description      This function is called to write LE scan response.
**
** Parameters:      p_scan_rsp: scan response information.
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS BTM_BleWriteScanRsp(tBTM_BLE_AD_MASK data_mask, tBTM_BLE_ADV_DATA *p_data)
{
    tBTM_STATUS     status = BTM_NO_RESOURCES;
    UINT8   rsp_data[BTM_BLE_AD_DATA_LEN],
            *p = rsp_data;

    BTM_TRACE_EVENT (" BTM_BleWriteScanRsp");

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

    memset(rsp_data, 0, BTM_BLE_AD_DATA_LEN);
    btm_ble_build_adv_data(&data_mask, &p, p_data);

    if (btsnd_hcic_ble_set_scan_rsp_data((UINT8)(p - rsp_data), rsp_data)) {
        status = BTM_SUCCESS;

        if (data_mask != 0) {
            btm_cb.ble_ctr_cb.inq_var.scan_rsp = TRUE;
        } else {
            btm_cb.ble_ctr_cb.inq_var.scan_rsp = FALSE;
        }
    } else {
        status = BTM_ILLEGAL_VALUE;
    }

    return status;
}

/*******************************************************************************
**
** Function         BTM_BleWriteScanRspRaw
**
** Description      This function is called to write raw scan response data
**
** Parameters:      None.
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS BTM_BleWriteScanRspRaw(UINT8 *p_raw_scan_rsp, UINT32 raw_scan_rsp_len)
{
    if (btsnd_hcic_ble_set_scan_rsp_data((UINT8)raw_scan_rsp_len, p_raw_scan_rsp)) {
        return BTM_SUCCESS;
    } else {
        return BTM_NO_RESOURCES;
    }
}

/*******************************************************************************
**
** Function         BTM_BleWriteAdvData
**
** Description      This function is called to write advertising data.
**
** Parameters:       None.
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS BTM_BleWriteAdvData(tBTM_BLE_AD_MASK data_mask, tBTM_BLE_ADV_DATA *p_data)
{
    tBTM_BLE_LOCAL_ADV_DATA *p_cb_data = &btm_cb.ble_ctr_cb.inq_var.adv_data;
    UINT8  *p;
    tBTM_BLE_AD_MASK   mask = data_mask;

    BTM_TRACE_EVENT ("BTM_BleWriteAdvData ");

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

    memset(p_cb_data, 0, sizeof(tBTM_BLE_LOCAL_ADV_DATA));
    p = p_cb_data->ad_data;
    p_cb_data->data_mask = data_mask;

    p_cb_data->p_flags = btm_ble_build_adv_data(&mask, &p, p_data);

    p_cb_data->p_pad = p;

    if (mask != 0) {
        BTM_TRACE_DEBUG("Partial data write into ADV");
    }

    p_cb_data->data_mask &= ~mask;

    if (btsnd_hcic_ble_set_adv_data((UINT8)(p_cb_data->p_pad - p_cb_data->ad_data),
                                    p_cb_data->ad_data)) {
        return BTM_SUCCESS;
    } else {
        return BTM_NO_RESOURCES;
    }

}


/*******************************************************************************
**
** Function         BTM_BleWriteAdvDataRaw
**
** Description      This function is called to write raw advertising data.
**
** Parameters:       None.
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS BTM_BleWriteAdvDataRaw(UINT8 *p_raw_adv, UINT32 raw_adv_len)
{
    if (btsnd_hcic_ble_set_adv_data((UINT8)raw_adv_len, p_raw_adv)) {
        return BTM_SUCCESS;
    } else {
        return BTM_NO_RESOURCES;
    }
}


/*******************************************************************************
**
** Function         BTM_BleSetRandAddress
**
** Description      This function is called to set the LE random address.
**
** Parameters:       None.
**
** Returns          void
**
*******************************************************************************/
BOOLEAN BTM_BleSetRandAddress(BD_ADDR rand_addr)
{
    BOOLEAN set_flag = false;

	if (rand_addr == NULL)
		return set_flag;

    //send the set random address to the controller
    set_flag = btsnd_hcic_ble_set_random_addr(rand_addr);
    return set_flag;
}

/*******************************************************************************
**
** Function         BTM_CheckAdvData
**
** Description      This function is called to get ADV data for a specific type.
**
** Parameters       p_adv - pointer of ADV data
**                  type   - finding ADV data type
**                  p_length - return the length of ADV data not including type
**
** Returns          pointer of ADV data
**
*******************************************************************************/
UINT8 *BTM_CheckAdvData( UINT8 *p_adv, UINT8 type, UINT8 *p_length)
{
    UINT8 *p = p_adv;
    UINT8 length;
    UINT8 adv_type;
    BTM_TRACE_API("BTM_CheckAdvData type=0x%02X", type);

    STREAM_TO_UINT8(length, p);

    while ( length && (p - p_adv <= BTM_BLE_CACHE_ADV_DATA_MAX)) {
        STREAM_TO_UINT8(adv_type, p);

        if ( adv_type == type ) {
            /* length doesn't include itself */
            *p_length = length - 1; /* minus the length of type */
            return p;
        }
        p += length - 1; /* skip the length of data */
        STREAM_TO_UINT8(length, p);
    }

    *p_length = 0;
    return NULL;
}

/*******************************************************************************
**
** Function         BTM__BLEReadDiscoverability
**
** Description      This function is called to read the current LE discoverability
**                  mode of the device.
**
** Returns          BTM_BLE_NON_DISCOVERABLE ,BTM_BLE_LIMITED_DISCOVERABLE or
**                     BTM_BLE_GENRAL_DISCOVERABLE
**
*******************************************************************************/
UINT16 BTM_BleReadDiscoverability()
{
    BTM_TRACE_API("%s\n", __FUNCTION__);

    return (btm_cb.ble_ctr_cb.inq_var.discoverable_mode);
}

/*******************************************************************************
**
** Function         BTM__BLEReadConnectability
**
** Description      This function is called to read the current LE connectibility
**                  mode of the device.
**
** Returns          BTM_BLE_NON_CONNECTABLE or BTM_BLE_CONNECTABLE
**
*******************************************************************************/
UINT16 BTM_BleReadConnectability()
{
    BTM_TRACE_API ("%s\n", __FUNCTION__);

    return (btm_cb.ble_ctr_cb.inq_var.connectable_mode);
}

void BTM_Recovery_Pre_State(void)
{
    tBTM_BLE_INQ_CB *ble_inq_cb = &btm_cb.ble_ctr_cb.inq_var;

    if (ble_inq_cb->state == BTM_BLE_ADVERTISING) {
        btm_ble_stop_adv();
        btm_ble_start_adv();
    } else if (ble_inq_cb->state == BTM_BLE_SCANNING) {
        btm_ble_start_scan();
    }

    return;
}

/*******************************************************************************
**
** Function         btm_ble_build_adv_data
**
** Description      This function is called build the adv data and rsp data.
*******************************************************************************/
UINT8 *btm_ble_build_adv_data(tBTM_BLE_AD_MASK *p_data_mask, UINT8 **p_dst,
                              tBTM_BLE_ADV_DATA *p_data)
{
    UINT32 data_mask = *p_data_mask;
    UINT8   *p = *p_dst,
             *p_flag = NULL;
    UINT16  len = BTM_BLE_AD_DATA_LEN, cp_len = 0;
    UINT8   i = 0;
    tBTM_BLE_PROP_ELEM      *p_elem;

    BTM_TRACE_EVENT (" btm_ble_build_adv_data");

    /* build the adv data structure and build the data string */
    if (data_mask) {
        /* flags */
        if (data_mask & BTM_BLE_AD_BIT_FLAGS) {
            *p++ = MIN_ADV_LENGTH;
            *p++ = BTM_BLE_AD_TYPE_FLAG;
            p_flag = p;
            if (p_data) {
                *p++ = p_data->flag;
            } else {
                *p++ = 0;
            }

            len -= 3;

            data_mask &= ~BTM_BLE_AD_BIT_FLAGS;
        }
        /* appearance data */
        if (len > 3 && data_mask & BTM_BLE_AD_BIT_APPEARANCE) {
            *p++ = 3; /* length */
            *p++ = BTM_BLE_AD_TYPE_APPEARANCE;
            UINT16_TO_STREAM(p, p_data->appearance);
            len -= 4;

            data_mask &= ~BTM_BLE_AD_BIT_APPEARANCE;
        }
        /* device name */
#if BTM_MAX_LOC_BD_NAME_LEN > 0
        if (len > MIN_ADV_LENGTH && data_mask & BTM_BLE_AD_BIT_DEV_NAME) {
            if (strlen(btm_cb.cfg.bd_name) > (UINT16)(len - MIN_ADV_LENGTH)) {
                *p++ = len - MIN_ADV_LENGTH + 1;
                *p++ = BTM_BLE_AD_TYPE_NAME_SHORT;
                ARRAY_TO_STREAM(p, btm_cb.cfg.bd_name, len - MIN_ADV_LENGTH);
            } else {
                cp_len = (UINT16)strlen(btm_cb.cfg.bd_name);
                *p++ = cp_len + 1;
                *p++ = BTM_BLE_AD_TYPE_NAME_CMPL;
                ARRAY_TO_STREAM(p, btm_cb.cfg.bd_name, cp_len);
            }
            len -= (cp_len + MIN_ADV_LENGTH);
            data_mask &= ~BTM_BLE_AD_BIT_DEV_NAME;
        }
#endif
        /* manufacturer data */
        if (len > MIN_ADV_LENGTH && data_mask & BTM_BLE_AD_BIT_MANU &&
                p_data && p_data->p_manu &&
                p_data->p_manu->len != 0 && p_data->p_manu->p_val) {
            if (p_data->p_manu->len > (len - MIN_ADV_LENGTH)) {
                cp_len = len - MIN_ADV_LENGTH;
            } else {
                cp_len = p_data->p_manu->len;
            }
            LOG_DEBUG("cp_len = %d\n,p_data->p_manu->len=%d\n", cp_len, p_data->p_manu->len);
            for (int i = 0; i < p_data->p_manu->len; i++) {
                LOG_DEBUG("p_data->p_manu->p_val[%d] = %x\n", i, p_data->p_manu->p_val[i]);
            }
            *p++ = cp_len + 1;
            *p++ = BTM_BLE_AD_TYPE_MANU;
            ARRAY_TO_STREAM(p, p_data->p_manu->p_val, cp_len);
            LOG_DEBUG("p_addr = %p\n,p_data->p_manu->p_val = %p\n", p, p_data->p_manu->p_val);
            len -= (cp_len + MIN_ADV_LENGTH);
            data_mask &= ~BTM_BLE_AD_BIT_MANU;
        }
        /* TX power */
        if (len > MIN_ADV_LENGTH && data_mask & BTM_BLE_AD_BIT_TX_PWR) {
            *p++ = MIN_ADV_LENGTH;
            *p++ = BTM_BLE_AD_TYPE_TX_PWR;
            if (p_data->tx_power > BTM_BLE_ADV_TX_POWER_MAX) {
                p_data->tx_power = BTM_BLE_ADV_TX_POWER_MAX;
            }
            *p++ = btm_ble_map_adv_tx_power(p_data->tx_power);
            len -= 3;
            data_mask &= ~BTM_BLE_AD_BIT_TX_PWR;
        }
        /* 16 bits services */
        if (len > MIN_ADV_LENGTH && data_mask & BTM_BLE_AD_BIT_SERVICE &&
                p_data && p_data->p_services &&
                p_data->p_services->num_service != 0 &&
                p_data->p_services->p_uuid) {
            if (p_data->p_services->num_service * LEN_UUID_16 > (len - MIN_ADV_LENGTH)) {
                cp_len = (len - MIN_ADV_LENGTH) / LEN_UUID_16;
                *p ++ = 1 + cp_len * LEN_UUID_16;
                *p++ = BTM_BLE_AD_TYPE_16SRV_PART;
            } else {
                cp_len = p_data->p_services->num_service;
                *p++ = 1 + cp_len * LEN_UUID_16;
                *p++ = BTM_BLE_AD_TYPE_16SRV_CMPL;
            }
            for (i = 0; i < cp_len; i ++) {
                UINT16_TO_STREAM(p, *(p_data->p_services->p_uuid + i));
            }

            len -= (cp_len * MIN_ADV_LENGTH + MIN_ADV_LENGTH);
            data_mask &= ~BTM_BLE_AD_BIT_SERVICE;
        }
        /* 32 bits service uuid */
        if (len > MIN_ADV_LENGTH && data_mask & BTM_BLE_AD_BIT_SERVICE_32 &&
                p_data && p_data->p_service_32b &&
                p_data->p_service_32b->num_service != 0 &&
                p_data->p_service_32b->p_uuid) {
            if ((p_data->p_service_32b->num_service * LEN_UUID_32) > (len - MIN_ADV_LENGTH)) {
                cp_len = (len - MIN_ADV_LENGTH) / LEN_UUID_32;
                *p ++ = 1 + cp_len * LEN_UUID_32;
                *p++ = BTM_BLE_AD_TYPE_32SRV_PART;
            } else {
                cp_len = p_data->p_service_32b->num_service;
                *p++ = 1 + cp_len * LEN_UUID_32;
                *p++ = BTM_BLE_AD_TYPE_32SRV_CMPL;
            }
            for (i = 0; i < cp_len; i ++) {
                UINT32_TO_STREAM(p, *(p_data->p_service_32b->p_uuid + i));
            }

            len -= (cp_len * LEN_UUID_32 + MIN_ADV_LENGTH);
            data_mask &= ~BTM_BLE_AD_BIT_SERVICE_32;
        }
        /* 128 bits services */
        if (len >= (MAX_UUID_SIZE + 2) && data_mask & BTM_BLE_AD_BIT_SERVICE_128 &&
                p_data && p_data->p_services_128b) {
            *p ++ = 1 + MAX_UUID_SIZE;
            if (!p_data->p_services_128b->list_cmpl) {
                *p++ = BTM_BLE_AD_TYPE_128SRV_PART;
            } else {
                *p++ = BTM_BLE_AD_TYPE_128SRV_CMPL;
            }

            ARRAY_TO_STREAM(p, p_data->p_services_128b->uuid128, MAX_UUID_SIZE);

            len -= (MAX_UUID_SIZE + MIN_ADV_LENGTH);
            data_mask &= ~BTM_BLE_AD_BIT_SERVICE_128;
        }
        /* 32 bits Service Solicitation UUIDs */
        if (len > MIN_ADV_LENGTH && data_mask & BTM_BLE_AD_BIT_SERVICE_32SOL &&
                p_data && p_data->p_sol_service_32b &&
                p_data->p_sol_service_32b->num_service != 0 &&
                p_data->p_sol_service_32b->p_uuid) {
            if ((p_data->p_sol_service_32b->num_service * LEN_UUID_32) > (len - MIN_ADV_LENGTH)) {
                cp_len = (len - MIN_ADV_LENGTH) / LEN_UUID_32;
                *p ++ = 1 + cp_len * LEN_UUID_32;
            } else {
                cp_len = p_data->p_sol_service_32b->num_service;
                *p++ = 1 + cp_len * LEN_UUID_32;
            }

            *p++ = BTM_BLE_AD_TYPE_32SOL_SRV_UUID;
            for (i = 0; i < cp_len; i ++) {
                UINT32_TO_STREAM(p, *(p_data->p_sol_service_32b->p_uuid + i));
            }

            len -= (cp_len * LEN_UUID_32 + MIN_ADV_LENGTH);
            data_mask &= ~BTM_BLE_AD_BIT_SERVICE_32SOL;
        }
        /* 128 bits Solicitation services UUID */
        if (len >= (MAX_UUID_SIZE + MIN_ADV_LENGTH) && data_mask & BTM_BLE_AD_BIT_SERVICE_128SOL &&
                p_data && p_data->p_sol_service_128b) {
            *p ++ = 1 + MAX_UUID_SIZE;
            *p++ = BTM_BLE_AD_TYPE_128SOL_SRV_UUID;
            ARRAY_TO_STREAM(p, p_data->p_sol_service_128b->uuid128, MAX_UUID_SIZE);
            len -= (MAX_UUID_SIZE + MIN_ADV_LENGTH);
            data_mask &= ~BTM_BLE_AD_BIT_SERVICE_128SOL;
        }
        /* 16bits/32bits/128bits Service Data */
        if (len > MIN_ADV_LENGTH && data_mask & BTM_BLE_AD_BIT_SERVICE_DATA &&
                p_data && p_data->p_service_data->len != 0 && p_data->p_service_data->p_val) {
            if (len  > (p_data->p_service_data->service_uuid.len + MIN_ADV_LENGTH)) {
                if (p_data->p_service_data->len > (len - MIN_ADV_LENGTH)) {
                    cp_len = len - MIN_ADV_LENGTH - p_data->p_service_data->service_uuid.len;
                } else {
                    cp_len = p_data->p_service_data->len;
                }

                *p++ = cp_len + 1 + p_data->p_service_data->service_uuid.len;
                if (p_data->p_service_data->service_uuid.len == LEN_UUID_16) {
                    *p++ = BTM_BLE_AD_TYPE_SERVICE_DATA;
                    UINT16_TO_STREAM(p, p_data->p_service_data->service_uuid.uu.uuid16);
                } else if (p_data->p_service_data->service_uuid.len == LEN_UUID_32) {
                    *p++ = BTM_BLE_AD_TYPE_32SERVICE_DATA;
                    UINT32_TO_STREAM(p, p_data->p_service_data->service_uuid.uu.uuid32);
                } else {
                    *p++ = BTM_BLE_AD_TYPE_128SERVICE_DATA;
                    ARRAY_TO_STREAM(p, p_data->p_service_data->service_uuid.uu.uuid128,
                                    LEN_UUID_128);
                }

                ARRAY_TO_STREAM(p, p_data->p_service_data->p_val, cp_len);

                len -= (cp_len + MIN_ADV_LENGTH + p_data->p_service_data->service_uuid.len);
                data_mask &= ~BTM_BLE_AD_BIT_SERVICE_DATA;
            } else {
                BTM_TRACE_WARNING("service data does not fit");
            }
        }

        if (len >= 6 && data_mask & BTM_BLE_AD_BIT_INT_RANGE &&
                p_data) {
            *p++ = 5;
            *p++ = BTM_BLE_AD_TYPE_INT_RANGE;
            UINT16_TO_STREAM(p, p_data->int_range.low);
            UINT16_TO_STREAM(p, p_data->int_range.hi);
            len -= 6;
            data_mask &= ~BTM_BLE_AD_BIT_INT_RANGE;
        }
        if (data_mask & BTM_BLE_AD_BIT_PROPRIETARY && p_data && p_data->p_proprietary) {
            for (i = 0; i < p_data->p_proprietary->num_elem ; i ++) {
                p_elem = p_data->p_proprietary->p_elem  + i;

                if (len >= (MIN_ADV_LENGTH + p_elem->len))/* len byte(1) + ATTR type(1) + Uuid len(2)
                                                          + value length */
                {
                    *p ++ = p_elem->len + 1; /* Uuid len + value length */
                    *p ++ = p_elem->adv_type;
                    ARRAY_TO_STREAM(p, p_elem->p_val, p_elem->len);

                    len -= (MIN_ADV_LENGTH + p_elem->len);
                } else {
                    BTM_TRACE_WARNING("data exceed max adv packet length");
                    break;
                }
            }
            data_mask &= ~BTM_BLE_AD_BIT_PROPRIETARY;
        }
    }

    *p_data_mask = data_mask;
    *p_dst = p;

    return p_flag;
}
/*******************************************************************************
**
** Function         btm_ble_select_adv_interval
**
** Description      select adv interval based on device mode
**
** Returns          void
**
*******************************************************************************/
void btm_ble_select_adv_interval(tBTM_BLE_INQ_CB *p_cb, UINT8 evt_type, UINT16 *p_adv_int_min, UINT16 *p_adv_int_max)
{
    if (p_cb->adv_interval_min && p_cb->adv_interval_max) {
        *p_adv_int_min = p_cb->adv_interval_min;
        *p_adv_int_max = p_cb->adv_interval_max;
    } else {
        switch (evt_type) {
        case BTM_BLE_CONNECT_EVT:
        case BTM_BLE_CONNECT_LO_DUTY_DIR_EVT:
            *p_adv_int_min = *p_adv_int_max = BTM_BLE_GAP_ADV_FAST_INT_1;
            break;

        case BTM_BLE_NON_CONNECT_EVT:
        case BTM_BLE_DISCOVER_EVT:
            *p_adv_int_min = *p_adv_int_max = BTM_BLE_GAP_ADV_FAST_INT_2;
            break;

        /* connectable directed event */
        case BTM_BLE_CONNECT_DIR_EVT:
            *p_adv_int_min = BTM_BLE_GAP_ADV_DIR_MIN_INT;
            *p_adv_int_max = BTM_BLE_GAP_ADV_DIR_MAX_INT;
            break;

        default:
            *p_adv_int_min = *p_adv_int_max = BTM_BLE_GAP_ADV_SLOW_INT;
            break;
        }
    }
    return;
}

/*******************************************************************************
**
** Function         btm_ble_update_dmt_flag_bits
**
** Description      Obtain updated adv flag value based on connect and discoverability mode.
**                  Also, setup DMT support value in the flag based on whether the controller
**                  supports both LE and BR/EDR.
**
** Parameters:      flag_value (Input / Output) - flag value
**                  connect_mode (Input) - Connect mode value
**                  disc_mode (Input) - discoverability mode
**
** Returns          void
**
*******************************************************************************/
void btm_ble_update_dmt_flag_bits(UINT8 *adv_flag_value, const UINT16 connect_mode,
                                  const UINT16 disc_mode)
{
    /* BR/EDR non-discoverable , non-connectable */
    if ((disc_mode & BTM_DISCOVERABLE_MASK) == 0 &&
            (connect_mode & BTM_CONNECTABLE_MASK) == 0) {
        *adv_flag_value |= BTM_BLE_BREDR_NOT_SPT;
    } else {
        *adv_flag_value &= ~BTM_BLE_BREDR_NOT_SPT;
    }

    /* if local controller support, mark both controller and host support in flag */
    if (controller_get_interface()->supports_simultaneous_le_bredr()) {
        *adv_flag_value |= (BTM_BLE_DMT_CONTROLLER_SPT | BTM_BLE_DMT_HOST_SPT);
    } else {
        *adv_flag_value &= ~(BTM_BLE_DMT_CONTROLLER_SPT | BTM_BLE_DMT_HOST_SPT);
    }
}

/*******************************************************************************
**
** Function         btm_ble_set_adv_flag
**
** Description      Set adv flag in adv data.
**
** Parameters:      connect_mode (Input)- Connect mode value
**                  disc_mode (Input) - discoverability mode
**
** Returns          void
**
*******************************************************************************/
void btm_ble_set_adv_flag(UINT16 connect_mode, UINT16 disc_mode)
{
    UINT8 flag = 0, old_flag = 0;
    tBTM_BLE_LOCAL_ADV_DATA *p_adv_data = &btm_cb.ble_ctr_cb.inq_var.adv_data;

    if (p_adv_data->p_flags != NULL) {
        flag = old_flag = *(p_adv_data->p_flags);
    }

    btm_ble_update_dmt_flag_bits (&flag, connect_mode, disc_mode);

    LOG_DEBUG("disc_mode %04x", disc_mode);
    /* update discoverable flag */
    if (disc_mode & BTM_BLE_LIMITED_DISCOVERABLE) {
        flag &= ~BTM_BLE_GEN_DISC_FLAG;
        flag |= BTM_BLE_LIMIT_DISC_FLAG;
    } else if (disc_mode & BTM_BLE_GENERAL_DISCOVERABLE) {
        flag |= BTM_BLE_GEN_DISC_FLAG;
        flag &= ~BTM_BLE_LIMIT_DISC_FLAG;
    } else { /* remove all discoverable flags */
        flag &= ~(BTM_BLE_LIMIT_DISC_FLAG | BTM_BLE_GEN_DISC_FLAG);
    }

    if (flag != old_flag) {
        LOG_ERROR("flag = 0x%x,old_flag = 0x%x", flag, old_flag);
        btm_ble_update_adv_flag(flag);
    }
}
/*******************************************************************************
**
** Function         btm_ble_set_discoverability
**
** Description      This function is called to set BLE discoverable mode.
**
** Parameters:      combined_mode: discoverability mode.
**
** Returns          BTM_SUCCESS is status set successfully; otherwise failure.
**
*******************************************************************************/
tBTM_STATUS btm_ble_set_discoverability(UINT16 combined_mode)
{
    tBTM_LE_RANDOM_CB   *p_addr_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    tBTM_BLE_INQ_CB     *p_cb = &btm_cb.ble_ctr_cb.inq_var;
    UINT16              mode = (combined_mode &  BTM_BLE_DISCOVERABLE_MASK);
    UINT8               new_mode = BTM_BLE_ADV_ENABLE;
    UINT8               evt_type;
    tBTM_STATUS         status = BTM_SUCCESS;
    BD_ADDR             p_addr_ptr = {0};
    tBLE_ADDR_TYPE      init_addr_type = BLE_ADDR_PUBLIC,
                        own_addr_type = p_addr_cb->own_addr_type;
    UINT16              adv_int_min, adv_int_max;

    BTM_TRACE_EVENT ("%s mode=0x%0x combined_mode=0x%x\n", __FUNCTION__, mode, combined_mode);

    /*** Check mode parameter ***/
    if (mode > BTM_BLE_MAX_DISCOVERABLE) {
        return (BTM_ILLEGAL_VALUE);
    }

    p_cb->discoverable_mode = mode;

    evt_type = btm_set_conn_mode_adv_init_addr(p_cb, p_addr_ptr, &init_addr_type, &own_addr_type);

    if (p_cb->connectable_mode == BTM_BLE_NON_CONNECTABLE && mode == BTM_BLE_NON_DISCOVERABLE) {
        new_mode = BTM_BLE_ADV_DISABLE;
    }

    btm_ble_select_adv_interval(p_cb, evt_type, &adv_int_min, &adv_int_max);

    btu_stop_timer(&p_cb->fast_adv_timer);

    /* update adv params if start advertising */
    BTM_TRACE_EVENT ("evt_type=0x%x p-cb->evt_type=0x%x\n ", evt_type, p_cb->evt_type);

    if (new_mode == BTM_BLE_ADV_ENABLE) {
        btm_ble_set_adv_flag (btm_cb.btm_inq_vars.connectable_mode, combined_mode);

        if (evt_type != p_cb->evt_type || p_cb->adv_addr_type != own_addr_type
                || !p_cb->fast_adv_on) {
            btm_ble_stop_adv();

            /* update adv params */
            if (!btsnd_hcic_ble_write_adv_params (adv_int_min,
                                                  adv_int_max,
                                                  evt_type,
                                                  own_addr_type,
                                                  init_addr_type,
                                                  p_addr_ptr,
                                                  p_cb->adv_chnl_map,
                                                  p_cb->afp)) {
                status = BTM_NO_RESOURCES;
            } else {
                p_cb->evt_type = evt_type;
                p_cb->adv_addr_type = own_addr_type;
            }
        }
    }

    if (status == BTM_SUCCESS && p_cb->adv_mode != new_mode) {
        if (new_mode == BTM_BLE_ADV_ENABLE) {
            status = btm_ble_start_adv();
        } else {
            status = btm_ble_stop_adv();
        }
    }

    if (p_cb->adv_mode == BTM_BLE_ADV_ENABLE) {
        p_cb->fast_adv_on = TRUE;
        /* start initial GAP mode adv timer */
        btu_start_timer (&p_cb->fast_adv_timer, BTU_TTYPE_BLE_GAP_FAST_ADV,
                         BTM_BLE_GAP_FAST_ADV_TOUT);
    } else {
#if BLE_PRIVACY_SPT == TRUE
        btm_ble_disable_resolving_list(BTM_BLE_RL_ADV, TRUE);
#endif
    }

    /* set up stop advertising timer */
    if (status == BTM_SUCCESS && mode == BTM_BLE_LIMITED_DISCOVERABLE) {
        BTM_TRACE_EVENT ("start timer for limited disc mode duration=%d (180 secs)", BTM_BLE_GAP_LIM_TOUT);
        /* start Tgap(lim_timeout) */
        btu_start_timer (&p_cb->inq_timer_ent, BTU_TTYPE_BLE_GAP_LIM_DISC,
                         BTM_BLE_GAP_LIM_TOUT);
    }
    return status;
}

/*******************************************************************************
**
** Function         btm_ble_set_connectability
**
** Description      This function is called to set BLE connectability mode.
**
** Parameters:      combined_mode: connectability mode.
**
** Returns          BTM_SUCCESS is status set successfully; otherwise failure.
**
*******************************************************************************/
tBTM_STATUS btm_ble_set_connectability(UINT16 combined_mode)
{
    tBTM_LE_RANDOM_CB       *p_addr_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    tBTM_BLE_INQ_CB         *p_cb = &btm_cb.ble_ctr_cb.inq_var;
    UINT16                  mode = (combined_mode & BTM_BLE_CONNECTABLE_MASK);
    UINT8                   new_mode = BTM_BLE_ADV_ENABLE;
    UINT8                   evt_type;
    tBTM_STATUS             status = BTM_SUCCESS;
    BD_ADDR                 p_addr_ptr =  {0};
    tBLE_ADDR_TYPE          peer_addr_type = BLE_ADDR_PUBLIC,
                            own_addr_type = p_addr_cb->own_addr_type;
    UINT16                  adv_int_min, adv_int_max;

    BTM_TRACE_EVENT ("%s mode=0x%0x combined_mode=0x%x\n", __FUNCTION__, mode, combined_mode);

    /*** Check mode parameter ***/
    if (mode > BTM_BLE_MAX_CONNECTABLE) {
        return (BTM_ILLEGAL_VALUE);
    }

    p_cb->connectable_mode = mode;

    evt_type = btm_set_conn_mode_adv_init_addr(p_cb, p_addr_ptr, &peer_addr_type, &own_addr_type);

    if (mode == BTM_BLE_NON_CONNECTABLE && p_cb->discoverable_mode == BTM_BLE_NON_DISCOVERABLE) {
        new_mode = BTM_BLE_ADV_DISABLE;
    }

    btm_ble_select_adv_interval(p_cb, evt_type, &adv_int_min, &adv_int_max);

    btu_stop_timer(&p_cb->fast_adv_timer);
    /* update adv params if needed */
    if (new_mode == BTM_BLE_ADV_ENABLE) {
        btm_ble_set_adv_flag (combined_mode, btm_cb.btm_inq_vars.discoverable_mode);
        if (p_cb->evt_type != evt_type || p_cb->adv_addr_type != p_addr_cb->own_addr_type
                || !p_cb->fast_adv_on) {
            btm_ble_stop_adv();

            if (!btsnd_hcic_ble_write_adv_params (adv_int_min,
                                                  adv_int_max,
                                                  evt_type,
                                                  own_addr_type,
                                                  peer_addr_type,
                                                  p_addr_ptr,
                                                  p_cb->adv_chnl_map,
                                                  p_cb->afp)) {
                status = BTM_NO_RESOURCES;
            } else {
                p_cb->evt_type = evt_type;
                p_cb->adv_addr_type = own_addr_type;
            }
        }
    }

    /* update advertising mode */
    if (status == BTM_SUCCESS && new_mode != p_cb->adv_mode) {
        if (new_mode == BTM_BLE_ADV_ENABLE) {
            status = btm_ble_start_adv();
        } else {
            status = btm_ble_stop_adv();
        }
    }

    if (p_cb->adv_mode == BTM_BLE_ADV_ENABLE) {
        p_cb->fast_adv_on = TRUE;
        /* start initial GAP mode adv timer */
        btu_start_timer (&p_cb->fast_adv_timer, BTU_TTYPE_BLE_GAP_FAST_ADV,
                         BTM_BLE_GAP_FAST_ADV_TOUT);
    } else {
#if BLE_PRIVACY_SPT == TRUE
        btm_ble_disable_resolving_list(BTM_BLE_RL_ADV, TRUE);
#endif
    }
    return status;
}


/*******************************************************************************
**
** Function         btm_ble_start_inquiry
**
** Description      This function is called to start BLE inquiry procedure.
**                  If the duration is zero, the periodic inquiry mode is cancelled.
**
** Parameters:      mode - GENERAL or LIMITED inquiry
**                  p_inq_params - pointer to the BLE inquiry parameter.
**                  p_results_cb - callback returning pointer to results (tBTM_INQ_RESULTS)
**                  p_cmpl_cb - callback indicating the end of an inquiry
**
**
**
** Returns          BTM_CMD_STARTED if successfully started
**                  BTM_NO_RESOURCES if could not allocate a message buffer
**                  BTM_BUSY - if an inquiry is already active
**
*******************************************************************************/
tBTM_STATUS btm_ble_start_inquiry (UINT8 mode, UINT8   duration)
{
    tBTM_STATUS status = BTM_CMD_STARTED;
    tBTM_BLE_CB *p_ble_cb = &btm_cb.ble_ctr_cb;
    tBTM_INQUIRY_VAR_ST      *p_inq = &btm_cb.btm_inq_vars;

    BTM_TRACE_DEBUG("btm_ble_start_inquiry: mode = %02x inq_active = 0x%02x", mode, btm_cb.btm_inq_vars.inq_active);

    /* if selective connection is active, or inquiry is already active, reject it */
    if (BTM_BLE_IS_INQ_ACTIVE(p_ble_cb->scan_activity) ||
            BTM_BLE_IS_SEL_CONN_ACTIVE (p_ble_cb->scan_activity)) {
        BTM_TRACE_ERROR("LE Inquiry is active, can not start inquiry");
        return (BTM_BUSY);
    }

    if (!BTM_BLE_IS_SCAN_ACTIVE(p_ble_cb->scan_activity)) {
        btsnd_hcic_ble_set_scan_params(BTM_BLE_SCAN_MODE_ACTI,
                                       BTM_BLE_LOW_LATENCY_SCAN_INT,
                                       BTM_BLE_LOW_LATENCY_SCAN_WIN,
                                       btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type,
                                       SP_ADV_ALL);
#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
        /* enable IRK list */
        btm_ble_enable_resolving_list_for_platform(BTM_BLE_RL_SCAN);
#endif
        p_ble_cb->inq_var.scan_duplicate_filter  = BTM_BLE_DUPLICATE_DISABLE;
        status = btm_ble_start_scan();
    } else if ((p_ble_cb->inq_var.scan_interval != BTM_BLE_LOW_LATENCY_SCAN_INT) ||
               (p_ble_cb->inq_var.scan_window != BTM_BLE_LOW_LATENCY_SCAN_WIN)) {
        BTM_TRACE_DEBUG("%s, restart LE scan with low latency scan params", __FUNCTION__);
        btsnd_hcic_ble_set_scan_enable(BTM_BLE_SCAN_DISABLE, BTM_BLE_DUPLICATE_ENABLE);
        btsnd_hcic_ble_set_scan_params(BTM_BLE_SCAN_MODE_ACTI,
                                       BTM_BLE_LOW_LATENCY_SCAN_INT,
                                       BTM_BLE_LOW_LATENCY_SCAN_WIN,
                                       btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type,
                                       SP_ADV_ALL);
        btsnd_hcic_ble_set_scan_enable(BTM_BLE_SCAN_ENABLE, BTM_BLE_DUPLICATE_DISABLE);
    }

    if (status == BTM_CMD_STARTED) {
        p_inq->inq_active |= mode;
        p_ble_cb->scan_activity |= mode;

        BTM_TRACE_DEBUG("btm_ble_start_inquiry inq_active = 0x%02x", p_inq->inq_active);

        if (duration != 0) {
            /* start inquiry timer */
            btu_start_timer (&p_ble_cb->inq_var.inq_timer_ent, BTU_TTYPE_BLE_INQUIRY, duration);
        }
    }

    return status;

}

/*******************************************************************************
**
** Function         btm_ble_read_remote_name_cmpl
**
** Description      This function is called when BLE remote name is received.
**
** Returns          void
**
*******************************************************************************/
void btm_ble_read_remote_name_cmpl(BOOLEAN status, BD_ADDR bda, UINT16 length, char *p_name)
{
    UINT8   hci_status = HCI_SUCCESS;
    BD_NAME bd_name;

    memset(bd_name, 0, (BD_NAME_LEN + 1));
    if (length > BD_NAME_LEN) {
        length = BD_NAME_LEN;
    }
    memcpy((UINT8 *)bd_name, p_name, length);

    if ((!status) || (length == 0)) {
        hci_status = HCI_ERR_HOST_TIMEOUT;
    }

    btm_process_remote_name(bda, bd_name, length + 1, hci_status);
#if (SMP_INCLUDED == TRUE)
    btm_sec_rmt_name_request_complete (bda, (UINT8 *)p_name, hci_status);
#endif  ///SMP_INCLUDED == TRUE
}

/*******************************************************************************
**
** Function         btm_ble_read_remote_name
**
** Description      This function read remote LE device name using GATT read
**                  procedure.
**
** Parameters:       None.
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS btm_ble_read_remote_name(BD_ADDR remote_bda, tBTM_INQ_INFO *p_cur, tBTM_CMPL_CB *p_cb)
{
    tBTM_INQUIRY_VAR_ST      *p_inq = &btm_cb.btm_inq_vars;

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ERR_PROCESSING;
    }

    if (p_cur &&
            p_cur->results.ble_evt_type != BTM_BLE_EVT_CONN_ADV &&
            p_cur->results.ble_evt_type != BTM_BLE_EVT_CONN_DIR_ADV) {
        BTM_TRACE_DEBUG("name request to non-connectable device failed.");
        return BTM_ERR_PROCESSING;
    }

    /* read remote device name using GATT procedure */
    if (p_inq->remname_active) {
        return BTM_BUSY;
    }

#if (defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE && GATTS_INCLUDED == TRUE)
    if (!GAP_BleReadPeerDevName(remote_bda, btm_ble_read_remote_name_cmpl)) {
        return BTM_BUSY;
    }
#endif

    p_inq->p_remname_cmpl_cb = p_cb;
    p_inq->remname_active = TRUE;

    memcpy(p_inq->remname_bda, remote_bda, BD_ADDR_LEN);

    btu_start_timer (&p_inq->rmt_name_timer_ent,
                     BTU_TTYPE_BTM_RMT_NAME,
                     BTM_EXT_BLE_RMT_NAME_TIMEOUT);

    return BTM_CMD_STARTED;
}

/*******************************************************************************
**
** Function         btm_ble_cancel_remote_name
**
** Description      This function cancel read remote LE device name.
**
** Parameters:       None.
**
** Returns          void
**
*******************************************************************************/
BOOLEAN btm_ble_cancel_remote_name(BD_ADDR remote_bda)
{
    tBTM_INQUIRY_VAR_ST      *p_inq = &btm_cb.btm_inq_vars;
    BOOLEAN     status = TRUE;

#if (defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE && GATTS_INCLUDED == TRUE)
    status = GAP_BleCancelReadPeerDevName(remote_bda);
#endif

    p_inq->remname_active = FALSE;
    memset(p_inq->remname_bda, 0, BD_ADDR_LEN);
    btu_stop_timer(&p_inq->rmt_name_timer_ent);

    return status;
}

/*******************************************************************************
**
** Function         btm_ble_update_adv_flag
**
** Description      This function update the limited discoverable flag in the adv
**                  data.
**
** Parameters:       None.
**
** Returns          void
**
*******************************************************************************/
static void btm_ble_update_adv_flag(UINT8 flag)
{
    tBTM_BLE_LOCAL_ADV_DATA *p_adv_data = &btm_cb.ble_ctr_cb.inq_var.adv_data;
    UINT8   *p;

    BTM_TRACE_DEBUG ("btm_ble_update_adv_flag new=0x%x", flag);

    if (p_adv_data->p_flags != NULL) {
        BTM_TRACE_DEBUG ("btm_ble_update_adv_flag old=0x%x",   *p_adv_data->p_flags);
        *p_adv_data->p_flags = flag;
    } else { /* no FLAGS in ADV data*/
        p = (p_adv_data->p_pad == NULL) ? p_adv_data->ad_data : p_adv_data->p_pad;
        /* need 3 bytes space to stuff in the flags, if not */
        /* erase all written data, just for flags */
        if ((BTM_BLE_AD_DATA_LEN - (p - p_adv_data->ad_data)) < 3) {
            p = p_adv_data->p_pad = p_adv_data->ad_data;
            memset(p_adv_data->ad_data, 0, BTM_BLE_AD_DATA_LEN);
        }

        *p++ = 2;
        *p++ = BTM_BLE_AD_TYPE_FLAG;
        p_adv_data->p_flags = p;
        *p++ = flag;
        p_adv_data->p_pad = p;
    }

    if (btsnd_hcic_ble_set_adv_data((UINT8)(p_adv_data->p_pad - p_adv_data->ad_data),
                                    p_adv_data->ad_data)) {
        p_adv_data->data_mask |= BTM_BLE_AD_BIT_FLAGS;
    }

}

#if 0
/*******************************************************************************
**
** Function         btm_ble_parse_adv_data
**
** Description      This function parse the adv data into a structure.
**
** Returns          pointer to entry, or NULL if not found
**
*******************************************************************************/
static void btm_ble_parse_adv_data(tBTM_INQ_INFO *p_info, UINT8 *p_data,
                                   UINT8 len, tBTM_BLE_INQ_DATA *p_adv_data, UINT8 *p_buf)
{
    UINT8   *p_cur = p_data;
    UINT8   ad_len, ad_type, ad_flag;

    BTM_TRACE_EVENT (" btm_ble_parse_adv_data");

    while (len > 0) {
        BTM_TRACE_DEBUG("btm_ble_parse_adv_data: len = %d", len);
        if ((ad_len = *p_cur ++) == 0) {
            break;
        }

        ad_type = *p_cur ++;

        BTM_TRACE_DEBUG("     ad_type = %02x ad_len = %d", ad_type, ad_len);

        switch (ad_type) {
        case BTM_BLE_AD_TYPE_NAME_SHORT:

        case BTM_BLE_AD_TYPE_NAME_CMPL:
            p_adv_data->ad_mask |= BTM_BLE_AD_BIT_DEV_NAME;
            if (p_info) {
                p_info->remote_name_type = (ad_type == BTM_BLE_AD_TYPE_NAME_SHORT) ?
                                           BTM_BLE_NAME_SHORT : BTM_BLE_NAME_CMPL;
                memcpy(p_info->remote_name, p_cur, ad_len - 1);
                p_info->remote_name[ad_len] = 0;
                p_adv_data->p_remote_name = p_info->remote_name;
                p_info->remote_name_len = p_adv_data->remote_name_len = ad_len - 1;
                BTM_TRACE_DEBUG("BTM_BLE_AD_TYPE_NAME name = %s", p_adv_data->p_remote_name);
            }
            p_cur += (ad_len - 1);

            break;

        case BTM_BLE_AD_TYPE_FLAG:
            p_adv_data->ad_mask |= BTM_BLE_AD_BIT_FLAGS;
            ad_flag = *p_cur ++;
            p_adv_data->flag = (UINT8)(ad_flag & BTM_BLE_ADV_FLAG_MASK) ;
            BTM_TRACE_DEBUG("BTM_BLE_AD_TYPE_FLAG flag = %s | %s | %s",
                            (p_adv_data->flag & BTM_BLE_LIMIT_DISC_FLAG) ? "LE_LIMIT_DISC" : "",
                            (p_adv_data->flag & BTM_BLE_GEN_DISC_FLAG) ? "LE_GENERAL_DISC" : "",
                            (p_adv_data->flag & BTM_BLE_BREDR_NOT_SPT) ? "LE Only device" : "");
            break;

        case BTM_BLE_AD_TYPE_TX_PWR:
            p_adv_data->ad_mask |= BTM_BLE_AD_BIT_TX_PWR;
            p_adv_data->tx_power_level = (INT8) * p_cur ++;
            BTM_TRACE_DEBUG("BTM_BLE_AD_TYPE_TX_PWR tx_level = %d", p_adv_data->tx_power_level);
            break;

        case BTM_BLE_AD_TYPE_MANU:

        case BTM_BLE_AD_TYPE_16SRV_PART:
        case BTM_BLE_AD_TYPE_16SRV_CMPL:
            p_adv_data->ad_mask |= BTM_BLE_AD_BIT_SERVICE;
            /* need allocate memory to store UUID list */
            p_adv_data->service.num_service = (ad_len - 1) / 2;
            BTM_TRACE_DEBUG("service UUID list, num = %d", p_adv_data->service.num_service);
            p_cur += (ad_len - 1);
            break;

        case BTM_BLE_AD_TYPE_SOL_SRV_UUID:
            p_adv_data->ad_mask |= BTM_BLE_AD_BIT_SERVICE_SOL;
            /* need allocate memory to store UUID list */
            p_adv_data->service.num_service = (ad_len - 1) / 2;
            BTM_TRACE_DEBUG("service UUID list, num = %d", p_adv_data->service.num_service);
            p_cur += (ad_len - 1);
            break;

        case BTM_BLE_AD_TYPE_128SOL_SRV_UUID:
            p_adv_data->ad_mask |= BTM_BLE_AD_BIT_SERVICE_128SOL;
            /* need allocate memory to store UUID list */
            p_adv_data->service.num_service = (ad_len - 1) / 16;
            BTM_TRACE_DEBUG("service UUID list, num = %d", p_adv_data->service.num_service);
            p_cur += (ad_len - 1);
            break;

        case BTM_BLE_AD_TYPE_APPEARANCE:
        case BTM_BLE_AD_TYPE_PUBLIC_TARGET:
        case BTM_BLE_AD_TYPE_RANDOM_TARGET:
        default:
            break;
        }
        len -= (ad_len + 1);
    }
}
#endif

/*******************************************************************************
**
** Function         btm_ble_cache_adv_data
**
** Description      Update advertising cache data.
**
** Returns          void
**
*******************************************************************************/
void btm_ble_cache_adv_data(tBTM_INQ_RESULTS *p_cur, UINT8 data_len, UINT8 *p, UINT8 evt_type)
{
    tBTM_BLE_INQ_CB     *p_le_inq_cb = &btm_cb.ble_ctr_cb.inq_var;
    UINT8 *p_cache;
    UINT8 length;

    /* cache adv report/scan response data */
    if (evt_type != BTM_BLE_SCAN_RSP_EVT) {
        p_le_inq_cb->adv_len = 0;
        memset(p_le_inq_cb->adv_data_cache, 0, BTM_BLE_CACHE_ADV_DATA_MAX);
        p_cur->adv_data_len = 0;
        p_cur->scan_rsp_len = 0;
    }

    if (data_len > 0) {
        p_cache = &p_le_inq_cb->adv_data_cache[p_le_inq_cb->adv_len];
        STREAM_TO_UINT8(length, p);
        while ( length && ((p_le_inq_cb->adv_len + length + 1) <= BTM_BLE_CACHE_ADV_DATA_MAX)) {
            /* copy from the length byte & data into cache */
            memcpy(p_cache, p - 1, length + 1);
            /* advance the cache pointer past data */
            p_cache += length + 1;
            /* increment cache length */
            p_le_inq_cb->adv_len += length + 1;
            /* skip the length of data */
            p += length;
            STREAM_TO_UINT8(length, p);
        }
    }

    if (evt_type != BTM_BLE_SCAN_RSP_EVT) {
        p_cur->adv_data_len = p_le_inq_cb->adv_len;
    }
    else {
        p_cur->scan_rsp_len = p_le_inq_cb->adv_len - p_cur->adv_data_len;
    }

    /* parse service UUID from adv packet and save it in inq db eir_uuid */
    /* TODO */
}

/*******************************************************************************
**
** Function         btm_ble_is_discoverable
**
** Description      check ADV flag to make sure device is discoverable and match
**                  the search condition
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
UINT8 btm_ble_is_discoverable(BD_ADDR bda, UINT8 evt_type, UINT8 *p)
{
    UINT8               *p_flag, flag = 0, rt = 0;
    UINT8                data_len;
    tBTM_INQ_PARMS      *p_cond = &btm_cb.btm_inq_vars.inqparms;
    tBTM_BLE_INQ_CB     *p_le_inq_cb = &btm_cb.ble_ctr_cb.inq_var;

    UNUSED(p);

    /* for observer, always "discoverable */
    if (BTM_BLE_IS_OBS_ACTIVE(btm_cb.ble_ctr_cb.scan_activity)) {
        rt |= BTM_BLE_OBS_RESULT;
    }

    if (BTM_BLE_IS_SEL_CONN_ACTIVE(btm_cb.ble_ctr_cb.scan_activity) &&
            (evt_type == BTM_BLE_CONNECT_EVT || evt_type == BTM_BLE_CONNECT_DIR_EVT)) {
        rt |= BTM_BLE_SEL_CONN_RESULT;
    }

    /* does not match filter condition */
    if (p_cond->filter_cond_type == BTM_FILTER_COND_BD_ADDR &&
            memcmp(bda, p_cond->filter_cond.bdaddr_cond, BD_ADDR_LEN) != 0) {
        BTM_TRACE_DEBUG("BD ADDR does not meet filter condition");
        return rt;
    }

    if (p_le_inq_cb->adv_len != 0) {
        if ((p_flag = BTM_CheckAdvData(p_le_inq_cb->adv_data_cache,
                                       BTM_BLE_AD_TYPE_FLAG, &data_len)) != NULL) {
            flag = * p_flag;

            if ((btm_cb.btm_inq_vars.inq_active & BTM_BLE_GENERAL_INQUIRY) &&
                    (flag & (BTM_BLE_LIMIT_DISC_FLAG | BTM_BLE_GEN_DISC_FLAG)) != 0) {
                BTM_TRACE_DEBUG("Find Generable Discoverable device");
                rt |= BTM_BLE_INQ_RESULT;
            }

            else if (btm_cb.btm_inq_vars.inq_active & BTM_BLE_LIMITED_INQUIRY &&
                     (flag & BTM_BLE_LIMIT_DISC_FLAG) != 0) {
                BTM_TRACE_DEBUG("Find limited discoverable device");
                rt |= BTM_BLE_INQ_RESULT;
            }
        }
    }
    return rt;
}

static void btm_ble_appearance_to_cod(UINT16 appearance, UINT8 *dev_class)
{
    dev_class[0] = 0;

    switch (appearance) {
    case BTM_BLE_APPEARANCE_GENERIC_PHONE:
        dev_class[1] = BTM_COD_MAJOR_PHONE;
        dev_class[2] = BTM_COD_MINOR_UNCLASSIFIED;
        break;
    case BTM_BLE_APPEARANCE_GENERIC_COMPUTER:
        dev_class[1] = BTM_COD_MAJOR_COMPUTER;
        dev_class[2] = BTM_COD_MINOR_UNCLASSIFIED;
        break;
    case BTM_BLE_APPEARANCE_GENERIC_REMOTE:
        dev_class[1] = BTM_COD_MAJOR_PERIPHERAL;
        dev_class[2] = BTM_COD_MINOR_REMOTE_CONTROL;
        break;
    case BTM_BLE_APPEARANCE_GENERIC_THERMOMETER:
    case BTM_BLE_APPEARANCE_THERMOMETER_EAR:
        dev_class[1] = BTM_COD_MAJOR_HEALTH;
        dev_class[2] = BTM_COD_MINOR_THERMOMETER;
        break;
    case BTM_BLE_APPEARANCE_GENERIC_HEART_RATE:
    case BTM_BLE_APPEARANCE_HEART_RATE_BELT:
        dev_class[1] = BTM_COD_MAJOR_HEALTH;
        dev_class[2] = BTM_COD_MINOR_HEART_PULSE_MONITOR;
        break;
    case BTM_BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE:
    case BTM_BLE_APPEARANCE_BLOOD_PRESSURE_ARM:
    case BTM_BLE_APPEARANCE_BLOOD_PRESSURE_WRIST:
        dev_class[1] = BTM_COD_MAJOR_HEALTH;
        dev_class[2] = BTM_COD_MINOR_BLOOD_MONITOR;
        break;
    case BTM_BLE_APPEARANCE_GENERIC_PULSE_OXIMETER:
    case BTM_BLE_APPEARANCE_PULSE_OXIMETER_FINGERTIP:
    case BTM_BLE_APPEARANCE_PULSE_OXIMETER_WRIST:
        dev_class[1] = BTM_COD_MAJOR_HEALTH;
        dev_class[2] = BTM_COD_MINOR_PULSE_OXIMETER;
        break;
    case BTM_BLE_APPEARANCE_GENERIC_GLUCOSE:
        dev_class[1] = BTM_COD_MAJOR_HEALTH;
        dev_class[2] = BTM_COD_MINOR_GLUCOSE_METER;
        break;
    case BTM_BLE_APPEARANCE_GENERIC_WEIGHT:
        dev_class[1] = BTM_COD_MAJOR_HEALTH;
        dev_class[2] = BTM_COD_MINOR_WEIGHING_SCALE;
        break;
    case BTM_BLE_APPEARANCE_GENERIC_WALKING:
    case BTM_BLE_APPEARANCE_WALKING_IN_SHOE:
    case BTM_BLE_APPEARANCE_WALKING_ON_SHOE:
    case BTM_BLE_APPEARANCE_WALKING_ON_HIP:
        dev_class[1] = BTM_COD_MAJOR_HEALTH;
        dev_class[2] = BTM_COD_MINOR_STEP_COUNTER;
        break;
    case BTM_BLE_APPEARANCE_GENERIC_WATCH:
    case BTM_BLE_APPEARANCE_SPORTS_WATCH:
        dev_class[1] = BTM_COD_MAJOR_WEARABLE;
        dev_class[2] = BTM_COD_MINOR_WRIST_WATCH;
        break;
    case BTM_BLE_APPEARANCE_GENERIC_EYEGLASSES:
        dev_class[1] = BTM_COD_MAJOR_WEARABLE;
        dev_class[2] = BTM_COD_MINOR_GLASSES;
        break;
    case BTM_BLE_APPEARANCE_GENERIC_DISPLAY:
        dev_class[1] = BTM_COD_MAJOR_IMAGING;
        dev_class[2] = BTM_COD_MINOR_DISPLAY;
        break;
    case BTM_BLE_APPEARANCE_GENERIC_MEDIA_PLAYER:
        dev_class[1] = BTM_COD_MAJOR_AUDIO;
        dev_class[2] = BTM_COD_MINOR_UNCLASSIFIED;
        break;
    case BTM_BLE_APPEARANCE_GENERIC_BARCODE_SCANNER:
    case BTM_BLE_APPEARANCE_HID_BARCODE_SCANNER:
    case BTM_BLE_APPEARANCE_GENERIC_HID:
        dev_class[1] = BTM_COD_MAJOR_PERIPHERAL;
        dev_class[2] = BTM_COD_MINOR_UNCLASSIFIED;
        break;
    case BTM_BLE_APPEARANCE_HID_KEYBOARD:
        dev_class[1] = BTM_COD_MAJOR_PERIPHERAL;
        dev_class[2] = BTM_COD_MINOR_KEYBOARD;
        break;
    case BTM_BLE_APPEARANCE_HID_MOUSE:
        dev_class[1] = BTM_COD_MAJOR_PERIPHERAL;
        dev_class[2] = BTM_COD_MINOR_POINTING;
        break;
    case BTM_BLE_APPEARANCE_HID_JOYSTICK:
        dev_class[1] = BTM_COD_MAJOR_PERIPHERAL;
        dev_class[2] = BTM_COD_MINOR_JOYSTICK;
        break;
    case BTM_BLE_APPEARANCE_HID_GAMEPAD:
        dev_class[1] = BTM_COD_MAJOR_PERIPHERAL;
        dev_class[2] = BTM_COD_MINOR_GAMEPAD;
        break;
    case BTM_BLE_APPEARANCE_HID_DIGITIZER_TABLET:
        dev_class[1] = BTM_COD_MAJOR_PERIPHERAL;
        dev_class[2] = BTM_COD_MINOR_DIGITIZING_TABLET;
        break;
    case BTM_BLE_APPEARANCE_HID_CARD_READER:
        dev_class[1] = BTM_COD_MAJOR_PERIPHERAL;
        dev_class[2] = BTM_COD_MINOR_CARD_READER;
        break;
    case BTM_BLE_APPEARANCE_HID_DIGITAL_PEN:
        dev_class[1] = BTM_COD_MAJOR_PERIPHERAL;
        dev_class[2] = BTM_COD_MINOR_DIGITAL_PAN;
        break;
    case BTM_BLE_APPEARANCE_UKNOWN:
    case BTM_BLE_APPEARANCE_GENERIC_CLOCK:
    case BTM_BLE_APPEARANCE_GENERIC_TAG:
    case BTM_BLE_APPEARANCE_GENERIC_KEYRING:
    case BTM_BLE_APPEARANCE_GENERIC_CYCLING:
    case BTM_BLE_APPEARANCE_CYCLING_COMPUTER:
    case BTM_BLE_APPEARANCE_CYCLING_SPEED:
    case BTM_BLE_APPEARANCE_CYCLING_CADENCE:
    case BTM_BLE_APPEARANCE_CYCLING_POWER:
    case BTM_BLE_APPEARANCE_CYCLING_SPEED_CADENCE:
    case BTM_BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS:
    case BTM_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION:
    case BTM_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_AND_NAV:
    case BTM_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD:
    case BTM_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD_AND_NAV:
    default:
        dev_class[1] = BTM_COD_MAJOR_UNCLASSIFIED;
        dev_class[2] = BTM_COD_MINOR_UNCLASSIFIED;
    };
}

/*******************************************************************************
**
** Function         btm_ble_update_inq_result
**
** Description      Update adv packet information into inquiry result.
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
BOOLEAN btm_ble_update_inq_result(tINQ_DB_ENT *p_i, UINT8 addr_type, UINT8 evt_type, UINT8 *p)
{
    BOOLEAN             to_report = TRUE;
    tBTM_INQ_RESULTS     *p_cur = &p_i->inq_info.results;
    UINT8               len;
    UINT8               *p_flag;
    tBTM_INQUIRY_VAR_ST  *p_inq = &btm_cb.btm_inq_vars;
    UINT8                data_len, rssi;
    tBTM_BLE_INQ_CB     *p_le_inq_cb = &btm_cb.ble_ctr_cb.inq_var;
    UINT8 *p1;
    UINT8               *p_uuid16;

    STREAM_TO_UINT8    (data_len, p);

    if (data_len > BTM_BLE_ADV_DATA_LEN_MAX) {
        BTM_TRACE_WARNING("EIR data too long %d. discard", data_len);
        return FALSE;
    }
    btm_ble_cache_adv_data(p_cur, data_len, p, evt_type);

    p1 = (p + data_len);
    STREAM_TO_UINT8 (rssi, p1);

    /* Save the info */
    p_cur->inq_result_type = BTM_INQ_RESULT_BLE;
    p_cur->ble_addr_type    = addr_type;
    p_cur->rssi = rssi;

    /* active scan, always wait until get scan_rsp to report the result */
    if ((btm_cb.ble_ctr_cb.inq_var.scan_type == BTM_BLE_SCAN_MODE_ACTI &&
            (evt_type == BTM_BLE_CONNECT_EVT || evt_type == BTM_BLE_DISCOVER_EVT))) {
        BTM_TRACE_DEBUG("btm_ble_update_inq_result scan_rsp=false, to_report=false,\
                              scan_type_active=%d", btm_cb.ble_ctr_cb.inq_var.scan_type);
        p_i->scan_rsp = FALSE;
        to_report = FALSE;
    } else {
        p_i->scan_rsp = TRUE;
    }

    if (p_i->inq_count != p_inq->inq_counter) {
        p_cur->device_type = BT_DEVICE_TYPE_BLE;
    } else {
        p_cur->device_type |= BT_DEVICE_TYPE_BLE;
    }

    if (evt_type != BTM_BLE_SCAN_RSP_EVT) {
        p_cur->ble_evt_type     = evt_type;
    }

    p_i->inq_count = p_inq->inq_counter;   /* Mark entry for current inquiry */

    if (p_le_inq_cb->adv_len != 0) {
        if ((p_flag = BTM_CheckAdvData(p_le_inq_cb->adv_data_cache, BTM_BLE_AD_TYPE_FLAG, &len)) != NULL) {
            p_cur->flag = * p_flag;
        }
    }

    if (p_le_inq_cb->adv_len != 0) {
        /* Check to see the BLE device has the Appearance UUID in the advertising data.  If it does
         * then try to convert the appearance value to a class of device value Bluedroid can use.
         * Otherwise fall back to trying to infer if it is a HID device based on the service class.
         */
        p_uuid16 = BTM_CheckAdvData(p_le_inq_cb->adv_data_cache, BTM_BLE_AD_TYPE_APPEARANCE, &len);
        if (p_uuid16 && len == 2) {
            btm_ble_appearance_to_cod((UINT16)p_uuid16[0] | (p_uuid16[1] << 8), p_cur->dev_class);
        } else {
            if ((p_uuid16 = BTM_CheckAdvData(p_le_inq_cb->adv_data_cache,
                                             BTM_BLE_AD_TYPE_16SRV_CMPL, &len)) != NULL) {
                UINT8 i;
                for (i = 0; i + 2 <= len; i = i + 2) {
#if BTA_HH_LE_INCLUDED == TRUE
					/* if this BLE device support HID over LE, set HID Major in class of device */
                    if ((p_uuid16[i] | (p_uuid16[i + 1] << 8)) == UUID_SERVCLASS_LE_HID) {
                        p_cur->dev_class[0] = 0;
                        p_cur->dev_class[1] = BTM_COD_MAJOR_PERIPHERAL;
                        p_cur->dev_class[2] = 0;
                        break;
                    }
#endif /* BTA_HH_LE_INCLUDED */
                }
            }
        }
    }

    /* if BR/EDR not supported is not set, assume is a DUMO device */
    if ((p_cur->flag & BTM_BLE_BREDR_NOT_SPT) == 0 &&
            evt_type != BTM_BLE_CONNECT_DIR_EVT) {
        if (p_cur->ble_addr_type != BLE_ADDR_RANDOM) {
            BTM_TRACE_DEBUG("BR/EDR NOT support bit not set, treat as DUMO");
            p_cur->device_type |= BT_DEVICE_TYPE_DUMO;
        } else {
            BTM_TRACE_DEBUG("Random address, treating device as LE only");
        }
    } else {
        BTM_TRACE_DEBUG("BR/EDR NOT SUPPORT bit set, LE only device");
    }

    return to_report;

}

/*******************************************************************************
**
** Function         btm_clear_all_pending_le_entry
**
** Description      This function is called to clear all LE pending entry in
**                  inquiry database.
**
** Returns          void
**
*******************************************************************************/
void btm_clear_all_pending_le_entry(void)
{
    UINT16       xx;
    tINQ_DB_ENT  *p_ent = btm_cb.btm_inq_vars.inq_db;

    for (xx = 0; xx < BTM_INQ_DB_SIZE; xx++, p_ent++) {
        /* mark all pending LE entry as unused if an LE only device has scan response outstanding */
        if ((p_ent->in_use) &&
                (p_ent->inq_info.results.device_type == BT_DEVICE_TYPE_BLE) &&
                !p_ent->scan_rsp) {
            p_ent->in_use = FALSE;
        }
    }
}

/*******************************************************************************
**
** Function         btm_send_sel_conn_callback
**
** Description      send selection connection request callback.
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
void btm_send_sel_conn_callback(BD_ADDR remote_bda, UINT8 evt_type, UINT8 *p_data, UINT8 addr_type)
{
    UINT8   data_len, len;
    UINT8   *p_dev_name, remname[31] = {0};
    UNUSED(addr_type);

    if (btm_cb.ble_ctr_cb.p_select_cback == NULL ||
            /* non-connectable device */
            (evt_type != BTM_BLE_EVT_CONN_ADV && evt_type != BTM_BLE_EVT_CONN_DIR_ADV)) {
        return;
    }

    STREAM_TO_UINT8    (data_len, p_data);

    /* get the device name if exist in ADV data */
    if (data_len != 0) {
        p_dev_name = BTM_CheckAdvData(p_data, BTM_BLE_AD_TYPE_NAME_CMPL, &len);

        if (p_dev_name == NULL) {
            p_dev_name = BTM_CheckAdvData(p_data, BTM_BLE_AD_TYPE_NAME_SHORT, &len);
        }

        if (p_dev_name) {
            memcpy(remname, p_dev_name, len);
        }
    }
    /* allow connection */
    if ((* btm_cb.ble_ctr_cb.p_select_cback)(remote_bda, remname)) {
        /* terminate selective connection, initiate connection */
        btm_ble_initiate_select_conn(remote_bda);
    }
}

/*******************************************************************************
**
** Function         btm_ble_process_adv_pkt
**
** Description      This function is called when adv packet report events are
**                  received from the device. It updates the inquiry database.
**                  If the inquiry database is full, the oldest entry is discarded.
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
void btm_ble_process_adv_pkt (UINT8 *p_data)
{
    BD_ADDR             bda;
    UINT8               evt_type = 0, *p = p_data;
    UINT8               addr_type = 0;
    UINT8               num_reports;
    UINT8               data_len;
#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
    BOOLEAN             match = FALSE;
#endif

    /* Only process the results if the inquiry is still active */
    if (!BTM_BLE_IS_SCAN_ACTIVE(btm_cb.ble_ctr_cb.scan_activity)) {
        return;
    }

    /* Extract the number of reports in this event. */
    STREAM_TO_UINT8(num_reports, p);

    while (num_reports--) {
        /* Extract inquiry results */
        STREAM_TO_UINT8    (evt_type, p);
        STREAM_TO_UINT8    (addr_type, p);
        STREAM_TO_BDADDR   (bda, p);
        //BTM_TRACE_ERROR("btm_ble_process_adv_pkt:bda= %0x:%0x:%0x:%0x:%0x:%0x\n",
        //                              bda[0],bda[1],bda[2],bda[3],bda[4],bda[5]);
#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
        /* map address to security record */
        match = btm_identity_addr_to_random_pseudo(bda, &addr_type, FALSE);

        // BTM_TRACE_ERROR("btm_ble_process_adv_pkt:bda= %0x:%0x:%0x:%0x:%0x:%0x\n",
        //                             bda[0],bda[1],bda[2],bda[3],bda[4],bda[5]);
        /* always do RRA resolution on host */
        if (!match && BTM_BLE_IS_RESOLVE_BDA(bda)) {
            btm_ble_resolve_random_addr(bda, btm_ble_resolve_random_addr_on_adv, p_data);
        } else
#endif
            btm_ble_process_adv_pkt_cont(bda, addr_type, evt_type, p);

        STREAM_TO_UINT8(data_len, p);

        /* Advance to the next event data_len + rssi byte */
        p += data_len + 1;
    }
}

/*******************************************************************************
**
** Function         btm_ble_process_adv_pkt_cont
**
** Description      This function is called after random address resolution is
**                  done, and proceed to process adv packet.
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static void btm_ble_process_adv_pkt_cont(BD_ADDR bda, UINT8 addr_type, UINT8 evt_type, UINT8 *p)
{
    tINQ_DB_ENT          *p_i;
    tBTM_INQUIRY_VAR_ST  *p_inq = &btm_cb.btm_inq_vars;
    tBTM_INQ_RESULTS_CB  *p_inq_results_cb = p_inq->p_inq_results_cb;
    tBTM_INQ_RESULTS_CB  *p_obs_results_cb = btm_cb.ble_ctr_cb.p_obs_results_cb;
    tBTM_BLE_INQ_CB      *p_le_inq_cb = &btm_cb.ble_ctr_cb.inq_var;
    BOOLEAN     update = TRUE;
    UINT8       result = 0;

    p_i = btm_inq_db_find (bda);

    /* Check if this address has already been processed for this inquiry */
    if (btm_inq_find_bdaddr(bda)) {
        /* never been report as an LE device */
        if (p_i &&
                (!(p_i->inq_info.results.device_type & BT_DEVICE_TYPE_BLE) ||
                 /* scan repsonse to be updated */
                 (!p_i->scan_rsp))) {
            update = TRUE;
        } else if (BTM_BLE_IS_OBS_ACTIVE(btm_cb.ble_ctr_cb.scan_activity)) {
            update = FALSE;
        } else {
            /* if yes, skip it */
            return; /* assumption: one result per event */
        }
    }
    /* If existing entry, use that, else get  a new one (possibly reusing the oldest) */
    if (p_i == NULL) {
        if ((p_i = btm_inq_db_new (bda)) != NULL) {
            p_inq->inq_cmpl_info.num_resp++;
        } else {
            return;
        }
    } else if (p_i->inq_count != p_inq->inq_counter) { /* first time seen in this inquiry */
        p_inq->inq_cmpl_info.num_resp++;
    }
    /* update the LE device information in inquiry database */
    if (!btm_ble_update_inq_result(p_i, addr_type, evt_type, p)) {
        return;
    }

    if ((result = btm_ble_is_discoverable(bda, evt_type, p)) == 0) {
        LOG_WARN("%s device is no longer discoverable so discarding advertising packet pkt",
                 __func__);
        return;
    }
    if (!update) {
        result &= ~BTM_BLE_INQ_RESULT;
    }
    /* If the number of responses found and limited, issue a cancel inquiry */
    if (p_inq->inqparms.max_resps &&
            p_inq->inq_cmpl_info.num_resp == p_inq->inqparms.max_resps) {
        /* new device */
        if (p_i == NULL ||
                /* assume a DUMO device, BR/EDR inquiry is always active */
                (p_i &&
                 (p_i->inq_info.results.device_type & BT_DEVICE_TYPE_BLE) == BT_DEVICE_TYPE_BLE &&
                 p_i->scan_rsp)) {
            BTM_TRACE_WARNING("INQ RES: Extra Response Received...cancelling inquiry..");

            /* if is non-periodic inquiry active, cancel now */
            if ((p_inq->inq_active & BTM_BR_INQ_ACTIVE_MASK) != 0 &&
                    (p_inq->inq_active & BTM_PERIODIC_INQUIRY_ACTIVE) == 0) {
                btsnd_hcic_inq_cancel();
            }

            btm_ble_stop_inquiry();

            btm_acl_update_busy_level (BTM_BLI_INQ_DONE_EVT);
        }
    }
    /* background connection in selective connection mode */
    if (btm_cb.ble_ctr_cb.bg_conn_type == BTM_BLE_CONN_SELECTIVE) {
        if (result & BTM_BLE_SEL_CONN_RESULT) {
            btm_send_sel_conn_callback(bda, evt_type, p, addr_type);
        } else {
            BTM_TRACE_DEBUG("None LE device, can not initiate selective connection\n");
        }
    } else {
        if (p_inq_results_cb && (result & BTM_BLE_INQ_RESULT)) {
            (p_inq_results_cb)((tBTM_INQ_RESULTS *) &p_i->inq_info.results, p_le_inq_cb->adv_data_cache);
        }
        if (p_obs_results_cb && (result & BTM_BLE_OBS_RESULT)) {
            (p_obs_results_cb)((tBTM_INQ_RESULTS *) &p_i->inq_info.results, p_le_inq_cb->adv_data_cache);
        }
    }
}

/*******************************************************************************
**
** Function         btm_ble_start_scan
**
** Description      Start the BLE scan.
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS btm_ble_start_scan(void)
{
    tBTM_BLE_INQ_CB *p_inq = &btm_cb.ble_ctr_cb.inq_var;
    tBTM_STATUS status = BTM_CMD_STARTED;

    if (p_inq->adv_mode != BTM_BLE_ADV_DISABLE) {
        status = BTM_NO_RESOURCES;
    } else {
        /* start scan, disable duplicate filtering */
        if (!btsnd_hcic_ble_set_scan_enable (BTM_BLE_SCAN_ENABLE, p_inq->scan_duplicate_filter)) {
            status = BTM_NO_RESOURCES;
        } else {
            btm_cb.ble_ctr_cb.inq_var.state = BTM_BLE_SCANNING;
            if (p_inq->scan_type == BTM_BLE_SCAN_MODE_ACTI) {
                btm_ble_set_topology_mask(BTM_BLE_STATE_ACTIVE_SCAN_BIT);
            } else {
                btm_ble_set_topology_mask(BTM_BLE_STATE_PASSIVE_SCAN_BIT);
            }
        }
    }
    return status;
}

/*******************************************************************************
**
** Function         btm_ble_stop_scan
**
** Description      Stop the BLE scan.
**
** Returns          void
**
*******************************************************************************/
void btm_ble_stop_scan(void)
{
    BTM_TRACE_EVENT ("btm_ble_stop_scan ");

    if (btm_cb.ble_ctr_cb.inq_var.adv_mode == BTM_BLE_ADV_DISABLE) {
        /* Clear the inquiry callback if set */
        btm_cb.ble_ctr_cb.inq_var.scan_type = BTM_BLE_SCAN_MODE_NONE;
        btm_cb.ble_ctr_cb.inq_var.state = BTM_BLE_STOP_SCAN;
        /* stop discovery now */
        btsnd_hcic_ble_set_scan_enable (BTM_BLE_SCAN_DISABLE, BTM_BLE_DUPLICATE_ENABLE);

        btm_update_scanner_filter_policy(SP_ADV_ALL);

        btm_cb.ble_ctr_cb.wl_state &= ~BTM_BLE_WL_SCAN;
    }
}
/*******************************************************************************
**
** Function         btm_ble_stop_inquiry
**
** Description      Stop the BLE Inquiry.
**
** Returns          void
**
*******************************************************************************/
void btm_ble_stop_inquiry(void)
{
    tBTM_INQUIRY_VAR_ST *p_inq = &btm_cb.btm_inq_vars;
    tBTM_BLE_CB *p_ble_cb = &btm_cb.ble_ctr_cb;

    btu_stop_timer (&p_ble_cb->inq_var.inq_timer_ent);

    p_ble_cb->scan_activity &=  ~BTM_BLE_INQUIRY_MASK;

    /* If no more scan activity, stop LE scan now */
    if (!BTM_BLE_IS_SCAN_ACTIVE(p_ble_cb->scan_activity)) {
        btm_ble_stop_scan();
    } else if ((p_ble_cb->inq_var.scan_interval != BTM_BLE_LOW_LATENCY_SCAN_INT) ||
               (p_ble_cb->inq_var.scan_window != BTM_BLE_LOW_LATENCY_SCAN_WIN)) {
        BTM_TRACE_DEBUG("%s: setting default params for ongoing observe", __FUNCTION__);
        btm_ble_stop_scan();
        btm_ble_start_scan();
    }

    /* If we have a callback registered for inquiry complete, call it */
    BTM_TRACE_DEBUG ("BTM Inq Compl Callback: status 0x%02x, num results %d",
                     p_inq->inq_cmpl_info.status, p_inq->inq_cmpl_info.num_resp);

    btm_process_inq_complete(HCI_SUCCESS, (UINT8)(p_inq->inqparms.mode & BTM_BLE_INQUIRY_MASK));
}

/*******************************************************************************
**
** Function         btm_ble_stop_observe
**
** Description      Stop the BLE Observe.
**
** Returns          void
**
*******************************************************************************/
static void btm_ble_stop_observe(void)
{
    tBTM_BLE_CB *p_ble_cb = & btm_cb.ble_ctr_cb;
    tBTM_CMPL_CB *p_obs_cb = p_ble_cb->p_obs_cmpl_cb;

    btu_stop_timer (&p_ble_cb->obs_timer_ent);

    p_ble_cb->scan_activity &= ~BTM_LE_OBSERVE_ACTIVE;

    p_ble_cb->p_obs_results_cb = NULL;
    p_ble_cb->p_obs_cmpl_cb = NULL;

    if (!BTM_BLE_IS_SCAN_ACTIVE(p_ble_cb->scan_activity)) {
        btm_ble_stop_scan();
    }

    if (p_obs_cb) {
        (p_obs_cb)((tBTM_INQUIRY_CMPL *) &btm_cb.btm_inq_vars.inq_cmpl_info);
    }
}
/*******************************************************************************
**
** Function         btm_ble_adv_states_operation
**
** Description      Set or clear adv states in topology mask
**
** Returns          operation status. TRUE if sucessful, FALSE otherwise.
**
*******************************************************************************/
typedef BOOLEAN (BTM_TOPOLOGY_FUNC_PTR)(tBTM_BLE_STATE_MASK);
static BOOLEAN btm_ble_adv_states_operation(BTM_TOPOLOGY_FUNC_PTR *p_handler, UINT8 adv_evt)
{
    BOOLEAN rt = FALSE;

    switch (adv_evt) {
    case BTM_BLE_CONNECT_EVT:
        rt  = (*p_handler)(BTM_BLE_STATE_CONN_ADV_BIT);
        break;

    case  BTM_BLE_NON_CONNECT_EVT:
        rt  = (*p_handler) (BTM_BLE_STATE_NON_CONN_ADV_BIT);
        break;
    case BTM_BLE_CONNECT_DIR_EVT:
        rt  =  (*p_handler) (BTM_BLE_STATE_HI_DUTY_DIR_ADV_BIT);
        break;

    case BTM_BLE_DISCOVER_EVT:
        rt  =  (*p_handler) (BTM_BLE_STATE_SCAN_ADV_BIT);
        break;

    case BTM_BLE_CONNECT_LO_DUTY_DIR_EVT:
        rt = (*p_handler) (BTM_BLE_STATE_LO_DUTY_DIR_ADV_BIT);
        break;

    default:
        BTM_TRACE_ERROR("unknown adv event : %d", adv_evt);
        break;
    }

    return rt;
}


/*******************************************************************************
**
** Function         btm_ble_start_adv
**
** Description      start the BLE advertising.
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS btm_ble_start_adv(void)
{
    tBTM_BLE_CB *p_ble_cb = & btm_cb.ble_ctr_cb;
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;
    tBTM_STATUS     rt = BTM_NO_RESOURCES;
    BTM_TRACE_EVENT ("btm_ble_start_adv\n");

    if (BTM_BLE_IS_OBS_ACTIVE(p_ble_cb->scan_activity)) {
        return BTM_NO_RESOURCES;
    }

    if (!btm_ble_adv_states_operation (btm_ble_topology_check, p_cb->evt_type)) {
        return BTM_WRONG_MODE;
    }

#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
    /* To relax resolving list,  always have resolving list enabled, unless directed adv */
    if (p_cb->evt_type != BTM_BLE_CONNECT_LO_DUTY_DIR_EVT &&
            p_cb->evt_type != BTM_BLE_CONNECT_DIR_EVT)
        /* enable resolving list is desired */
    {
        btm_ble_enable_resolving_list_for_platform(BTM_BLE_RL_ADV);
    }
#endif
    if (p_cb->afp != AP_SCAN_CONN_ALL) {
        btm_execute_wl_dev_operation();
        btm_cb.ble_ctr_cb.wl_state |= BTM_BLE_WL_ADV;
    }

    if (btsnd_hcic_ble_set_adv_enable (BTM_BLE_ADV_ENABLE)) {
        p_cb->adv_mode = BTM_BLE_ADV_ENABLE;
        p_cb->state = BTM_BLE_ADVERTISING;
        btm_ble_adv_states_operation(btm_ble_set_topology_mask, p_cb->evt_type);
        rt = BTM_SUCCESS;
        BTM_TRACE_EVENT ("BTM_SUCCESS\n");
    } else {
        p_cb->adv_mode = BTM_BLE_ADV_DISABLE;
        btm_cb.ble_ctr_cb.wl_state &= ~BTM_BLE_WL_ADV;
    }
    return rt;
}

/*******************************************************************************
**
** Function         btm_ble_stop_adv
**
** Description      Stop the BLE advertising.
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS btm_ble_stop_adv(void)
{
    tBTM_BLE_CB *p_ble_cb = & btm_cb.ble_ctr_cb;
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;
    tBTM_STATUS rt = BTM_SUCCESS;

    if (p_cb->adv_mode == BTM_BLE_ADV_ENABLE
            && !BTM_BLE_IS_OBS_ACTIVE(p_ble_cb->scan_activity)) {
        if (btsnd_hcic_ble_set_adv_enable (BTM_BLE_ADV_DISABLE)) {
            p_cb->fast_adv_on = FALSE;
            p_cb->adv_mode = BTM_BLE_ADV_DISABLE;
            p_cb->state = BTM_BLE_STOP_ADV;
            btm_cb.ble_ctr_cb.wl_state &= ~BTM_BLE_WL_ADV;

            /* clear all adv states */
            btm_ble_clear_topology_mask (BTM_BLE_STATE_ALL_ADV_MASK);
        } else {
            rt = BTM_NO_RESOURCES;
        }
    }
    return rt;
}


/*******************************************************************************
**
** Function         btm_ble_start_slow_adv
**
** Description      Restart adv with slow adv interval
**
** Returns          void
**
*******************************************************************************/
static void btm_ble_start_slow_adv (void)
{
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;

    if (p_cb->adv_mode == BTM_BLE_ADV_ENABLE) {
        tBTM_LE_RANDOM_CB *p_addr_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
        BD_ADDR p_addr_ptr = {0};
        tBLE_ADDR_TYPE init_addr_type = BLE_ADDR_PUBLIC;
        tBLE_ADDR_TYPE own_addr_type = p_addr_cb->own_addr_type;

        btm_ble_stop_adv();

        p_cb->evt_type = btm_set_conn_mode_adv_init_addr(p_cb, p_addr_ptr, &init_addr_type,
                         &own_addr_type);

        /* slow adv mode never goes into directed adv */
        btsnd_hcic_ble_write_adv_params (BTM_BLE_GAP_ADV_SLOW_INT, BTM_BLE_GAP_ADV_SLOW_INT,
                                         p_cb->evt_type, own_addr_type,
                                         init_addr_type, p_addr_ptr,
                                         p_cb->adv_chnl_map, p_cb->afp);

        btm_ble_start_adv();
    }
}
/*******************************************************************************
**
** Function         btm_ble_timeout
**
** Description      Called when BTM BLE inquiry timer expires
**
** Returns          void
**
*******************************************************************************/
void btm_ble_timeout(TIMER_LIST_ENT *p_tle)
{
    BTM_TRACE_EVENT ("btm_ble_timeout");

    switch (p_tle->event) {
    case BTU_TTYPE_BLE_OBSERVE:
        btm_ble_stop_observe();
        break;

    case BTU_TTYPE_BLE_INQUIRY:
        btm_ble_stop_inquiry();
        break;

    case BTU_TTYPE_BLE_GAP_LIM_DISC:
        /* lim_timeout expiried, limited discovery should exit now */
        btm_cb.btm_inq_vars.discoverable_mode &= ~BTM_BLE_LIMITED_DISCOVERABLE;
        btm_ble_set_adv_flag(btm_cb.btm_inq_vars.connectable_mode, btm_cb.btm_inq_vars.discoverable_mode);
        break;

    case BTU_TTYPE_BLE_RANDOM_ADDR:
        if (btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type == BLE_ADDR_RANDOM) {
            if (NULL == (void *)(p_tle->param)) {
                /* refresh the random addr */
                btm_gen_resolvable_private_addr((void *)btm_gen_resolve_paddr_low);
            } else {
                if (BTM_BleMaxMultiAdvInstanceCount() > 0) {
                    btm_ble_multi_adv_configure_rpa((tBTM_BLE_MULTI_ADV_INST *)p_tle->param);
                }
            }
        }
        break;

    case BTU_TTYPE_BLE_GAP_FAST_ADV:
        /* fast adv is completed, fall back to slow adv interval */
        btm_ble_start_slow_adv();
        break;

    default:
        break;

    }
}


/*******************************************************************************
**
** Function         btm_ble_read_remote_features_complete
**
** Description      This function is called when the command complete message
**                  is received from the HCI for the read LE remote feature supported
**                  complete event.
**
** Returns          void
**
*******************************************************************************/
void btm_ble_read_remote_features_complete(UINT8 *p)
{
    tACL_CONN        *p_acl_cb = &btm_cb.acl_db[0];
    UINT16            handle;
    UINT8             status;
    int               xx;

    BTM_TRACE_EVENT ("btm_ble_read_remote_features_complete ");

    STREAM_TO_UINT8(status, p);

    // if LE read remote feature failed for HCI_ERR_CONN_FAILED_ESTABLISHMENT,
    // expect disconnect complete to be received
    if (status != HCI_ERR_CONN_FAILED_ESTABLISHMENT) {
        STREAM_TO_UINT16 (handle, p);

        /* Look up the connection by handle and copy features */
        for (xx = 0; xx < MAX_L2CAP_LINKS; xx++, p_acl_cb++) {
            if ((p_acl_cb->in_use) && (p_acl_cb->hci_handle == handle)) {
                STREAM_TO_ARRAY(p_acl_cb->peer_le_features, p, BD_FEATURES_LEN);
                btsnd_hcic_rmt_ver_req (p_acl_cb->hci_handle);
                break;
            }
        }
    }

}

/*******************************************************************************
**
** Function         btm_ble_write_adv_enable_complete
**
** Description      This function process the write adv enable command complete.
**
** Returns          void
**
*******************************************************************************/
void btm_ble_write_adv_enable_complete(UINT8 *p)
{
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;

    /* if write adv enable/disbale not succeed */
    if (*p != HCI_SUCCESS) {
        /* toggle back the adv mode */
        p_cb->adv_mode = !p_cb->adv_mode;
    }
}

/*******************************************************************************
**
** Function         btm_ble_dir_adv_tout
**
** Description      when directed adv time out
**
** Returns          void
**
*******************************************************************************/
void btm_ble_dir_adv_tout(void)
{
    btm_cb.ble_ctr_cb.inq_var.adv_mode = BTM_BLE_ADV_DISABLE;

    /* make device fall back into undirected adv mode by default */
    btm_cb.ble_ctr_cb.inq_var.directed_conn = FALSE;
}

/*******************************************************************************
**
** Function         btm_ble_set_topology_mask
**
** Description      set BLE topology mask
**
** Returns          TRUE is request is allowed, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN btm_ble_set_topology_mask(tBTM_BLE_STATE_MASK request_state_mask)
{
    request_state_mask &= BTM_BLE_STATE_ALL_MASK;
    btm_cb.ble_ctr_cb.cur_states |= (request_state_mask & BTM_BLE_STATE_ALL_MASK);
    return TRUE;
}

/*******************************************************************************
**
** Function         btm_ble_clear_topology_mask
**
** Description      Clear BLE topology bit mask
**
** Returns          TRUE is request is allowed, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN btm_ble_clear_topology_mask (tBTM_BLE_STATE_MASK request_state_mask)
{
    request_state_mask &= BTM_BLE_STATE_ALL_MASK;
    btm_cb.ble_ctr_cb.cur_states &= ~request_state_mask;
    return TRUE;
}

/*******************************************************************************
**
** Function         btm_ble_update_link_topology_mask
**
** Description      This function update the link topology mask
**
** Returns          void
**
*******************************************************************************/
void btm_ble_update_link_topology_mask(UINT8 link_role, BOOLEAN increase)
{
    btm_ble_clear_topology_mask (BTM_BLE_STATE_ALL_CONN_MASK);

    if (increase) {
        btm_cb.ble_ctr_cb.link_count[link_role]++;
    } else if (btm_cb.ble_ctr_cb.link_count[link_role] > 0) {
        btm_cb.ble_ctr_cb.link_count[link_role]--;
    }

    if (btm_cb.ble_ctr_cb.link_count[HCI_ROLE_MASTER]) {
        btm_ble_set_topology_mask (BTM_BLE_STATE_MASTER_BIT);
    }

    if (btm_cb.ble_ctr_cb.link_count[HCI_ROLE_SLAVE]) {
        btm_ble_set_topology_mask(BTM_BLE_STATE_SLAVE_BIT);
    }

    if (link_role == HCI_ROLE_SLAVE && increase) {
        btm_cb.ble_ctr_cb.inq_var.adv_mode = BTM_BLE_ADV_DISABLE;
        /* make device fall back into undirected adv mode by default */
        btm_cb.ble_ctr_cb.inq_var.directed_conn = BTM_BLE_CONNECT_EVT;
        /* clear all adv states */
        btm_ble_clear_topology_mask(BTM_BLE_STATE_ALL_ADV_MASK);
    }
}

/*******************************************************************************
**
** Function         btm_ble_update_mode_operation
**
** Description      This function update the GAP role operation when a link status
**                  is updated.
**
** Returns          void
**
*******************************************************************************/
void btm_ble_update_mode_operation(UINT8 link_role, BD_ADDR bd_addr, UINT8 status)
{
    if (status == HCI_ERR_DIRECTED_ADVERTISING_TIMEOUT) {
        btm_cb.ble_ctr_cb.inq_var.adv_mode  = BTM_BLE_ADV_DISABLE;
        /* make device fall back into undirected adv mode by default */
        btm_cb.ble_ctr_cb.inq_var.directed_conn = BTM_BLE_CONNECT_EVT;
        /* clear all adv states */
        btm_ble_clear_topology_mask (BTM_BLE_STATE_ALL_ADV_MASK);
    }

    if (btm_cb.ble_ctr_cb.inq_var.connectable_mode == BTM_BLE_CONNECTABLE) {
        btm_ble_set_connectability(btm_cb.btm_inq_vars.connectable_mode |
                                   btm_cb.ble_ctr_cb.inq_var.connectable_mode);
    }

    /* when no connection is attempted, and controller is not rejecting last request
       due to resource limitation, start next direct connection or background connection
       now in order */
    if (btm_ble_get_conn_st() == BLE_CONN_IDLE && status != HCI_ERR_HOST_REJECT_RESOURCES &&
            !btm_send_pending_direct_conn()) {
        btm_ble_resume_bg_conn();
    }
}

/*******************************************************************************
**
** Function         btm_ble_init
**
** Description      Initialize the control block variable values.
**
** Returns          void
**
*******************************************************************************/
void btm_ble_init (void)
{
    tBTM_BLE_CB *p_cb = &btm_cb.ble_ctr_cb;

    BTM_TRACE_DEBUG("%s", __func__);

    memset(p_cb, 0, sizeof(tBTM_BLE_CB));
    memset(&(btm_cb.cmn_ble_vsc_cb), 0 , sizeof(tBTM_BLE_VSC_CB));
    btm_cb.cmn_ble_vsc_cb.values_read = FALSE;
    p_cb->cur_states       = 0;

    p_cb->inq_var.adv_mode = BTM_BLE_ADV_DISABLE;
    p_cb->inq_var.scan_type = BTM_BLE_SCAN_MODE_NONE;
    p_cb->inq_var.adv_chnl_map = BTM_BLE_DEFAULT_ADV_CHNL_MAP;
    p_cb->inq_var.afp = BTM_BLE_DEFAULT_AFP;
    p_cb->inq_var.sfp = BTM_BLE_DEFAULT_SFP;
    p_cb->inq_var.connectable_mode = BTM_BLE_NON_CONNECTABLE;
    p_cb->inq_var.discoverable_mode = BTM_BLE_NON_DISCOVERABLE;

    /* for background connection, reset connection params to be undefined */
    p_cb->scan_int = p_cb->scan_win = BTM_BLE_SCAN_PARAM_UNDEF;

    p_cb->inq_var.evt_type = BTM_BLE_NON_CONNECT_EVT;

#if BLE_VND_INCLUDED == FALSE
    btm_ble_adv_filter_init();
#endif
}

/*******************************************************************************
**
** Function         btm_ble_topology_check
**
** Description      check to see requested state is supported. One state check at
**                  a time is supported
**
** Returns          TRUE is request is allowed, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN btm_ble_topology_check(tBTM_BLE_STATE_MASK request_state_mask)
{
    BOOLEAN rt = FALSE;

    UINT8   state_offset = 0;
    UINT16  cur_states = btm_cb.ble_ctr_cb.cur_states;
    UINT8   mask, offset;
    UINT8   request_state = 0;

    /* check only one bit is set and within valid range */
    if (request_state_mask == BTM_BLE_STATE_INVALID ||
            request_state_mask > BTM_BLE_STATE_SCAN_ADV_BIT ||
            (request_state_mask & (request_state_mask - 1 )) != 0) {
        BTM_TRACE_ERROR("illegal state requested: %d", request_state_mask);
        return rt;
    }

    while (request_state_mask) {
        request_state_mask >>= 1;
        request_state ++;
    }

    /* check if the requested state is supported or not */
    mask = btm_le_state_combo_tbl[0][request_state - 1][0];
    offset = btm_le_state_combo_tbl[0][request_state - 1][1];

    const uint8_t *ble_supported_states = controller_get_interface()->get_ble_supported_states();

    if (!BTM_LE_STATES_SUPPORTED(ble_supported_states, mask, offset)) {
        BTM_TRACE_ERROR("state requested not supported: %d", request_state);
        return rt;
    }

    rt = TRUE;
    /* make sure currently active states are all supported in conjunction with the requested
       state. If the bit in table is not set, the combination is not supported */
    while (cur_states != 0) {
        if (cur_states & 0x01) {
            mask = btm_le_state_combo_tbl[request_state][state_offset][0];
            offset = btm_le_state_combo_tbl[request_state][state_offset][1];

            if (mask != 0 && offset != 0) {
                if (!BTM_LE_STATES_SUPPORTED(ble_supported_states, mask, offset)) {
                    rt = FALSE;
                    break;
                }
            }
        }
        cur_states >>= 1;
        state_offset ++;
    }
    return rt;
}


#endif  /* BLE_INCLUDED */

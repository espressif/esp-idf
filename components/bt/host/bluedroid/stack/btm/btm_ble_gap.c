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

#include "stack/bt_types.h"
//#include "bt_utils.h"
#include "btm_int.h"
#include "stack/btm_ble_api.h"
#include "stack/btu.h"
#include "device/controller.h"
#include "stack/hcimsgs.h"
#include "stack/gap_api.h"
#include "hci/hci_layer.h"
#if BLE_INCLUDED == TRUE
#include "l2c_int.h"

#include "stack/gattdefs.h"
#include "gatt_int.h"

#include "btm_ble_int.h"
//#define LOG_TAG "bt_btm_ble"
//#include "osi/include/log.h"
#include "osi/osi.h"
#include "osi/mutex.h"

#define BTM_BLE_NAME_SHORT                  0x01
#define BTM_BLE_NAME_CMPL                   0x02

#define BTM_BLE_FILTER_TARGET_UNKNOWN       0xff
#define BTM_BLE_POLICY_UNKNOWN              0xff

#define BTM_EXT_BLE_RMT_NAME_TIMEOUT        30
#define MIN_ADV_LENGTH                       2
#define BTM_VSC_CHIP_CAPABILITY_RSP_LEN_L_RELEASE 9

#define BTM_BLE_GAP_ADV_RPT_BATCH_SIZE           (10)


tBTM_CallbackFunc conn_callback_func;
// BLE vendor HCI event callback
#if (BLE_VENDOR_HCI_EN == TRUE)
static tBTM_BLE_VENDOR_HCI_EVT_CBACK *ble_vs_evt_callback = NULL;
#endif // #if (BLE_VENDOR_HCI_EN == TRUE)
/*******************************************************************************
**  Local functions
*******************************************************************************/
static void btm_ble_update_adv_flag(UINT8 flag);
#if (BLE_42_SCAN_EN == TRUE)
static void btm_ble_process_adv_pkt_cont(BD_ADDR bda, UINT8 addr_type, UINT8 evt_type, UINT8 *p);
#endif // (BLE_42_SCAN_EN == TRUE)

UINT8 *btm_ble_build_adv_data(tBTM_BLE_AD_MASK *p_data_mask, UINT8 **p_dst,
                              tBTM_BLE_ADV_DATA *p_data);

static UINT8 btm_set_conn_mode_adv_init_addr(tBTM_BLE_INQ_CB *p_cb,
        BD_ADDR_PTR p_peer_addr_ptr,
        tBLE_ADDR_TYPE *p_peer_addr_type,
        tBLE_ADDR_TYPE *p_own_addr_type);
static void btm_ble_stop_discover(void);
#if (BLE_42_SCAN_EN == TRUE)
static void btm_adv_pkt_handler(void *arg);
#endif // #if (BLE_42_SCAN_EN == TRUE)
uint32_t BTM_BleUpdateOwnType(uint8_t *own_bda_type, tBTM_START_ADV_CMPL_CBACK *cb);

#define BTM_BLE_INQ_RESULT          0x01
#define BTM_BLE_OBS_RESULT          0x02
#define BTM_BLE_SEL_CONN_RESULT     0x04
#define BTM_BLE_DISCO_RESULT        0x08

static bool is_ble50_inter = false;

void btm_ble_inter_set(bool extble_inter)
{
    is_ble50_inter = extble_inter;
}

bool btm_ble_inter_get(void)
{
    return is_ble50_inter;
}

#if (BLE_TOPOLOGY_CHECK == TRUE)
/* LE states combo bit to check */
const UINT8 btm_le_state_combo_tbl[BTM_BLE_STATE_MAX][BTM_BLE_STATE_MAX][2] = {
    {/* single state support */
        {HCI_SUPP_LE_STATES_CONN_ADV_MASK, HCI_SUPP_LE_STATES_CONN_ADV_OFF},  /* conn_adv */
        {HCI_SUPP_LE_STATES_INIT_MASK, HCI_SUPP_LE_STATES_INIT_OFF}, /* init */
        {HCI_SUPP_LE_STATES_INIT_MASK, HCI_SUPP_LE_STATES_INIT_OFF}, /* master */
        {HCI_SUPP_LE_STATES_SLAVE_MASK, HCI_SUPP_LE_STATES_SLAVE_OFF}, /* slave */
        {HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_MASK, HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_OFF}, /* lo du dir adv */
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
        {HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_PASS_SCAN_MASK, HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_PASS_SCAN_OFF},   /* passive scan */
        {HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_ACTIVE_SCAN_MASK, HCI_SUPP_LE_STATES_LO_DUTY_DIR_ADV_ACTIVE_SCAN_OFF},  /* active scan */
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
#endif // (BLE_TOPOLOGY_CHECK == TRUE)

#if (BLE_42_ADV_EN == TRUE)
static osi_mutex_t adv_enable_lock;
static osi_mutex_t adv_data_lock;
static osi_mutex_t adv_param_lock;
osi_sem_t adv_enable_sem;
osi_sem_t adv_data_sem;
osi_sem_t adv_param_sem;
uint8_t adv_enable_status  = 0;
uint8_t adv_data_status    = 0;
uint8_t adv_param_status   = 0;
#endif // #if (BLE_42_ADV_EN == TRUE)

#if (BLE_42_SCAN_EN == TRUE)
static osi_mutex_t scan_enable_lock;
static osi_mutex_t scan_param_lock;
osi_sem_t scan_enable_sem;
osi_sem_t scan_param_sem;
uint8_t scan_enable_status = 0;
uint8_t scan_param_status  = 0;
#endif // #if (BLE_42_SCAN_EN == TRUE)

void btm_ble_lock_init(void)
{
#if (BLE_42_ADV_EN == TRUE)
    osi_mutex_new(&adv_enable_lock);
    osi_mutex_new(&adv_data_lock);
    osi_mutex_new(&adv_param_lock);
#endif // #if (BLE_42_ADV_EN == TRUE)
#if (BLE_42_SCAN_EN == TRUE)
    osi_mutex_new(&scan_enable_lock);
    osi_mutex_new(&scan_param_lock);
#endif // #if (BLE_42_SCAN_EN == TRUE)
}

void btm_ble_lock_free(void)
{
#if (BLE_42_ADV_EN == TRUE)
    osi_mutex_free(&adv_enable_lock);
    osi_mutex_free(&adv_data_lock);
    osi_mutex_free(&adv_param_lock);
#endif // #if (BLE_42_ADV_EN == TRUE)
#if (BLE_42_SCAN_EN == TRUE)
    osi_mutex_free(&scan_enable_lock);
    osi_mutex_free(&scan_param_lock);
#endif // #if (BLE_42_SCAN_EN == TRUE)
}

void btm_ble_sem_init(void)
{
#if (BLE_42_ADV_EN == TRUE)
    osi_sem_new(&adv_enable_sem, 1, 0);
    osi_sem_new(&adv_data_sem, 1, 0);
    osi_sem_new(&adv_param_sem, 1, 0);
#endif // #if (BLE_42_ADV_EN == TRUE)
#if (BLE_42_SCAN_EN == TRUE)
    osi_sem_new(&scan_enable_sem, 1, 0);
    osi_sem_new(&scan_param_sem, 1, 0);
#endif // #if (BLE_42_SCAN_EN == TRUE)
}

void btm_ble_sem_free(void)
{
#if (BLE_42_ADV_EN == TRUE)
    osi_sem_free(&adv_enable_sem);
    osi_sem_free(&adv_data_sem);
    osi_sem_free(&adv_param_sem);
#endif // #if (BLE_42_ADV_EN == TRUE)
#if (BLE_42_SCAN_EN == TRUE)
    osi_sem_free(&scan_enable_sem);
    osi_sem_free(&scan_param_sem);
#endif // #if (BLE_42_SCAN_EN == TRUE)
}

/*******************************************************************************
**
** Function         BTM_BleRegiseterConnParamCallback
**
** Description      register connection parameters update callback func
**
** Returns          void
**
*******************************************************************************/
void BTM_BleRegiseterConnParamCallback(tBTM_UPDATE_CONN_PARAM_CBACK *update_conn_param_cb)
{
    conn_callback_func.update_conn_param_cb = update_conn_param_cb;
}

/*******************************************************************************
**
** Function         BTM_BleRegiseterPktLengthChangeCallback
**
** Description      Registers a callback function for packet length changes.
**
** Returns          void
**
*******************************************************************************/
void BTM_BleRegiseterPktLengthChangeCallback(tBTM_SET_PKT_DATA_LENGTH_CBACK *ptk_len_chane_cb)
{
    conn_callback_func.set_pkt_data_length_cb = ptk_len_chane_cb;
}

#if (BLE_VENDOR_HCI_EN == TRUE)
void BTM_BleRegisterVendorHciEventCallback(tBTM_BLE_VENDOR_HCI_EVT_CBACK *vendor_hci_evt_cb)
{
    ble_vs_evt_callback = vendor_hci_evt_cb;
}
#endif // #if (BLE_VENDOR_HCI_EN == TRUE)

/*******************************************************************************
**
** Function         BTM_BleUpdateAdvWhitelist
**
** Description      Add or remove device from advertising white list
**
** Returns          void
**
*******************************************************************************/
BOOLEAN BTM_BleUpdateAdvWhitelist(BOOLEAN add_remove, BD_ADDR remote_bda, tBLE_ADDR_TYPE addr_type, tBTM_UPDATE_WHITELIST_CBACK *update_wl_cb)
{
    return btm_update_dev_to_white_list(add_remove, remote_bda, addr_type, update_wl_cb);
}

/*******************************************************************************
**
** Function         BTM_BleUpdateAdvWhitelist
**
** Description      Add or remove device from advertising white list
**
** Returns          void
**
*******************************************************************************/
void BTM_BleClearWhitelist(tBTM_UPDATE_WHITELIST_CBACK *update_wl_cb)
{
   btm_ble_clear_white_list(update_wl_cb);
}

#if (BLE_42_SCAN_EN == TRUE)
/*******************************************************************************
**
** Function         BTM_BleScan
**
** Description      This procedure keep the device listening for advertising
**                  events from a broadcast device.
**
** Parameters       start: start or stop scan.
**                  white_list: use white list in observer mode or not.
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS BTM_BleScan(BOOLEAN start, UINT32 duration,
                           tBTM_INQ_RESULTS_CB *p_results_cb, tBTM_CMPL_CB *p_cmpl_cb, tBTM_INQ_DIS_CB *p_discard_cb)
{
    tBTM_BLE_INQ_CB *p_inq = &btm_cb.ble_ctr_cb.inq_var;
    tBTM_STATUS status = BTM_WRONG_MODE;

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

    if (start) {
        /* shared inquiry database, do not allow scan if any inquiry is active */
        if (BTM_BLE_IS_DISCO_ACTIVE(btm_cb.ble_ctr_cb.scan_activity)) {
            BTM_TRACE_ERROR("%s scan already active", __func__);
            return status;
        }

        btm_cb.ble_ctr_cb.p_scan_results_cb = p_results_cb;
        btm_cb.ble_ctr_cb.p_scan_cmpl_cb = p_cmpl_cb;
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
        btm_cb.ble_ctr_cb.p_obs_discard_cb = p_discard_cb;
#endif // (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
        status = BTM_CMD_STARTED;

        /* scan is not started */
        if (!BTM_BLE_IS_SCAN_ACTIVE(btm_cb.ble_ctr_cb.scan_activity)) {
            /* assume observe always not using white list */
#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
            /* enable resolving list */
            //btm_ble_enable_resolving_list_for_platform(BTM_BLE_RL_SCAN);
#endif
            // if not set scan params, set default scan params
            if (!p_inq->scan_params_set) {
                /* allow config of scan type */
                p_inq->scan_type = BTM_BLE_SCAN_MODE_ACTI;
                p_inq->scan_interval = BTM_BLE_GAP_DISC_SCAN_INT;
                p_inq->scan_window = BTM_BLE_GAP_DISC_SCAN_WIN;
                p_inq->sfp = BTM_BLE_DEFAULT_SFP;
                p_inq->scan_params_set = TRUE;
                p_inq->scan_duplicate_filter = BTM_BLE_DUPLICATE_DISABLE;
                btsnd_hcic_ble_set_scan_params(p_inq->scan_type, p_inq->scan_interval,
                                               p_inq->scan_window,
                                               btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type,
                                               p_inq->sfp);
            }
            status = btm_ble_start_scan();
        }

        if (status == BTM_CMD_STARTED) {
            btm_cb.ble_ctr_cb.scan_activity |= BTM_LE_DISCOVER_ACTIVE;
            if (duration != 0)
                /* start observer timer */
            {
                btu_start_timer (&btm_cb.ble_ctr_cb.scan_timer_ent, BTU_TTYPE_BLE_SCAN, duration);
            }
        }
    } else if (BTM_BLE_IS_DISCO_ACTIVE(btm_cb.ble_ctr_cb.scan_activity)) {
        status = BTM_CMD_STARTED;
        btm_ble_stop_discover();
    } else {
        BTM_TRACE_ERROR("%s scan not active\n", __func__);
    }

    return status;

}
#endif // #if (BLE_42_SCAN_EN == TRUE)

#if (BLE_42_ADV_EN == TRUE)
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
tBTM_STATUS BTM_BleBroadcast(BOOLEAN start, tBTM_START_STOP_ADV_CMPL_CBACK  *p_stop_adv_cback)
{
    tBTM_STATUS status = BTM_NO_RESOURCES;
    // tBTM_LE_RANDOM_CB *p_addr_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;
    //UINT8 evt_type = p_cb->scan_rsp ? BTM_BLE_DISCOVER_EVT : BTM_BLE_NON_CONNECT_EVT;

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

#ifdef  BTM_BLE_PC_ADV_TEST_MODE
    if (BTM_BLE_PC_ADV_TEST_MODE) {
        evt_type = p_cb->scan_rsp ? BTM_BLE_CONNECT_EVT : BTM_BLE_NON_CONNECT_EVT;
    }
#endif

    if (start) {
// "start" should not be true
#if (0)
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
#endif //
    } else {
        //save the stop adv callback to the BTM env.
        p_cb->p_stop_adv_cb = p_stop_adv_cback;
        status = btm_ble_stop_adv();
#if BLE_PRIVACY_SPT == TRUE
        btm_ble_disable_resolving_list(BTM_BLE_RL_ADV, TRUE);
#endif
    }
    return status;
}
#endif // #if (BLE_42_ADV_EN == TRUE)

#if (BLE_VENDOR_HCI_EN == TRUE)
void BTM_VendorHciEchoCmdCallback(tBTM_VSC_CMPL *p1)
{
#if (!CONFIG_BT_STACK_NO_LOG)
    if (!p1) {
        return;
    }
    uint8_t *p = p1->p_param_buf;
    uint8_t status, echo;
    STREAM_TO_UINT8  (status, p);
    STREAM_TO_UINT8  (echo, p);
#endif
    BTM_TRACE_DEBUG("%s status 0x%x echo 0x%x", __func__, status, echo);
}

/******************************************************************************
**
** Function         BTM_VendorHciEchoCmdTest
**
** Description      vendor common echo hci cmd test, controller will return status and echo
**
** Parameters:      echo : echo value
**
** Returns          void
**
*******************************************************************************/
void BTM_VendorHciEchoCmdTest(uint8_t echo)
{
    BTM_VendorSpecificCommand (HCI_VENDOR_COMMON_ECHO_CMD_OPCODE,
                                1,
                                &echo,
                                BTM_VendorHciEchoCmdCallback);
}
#endif // (BLE_VENDOR_HCI_EN == TRUE)

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
BOOLEAN BTM_BleConfigPrivacy(BOOLEAN privacy_mode, tBTM_SET_LOCAL_PRIVACY_CBACK *set_local_privacy_cback)
{
#if BLE_PRIVACY_SPT == TRUE
    tBTM_BLE_CB  *p_cb = &btm_cb.ble_ctr_cb;
    tBTM_LE_RANDOM_CB *random_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    if (random_cb){
        random_cb->set_local_privacy_cback = set_local_privacy_cback;
    }else{
        BTM_TRACE_ERROR("%s,random_cb = NULL", __func__);
    }

    BTM_TRACE_EVENT ("%s\n", __func__);

    /* if LE is not supported, return error */
    if (!controller_get_interface()->supports_ble()) {
        return FALSE;
    }

#if (defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE && GATTS_INCLUDED == TRUE)
    uint8_t addr_resolution = 0;
#endif  /* defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE && GATTS_INCLUDED == TRUE */
    if (!privacy_mode) { /* if privacy disabled, always use public address */
        p_cb->addr_mgnt_cb.exist_addr_bit &= (~BTM_BLE_GAP_ADDR_BIT_RESOLVABLE);
        memset(p_cb->addr_mgnt_cb.resolvale_addr, 0, BD_ADDR_LEN);
        p_cb->addr_mgnt_cb.own_addr_type = BLE_ADDR_PUBLIC;
        p_cb->privacy_mode = BTM_PRIVACY_NONE;
        // Disable RPA function
        btsnd_hcic_ble_set_addr_resolution_enable(FALSE);
    } else { /* privacy is turned on*/
#if (CONTROLLER_RPA_LIST_ENABLE == FALSE)
        /* always set host random address, used when privacy 1.1 or priavcy 1.2 is disabled */
        btm_gen_resolvable_private_addr((void *)btm_gen_resolve_paddr_low);
#endif

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
        // Enable RPA function
        btsnd_hcic_ble_set_addr_resolution_enable(TRUE);
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
** Function         BTMGetLocalResolvablePrivateAddr
**
** Description      This function is called to get local RPA address
**
** Parameters       bda: address pointer.
**
**
*******************************************************************************/

BOOLEAN BTM_GetLocalResolvablePrivateAddr(BD_ADDR bda)
{
    tBTM_LE_RANDOM_CB *p_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    BTM_TRACE_DEBUG ("get owm resolvable random address");

    if (bda) {
        /* if privacy disabled, return false */
        if ((p_cb->exist_addr_bit & BTM_BLE_GAP_ADDR_BIT_RESOLVABLE) == BTM_BLE_GAP_ADDR_BIT_RESOLVABLE) {
            memcpy(bda, p_cb->resolvale_addr, BD_ADDR_LEN);
            BTM_TRACE_DEBUG("own resolvable random address: 0x%02x:%02x:%02x:%02x:%02x:%02x",
                            p_cb->resolvale_addr[0], p_cb->resolvale_addr[1],
                            p_cb->resolvale_addr[2], p_cb->resolvale_addr[3],
                            p_cb->resolvale_addr[4], p_cb->resolvale_addr[5]);
            return TRUE;
        }

        return FALSE;
    }

   return FALSE;
}

/*******************************************************************************
**
** Function         BTM_UpdateAddrInfor
**
** Description      This function is called to update address information
**
** Parameters       addr_type: address type
**                  bda: address pointer.
**
**
*******************************************************************************/
void BTM_UpdateAddrInfor(uint8_t addr_type, BD_ADDR bda)
{
    btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type = addr_type;
    memcpy(btm_cb.ble_ctr_cb.addr_mgnt_cb.private_addr, bda, BD_ADDR_LEN);
}

/*******************************************************************************
**
** Function         BTM_BleSetStaticAddr
**
** Description      This function is called to save random address
**
** Parameters       rand_addr: address pointer.
**
**
*******************************************************************************/
void BTM_BleSetStaticAddr(BD_ADDR rand_addr)
{
    memcpy(btm_cb.ble_ctr_cb.addr_mgnt_cb.static_rand_addr, rand_addr, BD_ADDR_LEN);
    btm_cb.ble_ctr_cb.addr_mgnt_cb.exist_addr_bit |= BTM_BLE_GAP_ADDR_BIT_RANDOM;
}

#if (CONTROLLER_RPA_LIST_ENABLE == FALSE)
uint32_t BTM_BleUpdateOwnType(uint8_t *own_bda_type, tBTM_START_ADV_CMPL_CBACK *cb)
{
    if(*own_bda_type == BLE_ADDR_RANDOM) {
        if((btm_cb.ble_ctr_cb.addr_mgnt_cb.exist_addr_bit & BTM_BLE_GAP_ADDR_BIT_RANDOM) == BTM_BLE_GAP_ADDR_BIT_RANDOM) {
            //close privacy
            #if BLE_PRIVACY_SPT == TRUE
            if (btm_cb.ble_ctr_cb.privacy_mode != BTM_PRIVACY_NONE) {
                BTM_BleConfigPrivacy(FALSE, NULL);
            }
            #endif
            btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type = BLE_ADDR_RANDOM;
            memcpy(btm_cb.ble_ctr_cb.addr_mgnt_cb.private_addr, btm_cb.ble_ctr_cb.addr_mgnt_cb.static_rand_addr, BD_ADDR_LEN);
            // set address to controller
            btsnd_hcic_ble_set_random_addr(btm_cb.ble_ctr_cb.addr_mgnt_cb.static_rand_addr);

        } else if((btm_cb.ble_ctr_cb.addr_mgnt_cb.exist_addr_bit & BTM_BLE_GAP_ADDR_BIT_RESOLVABLE) == BTM_BLE_GAP_ADDR_BIT_RESOLVABLE) {
            btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type = BLE_ADDR_RANDOM;
            memcpy(btm_cb.ble_ctr_cb.addr_mgnt_cb.private_addr, btm_cb.ble_ctr_cb.addr_mgnt_cb.resolvale_addr, BD_ADDR_LEN);
            btsnd_hcic_ble_set_random_addr(btm_cb.ble_ctr_cb.addr_mgnt_cb.resolvale_addr);
        }else {
            BTM_TRACE_ERROR ("No random address yet, please set random address using API \"esp_ble_gap_set_rand_addr\" and retry\n");
            if(cb) {
                (* cb)(HCI_ERR_ESP_VENDOR_FAIL);
            }
            return BTM_ILLEGAL_VALUE;
        }
    } else if(*own_bda_type == BLE_ADDR_PUBLIC_ID || *own_bda_type == BLE_ADDR_RANDOM_ID) {
        if((btm_cb.ble_ctr_cb.addr_mgnt_cb.exist_addr_bit & BTM_BLE_GAP_ADDR_BIT_RESOLVABLE) == BTM_BLE_GAP_ADDR_BIT_RESOLVABLE) {
            *own_bda_type = BLE_ADDR_RANDOM;
            btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type = BLE_ADDR_RANDOM;
            memcpy(btm_cb.ble_ctr_cb.addr_mgnt_cb.private_addr, btm_cb.ble_ctr_cb.addr_mgnt_cb.resolvale_addr, BD_ADDR_LEN);
            btsnd_hcic_ble_set_random_addr(btm_cb.ble_ctr_cb.addr_mgnt_cb.resolvale_addr);
        } else {
            #if BLE_PRIVACY_SPT == TRUE
            if(btm_cb.ble_ctr_cb.privacy_mode != BTM_PRIVACY_NONE) {
                BTM_TRACE_ERROR ("Error state\n");
                if(cb) {
                    (* cb)(HCI_ERR_ESP_VENDOR_FAIL);
                }
                return BTM_ILLEGAL_VALUE;
            }
            #endif
            if(*own_bda_type == BLE_ADDR_PUBLIC_ID) {
                *own_bda_type = BLE_ADDR_PUBLIC;
                btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type = BLE_ADDR_PUBLIC;
            } else { //own_bda_type == BLE_ADDR_RANDOM_ID
                if((btm_cb.ble_ctr_cb.addr_mgnt_cb.exist_addr_bit & BTM_BLE_GAP_ADDR_BIT_RANDOM) == BTM_BLE_GAP_ADDR_BIT_RANDOM) {
                    *own_bda_type = BLE_ADDR_RANDOM;
                    btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type = BLE_ADDR_RANDOM;
                    memcpy(btm_cb.ble_ctr_cb.addr_mgnt_cb.private_addr, btm_cb.ble_ctr_cb.addr_mgnt_cb.static_rand_addr, BD_ADDR_LEN);
                    btsnd_hcic_ble_set_random_addr(btm_cb.ble_ctr_cb.addr_mgnt_cb.static_rand_addr);
                } else {
                    BTM_TRACE_ERROR ("No RPA and no random address yet, please set RPA or random address and try\n");
                    if(cb) {
                        (* cb)(HCI_ERR_ESP_VENDOR_FAIL);
                    }
                    return BTM_ILLEGAL_VALUE;
                }
            }
        }
    } else {
        btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type = BLE_ADDR_PUBLIC;
    }

    return BTM_SUCCESS;
}
#else
uint32_t BTM_BleUpdateOwnType(uint8_t *own_bda_type, tBTM_START_ADV_CMPL_CBACK *cb)
{
    tBTM_LE_RANDOM_CB *p_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;

    if((*own_bda_type == BLE_ADDR_RANDOM) || (*own_bda_type == BLE_ADDR_RANDOM_ID)) {
        if((p_cb->exist_addr_bit & BTM_BLE_GAP_ADDR_BIT_RANDOM) != BTM_BLE_GAP_ADDR_BIT_RANDOM) {
            BTM_TRACE_ERROR("No random address yet, please set random address and try\n");
            if(cb) {
                (* cb)(HCI_ERR_ESP_VENDOR_FAIL);
            }
            return BTM_ILLEGAL_VALUE;
        }

        // If a device is using RPA, it shall also have an Identity Address
        if ((*own_bda_type == BLE_ADDR_RANDOM_ID) && BTM_BLE_IS_NON_RESLVE_BDA(p_cb->static_rand_addr)) {
            BTM_TRACE_ERROR("No identity address yet, please set static random address and try\n");
            if (cb) {
                (* cb)(HCI_ERR_ESP_VENDOR_FAIL);
            }
            return BTM_ILLEGAL_VALUE;
        }
    }

    p_cb->own_addr_type = *own_bda_type;

    return BTM_SUCCESS;
}
#endif


/*******************************************************************************
**
** Function         BTM_BleConfigLocalIcon
**
** Description      This function is called to set local icon
**
** Parameters       icon:  appearance value.
**
**
*******************************************************************************/
void BTM_BleConfigLocalIcon(uint16_t icon)
{
#if (defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE && GATTS_INCLUDED == TRUE)
    tGAP_BLE_ATTR_VALUE p_value;
    p_value.icon = icon;
    GAP_BleAttrDBUpdate(GATT_UUID_GAP_ICON, &p_value);
#else
    BTM_TRACE_ERROR("%s\n", __func__);
#endif
}

#if (BT_GATTS_KEY_MATERIAL_CHAR == TRUE)
/*******************************************************************************
**
** Function         BTM_BleSetKeyMaterial
**
** Description      Set the Encrypted Data Key Material in GAP service
**
** Parameters       session_key: 16-byte session key (must not be NULL)
**                  iv:          8-byte initialization vector (must not be NULL)
**
** Returns          void
**
*******************************************************************************/
void BTM_BleSetKeyMaterial(const uint8_t *session_key, const uint8_t *iv)
{
#if (defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE && GATTS_INCLUDED == TRUE)
    tGAP_BLE_ATTR_VALUE p_value;

    if (session_key == NULL || iv == NULL) {
        BTM_TRACE_ERROR("%s: NULL pointer parameter", __func__);
        return;
    }

    memset(&p_value, 0, sizeof(tGAP_BLE_ATTR_VALUE));
    memcpy(p_value.key_material.session_key, session_key, GAP_KEY_MATERIAL_SESSION_KEY_SIZE);
    memcpy(p_value.key_material.iv, iv, GAP_KEY_MATERIAL_IV_SIZE);
    GAP_BleAttrDBUpdate(GATT_UUID_GAP_KEY_MATERIAL, &p_value);
#else
    BTM_TRACE_ERROR("%s\n", __func__);
#endif
}
#endif

/*******************************************************************************
**
** Function         BTM_BleConfigConnParams
**
** Description      This function is called to set the connection parameters
**
** Parameters       int_min:  minimum connection interval
**                  int_max:  maximum connection interval
**                  latency:  slave latency
**                  timeout:  supervision timeout
**
*******************************************************************************/
void BTM_BleConfigConnParams(uint16_t int_min, uint16_t int_max, uint16_t latency, uint16_t timeout)
{
#if (defined(GAP_INCLUDED) && GAP_INCLUDED == TRUE && GATTS_INCLUDED == TRUE)
    tGAP_BLE_ATTR_VALUE p_value;

    p_value.conn_param.int_min = int_min;
    p_value.conn_param.int_max = int_max;
    p_value.conn_param.latency = latency;
    p_value.conn_param.sp_tout = timeout;
    GAP_BleAttrDBUpdate(GATT_UUID_GAP_PREF_CONN_PARAM, &p_value);
#else
    BTM_TRACE_ERROR("%s\n", __func__);
#endif
}

#if BLE_PRIVACY_SPT == TRUE
#if (BLE_42_SCAN_EN == TRUE)
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
#endif // #if (BLE_42_SCAN_EN == TRUE)
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

#if (BLE_GATT_BGCONN == TRUE)
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
#endif // (BLE_GATT_BGCONN == TRUE)

#if (GATT_BG_CONN_DEV == TRUE)
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
    btm_ble_clear_white_list(NULL);
    gatt_reset_bgdev_list();
}
#endif // #if (GATT_BG_CONN_DEV == TRUE)

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
    return btm_update_dev_to_white_list(add_remove, remote_bda, 0, NULL);
}
#if 0
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
#endif
/*******************************************************************************
**
** Function         btm_set_conn_mode_adv_init_addr
**
** Description      set initiator address type and local address type based on adv
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
    list_node_t         *p_node = NULL;
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
                    //btm_ble_enable_resolving_list(BTM_BLE_RL_ADV);
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
        for (p_node = list_begin(btm_cb.p_sec_dev_rec_list); p_node; p_node = list_next(p_node)) {
            p_dev_rec = list_node(p_node);
            if ((p_dev_rec->sec_flags & BTM_SEC_IN_USE) != 0 &&
                    (p_dev_rec->ble.in_controller_list & BTM_RESOLVING_LIST_BIT) != 0) {
                memcpy(p_peer_addr_ptr, p_dev_rec->ble.static_addr, BD_ADDR_LEN);
                *p_peer_addr_type = p_dev_rec->ble.static_addr_type;
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

#if (BLE_42_ADV_EN == TRUE)
/*******************************************************************************
**
** Function         BTM_BleSetAdvParamsAll
**
** Description      This function is called to set all of the advertising parameters.
**
** Parameters:       None.
**
** Returns          void
**
*******************************************************************************/
tBTM_STATUS BTM_BleSetAdvParamsAll(UINT16 adv_int_min, UINT16 adv_int_max, UINT8 adv_type,
                                        tBLE_ADDR_TYPE own_bda_type, tBLE_BD_ADDR *p_dir_bda,
                                        tBTM_BLE_ADV_CHNL_MAP chnl_map, tBTM_BLE_AFP afp, tBTM_START_ADV_CMPL_CBACK *adv_cb)
{
    tBTM_LE_RANDOM_CB *p_addr_cb = &btm_cb.ble_ctr_cb.addr_mgnt_cb;
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;

    BTM_TRACE_EVENT ("BTM_BleSetAdvParamsAll\n");

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }
    if (BTM_BleUpdateOwnType(&own_bda_type, adv_cb) != 0) {
        return BTM_ILLEGAL_VALUE;
    }
    if (!BTM_BLE_ISVALID_PARAM(adv_int_min, BTM_BLE_ADV_INT_MIN, BTM_BLE_ADV_INT_MAX) ||
            !BTM_BLE_ISVALID_PARAM(adv_int_max, BTM_BLE_ADV_INT_MIN, BTM_BLE_ADV_INT_MAX)) {
         BTM_TRACE_ERROR ("adv_int_min or adv_int_max is invalid\n");
        if(adv_cb) {
            (* adv_cb)(HCI_ERR_ESP_VENDOR_FAIL);
        }
        return BTM_ILLEGAL_VALUE;
    }

    btm_ble_stop_adv();

    osi_mutex_lock(&adv_param_lock, OSI_MUTEX_MAX_TIMEOUT);
#if (BLE_TOPOLOGY_CHECK == TRUE)
    if(adv_type == BTM_BLE_CONNECT_DIR_EVT){
        btm_ble_set_topology_mask(BTM_BLE_STATE_HI_DUTY_DIR_ADV_BIT);
    }else if(adv_type == BTM_BLE_CONNECT_LO_DUTY_DIR_EVT){
        btm_ble_set_topology_mask(BTM_BLE_STATE_LO_DUTY_DIR_ADV_BIT);
    }else if(adv_type == BTM_BLE_NON_CONNECT_EVT){
        btm_ble_set_topology_mask(BTM_BLE_STATE_NON_CONN_ADV_BIT);
    }
#endif // (BLE_TOPOLOGY_CHECK == TRUE)
    p_cb->adv_interval_min = adv_int_min;
    p_cb->adv_interval_max = adv_int_max;
    p_cb->adv_chnl_map = chnl_map;
    p_addr_cb->own_addr_type = own_bda_type;
    p_cb->evt_type = adv_type;
    p_cb->afp = afp;
    p_cb->p_adv_cb = adv_cb;

    if (p_dir_bda) {
        memcpy(&p_cb->direct_bda, p_dir_bda, sizeof(tBLE_BD_ADDR));
    } else {
        return BTM_ILLEGAL_VALUE;
    }

    BTM_TRACE_EVENT ("update params for an active adv\n");

    tBTM_STATUS status = BTM_SUCCESS;
    /* update adv params */
    if (btsnd_hcic_ble_write_adv_params (adv_int_min,
                                        adv_int_max,
                                        adv_type,
                                        own_bda_type,
                                        p_dir_bda->type,
                                        p_dir_bda->bda,
                                        chnl_map,
                                        p_cb->afp)) {
        osi_sem_take(&adv_param_sem, OSI_SEM_MAX_TIMEOUT);
        status = adv_param_status;
    } else {
        status = BTM_NO_RESOURCES;
    }
    osi_mutex_unlock(&adv_param_lock);
    return status;
}

tBTM_STATUS BTM_BleStartAdv(void)
{
    tBTM_STATUS status = BTM_SUCCESS;
    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

    btm_ble_stop_adv();

    status = btm_ble_start_adv();

    return status;
}
#endif // #if (BLE_42_ADV_EN == TRUE)

#if (BLE_42_SCAN_EN == TRUE)
tBTM_STATUS BTM_BleSetScanFilterParams(tGATT_IF client_if, UINT32 scan_interval, UINT32 scan_window,
                                tBLE_SCAN_MODE scan_mode, UINT8 addr_type_own, UINT8 scan_duplicate_filter, tBTM_BLE_SFP scan_filter_policy,
                                tBLE_SCAN_PARAM_SETUP_CBACK scan_setup_status_cback)
{
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;
    UINT32 max_scan_interval;
    UINT32 max_scan_window;
    tBTM_STATUS ret = BTM_SUCCESS;

    BTM_TRACE_EVENT ("%s\n", __func__);
    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }
    if (BTM_BleUpdateOwnType(&addr_type_own, NULL) != 0) {
        return BTM_ILLEGAL_VALUE;
    }

    max_scan_interval = BTM_BLE_SCAN_INT_MAX;
    max_scan_window = BTM_BLE_SCAN_WIN_MAX;


    osi_mutex_lock(&scan_param_lock, OSI_MUTEX_MAX_TIMEOUT);

    if (BTM_BLE_ISVALID_PARAM(scan_interval, BTM_BLE_SCAN_INT_MIN, max_scan_interval) &&
            BTM_BLE_ISVALID_PARAM(scan_window, BTM_BLE_SCAN_WIN_MIN, max_scan_window) &&
            (scan_mode == BTM_BLE_SCAN_MODE_ACTI || scan_mode == BTM_BLE_SCAN_MODE_PASS) &&
            (scan_duplicate_filter < BTM_BLE_SCAN_DUPLICATE_MAX) && (scan_window <= scan_interval)) {
        p_cb->scan_type = scan_mode;
        p_cb->scan_interval = scan_interval;
        p_cb->scan_window = scan_window;
        p_cb->sfp = scan_filter_policy;
        p_cb->scan_params_set = TRUE;
        p_cb->scan_duplicate_filter = scan_duplicate_filter;


        if (btsnd_hcic_ble_set_scan_params(p_cb->scan_type, (UINT16)scan_interval,
                                       (UINT16)scan_window,
                                       addr_type_own,
                                       scan_filter_policy)) {
            osi_sem_take(&scan_param_sem, OSI_SEM_MAX_TIMEOUT);
            ret = scan_param_status;
        }
    } else {
        ret = BTM_ILLEGAL_VALUE;
        BTM_TRACE_ERROR("Illegal params: scan_interval = %d scan_window = %d\n",
                        scan_interval, scan_window);
    }
    osi_mutex_unlock(&scan_param_lock);
    return ret;
}
#endif // #if (BLE_42_SCAN_EN == TRUE)

#if (BLE_42_ADV_EN == TRUE)
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
    tBTM_STATUS     ret;
    UINT8   rsp_data[BTM_BLE_AD_DATA_LEN],
            *p = rsp_data;

    BTM_TRACE_EVENT (" BTM_BleWriteScanRsp");

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }

    osi_mutex_lock(&adv_data_lock, OSI_MUTEX_MAX_TIMEOUT);
    memset(rsp_data, 0, BTM_BLE_AD_DATA_LEN);
    btm_ble_build_adv_data(&data_mask, &p, p_data);
    if (data_mask != 0) {
        //data length should not exceed 31 bytes
        BTM_TRACE_WARNING("%s, Partial data write into ADV", __func__);
    }

    if (btsnd_hcic_ble_set_scan_rsp_data((UINT8)(p - rsp_data), rsp_data)) {
        osi_sem_take(&adv_data_sem, OSI_SEM_MAX_TIMEOUT);
        ret = adv_data_status;

        if (adv_data_status == BTM_SUCCESS && data_mask != 0) {
            btm_cb.ble_ctr_cb.inq_var.scan_rsp = TRUE;
        } else {
            btm_cb.ble_ctr_cb.inq_var.scan_rsp = FALSE;
        }
    } else {
        ret = BTM_ILLEGAL_VALUE;
    }

    osi_mutex_unlock(&adv_data_lock);
    return ret;
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
    tBTM_STATUS     ret;

    osi_mutex_lock(&adv_data_lock, OSI_MUTEX_MAX_TIMEOUT);
    if (btsnd_hcic_ble_set_scan_rsp_data((UINT8)raw_scan_rsp_len, p_raw_scan_rsp)) {
        osi_sem_take(&adv_data_sem, OSI_SEM_MAX_TIMEOUT);
        ret = adv_data_status;
    } else {
        ret = BTM_NO_RESOURCES;
    }
    osi_mutex_unlock(&adv_data_lock);

    return ret;
}
#endif // #if (BLE_42_ADV_EN == TRUE)

#if ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))
/*******************************************************************************
**
** Function         BTM_UpdateBleDuplicateExceptionalList
**
** Description      This function is called to update duplicate scan exceptional list.
**
** Parameters:      subcode: add, remove or clean duplicate scan exceptional list.
**                  type: device info type
**                  device_info: device information
**                  update_exceptional_list_cmp_cb: complete callback
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS BTM_UpdateBleDuplicateExceptionalList(uint8_t subcode, uint32_t type, BD_ADDR device_info,
                                                tBTM_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_CMPL_CBACK update_exceptional_list_cmp_cb)
{
    tBTM_BLE_CB *ble_cb = &btm_cb.ble_ctr_cb;
    tBTM_STATUS status = BTM_NO_RESOURCES;

    ble_cb->update_exceptional_list_cmp_cb = update_exceptional_list_cmp_cb;

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }
    if(!device_info) {
        return BTM_ILLEGAL_VALUE;
    }
    // subcoe + type + device info
    uint8_t device_info_array[1 + 4 + BD_ADDR_LEN] = {0};
    device_info_array[0] = subcode;
    device_info_array[1] = type & 0xff;
    device_info_array[2] = (type >> 8) & 0xff;
    device_info_array[3] = (type >> 16) & 0xff;
    device_info_array[4] = (type >> 24) & 0xff;
    switch (type)
    {
        case BTM_DUPLICATE_SCAN_EXCEPTIONAL_INFO_ADV_ADDR:
            bt_rcopy(&device_info_array[5], device_info, BD_ADDR_LEN);
            break;
        case BTM_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_LINK_ID:
            memcpy(&device_info_array[5], device_info, 4);
            break;
        case BTM_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_BEACON_TYPE:
            //do nothing
            break;
        case BTM_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_PROV_SRV_ADV:
            //do nothing
            break;
        case BTM_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_PROXY_SRV_ADV:
            //do nothing
            break;
        default:
            //do nothing
            break;
    }

    status = BTM_VendorSpecificCommand(HCI_VENDOR_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST, 1 + 4 + BD_ADDR_LEN, device_info_array, NULL);
    if(status == BTM_CMD_STARTED) {
        status = BTM_SUCCESS;
    }

    return status;
}
#endif // ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))

#if (BLE_42_ADV_EN == TRUE)
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
    tBTM_STATUS ret;

    BTM_TRACE_EVENT ("BTM_BleWriteAdvData ");

    if (!controller_get_interface()->supports_ble()) {
        return BTM_ILLEGAL_VALUE;
    }
    osi_mutex_lock(&adv_data_lock, OSI_MUTEX_MAX_TIMEOUT);
    memset(p_cb_data, 0, sizeof(tBTM_BLE_LOCAL_ADV_DATA));
    p = p_cb_data->ad_data;
    p_cb_data->data_mask = data_mask;

    p_cb_data->p_flags = btm_ble_build_adv_data(&mask, &p, p_data);

    p_cb_data->p_pad = p;

    if (mask != 0) {
        //data length should not exceed 31 bytes
        BTM_TRACE_WARNING("%s, Partial data write into ADV", __func__);
    }

    p_cb_data->data_mask &= ~mask;

    if (btsnd_hcic_ble_set_adv_data((UINT8)(p_cb_data->p_pad - p_cb_data->ad_data),
                                    p_cb_data->ad_data)) {
        osi_sem_take(&adv_data_sem, OSI_SEM_MAX_TIMEOUT);
        ret = adv_data_status;
    } else {
        ret = BTM_NO_RESOURCES;
    }
    osi_mutex_unlock(&adv_data_lock);
    return ret;
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
    tBTM_STATUS ret;
    osi_mutex_lock(&adv_data_lock, OSI_MUTEX_MAX_TIMEOUT);
    if (btsnd_hcic_ble_set_adv_data((UINT8)raw_adv_len, p_raw_adv)) {
        osi_sem_take(&adv_data_sem, OSI_SEM_MAX_TIMEOUT);
        ret = adv_data_status;
    } else {
        ret = BTM_NO_RESOURCES;
    }
    osi_mutex_unlock(&adv_data_lock);

    return ret;
}
#endif // #if (BLE_42_ADV_EN == TRUE)

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
tBTM_STATUS BTM_BleSetRandAddress(BD_ADDR rand_addr)
{
	if (rand_addr == NULL) {
		return BTM_SET_STATIC_RAND_ADDR_FAIL;
    }

    if (btm_cb.ble_ctr_cb.inq_var.state != BTM_BLE_IDLE) {
        BTM_TRACE_ERROR("Advertising or scanning now, can't set randaddress %d", btm_cb.ble_ctr_cb.inq_var.state);
        return BTM_SET_STATIC_RAND_ADDR_FAIL;
    }
    memcpy(btm_cb.ble_ctr_cb.addr_mgnt_cb.private_addr, rand_addr, BD_ADDR_LEN);
    memcpy(btm_cb.ble_ctr_cb.addr_mgnt_cb.static_rand_addr, rand_addr, BD_ADDR_LEN);
    //send the set random address to the controller
    if(btsnd_hcic_ble_set_random_addr(rand_addr)) {
        btm_cb.ble_ctr_cb.addr_mgnt_cb.exist_addr_bit |= BTM_BLE_GAP_ADDR_BIT_RANDOM;
        return BTM_SUCCESS;
    } else {
        return BTM_SET_STATIC_RAND_ADDR_FAIL;
    }
}

/*******************************************************************************
**
** Function         BTM_BleClearRandAddress
**
** Description      This function is called to clear the LE random address.
**
** Parameters:       None.
**
** Returns          void
**
*******************************************************************************/
void BTM_BleClearRandAddress(void)
{
    tBTM_BLE_CB  *p_cb = &btm_cb.ble_ctr_cb;
    if (btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type == BLE_ADDR_RANDOM && (p_cb->inq_var.state != BTM_BLE_IDLE)) {
        BTM_TRACE_ERROR("Advertising or scanning now, can't restore public address ");
        return;
    }
    memset(btm_cb.ble_ctr_cb.addr_mgnt_cb.static_rand_addr, 0, BD_ADDR_LEN);
    btm_cb.ble_ctr_cb.addr_mgnt_cb.exist_addr_bit &= (~BTM_BLE_GAP_ADDR_BIT_RANDOM);
    btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type = BLE_ADDR_PUBLIC;
}
/*******************************************************************************
**
** Function         BTM_BleGetCurrentAddress
**
** Description      This function is called to get local used BLE address.
**
** Parameters:       None.
**
** Returns          success or fail
**
*******************************************************************************/
BOOLEAN BTM_BleGetCurrentAddress(BD_ADDR addr, uint8_t *addr_type)
{
    if(addr == NULL || addr_type == NULL) {
        BTM_TRACE_ERROR("%s addr or addr_type is NULL\n", __func__);
        return FALSE;
    }
    if(btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type == BLE_ADDR_RANDOM) {
        *addr_type = BLE_ADDR_RANDOM;
        memcpy(addr, btm_cb.ble_ctr_cb.addr_mgnt_cb.private_addr, BD_ADDR_LEN);
    } else if(btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type == BLE_ADDR_PUBLIC) {
        *addr_type = BLE_ADDR_PUBLIC;
        memcpy(addr, &controller_get_interface()->get_address()->address, BD_ADDR_LEN);
    } else {
        BTM_TRACE_ERROR("%s\n", __func__);
        memset(addr, 0, BD_ADDR_LEN);
        return FALSE;
    }
    return TRUE;
}

#if ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))
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
UINT8 *BTM_CheckAdvData( UINT8 *p_adv, UINT16 adv_data_len, UINT8 type, UINT8 *p_length)
{
    UINT8 *p = p_adv;
    UINT8 length;
    UINT8 adv_type;

    STREAM_TO_UINT8(length, p);

    while ( length && (p - p_adv < adv_data_len)) {
        STREAM_TO_UINT8(adv_type, p);

        if ( adv_type == type ) {

            if((p + length - 1) > (p_adv + adv_data_len)) {
                /* avoid memory overflow*/
                *p_length = 0;
                return NULL;
            }

            /* length doesn't include itself */
            *p_length = length - 1; /* minus the length of type */
            return p;
        }

        p += length - 1; /* skip the length of data */

        /* Break loop if advertising data is in an incorrect format,
           as it may lead to memory overflow */
        if (p >= p_adv + adv_data_len) {
            break;
        }

        STREAM_TO_UINT8(length, p);
    }

    *p_length = 0;
    return NULL;
}
#endif // #if ((BLE_42_SCAN_EN == TRUE) || (BLE_50_EXTEND_SCAN_EN == TRUE))
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
UINT16 BTM_BleReadDiscoverability(void)
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
UINT16 BTM_BleReadConnectability(void)
{
    BTM_TRACE_API ("%s\n", __FUNCTION__);

    return (btm_cb.ble_ctr_cb.inq_var.connectable_mode);
}

void BTM_Recovery_Pre_State(void)
{
#if ((BLE_42_ADV_EN == TRUE) || (BLE_42_SCAN_EN == TRUE))
    tBTM_BLE_INQ_CB *ble_inq_cb = &btm_cb.ble_ctr_cb.inq_var;
    BTM_TRACE_DEBUG("%s state=0x%x", __func__, ble_inq_cb->state);
#endif // #if ((BLE_42_ADV_EN == TRUE) || (BLE_42_SCAN_EN == TRUE))
#if (BLE_42_ADV_EN == TRUE)
    if (ble_inq_cb->state & BTM_BLE_ADVERTISING) {
        btm_ble_stop_adv();
        btm_ble_start_adv();
    }
#endif // #if (BLE_42_ADV_EN == TRUE)
#if (BLE_42_SCAN_EN == TRUE)
    if (ble_inq_cb->state & BTM_BLE_SCANNING) {
        btm_ble_start_scan();
    }
#endif // #if (BLE_42_SCAN_EN == TRUE)
    return;
}

/*******************************************************************************
**
** Function         BTM_GetCurrentConnParams
**
** Description      This function is called to read the current connection parameters
**                  of the device
**
** Returns          TRUE or FALSE
**
*******************************************************************************/

BOOLEAN BTM_GetCurrentConnParams(BD_ADDR bda, uint16_t *interval, uint16_t *latency, uint16_t *timeout)
{
    if( (interval == NULL) || (latency == NULL) || (timeout == NULL) ) {
        BTM_TRACE_ERROR("%s error ", __func__);
        return FALSE;
    }

    if(btm_get_current_conn_params(bda, interval, latency, timeout)) {
        return TRUE;
    }

    return FALSE;
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
            if (strlen(btm_cb.cfg.ble_bd_name) > (UINT16)(len - MIN_ADV_LENGTH)) {
                cp_len = (UINT16)(len - MIN_ADV_LENGTH);
                *p++ = cp_len + 1;
                *p++ = BTM_BLE_AD_TYPE_NAME_SHORT;
                ARRAY_TO_STREAM(p, btm_cb.cfg.ble_bd_name, cp_len);
            } else {
                cp_len = (UINT16)strlen(btm_cb.cfg.ble_bd_name);
                *p++ = cp_len + 1;
                *p++ = BTM_BLE_AD_TYPE_NAME_CMPL;
                ARRAY_TO_STREAM(p, btm_cb.cfg.ble_bd_name, cp_len);
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
            BTM_TRACE_DEBUG("cp_len = %d\n,p_data->p_manu->len=%d\n", cp_len, p_data->p_manu->len);
            for (int i = 0; i < p_data->p_manu->len; i++) {
                BTM_TRACE_DEBUG("p_data->p_manu->p_val[%d] = %x\n", i, p_data->p_manu->p_val[i]);
            }
            *p++ = cp_len + 1;
            *p++ = BTM_BLE_AD_TYPE_MANU;
            ARRAY_TO_STREAM(p, p_data->p_manu->p_val, cp_len);
            BTM_TRACE_DEBUG("p_addr = %p\n,p_data->p_manu->p_val = %p\n", p, p_data->p_manu->p_val);
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

    BTM_TRACE_DEBUG("disc_mode %04x", disc_mode);
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
        BTM_TRACE_ERROR("flag = 0x%x,old_flag = 0x%x", flag, old_flag);
        btm_ble_update_adv_flag(flag);
    }
}
#if 0
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
#if (BLE_42_ADV_EN == TRUE)
            btm_ble_stop_adv();
#endif // #if (BLE_42_ADV_EN == TRUE)

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
#if (BLE_42_ADV_EN == TRUE)
        if (new_mode == BTM_BLE_ADV_ENABLE) {
            status = btm_ble_start_adv();
        } else {
            status = btm_ble_stop_adv();
        }
#endif // #if (BLE_42_ADV_EN == TRUE)
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
#endif
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
#if (BLE_42_ADV_EN == TRUE)
            btm_ble_stop_adv();
#endif // #if (BLE_42_ADV_EN == TRUE)

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
#if (BLE_42_ADV_EN == TRUE)
        if (new_mode == BTM_BLE_ADV_ENABLE) {
            status = btm_ble_start_adv();
        } else {
            status = btm_ble_stop_adv();
        }
#endif // #if (BLE_42_ADV_EN == TRUE)
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
void btm_ble_cache_adv_data(BD_ADDR bda, tBTM_INQ_RESULTS *p_cur, UINT8 data_len, UINT8 *p, UINT8 evt_type)
{
    tBTM_BLE_INQ_CB     *p_le_inq_cb = &btm_cb.ble_ctr_cb.inq_var;
    UINT8 *p_cache;
    //UINT8 length;

    /* cache adv report/scan response data */
    if (evt_type != BTM_BLE_SCAN_RSP_EVT) {
        p_le_inq_cb->adv_len = 0;
        memset(p_le_inq_cb->adv_data_cache, 0, BTM_BLE_CACHE_ADV_DATA_MAX);
        p_cur->adv_data_len = 0;
        p_cur->scan_rsp_len = 0;
    }

    //Clear the adv cache if the addresses are not equal
    if(memcmp(bda, p_le_inq_cb->adv_addr, BD_ADDR_LEN) != 0) {
        p_le_inq_cb->adv_len = 0;
        memcpy(p_le_inq_cb->adv_addr, bda, BD_ADDR_LEN);
        memset(p_le_inq_cb->adv_data_cache, 0, BTM_BLE_CACHE_ADV_DATA_MAX);
        p_cur->adv_data_len = 0;
        p_cur->scan_rsp_len = 0;
    }

    /* Additional validation to prevent potential integer overflow */
    if (data_len > BTM_BLE_CACHE_ADV_DATA_MAX) {
        BTM_TRACE_ERROR("BLE advertising data length exceeds maximum: %u > %u",
                    data_len, BTM_BLE_CACHE_ADV_DATA_MAX);
        return;
    }

    if (data_len > 0) {
        p_cache = &p_le_inq_cb->adv_data_cache[p_le_inq_cb->adv_len];
        if((data_len + p_le_inq_cb->adv_len) <= BTM_BLE_CACHE_ADV_DATA_MAX) {
            memcpy(p_cache, p, data_len);
            p_le_inq_cb->adv_len += data_len;
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
    /* for discover, always "discoverable */
    if (BTM_BLE_IS_DISCO_ACTIVE(btm_cb.ble_ctr_cb.scan_activity)) {
        rt |= BTM_BLE_DISCO_RESULT;
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
        if ((p_flag = BTM_CheckAdvData(p_le_inq_cb->adv_data_cache, p_le_inq_cb->adv_len,
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
    case BTM_BLE_APPEARANCE_UNKNOWN:
    case BTM_BLE_APPEARANCE_GENERIC_CLOCK:
    case BTM_BLE_APPEARANCE_GENERIC_TAG:
    case BTM_BLE_APPEARANCE_GENERIC_KEYRING:
    case BTM_BLE_APPEARANCE_GENERIC_CYCLING:
    case BTM_BLE_APPEARANCE_CYCLING_COMPUTER:
    case BTM_BLE_APPEARANCE_CYCLING_SPEED:
    case BTM_BLE_APPEARANCE_CYCLING_CADENCE:
    case BTM_BLE_APPEARANCE_CYCLING_POWER:
    case BTM_BLE_APPEARANCE_CYCLING_SPEED_CADENCE:
    case BTM_BLE_APPEARANCE_STANDALONE_SPEAKER:
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
BOOLEAN btm_ble_update_inq_result(BD_ADDR bda, tINQ_DB_ENT *p_i, UINT8 addr_type, UINT8 evt_type, UINT8 *p)
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
    btm_ble_cache_adv_data(bda, p_cur, data_len, p, evt_type);

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
#if BTM_BLE_ACTIVE_SCAN_REPORT_ADV_SCAN_RSP_INDIVIDUALLY == FALSE
        to_report = FALSE;
#endif
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
#if BTM_BLE_ACTIVE_SCAN_REPORT_ADV_SCAN_RSP_INDIVIDUALLY
    if (evt_type == BTM_BLE_SCAN_RSP_EVT) {
        p_cur->ble_evt_type = evt_type;
    }
#endif
    p_i->inq_count = p_inq->inq_counter;   /* Mark entry for current inquiry */

    if (p_le_inq_cb->adv_len != 0) {
        if ((p_flag = BTM_CheckAdvData(p_le_inq_cb->adv_data_cache, p_le_inq_cb->adv_len, BTM_BLE_AD_TYPE_FLAG, &len)) != NULL) {
            p_cur->flag = * p_flag;
        }
    }

    if (p_le_inq_cb->adv_len != 0) {
        /* Check to see the BLE device has the Appearance UUID in the advertising data.  If it does
         * then try to convert the appearance value to a class of device value Bluedroid can use.
         * Otherwise fall back to trying to infer if it is a HID device based on the service class.
         */
        p_uuid16 = BTM_CheckAdvData(p_le_inq_cb->adv_data_cache, p_le_inq_cb->adv_len, BTM_BLE_AD_TYPE_APPEARANCE, &len);
        if (p_uuid16 && len == 2) {
            btm_ble_appearance_to_cod((UINT16)p_uuid16[0] | (p_uuid16[1] << 8), p_cur->dev_class);
        } else {
            if ((p_uuid16 = BTM_CheckAdvData(p_le_inq_cb->adv_data_cache, p_le_inq_cb->adv_len,
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

#if (BLE_GATT_BGCONN == TRUE)
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
        p_dev_name = BTM_CheckAdvData(p_data, data_len, BTM_BLE_AD_TYPE_NAME_CMPL, &len);

        if (p_dev_name == NULL) {
            p_dev_name = BTM_CheckAdvData(p_data, data_len, BTM_BLE_AD_TYPE_NAME_SHORT, &len);
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
#endif // #if (BLE_GATT_BGCONN == TRUE)

#if (BLE_42_SCAN_EN == TRUE)
static void btm_adv_pkt_handler(void *arg)
{
    UINT8   hci_evt_code, hci_evt_len;
    UINT8   ble_sub_code;

    tBTM_BLE_CB *p_cb = &btm_cb.ble_ctr_cb;
    size_t pkts_to_process = pkt_queue_length(p_cb->adv_rpt_queue);
    if (pkts_to_process > BTM_BLE_GAP_ADV_RPT_BATCH_SIZE) {
        pkts_to_process = BTM_BLE_GAP_ADV_RPT_BATCH_SIZE;
    }

    for (size_t i = 0; i < pkts_to_process; i++) {
        pkt_linked_item_t *linked_pkt = pkt_queue_dequeue(p_cb->adv_rpt_queue);
        assert(linked_pkt != NULL);
        BT_HDR *packet = (BT_HDR *)linked_pkt->data;
        uint8_t *p = packet->data + packet->offset;
        STREAM_TO_UINT8  (hci_evt_code, p);
        STREAM_TO_UINT8  (hci_evt_len, p);
        STREAM_TO_UINT8  (ble_sub_code, p);
        if (ble_sub_code == HCI_BLE_ADV_PKT_RPT_EVT) {
            btm_ble_process_adv_pkt(p);
        } else if (ble_sub_code == HCI_BLE_ADV_DISCARD_REPORT_EVT) {
            btm_ble_process_adv_discard_evt(p);
        } else if (ble_sub_code == HCI_BLE_DIRECT_ADV_EVT) {
            btm_ble_process_direct_adv_pkt(p);
        } else {
            assert (0);
        }

        osi_free(linked_pkt);
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
        hci_adv_credits_try_release(1);
#endif
    }

    if (pkt_queue_length(p_cb->adv_rpt_queue) != 0) {
        btu_task_post(SIG_BTU_HCI_ADV_RPT_MSG, NULL, OSI_THREAD_MAX_TIMEOUT);
    }

    UNUSED(hci_evt_code);
    UNUSED(hci_evt_len);
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
#if (!CONTROLLER_RPA_LIST_ENABLE)
    BD_ADDR             temp_bda;
    UINT8               temp_addr_type = 0;
#endif // (!CONTROLLER_RPA_LIST_ENABLE)
#endif//(defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)

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

#if (!CONTROLLER_RPA_LIST_ENABLE)
        temp_addr_type = addr_type;
        memcpy(temp_bda, bda, BD_ADDR_LEN);
#endif // (!CONTROLLER_RPA_LIST_ENABLE)

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
#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE && (!CONTROLLER_RPA_LIST_ENABLE))
        //save current adv addr information if p_dev_rec!= NULL
        tBTM_SEC_DEV_REC *p_dev_rec = btm_find_dev (bda);
        if(p_dev_rec) {
            p_dev_rec->ble.current_addr_type = temp_addr_type;
            memcpy(p_dev_rec->ble.current_addr, temp_bda, BD_ADDR_LEN);
            p_dev_rec->ble.current_addr_valid = true;
        }
#endif
        STREAM_TO_UINT8(data_len, p);

        /* Advance to the next event data_len + rssi byte */
        p += data_len + 1;
    }
}

/*******************************************************************************
**
** Function         btm_ble_process_last_adv_pkt
**
** Description      This function is called to report last adv packet
**
** Parameters
**
** Returns          void
**
*******************************************************************************/

static void btm_ble_process_last_adv_pkt(void)
{
    UINT8       result = 0;
    UINT8       null_bda[6] = {0};
    tBTM_INQ_RESULTS_CB  *p_scan_results_cb = btm_cb.ble_ctr_cb.p_scan_results_cb;
    tBTM_BLE_INQ_CB      *p_le_inq_cb = &btm_cb.ble_ctr_cb.inq_var;
    tINQ_DB_ENT          *p_i = btm_inq_db_find (p_le_inq_cb->adv_addr);

    if(memcmp(null_bda, p_le_inq_cb->adv_addr, BD_ADDR_LEN) == 0) {
        return;
    }

    if(p_i == NULL) {
        BTM_TRACE_DEBUG("no last adv");
        return;
    }

    if ((result = btm_ble_is_discoverable(p_le_inq_cb->adv_addr, p_i->inq_info.results.ble_evt_type, NULL)) == 0) {
        BTM_TRACE_WARNING("%s device is no longer discoverable so discarding advertising packet pkt",
                 __func__);
        return;
    }
#if (BLE_GATT_BGCONN == TRUE)
    /* background connection in selective connection mode */
    if (btm_cb.ble_ctr_cb.bg_conn_type == BTM_BLE_CONN_SELECTIVE) {
        //do nothing
    } else
#endif // (BLE_GATT_BGCONN == TRUE)
    {
        if (p_scan_results_cb && (result & BTM_BLE_DISCO_RESULT)) {
            (p_scan_results_cb)((tBTM_INQ_RESULTS *) &p_i->inq_info.results, p_le_inq_cb->adv_data_cache);
            p_le_inq_cb->adv_len = 0;
            memset(p_le_inq_cb->adv_addr, 0, BD_ADDR_LEN);
            p_i->inq_info.results.adv_data_len = 0;
            p_i->inq_info.results.scan_rsp_len = 0;
        }
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
    tBTM_INQ_RESULTS_CB  *p_scan_results_cb = btm_cb.ble_ctr_cb.p_scan_results_cb;
    tBTM_BLE_INQ_CB      *p_le_inq_cb = &btm_cb.ble_ctr_cb.inq_var;
    BOOLEAN     update = TRUE;
    UINT8       result = 0;

    /* Event_Type:
        0x00 Connectable undirected advertising (ADV_IND).
        0x01 Connectable directed advertising (ADV_DIRECT_IND)
        0x02 Scannable undirected advertising (ADV_SCAN_IND)
        0x03 Non connectable undirected advertising (ADV_NONCONN_IND)
        0x04 Scan Response (SCAN_RSP)
        0x05-0xFF Reserved for future use
    */
    // The adv packet without scan response is allowed to report to higher layer
    /*
    Bluedroid will put the advertising packet and scan response into a packet and send it to the higher layer.
    If two advertising packets are not with the same address, or can't be combined into a packet, then the first advertising
    packet will be discarded. So we added the following judgment:
    1. For different addresses, send the last advertising packet to higher layer
    2. For same address and same advertising type (not scan response), send the last advertising packet to higher layer
    3. For same address and scan response, do nothing
    */
    int same_addr = memcmp(bda, p_le_inq_cb->adv_addr, BD_ADDR_LEN);
    if (same_addr != 0 || (evt_type != BTM_BLE_SCAN_RSP_EVT)) {
        btm_ble_process_last_adv_pkt();
    }


    p_i = btm_inq_db_find (bda);

    /* Check if this address has already been processed for this inquiry */
    if (btm_inq_find_bdaddr(bda)) {
        /* never been report as an LE device */
        if (p_i &&
                (!(p_i->inq_info.results.device_type & BT_DEVICE_TYPE_BLE) ||
                 /* scan response to be updated */
                 (!p_i->scan_rsp))) {
            update = TRUE;
        } else if (BTM_BLE_IS_DISCO_ACTIVE(btm_cb.ble_ctr_cb.scan_activity)) {
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
    if (!btm_ble_update_inq_result(bda, p_i, addr_type, evt_type, p)) {
        return;
    }

    if ((result = btm_ble_is_discoverable(bda, evt_type, p)) == 0) {
        BTM_TRACE_WARNING("%s device is no longer discoverable so discarding advertising packet pkt",
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
                ((p_i->inq_info.results.device_type & BT_DEVICE_TYPE_BLE) == BT_DEVICE_TYPE_BLE &&
                 p_i->scan_rsp)) {

            btm_acl_update_busy_level (BTM_BLI_INQ_DONE_EVT);
        }
    }
#if (BLE_GATT_BGCONN == TRUE)
    /* background connection in selective connection mode */
    if (btm_cb.ble_ctr_cb.bg_conn_type == BTM_BLE_CONN_SELECTIVE) {
        if (result & BTM_BLE_SEL_CONN_RESULT) {
            btm_send_sel_conn_callback(bda, evt_type, p, addr_type);
        } else {
            BTM_TRACE_DEBUG("None LE device, can not initiate selective connection\n");
        }
    } else
#endif // (BLE_GATT_BGCONN == TRUE)
    {
        if (p_scan_results_cb && (result & BTM_BLE_DISCO_RESULT)) {
            (p_scan_results_cb)((tBTM_INQ_RESULTS *) &p_i->inq_info.results, p_le_inq_cb->adv_data_cache);
            p_le_inq_cb->adv_len = 0;
            memset(p_le_inq_cb->adv_addr, 0, BD_ADDR_LEN);
            p_i->inq_info.results.adv_data_len = 0;
            p_i->inq_info.results.scan_rsp_len = 0;
        }
    }
}

void btm_ble_process_adv_discard_evt(UINT8 *p)
{
#if (BLE_42_SCAN_EN == TRUE)
#if (BLE_ADV_REPORT_FLOW_CONTROL == TRUE)
    uint32_t num_dis = 0;
    STREAM_TO_UINT32 (num_dis, p);
    tBTM_INQ_DIS_CB *p_obs_discard_cb = btm_cb.ble_ctr_cb.p_obs_discard_cb;
    if(p_obs_discard_cb) {
        (p_obs_discard_cb)(num_dis);
    }
#endif
#endif // #if (BLE_42_SCAN_EN == TRUE)
}

void btm_ble_process_direct_adv_pkt(UINT8 *p)
{
    // TODO
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

    osi_mutex_lock(&scan_enable_lock, OSI_MUTEX_MAX_TIMEOUT);

    if(p_inq->scan_duplicate_filter > BTM_BLE_DUPLICATE_MAX) {
        p_inq->scan_duplicate_filter = BTM_BLE_DUPLICATE_DISABLE;
    }
    /* start scan, disable duplicate filtering */
    if (!btsnd_hcic_ble_set_scan_enable (BTM_BLE_SCAN_ENABLE, p_inq->scan_duplicate_filter)) {
        status = BTM_NO_RESOURCES;
    } else {
        osi_sem_take(&scan_enable_sem, OSI_SEM_MAX_TIMEOUT);
        if(scan_enable_status != BTM_SUCCESS) {
            status = BTM_NO_RESOURCES;
        }
        btm_cb.ble_ctr_cb.inq_var.state |= BTM_BLE_SCANNING;
#if (BLE_TOPOLOGY_CHECK == TRUE)
        if (p_inq->scan_type == BTM_BLE_SCAN_MODE_ACTI) {
            btm_ble_set_topology_mask(BTM_BLE_STATE_ACTIVE_SCAN_BIT);
        } else {
            btm_ble_set_topology_mask(BTM_BLE_STATE_PASSIVE_SCAN_BIT);
        }
#endif // (BLE_TOPOLOGY_CHECK == TRUE)
    }
    osi_mutex_unlock(&scan_enable_lock);
    return status;
}
#endif  // #if (BLE_42_SCAN_EN == TRUE)

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

    /* Clear the inquiry callback if set */
    btm_cb.ble_ctr_cb.inq_var.scan_type = BTM_BLE_SCAN_MODE_NONE;
    btm_cb.ble_ctr_cb.inq_var.state &= ~BTM_BLE_SCANNING;
    /* stop discovery now */
    btsnd_hcic_ble_set_scan_enable (BTM_BLE_SCAN_DISABLE, BTM_BLE_DUPLICATE_ENABLE);

    btm_update_scanner_filter_policy(SP_ADV_ALL);

    btm_cb.ble_ctr_cb.wl_state &= ~BTM_BLE_WL_SCAN;
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
static void btm_ble_stop_discover(void)
{
#if (BLE_42_SCAN_EN == TRUE)
    tBTM_BLE_CB *p_ble_cb = & btm_cb.ble_ctr_cb;
    tBTM_CMPL_CB *p_scan_cb = p_ble_cb->p_scan_cmpl_cb;
    btu_stop_timer (&p_ble_cb->scan_timer_ent);

    osi_mutex_lock(&scan_enable_lock, OSI_MUTEX_MAX_TIMEOUT);
    p_ble_cb->scan_activity &= ~BTM_LE_DISCOVER_ACTIVE;

    p_ble_cb->p_scan_results_cb = NULL;
    p_ble_cb->p_scan_cmpl_cb = NULL;

    if (!BTM_BLE_IS_SCAN_ACTIVE(p_ble_cb->scan_activity)) {
        /* Clear the inquiry callback if set */
        btm_cb.ble_ctr_cb.inq_var.state &= ~BTM_BLE_SCANNING;
        /* stop discovery now */
        if(btsnd_hcic_ble_set_scan_enable (BTM_BLE_SCAN_DISABLE, BTM_BLE_DUPLICATE_ENABLE)) {
            osi_sem_take(&scan_enable_sem, OSI_SEM_MAX_TIMEOUT);
        }
#if (BLE_TOPOLOGY_CHECK == TRUE)
        /* reset status */
        btm_ble_clear_topology_mask(BTM_BLE_STATE_ACTIVE_SCAN_BIT);
        btm_ble_clear_topology_mask(BTM_BLE_STATE_PASSIVE_SCAN_BIT);
#endif // (BLE_TOPOLOGY_CHECK == TRUE)
    }

    if (p_scan_cb) {
        (p_scan_cb)((tBTM_INQUIRY_CMPL *) &btm_cb.btm_inq_vars.inq_cmpl_info);
    }
    osi_mutex_unlock(&scan_enable_lock);
#endif // #if (BLE_42_SCAN_EN == TRUE)
}

/*******************************************************************************
**
** Function         btm_ble_adv_states_operation
**
** Description      Set or clear adv states in topology mask
**
** Returns          operation status. TRUE if successful, FALSE otherwise.
**
*******************************************************************************/
typedef BOOLEAN (BTM_TOPOLOGY_FUNC_PTR)(tBTM_BLE_STATE_MASK);
#if (BLE_42_ADV_EN == TRUE)
#if (BLE_TOPOLOGY_CHECK == TRUE)
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
#endif // (BLE_TOPOLOGY_CHECK == TRUE)

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
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;
    tBTM_STATUS     rt = BTM_NO_RESOURCES;
    BTM_TRACE_EVENT ("btm_ble_start_adv\n");

#if (BLE_TOPOLOGY_CHECK == TRUE)
    if (!btm_ble_adv_states_operation (btm_ble_topology_check, p_cb->evt_type)) {
        return BTM_WRONG_MODE;
    }
#endif // (BLE_TOPOLOGY_CHECK == TRUE)

    osi_mutex_lock(&adv_enable_lock, OSI_MUTEX_MAX_TIMEOUT);

#if (defined BLE_PRIVACY_SPT && BLE_PRIVACY_SPT == TRUE)
    /* To relax resolving list,  always have resolving list enabled, unless directed adv */
    if (p_cb->evt_type != BTM_BLE_CONNECT_LO_DUTY_DIR_EVT &&
            p_cb->evt_type != BTM_BLE_CONNECT_DIR_EVT)
        /* enable resolving list is desired */
    {
        //btm_ble_enable_resolving_list_for_platform(BTM_BLE_RL_ADV);
    }
#endif
    if (p_cb->afp != AP_SCAN_CONN_ALL) {
        //find the device in the btm dev buffer and write it to the controller white list
        btm_execute_wl_dev_operation();
        btm_cb.ble_ctr_cb.wl_state |= BTM_BLE_WL_ADV;
    }
    /* The complete event comes up immediately after the 'btsnd_hcic_ble_set_adv_enable' being called in dual core,
    this causes the 'adv_mode' and 'state' not be set yet, so we set the state first */
    tBTM_BLE_GAP_STATE temp_state = p_cb->state;
    UINT8 adv_mode = p_cb->adv_mode;
    p_cb->adv_mode = BTM_BLE_ADV_ENABLE;
    p_cb->state |= BTM_BLE_ADVERTISING;
#if (BLE_TOPOLOGY_CHECK == TRUE)
    btm_ble_adv_states_operation(btm_ble_set_topology_mask, p_cb->evt_type);
#endif // (BLE_TOPOLOGY_CHECK == TRUE)
    if (btsnd_hcic_ble_set_adv_enable (BTM_BLE_ADV_ENABLE)) {
        osi_sem_take(&adv_enable_sem, OSI_SEM_MAX_TIMEOUT);
        rt = adv_enable_status;
        BTM_TRACE_EVENT ("BTM_SUCCESS\n");
    } else {
        p_cb->adv_mode = BTM_BLE_ADV_DISABLE;
        p_cb->state = temp_state;
        p_cb->adv_mode = adv_mode;
#if (BLE_TOPOLOGY_CHECK == TRUE)
        btm_ble_adv_states_operation(btm_ble_clear_topology_mask, p_cb->evt_type);
#endif // (BLE_TOPOLOGY_CHECK == TRUE)
        btm_cb.ble_ctr_cb.wl_state &= ~BTM_BLE_WL_ADV;
    }

    if(adv_enable_status != HCI_SUCCESS) {
        p_cb->adv_mode = adv_mode;
    }
    osi_mutex_unlock(&adv_enable_lock);
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
    tBTM_BLE_INQ_CB *p_cb = &btm_cb.ble_ctr_cb.inq_var;
    tBTM_STATUS rt = BTM_SUCCESS;
    if (p_cb) {
        osi_mutex_lock(&adv_enable_lock, OSI_MUTEX_MAX_TIMEOUT);
        UINT8 temp_adv_mode = p_cb->adv_mode;
        BOOLEAN temp_fast_adv_on = p_cb->fast_adv_on;
        tBTM_BLE_GAP_STATE temp_state = p_cb->state;
        tBTM_BLE_WL_STATE temp_wl_state = btm_cb.ble_ctr_cb.wl_state;
#if (BLE_TOPOLOGY_CHECK == TRUE)
        tBTM_BLE_STATE_MASK temp_mask = btm_ble_get_topology_mask ();
#endif // (BLE_TOPOLOGY_CHECK == TRUE)

        p_cb->fast_adv_on = FALSE;
        p_cb->adv_mode = BTM_BLE_ADV_DISABLE;
        p_cb->state &= ~BTM_BLE_ADVERTISING;
        btm_cb.ble_ctr_cb.wl_state &= ~BTM_BLE_WL_ADV;
#if (BLE_TOPOLOGY_CHECK == TRUE)
        /* clear all adv states */
        btm_ble_clear_topology_mask (BTM_BLE_STATE_ALL_ADV_MASK);
#endif // (BLE_TOPOLOGY_CHECK == TRUE)
        if (btsnd_hcic_ble_set_adv_enable (BTM_BLE_ADV_DISABLE)) {
            osi_sem_take(&adv_enable_sem, OSI_SEM_MAX_TIMEOUT);
            rt = adv_enable_status;
        } else {
            // reset state
            p_cb->fast_adv_on = temp_fast_adv_on;
            p_cb->adv_mode = temp_adv_mode;
            p_cb->state = temp_state;
            btm_cb.ble_ctr_cb.wl_state = temp_wl_state;
#if (BLE_TOPOLOGY_CHECK == TRUE)
            btm_ble_set_topology_mask (temp_mask);
#endif // (BLE_TOPOLOGY_CHECK == TRUE)
            rt = BTM_NO_RESOURCES;
        }
        if(adv_enable_status != HCI_SUCCESS) {
            p_cb->adv_mode = temp_adv_mode;
        }
        osi_mutex_unlock(&adv_enable_lock);
    }
    return rt;
}
#endif // #if (BLE_42_ADV_EN == TRUE)

tBTM_STATUS btm_ble_set_random_addr(BD_ADDR random_bda)
{
    tBTM_STATUS rt = BTM_SUCCESS;
#if (BLE_42_ADV_EN == TRUE)
    osi_mutex_lock(&adv_enable_lock, OSI_MUTEX_MAX_TIMEOUT);
#endif // #if (BLE_42_ADV_EN == TRUE)
#if (BLE_42_SCAN_EN == TRUE)
    osi_mutex_lock(&scan_enable_lock, OSI_MUTEX_MAX_TIMEOUT);
#endif // #if (BLE_42_SCAN_EN == TRUE)

#if (BLE_42_ADV_EN == TRUE)
    if (btm_cb.ble_ctr_cb.inq_var.adv_mode == BTM_BLE_ADV_ENABLE) {
        if (btsnd_hcic_ble_set_adv_enable (BTM_BLE_ADV_DISABLE)) {
            osi_sem_take(&adv_enable_sem, OSI_SEM_MAX_TIMEOUT);
            rt = adv_enable_status;
        } else {
            rt = BTM_BAD_VALUE_RET;
        }
    }
#endif // #if (BLE_42_ADV_EN == TRUE)

#if (BLE_42_SCAN_EN == TRUE)
    if (BTM_BLE_IS_DISCO_ACTIVE(btm_cb.ble_ctr_cb.scan_activity)) {
        if (btsnd_hcic_ble_set_scan_enable (BTM_BLE_SCAN_DISABLE, BTM_BLE_SCAN_DUPLICATE_DISABLE)) {
            osi_sem_take(&scan_enable_sem, OSI_SEM_MAX_TIMEOUT);
            rt = scan_enable_status;
        } else {
            rt = BTM_BAD_VALUE_RET;
        }
    }
#endif // #if (BLE_42_SCAN_EN == TRUE)

    if (rt == BTM_SUCCESS) {
        btsnd_hcic_ble_set_random_addr(random_bda);
    }

#if (BLE_42_ADV_EN == TRUE)
    if (btm_cb.ble_ctr_cb.inq_var.adv_mode == BTM_BLE_ADV_ENABLE) {
        if (btsnd_hcic_ble_set_adv_enable (BTM_BLE_ADV_ENABLE)) {
            osi_sem_take(&adv_enable_sem, OSI_SEM_MAX_TIMEOUT);
            rt = adv_enable_status;
        } else {
            rt = BTM_BAD_VALUE_RET;
        }
    }
#endif // #if (BLE_42_ADV_EN == TRUE)

#if (BLE_42_SCAN_EN == TRUE)
    if (BTM_BLE_IS_DISCO_ACTIVE(btm_cb.ble_ctr_cb.scan_activity)) {
        if (btsnd_hcic_ble_set_scan_enable (BTM_BLE_SCAN_ENABLE, btm_cb.ble_ctr_cb.inq_var.scan_duplicate_filter)) {
            osi_sem_take(&scan_enable_sem, OSI_SEM_MAX_TIMEOUT);
            rt = scan_enable_status;
        } else {
            rt = BTM_BAD_VALUE_RET;
        }
    }
#endif // #if (BLE_42_SCAN_EN == TRUE)

#if (BLE_42_ADV_EN == TRUE)
    osi_mutex_unlock(&adv_enable_lock);
#endif // #if (BLE_42_ADV_EN == TRUE)
#if (BLE_42_SCAN_EN == TRUE)
    osi_mutex_unlock(&scan_enable_lock);
#endif // #if (BLE_42_SCAN_EN == TRUE)

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
#if (BLE_42_ADV_EN == TRUE)
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
#endif // #if (BLE_42_ADV_EN == TRUE)
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
        break;
    case BTU_TTYPE_BLE_SCAN:
        btm_ble_stop_discover();
        break;
    case BTU_TTYPE_BLE_INQUIRY:
        break;

    case BTU_TTYPE_BLE_GAP_LIM_DISC:
        /* lim_timeout expired, limited discovery should exit now */
        btm_cb.btm_inq_vars.discoverable_mode &= ~BTM_BLE_LIMITED_DISCOVERABLE;
        btm_ble_set_adv_flag(btm_cb.btm_inq_vars.connectable_mode, btm_cb.btm_inq_vars.discoverable_mode);
        break;

    case BTU_TTYPE_BLE_RANDOM_ADDR:
        if (btm_cb.ble_ctr_cb.addr_mgnt_cb.own_addr_type == BLE_ADDR_RANDOM) {
            if (NULL == (void *)(p_tle->param)) {
#if (CONTROLLER_RPA_LIST_ENABLE == FALSE)
                /* refresh the random addr */
                btm_gen_resolvable_private_addr((void *)btm_gen_resolve_paddr_low);
#endif
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
    tACL_CONN        *p_acl_cb = NULL;
    UINT16            handle;
    UINT8             status;

    BTM_TRACE_EVENT ("btm_ble_read_remote_features_complete ");

    STREAM_TO_UINT8(status, p);

    // if LE read remote feature failed for HCI_ERR_CONN_FAILED_ESTABLISHMENT,
    // expect disconnect complete to be received
    if (status != HCI_ERR_CONN_FAILED_ESTABLISHMENT) {
        STREAM_TO_UINT16 (handle, p);

        /* Look up the connection by handle and copy features */
        p_acl_cb = btm_handle_to_acl(handle);
	if (p_acl_cb) {
	    {
                STREAM_TO_ARRAY(p_acl_cb->peer_le_features, p, BD_FEATURES_LEN);
#if BLE_INCLUDED == TRUE
                /* In the original Bluedroid version, slave need to send LL_VERSION_IND(call btsnd_hcic_rmt_ver_req)
                 * to remote device if it has not received ll_version_ind.
                 * Delete it to resolve Android 7.0 incompatible problem. But it may cause that slave host
                 * can't get remote device's version.*/
                if (p_acl_cb->link_role == HCI_ROLE_MASTER){
                    btsnd_hcic_rmt_ver_req (p_acl_cb->hci_handle);
                }
                else{
                    uint16_t data_length = controller_get_interface()->get_ble_default_data_packet_length();
                    uint16_t data_txtime = controller_get_interface()->get_ble_default_data_packet_txtime();
                    if (p_acl_cb->transport == BT_TRANSPORT_LE) {
                        if (HCI_LE_DATA_LEN_EXT_SUPPORTED(p_acl_cb->peer_le_features) &&
                            (p_acl_cb->data_length_params.tx_len != data_length)) {
                            p_acl_cb->data_len_updating = true;
                            btsnd_hcic_ble_set_data_length(p_acl_cb->hci_handle, data_length, data_txtime);
                        }
                        l2cble_notify_le_connection (p_acl_cb->remote_addr);
                    }
                }
#endif
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
    /* if write adv enable/disable not succeed */
    if (*p != HCI_SUCCESS) {
        BTM_TRACE_ERROR("%s failed", __func__);
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

#if (BLE_TOPOLOGY_CHECK == TRUE)
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
** Function         btm_ble_get_topology_mask
**
** Description      Get BLE topology bit mask
**
** Returns          state mask.
**
*******************************************************************************/
tBTM_BLE_STATE_MASK btm_ble_get_topology_mask (void)
{
    return btm_cb.ble_ctr_cb.cur_states;
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
#endif // (BLE_TOPOLOGY_CHECK == TRUE)

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
BOOLEAN btm_ble_update_mode_operation(UINT8 link_role, BD_ADDR bd_addr, UINT8 status)
{
    BOOLEAN bg_con = FALSE;
    if (status == HCI_ERR_DIRECTED_ADVERTISING_TIMEOUT) {
        btm_cb.ble_ctr_cb.inq_var.adv_mode  = BTM_BLE_ADV_DISABLE;
        /* make device fall back into undirected adv mode by default */
        btm_cb.ble_ctr_cb.inq_var.directed_conn = BTM_BLE_CONNECT_EVT;
#if (BLE_TOPOLOGY_CHECK == TRUE)
        /* clear all adv states */
        btm_ble_clear_topology_mask (BTM_BLE_STATE_ALL_ADV_MASK);
#endif // (BLE_TOPOLOGY_CHECK == TRUE)
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
#if (GATT_BG_CONN_DEV == TRUE)
        bg_con = btm_ble_resume_bg_conn();
#endif // #if (GATT_BG_CONN_DEV == TRUE)
    }

    return bg_con;
}

#if (BLE_VENDOR_HCI_EN == TRUE)
static void btm_ble_vs_evt_callback(UINT8 len, UINT8 *p)
{
    UINT8 sub_event;

    if (!len || !p) {
        return;
    }

    STREAM_TO_UINT8(sub_event, p);
    len--;

    if (sub_event < HCI_VSE_LE_SUBEVT_BASE) {
        return;
    }

    if (ble_vs_evt_callback) {
        ble_vs_evt_callback(sub_event, len, p);
    }
}
#endif // #if (BLE_VENDOR_HCI_EN == TRUE)

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
    BTM_TRACE_DEBUG("%s", __func__);

    tBTM_BLE_CB *p_cb = &btm_cb.ble_ctr_cb;
#if (BLE_42_SCAN_EN == TRUE)
    btu_free_timer(&p_cb->scan_timer_ent);
#endif // (BLE_42_SCAN_EN == TRUE)
    btu_free_timer(&p_cb->inq_var.fast_adv_timer);
    memset(p_cb, 0, sizeof(tBTM_BLE_CB));
#if (BLE_TOPOLOGY_CHECK == TRUE)
    p_cb->cur_states       = 0;
#endif // (BLE_TOPOLOGY_CHECK == TRUE)
    p_cb->conn_pending_q = fixed_queue_new(QUEUE_SIZE_MAX);

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
#if (BLE_42_SCAN_EN == TRUE)
    p_cb->adv_rpt_queue = pkt_queue_create();
    assert(p_cb->adv_rpt_queue != NULL);

    p_cb->adv_rpt_ready = osi_event_create(btm_adv_pkt_handler, NULL);
    assert(p_cb->adv_rpt_ready != NULL);
    osi_event_bind(p_cb->adv_rpt_ready, btu_get_current_thread(), 0);
#endif // #if (BLE_42_SCAN_EN == TRUE)

#if (BLE_VENDOR_HCI_EN == TRUE)
    BTM_RegisterForVSEvents(btm_ble_vs_evt_callback, TRUE);
#endif // #if (BLE_VENDOR_HCI_EN == TRUE)
}

/*******************************************************************************
**
** Function         btm_ble_free
**
** Description      free the control block variable values.
**
** Returns          void
**
*******************************************************************************/
void btm_ble_free (void)
{
    tBTM_BLE_CB *p_cb = &btm_cb.ble_ctr_cb;

    BTM_TRACE_DEBUG("%s", __func__);

    fixed_queue_free(p_cb->conn_pending_q, osi_free_func);
#if (BLE_42_SCAN_EN == TRUE)
    pkt_queue_destroy(p_cb->adv_rpt_queue, NULL);
    p_cb->adv_rpt_queue = NULL;

    osi_event_delete(p_cb->adv_rpt_ready);
    p_cb->adv_rpt_ready = NULL;
#endif // #if (BLE_42_SCAN_EN == TRUE)
}

static bool enable_topology_check_flag = true;
void esp_qa_enable_topology_check(bool enable)
{
    // This is a workaround: If the topology check is disabled, the 'Supported States' will not be checked.
    enable_topology_check_flag = enable;
}

#if (BLE_TOPOLOGY_CHECK == TRUE)
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
    if(!enable_topology_check_flag) return TRUE;
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
#endif // (BLE_TOPOLOGY_CHECK == TRUE)
/*******************************************************************************
 **
 ** Function         BTM_Ble_Authorization
 **
 ** Description      This function is used to authorize a specified device
 **
 ** Returns          TRUE or FALSE
 **
 *******************************************************************************/
BOOLEAN BTM_Ble_Authorization(BD_ADDR bd_addr, BOOLEAN authorize)
{
    if (bd_addr == NULL) {
        BTM_TRACE_ERROR("bd_addr is NULL");
        return FALSE;
    }

    if (btm_sec_dev_authorization(bd_addr, authorize)) {
        return TRUE;
    }

    BTM_TRACE_ERROR("Authorization fail");
    return FALSE;
}

#if (BLE_VENDOR_HCI_EN == TRUE)
/*******************************************************************************
**
** Function         BTM_BleClearAdv
**
** Description      This function is called to clear legacy advertising
**
** Parameter        p_clear_adv_cback - Command complete callback
**
*******************************************************************************/
BOOLEAN BTM_BleClearAdv(tBTM_CLEAR_ADV_CMPL_CBACK *p_clear_adv_cback)
{
    tBTM_BLE_CB *p_cb = &btm_cb.ble_ctr_cb;

    if (btsnd_hcic_ble_clear_adv() == FALSE) {
        BTM_TRACE_ERROR("%s: Unable to Clear Advertising", __FUNCTION__);
        return FALSE;
    }

    p_cb->inq_var.p_clear_adv_cb = p_clear_adv_cback;
    return TRUE;
}

BOOLEAN BTM_BleSetCsaSupport(UINT8 csa_select, tBTM_SET_CSA_SUPPORT_CMPL_CBACK *p_callback)
{
    if (btsnd_hcic_ble_set_csa_support(csa_select) != TRUE) {
        BTM_TRACE_ERROR("LE SetCsaSupport csa_select=%d: error", csa_select);
        return FALSE;
    }

    btm_cb.ble_ctr_cb.set_csa_support_cmpl_cb = p_callback;
    return TRUE;
}

BOOLEAN BTM_BleSetVendorEventMask(UINT32 evt_mask, tBTM_SET_VENDOR_EVT_MASK_CBACK *p_callback)
{
    if (btsnd_hcic_ble_set_vendor_evt_mask(evt_mask) != TRUE) {
        BTM_TRACE_ERROR("LE SetVendorEventMask evt_mask=%x: error", evt_mask);
        return FALSE;
    }

    btm_cb.ble_ctr_cb.set_vendor_evt_mask_cmpl_cb = p_callback;
    return TRUE;
}
#endif // #if (BLE_VENDOR_HCI_EN == TRUE)

BOOLEAN BTM_BleSetRpaTimeout(uint16_t rpa_timeout,tBTM_SET_RPA_TIMEOUT_CMPL_CBACK *p_set_rpa_timeout_cback)
{
    if ((btsnd_hcic_ble_set_rand_priv_addr_timeout(rpa_timeout)) != TRUE) {
        BTM_TRACE_ERROR("Set RPA Timeout error, rpa_timeout:0x%04x",rpa_timeout);
        return FALSE;
    }
    btm_cb.devcb.p_ble_set_rpa_timeout_cmpl_cb = p_set_rpa_timeout_cback;
    return TRUE;
}

BOOLEAN BTM_BleAddDevToResolvingList(BD_ADDR addr,
                                      uint8_t addr_type,
                                      uint8_t irk[],
                                      tBTM_ADD_DEV_TO_RESOLVING_LIST_CMPL_CBACK *p_add_dev_to_resolving_list_callback)
{
    UINT8 *local_irk = btm_cb.devcb.id_keys.irk;
    if ((btsnd_hcic_ble_add_device_resolving_list(addr_type, addr, irk, local_irk)) != TRUE) {
        BTM_TRACE_ERROR("Add device to resolving list error");
        return FALSE;
    }
    btm_cb.devcb.p_add_dev_to_resolving_list_cmpl_cb = p_add_dev_to_resolving_list_callback;
    return TRUE;
}

BOOLEAN BTM_BleSetPrivacyMode(UINT8 addr_type, BD_ADDR bd_addr, UINT8 privacy_mode, tBTM_SET_PRIVACY_MODE_CMPL_CBACK *p_callback)
{
    if (btsnd_hcic_ble_set_privacy_mode(addr_type, bd_addr, privacy_mode) != TRUE) {
        BTM_TRACE_ERROR("LE SetPrivacyMode Mode=%d: error", privacy_mode);
        return FALSE;
    }

    btm_cb.devcb.p_set_privacy_mode_cmpl_cb = p_callback;
    return TRUE;
}

#if (BLE_42_SCAN_EN == TRUE)
bool btm_ble_adv_pkt_ready(void)
{
    tBTM_BLE_CB *p_cb = &btm_cb.ble_ctr_cb;
    osi_thread_post_event(p_cb->adv_rpt_ready, OSI_THREAD_MAX_TIMEOUT);

    return true;
}

bool btm_ble_adv_pkt_post(pkt_linked_item_t *pkt)
{
    if (pkt == NULL) {
        return false;
    }

    tBTM_BLE_CB *p_cb = &btm_cb.ble_ctr_cb;
    pkt_queue_enqueue(p_cb->adv_rpt_queue, pkt);
    return true;
}
#endif // #if (BLE_42_SCAN_EN == TRUE)

#if (SMP_INCLUDED == TRUE)
/* Retrieve local IRK safely */
bool BTM_GetLocalIRK(uint8_t *irk)
{
    if (!irk) {
        return false;
    }

    memcpy(irk, btm_cb.devcb.id_keys.irk, sizeof(btm_cb.devcb.id_keys.irk));
    return true;
}
#endif // (SMP_INCLUDED == TRUE)
#endif  /* BLE_INCLUDED */

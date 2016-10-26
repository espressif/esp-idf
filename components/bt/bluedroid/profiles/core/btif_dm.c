/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
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

/************************************************************************************
 *
 *  Filename:      btif_dm.c
 *
 *  Description:   Contains Device Management (DM) related functionality
 *
 *
 ***********************************************************************************/

#define LOG_TAG "bt_btif_dm"

// #include <assert.h>
// #include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <sys/types.h>
// #include <unistd.h>

// #include <hardware/bluetooth.h>

// #include <cutils/properties.h>
#include "gki.h"
#include "btu.h"
// #include "btcore/include/bdaddr.h"
#include "bta_api.h"
#include "btif_api.h"
#include "btif_util.h"
#include "btif_dm.h"
// #include "btif_storage.h"
// #include "btif_hh.h"
// #include "btif_config.h"
// #include "btif_sdp.h"
// #include "bta_gatt_api.h"
// #include "device/include/interop.h"
// #include "include/stack_config.h"
// #include "osi/include/log.h"
#include "allocator.h"
#include "btm_int.h"
#include "bt_defs.h"
#include "future.h"
#include "stack_manager.h"

/******************************************************************************
**  Constants & Macros
******************************************************************************/
#define BTA_SERVICE_ID_TO_SERVICE_MASK(id)       (1 << (id))

/************************************************************************************
**  Static variables
************************************************************************************/
/******************************************************************************
**  Static functions
******************************************************************************/
/******************************************************************************
**  Externs
******************************************************************************/
extern bt_status_t btif_sdp_execute_service(BOOLEAN b_enable);

/******************************************************************************
**  Functions
******************************************************************************/

static void btif_dm_data_copy(uint16_t event, char *dst, char *src)
{
    tBTA_DM_SEC *dst_dm_sec = (tBTA_DM_SEC*)dst;
    tBTA_DM_SEC *src_dm_sec = (tBTA_DM_SEC*)src;

    if (!src_dm_sec)
        return;

    assert(dst_dm_sec);
    memcpy(dst_dm_sec, src_dm_sec, sizeof(tBTA_DM_SEC));

    if (event == BTA_DM_BLE_KEY_EVT) {
        dst_dm_sec->ble_key.p_key_value = osi_malloc(sizeof(tBTM_LE_KEY_VALUE));
        assert(src_dm_sec->ble_key.p_key_value);
        assert(dst_dm_sec->ble_key.p_key_value);
        memcpy(dst_dm_sec->ble_key.p_key_value, src_dm_sec->ble_key.p_key_value, sizeof(tBTM_LE_KEY_VALUE));
    }
}

static void btif_dm_data_free(uint16_t event, tBTA_DM_SEC *dm_sec)
{
    if (event == BTA_DM_BLE_KEY_EVT) {
        osi_free(dm_sec->ble_key.p_key_value);
    }
}

bt_status_t btif_in_execute_service_request(tBTA_SERVICE_ID service_id,
                                                BOOLEAN b_enable)
{
    BTIF_TRACE_DEBUG("%s service_id: %d", __FUNCTION__, service_id);
    /* Check the service_ID and invoke the profile's BT state changed API */
    switch (service_id) {
    case BTA_SDP_SERVICE_ID:
        btif_sdp_execute_service(b_enable);
        break;
    default:
        BTIF_TRACE_ERROR("%s: Unknown service being enabled", __FUNCTION__);
        return BT_STATUS_FAIL;
    }
    return BT_STATUS_SUCCESS;
}

void btif_dm_execute_service_request(UINT16 event, char *p_param)
{
    BOOLEAN b_enable = FALSE;
    if (event == BTIF_DM_ENABLE_SERVICE) {
        b_enable = TRUE;
    }

    btif_in_execute_service_request(*((tBTA_SERVICE_ID*)p_param), b_enable);
}

/*******************************************************************************
**
** Function         btif_dm_upstreams_cback
**
** Description      Executes UPSTREAMS events in btif context
**
** Returns          void
**
*******************************************************************************/
static void btif_dm_upstreams_evt(UINT16 event, char* p_param)
{
    tBTA_DM_SEC *p_data = (tBTA_DM_SEC*)p_param;
    tBTA_SERVICE_MASK service_mask;
    uint32_t i;
    BTIF_TRACE_EVENT("btif_dm_upstreams_cback  ev: %d\n", event);

    switch (event) {
    case BTA_DM_ENABLE_EVT:
        /* for each of the enabled services in the mask, trigger the profile
         * enable */
        service_mask = btif_get_enabled_services_mask();
        for (i=0; i <= BTA_MAX_SERVICE_ID; i++) {
            if (service_mask &
                (tBTA_SERVICE_MASK)(BTA_SERVICE_ID_TO_SERVICE_MASK(i))) {
                btif_in_execute_service_request(i, TRUE);
            }
        }
        btif_enable_bluetooth_evt(p_data->enable.status);
        break;
    case BTA_DM_DISABLE_EVT:
        /* for each of the enabled services in the mask, trigger the profile
         * disable */
        service_mask = btif_get_enabled_services_mask();
        for (i=0; i <= BTA_MAX_SERVICE_ID; i++) {
            if (service_mask &
                (tBTA_SERVICE_MASK)(BTA_SERVICE_ID_TO_SERVICE_MASK(i))) {
                btif_in_execute_service_request(i, FALSE);
            }
        }
        btif_disable_bluetooth_evt();
        break;
    case BTA_DM_PIN_REQ_EVT:
    case BTA_DM_AUTH_CMPL_EVT:
    case BTA_DM_BOND_CANCEL_CMPL_EVT:
    case BTA_DM_SP_CFM_REQ_EVT:
    case BTA_DM_SP_KEY_NOTIF_EVT:

    case BTA_DM_DEV_UNPAIRED_EVT:
    case BTA_DM_BUSY_LEVEL_EVT:
    case BTA_DM_LINK_UP_EVT:
    case BTA_DM_LINK_DOWN_EVT:
    case BTA_DM_HW_ERROR_EVT:

#if (defined(BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
    case BTA_DM_BLE_KEY_EVT:
    case BTA_DM_BLE_SEC_REQ_EVT:
    case BTA_DM_BLE_PASSKEY_NOTIF_EVT:
    case BTA_DM_BLE_PASSKEY_REQ_EVT:
    case BTA_DM_BLE_NC_REQ_EVT:
    case BTA_DM_BLE_OOB_REQ_EVT:
    case BTA_DM_BLE_LOCAL_IR_EVT:
    case BTA_DM_BLE_LOCAL_ER_EVT:
    case BTA_DM_BLE_AUTH_CMPL_EVT:
    case BTA_DM_LE_FEATURES_READ:
    case BTA_DM_ENER_INFO_READ:
#endif

    case BTA_DM_AUTHORIZE_EVT:
    case BTA_DM_SIG_STRENGTH_EVT:
    case BTA_DM_SP_RMT_OOB_EVT:
    case BTA_DM_SP_KEYPRESS_EVT:
    case BTA_DM_ROLE_CHG_EVT:

    default:
        BTIF_TRACE_WARNING( "btif_dm_cback : unhandled event (%d)", event );
        break;
    }

    btif_dm_data_free(event, p_data);
}

/*******************************************************************************
**
** Function         bte_dm_evt
**
** Description      Switches context from BTE to BTIF for all DM events
**
** Returns          void
**
*******************************************************************************/

void bte_dm_evt(tBTA_DM_SEC_EVT event, tBTA_DM_SEC *p_data)
{
    /* switch context to btif task context (copy full union size for convenience) */
    bt_status_t status = btif_transfer_context(btif_dm_upstreams_evt, (uint16_t)event,
                                (void*)p_data, sizeof(tBTA_DM_SEC), btif_dm_data_copy);

    /* catch any failed context transfers */
    ASSERTC(status == BT_STATUS_SUCCESS, "context transfer failed", status);
}

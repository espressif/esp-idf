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
#include <string.h>

#include "common/bt_defs.h"
#include "common/bt_target.h"
#include "common/bt_trace.h"
#include "device/controller.h"
#include "osi/alarm.h"
#include "osi/hash_map.h"
#include "osi/hash_functions.h"
#include "osi/thread.h"
#include "osi/mutex.h"

#include "l2c_int.h"
#include "stack/dyn_mem.h"
#include "stack/btu.h"
#include "btm_int.h"

#if SDP_INCLUDED == TRUE
#include "sdpint.h"
#endif

#if (BLE_INCLUDED == TRUE)
#include "stack/gatt_api.h"
#include "gatt_int.h"
#if SMP_INCLUDED == TRUE
#include "smp_int.h"
#endif
#endif

#define BTU_TASK_PINNED_TO_CORE         (TASK_PINNED_TO_CORE)
#define BTU_TASK_STACK_SIZE             (BT_BTU_TASK_STACK_SIZE + BT_TASK_EXTRA_STACK_SIZE)
#define BTU_TASK_PRIO                   (BT_TASK_MAX_PRIORITIES - 5)
#define BTU_TASK_NAME                   "btuT"

hash_map_t *btu_general_alarm_hash_map;
osi_mutex_t btu_general_alarm_lock;
static const size_t BTU_GENERAL_ALARM_HASH_MAP_SIZE = 34;

hash_map_t *btu_oneshot_alarm_hash_map;
osi_mutex_t btu_oneshot_alarm_lock;
static const size_t BTU_ONESHOT_ALARM_HASH_MAP_SIZE = 34;

hash_map_t *btu_l2cap_alarm_hash_map;
osi_mutex_t btu_l2cap_alarm_lock;
static const size_t BTU_L2CAP_ALARM_HASH_MAP_SIZE = 34;

osi_thread_t *btu_thread = NULL;

extern void PLATFORM_DisableHciTransport(UINT8 bDisable);

extern void btu_task_thread_handler(void *arg);
void btu_task_start_up(void * param);
void btu_task_shut_down(void);

/*****************************************************************************
**                          V A R I A B L E S                                *
******************************************************************************/
// TODO(cmanton) Move this out of this file
const BD_ADDR   BT_BD_ANY = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
/*****************************************************************************
**
** Function         btu_init_core
**
** Description      Initialize control block memory for each core component.
**
**
** Returns          void
**
******************************************************************************/
void btu_init_core(void)
{
    /* Initialize the mandatory core stack components */
    btm_init();

    l2c_init();

#if (defined(SDP_INCLUDED) && SDP_INCLUDED == TRUE)
    sdp_init();
#endif

#if BLE_INCLUDED == TRUE
#if (defined(GATT_INCLUDED) && GATT_INCLUDED == true)
    gatt_init();
#endif
#if (defined(SMP_INCLUDED) && SMP_INCLUDED == TRUE)
    SMP_Init();
#endif
    btm_ble_init();
#endif
}

/*****************************************************************************
**
** Function         btu_free_core
**
** Description      Releases control block memory for each core component.
**
**
** Returns          void
**
******************************************************************************/
void btu_free_core(void)
{
    // Free the mandatory core stack components
    l2c_free();

#if (defined(SDP_INCLUDED) && SDP_INCLUDED == TRUE)
    sdp_deinit();
#endif

#if BLE_INCLUDED == TRUE
#if (defined(GATT_INCLUDED) && GATT_INCLUDED == true)
    gatt_free();
#endif
#if SMP_INCLUDED == TRUE
    SMP_Free();
#endif
    btm_ble_free();
#endif
    btm_free();
}

/*****************************************************************************
**
** Function         BTU_StartUp
**
** Description      Initializes the BTU control block.
**
**                  NOTE: Must be called before creating any tasks
**                      (RPC, BTU, HCIT, APPL, etc.)
**
** Returns          void
**
******************************************************************************/
void BTU_StartUp(void)
{
#if BTU_DYNAMIC_MEMORY
    btu_cb_ptr = (tBTU_CB *)osi_malloc(sizeof(tBTU_CB));
#endif /* #if BTU_DYNAMIC_MEMORY */
    memset (&btu_cb, 0, sizeof (tBTU_CB));
    btu_cb.trace_level = HCI_INITIAL_TRACE_LEVEL;

    btu_general_alarm_hash_map = hash_map_new(BTU_GENERAL_ALARM_HASH_MAP_SIZE,
                                 hash_function_pointer, NULL, (data_free_fn)osi_alarm_free, NULL);
    if (btu_general_alarm_hash_map == NULL) {
        goto error_exit;
    }

    osi_mutex_new(&btu_general_alarm_lock);

    btu_oneshot_alarm_hash_map = hash_map_new(BTU_ONESHOT_ALARM_HASH_MAP_SIZE,
                                 hash_function_pointer, NULL, (data_free_fn)osi_alarm_free, NULL);
    if (btu_oneshot_alarm_hash_map == NULL) {
        goto error_exit;
    }

    osi_mutex_new(&btu_oneshot_alarm_lock);

    btu_l2cap_alarm_hash_map = hash_map_new(BTU_L2CAP_ALARM_HASH_MAP_SIZE,
                                            hash_function_pointer, NULL, (data_free_fn)osi_alarm_free, NULL);
    if (btu_l2cap_alarm_hash_map == NULL) {
        goto error_exit;
    }

    osi_mutex_new(&btu_l2cap_alarm_lock);

    btu_thread = osi_thread_create(BTU_TASK_NAME, BTU_TASK_STACK_SIZE, BTU_TASK_PRIO, BTU_TASK_PINNED_TO_CORE, 1);
    if (btu_thread == NULL) {
        goto error_exit;
    }

    if (btu_task_post(SIG_BTU_START_UP, NULL, OSI_THREAD_MAX_TIMEOUT) == false) {
        goto error_exit;
    }

    return;

error_exit:;
    LOG_ERROR("%s Unable to allocate resources for bt_workqueue", __func__);
    BTU_ShutDown();
}

/*****************************************************************************
**
** Function         BTU_ShutDown
**
** Description      Deinitializes the BTU control block.
**
** Returns          void
**
******************************************************************************/
void BTU_ShutDown(void)
{
#if BTU_DYNAMIC_MEMORY
    FREE_AND_RESET(btu_cb_ptr);
#endif
    btu_task_shut_down();

    hash_map_free(btu_general_alarm_hash_map);
    osi_mutex_free(&btu_general_alarm_lock);

    hash_map_free(btu_oneshot_alarm_hash_map);
    osi_mutex_free(&btu_oneshot_alarm_lock);

    hash_map_free(btu_l2cap_alarm_hash_map);
    osi_mutex_free(&btu_l2cap_alarm_lock);

    if (btu_thread) {
        osi_thread_free(btu_thread);
        btu_thread = NULL;
    }

    btu_general_alarm_hash_map = NULL;
    btu_oneshot_alarm_hash_map = NULL;
    btu_l2cap_alarm_hash_map = NULL;
}

/*****************************************************************************
**
** Function         BTU_BleAclPktSize
**
** Description      export the BLE ACL packet size.
**
** Returns          UINT16
**
******************************************************************************/
UINT16 BTU_BleAclPktSize(void)
{
#if BLE_INCLUDED == TRUE
    return controller_get_interface()->get_acl_packet_size_ble();
#else
    return 0;
#endif
}

#if SCAN_QUEUE_CONGEST_CHECK
bool BTU_check_queue_is_congest(void)
{
    if (osi_thread_queue_wait_size(btu_thread, 0) >= BT_QUEUE_CONGEST_SIZE) {
        return true;
    }

    return false;
}
#endif

/******************************************************************************
 *
 *  Copyright (C) 2002-2012 Broadcom Corporation
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
 *  This file contains the definition of the btm control block when
 *  BTM_DYNAMIC_MEMORY is used.
 *
 ******************************************************************************/

#include "stack/bt_types.h"
#include "common/bt_target.h"
#include <string.h>
#include "btm_int.h"
#include "osi/allocator.h"

/* Global BTM control block structure
*/
#if BTM_DYNAMIC_MEMORY == FALSE
tBTM_CB  btm_cb;
#else
tBTM_CB  *btm_cb_ptr;
#endif

#if (BLE_50_FEATURE_SUPPORT == TRUE)
extern void btm_ble_extendadvcb_init(void);
extern void btm_ble_advrecod_init(void);
#endif


/*******************************************************************************
**
** Function         btm_init
**
** Description      This function is called at BTM startup to allocate the
**                  control block (if using dynamic memory), and initializes the
**                  tracing level.  It then initializes the various components of
**                  btm.
**
** Returns          void
**
*******************************************************************************/
void btm_init (void)
{
#if BTM_DYNAMIC_MEMORY
    btm_cb_ptr = (tBTM_CB *)osi_malloc(sizeof(tBTM_CB));
#endif /* #if BTM_DYNAMIC_MEMORY */
    /* All fields are cleared; nonzero fields are reinitialized in appropriate function */
    memset(&btm_cb, 0, sizeof(tBTM_CB));
    btm_cb.page_queue = fixed_queue_new(QUEUE_SIZE_MAX);
    btm_cb.sec_pending_q = fixed_queue_new(QUEUE_SIZE_MAX);

#if defined(BTM_INITIAL_TRACE_LEVEL)
    btm_cb.trace_level = BTM_INITIAL_TRACE_LEVEL;
#else
    btm_cb.trace_level = BT_TRACE_LEVEL_NONE;
#endif
    /* Initialize BTM component structures */
    btm_inq_db_init();                  /* Inquiry Database and Structures */
    btm_acl_init();                     /* ACL Database and Structures */
#if (SMP_INCLUDED == TRUE)
    btm_sec_init(BTM_SEC_MODE_SP);      /* Security Manager Database and Structures */
#endif  ///SMP_INCLUDED == TRUE
#if BTM_SCO_INCLUDED == TRUE
    btm_sco_init();                     /* SCO Database and Structures (If included) */
#endif

    btm_dev_init();                     /* Device Manager Structures & HCI_Reset */
#if BLE_INCLUDED == TRUE
    btm_ble_lock_init();
    btm_ble_sem_init();
#endif
    btm_sec_dev_init();
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    btm_ble_extendadvcb_init();
    btm_ble_advrecod_init();
#endif

}


/*******************************************************************************
**
** Function         btm_free
**
** Description      This function is called at btu core free the fixed queue
**
** Returns          void
**
*******************************************************************************/
void btm_free(void)
{
    fixed_queue_free(btm_cb.page_queue, osi_free_func);
    fixed_queue_free(btm_cb.sec_pending_q, osi_free_func);
    btm_acl_free();
    btm_sec_dev_free();
#if BTM_DYNAMIC_MEMORY
    FREE_AND_RESET(btm_cb_ptr);
#endif
#if BLE_INCLUDED == TRUE
    btm_ble_lock_free();
    btm_ble_sem_free();
#endif
}

uint8_t btm_acl_active_count(void)
{
    list_node_t *p_node = NULL;
    tACL_CONN *p_acl_conn = NULL;
    uint8_t count = 0;

    for (p_node = list_begin(btm_cb.p_acl_db_list); p_node; p_node = list_next(p_node)) {
        p_acl_conn = list_node(p_node);
        if (p_acl_conn && p_acl_conn->in_use) {
            count++;
        }
    }

    return count;
}

uint8_t btdm_sec_dev_active_count(void)
{
    tBTM_SEC_DEV_REC *p_dev_rec = NULL;
    list_node_t *p_node = NULL;
    uint8_t count = 0;

    /* First look for the non-paired devices for the oldest entry */
    for (p_node = list_begin(btm_cb.p_sec_dev_rec_list); p_node; p_node = list_next(p_node)) {
        p_dev_rec = list_node(p_node);
        if (p_dev_rec && (p_dev_rec->sec_flags & BTM_SEC_IN_USE)) {
            count++;
        }
    }

    return count;
}

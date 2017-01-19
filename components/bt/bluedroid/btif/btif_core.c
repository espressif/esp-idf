/******************************************************************************
 *
 *  Copyright (C) 2014 The Android Open Source Project
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
 *  Filename:      btif_core.c
 *
 *  Description:   Contains core functionality related to interfacing between
 *                 Bluetooth HAL and BTE core stack.
 *
 ***********************************************************************************/

#include <ctype.h>
// #include <cutils/properties.h>
// #include <dirent.h>
// #include <fcntl.h>
// #include <hardware/bluetooth.h>
#include <stdlib.h>
#include <string.h>
// #include <sys/stat.h>
// #include <sys/types.h>

#define LOG_TAG "bt_btif_core"
// #include "btcore/include/bdaddr.h"

#include "bdaddr.h"
// #include "bt_utils.h"
#include "bta_api.h"
#include "bte.h"
#include "btif_api.h"
// #include "btif_av.h"
// #include "btif_config.h"
// #include "btif_pan.h"
// #include "btif_profile_queue.h"
// #include "btif_config.h"
// #include "btif_sock.h"
// #include "btif_storage.h"
#include "btif_util.h"
#include "btu.h"
#include "controller.h"
#include "fixed_queue.h"
#include "future.h"
#include "gki.h"
#include "osi.h"
// #include "osi/include/log.h"
#include "stack_manager.h"
#include "thread.h"
#include "btif_common.h"
#include "btif_dm.h"
/************************************************************************************
**  Constants & Macros
************************************************************************************/

/************************************************************************************
**  Local type definitions
************************************************************************************/

/************************************************************************************
**  Static variables
************************************************************************************/

static tBTA_SERVICE_MASK btif_enabled_services = 0;

static fixed_queue_t *btif_msg_queue = NULL;
static xTaskHandle  xBtifTaskHandle = NULL;

/************************************************************************************
**  Static functions
************************************************************************************/

/* sends message to btif task */
static void btif_sendmsg(void *p_msg);
static void btif_thread_post(uint32_t sig);
/************************************************************************************
**  Externs
************************************************************************************/
static fixed_queue_t *xBtifQueue = NULL;

/** TODO: Move these to _common.h */
void bte_main_boot_entry(void *);
void bte_main_disable(void);
void bte_main_shutdown(void);
void btif_dm_execute_service_request(UINT16 event, char *p_param);

/*******************************************************************************
**
** Function         btif_context_switched
**
** Description      Callback used to execute transferred context callback
**
**                  p_msg : message to be executed in btif context
**
** Returns          void
**
*******************************************************************************/

static void btif_context_switched(void *p_msg)
{

    BTIF_TRACE_VERBOSE("btif_context_switched");

    tBTIF_CONTEXT_SWITCH_CBACK *p = (tBTIF_CONTEXT_SWITCH_CBACK *) p_msg;

    /* each callback knows how to parse the data */
    if (p->p_cb) {
        p->p_cb(p->event, p->p_param);
    }
}


/*******************************************************************************
**
** Function         btif_transfer_context
**
** Description      This function switches context to btif task
**
**                  p_cback   : callback used to process message in btif context
**                  event     : event id of message
**                  p_params  : parameter area passed to callback (copied)
**                  param_len : length of parameter area
**                  p_copy_cback : If set this function will be invoked for deep copy
**
** Returns          void
**
*******************************************************************************/

bt_status_t btif_transfer_context (tBTIF_CBACK *p_cback, UINT16 event, char *p_params, int param_len, tBTIF_COPY_CBACK *p_copy_cback)
{
    tBTIF_CONTEXT_SWITCH_CBACK *p_msg;

    BTIF_TRACE_VERBOSE("btif_transfer_context event %d, len %d", event, param_len);

    /* allocate and send message that will be executed in btif context */
    if ((p_msg = (tBTIF_CONTEXT_SWITCH_CBACK *) GKI_getbuf(sizeof(tBTIF_CONTEXT_SWITCH_CBACK) + param_len)) != NULL) {
        p_msg->hdr.event = BT_EVT_CONTEXT_SWITCH_EVT; /* internal event */
        p_msg->p_cb = p_cback;

        p_msg->event = event;                         /* callback event */

        /* check if caller has provided a copy callback to do the deep copy */
        if (p_copy_cback) {
            p_copy_cback(event, p_msg->p_param, p_params);
        } else if (p_params) {
            memcpy(p_msg->p_param, p_params, param_len);  /* callback parameter data */
        }

        btif_sendmsg(p_msg);
        return BT_STATUS_SUCCESS;
    } else {
        /* let caller deal with a failed allocation */
        return BT_STATUS_NOMEM;
    }
}

int btif_is_enabled(void)
{
    return (stack_manager_is_stack_running());
}

void btif_init_ok(void)
{
    BTIF_TRACE_DEBUG("btif_task: received trigger stack init event");
    future_ready(stack_manager_get_hack_future(), FUTURE_SUCCESS);
}

/*******************************************************************************
**
** Function         btif_enable_bluetooth_evt
**
** Description      Event indicating bluetooth enable is completed
**                  Notifies HAL user with updated adapter state
**
** Returns          void
**
*******************************************************************************/

void btif_enable_bluetooth_evt(tBTA_STATUS status)
{
    if (status == BTA_SUCCESS) {
        future_ready(stack_manager_get_hack_future(), FUTURE_SUCCESS);
    } else {
        future_ready(stack_manager_get_hack_future(), FUTURE_FAIL);
    }
}

/*******************************************************************************
**
** Function         btif_disable_bluetooth_evt
**
** Description      Event notifying BT disable is now complete.
**                  Terminates main stack tasks and notifies HAL
**                  user with updated BT state.
**
** Returns          void
**
*******************************************************************************/

void btif_disable_bluetooth_evt(void)
{
    BTIF_TRACE_DEBUG("%s", __FUNCTION__);

    /* callback to HAL */
    future_ready(stack_manager_get_hack_future(), FUTURE_SUCCESS);
}

/*******************************************************************************
**
** Function         btif_task
**
** Description      BTIF task handler managing all messages being passed
**                  Bluetooth HAL and BTA.
**
** Returns          void
**
*******************************************************************************/
static void bt_jni_msg_ready(fixed_queue_t *queue)
{
    BT_HDR *p_msg;
    while (!fixed_queue_is_empty(queue)) {
        p_msg = (BT_HDR *)fixed_queue_dequeue(queue);
        BTIF_TRACE_VERBOSE("btif task fetched event %x", p_msg->event);
        switch (p_msg->event) {
        case BT_EVT_CONTEXT_SWITCH_EVT:
            btif_context_switched(p_msg);
            break;
        default:
            BTIF_TRACE_ERROR("unhandled btif event (%d)", p_msg->event & BT_EVT_MASK);                      break;
        }
        GKI_freebuf(p_msg);
    }
}

/*******************************************************************************
**
** Function         btif_sendmsg
**
** Description      Sends msg to BTIF task
**
** Returns          void
**
*******************************************************************************/

void btif_sendmsg(void *p_msg)
{
    fixed_queue_enqueue(btif_msg_queue, p_msg);
    btif_thread_post(SIG_BTIF_WORK);
}

static void btif_thread_post(uint32_t sig)
{
    BtTaskEvt_t *evt = (BtTaskEvt_t *)osi_malloc(sizeof(BtTaskEvt_t));
    if (evt == NULL) {
        return;
    }

    evt->sig = sig;
    evt->par = 0;

    if (xQueueSend(xBtifQueue, &evt, 10 / portTICK_RATE_MS) != pdTRUE) {
        ets_printf("xBtifQueue failed\n");
    }
}

/*****************************************************************************
**
** Function         btif_task_thread_handler
**
** Description      Process BTif Task Thread.
******************************************************************************/
void btif_task_thread_handler(void *arg)
{
    BtTaskEvt_t *e;
    for (;;) {
        if (pdTRUE == xQueueReceive(xBtifQueue, &e, (portTickType)portMAX_DELAY)) {
            if (e->sig == SIG_BTIF_WORK) {
                fixed_queue_process(btif_msg_queue);
            }
            osi_free(e);
        }
    }
}

/*******************************************************************************
**
** Function         btif_init_bluetooth
**
** Description      Creates BTIF task and prepares BT scheduler for startup
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_init_bluetooth(void)
{
    bte_main_boot_entry(btif_init_ok);

    btif_msg_queue = fixed_queue_new(SIZE_MAX);
    if (btif_msg_queue == NULL) {
        goto error_exit;
    }
    xBtifQueue = xQueueCreate(60, sizeof(void *));
    xTaskCreatePinnedToCore(btif_task_thread_handler, "BtifT", 2048+1024, NULL, configMAX_PRIORITIES - 1, &xBtifTaskHandle, 0);
    fixed_queue_register_dequeue(btif_msg_queue, bt_jni_msg_ready);

    return BT_STATUS_SUCCESS;

error_exit:;
    btif_shutdown_bluetooth();

    return BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         btif_enable_bluetooth
**
** Description      Inititates shutdown of Bluetooth system.
**                  Any active links will be dropped and device entering
**                  non connectable/discoverable mode
**
** Returns          void
**
*******************************************************************************/
bt_status_t btif_enable_bluetooth(void)
{
    BTIF_TRACE_DEBUG("BTIF ENABLE BLUETOOTH");

    BTA_EnableBluetooth(bte_dm_evt);

    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_disable_bluetooth
**
** Description      Inititates shutdown of Bluetooth system.
**                  Any active links will be dropped and device entering
**                  non connectable/discoverable mode
**
** Returns          void
**
*******************************************************************************/
bt_status_t btif_disable_bluetooth(void)
{
    BTIF_TRACE_DEBUG("BTIF DISABLE BLUETOOTH");

    // btif_dm_on_disable();
    /* cleanup rfcomm & l2cap api */
    // btif_sock_cleanup();
    // btif_pan_cleanup();
    BTA_DisableBluetooth();

    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_shutdown_bluetooth
**
** Description      Finalizes BT scheduler shutdown and terminates BTIF
**                  task.
**
** Returns          void
**
*******************************************************************************/

bt_status_t btif_shutdown_bluetooth(void)
{
    BTIF_TRACE_DEBUG("%s", __FUNCTION__);

    fixed_queue_unregister_dequeue(btif_msg_queue);
    fixed_queue_free(btif_msg_queue, NULL);
    btif_msg_queue = NULL;

    vTaskDelete(xBtifTaskHandle);
    xBtifTaskHandle = NULL;

    vQueueDelete(xBtifQueue);
    xBtifQueue = NULL;

    bte_main_shutdown();

    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_get_enabled_services_mask
**
** Description      Fetches currently enabled services
**
** Returns          tBTA_SERVICE_MASK
**
*******************************************************************************/

tBTA_SERVICE_MASK btif_get_enabled_services_mask(void)
{
    return btif_enabled_services;
}

/*******************************************************************************
**
** Function         btif_enable_service
**
** Description      Enables the service 'service_ID' to the service_mask.
**                  Upon BT enable, BTIF core shall invoke the BTA APIs to
**                  enable the profiles
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_enable_service(tBTA_SERVICE_ID service_id)
{
    tBTA_SERVICE_ID *p_id = &service_id;

    /* If BT is enabled, we need to switch to BTIF context and trigger the
     * enable for that profile
     *
     * Otherwise, we just set the flag. On BT_Enable, the DM will trigger
     * enable for the profiles that have been enabled */

    btif_enabled_services |= (1 << service_id);

    BTIF_TRACE_DEBUG("%s: current services:0x%x", __FUNCTION__, btif_enabled_services);

    if (btif_is_enabled()) {
        btif_transfer_context(btif_dm_execute_service_request,
                              BTIF_DM_ENABLE_SERVICE,
                              (char *)p_id, sizeof(tBTA_SERVICE_ID), NULL);
    }

    return BT_STATUS_SUCCESS;
}
/*******************************************************************************
**
** Function         btif_disable_service
**
** Description      Disables the service 'service_ID' to the service_mask.
**                  Upon BT disable, BTIF core shall invoke the BTA APIs to
**                  disable the profiles
**
** Returns          bt_status_t
**
*******************************************************************************/
bt_status_t btif_disable_service(tBTA_SERVICE_ID service_id)
{
    tBTA_SERVICE_ID *p_id = &service_id;

    /* If BT is enabled, we need to switch to BTIF context and trigger the
     * disable for that profile so that the appropriate uuid_property_changed will
     * be triggerred. Otherwise, we just need to clear the service_id in the mask
     */

    btif_enabled_services &=  (tBTA_SERVICE_MASK)(~(1 << service_id));

    BTIF_TRACE_DEBUG("%s: Current Services:0x%x", __FUNCTION__, btif_enabled_services);

    if (btif_is_enabled()) {
        btif_transfer_context(btif_dm_execute_service_request,
                              BTIF_DM_DISABLE_SERVICE,
                              (char *)p_id, sizeof(tBTA_SERVICE_ID), NULL);
    }

    return BT_STATUS_SUCCESS;
}

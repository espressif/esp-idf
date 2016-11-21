#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "fixed_queue.h"
#include "gki.h"
#include "bt_defs.h"
#include "bt_trace.h"
#include "bt_types.h"
#include "allocator.h"

#include "bta_api.h"
#include "bta_gatt_api.h"
#include "bt_app_common.h"

#include "controller.h"
#include "thread.h"
#include "bt_app_common.h"

static fixed_queue_t *bt_app_msg_queue;

xQueueHandle xBtAppQueue;
xTaskHandle xBtAppTaskHandle;

static void bt_app_context_switched(void *p_msg);
static void bt_app_send_msg(void *p_msg);
static void bt_app_task_handler(void *arg);
static void bta_app_msg_ready(fixed_queue_t *queue);
static void bt_app_task_shut_down(void);


extern void app_main_entry(void);

static void bt_app_task_handler(void *arg)
{
    app_main_entry();
    BtTaskEvt_t *e;
    for (;;) {
        if (pdTRUE == xQueueReceive(xBtAppQueue, &e, (portTickType)portMAX_DELAY)) {
            if (e->sig == 0xff) {
                fixed_queue_process(bt_app_msg_queue);
            }
            osi_free(e);
        }
    }
}

static void bt_app_task_post(void)
{
     BtTaskEvt_t *evt = (BtTaskEvt_t *)osi_malloc(sizeof(BtTaskEvt_t));
     if (evt == NULL)
        return;

     evt->sig = 0xff;
     evt->par = 0;

     if (xQueueSend(xBtAppQueue, &evt, 10/portTICK_RATE_MS) != pdTRUE) {
         ets_printf("btdm_post failed\n");
     }
}

static void bta_app_msg_ready(fixed_queue_t *queue) {
    BT_HDR *p_msg;
    while (!fixed_queue_is_empty(queue)) {
        p_msg = (BT_HDR *)fixed_queue_dequeue(queue);
        LOG_ERROR("bta_app_msg_ready, evt: %d\n", p_msg->event);
        switch (p_msg->event) {
        case BT_EVT_APP_CONTEXT_SWITCH:
            bt_app_context_switched(p_msg);
            break;
        default:
            LOG_ERROR("unhandled BT_APP event (%d)\n", p_msg->event & BT_EVT_MASK);
            break;
        }
        GKI_freebuf(p_msg);
    }
}

static void bt_app_context_switched(void *p_msg)
{
    tBTAPP_CONTEXT_SWITCH_CBACK *p = (tBTAPP_CONTEXT_SWITCH_CBACK *) p_msg;
    
    if (p->p_cb)
        p->p_cb(p->event, p->p_param);
}

static void bt_app_send_msg(void *p_msg)
{
    if (bt_app_msg_queue) {
        fixed_queue_enqueue(bt_app_msg_queue, p_msg);
        bt_app_task_post();
    }
}

bt_status_t bt_app_transfer_context (tBTAPP_CBACK *p_cback, UINT16 event, char* p_params, int param_len, tBTAPP_COPY_CBACK *p_copy_cback)
{
    tBTAPP_CONTEXT_SWITCH_CBACK *p_msg;

    LOG_ERROR("btapp_transfer_context evt %d, len %d\n", event, param_len);

    /* allocate and send message that will be executed in btif context */
    if ((p_msg = (tBTAPP_CONTEXT_SWITCH_CBACK *) GKI_getbuf(sizeof(tBTAPP_CONTEXT_SWITCH_CBACK) + param_len)) != NULL)
    {
        p_msg->hdr.event = BT_EVT_APP_CONTEXT_SWITCH; /* internal event */
        p_msg->p_cb = p_cback;

        p_msg->event = event;                         /* callback event */

        /* check if caller has provided a copy callback to do the deep copy */
        if (p_copy_cback)
        {
            p_copy_cback(event, p_msg->p_param, p_params);
        }
        else if (p_params)
        {
            memcpy(p_msg->p_param, p_params, param_len);  /* callback parameter data */
        }

        bt_app_send_msg(p_msg);
        return BT_STATUS_SUCCESS;
    }
    else
    {
        /* let caller deal with a failed allocation */
        return BT_STATUS_NOMEM;
    }
}

void bt_app_task_start_up(void)
{
    bt_app_msg_queue = fixed_queue_new(SIZE_MAX);
    if (bt_app_msg_queue == NULL)
        goto error_exit;
    //ke_event_callback_set(KE_EVENT_BT_APP_TASK, &bt_app_task_handler);

    xBtAppQueue = xQueueCreate(3, sizeof(void *));
    xTaskCreate(bt_app_task_handler, "BtaApp1T", 2048, NULL, configMAX_PRIORITIES - 3, xBtAppTaskHandle);

    fixed_queue_register_dequeue(bt_app_msg_queue, bta_app_msg_ready);

    return;

error_exit:
    LOG_ERROR("%s Unable to allocate resources for bt_app\n", __func__);
    bt_app_task_shut_down();
}

static void bt_app_task_shut_down(void)
{
    fixed_queue_unregister_dequeue(bt_app_msg_queue);
    fixed_queue_free(bt_app_msg_queue, NULL);
    bt_app_msg_queue = NULL;

    vTaskDelete(xBtAppTaskHandle);
    vQueueDelete(xBtAppQueue);
}

/*
static void bt_app_upstreams_evt(UINT16 event, char *p_param)
{
    tBTA_DM_SEC *p_data = (tBTA_DM_SEC*)p_param;
    switch (event) {
    default:
        break;
    }
}

static void bt_stack_evt(tBTA_DM_SEC_EVT event, tBTA_DM_SEC* p_data)
{
    LOG_ERROR("bt_stack_evt: %d\n", (uint16_t)event);
    bt_app_transfer_context(bt_app_upstreams_evt, (uint16_t)event, 
           (void *)p_data, sizeof(tBTA_DM_SEC), NULL);
}
*/

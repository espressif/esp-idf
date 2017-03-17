#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "bt_app_common.h"
#include "freertos/xtensa_api.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

int bt_app_trace_level = BT_APP_TRACE_LEVEL_DEBUG;

static bool bt_app_post_msg(bt_app_msg_t *msg);
static void bt_app_context_switched(bt_app_msg_t *msg);
static void bt_app_task_handler(void *arg);
extern void app_main_entry(void);

static xQueueHandle bt_app_task_queue = NULL;
static xTaskHandle bt_app_task_handle = NULL;

bool bt_app_transfer_context (bt_app_cb_t p_cback, uint16_t event, void *p_params, int param_len, bt_app_copy_cb_t p_copy_cback)
{
    bt_app_msg_t msg;
    
    memset(&msg, 0, sizeof(bt_app_msg_t));
    BT_APP_TRACE_EVENT("btapp_transfer_context evt 0x%x, len %d\n", event, param_len);

    msg.sig = BT_APP_SIG_CONTEXT_SWITCH;
    msg.event = event;
    msg.cb = p_cback;
    
    if (param_len == 0) {
        return bt_app_post_msg(&msg);
    } else if (p_params && param_len > 0) {
        if ((msg.param = malloc(param_len)) != NULL) {
            memcpy(msg.param, p_params, param_len);
            /* check if caller has provided a copy callback to do the deep copy */
            if (p_copy_cback) {
                p_copy_cback(&msg, msg.param, p_params);
            }
            return bt_app_post_msg(&msg);
        }
    }

    return false;
}

static bool bt_app_post_msg(bt_app_msg_t *msg)
{
    if (msg == NULL) {
        return false;
    }

    if (xQueueSend(bt_app_task_queue, msg, 10 / portTICK_RATE_MS) != pdTRUE) {
        BT_APP_TRACE_ERROR("bt_app msg post failed\n");
        return false;
    }
    return true;
}

static void bt_app_context_switched(bt_app_msg_t *msg)
{
    BT_APP_TRACE_DEBUG("bt app context switched\n");
    if (msg->cb) {
        msg->cb(msg->event, msg->param);
    }
}

static void bt_app_task_handler(void *arg)
{
    app_main_entry();
    bt_app_msg_t msg;
    for (;;) {
        if (pdTRUE == xQueueReceive(bt_app_task_queue, &msg, (portTickType)portMAX_DELAY)) {
	    BT_APP_TRACE_EVENT("btapp handle evt, sig 0x%x, 0x%x\n", msg.sig, msg.event);
            switch (msg.sig) {
            case BT_APP_SIG_CONTEXT_SWITCH:
                bt_app_context_switched(&msg);
                break;
            default:
                BT_APP_TRACE_WARNING("unhandled BT_APP event (%d)\n", msg.sig);
                break;
            } // switch (msg.sig)
            
            if (msg.param) {
                free(msg.param);
            }
        }
    }
}

void bt_app_task_start_up(void)
{
    bt_app_task_queue = xQueueCreate(10, sizeof(bt_app_msg_t));
    xTaskCreate(bt_app_task_handler, "BtAppT", 2048, NULL, configMAX_PRIORITIES - 3, bt_app_task_handle);
    return;
}

void bt_app_task_shut_down(void)
{
    if (bt_app_task_handle) {
        vTaskDelete(bt_app_task_handle);
        bt_app_task_handle = NULL;
    }
    if (bt_app_task_queue) {
        vQueueDelete(bt_app_task_queue);
        bt_app_task_queue = NULL;
    }
}

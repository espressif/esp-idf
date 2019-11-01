/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/xtensa_api.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bt_app_core.h"

#include "media_hal.h"

#ifdef CONFIG_A2DP_MEDIA_RINGBUF
#define MEDIA_DATA_BUF_SIZE (100*1024)  //100K

static RingbufHandle_t media_data_buf = NULL;
static xTaskHandle bt_media_task_handle = NULL;
#endif

static void bt_app_task_handler(void *arg);
static bool bt_app_send_msg(bt_app_msg_t *msg);
static void bt_app_work_dispatched(bt_app_msg_t *msg);

static xQueueHandle bt_app_task_queue = NULL;
static xTaskHandle bt_app_task_handle = NULL;

bool bt_app_work_dispatch(bt_app_cb_t p_cback, uint16_t event, void *p_params, int param_len, bt_app_copy_cb_t p_copy_cback)
{
    ESP_LOGD(BT_APP_CORE_TAG, "%s event 0x%x, param len %d", __func__, event, param_len);

    bt_app_msg_t msg;
    memset(&msg, 0, sizeof(bt_app_msg_t));

    msg.sig = BT_APP_SIG_WORK_DISPATCH;
    msg.event = event;
    msg.cb = p_cback;

    if (param_len == 0) {
        return bt_app_send_msg(&msg);
    } else if (p_params && param_len > 0) {
        if ((msg.param = malloc(param_len)) != NULL) {
            memcpy(msg.param, p_params, param_len);
            /* check if caller has provided a copy callback to do the deep copy */
            if (p_copy_cback) {
                p_copy_cback(&msg, msg.param, p_params);
            }
            return bt_app_send_msg(&msg);
        }
    }

    return false;
}

static bool bt_app_send_msg(bt_app_msg_t *msg)
{
    if (msg == NULL) {
        return false;
    }

    if (xQueueSend(bt_app_task_queue, msg, 10 / portTICK_RATE_MS) != pdTRUE) {
        ESP_LOGE(BT_APP_CORE_TAG, "%s xQueue send failed", __func__);
        return false;
    }
    return true;
}

static void bt_app_work_dispatched(bt_app_msg_t *msg)
{
    if (msg->cb) {
        msg->cb(msg->event, msg->param);
    }
}

static void bt_app_task_handler(void *arg)
{
    bt_app_msg_t msg;
    for (;;) {
        if (pdTRUE == xQueueReceive(bt_app_task_queue, &msg, (portTickType)portMAX_DELAY)) {
            ESP_LOGD(BT_APP_CORE_TAG, "%s, sig 0x%x, 0x%x", __func__, msg.sig, msg.event);
            switch (msg.sig) {
            case BT_APP_SIG_WORK_DISPATCH:
                bt_app_work_dispatched(&msg);
                break;
            default:
                ESP_LOGW(BT_APP_CORE_TAG, "%s, unhandled sig: %d", __func__, msg.sig);
                break;
            } // switch (msg.sig)

            if (msg.param) {
                free(msg.param);
            }
        }
    }
}

#ifdef CONFIG_A2DP_MEDIA_RINGBUF
static void bt_media_task_handler(void *arg)
{
    int bytes_written = 0;
    uint8_t *p, *current_p;
    size_t size;

    for (;;) {
        current_p = p = (uint8_t *)xRingbufferReceiveUpTo(media_data_buf, &size, portMAX_DELAY, 4096);
        if (p != NULL) {
            while (size > 0){
                bytes_written = media_hal_write_data(p, size);
                if (bytes_written <= 0) {
                    break;
                }
                p += bytes_written;
                size -= bytes_written;
            }
            vRingbufferReturnItem(media_data_buf, current_p);
        } else {
            media_hal_clear_data();
        }
    }
}

esp_err_t bt_media_data_post(const uint8_t *data, int len)
{
    BaseType_t ret;
    
    ret = xRingbufferSend(media_data_buf, data, len, portMAX_DELAY);

    return (ret == pdTRUE ? ESP_OK : ESP_FAIL);
}
#endif

esp_err_t bt_app_task_start_up(void)
{
#ifdef CONFIG_A2DP_MEDIA_RINGBUF
    media_data_buf = xRingbufferCreate(MEDIA_DATA_BUF_SIZE, RINGBUF_TYPE_BYTEBUF);
    if (media_data_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }

    xTaskCreatePinnedToCore(bt_media_task_handler, "media_task", 2048, NULL, configMAX_PRIORITIES - 3, &bt_media_task_handle, 0);
#endif
    bt_app_task_queue = xQueueCreate(10, sizeof(bt_app_msg_t));
    xTaskCreatePinnedToCore(bt_app_task_handler, "BtAppT", 4096, NULL, configMAX_PRIORITIES - 3, &bt_app_task_handle, 0);

    return ESP_OK;
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

#ifdef CONFIG_A2DP_MEDIA_RINGBUF
    if (bt_media_task_handle) {
        vTaskDelete(bt_media_task_handle);
        bt_media_task_handle = NULL;
    }
    if (media_data_buf) {
        vRingbufferDelete(media_data_buf);
        media_data_buf = NULL;
    }
#endif
}

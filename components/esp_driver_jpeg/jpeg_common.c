/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "esp_err.h"
#include "hal/jpeg_ll.h"
#include "esp_private/periph_ctrl.h"
#include "jpeg_private.h"
#include "hal/jpeg_hal.h"
#include "esp_memory_utils.h"
#include "driver/jpeg_types.h"
#include "sys/lock.h"
#include "sys/queue.h"
#include "sdkconfig.h"
#if CONFIG_JPEG_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_check.h"

static const char *TAG = "jpeg.common";

typedef struct jpeg_platform_t {
    _lock_t mutex;                       // platform level mutex lock.
    jpeg_codec_handle_t jpeg_codec;      // JPEG codec instances.
    uint32_t count;                      // reference count used to protect group install/uninstall.
} jpeg_platform_t;

static jpeg_platform_t s_jpeg_platform = {};  // singleton platform

esp_err_t jpeg_acquire_codec_handle(jpeg_codec_handle_t *jpeg_new_codec)
{
#if CONFIG_JPEG_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    bool new_codec = false;
    jpeg_codec_t *codec = NULL;
    _lock_acquire(&s_jpeg_platform.mutex);
    if (!s_jpeg_platform.jpeg_codec) {
        codec = heap_caps_calloc(1, sizeof(jpeg_codec_t), JPEG_MEM_ALLOC_CAPS);
        if (codec) {
            new_codec = true;
            s_jpeg_platform.jpeg_codec = codec;
            codec->intr_priority = -1;
            codec->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
            codec->codec_mutex = xSemaphoreCreateBinaryWithCaps(JPEG_MEM_ALLOC_CAPS);
            ESP_RETURN_ON_FALSE(codec->codec_mutex, ESP_ERR_NO_MEM, TAG, "No memory for codec mutex");
            SLIST_INIT(&codec->jpeg_isr_handler_list);
            xSemaphoreGive(codec->codec_mutex);
            // init the clock
            PERIPH_RCC_ATOMIC() {
                jpeg_ll_enable_bus_clock(true);
                jpeg_ll_reset_module_register();
            }
#if CONFIG_PM_ENABLE
            ESP_RETURN_ON_ERROR(esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "jpeg_codec", &codec->pm_lock), TAG, "create pm lock failed");
#endif
            jpeg_hal_init(&codec->hal);
        } else {
            ESP_LOGE(TAG, "No more memory for acquiring JPEG codec module");
            ret = ESP_ERR_NO_MEM;
        }
    }

    if (s_jpeg_platform.jpeg_codec) {
        s_jpeg_platform.count++;
    }
    if (new_codec) {
        ESP_LOGD(TAG, "new jpeg module has been acquired at (%p)", codec);
    }

    *jpeg_new_codec = s_jpeg_platform.jpeg_codec;
    _lock_release(&s_jpeg_platform.mutex);
    return ret;
}

esp_err_t jpeg_release_codec_handle(jpeg_codec_handle_t jpeg_codec)
{
    bool do_deinitialize = false;
    _lock_acquire(&s_jpeg_platform.mutex);

    if (s_jpeg_platform.jpeg_codec) {
        s_jpeg_platform.count--;
        if (s_jpeg_platform.count == 0) {
            do_deinitialize = true;
            s_jpeg_platform.jpeg_codec = NULL;

            if (jpeg_codec->codec_mutex) {
                vSemaphoreDeleteWithCaps(jpeg_codec->codec_mutex);
                jpeg_codec->codec_mutex = NULL;
            }
            if (jpeg_codec->pm_lock) {
                esp_pm_lock_delete(jpeg_codec->pm_lock);
            }
            PERIPH_RCC_ATOMIC() {
                jpeg_ll_enable_bus_clock(false);
            }
            free(jpeg_codec);
        }
    }

    if (do_deinitialize) {
        ESP_LOGD(TAG, "delete codec");
    }
    _lock_release(&s_jpeg_platform.mutex);

    return ESP_OK;
}

/*---------------------------------------------------------------
                    JPEG INTERRUPT HANDLER
---------------------------------------------------------------*/

static void jpeg_isr(void* arg)
{
    jpeg_codec_handle_t jpeg_codec = (jpeg_codec_handle_t) arg;
    uint32_t status = jpeg_ll_get_intr_status(jpeg_codec->hal.dev);
    jpeg_isr_handler_t* it;
    SLIST_FOREACH(it, &jpeg_codec->jpeg_isr_handler_list, next) {
        if (it->mask & status) {
            (*it->handler)(it->handler_arg);
        }
    }
    jpeg_ll_clear_intr_mask(jpeg_codec->hal.dev, status);
}

esp_err_t jpeg_isr_register(jpeg_codec_handle_t jpeg_codec, intr_handler_t handler, void* handler_arg, uint32_t jpeg_intr_mask, uint32_t flags, jpeg_isr_handler_t** jpeg_intr_handler)
{
    if (jpeg_codec->intr_handle == NULL) {
        // The jpeg codec interrupt has not been allocated.
        esp_err_t err = esp_intr_alloc_intrstatus(ETS_JPEG_INTR_SOURCE, flags, (uint32_t)jpeg_ll_get_interrupt_status_reg(jpeg_codec->hal.dev), JPEG_LL_DECODER_EVENT_INTR | JPEG_LL_ENCODER_EVENT_INTR, &jpeg_isr, jpeg_codec, &jpeg_codec->intr_handle);
        if (err != ESP_OK) {
            return err;
        }
    }

    jpeg_isr_handler_t* item = heap_caps_calloc(1, sizeof(jpeg_isr_handler_t), JPEG_MEM_ALLOC_CAPS);
    if (item == NULL) {
        return ESP_ERR_NO_MEM;
    }
    item->handler = handler;
    item->handler_arg = handler_arg;
    item->mask = jpeg_intr_mask;
    item->flags = flags;
    SLIST_INSERT_HEAD(&jpeg_codec->jpeg_isr_handler_list, item, next);
    *jpeg_intr_handler = item;
    return ESP_OK;
}

esp_err_t jpeg_isr_deregister(jpeg_codec_handle_t jpeg_codec, jpeg_isr_handler_t *jpeg_intr_handler)
{
    jpeg_isr_handler_t* it;
    jpeg_isr_handler_t* prev = NULL;
    bool found = false;
    SLIST_FOREACH(it, &jpeg_codec->jpeg_isr_handler_list, next) {
        if (it == jpeg_intr_handler) {
            if (it == SLIST_FIRST(&jpeg_codec->jpeg_isr_handler_list)) {
                SLIST_REMOVE_HEAD(&jpeg_codec->jpeg_isr_handler_list, next);
            } else {
                SLIST_REMOVE_AFTER(prev, next);
            }
            found = true;
            break;
        }
        prev = it;
    }
    if (found) {
        free(it);
    }
    if (unlikely(found != true)) {
        return ESP_ERR_INVALID_STATE;
    }

    if (SLIST_EMPTY(&jpeg_codec->jpeg_isr_handler_list)) {
        // All interrupt is removed
        if (jpeg_codec->intr_handle) {
            esp_intr_free(jpeg_codec->intr_handle);
        }
    }
    return ESP_OK;
}

esp_err_t jpeg_check_intr_priority(jpeg_codec_handle_t jpeg_codec, int intr_priority)
{
    esp_err_t ret = ESP_OK;
    bool intr_priority_conflict = false;
    if (jpeg_codec->intr_priority == -1) {
        jpeg_codec->intr_priority = intr_priority;
    } else if (intr_priority != 0) {
        intr_priority_conflict = (jpeg_codec->intr_priority != intr_priority);
    }
    ESP_RETURN_ON_FALSE(!intr_priority_conflict, ESP_ERR_INVALID_STATE, TAG, "intr_priority conflict, already is %d but attempt to %d", jpeg_codec->intr_priority, intr_priority);
    return ret;
}

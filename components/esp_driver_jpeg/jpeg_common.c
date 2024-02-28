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
        new_codec = true;
        codec = heap_caps_calloc(1, sizeof(jpeg_codec_t), JPEG_MEM_ALLOC_CAPS);
        if (codec) {
            s_jpeg_platform.jpeg_codec = codec;
            codec->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
            codec->codec_mux = xSemaphoreCreateBinaryWithCaps(JPEG_MEM_ALLOC_CAPS);
            ESP_RETURN_ON_FALSE(codec->codec_mux, ESP_ERR_NO_MEM, TAG, "No memory for codec mutex");
            xSemaphoreGive(codec->codec_mux);
            // init the clock
            PERIPH_RCC_ATOMIC() {
                jpeg_ll_enable_bus_clock(true);
                jpeg_ll_reset_module_register();
            }

            jpeg_hal_init(&codec->hal);
        } else {
            ESP_LOGE(TAG, "No more memory for acquiring JPEG codec module");
            ret = ESP_ERR_NO_MEM;
        }
    }

    if (codec) {
        s_jpeg_platform.count++;
    }
    if (new_codec) {
        ESP_LOGD(TAG, "new jpeg module has been acquired at (%p)", codec);
    }

    *jpeg_new_codec = codec;
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

            if (jpeg_codec->codec_mux) {
                vSemaphoreDeleteWithCaps(jpeg_codec->codec_mux);
                jpeg_codec->codec_mux = NULL;
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

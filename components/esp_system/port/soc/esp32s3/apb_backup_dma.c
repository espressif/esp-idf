/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "esp_private/esp_system_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "esp32s3/rom/apb_backup_dma.h"
#include "sdkconfig.h"

static portMUX_TYPE s_apb_backup_dma_mutex = portMUX_INITIALIZER_UNLOCKED;

static void ESP_SYSTEM_IRAM_ATTR apb_backup_dma_lock(void)
{
    if (xPortInIsrContext()) {
        portENTER_CRITICAL_ISR(&s_apb_backup_dma_mutex);
    } else {
        portENTER_CRITICAL(&s_apb_backup_dma_mutex);
    }
}

static void ESP_SYSTEM_IRAM_ATTR apb_backup_dma_unlock(void)
{
    if (xPortInIsrContext()) {
        portEXIT_CRITICAL_ISR(&s_apb_backup_dma_mutex);
    } else {
        portEXIT_CRITICAL(&s_apb_backup_dma_mutex);
    }
}

void esp_apb_backup_dma_lock_init(void)
{
    ets_apb_backup_init_lock_func(apb_backup_dma_lock, apb_backup_dma_unlock);
}

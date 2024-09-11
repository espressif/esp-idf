/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <inttypes.h>
#include <sys/lock.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_check.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "hal/rtc_hal.h"
#include "soc/rtc_periph.h"
#include "soc/soc_caps.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/sleep_event.h"
#include "sdkconfig.h"


static __attribute__((unused)) const char *TAG = "sleep";

typedef struct {
    uint32_t start;
    uint32_t end;
} cpu_domain_dev_regs_region_t;

typedef struct {
    cpu_domain_dev_regs_region_t *region;
    int region_num;
    uint32_t *regs_frame;
} cpu_domain_dev_sleep_frame_t;

/**
 * Internal structure which holds all requested light sleep cpu retention parameters
 */
typedef struct {
    rtc_cntl_sleep_retent_t retent;
} sleep_cpu_retention_t;

static DRAM_ATTR  sleep_cpu_retention_t s_cpu_retention;

esp_err_t esp_sleep_cpu_pd_low_init(void)
{
    if (s_cpu_retention.retent.cpu_pd_mem == NULL) {
        void *buf = heap_caps_aligned_calloc(SOC_RTC_CNTL_CPU_PD_DMA_ADDR_ALIGN, 1,
                                            SOC_RTC_CNTL_CPU_PD_RETENTION_MEM_SIZE + RTC_HAL_DMA_LINK_NODE_SIZE,
                                            MALLOC_CAP_RETENTION);
        if (buf) {
            s_cpu_retention.retent.cpu_pd_mem = rtc_cntl_hal_dma_link_init(buf,
                                  buf + RTC_HAL_DMA_LINK_NODE_SIZE, SOC_RTC_CNTL_CPU_PD_RETENTION_MEM_SIZE, NULL);
        } else {
            return ESP_ERR_NO_MEM;
        }
    }

    return ESP_OK;
}

esp_err_t esp_sleep_cpu_pd_low_deinit(void)
{
    if (s_cpu_retention.retent.cpu_pd_mem) {
        heap_caps_free(s_cpu_retention.retent.cpu_pd_mem);
        s_cpu_retention.retent.cpu_pd_mem = NULL;
    }

    return ESP_OK;
}

void sleep_enable_cpu_retention(void)
{
    rtc_cntl_hal_enable_cpu_retention(&s_cpu_retention.retent);
}

void IRAM_ATTR sleep_disable_cpu_retention(void)
{
    rtc_cntl_hal_disable_cpu_retention(&s_cpu_retention.retent);
}

esp_err_t esp_sleep_cpu_retention_init(void)
{
    return esp_sleep_cpu_pd_low_init();
}

esp_err_t esp_sleep_cpu_retention_deinit(void)
{
    return esp_sleep_cpu_pd_low_deinit();
}

bool cpu_domain_pd_allowed(void)
{
    return (s_cpu_retention.retent.cpu_pd_mem != NULL);
}

esp_err_t sleep_cpu_configure(bool light_sleep_enable)
{
#if ESP_SLEEP_POWER_DOWN_CPU
    if (light_sleep_enable) {
        ESP_RETURN_ON_ERROR(esp_sleep_cpu_retention_init(), TAG, "Failed to enable CPU power down during light sleep.");
    } else {
        ESP_RETURN_ON_ERROR(esp_sleep_cpu_retention_deinit(), TAG, "Failed to release CPU retention memory");
    }
#endif
    return ESP_OK;
}

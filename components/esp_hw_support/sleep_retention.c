/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"
#include "hal/rtc_hal.h"
#include "esp_private/sleep_retention.h"
#include "sdkconfig.h"

/**
 * Internal structure which holds all requested light sleep memory retention parameters
 */
typedef struct {
    rtc_cntl_sleep_retent_t retent;
} sleep_retention_t;

static DRAM_ATTR sleep_retention_t s_retention;

#if SOC_PM_SUPPORT_CPU_PD

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2
#define CPU_PD_MEM_TYPE_CAPS        (MALLOC_CAP_RETENTION | MALLOC_CAP_DEFAULT)
#else
#define CPU_PD_MEM_TYPE_CAPS        (MALLOC_CAP_DMA | MALLOC_CAP_DEFAULT)
#endif

esp_err_t esp_sleep_cpu_pd_low_init(bool enable)
{
    if (enable) {
        if (s_retention.retent.cpu_pd_mem == NULL) {
            void *buf = heap_caps_aligned_alloc(SOC_RTC_CNTL_CPU_PD_DMA_ADDR_ALIGN,
                                                SOC_RTC_CNTL_CPU_PD_RETENTION_MEM_SIZE + RTC_HAL_DMA_LINK_NODE_SIZE,
                                                CPU_PD_MEM_TYPE_CAPS);
            if (buf) {
                memset(buf, 0, SOC_RTC_CNTL_CPU_PD_RETENTION_MEM_SIZE + RTC_HAL_DMA_LINK_NODE_SIZE);
                s_retention.retent.cpu_pd_mem = rtc_cntl_hal_dma_link_init(buf,
                                      buf + RTC_HAL_DMA_LINK_NODE_SIZE, SOC_RTC_CNTL_CPU_PD_RETENTION_MEM_SIZE, NULL);
            } else {
                return ESP_ERR_NO_MEM;
            }
        }
    } else {
        if (s_retention.retent.cpu_pd_mem) {
            heap_caps_free(s_retention.retent.cpu_pd_mem);
            s_retention.retent.cpu_pd_mem = NULL;
        }
    }
    return ESP_OK;
}

bool cpu_domain_pd_allowed(void)
{
    return (s_retention.retent.cpu_pd_mem != NULL);
}

#endif // SOC_PM_SUPPORT_CPU_PD

#if SOC_PM_SUPPORT_CPU_PD || SOC_PM_SUPPORT_TAGMEM_PD

void sleep_enable_memory_retention(void)
{
#if SOC_PM_SUPPORT_CPU_PD
    rtc_cntl_hal_enable_cpu_retention(&s_retention.retent);
#endif
}

void IRAM_ATTR sleep_disable_memory_retention(void)
{
#if SOC_PM_SUPPORT_CPU_PD
    rtc_cntl_hal_disable_cpu_retention(&s_retention.retent);
#endif
}

#endif // SOC_PM_SUPPORT_CPU_PD || SOC_PM_SUPPORT_TAGMEM_PD

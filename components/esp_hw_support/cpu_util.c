/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include "soc/cpu.h"
#include "soc/soc.h"
#include "soc/rtc_periph.h"
#include "sdkconfig.h"

#include "hal/cpu_hal.h"
#include "hal/cpu_types.h"
#include "hal/mpu_hal.h"

#include "esp_cpu.h"

#include "hal/soc_hal.h"
#include "soc/soc_caps.h"

#include "sdkconfig.h"

void IRAM_ATTR esp_cpu_stall(int cpu_id)
{
#if SOC_CPU_CORES_NUM > 1
    soc_hal_stall_core(cpu_id);
#endif
}

void IRAM_ATTR esp_cpu_unstall(int cpu_id)
{
#if SOC_CPU_CORES_NUM > 1
    soc_hal_unstall_core(cpu_id);
#endif
}

void IRAM_ATTR esp_cpu_reset(int cpu_id)
{
    soc_hal_reset_core(cpu_id);
}

esp_err_t IRAM_ATTR esp_cpu_set_watchpoint(int no, void *adr, int size, int flags)
{
    watchpoint_trigger_t trigger;

    switch (flags)
    {
    case ESP_WATCHPOINT_LOAD:
        trigger = WATCHPOINT_TRIGGER_ON_RO;
        break;
    case ESP_WATCHPOINT_STORE:
        trigger = WATCHPOINT_TRIGGER_ON_WO;
        break;
    case ESP_WATCHPOINT_ACCESS:
        trigger = WATCHPOINT_TRIGGER_ON_RW;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    cpu_hal_set_watchpoint(no, adr, size, trigger);
    return ESP_OK;
}

void IRAM_ATTR esp_cpu_clear_watchpoint(int no)
{
    cpu_hal_clear_watchpoint(no);
}

bool IRAM_ATTR esp_cpu_in_ocd_debug_mode(void)
{
#if CONFIG_ESP32_DEBUG_OCDAWARE || \
    CONFIG_ESP32S2_DEBUG_OCDAWARE || \
    CONFIG_ESP32S3_DEBUG_OCDAWARE || \
    CONFIG_ESP32C3_DEBUG_OCDAWARE || \
    CONFIG_ESP32H2_DEBUG_OCDAWARE
    return cpu_ll_is_debugger_attached();
#else
    return false; // Always return false if "OCD aware" is disabled
#endif
}

#if __XTENSA__

void esp_cpu_configure_region_protection(void)
{
    /* Note: currently this is configured the same on all Xtensa targets
     *
     * Both chips have the address space divided into 8 regions, 512MB each.
     */
    const int illegal_regions[] = {0, 4, 5, 6, 7}; // 0x00000000, 0x80000000, 0xa0000000, 0xc0000000, 0xe0000000
    for (size_t i = 0; i < sizeof(illegal_regions) / sizeof(illegal_regions[0]); ++i) {
        mpu_hal_set_region_access(illegal_regions[i], MPU_REGION_ILLEGAL);
    }

    mpu_hal_set_region_access(1, MPU_REGION_RW); // 0x20000000
}

#endif

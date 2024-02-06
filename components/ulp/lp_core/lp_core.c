/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_rom_caps.h"
#include "esp_log.h"
#include "esp_assert.h"
#include "soc/pmu_reg.h"
#include "hal/misc.h"
#include "esp_private/periph_ctrl.h"
#include "ulp_common.h"
#include "ulp_lp_core.h"
#include "ulp_lp_core_memory_shared.h"
#include "ulp_lp_core_lp_timer_shared.h"
#include "hal/lp_core_ll.h"

#if CONFIG_IDF_TARGET_ESP32P4
#define LP_CORE_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define LP_CORE_RCC_ATOMIC()
#endif

const static char* TAG = "ulp-lp-core";

#define WAKEUP_SOURCE_MAX_NUMBER 5

/* Maps the flags defined in ulp_lp_core.h e.g. ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU to their actual HW values */
static uint32_t wakeup_src_sw_to_hw_flag_lookup[WAKEUP_SOURCE_MAX_NUMBER] = {
    LP_CORE_LL_WAKEUP_SOURCE_HP_CPU,
    LP_CORE_LL_WAKEUP_SOURCE_LP_UART,
    LP_CORE_LL_WAKEUP_SOURCE_LP_IO,
    LP_CORE_LL_WAKEUP_SOURCE_ETM,
    LP_CORE_LL_WAKEUP_SOURCE_LP_TIMER,
};

/* Convert the wake-up sources defined in ulp_lp_core.h to the actual HW wake-up source values */
static uint32_t lp_core_get_wakeup_source_hw_flags(uint32_t flags)
{
    uint32_t hw_flags = 0;
    for (int i = 0; i < WAKEUP_SOURCE_MAX_NUMBER; i++) {
        if (flags & (1 << i)) {
            hw_flags |= wakeup_src_sw_to_hw_flag_lookup[i];
        }
    }
    return hw_flags;
}

esp_err_t ulp_lp_core_run(ulp_lp_core_cfg_t* cfg)
{
    if (!cfg->wakeup_source) {
        ESP_LOGE(TAG, "No valid wakeup source specified");
        return ESP_ERR_INVALID_ARG;
    }

    ulp_lp_core_memory_shared_cfg_t* shared_mem = ulp_lp_core_memory_shared_cfg_get();

#if ESP_ROM_HAS_LP_ROM
    /* If we have a LP ROM we boot from it, before jumping to the app code */
    intptr_t boot_addr;
    if (cfg->skip_lp_rom_boot) {
        boot_addr = RTC_SLOW_MEM;
    } else {
        boot_addr = SOC_LP_ROM_LOW;
    }

    lp_core_ll_set_boot_address(boot_addr);
    lp_core_ll_set_app_boot_address(RTC_SLOW_MEM);
#endif //ESP_ROM_HAS_LP_ROM

    LP_CORE_RCC_ATOMIC() {
        lp_core_ll_reset_register();
        lp_core_ll_enable_bus_clock(true);
    }

#if CONFIG_IDF_TARGET_ESP32C6
    /* Disable fast LP mem access to allow LP core to access LP memory during sleep */
    lp_core_ll_fast_lp_mem_enable(false);
#endif //CONFIG_IDF_TARGET_ESP32C6

    /* Enable stall at sleep request*/
    lp_core_ll_stall_at_sleep_request(true);

    /* Enable reset CPU when going to sleep */
    lp_core_ll_rst_at_sleep_enable(true);

    /* Set wake-up sources */
    lp_core_ll_set_wakeup_source(lp_core_get_wakeup_source_hw_flags(cfg->wakeup_source));

    /* Enable JTAG debugging */
    lp_core_ll_debug_module_enable(true);

    if (cfg->wakeup_source & ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU) {
        lp_core_ll_hp_wake_lp();
    }

    if (cfg->wakeup_source & ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER) {
        if (!cfg->lp_timer_sleep_duration_us) {
            ESP_LOGI(TAG, "LP timer specified as wakeup source, but no sleep duration set. ULP will only wake-up once unless it calls ulp_lp_core_lp_timer_set_wakeup_time()");
        }
        shared_mem->sleep_duration_us = cfg->lp_timer_sleep_duration_us;

        /* Set first wakeup alarm */
        ulp_lp_core_lp_timer_set_wakeup_time(cfg->lp_timer_sleep_duration_us);
    }

    if (cfg->wakeup_source & (ULP_LP_CORE_WAKEUP_SOURCE_LP_UART | ULP_LP_CORE_WAKEUP_SOURCE_LP_IO | ULP_LP_CORE_WAKEUP_SOURCE_ETM)) {
        ESP_LOGE(TAG, "Wake-up source not yet supported");
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

esp_err_t ulp_lp_core_load_binary(const uint8_t* program_binary, size_t program_size_bytes)
{
    if (program_binary == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (program_size_bytes > CONFIG_ULP_COPROC_RESERVE_MEM) {
        return ESP_ERR_INVALID_SIZE;
    }

    /* Turn off LP CPU before loading binary */
    ulp_lp_core_stop();

    uint8_t* base = (uint8_t*) RTC_SLOW_MEM;

    //Start by clearing memory reserved with zeros, this will also will initialize the bss:
    hal_memset(base, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
    hal_memcpy(base, program_binary, program_size_bytes);

    return ESP_OK;
}

void ulp_lp_core_stop(void)
{
    /* Disable wake-up source and put lp core to sleep */
    lp_core_ll_set_wakeup_source(0);
    lp_core_ll_request_sleep();
}

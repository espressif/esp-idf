/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_log.h"
#include "soc/pmu_reg.h"
#include "soc/lp_aon_reg.h"
#include "soc/lpperi_reg.h"
#include "hal/misc.h"
#include "ulp_common.h"
#include "ulp_lp_core.h"
#include "ulp_lp_core_memory_shared.h"
#include "ulp_lp_core_lp_timer_shared.h"

const static char* TAG = "ulp-lp-core";

esp_err_t ulp_lp_core_run(ulp_lp_core_cfg_t* cfg)
{
    if (!cfg->wakeup_source) {
        ESP_LOGE(TAG, "No valid wakeup source specified");
        return ESP_ERR_INVALID_ARG;
    }

    ulp_lp_core_memory_shared_cfg_t* shared_mem = ulp_lp_core_memory_shared_cfg_get();

    /* Enable LP-Core */
    REG_CLR_BIT(LP_AON_LPCORE_REG, LP_AON_LPCORE_DISABLE);

    /* Allow LP core to access LP memory during sleep */
    REG_CLR_BIT(LP_AON_LPBUS_REG, LP_AON_FAST_MEM_MUX_SEL);
    REG_SET_BIT(LP_AON_LPBUS_REG, LP_AON_FAST_MEM_MUX_SEL_UPDATE);

    /* Enable stall at sleep request*/
    REG_SET_FIELD(PMU_LP_CPU_PWR0_REG, PMU_LP_CPU_SLP_STALL_EN, 1);

    /* Enable reset after wake-up */
    REG_SET_BIT(PMU_LP_CPU_PWR0_REG, PMU_LP_CPU_SLP_RESET_EN);

    /* Set wake-up sources */
    REG_SET_FIELD(PMU_LP_CPU_PWR1_REG, PMU_LP_CPU_WAKEUP_EN, cfg->wakeup_source);

    /* Enable JTAG debugging */
    REG_CLR_BIT(LPPERI_CPU_REG, LPPERI_LPCORE_DBGM_UNAVALIABLE);

    if (cfg->wakeup_source & ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU) {
        REG_SET_FIELD(PMU_HP_LP_CPU_COMM_REG, PMU_HP_TRIGGER_LP, 1);
    }

    if (cfg->wakeup_source & ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER) {
        if (!cfg->lp_timer_sleep_duration_us) {
            ESP_LOGI(TAG, "LP timer specified as wakeup source, but no sleep duration set. ULP will only wake-up once unless it calls ulp_lp_core_lp_timer_set_wakeup_time()");
        }
        shared_mem->sleep_duration_us = cfg->lp_timer_sleep_duration_us;

        /* Set first wakeup alarm */
        ulp_lp_core_lp_timer_set_wakeup_time(cfg->lp_timer_sleep_duration_us);
    }

    if (cfg->wakeup_source & (ULP_LP_CORE_WAKEUP_SOURCE_LP_UART | ULP_LP_CORE_WAKEUP_SOURCE_LP_IO | ULP_LP_CORE_WAKEUP_SOURCE_ETM )) {
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
    REG_SET_FIELD(PMU_LP_CPU_PWR1_REG, PMU_LP_CPU_WAKEUP_EN, 0);
    REG_SET_FIELD(PMU_LP_CPU_PWR1_REG, PMU_LP_CPU_SLEEP_REQ, 1);
}

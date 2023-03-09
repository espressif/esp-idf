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

const static char* TAG = "ulp-lp-core";

esp_err_t ulp_lp_core_run(ulp_lp_core_cfg_t* cfg)
{
    /* Enable LP-Core */
    REG_CLR_BIT(LP_AON_LPCORE_REG, LP_AON_LPCORE_DISABLE);

    /* Allow LP core to access LP memory during sleep */
    REG_CLR_BIT(LP_AON_LPBUS_REG, LP_AON_FAST_MEM_MUX_SEL);
    REG_SET_BIT(LP_AON_LPBUS_REG, LP_AON_FAST_MEM_MUX_SEL_UPDATE);

    REG_SET_FIELD(PMU_LP_CPU_PWR1_REG, PMU_LP_CPU_WAKEUP_EN, 1);

    /* Enable JTAG debugging */
    REG_CLR_BIT(LPPERI_CPU_REG, LPPERI_LPCORE_DBGM_UNAVALIABLE);

    switch(cfg->wakeup_source) {
        case ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU:
            REG_SET_FIELD(PMU_HP_LP_CPU_COMM_REG, PMU_HP_TRIGGER_LP, 1);
            break;
        default:
            ESP_LOGE(TAG, "No valid wakeup source specified");
            break;
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
    REG_SET_FIELD(PMU_LP_CPU_PWR1_REG, PMU_LP_CPU_WAKEUP_EN, 0);
    REG_SET_FIELD(PMU_LP_CPU_PWR1_REG, PMU_LP_CPU_SLEEP_REQ, 1);

    uint8_t* base = (uint8_t*) RTC_SLOW_MEM;

    //Start by clearing memory reserved with zeros, this will also will initialize the bss:
    hal_memset(base, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
    hal_memcpy(base, program_binary, program_size_bytes);

    return ESP_OK;
}

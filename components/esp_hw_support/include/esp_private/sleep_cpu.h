/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_sleep.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sleep_cpu.h
 *
 * This file contains declarations of cpu retention related functions in light sleep mode.
 */

#if ESP_SLEEP_POWER_DOWN_CPU || SOC_PM_SUPPORT_CPU_PD
/**
 * @brief Whether to allow the cpu power domain to be powered off.
 *
 * In light sleep mode, only when the system can provide enough memory
 * for cpu retention, the cpu power domain can be powered off.
 */
bool cpu_domain_pd_allowed(void);
#endif

#if ESP_SLEEP_POWER_DOWN_CPU
/**
 * @brief Configure the parameters of the CPU domain during the sleep process
 *
 * @param light_sleep_enable true for enable light sleep mode, false for disable light sleep mode
 *
 * @return
 *  - ESP_OK on success
 */
esp_err_t sleep_cpu_configure(bool light_sleep_enable);

#if SOC_PM_CPU_RETENTION_BY_RTCCNTL
/**
 * @brief Enable cpu retention of some modules.
 *
 * In light sleep mode, before the system goes to sleep, enable the cpu
 * retention of modules such as CPU and I/D-cache tag memory.
 */
void sleep_enable_cpu_retention(void);

/**
 * @brief Disable cpu retention of some modules.
 *
 * In light sleep mode, after the system exits sleep, disable the cpu
 * retention of modules such as CPU and I/D-cache tag memory.
 */
void sleep_disable_cpu_retention(void);
#endif // SOC_PM_CPU_RETENTION_BY_RTCCNTL

#if SOC_PM_CPU_RETENTION_BY_SW
esp_err_t esp_sleep_cpu_retention(uint32_t (*goto_sleep)(uint32_t, uint32_t, uint32_t, bool),
        uint32_t wakeup_opt, uint32_t reject_opt, uint32_t lslp_mem_inf_fpu, bool dslp);
#endif // SOC_PM_CPU_RETENTION_BY_SW
#endif // ESP_SLEEP_POWER_DOWN_CPU

#if !CONFIG_FREERTOS_UNICORE && ESP_SLEEP_POWER_DOWN_CPU
/**
 * Do sleep prepare for other smp cores
 */
void sleep_smp_cpu_sleep_prepare(void);

/**
 * Do wakeup prepare for other smp cores
 */
void sleep_smp_cpu_wakeup_prepare(void);

/**
 * Notify the other core that this sleep does not require retention.
 */
void esp_sleep_cpu_skip_retention(void);
#endif // !CONFIG_FREERTOS_UNICORE && ESP_SLEEP_POWER_DOWN_CPU

#ifdef __cplusplus
}
#endif

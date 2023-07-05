/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc_caps.h"

#if SOC_XT_WDT_SUPPORTED
#include "hal/xt_wdt_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


#if SOC_XT_WDT_SUPPORTED

typedef struct {
    rtc_cntl_dev_t *dev;    /* Pointer to the RTC register struct */
} xt_wdt_hal_context_t;     /* HAL context struct */


typedef struct {
    uint32_t timeout;       /* Watchdog timer timeout in RTC_CLK cycles */
} xt_wdt_hal_config_t;      /* HAL config parameter struct */

/* ---------------------------- Init and Config ----------------------------- */

/**
 * @brief Initialize the WDTs associated HAL context
 *
 * Prepares the register for enabling the WDT and sets the timeout value
 *
 * @param hal Pointer to the HAL layer context
 * @param config Pointer to config struct
 */
void xt_wdt_hal_init(xt_wdt_hal_context_t *hal, const xt_wdt_hal_config_t *config);


/**
 * @brief Enable or disable the WDT
 *
 * @param hal Pointer to the HAL layer context
 * @param enable true for enable WDT, false for disable
 */
void xt_wdt_hal_enable(xt_wdt_hal_context_t *hal, bool enable);

/**
 * @brief Enable the automatic RTC backup clock with the given frequency
 *
 * Calculates and sets the necessary hardware parameters to meet the desired
 * backup clock frequency
 *
 * @param hal Pointer to the HAL layer context
 * @param rtc_clk_frequency_khz desired frequency for the backup clock
 * @return uint32_t the calculated clock factor value
 */
uint32_t xt_wdt_hal_enable_backup_clk(xt_wdt_hal_context_t *hal, uint32_t rtc_clk_frequency_khz);

#endif

#ifdef __cplusplus
}
#endif

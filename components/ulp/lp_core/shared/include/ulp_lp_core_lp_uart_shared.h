/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "soc/soc_caps.h"
#include "driver/uart_wakeup.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_DEEP_SLEEP_SUPPORTED
/**
 * @brief Initialize and configure the LP UART wakeup mode.
 *
 * @note This function configures the LP UART wakeup mode. Ensure that the UART has already been initialized
 *       with the lp_core_uart_init() call.
 *       Once the LP Core wakes up due to the LP UART, the wakeup will be kept triggered in the chips with
 *       SOC_LP_CORE_LP_UART_WAKEUP_KEEP_TRIGGERED. You need to call ulp_lp_core_lp_uart_reset_wakeup_en()
 *       to reset the wakeup signal and the UART buffer manually after waking up, which is done in ulp
 *       startup phase.
 *       Also be aware of limitations in different modes mentioned in the uart_wakeup_cfg_t struct.
 *
 *
 * @param cfg    Configuration for LP UART wakeup mode.
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t lp_core_uart_wakeup_setup(const uart_wakeup_cfg_t *cfg);
#endif /* SOC_DEEP_SLEEP_SUPPORTED */

/**
 * @brief Clear LP UART rx tx buffer.
 *
 * @note Clear LP UART rx tx buffer.
 */
void lp_core_uart_clear_buf(void);

#ifdef __cplusplus
}
#endif

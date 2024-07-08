/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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

/**
 * @brief Initialize and configure the LP UART wakeup mode.
 *
 * @note This function configures the LP UART wakeup mode. Ensure that the UART has already been initialized
 *       with the lp_core_uart_init() call.
 *       Once the LP Core wakes up due to the LP UART, the wakeup feature is disabled.
 *       To re-enable the wakeup from the LP UART, you must call
 *       ulp_lp_core_lp_uart_reset_wakeup_en() again before the LP core goes to sleep.
 *       Also be aware of limitations in different modes mentioned in the uart_wakeup_cfg_t struct.
 *
 *
 * @param cfg    Configuration for LP UART wakeup mode.
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t lp_core_uart_wakeup_setup(const uart_wakeup_cfg_t *cfg);

/**
 * @brief Clear LP UART rx tx buffer.
 *
 * @note Clear LP UART rx tx buffer.
 */
void lp_core_uart_clear_buf(void);

#ifdef __cplusplus
}
#endif

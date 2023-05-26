/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "esp_err.h"
#include "ulp_common.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU    BIT(0) // Started by HP core (1 single wakeup)
#define ULP_LP_CORE_WAKEUP_SOURCE_LP_UART   BIT(1) // Enable wake-up by a certain number of LP UART RX pulses
#define ULP_LP_CORE_WAKEUP_SOURCE_LP_IO     BIT(2) // Enable wake-up by LP IO interrupt
#define ULP_LP_CORE_WAKEUP_SOURCE_ETM       BIT(3) // Enable wake-up by ETM event
#define ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER  BIT(4) // Enable wake-up by LP timer

/**
 * @brief ULP LP core init parameters
 *
 */
typedef struct {
    uint32_t wakeup_source;                 // Wakeup source flags
    uint32_t lp_timer_sleep_duration_us;    // Sleep duration when ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER is specified.
} ulp_lp_core_cfg_t;

/**
 * @brief Configure the ULP
 *        and run the program loaded into RTC memory
 *
 * @return  ESP_OK on success
 */
esp_err_t ulp_lp_core_run(ulp_lp_core_cfg_t* cfg);

/**
 * @brief Load the program binary into RTC memory
 *
 * @param program_binary pointer to program binary
 * @param program_size_bytes size of the program binary
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_SIZE if program_size_bytes is more than KiB
 */
esp_err_t ulp_lp_core_load_binary(const uint8_t* program_binary, size_t program_size_bytes);

/**
 * @brief Puts the ulp to sleep and disables all wakeup sources.
 *        To restart the ULP call ulp_lp_core_run() with the desired
 *        wake up trigger.
 */
void ulp_lp_core_stop(void);

#ifdef __cplusplus
}
#endif

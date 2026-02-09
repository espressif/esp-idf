/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include "esp_err.h"
#include "ulp_common.h"
#include "esp_rom_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU    BIT(0) // Started by HP core (1 single wakeup)
#define ULP_LP_CORE_WAKEUP_SOURCE_LP_UART   BIT(1) // Enable wake-up by a certain number of LP UART RX pulses
#define ULP_LP_CORE_WAKEUP_SOURCE_LP_IO     BIT(2) // Enable wake-up by LP IO interrupt
#define ULP_LP_CORE_WAKEUP_SOURCE_ETM       BIT(3) // Enable wake-up by ETM event
#define ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER  BIT(4) // Enable wake-up by LP timer
#define ULP_LP_CORE_WAKEUP_SOURCE_LP_VAD    BIT(5) // Enable wake-up by LP VAD

/**
 * @brief ULP LP core init parameters
 *
 */
typedef struct {
    uint32_t wakeup_source;                  /*!< Wakeup source flags */
    uint32_t lp_timer_sleep_duration_us;     /*!< Sleep duration when ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER is specified. Measurement unit: us */
#if ESP_ROM_HAS_LP_ROM
    bool    skip_lp_rom_boot;               /*!< Skips the LP rom code and boots directly into the app code placed in LP RAM,
                                                 this gives faster boot time for time sensitive use-cases at the cost of skipping
                                                 setup e.g. of UART */
#endif //ESP_ROM_HAS_LP_ROM
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

/**
 * @brief Enable or disable the software interrupts requested by the LP CPU via the PMU
 */
void ulp_lp_core_sw_intr_from_lp_enable(bool enable);

/**
 * @brief Clear the software interrupt requested by the LP CPU via the PMU
 */
void ulp_lp_core_sw_intr_from_lp_clear(void);

/**
 * @brief Trigger a software interrupt to the LP CPU from the PMU
 *
 * @note This is the same SW trigger that is used to wake up the LP CPU
 *
 */
void ulp_lp_core_sw_intr_to_lp_trigger(void);

/**
 * @brief Trigger a software interrupt to the LP CPU from the PMU
 *
 * @note This function is an alias for `ulp_lp_core_sw_intr_to_lp_trigger`
 *
 */
static inline void ulp_lp_core_sw_intr_trigger(void)
{
    ulp_lp_core_sw_intr_to_lp_trigger();
}

/**
 * @brief Trigger a software interrupt on the HP core from the PMU
 *
 * @note This function generates the same software interrupt that the LP core would normally
 *       use to notify the HP core. It can be called directly by the HP core itself in order
 *       to simulate or re-issue such a notification. This is particularly useful if the HP
 *       core missed a previous interrupt from the LP core and needs to retrigger the
 *       notification sequence.
 */
void ulp_lp_core_sw_intr_trigger_self(void);

#ifdef __cplusplus
}
#endif

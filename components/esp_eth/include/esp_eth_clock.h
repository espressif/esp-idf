/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <sys/time.h>
#include "esp_err.h"
#include "esp_eth_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SOC_EMAC_IEEE1588V2_SUPPORTED

/**
 * @brief PTP clock identifier
 *
 */
#define CLOCK_PTP_SYSTEM         ((clockid_t) 19)

/**
 * @brief Configuration of clock during initialization
 *
 */
typedef struct {
    clockid_t clock_id;
} esp_eth_clock_cfg_t;

/**
 * @brief Set the target time for the PTP clock.
 *
 * @param clk_id Identifier of the clock to set the target time for
 * @param tp Pointer to the target time
 *
 * @return
 *     - 0: Success
 *     - -1: Failure
 */
int esp_eth_clock_set_target_time(clockid_t clock_id, struct timespec *tp);

/**
 * @brief Register callback function invoked on Time Stamp target time exceeded interrupt
 *
 * @param clock_id Identifier of the clock
 * @param ts_callback callback function to be registered
 * @return
 *     - 0: Success
 *     - -1: Failure
 */
int esp_eth_clock_register_target_cb(clockid_t clock_id,
                                     ts_target_exceed_cb_from_isr_t ts_callback);

/**
 * @brief Initialize the Ethernet PTP clock subsystem
 *
 * Enables PTP timestamping on the Ethernet MAC and registers the PTP clock
 * with the POSIX time subsystem. After initialization, the clock is accessible
 * via standard clock_gettime(CLOCK_PTP_SYSTEM, ...),
 * clock_settime(CLOCK_PTP_SYSTEM, ...), and
 * clock_adjtime(CLOCK_PTP_SYSTEM, ...) with ADJ_OFFSET / ADJ_FREQUENCY.
 *
 * @param eth_hndl Ethernet handle to initialize the clock for
 * @param cfg Pointer to the configuration structure
 *
 * @return
 *     - ESP_OK: Success
 *     - ESP_FAIL: Failure
 *     - ESP_ERR_INVALID_ARG: Invalid argument
 *     - ESP_ERR_INVALID_STATE: Clock is already initialized
 */
esp_err_t esp_eth_clock_init(esp_eth_handle_t eth_hndl, const esp_eth_clock_cfg_t *cfg);

#endif // SOC_EMAC_IEEE1588V2_SUPPORTED

#ifdef __cplusplus
}
#endif

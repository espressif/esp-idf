/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MSPI ISR
 */
typedef struct mspi_isr_s {
    /**
     * @brief MSPI PSRAM ISR
     *
     * @param[in] arg          Argument to the ISR
     * @param[in] intr_events  Interrupt events
     */
    void (*psram_isr)(void *arg, uint32_t intr_events);

    /**
     * @brief MSPI Flash ISR
     *
     * @param[in] arg          Argument to the ISR
     * @param[in] intr_events  Interrupt events
     */
    void (*flash_isr)(void *arg, uint32_t intr_events);
} mspi_isr_t;

/**
 * @brief Register MSPI interrupt
 * @note ISR dispatcher will decide if abort, dispatched ISRs should not abort
 *
 * This ISR mainly:
 * - Report MSPI bus errors, e.g. fifo overflow, underflow.
 * - ECC error, which will be useful for a Nand flash replacement mechanism.
 * - etc.
 *
 * @return ESP_OK on success, otherwise an error code
 */
esp_err_t esp_mspi_register_isr(mspi_isr_t *isr);

/**
 * @brief Unregister MSPI interrupt
 *
 * @return ESP_OK on success, otherwise an error code
 */
esp_err_t esp_mspi_unregister_isr(void);

#ifdef __cplusplus
}
#endif

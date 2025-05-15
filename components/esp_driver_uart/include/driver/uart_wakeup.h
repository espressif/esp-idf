/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "soc/soc_caps.h"
#include "hal/uart_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Structure that holds configuration for UART wakeup.
 *
 * This structure is used to configure the wakeup behavior for a UART port. The wakeup mode can be
 * selected from several options, such as active threshold, FIFO threshold, start bit detection, and
 * character sequence detection. The availability of different wakeup modes depends on the SOC capabilities.
 */
typedef struct {
    /** Wakeup mode selection */
    uart_wakeup_mode_t wakeup_mode;

#if SOC_UART_WAKEUP_SUPPORT_ACTIVE_THRESH_MODE
    /** Used in Active threshold wake-up;
    related: UART_WK_MODE_ACTIVE_THRESH;
    Configures the number of RXD edge changes to wake up the chip.*/
    uint16_t rx_edge_threshold;
#endif

#if SOC_UART_WAKEUP_SUPPORT_FIFO_THRESH_MODE
    /** Used in Bytes received in RX FIFO wake-up;
    related: UART_WK_MODE_FIFO_THRESH;
    Configures the number of received data bytes to wake up the chip.*/
    uint16_t rx_fifo_threshold;
#endif

#if SOC_UART_WAKEUP_SUPPORT_CHAR_SEQ_MODE
    /** Used in Character sequence detection(Trigger phrase) wake-up;
    related: UART_WK_MODE_CHAR_SEQ;
    '*' represents any symbol.
    The end character cannot be '*'.
    Example: "he**o" matches hello, heyyo. */
    char wake_chars_seq[SOC_UART_WAKEUP_CHARS_SEQ_MAX_LEN];
#endif

} uart_wakeup_cfg_t;

/**
 * @brief Initializes the UART wakeup functionality.
 *
 * This function configures the wakeup behavior for a specified UART port based on the provided configuration.
 * The behavior depends on the selected wakeup mode and additional parameters such as active threshold or
 * character sequence, if applicable. It is important that the provided configuration matches the capabilities
 * of the SOC to ensure proper operation.
 *
 * @param uart_num The UART port to initialize for wakeup (e.g., UART_NUM_0, UART_NUM_1, etc.).
 * @param cfg Pointer to the `uart_wakeup_cfg_t` structure that contains the wakeup configuration settings.
 *
 * @return
 * - `ESP_OK` if the wakeup configuration was successfully applied.
 * - `ESP_ERR_INVALID_ARG` if the provided configuration is invalid (e.g., threshold values out of range).
 */
esp_err_t uart_wakeup_setup(uart_port_t uart_num, const uart_wakeup_cfg_t *cfg);

/**
 * @brief Clear the UART wakeup configuration.
 *
 * This function will clear the UART wakeup behavior and set to its default configuration.
 *
 * @param uart_num The UART port to initialize for wakeup (e.g., UART_NUM_0, UART_NUM_1, etc.).
 * @param wakeup_mode The UART wakeup mode set in `uart_wakeup_setup`.
 *
 * @return
 * - `ESP_OK` Clear wakeup configuration successfully.
 */
esp_err_t uart_wakeup_clear(uart_port_t uart_num, uart_wakeup_mode_t wakeup_mode);

#ifdef __cplusplus
}
#endif

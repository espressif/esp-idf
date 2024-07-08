/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * WARNING:
 *
 * This file is shared between the HP and LP cores.
 * Updates to this file should be made carefully and should not include FreeRTOS APIs or other IDF-specific functionalities, such as the interrupt allocator.
 */

#include "driver/uart_wakeup.h"
#include "hal/uart_hal.h"

#if SOC_UART_WAKEUP_SUPPORT_CHAR_SEQ_MODE
static esp_err_t uart_char_seq_wk_configure(uart_dev_t *hw, const char* phrase)
{
    if (phrase == NULL || phrase[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t ret = ESP_OK;

    uint32_t mask = 0;
    uint32_t index = 0;

    while (index < SOC_UART_WAKEUP_CHARS_SEQ_MAX_LEN && phrase[index] != '\0') {
        if (phrase[index] == '*') {
            mask |= 1 << index;
        } else {
            uart_ll_set_char_seq_wk_char(hw, index, phrase[index]);
        }
        index++;
    }

    if (
        index == 0 ||
        phrase[index - 1] == '*' ||
        mask > 0xF
    ) {
        return ESP_ERR_INVALID_ARG;
    }
    uart_ll_set_wakeup_char_seq_mask(hw, mask);
    uart_ll_set_wakeup_char_seq_char_num(hw, index - 1);

    return ret;
}
#endif

esp_err_t uart_wakeup_setup(uart_port_t uart_num, const uart_wakeup_cfg_t *cfg)
{

    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    uart_dev_t *hw = UART_LL_GET_HW(uart_num);

    // This should be mocked at ll level if the selection of the UART wakeup mode is not supported by this SOC.
    uart_ll_set_wakeup_mode(hw, cfg->wakeup_mode);

    switch (cfg->wakeup_mode) {
#if SOC_UART_WAKEUP_SUPPORT_ACTIVE_THRESH_MODE
    case UART_WK_MODE_ACTIVE_THRESH:
        // UART_ACTIVE_THRESHOLD register has only 10 bits, and the min value is 3.
        if (cfg->rx_edge_threshold < UART_LL_WAKEUP_EDGE_THRED_MIN || cfg->rx_edge_threshold > UART_LL_WAKEUP_EDGE_THRED_MAX(hw)) {
            return ESP_ERR_INVALID_ARG;
        }
        uart_ll_set_wakeup_edge_thrd(hw, cfg->rx_edge_threshold);
        return ESP_OK;
#endif
#if SOC_UART_WAKEUP_SUPPORT_FIFO_THRESH_MODE
    case UART_WK_MODE_FIFO_THRESH:
        if (cfg->rx_fifo_threshold > UART_LL_WAKEUP_FIFO_THRED_MAX(hw)) {
            return ESP_ERR_INVALID_ARG;
        }
        uart_ll_set_wakeup_fifo_thrd(hw, cfg->rx_fifo_threshold);
        return ESP_OK;
#endif
#if SOC_UART_WAKEUP_SUPPORT_START_BIT_MODE
    case UART_WK_MODE_START_BIT:
        return ESP_OK;
#endif
#if SOC_UART_WAKEUP_SUPPORT_CHAR_SEQ_MODE
    case UART_WK_MODE_CHAR_SEQ:
        return uart_char_seq_wk_configure(hw, cfg->wake_chars_seq);
#endif
    }

    return ESP_ERR_INVALID_ARG;
}

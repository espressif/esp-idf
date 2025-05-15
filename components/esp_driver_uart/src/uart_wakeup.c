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

#include "esp_check.h"
#include "driver/uart_wakeup.h"
#include "hal/uart_hal.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_log.h"

const __attribute__((unused)) static char *TAG = "uart_wakeup";

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
    ESP_RETURN_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, TAG, "cfg is NULL");
    uart_dev_t *hw = UART_LL_GET_HW(uart_num);
    uart_hal_context_t hal = {
        .dev = hw,
    };
    soc_module_clk_t src_clk;
    uart_hal_get_sclk(&hal, &src_clk);
    if (uart_num < SOC_UART_HP_NUM && cfg->wakeup_mode != UART_WK_MODE_ACTIVE_THRESH) {
        // For wakeup modes except ACTIVE_THRESH, the function clock needs to be exist to trigger wakeup
        ESP_RETURN_ON_FALSE(src_clk == SOC_MOD_CLK_XTAL, ESP_ERR_NOT_SUPPORTED, TAG, "failed to setup uart wakeup due to the clock source is not XTAL!");
    }

    esp_err_t ret = ESP_OK;

    // This should be mocked at ll level if the selection of the UART wakeup mode is not supported by this SOC.
    uart_ll_set_wakeup_mode(hw, cfg->wakeup_mode);

#if SOC_PM_SUPPORT_PMU_CLK_ICG
    // When hp uarts are utilized, the main XTAL need to be PU and UARTx & IOMX ICG need to be ungate
    if (uart_num < SOC_UART_HP_NUM && cfg->wakeup_mode != UART_WK_MODE_ACTIVE_THRESH) {
        esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_ON);
        esp_sleep_clock_config(UART_LL_SLEEP_CLOCK(uart_num), ESP_SLEEP_CLOCK_OPTION_UNGATE);
        esp_sleep_clock_config(ESP_SLEEP_CLOCK_IOMUX, ESP_SLEEP_CLOCK_OPTION_UNGATE);
    }
#endif

    switch (cfg->wakeup_mode) {
#if SOC_UART_WAKEUP_SUPPORT_ACTIVE_THRESH_MODE
    case UART_WK_MODE_ACTIVE_THRESH:
        if (cfg->rx_edge_threshold < UART_LL_WAKEUP_EDGE_THRED_MIN || cfg->rx_edge_threshold > UART_LL_WAKEUP_EDGE_THRED_MAX(hw)) {
            ret = ESP_ERR_INVALID_ARG;
        } else {
            uart_ll_set_wakeup_edge_thrd(hw, cfg->rx_edge_threshold);
        }
        break;
#endif
#if SOC_UART_WAKEUP_SUPPORT_FIFO_THRESH_MODE
    case UART_WK_MODE_FIFO_THRESH:
        if (cfg->rx_fifo_threshold > UART_LL_WAKEUP_FIFO_THRED_MAX(hw)) {
            ret = ESP_ERR_INVALID_ARG;
        } else {
            uart_ll_set_wakeup_fifo_thrd(hw, cfg->rx_fifo_threshold);
        }
        break;
#endif
#if SOC_UART_WAKEUP_SUPPORT_START_BIT_MODE
    case UART_WK_MODE_START_BIT:
        break;
#endif
#if SOC_UART_WAKEUP_SUPPORT_CHAR_SEQ_MODE
    case UART_WK_MODE_CHAR_SEQ:
        ret = uart_char_seq_wk_configure(hw, cfg->wake_chars_seq);
        break;
#endif
    default:
        ret = ESP_ERR_INVALID_ARG;
        break;
    }

    return ret;
}

esp_err_t uart_wakeup_clear(uart_port_t uart_num, uart_wakeup_mode_t wakeup_mode)
{
#if SOC_PM_SUPPORT_PMU_CLK_ICG
    // When hp uarts are utilized, the main XTAL need to be PU and UARTx & IOMX ICG need to be ungate
    if (uart_num < SOC_UART_HP_NUM && wakeup_mode != UART_WK_MODE_ACTIVE_THRESH) {
        esp_sleep_clock_config(UART_LL_SLEEP_CLOCK(uart_num), ESP_SLEEP_CLOCK_OPTION_GATE);
        esp_sleep_clock_config(ESP_SLEEP_CLOCK_IOMUX, ESP_SLEEP_CLOCK_OPTION_GATE);
        esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);
    }
#endif
    return ESP_OK;
}

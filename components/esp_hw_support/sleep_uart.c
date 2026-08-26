/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_sleep.h"
#include "esp_private/sleep_uart.h"
#include "esp_rom_serial_output.h"
#include "hal/uart_ll.h"

#if SOC_PM_SUPPORT_TOP_PD
#include "esp_private/esp_pmu.h"
#include "hal/pmu_ll.h"
#endif

#if CONFIG_PM_SLP_IRAM_OPT
#define SLEEP_UART_FN_ATTR      FORCE_IRAM_ATTR
#else
#define SLEEP_UART_FN_ATTR
#endif

// UART handling mode configuration for each UART port
static esp_sleep_uart_handling_mode_t s_uart_handling[SOC_UART_HP_NUM] = {
    [0 ... SOC_UART_HP_NUM - 1] = ESP_SLEEP_AUTO_FLUSH_SUSPEND_UART
};

// Bitmap of suspended UARTs for resume
static uint32_t s_suspended_uarts_bmap = 0;

// Suspend a single UART and record it in the bitmap
static SLEEP_UART_FN_ATTR void suspend_uart(int uart_num)
{
    uart_ll_force_xoff(uart_num);
    s_suspended_uarts_bmap |= BIT(uart_num);
#ifdef UART_LL_FSM_TX_WAIT_SEND
    uint32_t uart_fsm = 0;
    do {
        uart_fsm = uart_ll_get_tx_fsm_status(uart_num);
    } while (!(uart_fsm == UART_LL_FSM_IDLE || uart_fsm == UART_LL_FSM_TX_WAIT_SEND));
#else
    while (uart_ll_get_tx_fsm_status(uart_num) != 0) {}
#endif
}

/**
 * @brief Determine the actual UART handling mode based on configuration and sleep parameters
 *
 * Default strategy selection:
 * - Deep sleep: Always flush FIFO before entering sleep to avoid data loss
 * - Light sleep (chips without PD_TOP support): Suspend UART before CPU frequency switch
 * - Light sleep (chips with PD_TOP support):
 *   - If TOP domain is NOT powered down: Suspend UART for faster sleep entry
 *   - If TOP domain IS powered down: Flush console UART for data integrity,
 *     discard non-console UARTs to save power
 *
 * Special handling:
 * - Console UART with cache-safe assertion enabled: Always flush to ensure
 *   debug output is visible even if cache is disabled
 */
static SLEEP_UART_FN_ATTR esp_sleep_uart_handling_mode_t get_uart_handling_mode(int uart_num, esp_sleep_uart_handling_mode_t configured_handling, uint32_t sleep_flags, bool deep_sleep)
{
    esp_sleep_uart_handling_mode_t handling = configured_handling;
    __attribute__((unused)) bool is_console_uart = false;
#if (CONFIG_ESP_CONSOLE_UART_NUM != -1)
    is_console_uart = (uart_num == CONFIG_ESP_CONSOLE_UART_NUM);
#if CONFIG_ESP_SLEEP_CACHE_SAFE_ASSERTION
    if (is_console_uart) {
        handling = ESP_SLEEP_ALWAYS_FLUSH_UART;
    }
#endif /* CONFIG_ESP_SLEEP_CACHE_SAFE_ASSERTION */
#endif /* CONFIG_ESP_CONSOLE_UART_NUM != -1 */

    // Resolve AUTO mode into specific strategy
    if (handling == ESP_SLEEP_AUTO_FLUSH_SUSPEND_UART) {
        // Default: flush for deep sleep, suspend for light sleep
        handling = deep_sleep ? ESP_SLEEP_ALWAYS_FLUSH_UART : ESP_SLEEP_ALWAYS_SUSPEND_UART;
#if SOC_PM_SUPPORT_TOP_PD
        // If TOP domain (where UART belongs) is powered down during sleep:
        // - Console UART: flush to preserve debug output
        // - Non-console UART: discard to save power and enter sleep faster
        if (sleep_flags & PMU_SLEEP_PD_TOP) {
            handling = is_console_uart ? ESP_SLEEP_ALWAYS_FLUSH_UART : ESP_SLEEP_ALWAYS_DISCARD_UART;
        }
#endif
    }
    return handling;
}

void SLEEP_UART_FN_ATTR sleep_uart_prepare(uint32_t sleep_flags, bool deep_sleep)
{
    s_suspended_uarts_bmap = 0;
    for (int i = 0; i < SOC_UART_HP_NUM; ++i) {
        if (!uart_ll_is_enabled(i)) {
            continue;
        }
        esp_sleep_uart_handling_mode_t handling = get_uart_handling_mode(i, s_uart_handling[i], sleep_flags, deep_sleep);
        switch (handling) {
            case ESP_SLEEP_ALWAYS_FLUSH_UART:
                esp_rom_output_tx_wait_idle(i);
                break;
            case ESP_SLEEP_ALWAYS_SUSPEND_UART:
                suspend_uart(i);
                break;
            case ESP_SLEEP_ALWAYS_DISCARD_UART:
                // Suspend uart first before reset uart to avoid garbled code.
                suspend_uart(i);
                uart_ll_txfifo_rst(UART_LL_GET_HW(i));
                uart_ll_rxfifo_rst(UART_LL_GET_HW(i));
                break;
            default:
                // ESP_SLEEP_AUTO_FLUSH_SUSPEND_UART should have been resolved in get_uart_handling_mode
                break;
        }
    }
}

void SLEEP_UART_FN_ATTR sleep_uart_resume(void)
{
    for (int i = 0; i < SOC_UART_HP_NUM; ++i) {
        if (s_suspended_uarts_bmap & 0x1) {
            uart_ll_force_xon(i);
        }
        s_suspended_uarts_bmap >>= 1;
    }
}

esp_err_t sleep_uart_set_handling_mode(int uart_num, esp_sleep_uart_handling_mode_t handling_mode)
{
    if (handling_mode > ESP_SLEEP_NO_HANDLING) {
        return ESP_ERR_INVALID_ARG;
    }
    if (uart_num < 0 || uart_num >= SOC_UART_HP_NUM) {
        return ESP_ERR_INVALID_ARG;
    }
    s_uart_handling[uart_num] = handling_mode;
    return ESP_OK;
}

esp_err_t esp_sleep_set_console_uart_handling_mode(esp_sleep_uart_handling_mode_t handling_mode)
{
    esp_err_t ret = ESP_ERR_NOT_SUPPORTED;
#if (CONFIG_ESP_CONSOLE_UART_NUM != -1)
    ret = sleep_uart_set_handling_mode(CONFIG_ESP_CONSOLE_UART_NUM, handling_mode);
#endif
    return ret;
}

/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_err.h"
#include "esp_check.h"
#include "lp_core_uart.h"
#include "driver/rtc_io.h"
#include "driver/lp_io.h"
#include "soc/uart_periph.h"
#include "soc/lp_uart_struct.h"
#include "hal/uart_hal.h"
#include "hal/rtc_io_hal.h"
#include "hal/rtc_io_types.h"
#include "esp_clk_tree.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/uart_share_hw_ctrl.h"

static const char *LP_UART_TAG = "lp_uart";

#define LP_UART_PORT_NUM    LP_UART_NUM_0
#define LP_UART_TX_IDLE_NUM_DEFAULT     (0U)

/* LP UART HAL Context */
uart_hal_context_t hal = {
    .dev = (uart_dev_t *)UART_LL_GET_HW(LP_UART_NUM_0),
};

static esp_err_t lp_core_uart_param_config(const lp_core_uart_cfg_t *cfg)
{
    esp_err_t ret = ESP_OK;

    /* Argument sanity check */
    if ((cfg->uart_proto_cfg.rx_flow_ctrl_thresh > SOC_LP_UART_FIFO_LEN) ||
            (cfg->uart_proto_cfg.flow_ctrl > UART_HW_FLOWCTRL_MAX) ||
            (cfg->uart_proto_cfg.data_bits > UART_DATA_BITS_MAX)) {
        // Invalid config
        return ESP_ERR_INVALID_ARG;
    }

    /* Get LP UART source clock frequency */
    uint32_t sclk_freq = 0;
    soc_periph_lp_uart_clk_src_t clk_src = cfg->lp_uart_source_clk ? cfg->lp_uart_source_clk : LP_UART_SCLK_DEFAULT;
    ret = esp_clk_tree_src_get_freq_hz((soc_module_clk_t)clk_src, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &sclk_freq);
    if (ret != ESP_OK) {
        // Unable to fetch LP UART source clock frequency
        return ESP_FAIL;
    }

    // LP UART clock source is mixed with other peripherals in the same register
    LP_UART_SRC_CLK_ATOMIC() {
        /* Enable LP UART bus clock */
        lp_uart_ll_enable_bus_clock(0, true);
        lp_uart_ll_set_source_clk(hal.dev, clk_src);
    }

    /* Initialize LP UART HAL with default parameters */
    uart_hal_init(&hal, LP_UART_PORT_NUM);

    /* Override protocol parameters from the configuration */
    lp_uart_ll_set_baudrate(hal.dev, cfg->uart_proto_cfg.baud_rate, sclk_freq);
    uart_hal_set_parity(&hal, cfg->uart_proto_cfg.parity);
    uart_hal_set_data_bit_num(&hal, cfg->uart_proto_cfg.data_bits);
    uart_hal_set_stop_bits(&hal, cfg->uart_proto_cfg.stop_bits);
    uart_hal_set_tx_idle_num(&hal, LP_UART_TX_IDLE_NUM_DEFAULT);
    uart_hal_set_hw_flow_ctrl(&hal, cfg->uart_proto_cfg.flow_ctrl, cfg->uart_proto_cfg.rx_flow_ctrl_thresh);

    /* Reset Tx/Rx FIFOs */
    uart_hal_rxfifo_rst(&hal);
    uart_hal_txfifo_rst(&hal);

    return ret;
}

static esp_err_t lp_uart_config_io(gpio_num_t pin, rtc_gpio_mode_t direction, uint32_t idx)
{
    /* Skip configuration if the LP_IO is -1 */
    if (pin < 0) {
        return ESP_OK;
    }

    /* Initialize LP_IO */
    esp_err_t ret = rtc_gpio_init(pin);
    if (ret != ESP_OK) {
        return ESP_FAIL;
    }

    /* Set LP_IO direction */
    ret = rtc_gpio_set_direction(pin, direction);
    if (ret != ESP_OK) {
        return ESP_FAIL;
    }

    /* Connect pins */
    const uart_periph_sig_t *upin = &uart_periph_signal[LP_UART_PORT_NUM].pins[idx];
#if !SOC_LP_GPIO_MATRIX_SUPPORTED
    /* When LP_IO Matrix is not support, LP_IO Mux must be connected to the pins */
    ret = rtc_gpio_iomux_func_sel(pin, upin->iomux_func);
#else
    /* If the configured pin is the default LP_IO Mux pin for LP UART, then set the LP_IO MUX function */
    if (upin->default_gpio == pin) {
        ret = rtc_gpio_iomux_func_sel(pin, upin->iomux_func);
    } else {
        /* Select FUNC1 for LP_IO Matrix */
        ret = rtc_gpio_iomux_func_sel(pin, 1);
        /* Connect the LP_IO to the LP UART peripheral signal */
        if (direction == RTC_GPIO_MODE_OUTPUT_ONLY) {
            ret = lp_gpio_connect_out_signal(pin, UART_PERIPH_SIGNAL(LP_UART_PORT_NUM, idx), 0, 0);
        } else {
            ret = lp_gpio_connect_in_signal(pin, UART_PERIPH_SIGNAL(LP_UART_PORT_NUM, idx), 0);
        }
    }
#endif /* SOC_LP_GPIO_MATRIX_SUPPORTED */

    return ret;
}

static esp_err_t lp_core_uart_set_pin(const lp_core_uart_cfg_t *cfg)
{
    esp_err_t ret = ESP_OK;

    /* Argument sanity check */
#if !SOC_LP_GPIO_MATRIX_SUPPORTED
    const uart_periph_sig_t *pins = uart_periph_signal[LP_UART_PORT_NUM].pins;
    // LP_UART has its fixed IOs
    ESP_RETURN_ON_FALSE((cfg->uart_pin_cfg.tx_io_num < 0 || (cfg->uart_pin_cfg.tx_io_num == pins[SOC_UART_TX_PIN_IDX].default_gpio)), ESP_FAIL, LP_UART_TAG, "tx_io_num error");
    ESP_RETURN_ON_FALSE((cfg->uart_pin_cfg.rx_io_num < 0 || (cfg->uart_pin_cfg.rx_io_num == pins[SOC_UART_RX_PIN_IDX].default_gpio)), ESP_FAIL, LP_UART_TAG, "rx_io_num error");
    ESP_RETURN_ON_FALSE((cfg->uart_pin_cfg.rts_io_num < 0 || (cfg->uart_pin_cfg.rts_io_num == pins[SOC_UART_RTS_PIN_IDX].default_gpio)), ESP_FAIL, LP_UART_TAG, "rts_io_num error");
    ESP_RETURN_ON_FALSE((cfg->uart_pin_cfg.cts_io_num < 0 || (cfg->uart_pin_cfg.cts_io_num == pins[SOC_UART_CTS_PIN_IDX].default_gpio)), ESP_FAIL, LP_UART_TAG, "cts_io_num error");
#else
    // LP_UART signals can be routed to any LP_IOs
    ESP_RETURN_ON_FALSE((cfg->uart_pin_cfg.tx_io_num < 0 || rtc_gpio_is_valid_gpio(cfg->uart_pin_cfg.tx_io_num)), ESP_FAIL, LP_UART_TAG, "tx_io_num error");
    ESP_RETURN_ON_FALSE((cfg->uart_pin_cfg.rx_io_num < 0 || rtc_gpio_is_valid_gpio(cfg->uart_pin_cfg.rx_io_num)), ESP_FAIL, LP_UART_TAG, "rx_io_num error");
    ESP_RETURN_ON_FALSE((cfg->uart_pin_cfg.rts_io_num < 0 || rtc_gpio_is_valid_gpio(cfg->uart_pin_cfg.rts_io_num)), ESP_FAIL, LP_UART_TAG, "rts_io_num error");
    ESP_RETURN_ON_FALSE((cfg->uart_pin_cfg.cts_io_num < 0 || rtc_gpio_is_valid_gpio(cfg->uart_pin_cfg.cts_io_num)), ESP_FAIL, LP_UART_TAG, "cts_io_num error");
#endif  /* SOC_LP_GPIO_MATRIX_SUPPORTED */

    /* Configure Tx Pin */
    ret = lp_uart_config_io(cfg->uart_pin_cfg.tx_io_num, RTC_GPIO_MODE_OUTPUT_ONLY, SOC_UART_TX_PIN_IDX);
    /* Configure Rx Pin */
    ret = lp_uart_config_io(cfg->uart_pin_cfg.rx_io_num, RTC_GPIO_MODE_INPUT_ONLY, SOC_UART_RX_PIN_IDX);
    /* Configure RTS Pin */
    ret = lp_uart_config_io(cfg->uart_pin_cfg.rts_io_num, RTC_GPIO_MODE_OUTPUT_ONLY, SOC_UART_RTS_PIN_IDX);
    /* Configure CTS Pin */
    ret = lp_uart_config_io(cfg->uart_pin_cfg.cts_io_num, RTC_GPIO_MODE_INPUT_ONLY, SOC_UART_CTS_PIN_IDX);

    return ret;
}

esp_err_t lp_core_uart_init(const lp_core_uart_cfg_t *cfg)
{
    esp_err_t ret = ESP_OK;

    /* Argument sanity check */
    if (!cfg) {
        // NULL configuration
        return ESP_ERR_INVALID_ARG;
    }

    /* Configure LP UART protocol parameters */
    ret = lp_core_uart_param_config(cfg);
    if (ret != ESP_OK) {
        // Invalid protocol configuration
        return ESP_FAIL;
    }

    /* Configure LP UART IO pins */
    ret = lp_core_uart_set_pin(cfg);
    if (ret != ESP_OK) {
        // Invalid IO configuration
        return ESP_FAIL;
    }

    return ret;
}

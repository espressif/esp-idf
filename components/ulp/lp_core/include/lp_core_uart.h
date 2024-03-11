/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp_err.h"
#include "hal/uart_types.h"
#include "hal/gpio_types.h"

/**
 * Default LP_IO Mux pins for LP UART
 */
#if CONFIG_IDF_TARGET_ESP32P4
#define LP_UART_DEFAULT_TX_GPIO_NUM GPIO_NUM_14
#define LP_UART_DEFAULT_RX_GPIO_NUM GPIO_NUM_15
#define LP_UART_DEFAULT_RTS_GPIO_NUM (-1)
#define LP_UART_DEFAULT_CTS_GPIO_NUM (-1)
#elif CONFIG_IDF_TARGET_ESP32C6
#define LP_UART_DEFAULT_TX_GPIO_NUM GPIO_NUM_5
#define LP_UART_DEFAULT_RX_GPIO_NUM GPIO_NUM_4
#define LP_UART_DEFAULT_RTS_GPIO_NUM GPIO_NUM_2
#define LP_UART_DEFAULT_CTS_GPIO_NUM GPIO_NUM_3
#else
#error "LP IO Mux pins undefined for LP UART"
#endif /* CONFIG_IDF_TARGET_ESP32P4 */

/**
 * @brief LP UART IO pins configuration
 */
typedef struct {
    gpio_num_t tx_io_num;               /*!< GPIO pin for UART Tx signal. Only GPIO#5 can be used as the UART Tx pin */
    gpio_num_t rx_io_num;               /*!< GPIO pin for UART Rx signal. Only GPIO#4 can be used as the UART Rx pin */
    gpio_num_t rts_io_num;              /*!< GPIO pin for UART RTS signal. Only GPIO#2 can be used as the UART RTS pin */
    gpio_num_t cts_io_num;              /*!< GPIO pin for UART CTS signal. Only GPIO#3 can be used as the UART CTS pin */
} lp_core_uart_pin_cfg_t;

/**
 * @brief LP UART protocol configuration
 */
typedef struct {
    int baud_rate;                      /*!< LP UART baud rate */
    uart_word_length_t data_bits;       /*!< LP UART byte size */
    uart_parity_t parity;               /*!< LP UART parity mode */
    uart_stop_bits_t stop_bits;         /*!< LP UART stop bits */
    uart_hw_flowcontrol_t flow_ctrl;    /*!< LP UART HW flow control mode (cts/rts) */
    uint8_t rx_flow_ctrl_thresh;        /*!< LP UART HW RTS threshold */
} lp_core_uart_proto_cfg_t;

/**
 * @brief LP UART configuration parameters
 */
typedef struct {
    lp_core_uart_pin_cfg_t uart_pin_cfg;        /*!< LP UART pin configuration */
    lp_core_uart_proto_cfg_t uart_proto_cfg;    /*!< LP UART protocol configuration */
    lp_uart_sclk_t lp_uart_source_clk;          /*!< LP UART source clock selection */
} lp_core_uart_cfg_t;

/* Default LP UART GPIO settings */
#define LP_UART_DEFAULT_GPIO_CONFIG()                               \
        .uart_pin_cfg.tx_io_num = LP_UART_DEFAULT_TX_GPIO_NUM,      \
        .uart_pin_cfg.rx_io_num = LP_UART_DEFAULT_RX_GPIO_NUM,      \
        .uart_pin_cfg.rts_io_num = LP_UART_DEFAULT_RTS_GPIO_NUM,    \
        .uart_pin_cfg.cts_io_num = LP_UART_DEFAULT_CTS_GPIO_NUM,    \

/* Default LP UART protocol config */
#define LP_UART_DEFAULT_PROTO_CONFIG()                           \
        .uart_proto_cfg.baud_rate = 115200,                      \
        .uart_proto_cfg.data_bits = UART_DATA_8_BITS,            \
        .uart_proto_cfg.parity = UART_PARITY_DISABLE,            \
        .uart_proto_cfg.stop_bits = UART_STOP_BITS_1,            \
        .uart_proto_cfg.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,    \
        .uart_proto_cfg.rx_flow_ctrl_thresh = 0,                 \

/* Default LP UART source clock config */
#define LP_UART_DEFAULT_CLOCK_CONFIG()              \
        .lp_uart_source_clk = LP_UART_SCLK_DEFAULT, \

/* Default LP UART GPIO settings and protocol parametes */
#define LP_CORE_UART_DEFAULT_CONFIG()               \
    {                                               \
        LP_UART_DEFAULT_GPIO_CONFIG()               \
        LP_UART_DEFAULT_PROTO_CONFIG()              \
        LP_UART_DEFAULT_CLOCK_CONFIG()              \
    }

/**
 * @brief Initialize and configure the LP UART to be used from the LP core
 *
 * @note The LP UART initialization must be called from the main core (HP CPU)
 *
 * @param cfg           Configuration parameters
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t lp_core_uart_init(const lp_core_uart_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc_caps.h"
#include "soc/interrupts.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "soc/uart_pins.h"
#include "soc/uart_struct.h"
#include "soc/uart_reg.h"
#if SOC_PAU_SUPPORTED
#include "soc/regdma.h"
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SOC_UART_PERIPH_SIGNAL_TX = 0,      /* Transmit signal*/
    SOC_UART_PERIPH_SIGNAL_RX = 1,      /* Receive signal*/
    SOC_UART_PERIPH_SIGNAL_RTS = 2,     /* Request to send signal (0 indicates allowing the other end to send data)*/
    SOC_UART_PERIPH_SIGNAL_CTS = 3,     /* Clear to send signal (0 indicates I am allowed to send data)*/
    SOC_UART_PERIPH_SIGNAL_DTR = 4,     /* Data terminal ready signal (1 indicates I am ready to transmit data)*/
    SOC_UART_PERIPH_SIGNAL_DSR = 5,     /* Data set ready signal (1 indicates the other end is ready to transmit data)*/
    SOC_UART_PERIPH_SIGNAL_MAX,
} soc_uart_periph_signal_t;

/**
 * @brief Macro that can be used to retrieve the signal of a certain pin for a
 *        certain UART.
 */
#define UART_PERIPH_SIGNAL(IDX, PIN) (uart_periph_signal[(IDX)].pins[(PIN)].signal)

typedef struct {
    /* Default GPIO number for this UART pin in the IOMUX.
     * This value can be -1 if there is no default GPIO for a pin.
     * For example, ESP32-C3 doesn't have any default GPIO for
     * U0CTS and U0RTS. */
    int32_t default_gpio    : 15;
    /* Func which should be assigned to the GPIO to be used as UART */
    int32_t iomux_func      : 4;
    /* Marks if the current UART pin is input (or not) */
    uint32_t input          : 1;
    /* Signal in the GPIO signal map. */
    uint32_t signal         : 12;
} uart_periph_sig_t;

typedef struct {
    const uart_periph_sig_t pins[SOC_UART_PERIPH_SIGNAL_MAX];
    const uint8_t irq;
} uart_signal_conn_t;

extern const uart_signal_conn_t uart_periph_signal[SOC_UART_NUM];

#if SOC_PAU_SUPPORTED
typedef struct {
    const periph_retention_module_t module;
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
} uart_reg_retention_info_t;

extern const uart_reg_retention_info_t uart_reg_retention_info[SOC_UART_HP_NUM];
#endif

#ifdef __cplusplus
}
#endif

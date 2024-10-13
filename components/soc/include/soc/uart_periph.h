/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
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
#include "soc/regdma.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SOC_UART_TX_PIN_IDX  (0)
#define SOC_UART_RX_PIN_IDX  (1)
#define SOC_UART_RTS_PIN_IDX (2)
#define SOC_UART_CTS_PIN_IDX (3)

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
    const uart_periph_sig_t pins[SOC_UART_PINS_COUNT];
    const uint8_t irq;
} uart_signal_conn_t;

extern const uart_signal_conn_t uart_periph_signal[SOC_UART_NUM];

#if SOC_UART_SUPPORT_SLEEP_RETENTION
typedef struct {
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
} uart_reg_retention_info_t;

extern const uart_reg_retention_info_t uart_reg_retention_info[SOC_UART_HP_NUM];
#endif

#ifdef __cplusplus
}
#endif

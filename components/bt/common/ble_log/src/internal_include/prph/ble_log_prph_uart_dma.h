/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_PRPH_UART_DMA_H__
#define __BLE_LOG_PRPH_UART_DMA_H__

/* ----------------------------------------- */
/* BLE Log - Peripheral Interface - UART DMA */
/* ----------------------------------------- */

/* INCLUDE */
#include "ble_log_prph.h"

#include "driver/uart.h"
#include "driver/uhci.h"

/* MACRO */
#if CONFIG_BLE_LOG_PRPH_UART_DMA_PORT == UART_NUM_0
#define BLE_LOG_PRPH_UART_DMA_REDIR         (1)
#else
#define BLE_LOG_PRPH_UART_DMA_REDIR         (0)
#endif /* CONFIG_BLE_LOG_PRPH_UART_DMA_PORT == UART_NUM_0 */

/* TYPEDEF */
typedef struct {
    ble_log_prph_trans_t *trans;
    uint8_t trans_buf[0];
} ble_log_prph_trans_ctx_t;

#endif /* __BLE_LOG_PRPH_UART_DMA_H__ */

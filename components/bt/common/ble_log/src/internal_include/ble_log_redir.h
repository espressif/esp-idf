/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_REDIR_H__
#define __BLE_LOG_REDIR_H__

/* ---------------------------------------------- */
/*     BLE Log - UART Redirection Stream API      */
/* ---------------------------------------------- */

#include "ble_log_lbm_v2.h"

#if BLE_LOG_UART_REDIR_ENABLED
void ble_log_lbm_stream_write(ble_log_redir_t *redir, ble_log_src_t src_code,
                              const uint8_t *data, size_t len);
void ble_log_lbm_stream_flush(ble_log_redir_t *redir, ble_log_src_t src_code);
#endif /* BLE_LOG_UART_REDIR_ENABLED */

#endif /* __BLE_LOG_REDIR_H__ */

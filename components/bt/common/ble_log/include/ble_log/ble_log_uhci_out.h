/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_SPI_OUT_H__
#define __BT_SPI_OUT_H__

#include <stdarg.h>
#include <string.h>
#include "driver/uhci.h"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#include "esp_rom_uart.h"
#include "esp_timer.h"
#include "freertos/semphr.h"

// Public enums
enum {
    BLE_LOG_UHCI_OUT_SOURCE_ESP = 0,
    BLE_LOG_UHCI_OUT_SOURCE_ESP_ISR = 6,
    BLE_LOG_UHCI_OUT_SOURCE_LL_HCI = 8,
    BLE_LOG_UHCI_OUT_SOURCE_USER = 0x10,
    BLE_LOG_UHCI_OUT_SOURCE_LOSS = 0xFF,
};

// Public functions
int ble_log_uhci_out_init(void);
void ble_log_uhci_out_deinit(void);
void ble_log_uhci_out_ll_write(uint32_t len, const uint8_t *addr, uint32_t len_append,
                               const uint8_t *addr_append, uint32_t flag);
void ble_log_uhci_out_ll_log_ev_proc(void);
void ble_log_uhci_out_dump_all(void);

#endif // __BT_SPI_OUT_H__

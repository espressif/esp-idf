/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_SPI_OUT_H__
#define __BT_SPI_OUT_H__

#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/semphr.h"

// Public typedefs
#define BLE_LOG_SPI_OUT_SOURCE_ESP 0
#define BLE_LOG_SPI_OUT_SOURCE_ESP_LEGACY 1
#define BLE_LOG_SPI_OUT_SOURCE_BLUEDROID 2
#define BLE_LOG_SPI_OUT_SOURCE_NIMBLE 3
#define BLE_LOG_SPI_OUT_SOURCE_HCI_UPSTREAM 4
#define BLE_LOG_SPI_OUT_SOURCE_HCI_DOWNSTREAM 5
#define BLE_LOG_SPI_OUT_SOURCE_SYNC 0xFE
#define BLE_LOG_SPI_OUT_SOURCE_LOSS 0xFF

// Public functions
void ble_log_spi_out_init(void);
void ble_log_spi_out_deinit(void);
void ble_log_spi_out_write(uint8_t source, const uint8_t *addr, uint16_t len);
void ble_log_spi_out_write_esp(uint32_t len, const uint8_t *addr, bool end);
void ble_log_spi_out_ts_sync_start(void);
void ble_log_spi_out_ts_sync_stop(void);

#endif // __BT_SPI_OUT_H__

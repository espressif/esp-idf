/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_SPI_OUT_H__
#define __BT_SPI_OUT_H__

#include <stdarg.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/semphr.h"

// Public typedefs
#define BLE_LOG_SPI_OUT_SOURCE_ESP 0
#define BLE_LOG_SPI_OUT_SOURCE_ESP_LEGACY 1
#define BLE_LOG_SPI_OUT_SOURCE_BLUEDROID 2
#define BLE_LOG_SPI_OUT_SOURCE_NIMBLE 3
#define BLE_LOG_SPI_OUT_SOURCE_HCI_UPSTREAM 4
#define BLE_LOG_SPI_OUT_SOURCE_HCI_DOWNSTREAM 5
#define BLE_LOG_SPI_OUT_SOURCE_ESP_ISR 6
#define BLE_LOG_SPI_OUT_SOURCE_ESP_LEGACY_ISR 7
#define BLE_LOG_SPI_OUT_SOURCE_USER 0x10
#define BLE_LOG_SPI_OUT_SOURCE_SYNC 0xFE
#define BLE_LOG_SPI_OUT_SOURCE_LOSS 0xFF

// SPI Log Level Definitions
#define BLE_LOG_SPI_OUT_LEVEL_NONE      0  /*!< No log output */
#define BLE_LOG_SPI_OUT_LEVEL_ERROR     1  /*!< Critical errors that SPI driver cannot recover from */
#define BLE_LOG_SPI_OUT_LEVEL_WARN      2  /*!< Recoverable error conditions in SPI communication */
#define BLE_LOG_SPI_OUT_LEVEL_INFO      3  /*!< Informational messages about SPI transactions */
#define BLE_LOG_SPI_OUT_LEVEL_DEBUG     4  /*!< Detailed debug information, such as SPI register values */
#define BLE_LOG_SPI_OUT_LEVEL_VERBOSE   5  /*!< Very detailed debugging logs, potentially flooding output */
#define BLE_LOG_SPI_OUT_LEVEL_MAX       6  /*!< Number of SPI log levels supported */

// Public functions
int ble_log_spi_out_init(void);
void ble_log_spi_out_deinit(void);
void ble_log_spi_out_timer_control(bool enable);
int ble_log_spi_out_write(uint8_t source, const uint8_t *addr, uint16_t len);
void ble_log_spi_out_ll_write(uint32_t len, const uint8_t *addr, uint32_t len_append,\
                              const uint8_t *addr_append, uint32_t flag);
void ble_log_spi_out_ll_log_ev_proc(void);
void ble_log_spi_out_ts_sync_start(void);
void ble_log_spi_out_ts_sync_stop(void);
int ble_log_spi_out_printf(uint8_t source, const char *format, ...);
int ble_log_spi_out_printf_enh(uint8_t source, uint8_t level, const char *tag, const char *format, ...);
int ble_log_spi_out_write_with_ts(uint8_t source, const uint8_t *addr, uint16_t len);
void ble_log_spi_out_dump_all(void);

#endif // __BT_SPI_OUT_H__

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

// Public enums
enum {
    BLE_LOG_SPI_OUT_SOURCE_ESP = 0,
    BLE_LOG_SPI_OUT_SOURCE_ESP_LEGACY,
    BLE_LOG_SPI_OUT_SOURCE_BLUEDROID,
    BLE_LOG_SPI_OUT_SOURCE_NIMBLE,
    BLE_LOG_SPI_OUT_SOURCE_HCI_UPSTREAM,
    BLE_LOG_SPI_OUT_SOURCE_HCI_DOWNSTREAM,
    BLE_LOG_SPI_OUT_SOURCE_ESP_ISR,
    BLE_LOG_SPI_OUT_SOURCE_ESP_LEGACY_ISR,
    BLE_LOG_SPI_OUT_SOURCE_LL_HCI,
    BLE_LOG_SPI_OUT_SOURCE_LE_AUDIO,
    BLE_LOG_SPI_OUT_SOURCE_USER = 0x10,
    BLE_LOG_SPI_OUT_SOURCE_SSC = 0xFD,
    BLE_LOG_SPI_OUT_SOURCE_SYNC,
    BLE_LOG_SPI_OUT_SOURCE_LOSS,
};

enum {
    BLE_LOG_SPI_OUT_LEVEL_NONE = 0,
    BLE_LOG_SPI_OUT_LEVEL_ERROR,
    BLE_LOG_SPI_OUT_LEVEL_WARN,
    BLE_LOG_SPI_OUT_LEVEL_INFO,
    BLE_LOG_SPI_OUT_LEVEL_DEBUG,
    BLE_LOG_SPI_OUT_LEVEL_VERBOSE,
    BLE_LOG_SPI_OUT_LEVEL_MAX,
};

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
void ble_log_spi_out_enable(bool enable);
void ble_log_spi_out_flush(void);
void ble_log_spi_out_le_audio_write(const uint8_t *addr, uint16_t len);

#endif // __BT_SPI_OUT_H__

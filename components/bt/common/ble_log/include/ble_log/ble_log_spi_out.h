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
#include "freertos/semphr.h"
#include "esp_heap_caps.h"
#include "esp_task.h"

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
    BLE_LOG_SPI_OUT_SOURCE_MESH,
    BLE_LOG_SPI_OUT_SOURCE_USER = 0x10,
    BLE_LOG_SPI_OUT_SOURCE_SSC = 0xFD,
    BLE_LOG_SPI_OUT_SOURCE_SYNC,
    BLE_LOG_SPI_OUT_SOURCE_LOSS,
};

#define BLE_LOG_SPI_OUT_LEVEL_NONE                  0
#define BLE_LOG_SPI_OUT_LEVEL_ERROR                 1
#define BLE_LOG_SPI_OUT_LEVEL_WARN                  2
#define BLE_LOG_SPI_OUT_LEVEL_INFO                  3
#define BLE_LOG_SPI_OUT_LEVEL_DEBUG                 4
#define BLE_LOG_SPI_OUT_LEVEL_VERBOSE               5
#define BLE_LOG_SPI_OUT_STR(x)                      #x
#define BLE_LOG_SPI_OUT_XSTR(x)                     BLE_LOG_SPI_OUT_STR(x)
#define BLE_LOG_SPI_OUT_BUILD_PREFIX(LEVEL, TAG)    "[" BLE_LOG_SPI_OUT_XSTR(LEVEL) "][" TAG "]"

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
void ble_log_spi_out_dump_all(void);
void ble_log_spi_out_enable(bool enable);
void ble_log_spi_out_flush(void);
void ble_log_spi_out_le_audio_write(const uint8_t *addr, uint16_t len);
int ble_log_spi_out_host_write(uint8_t source, const char *prefix, const char *format, ...);
int ble_log_spi_out_hci_write(uint8_t source, const uint8_t *addr, uint16_t len);
int ble_log_spi_out_mesh_write(const char *prefix, const char *format, ...);
#endif // __BT_SPI_OUT_H__

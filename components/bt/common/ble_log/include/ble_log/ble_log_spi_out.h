/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_SPI_OUT_H__
#define __BT_SPI_OUT_H__

#include <string.h>
#include "driver/spi_master.h"
#include "freertos/semphr.h"

// Public typedefs
typedef enum
{
    esp_controller = 0,
    ceva_controller = 1
} spi_out_source_t;

// Public functions
void ble_log_spi_out_init(void);
void ble_log_spi_out_deinit(void);
void ble_log_spi_out_write(uint32_t len, const uint8_t *addr, spi_out_source_t source);
void ble_log_spi_out_write_esp(uint32_t len, const uint8_t *addr, bool end);

#endif // __BT_SPI_OUT_H__

/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_rom_sys.h"
#include "esp_attr.h"

uint8_t IRAM_ATTR regi2c_read_impl(uint8_t block, uint8_t host_id, uint8_t reg_add)
{
    // TODO: [ESP32S31] IDF-14680
    return 0;
}

uint8_t IRAM_ATTR regi2c_read_mask_impl(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb)
{
    // TODO: [ESP32S31] IDF-14680
    return 0;
}

void IRAM_ATTR regi2c_write_impl(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data)
{
    // TODO: [ESP32S31] IDF-14680
}

void IRAM_ATTR regi2c_write_mask_impl(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data)
{
    // TODO: [ESP32S31] IDF-14680
}

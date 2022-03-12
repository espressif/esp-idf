/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Espressif private functions. Not for peripherals. Don't use it in your app.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read internal register
 *
 * @param block     Block of the register
 * @param host_id   Host of the register
 * @param reg_add   Address of the register
 * @return uint8_t  Register value
 */
uint8_t esp_rom_regi2c_read(uint8_t block, uint8_t host_id, uint8_t reg_add);

/**
 * @brief Read internal register, in bits
 *
 * @param block     Block of the register
 * @param host_id   Host of the register
 * @param reg_add   Address of the register
 * @param msb       MSB of the register
 * @param lsb       LSB of the register
 * @return uint8_t  Register value
 */
uint8_t esp_rom_regi2c_read_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb);

/**
 * @brief Write internal register
 *
 * @param block     Block of the register
 * @param host_id   Host of the register
 * @param reg_add   Address of the register
 * @param data      Value to write
 */
void esp_rom_regi2c_write(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t data);

/**
 * @brief Write internal register, in bits
 *
 * @param block     Block of the register
 * @param host_id   Host of the register
 * @param reg_add   Address of the register
 * @param msb       MSB of the register
 * @param lsb       LSB of the register
 * @param data      Value to write
 */
void esp_rom_regi2c_write_mask(uint8_t block, uint8_t host_id, uint8_t reg_add, uint8_t msb, uint8_t lsb, uint8_t data);

#ifdef __cplusplus
}
#endif

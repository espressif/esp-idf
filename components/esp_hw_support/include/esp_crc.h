/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// This header is only a wrapper on ROM CRC API
#include "esp_rom_crc.h"

/**
* @brief Convenience wrappers for the ROM CRC helpers.
*
* These wrappers keep the ROM helper behavior unchanged: each call bitwise
* inverts the CRC value before and after processing. To start a calculation,
* pass the bitwise inverse of the CRC flavor's initial value. To continue over
* another buffer, pass the previous return value. After the last chunk, bitwise
* invert the return value again and then apply the CRC flavor's final XOR.
*
* The correct initial value and final XOR depend on the CRC flavor that you are
* implementing, so there is no single universal `INITIAL` or `FINAL_XOR`
* constant for a given helper.
*/

/**
* @brief CRC32 value in little endian.
*
* @param crc: Bitwise inverse of the CRC variant's initial value on the first
*             call, or the previous return value to continue a calculation
* @param buf: Data buffer that used to calculate the CRC value
* @param len: Length of the data buffer
* @return CRC32 value
*/
static inline uint32_t esp_crc32_le(uint32_t crc, uint8_t const *buf, uint32_t len)
{
    return esp_rom_crc32_le(crc, buf, len);
}

/**
* @brief CRC32 value in big endian.
*
* @param crc: Bitwise inverse of the CRC variant's initial value on the first
*             call, or the previous return value to continue a calculation
* @param buf: Data buffer that used to calculate the CRC value
* @param len: Length of the data buffer
* @return CRC32 value
*/
static inline uint32_t esp_crc32_be(uint32_t crc, uint8_t const *buf, uint32_t len)
{
    return esp_rom_crc32_be(crc, buf, len);
}

/**
* @brief CRC16 value in little endian.
*
* @param crc: Bitwise inverse of the CRC variant's initial value on the first
*             call, or the previous return value to continue a calculation
* @param buf: Data buffer that used to calculate the CRC value
* @param len: Length of the data buffer
* @return CRC16 value
*/
static inline uint16_t esp_crc16_le(uint16_t crc, uint8_t const *buf, uint32_t len)
{
    return esp_rom_crc16_le(crc, buf, len);
}

/**
* @brief CRC16 value in big endian.
*
* @param crc: Bitwise inverse of the CRC variant's initial value on the first
*             call, or the previous return value to continue a calculation
* @param buf: Data buffer that used to calculate the CRC value
* @param len: Length of the data buffer
* @return CRC16 value
*/
static inline uint16_t esp_crc16_be(uint16_t crc, uint8_t const *buf, uint32_t len)
{
    return esp_rom_crc16_be(crc, buf, len);
}

/**
* @brief CRC8 value in little endian.
*
* @param crc: Bitwise inverse of the CRC variant's initial value on the first
*             call, or the previous return value to continue a calculation
* @param buf: Data buffer that used to calculate the CRC value
* @param len: Length of the data buffer
* @return CRC8 value
*/
static inline uint8_t esp_crc8_le(uint8_t crc, uint8_t const *buf, uint32_t len)
{
    return esp_rom_crc8_le(crc, buf, len);
}

/**
* @brief CRC8 value in big endian.
*
* @param crc: Bitwise inverse of the CRC variant's initial value on the first
*             call, or the previous return value to continue a calculation
* @param buf: Data buffer that used to calculate the CRC value
* @param len: Length of the data buffer
* @return CRC8 value
*/
static inline uint8_t esp_crc8_be(uint8_t crc, uint8_t const *buf, uint32_t len)
{
    return esp_rom_crc8_be(crc, buf, len);
}

#ifdef __cplusplus
}
#endif

/*
 * SPDX-FileCopyrightText: 2010-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**           Notes about CRC API
 * The ROM CRC helpers accelerate CRC-8, CRC-16, and CRC-32 calculations for
 * the following base polynomials:
 * CRC-8        x8+x2+x1+1                                              0x07
 * CRC16-CCITT  x16+x12+x5+1                                            0x1021
 * CRC32        x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x1+1     0x04c11db7
 *
 * The `_le` and `_be` suffixes describe the bit processing order:
 *     - use the little-endian helper when `refin == true` and `refout == true`
 *     - use the big-endian helper when `refin == false` and `refout == false`
 *
 * These helpers invert the CRC register before and after processing each call.
 * As a result:
 *     - the first call must receive the bitwise inverse of the CRC variant's
 *       initial value
 *     - each later call must receive the previous helper's return value so
 *       non-contiguous buffers can be chained
 *     - the final CRC value must be bitwise inverted again before applying the
 *       CRC variant's `xorout`
 *
 * The initial value and final XOR depend on the CRC flavor that you are
 * implementing, so these helpers do not define a single universal
 * INITIAL/FINAL_XOR pair.
 *
 * Example for a CRC-32 flavor with `init = 0xffffffff`, `refin = true`,
 * `refout = true`, `xorout = 0xffffffff`:
 *     uint32_t crc = (uint32_t)~0xffffffff;
 *     crc = crc32_le(crc, buf0, length0);
 *     crc = crc32_le(crc, buf1, length1);
 *     crc = ~crc ^ 0xffffffff;
 *
 * Example for CRC-16/XMODEM with `init = 0x0000`, `refin = false`,
 * `refout = false`, `xorout = 0x0000`:
 *     uint16_t crc = (uint16_t)~0x0000;
 *     crc = crc16_be(crc, buf0, length0);
 *     crc = crc16_be(crc, buf1, length1);
 *     crc = (uint16_t)(~crc ^ 0x0000);
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
uint32_t esp_rom_crc32_le(uint32_t crc, uint8_t const *buf, uint32_t len);

/**
 * @brief CRC32 value in big endian.
 *
 * @param crc: Bitwise inverse of the CRC variant's initial value on the first
 *             call, or the previous return value to continue a calculation
 * @param buf: Data buffer that used to calculate the CRC value
 * @param len: Length of the data buffer
 * @return CRC32 value
 */
uint32_t esp_rom_crc32_be(uint32_t crc, uint8_t const *buf, uint32_t len);

/**
 * @brief CRC16 value in little endian.
 *
 * @param crc: Bitwise inverse of the CRC variant's initial value on the first
 *             call, or the previous return value to continue a calculation
 * @param buf: Data buffer that used to calculate the CRC value
 * @param len: Length of the data buffer
 * @return CRC16 value
 */
uint16_t esp_rom_crc16_le(uint16_t crc, uint8_t const *buf, uint32_t len);

/**
 * @brief CRC16 value in big endian.
 *
 * @param crc: Bitwise inverse of the CRC variant's initial value on the first
 *             call, or the previous return value to continue a calculation
 * @param buf: Data buffer that used to calculate the CRC value
 * @param len: Length of the data buffer
 * @return CRC16 value
 */
uint16_t esp_rom_crc16_be(uint16_t crc, uint8_t const *buf, uint32_t len);

/**
 * @brief CRC8 value in little endian.
 *
 * @param crc: Bitwise inverse of the CRC variant's initial value on the first
 *             call, or the previous return value to continue a calculation
 * @param buf: Data buffer that used to calculate the CRC value
 * @param len: Length of the data buffer
 * @return CRC8 value
 */
uint8_t esp_rom_crc8_le(uint8_t crc, uint8_t const *buf, uint32_t len);

/**
 * @brief CRC8 value in big endian.
 *
 * @param crc: Bitwise inverse of the CRC variant's initial value on the first
 *             call, or the previous return value to continue a calculation
 * @param buf: Data buffer that used to calculate the CRC value
 * @param len: Length of the data buffer
 * @return CRC8 value
 */
uint8_t esp_rom_crc8_be(uint8_t crc, uint8_t const *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

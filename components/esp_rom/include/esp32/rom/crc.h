// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ROM_CRC_H
#define ROM_CRC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup uart_apis, uart configuration and communication related apis
  * @brief uart apis
  */

/** @addtogroup uart_apis
  * @{
  */


/*           Notes about CRC APIs usage
 * The ESP32 ROM include some CRC tables and CRC APIs to speed up CRC calculation.
 * The CRC APIs include CRC8, CRC16, CRC32 algorithms for both little endian and big endian modes.
 * Here are the polynomials for the algorithms:
 * CRC-8        x8+x2+x1+1                                              0x07
 * CRC16-CCITT  x16+x12+x5+1                                            0x1021
 * CRC32        x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x1+1     0x04c11db7
 *
 * These group of CRC APIs are designed to calculate the data in buffers either continuous or not.
 * To make it easy, we had added a `~` at the beginning and the end of the functions.
 * To calculate non-continuous buffers, we can write the code like this:
 *     init = ~init;
 *     crc = crc32_le(init, buf0, length0);
 *     crc = crc32_le(crc, buf1, length1);
 *     crc = ~crc;
 *
 * However, it is not easy to select which API to use and give the correct parameters.
 * A specific CRC algorithm will include this parameters: width, polynomials, init, refin, refout, xorout
 *     refin and refout show the endian of the algorithm:
 *         if both of them are true, please use the little endian API.
 *         if both of them are false, please use the big endian API.
 *     xorout is the value which you need to be xored to the raw result.
 * However, these group of APIs need one '~' before and after the APIs.
 *
 * Here are some examples for CRC16:
 * CRC-16/CCITT, poly = 0x1021, init = 0x0000, refin = true, refout = true, xorout = 0x0000
 *     crc = ~crc16_le((uint16_t)~0x0000, buf, length);
 *
 * CRC-16/CCITT-FALSE, poly = 0x1021, init = 0xffff, refin = false, refout = false, xorout = 0x0000
 *     crc = ~crc16_be((uint16_t)~0xffff, buf, length);
 *
 * CRC-16/X25, poly = 0x1021, init = 0xffff, refin = true, refout = true, xorout = 0xffff
 *     crc = (~crc16_le((uint16_t)~(0xffff), buf, length))^0xffff;
 *
 * CRC-16/XMODEM, poly= 0x1021, init = 0x0000, refin = false, refout = false, xorout = 0x0000
 *     crc = ~crc16_be((uint16_t)~0x0000, buf, length);
 *
 *
 */

/**
  * @brief CRC32 value that is in little endian.
  *
  * @param  uint32_t crc : init crc value, use 0 at the first use.
  *
  * @param  uint8_t const *buf : buffer to start calculate crc.
  *
  * @param  uint32_t len : buffer length in byte.
  *
  * @return None
  */
uint32_t crc32_le(uint32_t crc, uint8_t const *buf, uint32_t len);

/**
  * @brief CRC32 value that is in big endian.
  *
  * @param  uint32_t crc : init crc value, use 0 at the first use.
  *
  * @param  uint8_t const *buf : buffer to start calculate crc.
  *
  * @param  uint32_t len : buffer length in byte.
  *
  * @return None
  */
uint32_t crc32_be(uint32_t crc, uint8_t const *buf, uint32_t len);

/**
  * @brief CRC16 value that is in little endian.
  *
  * @param  uint16_t crc : init crc value, use 0 at the first use.
  *
  * @param  uint8_t const *buf : buffer to start calculate crc.
  *
  * @param  uint32_t len : buffer length in byte.
  *
  * @return None
  */
uint16_t crc16_le(uint16_t crc, uint8_t const *buf, uint32_t len);

/**
  * @brief CRC16 value that is in big endian.
  *
  * @param  uint16_t crc : init crc value, use 0 at the first use.
  *
  * @param  uint8_t const *buf : buffer to start calculate crc.
  *
  * @param  uint32_t len : buffer length in byte.
  *
  * @return None
  */
uint16_t crc16_be(uint16_t crc, uint8_t const *buf, uint32_t len);

/**
  * @brief CRC8 value that is in little endian.
  *
  * @param  uint8_t crc : init crc value, use 0 at the first use.
  *
  * @param  uint8_t const *buf : buffer to start calculate crc.
  *
  * @param  uint32_t len : buffer length in byte.
  *
  * @return None
  */
uint8_t crc8_le(uint8_t crc, uint8_t const *buf, uint32_t len);

/**
  * @brief CRC8 value that is in big endian.
  *
  * @param  uint32_t crc : init crc value, use 0 at the first use.
  *
  * @param  uint8_t const *buf : buffer to start calculate crc.
  *
  * @param  uint32_t len : buffer length in byte.
  *
  * @return None
  */
uint8_t crc8_be(uint8_t crc, uint8_t const *buf, uint32_t len);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif


#endif

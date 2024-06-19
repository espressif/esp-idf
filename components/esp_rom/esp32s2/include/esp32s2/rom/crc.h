/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ROM_CRC_H
#define ROM_CRC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup crc_apis, uart configuration and communication related apis
  * @brief crc apis
  */

/** @addtogroup crc_apis
  * @{
  */


/* Standard CRC8/16/32 algorithms. */
// CRC-8        x8+x2+x1+1              0x07
// CRC16-CCITT  x16+x12+x5+1   1021   ISO HDLC, ITU X.25, V.34/V.41/V.42, PPP-FCS
// CRC32:
//G(x) = x32 +x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x1 + 1
//If your buf is not continuous, you can use the first result to be the second parameter.

/**
  * @brief Crc32 value that is in little endian.
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
  * @brief Crc16 value that is in little endian.
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
  * @brief Crc8 value that is in little endian.
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
  * @}
  */

#ifdef __cplusplus
}
#endif


#endif

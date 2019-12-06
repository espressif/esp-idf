// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "sdkconfig.h"

#if defined(CONFIG_IDF_TARGET_ESP32)
#include "esp32/rom/crc.h"
#endif

/******************* Polynomials Used in the CRC APIs ****************************
* CRC-8        x8+x2+x1+1                                              0x07
* CRC16-CCITT  x16+x12+x5+1                                            0x1021
* CRC32        x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x1+1     0x04c11db7
********************************************************************************/

/**
* @brief CRC32 value in little endian.
*
* @param crc: Initial CRC value (result of last calculation or 0 for the first time)
* @param buf: Data buffer that used to calculate the CRC value
* @param len: Length of the data buffer
* @return CRC32 value
*/
static inline uint32_t esp_crc32_le(uint32_t crc, uint8_t const *buf, uint32_t len)
{
    return crc32_le(crc, buf, len);
}

/**
* @brief CRC32 value in big endian.
*
* @param crc: Initial CRC value (result of last calculation or 0 for the first time)
* @param buf: Data buffer that used to calculate the CRC value
* @param len: Length of the data buffer
* @return CRC32 value
*/
static inline uint32_t esp_crc32_be(uint32_t crc, uint8_t const *buf, uint32_t len)
{
    return crc32_be(crc, buf, len);
}

/**
* @brief CRC16 value in little endian.
*
* @param crc: Initial CRC value (result of last calculation or 0 for the first time)
* @param buf: Data buffer that used to calculate the CRC value
* @param len: Length of the data buffer
* @return CRC16 value
*/
static inline uint16_t esp_crc16_le(uint16_t crc, uint8_t const *buf, uint32_t len)
{
    return crc16_le(crc, buf, len);
}

/**
* @brief CRC16 value in big endian.
*
* @param crc: Initial CRC value (result of last calculation or 0 for the first time)
* @param buf: Data buffer that used to calculate the CRC value
* @param len: Length of the data buffer
* @return CRC16 value
*/
static inline uint16_t esp_crc16_be(uint16_t crc, uint8_t const *buf, uint32_t len)
{
    return crc16_be(crc, buf, len);
}

/**
* @brief CRC8 value in little endian.
*
* @param crc: Initial CRC value (result of last calculation or 0 for the first time)
* @param buf: Data buffer that used to calculate the CRC value
* @param len: Length of the data buffer
* @return CRC8 value
*/
static inline uint8_t esp_crc8_le(uint8_t crc, uint8_t const *buf, uint32_t len)
{
    return crc8_le(crc, buf, len);
}

/**
* @brief CRC8 value in big endian.
*
* @param crc: Initial CRC value (result of last calculation or 0 for the first time)
* @param buf: Data buffer that used to calculate the CRC value
* @param len: Length of the data buffer
* @return CRC8 value
*/
static inline uint8_t esp_crc8_be(uint8_t crc, uint8_t const *buf, uint32_t len)
{
    return crc8_be(crc, buf, len);
}

#ifdef __cplusplus
}
#endif

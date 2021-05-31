/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @brief Return CRC7 of data, in the format used by SD protocol
 * @param data array of data used to compute CRC
 * @param size size of data in bytes
 * @return CRC7 value
 */
uint8_t sdspi_crc7(const uint8_t *data, size_t size);

/**
 * @brief Return CRC16 of data, in the format used by SD protocol
 * @param data array of data used to compute CRC
 * @param size size of data in bytes
 * @return CRC16 value
 */
uint16_t sdspi_crc16(const uint8_t* data, size_t size);


#ifdef __cplusplus
}
#endif

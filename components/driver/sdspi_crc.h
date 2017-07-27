// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

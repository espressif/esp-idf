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

#ifdef __cplusplus
extern "C" {
#endif

/* Standard CRC8/16/32 algorithms. */
uint32_t crc32_le(uint32_t crc, uint8_t const * buf, uint32_t len);
uint32_t crc32_be(uint32_t crc, uint8_t const * buf, uint32_t len);
uint16_t crc16_le(uint16_t crc, uint8_t const * buf, uint32_t len);
uint16_t crc16_be(uint16_t crc, uint8_t const * buf, uint32_t len);
uint8_t crc8_le(uint8_t crc, uint8_t const * buf, uint32_t len);
uint8_t crc8_be(uint8_t crc, uint8_t const * buf, uint32_t len);

#ifdef __cplusplus
}
#endif


#endif

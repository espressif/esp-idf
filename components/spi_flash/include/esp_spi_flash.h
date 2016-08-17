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

#ifndef ESP_SPI_FLASH_H
#define ESP_SPI_FLASH_H

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_FLASH_BASE       0x10010
#define ESP_ERR_FLASH_OP_FAIL    (ESP_ERR_FLASH_BASE + 1)
#define ESP_ERR_FLASH_OP_TIMEOUT (ESP_ERR_FLASH_BASE + 2)

#define SPI_FLASH_SEC_SIZE  4096    /**< SPI Flash sector size */

/**
 * @brief  Erase the Flash sector.
 *
 * @param  uint16 sec : Sector number, the count starts at sector 0, 4KB per sector.
 *
 * @return esp_err_t
 */
esp_err_t spi_flash_erase_sector(uint16_t sec);

/**
 * @brief  Write data to Flash.
 *
 * @param  uint32 des_addr  : destination address in Flash.
 * @param  uint32 *src_addr : source address of the data.
 * @param  uint32 size      : length of data
 *
 * @return esp_err_t
 */
esp_err_t spi_flash_write(uint32_t des_addr, const uint32_t *src_addr, uint32_t size);

/**
 * @brief  Read data from Flash.
 *
 * @param  uint32 src_addr  : source address of the data in Flash.
 * @param  uint32 *des_addr : destination address.
 * @param  uint32 size      : length of data
 *
 * @return esp_err_t
 */
esp_err_t spi_flash_read(uint32_t src_addr, uint32_t *des_addr, uint32_t size);


#ifdef __cplusplus
}
#endif


#endif /* ESP_SPI_FLASH_H */


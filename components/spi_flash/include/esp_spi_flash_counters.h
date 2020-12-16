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

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "sdkconfig.h"

#if CONFIG_SPI_FLASH_ENABLE_COUNTERS

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure holding statistics for one type of operation
 */
typedef struct {
    uint32_t count;     // number of times operation was executed
    uint32_t time;      // total time taken, in microseconds
    uint32_t bytes;     // total number of bytes
} spi_flash_counter_t;

typedef struct {
    spi_flash_counter_t read;
    spi_flash_counter_t write;
    spi_flash_counter_t erase;
} spi_flash_counters_t;

/**
 * @brief  Reset SPI flash operation counters
 */
void spi_flash_reset_counters(void);

/**
 * @brief  Print SPI flash operation counters
 */
void spi_flash_dump_counters(void);

/**
 * @brief  Return current SPI flash operation counters
 *
 * @return  pointer to the spi_flash_counters_t structure holding values
 *          of the operation counters
 */
const spi_flash_counters_t* spi_flash_get_counters(void);

#ifdef __cplusplus
}
#endif

#endif //CONFIG_SPI_FLASH_ENABLE_COUNTERS

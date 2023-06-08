/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "sdkconfig.h"

#if CONFIG_SPI_FLASH_ENABLE_COUNTERS || defined __DOXYGEN__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure holding statistics for one type of operation
 */
typedef struct {
    uint32_t count;     /*!< number of times operation was executed */
    uint32_t time;      /*!< total time taken, in microseconds */
    uint32_t bytes;     /*!< total number of bytes */
} esp_flash_counter_t;

/**
 * Structure for counters of flash actions
*/
typedef struct {
    esp_flash_counter_t read;   /*!< counters for read action, like `esp_flash_read`*/
    esp_flash_counter_t write;  /*!< counters for write action, like `esp_flash_write`*/
    esp_flash_counter_t erase;  /*!< counters for erase action, like `esp_flash_erase`*/
} esp_flash_counters_t;

// for deprecate old api
typedef esp_flash_counter_t   spi_flash_counter_t;
typedef esp_flash_counters_t  spi_flash_counters_t;

/**
 * @brief  Reset SPI flash operation counters
 */
void esp_flash_reset_counters(void);
void spi_flash_reset_counters(void) __attribute__((deprecated("Please use 'esp_flash_reset_counters' instead")));

/**
 * @brief  Print SPI flash operation counters
 */
void esp_flash_dump_counters(FILE* stream);
void spi_flash_dump_counters(void) __attribute__((deprecated("Please use 'esp_flash_dump_counters' instead")));

/**
 * @brief  Return current SPI flash operation counters
 *
 * @return  pointer to the esp_flash_counters_t structure holding values
 *          of the operation counters
 */
const esp_flash_counters_t* esp_flash_get_counters(void);
const spi_flash_counters_t* spi_flash_get_counters(void) __attribute__((deprecated("Please use 'esp_flash_get_counters' instead")));

#ifdef __cplusplus
}
#endif

#endif //CONFIG_SPI_FLASH_ENABLE_COUNTERS || defined __DOXYGEN__

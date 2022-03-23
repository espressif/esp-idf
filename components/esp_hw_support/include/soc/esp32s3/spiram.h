/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize spiram interface/hardware. Normally called from cpu_start.c.
 *
 * @return ESP_OK on success
 */
esp_err_t esp_spiram_init(void);

/**
 * @brief Add the initialized SPI RAM to the heap allocator.
 */
esp_err_t esp_spiram_add_to_heapalloc(void);

/**
 * @brief Force a writeback of the data in the SPI RAM cache. This is to be called whenever
 * cache is disabled, because disabling cache on the ESP32 discards the data in the SPI
 * RAM cache.
 *
 * This is meant for use from within the SPI flash code.
 */
void esp_spiram_writeback_cache(void);

/**
 * @brief If SPI RAM(PSRAM) has been initialized
 *
 * @return
 *          - true SPI RAM has been initialized successfully
 *          - false SPI RAM hasn't been initialized or initialized failed
 */
bool esp_spiram_is_initialized(void);

/**
 * @brief get psram CS IO
 *
 * This interface should be called after PSRAM is enabled, otherwise it will
 * return an invalid value -1/0xff.
 *
 * @return psram CS IO or -1/0xff if psram not enabled
 */
uint8_t esp_spiram_get_cs_io(void);

/**
 * @brief Reserve a pool of internal memory for specific DMA/internal allocations
 *
 * @param size Size of reserved pool in bytes
 *
 * @return
 *          - ESP_OK on success
 *          - ESP_ERR_NO_MEM when no memory available for pool
 */
esp_err_t esp_spiram_reserve_dma_pool(size_t size);

/**
 * @brief If SPI RAM(PSRAM) has been initialized
 *
 * @return
 *          - true SPI RAM has been initialized successfully
 *          - false SPI RAM hasn't been initialized or initialized failed
 */
bool esp_spiram_is_initialized(void);

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS

extern int _instruction_reserved_start, _instruction_reserved_end;

/**
 * @brief Get the start page number of the instruction in SPI flash
 *
 * @return start page number
 */
uint32_t instruction_flash_start_page_get(void);
/**
 * @brief Get the end page number of the instruction in SPI flash
 *
 * @return end page number
 */
uint32_t instruction_flash_end_page_get(void);
/**
 * @brief Get the offset of instruction from SPI flash to SPI RAM
 *
 * @return instruction offset
 */
int instruction_flash2spiram_offset(void);
#endif

#if CONFIG_SPIRAM_RODATA

extern int _rodata_reserved_start, _rodata_reserved_end;

/**
 * @brief Get the start page number of the rodata in SPI flash
 *
 * @return start page number
 */
uint32_t rodata_flash_start_page_get(void);
/**
 * @brief Get the end page number of the rodata in SPI flash
 *
 * @return end page number
 */
uint32_t rodata_flash_end_page_get(void);
/**
 * @brief Get the offset number of rodata from SPI flash to SPI RAM
 *
 * @return rodata offset
 */
int rodata_flash2spiram_offset(void);
#endif

#ifdef __cplusplus
}
#endif

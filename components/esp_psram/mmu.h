/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This file will be moved out of `esp_psram` component. And will be
 * future MMU driver, to maintain all the external memory contexts including:
 * - Flash
 * - PSRAM
 * - DDR
 *
 * Now only support ESP32, ESP32S2, ESP32S3 virtual address maintenance, and is internal
 */


#define MMU_MEM_CAP_EXEC        (1<<0)
#define MMU_MEM_CAP_READ        (1<<1)
#define MMU_MEM_CAP_WRITE       (1<<2)
#define MMU_MEM_CAP_32BIT       (1<<3)
#define MMU_MEM_CAP_8BIT        (1<<4)

/**
 * @brief Initialise the MMU driver
 *
 * This is called once in the IDF startup code. Don't call it in applications
 */
void esp_mmu_init(void);

/**
 * @brief Get largest consecutive free external virtual memory block, with given capabilities
 *
 * @param[in] caps      Bitwise OR of MMU_MEM_CAP_* flags indicating the memory block
 * @param[out] out_len  Largest free block length, in bytes.
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments, could be null pointer
 */
esp_err_t esp_mmu_get_largest_free_block(int caps, size_t *out_len);

/**
 * @brief Find a consecutive external virtual memory range, with given capabilities and size
 *
 * @param[in] size      Size, in bytes, the amount of memory to find
 * @param[in] caps      Bitwise OR of MMU_MEM_CAP_* flags indicating the memory block
 * @param[out] out_ptr  Pointer to the memory range found
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments, could be wrong caps makeup, or null pointer
 *        - ESP_ERR_NOT_FOUND:   Didn't find enough memory with give caps
 */
esp_err_t esp_mmu_find_vaddr_range(size_t size, uint32_t caps, const void **out_ptr);

/**
 * @brief Dump internal memory region usage
 *
 * @return
 *        - ESP_OK: On success
 */
esp_err_t esp_mmu_dump_region_usage(void);

#ifdef __cplusplus
}
#endif

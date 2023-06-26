/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "esp_err.h"
#include "hal/mmu_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Memory Mapping Private APIs for MMU supported memory
 */

/**
 * @brief Initialise the MMU MMAP driver
 *
 * This is called once in the IDF startup code. Don't call it in applications
 */
void esp_mmu_map_init(void);

/**
 * @brief Reserve a consecutive external virtual memory block, with given capabilities and size
 *
 * @note  This private API shall be only called internally during startup stage. DO NOT call
 *        this API in your applications
 *
 * @param[in] size      Size, in bytes, the amount of memory to find
 * @param[in] caps      Bitwise OR of `mmu_mem_caps_t` flags indicating the memory block capability
 * @param[in] target    Target memory type. See `mmu_target_t`
 * @param[out] out_ptr  Pointer to start address of the memory block that is reserved
 *
 * @return
 *        - ESP_OK:              On success
 *        - ESP_ERR_INVALID_ARG: Invalid arguments, could be wrong caps makeup, or null pointer
 *        - ESP_ERR_NOT_FOUND:   Didn't find enough memory with give caps
 */
esp_err_t esp_mmu_map_reserve_block_with_caps(size_t size, mmu_mem_caps_t caps, mmu_target_t target, const void **out_ptr);

/*
 * @brief Dump all mapped blocks
 *
 * @return
 *        - ESP_OK
 */
esp_err_t esp_mmu_map_dump_mapped_blocks_private(void);


#ifdef __cplusplus
}
#endif

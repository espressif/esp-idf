/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "multi_heap.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Adjust size, alignment and caps of a memory allocation request to the specific
 *        hardware requirements, dependent on where the memory gets allocated.
 *
 * @note Note that heap_caps_base.c has its own definition for this function in order not to depend
 *       on this component.
 *
 * @param[in,out] p_alignment Pointer to alignment requirements. This may be modified upwards if the
 *                            hardware has stricter alignment requirements.
 * @param[in,out] p_size      Pointer to size of memory to be allocated. This may be modified upwards
 *                            if e.g. the memory needs to be aligned to a cache line.
 * @param[in,out] p_caps      Pointer to memory requirements. This may be adjusted if the memory
 *                            requirements need modification for the heap caps allocator to work
 *                            properly.
 */
void esp_heap_adjust_alignment_to_hw(size_t *p_alignment, size_t *p_size, uint32_t *p_caps);

#ifdef __cplusplus
}
#endif

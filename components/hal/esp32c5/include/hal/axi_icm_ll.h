/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include "hal/assert.h"
#include "soc/hp_system_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set arbitration priority for GDMA master port
 *
 * @param priority Arbitration priority for GDMA master port
 */
static inline void axi_icm_ll_set_gdma_arbiter_prio(uint8_t priority)
{
    HAL_ASSERT(priority < 2); // Ensure priority is within valid range (0-1)
    HP_SYSTEM.axi_mst_pri.dma_priority = priority;
}

/**
 * @brief Set arbitration priority for Cache master port
 *
 * @param priority Arbitration priority for Cache master port
 */
static inline void axi_icm_ll_set_cache_arbiter_prio(uint8_t priority)
{
    HAL_ASSERT(priority < 2); // Ensure priority is within valid range (0-1)
    HP_SYSTEM.axi_mst_pri.cache_priority = priority;
}

#ifdef __cplusplus
}
#endif

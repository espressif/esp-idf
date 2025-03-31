/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/hp_system_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the burst buffer for L2 memory
 *
 * @note During AHB burst access to the L2MEM, in order to reduce the AHB request to the L2MEM arbiter,
 *       enabling the buffer mode can improve the performance.
 * @note This function must be called before any AHB burst access to the L2MEM.
 */
static inline void l2mem_ll_enable_ahb_burst_buffer(bool en_read, bool en_write)
{
    HP_SYSTEM.l2_mem_ahb_buffer_ctrl.l2_mem_ahb_rdbuffer_en = en_read;
    HP_SYSTEM.l2_mem_ahb_buffer_ctrl.l2_mem_ahb_wrbuffer_en = en_write;
}

#ifdef __cplusplus
}
#endif

/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "soc/pcr_struct.h"
#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
#include "hal/gdma_beta3_ll.h"
#define GDMA_LL_AHB_RX_BURST_NEEDS_ALIGNMENT  1
#else
#include "hal/ahb_dma_ll.h"
#define GDMA_LL_AHB_BURST_SIZE_ADJUSTABLE 1 // AHB GDMA supports adjustable burst size
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the bus clock for the DMA module
 */
static inline void gdma_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    PCR.gdma_conf.gdma_clk_en = enable;
}

/**
 * @brief Reset the DMA module
 */
static inline void gdma_ll_reset_register(int group_id)
{
    (void)group_id;
    PCR.gdma_conf.gdma_rst_en = 1;
    PCR.gdma_conf.gdma_rst_en = 0;
}

#ifdef __cplusplus
}
#endif

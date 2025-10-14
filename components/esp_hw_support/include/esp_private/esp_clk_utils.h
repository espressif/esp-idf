/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "soc/rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
/**
 * @brief Sync MSPI speed mode before CPU frequency switching, only needed when frequency is decreasing.
 *
 * @param target_cpu_src_freq   Target clock source frequency for CPU frequency switching
 * @param target_cpu_freq       CPU frequency switching target frequency
 */
void esp_clk_utils_mspi_speed_mode_sync_before_cpu_freq_switching(uint32_t target_cpu_src_freq, uint32_t target_cpu_freq);

/**
 * @brief Sync MSPI speed mode after CPU frequency switching, only needed when frequency is upcreasing.
 *
 * @param target_cpu_src_freq Target clock source frequency for CPU frequency switching
 * @param target_cpu_freq     CPU frequency switching target frequency
 */
void esp_clk_utils_mspi_speed_mode_sync_after_cpu_freq_switching(uint32_t target_cpu_src_freq, uint32_t target_cpu_freq);
#endif

#ifdef __cplusplus
}
#endif

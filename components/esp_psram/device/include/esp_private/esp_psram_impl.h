/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PSRAM_SIZE_2MB                  (2 * 1024 * 1024)
#define PSRAM_SIZE_4MB                  (4 * 1024 * 1024)
#define PSRAM_SIZE_8MB                  (8 * 1024 * 1024)
#define PSRAM_SIZE_16MB                 (16 * 1024 * 1024)
#define PSRAM_SIZE_32MB                 (32 * 1024 * 1024)
#define PSRAM_SIZE_64MB                 (64 * 1024 * 1024)

#if CONFIG_PM_SLP_SPIRAM_HALFSLEEP_ENABLED
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#define PSRAM_HALFSLEEP_DATA_ATTR                   TCM_DRAM_ATTR
#define PSRAM_HALFSLEEP_SLEEP_CODE_ATTR             TCM_IRAM_ATTR
#else
#define PSRAM_HALFSLEEP_DATA_ATTR                   DRAM_ATTR
#define PSRAM_HALFSLEEP_SLEEP_CODE_ATTR             IRAM_ATTR
#endif
#define PSRAM_HALFSLEEP_RESUME_CODE_ATTR            IRAM_ATTR
#else
#define PSRAM_HALFSLEEP_DATA_ATTR
#define PSRAM_HALFSLEEP_SLEEP_CODE_ATTR
#define PSRAM_HALFSLEEP_RESUME_CODE_ATTR
#endif

/**
 * @brief To get the physical psram size in bytes.
 *
 * @param[out] out_size_bytes    physical psram size in bytes.
 */
esp_err_t esp_psram_impl_get_physical_size(uint32_t *out_size_bytes);

/**
 * @brief To get the available physical psram size in bytes.
 *
 * @param[out] out_size_bytes    available physical psram size in bytes.
 */
esp_err_t esp_psram_impl_get_available_size(uint32_t *out_size_bytes);

/**
 * @brief Enable psram and configure it to a ready state
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_NOT_SUPPORTED: PSRAM ID / vendor ID check fail
 *        - ESP_ERR_INVALID_STATE: On esp32, when VSPI peripheral is needed but cannot be claimed
 */
esp_err_t esp_psram_impl_enable(void);

/**
 * @brief get psram CS IO
 *
 * @return psram CS IO
 */
uint8_t esp_psram_impl_get_cs_io(void);

/**
 * @brief Set psram to halfsleep mode to save power consumption
 *
 * It will also set refresh rate to 0.5x.
 */
void esp_psram_impl_enter_halfsleep_mode(void);

/**
 * @brief Make psram exit halfsleep mode (no wait, wait is done in esp_psram_impl_resume_from_halfsleep_mode)
 */
void esp_psram_impl_exit_halfsleep_mode(void);

/**
 * @brief Resume psram from halfsleep mode (also restore refresh rate)
 *
 * @param slowclk_period RTC slow clock period, calibration value obtained using rtc_clk_cal
 *
 * @note PSRAM requires some time (tHSPU) to fully recover from halfsleep mode.
 *       This function examines the time between exiting halfsleep mode and current time, and busy wait if the time is not long enough.
 *       This shortens the overall busy loop time if wait is done inside esp_psram_impl_exit_halfsleep_mode.
 */
void esp_psram_impl_resume_from_halfsleep_mode(uint32_t slowclk_period);

#ifdef __cplusplus
}
#endif

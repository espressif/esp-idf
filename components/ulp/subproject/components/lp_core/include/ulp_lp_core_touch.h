/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read raw data of touch sensor on the LP core
 *
 * @param[in]  touch_num    Touch pad index
 * @param[out] raw_data     Raw data buffer pointer to accept touch sensor raw value,
 *                          buffer size should be equal to the number of enabled sampling frequencies
 * @return esp_err_t        ESP_OK when successful
 */
esp_err_t lp_core_touch_pad_read_raw_data(int touch_num, uint32_t *raw_data);

/**
 * @brief Read benchmark of touch sensor on the LP core
 *
 * @param[in]  touch_num    Touch pad index
 * @param[out] benchmark    Benchmark data buffer pointer to accept touch sensor benchmark value,
 *                          buffer size should be equal to the number of enabled sampling frequencies
 * @return esp_err_t        ESP_OK when successful
 */
esp_err_t lp_core_touch_pad_read_benchmark(int touch_num, uint32_t *benchmark);

/**
 * @brief Read the filtered (smoothened) touch sensor data on the LP core
 *
 * @param[in]  touch_num    Touch pad index
 * @param[out] smooth_data  Smooth data buffer pointer to accept touch sensor smooth value,
 *                          buffer size should be equal to the number of enabled sampling frequencies
 * @return esp_err_t        ESP_OK when successful
 */
esp_err_t lp_core_touch_pad_filter_read_smooth(int touch_num, uint32_t *smooth_data);

/**
 * @brief Force reset benchmark to raw data of touch sensor.
 *
 * @param[in]  touch_num    Touch pad index
 * @param[in]  mask         Mask of the sample freuqencies that need to be reset
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t lp_core_touch_pad_reset_benchmark(int touch_num, uint32_t mask);

/**
 * @brief Read raw data of touch sensor sleep channel on the LP core
 *
 * @param[in]  touch_num    Touch pad index that has been registered as sleep channel
 * @param[out] raw_data     Raw data buffer pointer to accept touch sensor raw value,
 *                          buffer size should be equal to the number of enabled sampling frequencies
 * @return esp_err_t        ESP_OK when successful
 */
esp_err_t lp_core_touch_pad_sleep_channel_read_data(int touch_num, uint32_t *raw_data);

/**
 * @brief Read benchmark of touch sensor sleep channel on the LP core
 *
 * @param[in]  touch_num    Touch pad index that has been registered as sleep channel
 * @param[out] benchmark    Benchmark data buffer pointer to accept touch sensor benchmark value,
 *                          buffer size should be equal to the number of enabled sampling frequencies
 * @return esp_err_t        ESP_OK when successful
 */
esp_err_t lp_core_touch_pad_sleep_channel_read_benchmark(int touch_num, uint32_t *benchmark);

/**
 * @brief Read the filtered (smoothened) touch sensor sleep channel data on the LP core
 *
 * @param[in]  touch_num    Touch pad index that has been registered as sleep channel
 * @param[out] smooth_dat   Smooth data buffer pointer to accept touch sensor smooth value,
 *                          buffer size should be equal to the number of enabled sampling frequencies
 * @return esp_err_t        ESP_OK when successful
 */
esp_err_t lp_core_touch_pad_sleep_channel_read_smooth(int touch_num, uint32_t *smooth_data);

/**
 * @brief Reset benchmark of touch sensor sleep channel.
 * @param[in]  mask         Mask of the sample freuqencies that need to be reset
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t lp_core_touch_pad_sleep_channel_reset_benchmark(uint32_t mask);

#ifdef __cplusplus
}
#endif

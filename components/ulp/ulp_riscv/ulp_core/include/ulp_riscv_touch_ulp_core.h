/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "ulp_riscv_register_ops.h"
#include "hal/touch_sensor_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read raw data of touch sensor on the ULP RISC-V core
 * @note Refer `touch_pad_read_raw_data()` for more details
 *
 * @param touch_num     Touch pad index
 * @param raw_data      Pointer to accept touch sensor value
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t ulp_riscv_touch_pad_read_raw_data(touch_pad_t touch_num, uint32_t *raw_data);

/**
 * @brief Read benchmark of touch sensor on the ULP RISC-V core
 * @note Refer `touch_pad_read_benchmark()` for more details
 *
 * @param touch_num     Touch pad index
 * @param benchmark     Pointer to accept touch sensor benchmark value
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t ulp_riscv_touch_pad_read_benchmark(touch_pad_t touch_num, uint32_t *benchmark);

/**
 * @brief Read the filtered (smoothened) touch sensor data on the ULP RISC-V core
 * @note Refer `touch_pad_filter_read_smooth()` for more details
 *
 * @param touch_num     Touch pad index
 * @param smooth_data   Pointer to accept smoothened touch sensor value
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t ulp_riscv_touch_pad_filter_read_smooth(touch_pad_t touch_num, uint32_t *smooth_data);

/**
 * @brief Force reset benchmark to raw data of touch sensor.
 * @note Refer `touch_pad_reset_benchmark()` for more details
 *
 * @param touch_num     Touch pad index (TOUCH_PAD_MAX resets basaline of all channels)
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t ulp_riscv_touch_pad_reset_benchmark(touch_pad_t touch_num);

/**
 * @brief Read raw data of touch sensor sleep channel on the ULP RISC-V core
 * @note Refer `touch_pad_sleep_channel_read_data()` for more details
 *
 * @param touch_num     Touch pad index (Only one touch sensor channel is supported in deep sleep)
 * @param raw_data      Pointer to accept touch sensor value
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t ulp_riscv_touch_pad_sleep_channel_read_data(touch_pad_t touch_num, uint32_t *raw_data);

/**
 * @brief Read benchmark of touch sensor sleep channel on the ULP RISC-V core
 * @note Refer `touch_pad_sleep_channel_read_benchmark()` for more details
 *
 * @param touch_num     Touch pad index (Only one touch sensor channel is supported in deep sleep)
 * @param benchmark     Pointer to accept touch sensor benchmark value
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t ulp_riscv_touch_pad_sleep_channel_read_benchmark(touch_pad_t touch_num, uint32_t *benchmark);

/**
 * @brief Read the filtered (smoothened) touch sensor sleep channel data on the ULP RISC-V core
 * @note Refer `touch_pad_sleep_channel_read_smooth()` for more details
 *
 * @param touch_num     Touch pad index (Only one touch sensor channel is supported in deep sleep)
 * @param smooth_data   Pointer to accept smoothened touch sensor value
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t ulp_riscv_touch_pad_sleep_channel_read_smooth(touch_pad_t touch_num, uint32_t *smooth_data);


/**
 * @brief Reset benchmark of touch sensor sleep channel.
 * @note Refer `touch_pad_sleep_channel_reset_benchmark()` for more details
 *
 * @return esp_err_t    ESP_OK when successful
 */
esp_err_t ulp_riscv_touch_pad_sleep_channel_reset_benchmark(void);

#ifdef __cplusplus
}
#endif

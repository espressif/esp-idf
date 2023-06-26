/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ulp_riscv_touch_ulp_core.h"
#include "soc/soc_caps.h"
#include "soc/touch_sensor_pins.h"
#include "hal/touch_sensor_hal.h"

/* Check Touch Channel correctness */
#define ULP_RISCV_TOUCH_CHANNEL_CHECK_AND_RETURN(channel)   \
{                                                           \
    if (channel >= SOC_TOUCH_SENSOR_NUM ||                  \
        channel < 0 ||                                      \
        channel == SOC_TOUCH_DENOISE_CHANNEL) {             \
            return ESP_ERR_INVALID_ARG;                     \
    }                                                       \
}                                                           \

esp_err_t ulp_riscv_touch_pad_read_raw_data(touch_pad_t touch_num, uint32_t *raw_data)
{
    /* Check Arguments */
    if (!raw_data) {
        return ESP_ERR_INVALID_ARG;
    }
    ULP_RISCV_TOUCH_CHANNEL_CHECK_AND_RETURN(touch_num);

    /* Read raw touch data */
    *raw_data = touch_hal_read_raw_data(touch_num);

    return ESP_OK;
}

esp_err_t ulp_riscv_touch_pad_read_benchmark(touch_pad_t touch_num, uint32_t *benchmark)
{
    /* Check Arguments */
    if (!benchmark) {
        return ESP_ERR_INVALID_ARG;
    }
    ULP_RISCV_TOUCH_CHANNEL_CHECK_AND_RETURN(touch_num);

    /* Read benchmark data */
    touch_hal_read_benchmark(touch_num, benchmark);

    return ESP_OK;
}

esp_err_t ulp_riscv_touch_pad_filter_read_smooth(touch_pad_t touch_num, uint32_t *smooth_data)
{
    /* Check Arguments */
    if (!smooth_data) {
        return ESP_ERR_INVALID_ARG;
    }
    ULP_RISCV_TOUCH_CHANNEL_CHECK_AND_RETURN(touch_num);

    /* Read smoothened touch sensor data */
    touch_hal_filter_read_smooth(touch_num, smooth_data);

    return ESP_OK;
}

esp_err_t ulp_riscv_touch_pad_reset_benchmark(touch_pad_t touch_num)
{
    /* Check Arguments */
    if (touch_num > TOUCH_PAD_MAX || touch_num < 0) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Reset benchmark */
    touch_hal_reset_benchmark(touch_num);

    return ESP_OK;
}

esp_err_t ulp_riscv_touch_pad_sleep_channel_read_data(touch_pad_t touch_num, uint32_t *raw_data)
{
    /* Check Arguments */
    if (!raw_data) {
        return ESP_ERR_INVALID_ARG;
    }
    ULP_RISCV_TOUCH_CHANNEL_CHECK_AND_RETURN(touch_num);

    /* Read raw touch data */
    touch_hal_sleep_read_data(raw_data);

    return ESP_OK;
}

esp_err_t ulp_riscv_touch_pad_sleep_channel_read_benchmark(touch_pad_t touch_num, uint32_t *benchmark)
{
    /* Check Arguments */
    if (!benchmark) {
        return ESP_ERR_INVALID_ARG;
    }
    ULP_RISCV_TOUCH_CHANNEL_CHECK_AND_RETURN(touch_num);

    /* Read benchmark data */
    touch_hal_sleep_read_benchmark(benchmark);

    return ESP_OK;
}

esp_err_t ulp_riscv_touch_pad_sleep_channel_read_smooth(touch_pad_t touch_num, uint32_t *smooth_data)
{
    /* Check Arguments */
    if (!smooth_data) {
        return ESP_ERR_INVALID_ARG;
    }
    ULP_RISCV_TOUCH_CHANNEL_CHECK_AND_RETURN(touch_num);

    /* Read smoothened touch sensor data */
    touch_hal_sleep_read_smooth(smooth_data);

    return ESP_OK;
}

esp_err_t ulp_riscv_touch_pad_sleep_channel_reset_benchmark(void)
{
    /* Reset benchmark */
    touch_hal_sleep_reset_benchmark();

    return ESP_OK;
}

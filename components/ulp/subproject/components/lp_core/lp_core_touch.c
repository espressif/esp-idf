/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "hal/touch_sensor_ll.h"
#include "esp_err.h"

/* Check Touch Channel correctness */
#define LP_CORE_TOUCH_CHANNEL_CHECK_AND_RETURN(channel)   \
{                                                           \
    if (channel >= SOC_TOUCH_MAX_CHAN_ID ||                 \
        channel < SOC_TOUCH_MIN_CHAN_ID) {                  \
            return ESP_ERR_INVALID_ARG;                     \
    }                                                       \
}                                                           \

esp_err_t lp_core_touch_pad_read_raw_data(int touch_num, uint32_t *raw_data)
{
    /* Check Arguments */
    if (!raw_data) {
        return ESP_ERR_INVALID_ARG;
    }
    LP_CORE_TOUCH_CHANNEL_CHECK_AND_RETURN(touch_num);

    /* Read raw touch data */
    uint32_t engaged_sampler_cnt = touch_ll_sample_cfg_get_engaged_num();
    for (int i = 0; i < engaged_sampler_cnt; i++) {
        // Only have smooth data on V3, no raw data
        touch_ll_read_chan_data((int)touch_num, i + 1, TOUCH_LL_READ_SMOOTH, &raw_data[i]);
    }

    return ESP_OK;
}

esp_err_t lp_core_touch_pad_read_benchmark(int touch_num, uint32_t *benchmark)
{
    /* Check Arguments */
    if (!benchmark) {
        return ESP_ERR_INVALID_ARG;
    }
    LP_CORE_TOUCH_CHANNEL_CHECK_AND_RETURN(touch_num);

    /* Read benchmark data */
    uint32_t engaged_sampler_cnt = touch_ll_sample_cfg_get_engaged_num();
    for (int i = 0; i < engaged_sampler_cnt; i++) {
        touch_ll_read_chan_data((int)touch_num, i + 1, TOUCH_LL_READ_BENCHMARK, &benchmark[i]);
    }

    return ESP_OK;
}

esp_err_t lp_core_touch_pad_filter_read_smooth(int touch_num, uint32_t *smooth_data)
{
    /* Check Arguments */
    if (!smooth_data) {
        return ESP_ERR_INVALID_ARG;
    }
    LP_CORE_TOUCH_CHANNEL_CHECK_AND_RETURN(touch_num);

    /* Read smoothened touch sensor data */
    uint32_t engaged_sampler_cnt = touch_ll_sample_cfg_get_engaged_num();
    for (int i = 0; i < engaged_sampler_cnt; i++) {
        touch_ll_read_chan_data((int)touch_num, i + 1, TOUCH_LL_READ_SMOOTH, &smooth_data[i]);
    }

    return ESP_OK;
}

esp_err_t lp_core_touch_pad_reset_benchmark(int touch_num, uint32_t mask)
{
    (void) mask;  // Currently not support, reserved for future use
    /* Check Arguments */
    LP_CORE_TOUCH_CHANNEL_CHECK_AND_RETURN(touch_num);

    /* Reset benchmark */
    touch_ll_reset_chan_benchmark(BIT(touch_num));

    return ESP_OK;
}

esp_err_t lp_core_touch_pad_sleep_channel_read_data(int touch_num, uint32_t *raw_data)
{
    /* Check Arguments */
    if (!raw_data) {
        return ESP_ERR_INVALID_ARG;
    }
    LP_CORE_TOUCH_CHANNEL_CHECK_AND_RETURN(touch_num);

    /* Read raw touch data */
    uint32_t engaged_sampler_cnt = touch_ll_sample_cfg_get_engaged_num();
    for (int i = 0; i < engaged_sampler_cnt; i++) {
        // Only have smooth data on V3, no raw data
        touch_ll_sleep_read_chan_data(TOUCH_LL_READ_SMOOTH, i + 1, &raw_data[i]);
    }

    return ESP_OK;
}

esp_err_t lp_core_touch_pad_sleep_channel_read_benchmark(int touch_num, uint32_t *benchmark)
{
    /* Check Arguments */
    if (!benchmark) {
        return ESP_ERR_INVALID_ARG;
    }
    LP_CORE_TOUCH_CHANNEL_CHECK_AND_RETURN(touch_num);

    /* Read benchmark data */
    uint32_t engaged_sampler_cnt = touch_ll_sample_cfg_get_engaged_num();
    for (int i = 0; i < engaged_sampler_cnt; i++) {
        touch_ll_sleep_read_chan_data(TOUCH_LL_READ_BENCHMARK, i + 1, &benchmark[i]);
    }

    return ESP_OK;
}

esp_err_t lp_core_touch_pad_sleep_channel_read_smooth(int touch_num, uint32_t *smooth_data)
{
    /* Check Arguments */
    if (!smooth_data) {
        return ESP_ERR_INVALID_ARG;
    }
    LP_CORE_TOUCH_CHANNEL_CHECK_AND_RETURN(touch_num);

    /* Read smoothened touch sensor data */
    uint32_t engaged_sampler_cnt = touch_ll_sample_cfg_get_engaged_num();
    for (int i = 0; i < engaged_sampler_cnt; i++) {
        touch_ll_sleep_read_chan_data(TOUCH_LL_READ_SMOOTH, i + 1, &smooth_data[i]);
    }

    return ESP_OK;
}

esp_err_t lp_core_touch_pad_sleep_channel_reset_benchmark(uint32_t mask)
{
    (void) mask;  // Currently not support, reserved for future use
    /* Reset benchmark */
    touch_ll_sleep_reset_benchmark();

    return ESP_OK;
}

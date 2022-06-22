/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "hal/sdm_types.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of Sigma Delta channel handle
 */
typedef struct sdm_channel_t *sdm_channel_handle_t;

/**
 * @brief Sigma Delta channel configuration
 */
typedef struct {
    int gpio_num;               /*!< GPIO number */
    sdm_clock_source_t clk_src; /*!< Clock source */
    uint32_t sample_rate_hz;    /*!< Sample rate in Hz, it determines how frequent the modulator outputs a pulse */
    struct {
        uint32_t invert_out: 1;   /*!< Whether to invert the output signal */
        uint32_t io_loop_back: 1; /*!< For debug/test, the signal output from the GPIO will be fed to the input path as well */
    } flags;                      /*!< Extra flags */
} sdm_config_t;

/**
 * @brief Create a new Sigma Delta channel
 *
 * @param[in] config SDM configuration
 * @param[out] ret_chan Returned SDM channel handle
 * @return
 *      - ESP_OK: Create SDM channel successfully
 *      - ESP_ERR_INVALID_ARG: Create SDM channel failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create SDM channel failed because out of memory
 *      - ESP_ERR_NOT_FOUND: Create SDM channel failed because all channels are used up and no more free one
 *      - ESP_FAIL: Create SDM channel failed because of other error
 */
esp_err_t sdm_new_channel(const sdm_config_t *config, sdm_channel_handle_t *ret_chan);

/**
 * @brief Delete the Sigma Delta channel
 *
 * @param[in] chan SDM channel created by `sdm_new_channel`
 * @return
 *      - ESP_OK: Delete the SDM channel successfully
 *      - ESP_ERR_INVALID_ARG: Delete the SDM channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Delete the SDM channel failed because the channel is not in init state
 *      - ESP_FAIL: Delete the SDM channel failed because of other error
 */
esp_err_t sdm_del_channel(sdm_channel_handle_t chan);

/**
 * @brief Enable the Sigma Delta channel
 *
 * @note This function will transit the channel state from init to enable.
 * @note This function will acquire a PM lock, if a specific source clock (e.g. APB) is selected in the `sdm_config_t`, while `CONFIG_PM_ENABLE` is enabled.
 *
 * @param[in] chan SDM channel created by `sdm_new_channel`
 * @return
 *      - ESP_OK: Enable SDM channel successfully
 *      - ESP_ERR_INVALID_ARG: Enable SDM channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Enable SDM channel failed because the channel is already enabled
 *      - ESP_FAIL: Enable SDM channel failed because of other error
 */
esp_err_t sdm_channel_enable(sdm_channel_handle_t chan);

/**
 * @brief Disable the Sigma Delta channel
 *
 * @note This function will do the opposite work to the `sdm_channel_enable()`
 *
 * @param[in] chan SDM channel created by `sdm_new_channel`
 * @return
 *      - ESP_OK: Disable SDM channel successfully
 *      - ESP_ERR_INVALID_ARG: Disable SDM channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Disable SDM channel failed because the channel is not enabled yet
 *      - ESP_FAIL: Disable SDM channel failed because of other error
 */
esp_err_t sdm_channel_disable(sdm_channel_handle_t chan);

/**
 * @brief Set the duty cycle of the PDM output signal.
 *
 * @note For PDM signals, duty cycle refers to the percentage of high level cycles to the whole statistical period.
 *       The average output voltage could be Vout = VDD_IO / 256 * duty + VDD_IO / 2
 * @note If the duty is set to zero, the output signal is like a 50% duty cycle square wave, with a frequency around (sample_rate_hz / 4).
 * @note The duty is proportional to the equivalent output voltage after a low-pass-filter.
 * @note This function is allowed to run within ISR context
 * @note This function will be placed into IRAM if `CONFIG_SDM_CTRL_FUNC_IN_IRAM` is on, so that it's allowed to be executed when Cache is disabled
 *
 * @param[in] chan SDM channel created by `sdm_new_channel`
 * @param[in] duty Equivalent duty cycle of the PDM output signal, ranges from -128 to 127. But the range of [-90, 90] can provide a better randomness.
 * @return
 *      - ESP_OK: Set duty cycle successfully
 *      - ESP_ERR_INVALID_ARG: Set duty cycle failed because of invalid argument
 *      - ESP_FAIL: Set duty cycle failed because of other error
 */
esp_err_t sdm_channel_set_duty(sdm_channel_handle_t chan, int8_t duty);

#ifdef __cplusplus
}
#endif

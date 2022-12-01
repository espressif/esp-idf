/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function is used to enable the digital RC_FAST clock,
 *        to support the peripherals.
 *
 * @note If this function is called a number of times, the `periph_rtc_dig_clk8m_disable`
 *       function needs to be called same times to disable.
 *
 * @return true: success for enable the RC_FAST clock, false: RC_FAST clock enable failed
 */
bool periph_rtc_dig_clk8m_enable(void);

/**
 * @brief This function is used to disable the digital RC_FAST clock, which should be called
 *        with the `periph_rtc_dig_clk8m_enable` pairedly
 *
 * @note If this function is called a number of times, the `periph_rtc_dig_clk8m_disable`
 *       function needs to be called same times to disable.
 */
void periph_rtc_dig_clk8m_disable(void);

/**
 * @brief This function is used to get the real clock frequency value of RC_FAST clock
 *
 * @return The real clock value, in Hz
 */
uint32_t periph_rtc_dig_clk8m_get_freq(void);

#if SOC_CLK_APLL_SUPPORTED
/**
 * @brief Enable APLL power if it has not enabled
 */
void periph_rtc_apll_acquire(void);

/**
 * @brief Shut down APLL power if no peripherals using APLL
 */
void periph_rtc_apll_release(void);

/**
 * @brief Calculate and set APLL coefficients by given frequency
 * @note  Have to call 'periph_rtc_apll_acquire' to enable APLL power before setting frequency
 * @note  This calculation is based on the inequality:
 *        xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536) >= SOC_APLL_MULTIPLIER_OUT_MIN_HZ(350 MHz)
 *        It will always calculate the minimum coefficients that can satisfy the inequality above, instead of loop them one by one.
 *        which means more appropriate coefficients are likely to exist.
 *        But this algorithm can meet almost all the cases and the accuracy can be guaranteed as well.
 * @note  The APLL frequency is only allowed to set when there is only one peripheral refer to it.
 *        If APLL is already set by another peripheral, this function will return `ESP_ERR_INVALID_STATE`
 *        and output the current frequency by parameter `real_freq`.
 *
 * @param expt_freq Expected APLL frequency (unit: Hz)
 * @param real_freq APLL real working frequency [output] (unit: Hz)
 * @return
 *      - ESP_OK: APLL frequency set success
 *      - ESP_ERR_INVALID_ARG: The input expt_freq is out of APLL support range
 *      - ESP_ERR_INVALID_STATE: APLL is refered by more than one peripherals, not allowed to change its frequency now
 */
esp_err_t periph_rtc_apll_freq_set(uint32_t expt_freq, uint32_t *real_freq);
#endif // SOC_CLK_APLL_SUPPORTED

#ifdef __cplusplus
}
#endif

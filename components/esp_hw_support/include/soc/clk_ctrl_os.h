/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/rtc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function is used to enable the digital 8m rtc clock,
 *        to support the peripherals.
 *
 * @note If this function is called a number of times, the `periph_rtc_dig_clk8m_disable`
 *       function needs to be called same times to disable.
 *
 * @return true: success for enable the rtc 8M clock, false: rtc 8M clock enable failed
 */
bool periph_rtc_dig_clk8m_enable(void);

/**
 * @brief This function is used to disable the rtc digital clock, which should be called
 *        with the `periph_rtc_dig_clk8m_enable` pairedly
 *
 * @note If this function is called a number of times, the `periph_rtc_dig_clk8m_disable`
 *       function needs to be called same times to disable.
 */
void periph_rtc_dig_clk8m_disable(void);

/**
 * @brief This function is used to get the real clock frequency value of the rtc clock
 *
 * @return The real clock value
 */
uint32_t periph_rtc_dig_clk8m_get_freq(void);

#ifdef __cplusplus
}
#endif

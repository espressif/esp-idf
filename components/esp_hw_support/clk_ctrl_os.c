/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <freertos/FreeRTOS.h>
#include "clk_ctrl_os.h"
#include "esp_check.h"
#include "sdkconfig.h"

static portMUX_TYPE periph_spinlock = portMUX_INITIALIZER_UNLOCKED;

static uint8_t s_periph_ref_counts = 0;
static uint32_t s_rtc_clk_freq = 0; // Frequency of the 8M/256 clock in Hz
#if SOC_CLK_APLL_SUPPORTED
static const char *TAG = "clk_ctrl_os";
// Current APLL frequency, in HZ. Zero if APLL is not enabled.
static uint32_t s_cur_apll_freq = 0;
static int s_apll_ref_cnt = 0;
#endif


bool periph_rtc_dig_clk8m_enable(void)
{
    portENTER_CRITICAL(&periph_spinlock);
    if (s_periph_ref_counts == 0) {
        rtc_dig_clk8m_enable();
#if !CONFIG_IDF_TARGET_ESP32H2
        s_rtc_clk_freq = rtc_clk_freq_cal(rtc_clk_cal(RTC_CAL_8MD256, 100));
        if (s_rtc_clk_freq == 0) {
            portEXIT_CRITICAL(&periph_spinlock);
            return false;
        }
#endif
    }
    s_periph_ref_counts++;
    portEXIT_CRITICAL(&periph_spinlock);
    return true;
}

uint32_t periph_rtc_dig_clk8m_get_freq(void)
{
#if CONFIG_IDF_TARGET_ESP32H2
    /* Workaround: H2 doesn't have 8MD256 clk, so calibration cannot be done, we just return its theoretic frequency */
    return SOC_CLK_RC_FAST_FREQ_APPROX;
#else
    return s_rtc_clk_freq * 256;
#endif
}

void periph_rtc_dig_clk8m_disable(void)
{
    portENTER_CRITICAL(&periph_spinlock);
    assert(s_periph_ref_counts > 0);
    s_periph_ref_counts--;
    if (s_periph_ref_counts == 0) {
        s_rtc_clk_freq = 0;
        rtc_dig_clk8m_disable();
    }
    portEXIT_CRITICAL(&periph_spinlock);
}

#if SOC_CLK_APLL_SUPPORTED
void periph_rtc_apll_acquire(void)
{
    portENTER_CRITICAL(&periph_spinlock);
    s_apll_ref_cnt++;
    if (s_apll_ref_cnt == 1) {
        // For the first time enable APLL, need to set power up
        rtc_clk_apll_enable(true);
    }
    portEXIT_CRITICAL(&periph_spinlock);
}

void periph_rtc_apll_release(void)
{
    portENTER_CRITICAL(&periph_spinlock);
    assert(s_apll_ref_cnt > 0);
    s_apll_ref_cnt--;
    if (s_apll_ref_cnt == 0) {
        // If there is no peripheral using APLL, shut down the power
        s_cur_apll_freq = 0;
        rtc_clk_apll_enable(false);
    }
    portEXIT_CRITICAL(&periph_spinlock);
}

esp_err_t periph_rtc_apll_freq_set(uint32_t expt_freq, uint32_t *real_freq)
{
    uint32_t o_div = 0;
    uint32_t sdm0 = 0;
    uint32_t sdm1 = 0;
    uint32_t sdm2 = 0;
    // Guarantee 'periph_rtc_apll_acquire' has been called before set apll freq
    assert(s_apll_ref_cnt > 0);
    uint32_t apll_freq = rtc_clk_apll_coeff_calc(expt_freq, &o_div, &sdm0, &sdm1, &sdm2);

    ESP_RETURN_ON_FALSE(apll_freq, ESP_ERR_INVALID_ARG, TAG, "APLL coefficients calculate failed");
    bool need_config = true;
    portENTER_CRITICAL(&periph_spinlock);
    /* If APLL is not in use or only one peripheral in use, its frequency can be changed as will
     * But when more than one peripheral refers APLL, its frequency is not allowed to change once it is set */
    if (s_cur_apll_freq == 0 || s_apll_ref_cnt < 2) {
        s_cur_apll_freq = apll_freq;
    } else {
        apll_freq = s_cur_apll_freq;
        need_config = false;
    }
    portEXIT_CRITICAL(&periph_spinlock);
    *real_freq = apll_freq;

    if (need_config) {
        ESP_LOGD(TAG, "APLL will working at %d Hz with coefficients [sdm0] %d [sdm1] %d [sdm2] %d [o_div] %d",
                       apll_freq, sdm0, sdm1, sdm2, o_div);
        /* Set coefficients for APLL, notice that it doesn't mean APLL will start */
        rtc_clk_apll_coeff_set(o_div, sdm0, sdm1, sdm2);
    } else {
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}
#endif // SOC_CLK_APLL_SUPPORTED

/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <freertos/FreeRTOS.h>
#include "clk_ctrl_os.h"
#include "soc/rtc.h"
#include "esp_private/esp_clk_tree_common.h"
#include "esp_check.h"
#if SOC_CLK_MPLL_SUPPORTED
#include "rtc_clk.h"
#include "esp_private/esp_ldo_psram.h"
#endif

#if SOC_CLK_APLL_SUPPORTED || SOC_CLK_MPLL_SUPPORTED
static const char *TAG = "clk_ctrl_os";
#endif

static portMUX_TYPE periph_spinlock = portMUX_INITIALIZER_UNLOCKED;

static uint8_t s_periph_ref_counts = 0;
static uint32_t s_rc_fast_freq = 0; // Frequency of the RC_FAST clock in Hz
#if SOC_CLK_APLL_SUPPORTED
// Current APLL frequency, in HZ. Zero if APLL is not enabled.
static uint32_t s_cur_apll_freq = 0;
static int s_apll_ref_cnt = 0;
#endif
#if SOC_CLK_MPLL_SUPPORTED
static uint32_t s_cur_mpll_freq = 0;
static int s_mpll_ref_cnt = 0;
static esp_ldo_unit_handle_t s_ldo_unit_hndl = NULL;
#endif

bool periph_rtc_dig_clk8m_enable(void)
{
    portENTER_CRITICAL(&periph_spinlock);
    if (s_periph_ref_counts == 0) {
        rtc_dig_clk8m_enable();
#if SOC_CLK_RC_FAST_SUPPORT_CALIBRATION
        s_rc_fast_freq = esp_clk_tree_rc_fast_get_freq_hz(ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT);
        if (s_rc_fast_freq == 0) {
            rtc_dig_clk8m_disable();
            portEXIT_CRITICAL(&periph_spinlock);
            return false;
        }
#endif //SOC_CLK_RC_FAST_SUPPORT_CALIBRATION
    }
    s_periph_ref_counts++;
    portEXIT_CRITICAL(&periph_spinlock);
    return true;
}

uint32_t periph_rtc_dig_clk8m_get_freq(void)
{
#if !SOC_CLK_RC_FAST_SUPPORT_CALIBRATION
    /* Workaround: CLK8M calibration cannot be performed, we can only return its theoretic value */
    return SOC_CLK_RC_FAST_FREQ_APPROX;
#else
    return s_rc_fast_freq;
#endif
}

void periph_rtc_dig_clk8m_disable(void)
{
    portENTER_CRITICAL(&periph_spinlock);
    assert(s_periph_ref_counts > 0);
    s_periph_ref_counts--;
    if (s_periph_ref_counts == 0) {
        s_rc_fast_freq = 0;
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
        ESP_LOGD(TAG, "APLL will working at %"PRIu32" Hz with coefficients [sdm0] %"PRIu32" [sdm1] %"PRIu32" [sdm2] %"PRIu32" [o_div] %"PRIu32"",
                       apll_freq, sdm0, sdm1, sdm2, o_div);
        /* Set coefficients for APLL, notice that it doesn't mean APLL will start */
        rtc_clk_apll_coeff_set(o_div, sdm0, sdm1, sdm2);
    } else {
        return ESP_ERR_INVALID_STATE;
    }

    return ESP_OK;
}
#endif // SOC_CLK_APLL_SUPPORTED

#if SOC_CLK_MPLL_SUPPORTED
void periph_rtc_mpll_early_acquire(void)
{
    portENTER_CRITICAL(&periph_spinlock);
    s_mpll_ref_cnt++;
    if (s_mpll_ref_cnt == 1) {
#if SOC_PSRAM_VDD_POWER_MPLL
        // configure MPPL power (MPLL power pin is the same as for the PSRAM)
        s_ldo_unit_hndl = esp_ldo_vdd_psram_early_init();
#endif
        // For the first time enable MPLL, need to set power up
        rtc_clk_mpll_enable();
    }
    portEXIT_CRITICAL(&periph_spinlock);
}

esp_err_t periph_rtc_mpll_acquire(void)
{
    esp_err_t ret = ESP_OK;
    portENTER_CRITICAL(&periph_spinlock);
    s_mpll_ref_cnt++;
    if (s_mpll_ref_cnt == 1) {
#if SOC_PSRAM_VDD_POWER_MPLL
        // configure MPPL power (MPLL power pin is the same as for the PSRAM)
        ret = esp_ldo_vdd_psram_init(&s_ldo_unit_hndl);
        // external power supply in use is a valid condition
        if (ret == ESP_ERR_INVALID_STATE) {
            ret = ESP_OK;
        } else if (ret != ESP_OK ) {
            portEXIT_CRITICAL(&periph_spinlock);
            ESP_LOGE(TAG, "failed to initialize PSRAM internal LDO");
            goto err;
        }
#endif
        // For the first time enable MPLL, need to set power up
        rtc_clk_mpll_enable();
    }
    portEXIT_CRITICAL(&periph_spinlock);
#if SOC_PSRAM_VDD_POWER_MPLL
err:
#endif
    return ret;
}

void periph_rtc_mpll_release(void)
{
    portENTER_CRITICAL(&periph_spinlock);
    assert(s_mpll_ref_cnt > 0);
    s_mpll_ref_cnt--;
    if (s_mpll_ref_cnt == 0) {
        // If there is no peripheral using MPLL, shut down the power
        s_cur_mpll_freq = 0;
        rtc_clk_mpll_disable();
#if SOC_PSRAM_VDD_POWER_MPLL
        if (s_ldo_unit_hndl) {
            esp_ldo_vdd_psram_deinit(s_ldo_unit_hndl);
        }
#endif
    }
    portEXIT_CRITICAL(&periph_spinlock);
}

esp_err_t periph_rtc_mpll_freq_set(uint32_t expt_freq, uint32_t *real_freq)
{
    esp_err_t ret = ESP_OK;

    // Guarantee 'periph_rtc_apll_acquire' has been called before set apll freq
    assert(s_mpll_ref_cnt > 0);

    portENTER_CRITICAL(&periph_spinlock);
    if (s_cur_mpll_freq == expt_freq) {
        goto end;
    }
    /* If MPLL is not in use or only one peripheral in use, its frequency can be changed as will
     * But when more than one peripheral refers MPLL, its frequency is not allowed to change once it is set */
    if (s_cur_mpll_freq == 0 || s_mpll_ref_cnt < 2) {
        uint32_t xtal_freq = 0;
        ESP_ERROR_CHECK(esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_XTAL, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &xtal_freq));
        rtc_clk_mpll_configure(xtal_freq / MHZ, expt_freq / MHZ);
        ESP_ERROR_CHECK(esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_MPLL, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &s_cur_mpll_freq));
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }
end:
    if (real_freq != NULL) {
        *real_freq = s_cur_mpll_freq;
    }
    portEXIT_CRITICAL(&periph_spinlock);
    return ret;
}
#endif // SOC_CLK_MPLL_SUPPORTED

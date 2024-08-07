/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "hal/lp_i2s_hal.h"
#include "hal/lp_i2s_ll.h"
#include "driver/i2s_types.h"
#include "driver/lp_i2s.h"
#include "driver/lp_i2s_pdm.h"
#include "driver/lp_io.h"
#include "driver/rtc_io.h"
#include "esp_private/periph_ctrl.h"
#include "i2s_private.h"

static const char *TAG = "LP_I2S_PDM";
static esp_err_t s_io_init(lp_i2s_chan_handle_t chan, const lp_i2s_pdm_rx_gpio_config_t *config);

static void s_get_cut_off_coef(uint32_t freq, uint32_t *param0, uint32_t *param5)
{
    uint32_t cut_off_coef[LP_I2S_LL_CUT_OFF_COEF_X_NUM][LP_I2S_LL_CUT_OFF_COEF_Y_NUM] = LP_I2S_LL_CUT_OFF_COEF;
    uint8_t cnt = 0;
    uint32_t min = 10000;
    /* Find the closest cut-off frequency and its coefficients */
    for (int i = 0; i < LP_I2S_LL_CUT_OFF_COEF_X_NUM; i++) {
        uint32_t tmp = cut_off_coef[i][0] < freq ? freq - cut_off_coef[i][0] : cut_off_coef[i][0] - freq;
        if (tmp < min) {
            min = tmp;
            cnt = i;
        }
    }
    *param0 = (uint32_t)cut_off_coef[cnt][1];
    *param5 = (uint32_t)cut_off_coef[cnt][2];
}

esp_err_t lp_i2s_channel_init_pdm_rx_mode(lp_i2s_chan_handle_t handle, const lp_i2s_pdm_rx_config_t *pdm_cfg)
{
    ESP_RETURN_ON_FALSE(handle && pdm_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(handle->state < I2S_CHAN_STATE_RUNNING, ESP_ERR_INVALID_STATE, TAG, "the channel is in enabled state already");

    //Pin config
    ESP_RETURN_ON_ERROR(s_io_init(handle, &pdm_cfg->pin_cfg), TAG, "failed to init IOs");

    //Clock config
    if (handle->role == I2S_ROLE_SLAVE) {
        PERIPH_RCC_ATOMIC() {
            lp_i2s_ll_select_rx_clk_source(handle->ctlr->id, LP_I2S_CLK_SRC_XTAL_D2);
        }
        lp_i2s_ll_clk_source_div_num(handle->ctlr->id, 2);
        lp_i2s_ll_rx_set_raw_clk_div(handle->ctlr->id, 0, 1);
        lp_i2s_ll_rx_set_bck_div_num(handle->ctlr->hal.dev, 1);
    }
    // TODO: make this divisions configurable when support master mode.

    //Slot config
    uint32_t slot_bit_width = (int)pdm_cfg->slot_cfg.slot_bit_width < (int)pdm_cfg->slot_cfg.data_bit_width ?
                              pdm_cfg->slot_cfg.data_bit_width : pdm_cfg->slot_cfg.slot_bit_width;

    lp_i2s_ll_rx_set_sample_bit(handle->ctlr->hal.dev, slot_bit_width, pdm_cfg->slot_cfg.data_bit_width);
    lp_i2s_ll_rx_enable_mono_mode(handle->ctlr->hal.dev, false);

    uint32_t slot_mask = pdm_cfg->slot_cfg.slot_mode == I2S_SLOT_MODE_STEREO ? I2S_PDM_SLOT_BOTH : pdm_cfg->slot_cfg.slot_mask;
    lp_i2s_ll_rx_set_active_chan(handle->ctlr->hal.dev, slot_mask);
    lp_i2s_ll_rx_set_half_sample_bits(handle->ctlr->hal.dev, 16);
    lp_i2s_ll_rx_set_ws_pol(handle->ctlr->hal.dev, pdm_cfg->slot_cfg.ws_pol);

    uint32_t param0 = 0;
    uint32_t param5 = 0;
    s_get_cut_off_coef(pdm_cfg->slot_cfg.hp_cut_off_freq_hz, &param0, &param5);
    lp_i2s_ll_rx_enable_pdm_hp_filter(handle->ctlr->hal.dev, pdm_cfg->slot_cfg.hp_en);
    lp_i2s_ll_rx_set_pdm_hp_filter_param0(handle->ctlr->hal.dev, param0);
    lp_i2s_ll_rx_set_pdm_hp_filter_param5(handle->ctlr->hal.dev, param5);
    /* Set the amplification number, the default and the minimum value is 1. 0 will mute the channel */
    lp_i2s_ll_rx_set_pdm_amplify_num(handle->ctlr->hal.dev, pdm_cfg->slot_cfg.amplify_num ? pdm_cfg->slot_cfg.amplify_num : 1);

    lp_i2s_ll_rx_enable_pdm(handle->ctlr->hal.dev);
    lp_i2s_ll_rx_set_pdm2pcm_en(handle->ctlr->hal.dev, true);

    return ESP_OK;
}

/*---------------------------------------------------------------
                      IO
---------------------------------------------------------------*/
static esp_err_t s_io_init(lp_i2s_chan_handle_t chan, const lp_i2s_pdm_rx_gpio_config_t *config)
{
    if (config->clk >= 0) {
        rtc_gpio_set_direction(config->clk, RTC_GPIO_MODE_INPUT_ONLY);
        rtc_gpio_init(config->clk);
        rtc_gpio_iomux_func_sel(config->clk, 0);
        if (chan->role == I2S_ROLE_MASTER) {
            lp_gpio_connect_in_signal(config->clk, lp_i2s_periph_signal[chan->ctlr->id].m_rx_ws_sig, 0);
        } else {
            lp_gpio_connect_in_signal(config->clk, lp_i2s_periph_signal[chan->ctlr->id].s_rx_ws_sig, 0);
        }
        ESP_LOGD(TAG, "clk io: %d, role: %d, signal: %"PRId8, config->clk, chan->role, chan->role ? lp_i2s_periph_signal[chan->ctlr->id].s_rx_ws_sig : lp_i2s_periph_signal[chan->ctlr->id].m_rx_ws_sig);
    }

    if (config->din >= 0) {
        rtc_gpio_set_direction(config->din, RTC_GPIO_MODE_INPUT_ONLY);
        rtc_gpio_init(config->din);
        rtc_gpio_iomux_func_sel(config->din, 0);
        lp_gpio_connect_in_signal(config->din, lp_i2s_periph_signal[chan->ctlr->id].data_in_sigs[0], 0);
        ESP_LOGD(TAG, "din io: %d, signal: %"PRId8, config->din, lp_i2s_periph_signal[chan->ctlr->id].data_in_sigs[0]);
    }

    return ESP_OK;
}

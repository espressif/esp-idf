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
#include "driver/lp_i2s_std.h"
#include "driver/lp_io.h"
#include "driver/rtc_io.h"
#include "esp_private/periph_ctrl.h"
#include "i2s_private.h"

static const char *TAG = "LP_I2S_STD";
static esp_err_t s_io_init(lp_i2s_chan_handle_t chan, const lp_i2s_std_gpio_config_t *config);

esp_err_t lp_i2s_channel_init_std_mode(lp_i2s_chan_handle_t handle, const lp_i2s_std_config_t *std_cfg)
{
    ESP_RETURN_ON_FALSE(handle && std_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(std_cfg->slot_cfg.data_bit_width <= LP_I2S_LL_TDM_MAX_DATA_BIT_WIDTH && std_cfg->slot_cfg.slot_bit_width <= LP_I2S_LL_TDM_MAX_CHAN_BIT_WIDTH, ESP_ERR_INVALID_ARG, TAG, "invalid argument: wrong data bit width or slot bit width");
    ESP_RETURN_ON_FALSE(handle->state < I2S_CHAN_STATE_RUNNING, ESP_ERR_INVALID_STATE, TAG, "the channel is in enabled state already");

    //Pin config
    ESP_RETURN_ON_ERROR(s_io_init(handle, &std_cfg->pin_cfg), TAG, "failed to init IOs");

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
    uint32_t slot_bit_width = (int)std_cfg->slot_cfg.slot_bit_width < (int)std_cfg->slot_cfg.data_bit_width ?
                              std_cfg->slot_cfg.data_bit_width : std_cfg->slot_cfg.slot_bit_width;

    lp_i2s_ll_rx_set_sample_bit(handle->ctlr->hal.dev, slot_bit_width, std_cfg->slot_cfg.data_bit_width);
    lp_i2s_ll_rx_enable_mono_mode(handle->ctlr->hal.dev, std_cfg->slot_cfg.slot_mode == I2S_SLOT_MODE_MONO);
    lp_i2s_ll_rx_enable_msb_shift(handle->ctlr->hal.dev, std_cfg->slot_cfg.bit_shift);
    lp_i2s_ll_rx_set_ws_width(handle->ctlr->hal.dev, std_cfg->slot_cfg.ws_width);
    lp_i2s_ll_rx_select_std_chan(handle->ctlr->hal.dev, (uint32_t)std_cfg->slot_cfg.slot_mask);
    lp_i2s_ll_rx_set_half_sample_bits(handle->ctlr->hal.dev, slot_bit_width);
    lp_i2s_ll_rx_set_ws_pol(handle->ctlr->hal.dev, std_cfg->slot_cfg.ws_pol);
    lp_i2s_ll_rx_set_bit_order(handle->ctlr->hal.dev, std_cfg->slot_cfg.bit_order_lsb);
    lp_i2s_ll_rx_set_alignment_mode(handle->ctlr->hal.dev, std_cfg->slot_cfg.left_align);
    lp_i2s_ll_rx_set_endian(handle->ctlr->hal.dev, std_cfg->slot_cfg.big_endian);
    lp_i2s_ll_rx_enable_tdm(handle->ctlr->hal.dev);

    return ESP_OK;
}

/*---------------------------------------------------------------
                      IO
---------------------------------------------------------------*/
static esp_err_t s_io_init(lp_i2s_chan_handle_t chan, const lp_i2s_std_gpio_config_t *config)
{
    if (config->bck >= 0) {
        rtc_gpio_set_direction(config->bck, RTC_GPIO_MODE_INPUT_ONLY);
        rtc_gpio_init(config->bck);
        rtc_gpio_iomux_func_sel(config->bck, 0);
        if (chan->role == I2S_ROLE_MASTER) {
            lp_gpio_connect_in_signal(config->bck, lp_i2s_periph_signal[chan->ctlr->id].m_rx_bck_sig, 0);
        } else {
            lp_gpio_connect_in_signal(config->bck, lp_i2s_periph_signal[chan->ctlr->id].s_rx_bck_sig, 0);
        }
        ESP_LOGD(TAG, "bck io: %d, role: %d, signal: %"PRId8, config->bck, chan->role, chan->role ? lp_i2s_periph_signal[chan->ctlr->id].s_rx_bck_sig : lp_i2s_periph_signal[chan->ctlr->id].m_rx_bck_sig);
    }

    if (config->ws >= 0) {
        rtc_gpio_set_direction(config->ws, RTC_GPIO_MODE_INPUT_ONLY);
        rtc_gpio_init(config->ws);
        rtc_gpio_iomux_func_sel(config->ws, 0);
        if (chan->role == I2S_ROLE_MASTER) {
            lp_gpio_connect_in_signal(config->ws, lp_i2s_periph_signal[chan->ctlr->id].m_rx_ws_sig, 0);
        } else {
            lp_gpio_connect_in_signal(config->ws, lp_i2s_periph_signal[chan->ctlr->id].s_rx_ws_sig, 0);
        }
        ESP_LOGD(TAG, "ws io: %d, role: %d, signal: %"PRId8, config->ws, chan->role, chan->role ? lp_i2s_periph_signal[chan->ctlr->id].s_rx_ws_sig : lp_i2s_periph_signal[chan->ctlr->id].m_rx_ws_sig);
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

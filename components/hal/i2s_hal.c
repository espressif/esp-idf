/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


// The HAL layer for I2S (common part)

#include "soc/soc.h"
#include "hal/i2s_hal.h"

#include "hal/i2s_std.h"
#include "hal/i2s_pdm.h"
#include "hal/i2s_tdm.h"

#if SOC_I2S_HW_VERSION_2 && SOC_I2S_SUPPORTS_PDM_TX
/* PDM tx high pass filter cut-off frequency and coeffecients list
 * [0]: cut-off frequency; [1]: param0; [2]: param5 */
static const float cut_off_coef[21][3] = {
    {185, 0, 0}, {172,  0, 1}, {160,  1, 1},
    {150, 1, 2}, {137,  2, 2}, {126,  2, 3},
    {120, 0, 3}, {115,  3, 3}, {106,  1, 7},
    {104, 2, 4}, {92,   4, 4}, {91.5, 2, 7},
    {81,  4, 5}, {77.2, 3, 7}, {69,   5, 5},
    {63,  4, 7}, {58,   5, 6}, {49,   5, 7},
    {46,  6, 6}, {35.5, 6, 7}, {23.3, 7, 7}};
#endif

void i2s_hal_init(i2s_hal_context_t *hal, int port_id)
{
    /* Get hardware instance */
    hal->dev = I2S_LL_GET_HW(port_id);
}

void i2s_hal_set_tx_clock(i2s_hal_context_t *hal, const i2s_clock_info_t *clk_info, i2s_clock_src_t clk_src)
{
#if SOC_I2S_HW_VERSION_2
    i2s_ll_tx_enable_clock(hal->dev);
    i2s_ll_mclk_bind_to_tx_clk(hal->dev);
#endif
    i2s_ll_tx_clk_set_src(hal->dev, clk_src);
    i2s_ll_tx_set_mclk(hal->dev, clk_info->sclk, clk_info->mclk, clk_info->mclk_div);
    i2s_ll_tx_set_bck_div_num(hal->dev, clk_info->bclk_div);
}

void i2s_hal_set_rx_clock(i2s_hal_context_t *hal, const i2s_clock_info_t *clk_info, i2s_clock_src_t clk_src)
{
#if SOC_I2S_HW_VERSION_2
    i2s_ll_rx_enable_clock(hal->dev);
    i2s_ll_mclk_bind_to_rx_clk(hal->dev);
#endif
    i2s_ll_rx_clk_set_src(hal->dev, clk_src);
    i2s_ll_rx_set_mclk(hal->dev, clk_info->sclk, clk_info->mclk, clk_info->mclk_div);
    i2s_ll_rx_set_bck_div_num(hal->dev, clk_info->bclk_div);
}

/*-------------------------------------------------------------------------
 |                    STD Specific Slot Configurations                    |
 -------------------------------------------------------------------------*/
void i2s_hal_std_set_tx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_slot_config_t *slot_config)
{
    i2s_std_slot_config_t *slot_cfg = (i2s_std_slot_config_t*)slot_config;
    uint32_t slot_bit_width = (int)slot_cfg->slot_bit_width < (int)slot_cfg->data_bit_width ?
                              slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    i2s_ll_tx_reset(hal->dev);
    i2s_ll_tx_set_slave_mod(hal->dev, is_slave); //TX Slave
    i2s_ll_tx_set_sample_bit(hal->dev, slot_bit_width, slot_cfg->data_bit_width);
    i2s_ll_tx_enable_mono_mode(hal->dev, slot_cfg->slot_mode == I2S_SLOT_MODE_MONO);
    i2s_ll_tx_enable_msb_shift(hal->dev, slot_cfg->bit_shift);
    i2s_ll_tx_set_ws_width(hal->dev, slot_cfg->ws_width);
#if SOC_I2S_HW_VERSION_1
    i2s_ll_tx_enable_msb_right(hal->dev, slot_cfg->msb_right);
    i2s_ll_tx_enable_right_first(hal->dev, slot_cfg->ws_pol);
    /* Should always enable fifo */
    i2s_ll_tx_force_enable_fifo_mod(hal->dev, true);
#elif SOC_I2S_HW_VERSION_2
    /* There are always 2 slots in std mode */
    i2s_ll_tx_set_chan_num(hal->dev, 2);
    /* In mono mode, there only should be one slot enabled, another inactive slot will transmit same data as enabled slot
     * Otherwise always enable the first two slots */
    i2s_ll_tx_set_active_chan_mask(hal->dev, (slot_cfg->slot_mode == I2S_SLOT_MODE_MONO) ? 0x01 : 0x03);
    i2s_ll_tx_set_half_sample_bit(hal->dev, slot_bit_width);
    i2s_ll_tx_set_ws_idle_pol(hal->dev, slot_cfg->ws_pol);
    i2s_ll_tx_set_bit_order(hal->dev, slot_cfg->bit_order_lsb);
    i2s_ll_tx_enable_left_align(hal->dev, slot_cfg->left_align);
    i2s_ll_tx_enable_big_endian(hal->dev, slot_cfg->big_endian);
#endif
}

void i2s_hal_std_set_rx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_slot_config_t *slot_config)
{
    i2s_std_slot_config_t *slot_cfg = (i2s_std_slot_config_t*)slot_config;
    uint32_t slot_bit_width = (int)slot_cfg->slot_bit_width < (int)slot_cfg->data_bit_width ?
                              slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    i2s_ll_rx_reset(hal->dev);
    i2s_ll_rx_set_slave_mod(hal->dev, is_slave); //RX Slave
    i2s_ll_rx_set_sample_bit(hal->dev, slot_bit_width, slot_cfg->data_bit_width);
    i2s_ll_rx_enable_mono_mode(hal->dev, slot_cfg->slot_mode == I2S_SLOT_MODE_MONO);
    i2s_ll_rx_enable_msb_shift(hal->dev, slot_cfg->bit_shift);
    i2s_ll_rx_set_ws_width(hal->dev, slot_cfg->ws_width);
#if SOC_I2S_HW_VERSION_1
    i2s_ll_rx_enable_msb_right(hal->dev, slot_cfg->msb_right);
    i2s_ll_rx_enable_right_first(hal->dev, slot_cfg->ws_pol);
    /* Should always enable fifo */
    i2s_ll_rx_force_enable_fifo_mod(hal->dev, true);
#elif SOC_I2S_HW_VERSION_2
    /* There are always 2 slots in std mode */
    i2s_ll_rx_set_chan_num(hal->dev, 2);
    /* In mono mode, there only should be one slot enabled, another inactive slot will transmit same data as enabled slot
     * Otherwise always enable the first two slots */
    i2s_ll_rx_set_active_chan_mask(hal->dev, (slot_cfg->slot_mode == I2S_SLOT_MODE_MONO) ? 0x01 : 0x03);
    i2s_ll_rx_set_half_sample_bit(hal->dev, slot_bit_width);
    i2s_ll_rx_set_ws_idle_pol(hal->dev, slot_cfg->ws_pol);
    i2s_ll_rx_set_bit_order(hal->dev, slot_cfg->bit_order_lsb);
    i2s_ll_rx_enable_left_align(hal->dev, slot_cfg->left_align);
    i2s_ll_rx_enable_big_endian(hal->dev, slot_cfg->big_endian);
#endif
}

void i2s_hal_std_enable_tx_channel(i2s_hal_context_t *hal)
{
    i2s_ll_tx_enable_std(hal->dev);
}

void i2s_hal_std_enable_rx_channel(i2s_hal_context_t *hal)
{
    i2s_ll_rx_enable_std(hal->dev);
}

/*-------------------------------------------------------------------------
 |                  PDM Specific Slot Configurations                      |
 -------------------------------------------------------------------------*/
#if SOC_I2S_SUPPORTS_PDM_TX
void i2s_hal_pdm_set_tx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_slot_config_t *slot_config)
{
    i2s_pdm_tx_slot_config_t *slot_cfg = (i2s_pdm_tx_slot_config_t *)slot_config;
    uint32_t slot_bit_width = (int)slot_cfg->slot_bit_width < (int)slot_cfg->data_bit_width ?
                              slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    i2s_ll_tx_reset(hal->dev);
    i2s_ll_tx_set_slave_mod(hal->dev, is_slave); //TX Slave
    i2s_ll_tx_set_sample_bit(hal->dev, slot_bit_width, slot_cfg->data_bit_width);
    i2s_ll_tx_enable_mono_mode(hal->dev, slot_cfg->slot_mode == I2S_SLOT_MODE_MONO);

    i2s_ll_tx_set_pdm_prescale(hal->dev, slot_cfg->sd_prescale);
    i2s_ll_tx_set_pdm_hp_scale(hal->dev, slot_cfg->hp_scale);
    i2s_ll_tx_set_pdm_lp_scale(hal->dev, slot_cfg->lp_scale);
    i2s_ll_tx_set_pdm_sinc_scale(hal->dev, slot_cfg->sinc_scale);
    i2s_ll_tx_set_pdm_sd_scale(hal->dev, slot_cfg->sd_scale);

#if SOC_I2S_HW_VERSION_1
    i2s_ll_tx_force_enable_fifo_mod(hal->dev, true);
#elif SOC_I2S_HW_VERSION_2
    /* Still need to enable the first 2 TDM channel mask to get the correct number of frame */
    i2s_ll_tx_set_active_chan_mask(hal->dev, I2S_TDM_SLOT0 | I2S_TDM_SLOT1);
    i2s_ll_tx_enable_pdm_hp_filter(hal->dev, slot_cfg->hp_en);
    uint8_t cnt = 0;
    float min = 1000;
    float expt_cut_off = slot_cfg->hp_cut_off_freq_hz;
    /* Find the closest cut-off frequency and its coefficients */
    for (int i = 0; i < 21; i++) {
        float tmp = cut_off_coef[i][0] < expt_cut_off ? expt_cut_off - cut_off_coef[i][0] : cut_off_coef[i][0] - expt_cut_off;
        if (tmp < min) {
            min = tmp;
            cnt = i;
        }
    }
    i2s_ll_tx_set_pdm_hp_filter_param0(hal->dev, cut_off_coef[cnt][1]);
    i2s_ll_tx_set_pdm_hp_filter_param5(hal->dev, cut_off_coef[cnt][2]);
    i2s_ll_tx_enable_pdm_sd_codec(hal->dev, slot_cfg->sd_en);
    i2s_ll_tx_set_pdm_sd_dither(hal->dev, slot_cfg->sd_dither);
    i2s_ll_tx_set_pdm_sd_dither2(hal->dev, slot_cfg->sd_dither2);
#endif
}

void i2s_hal_pdm_enable_tx_channel(i2s_hal_context_t *hal)
{
    i2s_ll_tx_enable_pdm(hal->dev);
}
#endif

#if SOC_I2S_SUPPORTS_PDM_RX
void i2s_hal_pdm_set_rx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_slot_config_t *slot_config)
{
    i2s_pdm_rx_slot_config_t *slot_cfg = (i2s_pdm_rx_slot_config_t *)slot_config;
    uint32_t slot_bit_width = (int)slot_cfg->slot_bit_width < (int)slot_cfg->data_bit_width ?
                              slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    i2s_ll_rx_reset(hal->dev);
    i2s_ll_rx_set_slave_mod(hal->dev, is_slave); //RX Slave
    i2s_ll_rx_set_sample_bit(hal->dev, slot_bit_width, slot_cfg->data_bit_width);
    i2s_ll_rx_enable_mono_mode(hal->dev, slot_cfg->slot_mode == I2S_SLOT_MODE_MONO);
#if SOC_I2S_HW_VERSION_1
    i2s_ll_rx_force_enable_fifo_mod(hal->dev, true);
#elif SOC_I2S_HW_VERSION_2
    /* Still need to enable the first 2 TDM channel mask to get the correct number of frame */
    i2s_ll_rx_set_active_chan_mask(hal->dev, I2S_TDM_SLOT0 | I2S_TDM_SLOT1);
#endif
}

void i2s_hal_pdm_enable_rx_channel(i2s_hal_context_t *hal)
{
    i2s_ll_rx_enable_pdm(hal->dev);
}
#endif

/*-------------------------------------------------------------------------
 |                   TDM Specific Slot Configurations                     |
 -------------------------------------------------------------------------*/
#if SOC_I2S_SUPPORTS_TDM
void i2s_hal_tdm_set_tx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_slot_config_t *slot_config)
{
    i2s_tdm_slot_config_t *slot_cfg = (i2s_tdm_slot_config_t*)slot_config;
    uint32_t slot_bit_width = (int)slot_cfg->slot_bit_width < (int)slot_cfg->data_bit_width ?
                              slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    uint32_t cnt;
    uint32_t msk = slot_cfg->slot_mask;
    /* Get the maximum slot number */
    cnt = 32 - __builtin_clz(msk);
    /* There should be at least 2 slots in total even for mono mode */
    cnt = cnt < 2 ? 2 : cnt;
    uint32_t total_slot = slot_cfg->total_slot > cnt ? slot_cfg->total_slot : cnt;
    i2s_ll_tx_reset(hal->dev);
    i2s_ll_tx_set_slave_mod(hal->dev, is_slave); //TX Slave
    i2s_ll_tx_set_sample_bit(hal->dev, slot_bit_width, slot_cfg->data_bit_width);
    i2s_ll_tx_enable_mono_mode(hal->dev, slot_cfg->slot_mode == I2S_SLOT_MODE_MONO);
    i2s_ll_tx_enable_msb_shift(hal->dev, slot_cfg->bit_shift);
    if (slot_cfg->ws_width == I2S_TDM_AUTO_WS_WIDTH) {
        i2s_ll_tx_set_ws_width(hal->dev, (total_slot * slot_bit_width) / 2);
    } else {
        i2s_ll_tx_set_ws_width(hal->dev, slot_cfg->ws_width);
    }

    i2s_ll_tx_set_ws_idle_pol(hal->dev, slot_cfg->ws_pol);
    i2s_ll_tx_set_chan_num(hal->dev, total_slot);
    /* In mono mode, there only should be one slot enabled, other inactive slots will transmit same data as enabled slot */
    i2s_ll_tx_set_active_chan_mask(hal->dev, (slot_cfg->slot_mode == I2S_SLOT_MODE_MONO) ?
                                   I2S_TDM_SLOT0 : (uint32_t)slot_cfg->slot_mask);
    i2s_ll_tx_set_skip_mask(hal->dev, slot_cfg->skip_mask);
    i2s_ll_tx_set_half_sample_bit(hal->dev, total_slot * slot_bit_width / 2);
    i2s_ll_tx_set_bit_order(hal->dev, slot_cfg->bit_order_lsb);
    i2s_ll_tx_enable_left_align(hal->dev, slot_cfg->left_align);
    i2s_ll_tx_enable_big_endian(hal->dev, slot_cfg->big_endian);
}

void i2s_hal_tdm_set_rx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_slot_config_t *slot_config)
{
    i2s_tdm_slot_config_t *slot_cfg = (i2s_tdm_slot_config_t*)slot_config;
    uint32_t slot_bit_width = (int)slot_cfg->slot_bit_width < (int)slot_cfg->data_bit_width ?
                              slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    uint32_t cnt;
    uint32_t msk = slot_cfg->slot_mask;
    for (cnt = 0; msk; cnt++, msk >>= 1);
    /* Get the maximum slot number */
    cnt = 32 - __builtin_clz(msk);
    /* There should be at least 2 slots in total even for mono mode */
    cnt = cnt < 2 ? 2 : cnt;
    uint32_t total_slot = slot_cfg->total_slot > cnt ? slot_cfg->total_slot : cnt;
    i2s_ll_rx_reset(hal->dev);
    i2s_ll_rx_set_slave_mod(hal->dev, is_slave); //RX Slave
    i2s_ll_rx_set_sample_bit(hal->dev, slot_bit_width, slot_cfg->data_bit_width);
    i2s_ll_rx_enable_mono_mode(hal->dev, slot_cfg->slot_mode == I2S_SLOT_MODE_MONO);
    i2s_ll_rx_enable_msb_shift(hal->dev, slot_cfg->bit_shift);
    if (slot_cfg->ws_width == I2S_TDM_AUTO_WS_WIDTH) {
        i2s_ll_rx_set_ws_width(hal->dev, (total_slot * slot_bit_width) / 2);
    } else {
        i2s_ll_rx_set_ws_width(hal->dev, slot_cfg->ws_width);
    }

    i2s_ll_rx_set_ws_idle_pol(hal->dev, slot_cfg->ws_pol);
    i2s_ll_rx_set_chan_num(hal->dev, total_slot);
    /* In mono mode, there only should be one slot enabled, other inactive slots will transmit same data as enabled slot */
    i2s_ll_rx_set_active_chan_mask(hal->dev, (slot_cfg->slot_mode == I2S_SLOT_MODE_MONO) ?
                                   I2S_TDM_SLOT0 : (uint32_t)slot_cfg->slot_mask);
    i2s_ll_rx_set_half_sample_bit(hal->dev, total_slot * slot_bit_width / 2);
    i2s_ll_rx_set_bit_order(hal->dev, slot_cfg->bit_order_lsb);
    i2s_ll_rx_enable_left_align(hal->dev, slot_cfg->left_align);
    i2s_ll_rx_enable_big_endian(hal->dev, slot_cfg->big_endian);
}

void i2s_hal_tdm_enable_tx_channel(i2s_hal_context_t *hal)
{
    i2s_ll_tx_enable_tdm(hal->dev);
}

void i2s_hal_tdm_enable_rx_channel(i2s_hal_context_t *hal)
{
    i2s_ll_rx_enable_tdm(hal->dev);
}
#endif

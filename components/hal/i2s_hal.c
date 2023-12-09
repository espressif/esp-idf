/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


// The HAL layer for I2S (common part)

#include "soc/soc.h"
#include "hal/i2s_hal.h"

#if SOC_I2S_HW_VERSION_2 && (SOC_I2S_SUPPORTS_PDM_TX || SOC_I2S_SUPPORTS_PDM_RX_HP_FILTER)
/* PDM tx high pass filter cut-off frequency and coefficients list
 * [0]: cut-off frequency * 10; [1]: param0; [2]: param5
 * NOTE: the cut-off frequency was timed 10 to use integer type */
static const uint32_t cut_off_coef[21][3] = {
    {1850, 0, 0}, {1720,  0, 1}, {1600,  1, 1},
    {1500, 1, 2}, {1370,  2, 2}, {1260,  2, 3},
    {1200, 0, 3}, {1150,  3, 3}, {1060,  1, 7},
    {1040, 2, 4}, {920,   4, 4}, {915, 2, 7},
    {810,  4, 5}, {772, 3, 7}, {690,   5, 5},
    {630,  4, 7}, {580,   5, 6}, {490,   5, 7},
    {460,  6, 6}, {355, 6, 7}, {233, 7, 7}
};

static void s_i2s_hal_get_cut_off_coef(uint32_t freq, uint32_t *param0, uint32_t *param5)
{
    uint8_t cnt = 0;
    uint32_t min = 10000;
    /* Find the closest cut-off frequency and its coefficients */
    for (int i = 0; i < 21; i++) {
        uint32_t tmp = cut_off_coef[i][0] < freq ? freq - cut_off_coef[i][0] : cut_off_coef[i][0] - freq;
        if (tmp < min) {
            min = tmp;
            cnt = i;
        }
    }
    *param0 = (uint32_t)cut_off_coef[cnt][1];
    *param5 = (uint32_t)cut_off_coef[cnt][2];
}
#endif

/**
 * @brief Calculate the precise mclk division by sclk and mclk
 *
 * @param sclk      system clock
 * @param mclk      module clock
 * @param mclk_div   output the mclk division coefficients
 */
void i2s_hal_calc_mclk_precise_division(uint32_t sclk, uint32_t mclk, hal_utils_clk_div_t *mclk_div)
{
    hal_utils_clk_info_t i2s_clk_info = {
        .src_freq_hz = sclk,
        .exp_freq_hz = mclk,
        .max_integ = I2S_LL_CLK_FRAC_DIV_N_MAX,
        .min_integ = 1,
        .max_fract = I2S_LL_CLK_FRAC_DIV_AB_MAX,
    };
    hal_utils_calc_clk_div_frac_accurate(&i2s_clk_info, mclk_div);
}

void i2s_hal_init(i2s_hal_context_t *hal, int port_id)
{
    /* Get hardware instance */
    hal->dev = I2S_LL_GET_HW(port_id);
}

void _i2s_hal_set_tx_clock(i2s_hal_context_t *hal, const i2s_hal_clock_info_t *clk_info, i2s_clock_src_t clk_src)
{
    if (clk_info) {
        hal_utils_clk_div_t mclk_div = {};
#if SOC_I2S_HW_VERSION_2
        i2s_ll_tx_enable_clock(hal->dev);
        i2s_ll_mclk_bind_to_tx_clk(hal->dev);
#endif
        i2s_ll_tx_clk_set_src(hal->dev, clk_src);
        i2s_hal_calc_mclk_precise_division(clk_info->sclk, clk_info->mclk, &mclk_div);
        i2s_ll_tx_set_mclk(hal->dev, &mclk_div);
        i2s_ll_tx_set_bck_div_num(hal->dev, clk_info->bclk_div);
    } else {
        i2s_ll_tx_clk_set_src(hal->dev, clk_src);
    }
}

void _i2s_hal_set_rx_clock(i2s_hal_context_t *hal, const i2s_hal_clock_info_t *clk_info, i2s_clock_src_t clk_src)
{
    if (clk_info) {
        hal_utils_clk_div_t mclk_div = {};
#if SOC_I2S_HW_VERSION_2
        i2s_ll_rx_enable_clock(hal->dev);
        i2s_ll_mclk_bind_to_rx_clk(hal->dev);
#endif
        i2s_ll_rx_clk_set_src(hal->dev, clk_src);
        i2s_hal_calc_mclk_precise_division(clk_info->sclk, clk_info->mclk, &mclk_div);
        i2s_ll_rx_set_mclk(hal->dev, &mclk_div);
        i2s_ll_rx_set_bck_div_num(hal->dev, clk_info->bclk_div);
    } else {
        i2s_ll_rx_clk_set_src(hal->dev, clk_src);
    }
}

/*-------------------------------------------------------------------------
 |                    STD Specific Slot Configurations                    |
 -------------------------------------------------------------------------*/
void i2s_hal_std_set_tx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_hal_slot_config_t *slot_cfg)
{
    uint32_t slot_bit_width = (int)slot_cfg->slot_bit_width < (int)slot_cfg->data_bit_width ?
                              slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    i2s_ll_tx_reset(hal->dev);
    i2s_ll_tx_set_slave_mod(hal->dev, is_slave); //TX Slave
    i2s_ll_tx_set_sample_bit(hal->dev, slot_bit_width, slot_cfg->data_bit_width);
    i2s_ll_tx_enable_msb_shift(hal->dev, slot_cfg->std.bit_shift);
    i2s_ll_tx_set_ws_width(hal->dev, slot_cfg->std.ws_width);
#if SOC_I2S_HW_VERSION_1
    i2s_ll_tx_enable_mono_mode(hal->dev, slot_cfg->slot_mode == I2S_SLOT_MODE_MONO);
    i2s_ll_tx_select_std_slot(hal->dev, slot_cfg->std.slot_mask, slot_cfg->slot_mode == I2S_SLOT_MODE_MONO);
    // According to the test, the behavior of tx_msb_right is opposite with TRM, TRM is wrong?
    i2s_ll_tx_enable_msb_right(hal->dev, slot_cfg->std.msb_right);
    i2s_ll_tx_enable_right_first(hal->dev, slot_cfg->std.ws_pol);
    /* Should always enable fifo */
    i2s_ll_tx_force_enable_fifo_mod(hal->dev, true);
#elif SOC_I2S_HW_VERSION_2
    bool is_copy_mono = slot_cfg->slot_mode == I2S_SLOT_MODE_MONO && slot_cfg->std.slot_mask == I2S_STD_SLOT_BOTH;
    i2s_ll_tx_enable_mono_mode(hal->dev, is_copy_mono);
    i2s_ll_tx_select_std_slot(hal->dev, is_copy_mono ? I2S_STD_SLOT_LEFT : slot_cfg->std.slot_mask);
    i2s_ll_tx_set_skip_mask(hal->dev, (slot_cfg->std.slot_mask != I2S_STD_SLOT_BOTH) &&
                                      (slot_cfg->slot_mode == I2S_SLOT_MODE_STEREO));
    i2s_ll_tx_set_half_sample_bit(hal->dev, slot_bit_width);
    i2s_ll_tx_set_ws_idle_pol(hal->dev, slot_cfg->std.ws_pol);
    i2s_ll_tx_set_bit_order(hal->dev, slot_cfg->std.bit_order_lsb);
    i2s_ll_tx_enable_left_align(hal->dev, slot_cfg->std.left_align);
    i2s_ll_tx_enable_big_endian(hal->dev, slot_cfg->std.big_endian);
#endif
}

void i2s_hal_std_set_rx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_hal_slot_config_t *slot_cfg)
{
    uint32_t slot_bit_width = (int)slot_cfg->slot_bit_width < (int)slot_cfg->data_bit_width ?
                              slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    i2s_ll_rx_reset(hal->dev);
    i2s_ll_rx_set_slave_mod(hal->dev, is_slave); //RX Slave
    i2s_ll_rx_set_sample_bit(hal->dev, slot_bit_width, slot_cfg->data_bit_width);
    i2s_ll_rx_enable_mono_mode(hal->dev, slot_cfg->slot_mode == I2S_SLOT_MODE_MONO);
    i2s_ll_rx_enable_msb_shift(hal->dev, slot_cfg->std.bit_shift);
    i2s_ll_rx_set_ws_width(hal->dev, slot_cfg->std.ws_width);
#if SOC_I2S_HW_VERSION_1
    i2s_ll_rx_select_std_slot(hal->dev, slot_cfg->std.slot_mask, slot_cfg->std.msb_right);
    i2s_ll_rx_enable_msb_right(hal->dev, slot_cfg->std.msb_right);
    i2s_ll_rx_enable_right_first(hal->dev, slot_cfg->std.ws_pol);
    /* Should always enable fifo */
    i2s_ll_rx_force_enable_fifo_mod(hal->dev, true);
#elif SOC_I2S_HW_VERSION_2
    i2s_ll_rx_select_std_slot(hal->dev, slot_cfg->std.slot_mask);
    i2s_ll_rx_set_half_sample_bit(hal->dev, slot_bit_width);
    i2s_ll_rx_set_ws_idle_pol(hal->dev, slot_cfg->std.ws_pol);
    i2s_ll_rx_set_bit_order(hal->dev, slot_cfg->std.bit_order_lsb);
    i2s_ll_rx_enable_left_align(hal->dev, slot_cfg->std.left_align);
    i2s_ll_rx_enable_big_endian(hal->dev, slot_cfg->std.big_endian);
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
void i2s_hal_pdm_set_tx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_hal_slot_config_t *slot_cfg)
{
    bool is_mono = slot_cfg->slot_mode == I2S_SLOT_MODE_MONO;
    i2s_ll_tx_reset(hal->dev);
    i2s_ll_tx_set_slave_mod(hal->dev, is_slave); //TX Slave
    i2s_ll_tx_enable_msb_shift(hal->dev, false);

    i2s_ll_tx_set_pdm_prescale(hal->dev, slot_cfg->pdm_tx.sd_prescale);
    i2s_ll_tx_set_pdm_hp_scale(hal->dev, slot_cfg->pdm_tx.hp_scale);
    i2s_ll_tx_set_pdm_lp_scale(hal->dev, slot_cfg->pdm_tx.lp_scale);
    i2s_ll_tx_set_pdm_sinc_scale(hal->dev, slot_cfg->pdm_tx.sinc_scale);
    i2s_ll_tx_set_pdm_sd_scale(hal->dev, slot_cfg->pdm_tx.sd_scale);

#if SOC_I2S_HW_VERSION_1
    uint32_t slot_bit_width = (int)slot_cfg->slot_bit_width < (int)slot_cfg->data_bit_width ?
                              slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    i2s_ll_tx_force_enable_fifo_mod(hal->dev, true);
    i2s_ll_tx_set_sample_bit(hal->dev, slot_bit_width, slot_cfg->data_bit_width);
    i2s_ll_tx_enable_mono_mode(hal->dev, is_mono);
    i2s_ll_tx_select_pdm_slot(hal->dev, slot_cfg->pdm_tx.slot_mask & I2S_STD_SLOT_BOTH, is_mono);
    i2s_ll_tx_enable_msb_right(hal->dev, false);
    i2s_ll_tx_enable_right_first(hal->dev, false);
#elif SOC_I2S_HW_VERSION_2
    /* PDM TX line mode */
    i2s_ll_tx_pdm_line_mode(hal->dev, slot_cfg->pdm_tx.line_mode);
    /* Force use 32 bit in PDM TX stereo mode to satisfy the frequency */
    uint32_t slot_bit_width = is_mono ? 16 : 32;
    i2s_ll_tx_set_sample_bit(hal->dev, slot_bit_width, slot_bit_width);
    i2s_ll_tx_set_half_sample_bit(hal->dev, 16); // Fixed to 16 in PDM mode
    /* By default, taking the DMA data at the first half period of WS  */
    i2s_ll_tx_pdm_dma_take_mode(hal->dev, is_mono, true);
    i2s_ll_tx_set_ws_idle_pol(hal->dev, false);
    /* Slot mode seems not take effect according to the test, leave it default here */
    i2s_ll_tx_pdm_slot_mode(hal->dev, is_mono, false, I2S_PDM_SLOT_BOTH);
    uint32_t param0;
    uint32_t param5;
    s_i2s_hal_get_cut_off_coef(slot_cfg->pdm_tx.hp_cut_off_freq_hzx10, &param0, &param5);
    i2s_ll_tx_enable_pdm_hp_filter(hal->dev, slot_cfg->pdm_tx.hp_en);
    i2s_ll_tx_set_pdm_hp_filter_param0(hal->dev, param0);
    i2s_ll_tx_set_pdm_hp_filter_param5(hal->dev, param5);
    i2s_ll_tx_set_pdm_sd_dither(hal->dev, slot_cfg->pdm_tx.sd_dither);
    i2s_ll_tx_set_pdm_sd_dither2(hal->dev, slot_cfg->pdm_tx.sd_dither2);
#endif
}

void i2s_hal_pdm_enable_tx_channel(i2s_hal_context_t *hal)
{
    i2s_ll_tx_enable_pdm(hal->dev);
}
#endif

#if SOC_I2S_SUPPORTS_PDM_RX
void i2s_hal_pdm_set_rx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_hal_slot_config_t *slot_cfg)
{
    uint32_t slot_bit_width = (int)slot_cfg->slot_bit_width < (int)slot_cfg->data_bit_width ?
                              slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    i2s_ll_rx_reset(hal->dev);
    i2s_ll_rx_set_slave_mod(hal->dev, is_slave); //RX Slave
    i2s_ll_rx_set_sample_bit(hal->dev, slot_bit_width, slot_cfg->data_bit_width);
#if SOC_I2S_HW_VERSION_1
    i2s_ll_rx_enable_mono_mode(hal->dev, slot_cfg->slot_mode == I2S_SLOT_MODE_MONO);
    i2s_ll_rx_select_pdm_slot(hal->dev, slot_cfg->pdm_rx.slot_mask);
    i2s_ll_rx_force_enable_fifo_mod(hal->dev, true);
    i2s_ll_rx_enable_msb_right(hal->dev, false);
    i2s_ll_rx_enable_right_first(hal->dev, false);
#elif SOC_I2S_HW_VERSION_2
    i2s_ll_rx_set_half_sample_bit(hal->dev, 16);
    i2s_ll_rx_enable_mono_mode(hal->dev, false);
#if SOC_I2S_PDM_MAX_RX_LINES > 1
    uint32_t slot_mask = (slot_cfg->slot_mode == I2S_SLOT_MODE_STEREO && slot_cfg->pdm_rx.slot_mask <= I2S_PDM_SLOT_BOTH) ?
                          I2S_PDM_SLOT_BOTH : slot_cfg->pdm_rx.slot_mask;
#else
    /* Set the channel mask to enable corresponding slots, always enable two slots for stereo mode */
    uint32_t slot_mask = slot_cfg->slot_mode == I2S_SLOT_MODE_STEREO ? I2S_PDM_SLOT_BOTH : slot_cfg->pdm_rx.slot_mask;
#endif  // SOC_I2S_SUPPORTS_PDM_RX > 1
    i2s_ll_rx_set_active_chan_mask(hal->dev, slot_mask);
#endif  // SOC_I2S_HW_VERSION_1

#if SOC_I2S_SUPPORTS_PDM_RX_HP_FILTER
    uint32_t param0;
    uint32_t param5;
    s_i2s_hal_get_cut_off_coef(slot_cfg->pdm_rx.hp_cut_off_freq_hzx10, &param0, &param5);
    i2s_ll_rx_enable_pdm_hp_filter(hal->dev, slot_cfg->pdm_rx.hp_en);
    i2s_ll_rx_set_pdm_hp_filter_param0(hal->dev, param0);
    i2s_ll_rx_set_pdm_hp_filter_param5(hal->dev, param5);
    /* Set the amplification number, the default and the minimum value is 1. 0 will mute the channel */
    i2s_ll_rx_set_pdm_amplify_num(hal->dev, slot_cfg->pdm_rx.amplify_num ? slot_cfg->pdm_rx.amplify_num : 1);
#endif  // SOC_I2S_SUPPORTS_PDM_RX_HP_FILTER
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
void i2s_hal_tdm_set_tx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_hal_slot_config_t *slot_cfg)
{
    uint32_t slot_bit_width = (int)slot_cfg->slot_bit_width < (int)slot_cfg->data_bit_width ?
                              slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    uint32_t cnt;
    uint32_t msk = slot_cfg->tdm.slot_mask;
    /* Get the maximum slot number */
    cnt = 32 - __builtin_clz(msk);
    /* There should be at least 2 slots in total even for mono mode */
    cnt = cnt < 2 ? 2 : cnt;
    uint32_t total_slot = slot_cfg->tdm.total_slot > cnt ? slot_cfg->tdm.total_slot : cnt;
    i2s_ll_tx_reset(hal->dev);
    i2s_ll_tx_set_slave_mod(hal->dev, is_slave); //TX Slave
    i2s_ll_tx_set_sample_bit(hal->dev, slot_bit_width, slot_cfg->data_bit_width);
    i2s_ll_tx_enable_mono_mode(hal->dev, slot_cfg->slot_mode == I2S_SLOT_MODE_MONO);
    i2s_ll_tx_enable_msb_shift(hal->dev, slot_cfg->tdm.bit_shift);
    if (slot_cfg->tdm.ws_width == 0) { // 0: I2S_TDM_AUTO_WS_WIDTH
        i2s_ll_tx_set_ws_width(hal->dev, (total_slot * slot_bit_width) / 2);
    } else {
        i2s_ll_tx_set_ws_width(hal->dev, slot_cfg->tdm.ws_width);
    }

    i2s_ll_tx_set_ws_idle_pol(hal->dev, slot_cfg->tdm.ws_pol);
    i2s_ll_tx_set_chan_num(hal->dev, total_slot);
    /* In mono mode, there only should be one slot enabled, other inactive slots will transmit same data as enabled slot */
    i2s_ll_tx_set_active_chan_mask(hal->dev, (slot_cfg->slot_mode == I2S_SLOT_MODE_MONO) ?
                                   I2S_TDM_SLOT0 : (uint32_t)slot_cfg->tdm.slot_mask);
    i2s_ll_tx_set_skip_mask(hal->dev, slot_cfg->tdm.skip_mask);
    i2s_ll_tx_set_half_sample_bit(hal->dev, total_slot * slot_bit_width / 2);
    i2s_ll_tx_set_bit_order(hal->dev, slot_cfg->tdm.bit_order_lsb);
    i2s_ll_tx_enable_left_align(hal->dev, slot_cfg->tdm.left_align);
    i2s_ll_tx_enable_big_endian(hal->dev, slot_cfg->tdm.big_endian);
}

void i2s_hal_tdm_set_rx_slot(i2s_hal_context_t *hal, bool is_slave, const i2s_hal_slot_config_t *slot_cfg)
{
    uint32_t slot_bit_width = (int)slot_cfg->slot_bit_width < (int)slot_cfg->data_bit_width ?
                              slot_cfg->data_bit_width : slot_cfg->slot_bit_width;
    uint32_t cnt;
    uint32_t msk = slot_cfg->tdm.slot_mask;
    /* Get the maximum slot number */
    cnt = 32 - __builtin_clz(msk);
    /* There should be at least 2 slots in total even for mono mode */
    cnt = cnt < 2 ? 2 : cnt;
    uint32_t total_slot = slot_cfg->tdm.total_slot > cnt ? slot_cfg->tdm.total_slot : cnt;
    i2s_ll_rx_reset(hal->dev);
    i2s_ll_rx_set_slave_mod(hal->dev, is_slave); //RX Slave
    i2s_ll_rx_set_sample_bit(hal->dev, slot_bit_width, slot_cfg->data_bit_width);
    i2s_ll_rx_enable_mono_mode(hal->dev, slot_cfg->slot_mode == I2S_SLOT_MODE_MONO);
    i2s_ll_rx_enable_msb_shift(hal->dev, slot_cfg->tdm.bit_shift);
    if (slot_cfg->tdm.ws_width == 0) { // 0: I2S_TDM_AUTO_WS_WIDTH
        i2s_ll_rx_set_ws_width(hal->dev, (total_slot * slot_bit_width) / 2);
    } else {
        i2s_ll_rx_set_ws_width(hal->dev, slot_cfg->tdm.ws_width);
    }

    i2s_ll_rx_set_ws_idle_pol(hal->dev, slot_cfg->tdm.ws_pol);
    i2s_ll_rx_set_chan_num(hal->dev, total_slot);
    /* In mono mode, there only should be one slot enabled, other inactive slots will transmit same data as enabled slot */
    i2s_ll_rx_set_active_chan_mask(hal->dev, (slot_cfg->slot_mode == I2S_SLOT_MODE_MONO) ?
                                   I2S_TDM_SLOT0 : (uint32_t)slot_cfg->tdm.slot_mask);
    i2s_ll_rx_set_half_sample_bit(hal->dev, total_slot * slot_bit_width / 2);
    i2s_ll_rx_set_bit_order(hal->dev, slot_cfg->tdm.bit_order_lsb);
    i2s_ll_rx_enable_left_align(hal->dev, slot_cfg->tdm.left_align);
    i2s_ll_rx_enable_big_endian(hal->dev, slot_cfg->tdm.big_endian);
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

/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "soc/soc_caps.h"
#include "driver/i2s_std.h"
#if SOC_I2S_SUPPORTS_TDM
#include "driver/i2s_tdm.h"
#endif
#if SOC_I2S_SUPPORTS_PDM
#include "driver/i2s_pdm.h"
#endif

/**
 * Check that C-style designated initializers are valid in C++ file.
 */
void test_i2s_initializers(void)
{
    i2s_std_clk_config_t  std_clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000);
    i2s_std_slot_config_t std_philips_slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    i2s_std_slot_config_t std_msb_slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    i2s_std_slot_config_t std_pcm_slot_cfg = I2S_STD_PCM_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    (void) std_clk_cfg;
    (void) std_philips_slot_cfg;
    (void) std_msb_slot_cfg;
    (void) std_pcm_slot_cfg;
#if SOC_I2S_SUPPORTS_TDM
    i2s_tdm_clk_config_t  tdm_clk_cfg = I2S_TDM_CLK_DEFAULT_CONFIG(16000);
    i2s_tdm_slot_config_t tdm_philips_slot_cfg = I2S_TDM_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO, (i2s_tdm_slot_mask_t)0x03);
    i2s_tdm_slot_config_t tdm_msb_slot_cfg = I2S_TDM_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO, (i2s_tdm_slot_mask_t)0x03);
    i2s_tdm_slot_config_t tdm_pcm_s_slot_cfg = I2S_TDM_PCM_SHORT_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO, (i2s_tdm_slot_mask_t)0x03);
    i2s_tdm_slot_config_t tdm_pcm_l_slot_cfg = I2S_TDM_PCM_LONG_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO, (i2s_tdm_slot_mask_t)0x03);
    (void) tdm_clk_cfg;
    (void) tdm_philips_slot_cfg;
    (void) tdm_msb_slot_cfg;
    (void) tdm_pcm_s_slot_cfg;
    (void) tdm_pcm_l_slot_cfg;
#endif
#if SOC_I2S_SUPPORTS_PDM_RX
    i2s_pdm_rx_clk_config_t pdm_rx_clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(16000);
    i2s_pdm_rx_slot_config_t pdm_rx_pcm_slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    (void) pdm_rx_clk_cfg;
    (void) pdm_rx_pcm_slot_cfg;
#endif
#if SOC_I2S_SUPPORTS_PDM_TX
    i2s_pdm_tx_clk_config_t pdm_tx_clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(16000);
    i2s_pdm_tx_clk_config_t pdm_tx_dac_clk_cfg = I2S_PDM_TX_CLK_DAC_DEFAULT_CONFIG(16000);
    i2s_pdm_tx_slot_config_t pdm_tx_pcm_slot_cfg = I2S_PDM_TX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    (void) pdm_tx_clk_cfg;
    (void) pdm_tx_dac_clk_cfg;
    (void) pdm_tx_pcm_slot_cfg;
#if SOC_I2S_HW_VERSION_2
    i2s_pdm_tx_slot_config_t pdm_tx_pcm_dac_slot_cfg = I2S_PDM_TX_SLOT_DAC_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    (void) pdm_tx_pcm_dac_slot_cfg;
#endif
#endif
}

// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


// The HAL layer for I2S (common part)

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/gdma_channel.h"
#include "hal/i2s_hal.h"

#define I2S_MODE_I2S (I2S_MODE_MASTER|I2S_MODE_SLAVE|I2S_MODE_TX|I2S_MODE_RX) /*!< I2S normal mode*/

void i2s_hal_reset_tx(i2s_hal_context_t *hal)
{
    i2s_ll_reset_tx(hal->dev);
}

void i2s_hal_reset_rx(i2s_hal_context_t *hal)
{
    i2s_ll_reset_rx(hal->dev);
}

void i2s_hal_reset_tx_fifo(i2s_hal_context_t *hal)
{
    i2s_ll_reset_tx_fifo(hal->dev);
}

void i2s_hal_reset_rx_fifo(i2s_hal_context_t *hal)
{
    i2s_ll_reset_rx_fifo(hal->dev);
}

void i2s_hal_start_tx(i2s_hal_context_t *hal)
{
    i2s_ll_start_tx(hal->dev);
}

void i2s_hal_start_rx(i2s_hal_context_t *hal)
{
    i2s_ll_start_rx(hal->dev);
}

void i2s_hal_stop_tx(i2s_hal_context_t *hal)
{
    i2s_ll_stop_tx(hal->dev);
}

void i2s_hal_stop_rx(i2s_hal_context_t *hal)
{
    i2s_ll_stop_rx(hal->dev);
}

void i2s_hal_set_tx_sample_bit(i2s_hal_context_t *hal, int slot_bit, int data_bit)
{
    i2s_ll_set_tx_sample_bit(hal->dev, slot_bit, data_bit);
}

void i2s_hal_set_rx_sample_bit(i2s_hal_context_t *hal, int slot_bit, int data_bit)
{
    i2s_ll_set_rx_sample_bit(hal->dev, slot_bit, data_bit);
}

void i2s_hal_set_clock_src(i2s_hal_context_t *hal, i2s_clock_src_t sel)
{
    i2s_ll_set_tx_clk_src(hal->dev, sel);
    i2s_ll_set_rx_clk_src(hal->dev, sel);
}

void i2s_hal_tx_clock_config(i2s_hal_context_t *hal, uint32_t sclk, uint32_t fbck, int factor)
{
    i2s_clk_cal_t clk_set = {0};
    i2s_ll_clk_cal(sclk, fbck, factor, &clk_set);
    i2s_ll_set_tx_clk(hal->dev, &clk_set);
}

void i2s_hal_rx_clock_config(i2s_hal_context_t *hal, uint32_t sclk, uint32_t fbck, int factor)
{
    i2s_clk_cal_t clk_set = {0};
    i2s_ll_clk_cal(sclk, fbck, factor, &clk_set);
    i2s_ll_set_rx_clk(hal->dev, &clk_set);
}

void i2s_hal_set_rx_eof_num(i2s_hal_context_t *hal, uint32_t eof_byte)
{
    i2s_ll_set_rx_eof_num(hal->dev, eof_byte);
}

void i2s_hal_enable_master_fd_mode(i2s_hal_context_t *hal)
{
    i2s_ll_set_tx_slave_mod(hal->dev, 0); //TX master
    i2s_ll_set_rx_slave_mod(hal->dev, 1); //RX Slave
}

void i2s_hal_enable_slave_fd_mode(i2s_hal_context_t *hal)
{
    i2s_ll_set_tx_slave_mod(hal->dev, 1); //TX Slave
    i2s_ll_set_rx_slave_mod(hal->dev, 1); //RX Slave
}

#if SOC_I2S_SUPPORTS_PCM
void i2s_hal_tx_pcm_cfg(i2s_hal_context_t *hal, i2s_pcm_cfg_t cfg)
{
    i2s_ll_tx_pcm_cfg(hal->dev, cfg);
}

void i2s_hal_rx_pcm_cfg(i2s_hal_context_t *hal, i2s_pcm_cfg_t cfg)
{
    i2s_ll_rx_pcm_cfg(hal->dev, cfg);
}
#endif

void i2s_hal_enable_sig_loopback(i2s_hal_context_t *hal)
{
    i2s_ll_loop_back_ena(hal->dev, 1);
}

#if SOC_I2S_SUPPORTS_PDM_TX
void i2s_hal_set_tx_pdm_fpfs(i2s_hal_context_t *hal, int fp, int fs)
{
    i2s_ll_set_tx_pdm_fpfs(hal->dev, fp, fs);
}

void i2s_hal_get_tx_pdm_fpfs(i2s_hal_context_t *hal, int *fp, int *fs)
{
    i2s_ll_get_tx_pdm_fpfs(hal->dev, (uint32_t *)fp, (uint32_t *)fs);
}
#endif

#if SOC_I2S_SUPPORTS_PDM_RX
void i2s_hal_set_rx_pdm_dsr(i2s_hal_context_t *hal, i2s_pdm_dsr_t dsr)
{
    i2s_ll_set_pdm_rx_dsr(hal->dev, dsr);
}

void i2s_hal_get_rx_pdm_dsr(i2s_hal_context_t *hal, i2s_pdm_dsr_t *dsr)
{
    i2s_ll_get_pdm_rx_dsr(hal->dev, dsr);
}
#endif

void i2s_hal_init(i2s_hal_context_t *hal, int i2s_num)
{
    //Get hardware instance.
    hal->dev = I2S_LL_GET_HW(i2s_num);
#if SOC_GDMA_SUPPORTED
    hal->dma = &GDMA;
    if (i2s_num == 0) {
        hal->dma_ch = SOC_GDMA_I2S0_DMA_CHANNEL;
        hal->dma_peri_sel = SOC_GDMA_TRIG_PERIPH_I2S0;
    }
#if SOC_I2S_NUM > 1
    if (i2s_num == 1) {
        hal->dma_ch = SOC_GDMA_I2S1_DMA_CHANNEL;
        hal->dma_peri_sel = SOC_GDMA_TRIG_PERIPH_I2S1;
    }
#endif
    gdma_ll_enable_m2m_mode(hal->dma, hal->dma_ch, false);
#endif
    i2s_ll_general_init(hal->dev);
}

static void i2s_hal_format_config(i2s_hal_context_t *hal, i2s_mode_t i2s_mode, i2s_comm_format_t format, i2s_slot_channel_cfg_t slot_bit_cfg, i2s_slot_bits_cfg_t slot_ch_cfg)
{
    int active_slot_num = slot_ch_cfg & 0xffff;
#if !SOC_I2S_SUPPORTS_TDM
    switch (format) {
        case I2S_COMM_FORMAT_STAND_MSB:
            if (i2s_mode & I2S_MODE_TX) {
                i2s_ll_set_tx_format_msb_align(hal->dev);
            }
            if (i2s_mode & I2S_MODE_RX) {
                i2s_ll_set_rx_format_msb_align(hal->dev);
            }
            break;
        case I2S_COMM_FORMAT_STAND_PCM_SHORT:
            if (i2s_mode & I2S_MODE_TX) {
                i2s_ll_set_tx_pcm_short(hal->dev);
            }
            if (i2s_mode & I2S_MODE_RX) {
                i2s_ll_set_rx_pcm_short(hal->dev);
            }
            break;
        case I2S_COMM_FORMAT_STAND_PCM_LONG:
            if (i2s_mode & I2S_MODE_TX) {
                i2s_ll_set_tx_pcm_long(hal->dev);
            }
            if (i2s_mode & I2S_MODE_RX) {
                i2s_ll_set_rx_pcm_long(hal->dev);
            }
            break;
        default: //I2S_COMM_FORMAT_STAND_I2S
            if (i2s_mode & I2S_MODE_TX) {
                i2s_ll_set_tx_format_philip(hal->dev);
            }
            if (i2s_mode & I2S_MODE_RX) {
                i2s_ll_set_rx_format_philip(hal->dev);
            }
            break;
    }
    if (active_slot_num == I2S_CHANNEL_MONO) {
        if (i2s_mode & I2S_MODE_TX) {
            i2s_ll_tx_mono_mode_ena(hal->dev, active_slot_num == I2S_CHANNEL_MONO);
        }
        if (i2s_mode & I2S_MODE_RX) {
            i2s_ll_rx_mono_mode_ena(hal->dev, active_slot_num == I2S_CHANNEL_MONO);
        }
    }
#else
    int data_bits = slot_bit_cfg & 0xffff;
    int slot_bits = ((slot_bit_cfg >> SLOT_BIT_SHIFT) == I2S_BITS_PER_SLOT_SAMPLE_BIT_EQU) ? data_bits : slot_bit_cfg >> SLOT_BIT_SHIFT;
    int slot_num = ((slot_ch_cfg >> SLOT_CH_SHIFT) == 0 || (active_slot_num == I2S_CHANNEL_MONO)) ? 2 : (slot_ch_cfg >> SLOT_CH_SHIFT);
    bool msb_shift_en = false;
    int tdm_ws_width = 0;
    switch (format) {
        case I2S_COMM_FORMAT_STAND_MSB:
            msb_shift_en = false;
            tdm_ws_width = slot_num*slot_bits/2;
            break;
        case I2S_COMM_FORMAT_STAND_PCM_SHORT:
            msb_shift_en = false;
            tdm_ws_width = 1;
            break;
        case I2S_COMM_FORMAT_STAND_PCM_LONG:
            msb_shift_en = false;
            tdm_ws_width = slot_bits;
            break;
        default: //I2S_COMM_FORMAT_STAND_I2S
            msb_shift_en = true;
            tdm_ws_width = slot_num*slot_bits/2;
            break;
    }
    if (i2s_mode & I2S_MODE_TX) {
        i2s_ll_tx_msb_shift_enable(hal->dev, msb_shift_en);
        i2s_ll_set_tx_tdm_ws_width(hal->dev, tdm_ws_width);
        i2s_ll_set_tx_half_sample_bit(hal->dev, slot_num*slot_bits/2);
    }
    if (i2s_mode & I2S_MODE_RX) {
        i2s_ll_rx_msb_shift_enable(hal->dev, msb_shift_en);
        i2s_ll_set_rx_tdm_ws_width(hal->dev, tdm_ws_width);
        i2s_ll_set_rx_half_sample_bit(hal->dev, slot_num*slot_bits/2);
    }
#endif
}

void i2s_hal_samples_config(i2s_hal_context_t *hal, i2s_mode_t i2s_mode, i2s_comm_format_t communication_format, i2s_slot_channel_cfg_t slot_bit_cfg, i2s_slot_bits_cfg_t slot_ch_cfg)
{
    int active_slot_num = slot_ch_cfg & 0xffff;
    int data_bits = slot_bit_cfg & 0xffff;
    int slot_bits = ((slot_bit_cfg >> SLOT_BIT_SHIFT) == I2S_BITS_PER_SLOT_SAMPLE_BIT_EQU) ? data_bits : slot_bit_cfg >> SLOT_BIT_SHIFT;
    int slot_num = ((slot_ch_cfg >> SLOT_CH_SHIFT) == 0 || (active_slot_num == I2S_CHANNEL_MONO)) ? 2 : (slot_ch_cfg >> SLOT_CH_SHIFT);
#if SOC_I2S_SUPPORTS_TDM
    if (i2s_mode & I2S_MODE_TX) {
        i2s_ll_set_tx_slot_mun(hal->dev, slot_num);
        i2s_ll_set_tx_sample_bit(hal->dev, slot_bits, data_bits);
    }
    if (i2s_mode & I2S_MODE_RX) {
        i2s_ll_set_rx_slot_mun(hal->dev, slot_num);
        i2s_ll_set_rx_sample_bit(hal->dev, slot_bits, data_bits);
    }
#else
    if (i2s_mode & I2S_MODE_TX) {
        i2s_ll_set_tx_sample_bit(hal->dev, slot_bits, data_bits);
    }
    if (i2s_mode & I2S_MODE_RX) {
        i2s_ll_set_rx_sample_bit(hal->dev, slot_bits, data_bits);
    }
#endif
    //I2S standards config: Philip, MSB or PCM, Only I2S mode should do this configuration.
    if ((i2s_mode & (~(I2S_MODE_I2S))) == 0) {
        i2s_hal_format_config(hal, i2s_mode, communication_format, slot_bit_cfg, slot_ch_cfg);
    }
}

void i2s_hal_config_param(i2s_hal_context_t *hal, const i2s_config_param_t *i2s_config)
{
    if (i2s_config->mode & I2S_MODE_TX) {
        i2s_ll_tx_gen_init(hal->dev);
        if (i2s_config->mode & I2S_MODE_SLAVE) {
            i2s_ll_set_tx_slave_mod(hal->dev, 1); //TX Slave
        }
    }
    if (i2s_config->mode & I2S_MODE_RX) {
        i2s_ll_rx_gen_init(hal->dev);
        if (i2s_config->mode & I2S_MODE_SLAVE) {
            i2s_ll_set_rx_slave_mod(hal->dev, 1); //RX Slave
        }
    }
#if SOC_I2S_SUPPORTS_ADC_DAC
    if (i2s_config->mode & (I2S_MODE_DAC_BUILT_IN | I2S_MODE_ADC_BUILT_IN)) {
        if (i2s_config->mode & I2S_MODE_DAC_BUILT_IN) {
            i2s_ll_build_in_dac_ena(hal->dev);
        }
        if (i2s_config->mode & I2S_MODE_ADC_BUILT_IN) {
            i2s_ll_build_in_adc_ena(hal->dev);
        }
        // Buildin ADC and DAC have nothing to do with communication format configuration.
        return;
    }
#endif

#if SOC_I2S_SUPPORTS_PDM
    if (!(i2s_config->mode & I2S_MODE_PDM)) {
#if SOC_I2S_SUPPORTS_PDM_RX
        if (i2s_config->mode & I2S_MODE_RX) {
            i2s_ll_set_rx_pdm_en(hal->dev, false);
        }
#endif
#if SOC_I2S_SUPPORTS_PDM_TX
        if (i2s_config->mode & I2S_MODE_TX) {
            i2s_ll_set_tx_pdm_en(hal->dev, false);
        }
#endif
    } else {
#if SOC_I2S_SUPPORTS_PDM_TX
        if (i2s_config->mode & I2S_MODE_TX) {
            i2s_ll_tx_pdm_cfg(hal->dev, i2s_config->sample_rate);
        }
#endif

#if SOC_I2S_SUPPORTS_PDM_RX
        if(i2s_config->mode & I2S_MODE_RX) {
            i2s_ll_rx_pdm_cfg(hal->dev);
        }
#endif
    }
#endif

#if SOC_I2S_SUPPORTS_TDM
    if (i2s_config->mode & I2S_MODE_TX) {
        i2s_ll_set_tx_active_slot_mask(hal->dev, i2s_config->active_slot_mask);
        i2s_ll_tx_left_align_enable(hal->dev, i2s_config->left_align_en);
        i2s_ll_tx_big_endian_enable(hal->dev, i2s_config->big_edin_en);
        i2s_ll_tx_set_bit_order(hal->dev, i2s_config->bit_order_msb_en);
    }
    if (i2s_config->mode & I2S_MODE_RX) {
        i2s_ll_set_rx_active_slot_mask(hal->dev, i2s_config->active_slot_mask);
        i2s_ll_rx_left_align_enable(hal->dev, i2s_config->left_align_en);
        i2s_ll_rx_big_endian_enable(hal->dev, i2s_config->big_edin_en);
        i2s_ll_rx_set_bit_order(hal->dev, i2s_config->bit_order_msb_en);
    }
#endif

    //Configure I2S slot number,sample bit.
    i2s_hal_samples_config(hal, i2s_config->mode, i2s_config->communication_format, i2s_config->slot_bits_cfg, i2s_config->slot_channel_cfg);
}

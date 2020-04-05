// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_log.h"
#include "hal/i2s_hal.h"

void i2s_hal_reset_fifo(i2s_hal_context_t *hal)
{
    i2s_ll_reset_rx_fifo(hal->dev);
    i2s_ll_reset_tx_fifo(hal->dev);
}

void i2s_hal_set_tx_mode(i2s_hal_context_t *hal, i2s_channel_t ch, i2s_bits_per_sample_t bits)
{
    if (bits <= I2S_BITS_PER_SAMPLE_16BIT) {
        i2s_ll_set_tx_fifo_mod(hal->dev, (ch == I2S_CHANNEL_STEREO) ? 0 : 1);
    } else {
        i2s_ll_set_tx_fifo_mod(hal->dev, (ch == I2S_CHANNEL_STEREO) ? 2 : 3);
    }
    i2s_ll_set_tx_chan_mod(hal->dev, (ch == I2S_CHANNEL_STEREO) ? 0 : 1);
#if SOC_I2S_SUPPORTS_DMA_EQUAL
    i2s_ll_set_tx_dma_equal(hal->dev, (ch == I2S_CHANNEL_STEREO) ? 0 : 1);
#endif
}

void i2s_hal_set_rx_mode(i2s_hal_context_t *hal, i2s_channel_t ch, i2s_bits_per_sample_t bits)
{
    if (bits <= I2S_BITS_PER_SAMPLE_16BIT) {
        i2s_ll_set_rx_fifo_mod(hal->dev, (ch == I2S_CHANNEL_STEREO) ? 0 : 1);
    } else {
        i2s_ll_set_rx_fifo_mod(hal->dev, (ch == I2S_CHANNEL_STEREO) ? 2 : 3);
    }
    i2s_ll_set_rx_chan_mod(hal->dev, (ch == I2S_CHANNEL_STEREO) ? 0 : 1);
#if SOC_I2S_SUPPORTS_DMA_EQUAL
    i2s_ll_set_rx_dma_equal(hal->dev, (ch == I2S_CHANNEL_STEREO) ? 0 : 1);
#endif
}

void i2s_hal_set_in_link(i2s_hal_context_t *hal, uint32_t bytes_num, uint32_t addr)
{
    i2s_ll_set_in_link_addr(hal->dev, addr);
    i2s_ll_set_rx_eof_num(hal->dev, bytes_num);
}

#if SOC_I2S_SUPPORTS_PDM
void i2s_hal_get_tx_pdm(i2s_hal_context_t *hal, int *fp, int *fs)
{
    i2s_ll_get_tx_pdm_fp(hal->dev, (uint32_t *)fp);
    i2s_ll_get_tx_pdm_fs(hal->dev, (uint32_t *)fs);
}
#endif

void i2s_hal_set_clk_div(i2s_hal_context_t *hal, int div_num, int div_a, int div_b, int tx_bck_div, int rx_bck_div)
{
    i2s_ll_set_clkm_div_num(hal->dev, div_num);
    i2s_ll_set_clkm_div_a(hal->dev, div_a);
    i2s_ll_set_clkm_div_b(hal->dev, div_b);
    i2s_ll_set_tx_bck_div_num(hal->dev, tx_bck_div);
    i2s_ll_set_rx_bck_div_num(hal->dev, rx_bck_div);
}

void i2s_hal_set_tx_bits_mod(i2s_hal_context_t *hal, i2s_bits_per_sample_t bits)
{
    i2s_ll_set_tx_bits_mod(hal->dev, bits);
}

void i2s_hal_set_rx_bits_mod(i2s_hal_context_t *hal, i2s_bits_per_sample_t bits)
{
    i2s_ll_set_rx_bits_mod(hal->dev, bits);
}

void i2s_hal_reset(i2s_hal_context_t *hal)
{
    i2s_ll_reset_dma_in(hal->dev);
    i2s_ll_reset_dma_out(hal->dev);
    i2s_ll_reset_tx(hal->dev);
    i2s_ll_reset_rx(hal->dev);
}

void i2s_hal_start_tx(i2s_hal_context_t *hal)
{
    i2s_ll_start_out_link(hal->dev);
    i2s_ll_start_tx(hal->dev);
}

void i2s_hal_start_rx(i2s_hal_context_t *hal)
{
    i2s_ll_start_in_link(hal->dev);
    i2s_ll_start_rx(hal->dev);
}

void i2s_hal_stop_tx(i2s_hal_context_t *hal)
{
    i2s_ll_stop_out_link(hal->dev);
    i2s_ll_stop_tx(hal->dev);
}

void i2s_hal_stop_rx(i2s_hal_context_t *hal)
{
    i2s_ll_stop_in_link(hal->dev);
    i2s_ll_stop_rx(hal->dev);
}

void i2s_hal_config_param(i2s_hal_context_t *hal, const i2s_config_t *i2s_config)
{
    //reset i2s
    i2s_ll_reset_tx(hal->dev);
    i2s_ll_reset_rx(hal->dev);

    //reset dma
    i2s_ll_reset_dma_in(hal->dev);
    i2s_ll_reset_dma_out(hal->dev);

    i2s_ll_enable_dma(hal->dev);

    i2s_ll_set_lcd_en(hal->dev, 0);
    i2s_ll_set_camera_en(hal->dev, 0);
#if SOC_I2S_SUPPORTS_PDM
    i2s_ll_set_pcm2pdm_conv_en(hal->dev, 0);
    i2s_ll_set_pdm2pcm_conv_en(hal->dev, 0);
#endif

    i2s_ll_set_dscr_en(hal->dev, 0);

    i2s_ll_set_tx_chan_mod(hal->dev, i2s_config->channel_format < I2S_CHANNEL_FMT_ONLY_RIGHT ? i2s_config->channel_format : (i2s_config->channel_format >> 1)); // 0-two channel;1-right;2-left;3-righ;4-left
    i2s_ll_set_tx_fifo_mod(hal->dev, i2s_config->channel_format < I2S_CHANNEL_FMT_ONLY_RIGHT ? 0 : 1); // 0-right&left channel;1-one channel
    i2s_ll_set_tx_mono(hal->dev, 0);

    i2s_ll_set_rx_chan_mod(hal->dev, i2s_config->channel_format < I2S_CHANNEL_FMT_ONLY_RIGHT ? i2s_config->channel_format : (i2s_config->channel_format >> 1)); // 0-two channel;1-right;2-left;3-righ;4-left
    i2s_ll_set_rx_fifo_mod(hal->dev, i2s_config->channel_format < I2S_CHANNEL_FMT_ONLY_RIGHT ? 0 : 1); // 0-right&left channel;1-one channel
    i2s_ll_set_rx_mono(hal->dev, 0);

    i2s_ll_set_dscr_en(hal->dev, 1); //connect dma to fifo

    i2s_ll_stop_tx(hal->dev);
    i2s_ll_stop_rx(hal->dev);

    if (i2s_config->mode & I2S_MODE_TX) {
        i2s_ll_set_tx_msb_right(hal->dev, 0);
        i2s_ll_set_tx_right_first(hal->dev, 0);

        i2s_ll_set_tx_slave_mod(hal->dev, 0); // Master
        i2s_ll_set_tx_fifo_mod_force_en(hal->dev, 1);

        if (i2s_config->mode & I2S_MODE_SLAVE) {
            i2s_ll_set_tx_slave_mod(hal->dev, 1); //TX Slave
        }
    }

    if (i2s_config->mode & I2S_MODE_RX) {
        i2s_ll_set_rx_msb_right(hal->dev, 0);
        i2s_ll_set_rx_right_first(hal->dev, 0);
        i2s_ll_set_rx_slave_mod(hal->dev, 0); // Master
        i2s_ll_set_rx_fifo_mod_force_en(hal->dev, 1);

        if (i2s_config->mode & I2S_MODE_SLAVE) {
            i2s_ll_set_rx_slave_mod(hal->dev, 1); //RX Slave
        }
    }

#if SOC_I2S_SUPPORTS_ADC_DAC
    if (i2s_config->mode & (I2S_MODE_DAC_BUILT_IN | I2S_MODE_ADC_BUILT_IN)) {
        i2s_ll_set_lcd_en(hal->dev, 1);
        i2s_ll_set_tx_right_first(hal->dev, 1);
        i2s_ll_set_camera_en(hal->dev, 0);
    }
#endif

#if SOC_I2S_SUPPORTS_PDM
    if (i2s_config->mode & I2S_MODE_PDM) {
        i2s_ll_set_rx_fifo_mod_force_en(hal->dev, 1);
        i2s_ll_set_tx_fifo_mod_force_en(hal->dev, 1);

        i2s_ll_set_tx_pdm_fp(hal->dev, 960);
        i2s_ll_set_tx_pdm_fs(hal->dev, i2s_config->sample_rate / 1000 * 10);
        uint32_t fp, fs;
        i2s_ll_get_tx_pdm_fp(hal->dev, &fp);
        i2s_ll_get_tx_pdm_fs(hal->dev, &fs);
        i2s_ll_set_tx_sinc_osr2(hal->dev, fp / fs);

        i2s_ll_set_rx_sinc_dsr_16_en(hal->dev, 0);
        i2s_ll_set_rx_pdm_en(hal->dev, 1);
        i2s_ll_set_tx_pdm_en(hal->dev, 1);

        i2s_ll_set_pcm2pdm_conv_en(hal->dev, 1);
        i2s_ll_set_pdm2pcm_conv_en(hal->dev, 1);
    } else {
        i2s_ll_set_rx_pdm_en(hal->dev, 0);
        i2s_ll_set_tx_pdm_en(hal->dev, 0);
    }
#endif
    if (i2s_config->communication_format & I2S_COMM_FORMAT_I2S) {
        i2s_ll_set_tx_short_sync(hal->dev, 0);
        i2s_ll_set_rx_short_sync(hal->dev, 0);
        i2s_ll_set_tx_msb_shift(hal->dev, 1);
        i2s_ll_set_rx_msb_shift(hal->dev, 1);
        if (i2s_config->communication_format & I2S_COMM_FORMAT_I2S_LSB) {
            if (i2s_config->mode & I2S_MODE_TX) {
                i2s_ll_set_tx_msb_shift(hal->dev, 0);
            }
            if (i2s_config->mode & I2S_MODE_RX) {
                i2s_ll_set_rx_msb_shift(hal->dev, 0);
            }
        }
    }

    if (i2s_config->communication_format & I2S_COMM_FORMAT_PCM) {
        i2s_ll_set_tx_msb_shift(hal->dev, 0);
        i2s_ll_set_rx_msb_shift(hal->dev, 0);
        i2s_ll_set_tx_short_sync(hal->dev, 0);
        i2s_ll_set_rx_short_sync(hal->dev, 0);
        if (i2s_config->communication_format & I2S_COMM_FORMAT_PCM_SHORT) {
            if (i2s_config->mode & I2S_MODE_TX) {
                i2s_ll_set_tx_short_sync(hal->dev, 1);
            }
            if (i2s_config->mode & I2S_MODE_RX) {
                i2s_ll_set_rx_short_sync(hal->dev, 1);
            }
        } 
    }
}

void i2s_hal_enable_master_mode(i2s_hal_context_t *hal)
{
    i2s_ll_set_tx_slave_mod(hal->dev, 0); //MASTER Slave
    i2s_ll_set_rx_slave_mod(hal->dev, 1); //RX Slave
}

void i2s_hal_enable_slave_mode(i2s_hal_context_t *hal)
{
    i2s_ll_set_tx_slave_mod(hal->dev, 1); //TX Slave
    i2s_ll_set_rx_slave_mod(hal->dev, 1); //RX Slave
}

void i2s_hal_init(i2s_hal_context_t *hal, int i2s_num)
{
    //Get hardware instance.
    hal->dev = I2S_LL_GET_HW(i2s_num);
}

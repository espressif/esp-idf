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

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for I2S.
// There is no parameter check in the hal layer, so the caller must ensure the correctness of the parameters.

#pragma once

#include "soc/i2s_periph.h"
#include "soc/soc_caps.h"
#include "hal/i2s_types.h"
#include "hal/i2s_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2S channel bits configurations
 *
 */
typedef union {
    struct {
        uint32_t sample_bits : 16;  /*!< I2S sample bits in one channel */
        uint32_t chan_bits   : 16;  /*!< I2S total bits in one channel. Should not be smaller than 'sample_bits', default '0' means equal to 'sample_bits' */
    };
    uint32_t val;                   /*!< I2S cannel bits configiration value */
} i2s_hal_bits_cfg_t;

/**
 * @brief I2S HAL configurations
 */
typedef struct {
    i2s_mode_t              mode;                   /*!< I2S work mode, using ored mask of `i2s_mode_t`*/
    uint32_t                sample_rate;            /*!< I2S sample rate*/
    i2s_channel_t           ch;                     /*!< I2S channels*/
    i2s_comm_format_t       comm_fmt;               /*!< I2S communication format */
    i2s_channel_fmt_t       chan_fmt;               /*!< I2S channel format, there are total 16 channels in TDM mode.*/
    i2s_hal_bits_cfg_t      bits_cfg;               /*!< Channel bits configuration*/
#if SOC_I2S_SUPPORTS_TDM
    uint32_t                total_chan;             /*!< Total number of I2S channels */
    uint32_t                chan_mask;              /*!< Active channel bit mask, set value in `i2s_channel_t` to enable specific channel, the bit map of active channel can not exceed (0x1<<total_chan_num). */
    bool                    left_align;          /*!< Set to enable left aligment */
    bool                    big_edin;            /*!< Set to enable big edin */
    bool                    bit_order_msb;       /*!< Set to enable msb order */
    bool                    skip_msk;            /*!< Set to enable skip mask. If it is enabled, only the data of the enabled channels will be sent, otherwise all data stored in DMA TX buffer will be sent */
#endif
} i2s_hal_config_t;

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    i2s_dev_t *dev;
    uint32_t version;
} i2s_hal_context_t;

/**
 * @brief Reset I2S TX channel
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_reset_tx(hal)                   i2s_ll_tx_reset((hal)->dev)

/**
 * @brief Reset I2S TX fifo
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_reset_tx_fifo(hal)              i2s_ll_tx_reset_fifo((hal)->dev)

/**
 * @brief Reset I2S RX channel
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_reset_rx(hal)                   i2s_ll_rx_reset((hal)->dev)

/**
 * @brief Reset I2S RX fifo
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_reset_rx_fifo(hal)              i2s_ll_rx_reset_fifo((hal)->dev)

/**
 * @brief Init the I2S hal. This function should be called first before other hal layer function is called
 *
 * @param hal Context of the HAL layer
 * @param i2s_num The uart port number, the max port number is (I2S_NUM_MAX -1)
 */
void i2s_hal_init(i2s_hal_context_t *hal, int i2s_num);

/**
 * @brief Configure I2S source clock
 *
 * @param hal Context of the HAL layer
 * @param sel The source clock index
 */
void i2s_hal_set_clock_src(i2s_hal_context_t *hal, i2s_clock_src_t sel);

/**
 * @brief Set Tx channel style
 *
 * @param hal Context of the HAL layer
 * @param hal_cfg I2S hal configuration structer, refer to `i2s_hal_config_t`
 */
void i2s_hal_tx_set_channel_style(i2s_hal_context_t *hal, const i2s_hal_config_t *hal_cfg);

/**
 * @brief Set Rx channel style
 *
 * @param hal Context of the HAL layer
 * @param hal_cfg I2S hal configuration structer, refer to `i2s_hal_config_t`
 */
void i2s_hal_rx_set_channel_style(i2s_hal_context_t *hal, const i2s_hal_config_t *hal_cfg);

/**
 * @brief Config I2S param
 *
 * @param hal Context of the HAL layer
 * @param hal_cfg I2S hal configuration structer, refer to `i2s_hal_config_t`
 */
void i2s_hal_config_param(i2s_hal_context_t *hal, const i2s_hal_config_t *hal_cfg);

/**
 * @brief Enable I2S master full-duplex mode
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_enable_master_fd_mode(i2s_hal_context_t *hal);

/**
 * @brief Enable I2S slave full-duplex mode
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_enable_slave_fd_mode(i2s_hal_context_t *hal);

/**
 * @brief Start I2S tx
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_start_tx(hal)               i2s_ll_tx_start((hal)->dev)

/**
 * @brief Start I2S rx
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_start_rx(hal)               i2s_ll_rx_start((hal)->dev)

/**
 * @brief Stop I2S tx
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_stop_tx(hal)                i2s_ll_tx_stop((hal)->dev)

/**
 * @brief Stop I2S rx
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_stop_rx(hal)                i2s_ll_rx_stop((hal)->dev)

/**
 * @brief Set the received data length to trigger `in_suc_eof` interrupt.
 *
 * @param hal Context of the HAL layer
 * @param eof_byte The byte length that trigger in_suc_eof interrupt.
 */
#define i2s_hal_set_rx_eof_num(hal, eof_byte)   i2s_ll_rx_set_eof_num((hal)->dev, eof_byte)

/**
 * @brief Set I2S TX sample bit
 *
 * @param hal Context of the HAL layer
 * @param chan_bit I2S TX chan bit
 * @param data_bit The sample data bit length.
 */
#define i2s_hal_set_tx_sample_bit(hal, chan_bit, data_bit)  i2s_ll_tx_set_sample_bit((hal)->dev, chan_bit, data_bit)

/**
 * @brief Set I2S RX sample bit
 *
 * @param hal Context of the HAL layer
 * @param chan_bit I2S RX chan bit
 * @param data_bit The sample data bit length.
 */
#define i2s_hal_set_rx_sample_bit(hal, chan_bit, data_bit)  i2s_ll_rx_set_sample_bit((hal)->dev, chan_bit, data_bit)

/**
 * @brief Configure I2S TX module clock devider
 *
 * @param hal Context of the HAL layer
 * @param sclk I2S source clock freq
 * @param fbck I2S bck freq
 * @param factor bck factor, factor=sclk/fbck
 */
void i2s_hal_tx_clock_config(i2s_hal_context_t *hal, uint32_t sclk, uint32_t fbck, int factor);

/**
 * @brief Configure I2S RX module clock devider
 *
 * @param hal Context of the HAL layer
 * @param sclk I2S source clock freq
 * @param fbck I2S bck freq
 * @param factor bck factor, factor=sclk/fbck
 */
void i2s_hal_rx_clock_config(i2s_hal_context_t *hal, uint32_t sclk, uint32_t fbck, int factor);

#if SOC_I2S_SUPPORTS_PCM
/**
 * @brief Configure I2S TX PCM encoder or decoder.
 *
 * @param hal Context of the HAL layer
 * @param cfg PCM configure paramater, refer to `i2s_pcm_compress_t`
 */
#define i2s_hal_tx_pcm_cfg(hal, cfg)        i2s_ll_tx_set_pcm_type((hal)->dev, cfg)

/**
 * @brief Configure I2S RX PCM encoder or decoder.
 *
 * @param hal Context of the HAL layer
 * @param cfg PCM configure paramater, refer to `i2s_pcm_compress_t`
 */
#define i2s_hal_rx_pcm_cfg(hal, cfg)        i2s_ll_rx_set_pcm_type((hal)->dev, cfg)
#endif

/**
 * @brief Enable loopback mode
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_enable_sig_loopback(hal)    i2s_ll_enable_loop_back((hal)->dev, true)

/**
 * @brief Set I2S configuration for common TX mode
 * @note Common mode is for non-PDM mode like philip/MSB/PCM
 *
 * @param hal Context of the HAL layer
 * @param hal_cfg hal configuration structure
 */
void i2s_hal_tx_set_common_mode(i2s_hal_context_t *hal, const i2s_hal_config_t *hal_cfg);

/**
 * @brief Set I2S configuration for common RX mode
 * @note Common mode is for non-PDM mode like philip/MSB/PCM
 *
 * @param hal Context of the HAL layer
 * @param hal_cfg hal configuration structure
 */
void i2s_hal_rx_set_common_mode(i2s_hal_context_t *hal, const i2s_hal_config_t *hal_cfg);

#if SOC_I2S_SUPPORTS_PDM_TX
/**
 * @brief Configure I2S TX PDM sample rate
 *        Fpdm = 64*Fpcm*fp/fs
 *
 * @param hal Context of the HAL layer
 * @param fp TX PDM fp paramater configuration
 * @param fs TX PDM fs paramater configuration
 */
#define i2s_hal_set_tx_pdm_fpfs(hal, fp, fs)    i2s_ll_tx_set_pdm_fpfs((hal)->dev, fp, fs)

/**
 * @brief Get I2S TX PDM fp
 *
 * @param hal Context of the HAL layer
 * @return
 *        - fp configuration paramater
 */
#define i2s_hal_get_tx_pdm_fp(hal)      i2s_ll_tx_get_pdm_fp((hal)->dev)

/**
 * @brief Get I2S TX PDM fs
 *
 * @param hal Context of the HAL layer
 * @return
 *        - fs configuration paramater
 */
#define i2s_hal_get_tx_pdm_fs(hal)      i2s_ll_tx_get_pdm_fs((hal)->dev)

/**
 * @brief Set I2S default configuration for PDM TX mode
 *
 * @param hal Context of the HAL layer
 * @param sample_rate PDM sample rate
 */
void i2s_hal_tx_set_pdm_mode_default(i2s_hal_context_t *hal, uint32_t sample_rate);
#endif

#if SOC_I2S_SUPPORTS_PDM_RX

/**
 * @brief Configure RX PDM downsample
 *
 * @param hal Context of the HAL layer
 * @param dsr PDM downsample configuration paramater
 */
#define i2s_hal_set_rx_pdm_dsr(hal, dsr)         i2s_ll_rx_set_pdm_dsr((hal)->dev, dsr)

/**
 * @brief Get RX PDM downsample configuration
 *
 * @param hal Context of the HAL layer
 * @param dsr Pointer to accept PDM downsample configuration
 */
#define i2s_hal_get_rx_pdm_dsr(hal, dsr)        i2s_ll_rx_get_pdm_dsr((hal)->dev, dsr)

/**
 * @brief Set I2S default configuration for PDM R mode
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_rx_set_pdm_mode_default(i2s_hal_context_t *hal);
#endif

#if !SOC_GDMA_SUPPORTED
/**
 * @brief Enable I2S TX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_enable_tx_dma(hal) i2s_ll_enable_dma((hal)->dev,true)

/**
 * @brief Enable I2S RX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_enable_rx_dma(hal) i2s_ll_enable_dma((hal)->dev,true)

/**
 * @brief Disable I2S TX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_disable_tx_dma(hal) i2s_ll_enable_dma((hal)->dev,false)

/**
 * @brief Disable I2S RX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_disable_rx_dma(hal) i2s_ll_enable_dma((hal)->dev,false)

/**
 * @brief Get I2S interrupt status
 *
 * @param hal Context of the HAL layer
 * @return
 *        - module interrupt status
 */
#define i2s_hal_get_intr_status(hal) i2s_ll_get_intr_status((hal)->dev)

/**
 * @brief Get I2S interrupt status
 *
 * @param hal Context of the HAL layer
 * @param mask Interrupt mask to be cleared.
 */
#define i2s_hal_clear_intr_status(hal, mask) i2s_ll_clear_intr_status((hal)->dev, mask)

/**
 * @brief Enable I2S RX interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_enable_rx_intr(hal) i2s_ll_rx_enable_intr((hal)->dev)

/**
 * @brief Disable I2S RX interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_disable_rx_intr(hal) i2s_ll_rx_disable_intr((hal)->dev)

/**
 * @brief Disable I2S TX interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_disable_tx_intr(hal) i2s_ll_tx_disable_intr((hal)->dev)

/**
 * @brief Enable I2S TX interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_enable_tx_intr(hal) i2s_ll_tx_enable_intr((hal)->dev)

/**
 * @brief Configure TX DMA descriptor address and start TX DMA
 *
 * @param hal Context of the HAL layer
 * @param link_addr DMA descriptor link address.
 */
#define i2s_hal_start_tx_link(hal, link_addr) i2s_ll_tx_start_link((hal)->dev, link_addr)

/**
 * @brief Configure RX DMA descriptor address and start RX DMA
 *
 * @param hal Context of the HAL layer
 * @param link_addr DMA descriptor link address.
 */
#define i2s_hal_start_rx_link(hal, link_addr) i2s_ll_rx_start_link((hal)->dev, link_addr)

/**
 * @brief Stop TX DMA link
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_stop_tx_link(hal) i2s_ll_tx_stop_link((hal)->dev)

/**
 * @brief Stop RX DMA link
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_stop_rx_link(hal) i2s_ll_rx_stop_link((hal)->dev)

/**
 * @brief Reset RX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_reset_rxdma(hal) i2s_ll_rx_reset_dma((hal)->dev)

/**
 * @brief Reset TX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_reset_txdma(hal) i2s_ll_tx_reset_dma((hal)->dev)

/**
 * @brief Get I2S out eof descriptor address
 *
 * @param hal Context of the HAL layer
 * @param addr Pointer to accept out eof des address
 */
#define i2s_hal_get_out_eof_des_addr(hal, addr) i2s_ll_tx_get_eof_des_addr((hal)->dev, addr)

/**
 * @brief Get I2S in suc eof descriptor address
 *
 * @param hal Context of the HAL layer
 * @param addr Pointer to accept in suc eof des address
 */
#define i2s_hal_get_in_eof_des_addr(hal, addr) i2s_ll_rx_get_eof_des_addr((hal)->dev, addr)
#endif

#if SOC_I2S_SUPPORTS_ADC_DAC
/**
 * @brief Enable Builtin DAC
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_enable_builtin_dac(hal)      i2s_ll_enable_builtin_dac((hal)->dev, true);

/**
 * @brief Enable Builtin ADC
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_enable_builtin_adc(hal)      i2s_ll_enable_builtin_adc((hal)->dev, true);

/**
 * @brief Disable Builtin DAC
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_disable_builtin_dac(hal)     i2s_ll_enable_builtin_dac((hal)->dev, false);

/**
 * @brief Disable Builtin ADC
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_disable_builtin_adc(hal)     i2s_ll_enable_builtin_adc((hal)->dev, false);
#endif

#ifdef __cplusplus
}
#endif

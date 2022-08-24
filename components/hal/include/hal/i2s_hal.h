/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
 * @brief I2S clock configuration
 */
typedef struct {
    uint32_t sclk;          /*!< I2S module clock */
    uint32_t mclk;          /*!< I2S master clock */
    uint32_t bclk;          /*!< I2S bit clock */
    uint16_t mclk_div;      /*!< I2S master clock division */
    uint16_t bclk_div;      /*!< I2S bit clock division*/
} i2s_hal_clock_cfg_t;


/**
 * @brief I2S HAL configurations
 */
typedef struct {
    i2s_mode_t              mode;                   /*!< I2S work mode, using ored mask of `i2s_mode_t`*/
    uint32_t                sample_rate;            /*!< I2S sample rate*/
    i2s_comm_format_t       comm_fmt;               /*!< I2S communication format */
    i2s_channel_fmt_t       chan_fmt;               /*!< I2S channel format, there are total 16 channels in TDM mode.*/
    uint32_t                sample_bits;            /*!< I2S sample bits in one channel */
    uint32_t                chan_bits;              /*!< I2S total bits in one channel. Should not be smaller than 'sample_bits', default '0' means equal to 'sample_bits' */
    uint32_t                active_chan;            /*!< I2S active channel number */
    uint32_t                total_chan;             /*!< Total number of I2S channels */

#if SOC_I2S_SUPPORTS_TDM
    uint32_t                chan_mask;              /*!< Active channel bit mask, set value in `i2s_channel_t` to enable specific channel, the bit map of active channel can not exceed (0x1<<total_chan_num). */
    bool                    left_align;             /*!< Set to enable left aligment */
    bool                    big_edin;               /*!< Set to enable big edin */
    bool                    bit_order_msb;          /*!< Set to enable msb order */
    bool                    skip_msk;               /*!< Set to enable skip mask. If it is enabled, only the data of the enabled channels will be sent, otherwise all data stored in DMA TX buffer will be sent */
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
 * @brief Enable I2S module clock
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_enable_module_clock(hal)        i2s_ll_enable_clock((hal)->dev);

/**
 * @brief Disable I2S module clock
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_disable_module_clock(hal)       i2s_ll_disable_clock((hal)->dev);

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
 * @brief Get I2S hardware instance and enable I2S module clock
 * @note  This function should be called first before other hal layer function is called
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
 * @brief Calculate the closest sample rate clock configuration.
 *        clock relationship:
 *        Fmclk = bck_div*fbck = fsclk/(mclk_div+b/a)
 *
 * @param clk_cfg I2S clock configuration(input)
 * @param cal Point to `i2s_ll_mclk_div_t` structure(output).
 */
void i2s_hal_mclk_div_decimal_cal(i2s_hal_clock_cfg_t *clk_cfg, i2s_ll_mclk_div_t *cal);

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
 * @brief Initialize I2S hardware
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
void i2s_hal_start_tx(i2s_hal_context_t *hal);

/**
 * @brief Start I2S rx
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_start_rx(i2s_hal_context_t *hal);

/**
 * @brief Stop I2S tx
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_stop_tx(i2s_hal_context_t *hal);

/**
 * @brief Stop I2S rx
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_stop_rx(i2s_hal_context_t *hal);

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
 * @param clk_cfg I2S clock configuration
 */
void i2s_hal_tx_clock_config(i2s_hal_context_t *hal, i2s_hal_clock_cfg_t *clk_cfg);

/**
 * @brief Configure I2S RX module clock devider
 *
 * @param hal Context of the HAL layer
 * @param clk_cfg I2S clock configuration
 */
void i2s_hal_rx_clock_config(i2s_hal_context_t *hal, i2s_hal_clock_cfg_t *clk_cfg);

/**
 * @brief Set I2S tx clock source
 *
 * @param hal Context of the HAL layer
 * @param clk_src i2s tx clock source (see 'i2s_clock_src_t')
 */
#define i2s_hal_tx_set_clock_source(hal, clk_src)   i2s_ll_tx_clk_set_src((hal)->dev, clk_src)

/**
 * @brief Set I2S rx clock source
 *
 * @param hal Context of the HAL layer
 * @param clk_src i2s rx clock source (see 'i2s_clock_src_t')
 */
#define i2s_hal_rx_set_clock_source(hal, clk_src)   i2s_ll_rx_clk_set_src((hal)->dev, clk_src)

/**
 * @brief Enable I2S tx slave mode
 *
 * @param hal Context of the HAL layer
 * @param enable set 'true' to enable tx slave mode
 */
#define i2s_hal_tx_enable_slave_mode(hal, enable)   i2s_ll_tx_set_slave_mod((hal)->dev, enable)

/**
 * @brief Enable I2S rx slave mode
 *
 * @param hal Context of the HAL layer
 * @param enable set 'true' to enable rx slave mode
 */
#define i2s_hal_rx_enable_slave_mode(hal, enable)   i2s_ll_rx_set_slave_mod((hal)->dev, enable)

/**
 * @brief Enable loopback mode
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_enable_sig_loopback(hal)    i2s_ll_share_bck_ws((hal)->dev, true)

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
 * @param is_mono whether is mono
 */
void i2s_hal_tx_set_pdm_mode_default(i2s_hal_context_t *hal, uint32_t sample_rate, bool is_mono);
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

#if SOC_I2S_SUPPORTS_ADC
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
 * @brief Disable Builtin ADC
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_disable_builtin_adc(hal)     i2s_ll_enable_builtin_adc((hal)->dev, false);
#endif

#if SOC_I2S_SUPPORTS_DAC
/**
 * @brief Disable Builtin DAC
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_disable_builtin_dac(hal)     i2s_ll_enable_builtin_dac((hal)->dev, false);
#endif

#ifdef __cplusplus
}
#endif

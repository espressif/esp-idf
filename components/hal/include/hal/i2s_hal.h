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
#if SOC_GDMA_SUPPORTED
#include "hal/gdma_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    i2s_dev_t *dev;
#if SOC_GDMA_SUPPORTED
    gdma_dev_t *dma;
    int dma_ch;
    int dma_peri_sel;
#endif
    uint32_t version;
} i2s_hal_context_t;


/**
 * @brief Reset I2S TX channel
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_reset_tx(i2s_hal_context_t *hal);

/**
 * @brief Reset I2S TX fifo
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_reset_tx_fifo(i2s_hal_context_t *hal);

/**
 * @brief Reset I2S RX channel
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_reset_rx(i2s_hal_context_t *hal);

/**
 * @brief Reset I2S RX fifo
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_reset_rx_fifo(i2s_hal_context_t *hal);

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
 * @brief Configure communication format
 *
 * @param hal Context of the HAL layer
 * @param i2s_mode I2S mode. Using the ored mask of I2S_MODE_MASTER, I2S_MODE_SLAVE, I2S_MODE_TX, I2S_MODE_RX
 * @param communication_format I2S communication format. Can be a value of `i2s_comm_format_t`.
 * @param slot_bit_cfg I2S slot bit configuration
 * @param slot_ch_cfg I2S slot channel configuration
 */
void i2s_hal_samples_config(i2s_hal_context_t *hal, i2s_mode_t i2s_mode, i2s_comm_format_t communication_format, i2s_slot_channel_cfg_t slot_bit_cfg, i2s_slot_bits_cfg_t slot_ch_cfg);

/**
 * @brief Config I2S param
 *
 * @param hal Context of the HAL layer
 * @param i2s_config I2S paramater configuration structer, refer to `i2s_config_param_t`
 */
void i2s_hal_config_param(i2s_hal_context_t *hal, const i2s_config_param_t *i2s_config);

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
void i2s_hal_set_rx_eof_num(i2s_hal_context_t *hal, uint32_t eof_byte);

/**
 * @brief Set I2S TX sample bit
 *
 * @param hal Context of the HAL layer
 * @param slot_bit I2S TX slot bit
 * @param data_bit The sample data bit lengh.
 */
void i2s_hal_set_tx_sample_bit(i2s_hal_context_t *hal, int slot_bit, int data_bit);

/**
 * @brief Set I2S RX sample bit
 *
 * @param hal Context of the HAL layer
 * @param slot_bit I2S RX slot bit
 * @param data_bit The sample data bit lengh.
 */
void i2s_hal_set_rx_sample_bit(i2s_hal_context_t *hal, int slot_bit, int data_bit);

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
 * @param cfg PCM configure paramater, refer to `i2s_pcm_cfg_t`
 */
void i2s_hal_tx_pcm_cfg(i2s_hal_context_t *hal, i2s_pcm_cfg_t cfg);

/**
 * @brief Configure I2S RX PCM encoder or decoder.
 *
 * @param hal Context of the HAL layer
 * @param cfg PCM configure paramater, refer to `i2s_pcm_cfg_t`
 */
void i2s_hal_rx_pcm_cfg(i2s_hal_context_t *hal, i2s_pcm_cfg_t cfg);
#endif

/**
 * @brief Enable loopback mode
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_enable_sig_loopback(i2s_hal_context_t *hal);

#if SOC_I2S_SUPPORTS_PDM_TX
/**
 * @brief Configure I2S TX PDM sample rate
 *        Fpdm = 64*Fpcm*fp/fs
 *
 * @param hal Context of the HAL layer
 * @param fp TX PDM fp paramater configuration
 * @param fs TX PDM fs paramater configuration
 */
void i2s_hal_set_tx_pdm_fpfs(i2s_hal_context_t *hal, int fp, int fs);

/**
 * @brief Get I2S TX PDM configuration
 *
 * @param hal Context of the HAL layer
 * @param fp Pointer to accept TX PDM fp paramater configuration
 * @param fs Pointer to accept TX PDM fs paramater configuration
 */
void i2s_hal_get_tx_pdm_fpfs(i2s_hal_context_t *hal, int *fp, int *fs);
#endif

#if SOC_I2S_SUPPORTS_PDM_RX

/**
 * @brief Configure RX PDM downsample
 *
 * @param hal Context of the HAL layer
 * @param dsr PDM downsample configuration paramater
 */
void i2s_hal_set_rx_pdm_dsr(i2s_hal_context_t *hal, i2s_pdm_dsr_t dsr);

/**
 * @brief Get RX PDM downsample configuration
 *
 * @param hal Context of the HAL layer
 * @param dsr Pointer to accept PDM downsample configuration
 */
void i2s_hal_get_rx_pdm_dsr(i2s_hal_context_t *hal, i2s_pdm_dsr_t *dsr);
#endif

#if !SOC_GDMA_SUPPORTED
/**
 * @brief Enable I2S TX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_attach_tx_dma(hal) i2s_ll_dma_enable((hal)->dev,true)

/**
 * @brief Enable I2S RX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_attach_rx_dma(hal) i2s_ll_dma_enable((hal)->dev,true)

/**
 * @brief Get I2S interrupt status
 *
 * @param hal Context of the HAL layer
 * @param status Pointer to accept I2S interrupt status
 */
#define i2s_hal_get_intr_status(hal, status) i2s_ll_get_intr_status((hal)->dev, status)

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
#define i2s_hal_enable_rx_intr(hal) i2s_ll_enable_rx_intr((hal)->dev)

/**
 * @brief Disable I2S RX interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_disable_rx_intr(hal) i2s_ll_disable_rx_intr((hal)->dev)

/**
 * @brief Disable I2S TX interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_disable_tx_intr(hal) i2s_ll_disable_tx_intr((hal)->dev)

/**
 * @brief Enable I2S TX interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_enable_tx_intr(hal) i2s_ll_enable_tx_intr((hal)->dev)

/**
 * @brief Configure TX DMA descriptor address and start TX DMA
 *
 * @param hal Context of the HAL layer
 * @param link_addr DMA descriptor link address.
 */
#define i2s_hal_start_tx_link(hal, link_addr) i2s_ll_start_tx_link((hal)->dev, link_addr)

/**
 * @brief Configure RX DMA descriptor address and start RX DMA
 *
 * @param hal Context of the HAL layer
 * @param link_addr DMA descriptor link address.
 */
#define i2s_hal_start_rx_link(hal, link_addr) i2s_ll_start_rx_link((hal)->dev, link_addr)

/**
 * @brief Stop TX DMA link
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_stop_tx_link(hal) i2s_ll_stop_out_link((hal)->dev)

/**
 * @brief Stop RX DMA link
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_stop_rx_link(hal) i2s_ll_stop_in_link((hal)->dev)

/**
 * @brief Reset RX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_reset_rxdma(hal) i2s_ll_reset_dma_in((hal)->dev)

/**
 * @brief Reset TX DMA
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_reset_txdma(hal) i2s_ll_reset_dma_out((hal)->dev)

/**
 * @brief Get I2S out eof descriptor address
 *
 * @param hal Context of the HAL layer
 * @param addr Pointer to accept out eof des address
 */
#define i2s_hal_get_out_eof_des_addr(hal, addr) i2s_ll_get_out_eof_des_addr((hal)->dev, addr)

/**
 * @brief Get I2S in suc eof descriptor address
 *
 * @param hal Context of the HAL layer
 * @param addr Pointer to accept in suc eof des address
 */
#define i2s_hal_get_in_eof_des_addr(hal, addr) i2s_ll_get_in_eof_des_addr((hal)->dev, addr)
#else
#define i2s_hal_attach_tx_dma(hal)  gdma_ll_tx_connect_to_periph((hal)->dma, (hal)->dma_ch, (hal)->dma_peri_sel)
#define i2s_hal_attach_rx_dma(hal)  gdma_ll_rx_connect_to_periph((hal)->dma, (hal)->dma_ch, (hal)->dma_peri_sel)
#define i2s_hal_get_intr_status(hal, status) (*status = (gdma_ll_get_interrupt_status((hal)->dma, (hal)->dma_ch)))
#define i2s_hal_clear_intr_status(hal, mask) gdma_ll_clear_interrupt_status((hal)->dma, (hal)->dma_ch, mask)
#define i2s_hal_enable_rx_intr(hal) gdma_ll_enable_interrupt((hal)->dma, (hal)->dma_ch, I2S_INTR_IN_DSCR_ERR|I2S_INTR_IN_SUC_EOF, 1)
#define i2s_hal_disable_rx_intr(hal) gdma_ll_enable_interrupt((hal)->dma, (hal)->dma_ch, I2S_INTR_IN_DSCR_ERR|I2S_INTR_IN_SUC_EOF, 0)
#define i2s_hal_enable_tx_intr(hal) gdma_ll_enable_interrupt((hal)->dma, (hal)->dma_ch, I2S_INTR_OUT_DSCR_ERR|I2S_INTR_OUT_EOF, 1)
#define i2s_hal_disable_tx_intr(hal) gdma_ll_enable_interrupt((hal)->dma, (hal)->dma_ch, I2S_INTR_OUT_DSCR_ERR|I2S_INTR_OUT_EOF, 0)
#define i2s_hal_start_tx_link(hal, link_addr) do{\
                                                gdma_ll_tx_set_desc_addr((hal)->dma,(hal)->dma_ch,link_addr);\
                                                gdma_ll_tx_start((hal)->dma,(hal)->dma_ch);}while(0)
#define i2s_hal_start_rx_link(hal, link_addr) do{\
                                                  gdma_ll_rx_set_desc_addr((hal)->dma,(hal)->dma_ch,link_addr);\
                                                  gdma_ll_rx_start((hal)->dma,(hal)->dma_ch);}while(0)

#define i2s_hal_stop_tx_link(hal) gdma_ll_tx_stop((hal)->dma,(hal)->dma_ch)
#define i2s_hal_stop_rx_link(hal) gdma_ll_rx_stop((hal)->dma, (hal)->dma_ch)
#define i2s_hal_reset_rxdma(hal)  gdma_ll_rx_reset_channel((hal)->dma, (hal)->dma_ch)
#define i2s_hal_reset_txdma(hal)  gdma_ll_tx_reset_channel((hal)->dma, (hal)->dma_ch)
#define i2s_hal_get_out_eof_des_addr(hal, addr) (*addr = (gdma_ll_tx_get_eof_desc_addr((hal)->dma, (hal)->dma_ch)))
#define i2s_hal_get_in_eof_des_addr(hal, addr) (*addr = (gdma_ll_rx_get_success_eof_desc_addr((hal)->dma, (hal)->dma_ch)))
#endif

#ifdef __cplusplus
}
#endif

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
#include "hal/i2s_ll.h"
#include "hal/i2s_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    i2s_dev_t *dev;
    uint32_t version;
} i2s_hal_context_t;

/**
 * @brief Get I2S interrupt status
 *
 * @param hal Context of the HAL layer
 * @param status interrupt status
 */
#define i2s_hal_get_intr_status(hal, status) i2s_ll_get_intr_status((hal)->dev, status)

/**
 * @brief Clear I2S interrupt status
 *
 * @param hal Context of the HAL layer
 * @param mask interrupt status mask
 */
#define i2s_hal_clear_intr_status(hal, mask) i2s_ll_clear_intr_status((hal)->dev, mask)

/**
 * @brief Get I2S out eof des address
 *
 * @param hal Context of the HAL layer
 * @param addr out eof des address
 */
#define i2s_hal_get_out_eof_des_addr(hal, addr) i2s_ll_get_out_eof_des_addr((hal)->dev, addr)

/**
 * @brief Get I2S in eof des address
 *
 * @param hal Context of the HAL layer
 * @param addr in eof des address
 */
#define i2s_hal_get_in_eof_des_addr(hal, addr) i2s_ll_get_in_eof_des_addr((hal)->dev, addr)

/**
 * @brief Enable I2S rx interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_enable_rx_intr(hal) i2s_ll_enable_rx_intr((hal)->dev)

/**
 * @brief Disable I2S rx interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_disable_rx_intr(hal) i2s_ll_disable_rx_intr((hal)->dev)

/**
 * @brief Disable I2S tx interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_disable_tx_intr(hal) i2s_ll_disable_tx_intr((hal)->dev)

/**
 * @brief Enable I2S tx interrupt
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_enable_tx_intr(hal) i2s_ll_enable_tx_intr((hal)->dev)

/**
 * @brief Set I2S tx mode
 *
 * @param hal Context of the HAL layer
 * @param ch i2s channel
 * @param bits bits per sample
 */
void i2s_hal_set_tx_mode(i2s_hal_context_t *hal, i2s_channel_t ch, i2s_bits_per_sample_t bits);

/**
 * @brief Set I2S rx mode
 *
 * @param hal Context of the HAL layer
 * @param ch i2s channel
 * @param bits bits per sample
 */
void i2s_hal_set_rx_mode(i2s_hal_context_t *hal, i2s_channel_t ch, i2s_bits_per_sample_t bits);

/**
 * @brief Set I2S out link address
 *
 * @param hal Context of the HAL layer
 * @param addr out link address
 */
#define i2s_hal_set_out_link_addr(hal, addr) i2s_ll_set_out_link_addr((hal)->dev, addr)

/**
 * @brief Set I2S out link address
 *
 * @param hal Context of the HAL layer
 * @param addr out link address
 */
#define i2s_hal_set_out_link_addr(hal, addr) i2s_ll_set_out_link_addr((hal)->dev, addr)

/**
 * @brief Set I2S out link address
 *
 * @param hal Context of the HAL layer
 * @param addr out link address
 */
#define i2s_hal_set_out_link_addr(hal, addr) i2s_ll_set_out_link_addr((hal)->dev, addr)

/**
 * @brief Set I2S in link
 *
 * @param hal Context of the HAL layer
 * @param rx_eof_num in link eof num
 * @param addr in link address
 */
void i2s_hal_set_in_link(i2s_hal_context_t *hal, uint32_t rx_eof_num, uint32_t addr);

/**
 * @brief Set I2S clk div
 *
 * @param hal Context of the HAL layer
 * @param div_num i2s clkm div num
 * @param div_a i2s clkm div a
 * @param div_b i2s clkm div b
 * @param tx_bck_div tx bck div num
 * @param rx_bck_div rx bck div num
 */
void i2s_hal_set_clk_div(i2s_hal_context_t *hal, int div_num, int div_a, int div_b, int tx_bck_div, int rx_bck_div);

/**
 * @brief Set I2S clock sel
 *
 * @param hal Context of the HAL layer
 * @param sel clock sel
 */
#define i2s_hal_set_clock_sel(hal, sel) i2s_ll_set_clk_sel((hal)->dev, sel)

/**
 * @brief Set I2S tx bits mod
 *
 * @param hal Context of the HAL layer
 * @param bits bit width per sample.
 */
void i2s_hal_set_tx_bits_mod(i2s_hal_context_t *hal, i2s_bits_per_sample_t bits);

/**
 * @brief Set I2S rx bits mod
 *
 * @param hal Context of the HAL layer
 * @param bits bit width per sample.
 */
void i2s_hal_set_rx_bits_mod(i2s_hal_context_t *hal, i2s_bits_per_sample_t bits);

/**
 * @brief Reset I2S TX & RX module, including DMA and FIFO
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_reset(i2s_hal_context_t *hal);

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
 * @brief Config I2S param
 *
 * @param hal Context of the HAL layer
 * @param i2s_config I2S configurations - see i2s_config_t struct
 */
void i2s_hal_config_param(i2s_hal_context_t *hal, const i2s_config_t *i2s_config);

/**
 * @brief Enable I2S sig loopback
 *
 * @param hal Context of the HAL layer
 */
#define i2s_hal_enable_sig_loopback(hal) i2s_ll_set_sig_loopback((hal)->dev, 1)

/**
 * @brief Enable I2S master mode
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_enable_master_mode(i2s_hal_context_t *hal);

/**
 * @brief Enable I2S slave mode
 *
 * @param hal Context of the HAL layer
 */
void i2s_hal_enable_slave_mode(i2s_hal_context_t *hal);

/**
 * @brief Init the I2S hal and set the I2S to the default configuration. This function should be called first before other hal layer function is called
 *
 * @param hal Context of the HAL layer
 * @param i2s_num The uart port number, the max port number is (I2S_NUM_MAX -1)
 */
void i2s_hal_init(i2s_hal_context_t *hal, int i2s_num);

#if SOC_I2S_SUPPORTS_PDM
/**
 * @brief Set I2S tx pdm
 *
 * @param hal Context of the HAL layer
 * @param fp tx pdm fp
 * @param fs tx pdm fs
 */
void i2s_hal_tx_pdm_cfg(i2s_hal_context_t *hal, uint32_t fp, uint32_t fs);

/**
 * @brief Get I2S tx pdm
 *
 * @param hal Context of the HAL layer
 * @param dsr rx pdm dsr
 */
void i2s_hal_rx_pdm_cfg(i2s_hal_context_t *hal, uint32_t dsr);

/**
 * @brief Get I2S tx pdm configuration
 *
 * @param hal Context of the HAL layer
 * @param fp Pointer to receive tx PDM fp configuration
 * @param fs Pointer to receive tx PDM fs configuration
 */
void i2s_hal_get_tx_pdm(i2s_hal_context_t *hal, uint32_t *fp, uint32_t *fs);

/**
 * @brief Get I2S rx pdm configuration
 *
 * @param hal Context of the HAL layer
 * @param dsr rx pdm dsr
 */
void i2s_hal_get_rx_pdm(i2s_hal_context_t *hal, uint32_t *dsr);
#endif

#ifdef __cplusplus
}
#endif

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

// The HAL layer for AES

#include "hal/aes_hal.h"
#include "hal/aes_ll.h"
#include <stdlib.h>
#include <string.h>
#include "soc/soc_caps.h"

#if SOC_AES_CRYPTO_DMA
#include "soc/crypto_dma_reg.h"
#include "hal/crypto_dma_ll.h"
#elif SOC_AES_GENERAL_DMA
#include "hal/gdma_ll.h"
#include "soc/gdma_channel.h"
#endif

uint8_t aes_hal_setkey(const uint8_t *key, size_t key_bytes, int mode)
{
    aes_ll_set_mode(mode, key_bytes);

    uint8_t key_bytes_in_hardware = aes_ll_write_key(key, key_bytes / 4);
    /* Used for fault injection check: all words of key data should have been written to hardware */
    return key_bytes_in_hardware;
}

/**
 * @brief Busy wait until the AES accelerator is idle
 *
 */
static inline void aes_hal_wait_idle(void)
{
    while (aes_ll_get_state() != ESP_AES_STATE_IDLE) {
    }
}

void aes_hal_transform_block(const void *input_block, void *output_block)
{
    aes_ll_write_block(input_block);
    aes_ll_start_transform();
    aes_hal_wait_idle();
    aes_ll_read_block(output_block);
}

#if SOC_AES_SUPPORT_DMA

#if SOC_AES_GENERAL_DMA
/**
 * @brief Initialize the DMA
 *
 * @param input AES input descriptor (outlink)
 * @param output AES output descriptor (inlink)
 */
static inline void aes_hal_dma_init(const lldesc_t *input, const lldesc_t *output)
{
    /* Update driver when centralized DMA interface implemented, IDF-2192 */
    gdma_ll_tx_enable_descriptor_burst(&GDMA, SOC_GDMA_AES_DMA_CHANNEL, false);
    gdma_ll_tx_enable_data_burst(&GDMA, SOC_GDMA_AES_DMA_CHANNEL, false);
    gdma_ll_rx_enable_descriptor_burst(&GDMA, SOC_GDMA_AES_DMA_CHANNEL, false);
    gdma_ll_rx_enable_data_burst(&GDMA, SOC_GDMA_AES_DMA_CHANNEL, false);

    gdma_ll_tx_connect_to_periph(&GDMA, SOC_GDMA_AES_DMA_CHANNEL, SOC_GDMA_TRIG_PERIPH_AES0);
    gdma_ll_rx_connect_to_periph(&GDMA, SOC_GDMA_AES_DMA_CHANNEL, SOC_GDMA_TRIG_PERIPH_AES0);

#if SOC_GDMA_SUPPORT_EXTMEM
    /* An L2 FIFO bigger than 40 bytes is need when accessing external ram */
    gdma_ll_tx_extend_fifo_size_to(&GDMA, SOC_GDMA_AES_DMA_CHANNEL, 40);
    gdma_ll_rx_extend_l2_fifo_size_to(&GDMA, SOC_GDMA_AES_DMA_CHANNEL, 40);
    gdma_ll_tx_set_block_size_psram(&GDMA, SOC_GDMA_AES_DMA_CHANNEL, GDMA_OUT_EXT_MEM_BK_SIZE_16B);
    gdma_ll_rx_set_block_size_psram(&GDMA, SOC_GDMA_AES_DMA_CHANNEL, GDMA_OUT_EXT_MEM_BK_SIZE_16B);
#endif //SOC_GDMA_SUPPORT_EXTMEM

    /* Set descriptors */
    gdma_ll_tx_set_desc_addr(&GDMA, SOC_GDMA_AES_DMA_CHANNEL, (uint32_t)input);
    gdma_ll_rx_set_desc_addr(&GDMA, SOC_GDMA_AES_DMA_CHANNEL, (uint32_t)output);

    gdma_ll_rx_reset_channel(&GDMA, SOC_GDMA_AES_DMA_CHANNEL);
    gdma_ll_tx_reset_channel(&GDMA, SOC_GDMA_AES_DMA_CHANNEL);

    /* Start transfer */
    gdma_ll_tx_start(&GDMA, SOC_GDMA_AES_DMA_CHANNEL);
    gdma_ll_rx_start(&GDMA, SOC_GDMA_AES_DMA_CHANNEL);
}

static inline bool aes_hal_dma_done(const lldesc_t *output)
{
    return (gdma_ll_rx_is_fsm_idle(&GDMA, SOC_GDMA_AES_DMA_CHANNEL) && (output->owner == 0));
}
#endif //SOC_AES_GENERAL_DMA



#if SOC_AES_CRYPTO_DMA
/**
 * @brief Initialize the DMA
 *
 * @param input AES input descriptor (outlink)
 * @param output AES output descriptor (inlink)
 */
static inline void aes_hal_dma_init(const lldesc_t *input, const lldesc_t *output)
{
    crypto_dma_ll_reset();
    crypto_dma_ll_set_mode(CRYPTO_DMA_AES);

    /* Set descriptors, input to AES comes from outlink DMA and viceversa */
    crypto_dma_ll_outlink_set((uint32_t)input);
    crypto_dma_ll_inlink_set((uint32_t)output);

    /* Start transfer */
    crypto_dma_ll_outlink_start();
    crypto_dma_ll_inlink_start();
}

static inline bool aes_hal_dma_done(lldesc_t *output)
{
    return (crypto_dma_ll_inlink_is_eof() && (output->owner == 0));
}
#endif //SOC_AES_CRYPTO_DMA

void aes_hal_transform_dma_start(const lldesc_t *input, const lldesc_t *output, size_t num_blocks)
{
    aes_ll_dma_enable(true);
    aes_hal_dma_init(input, output);

    /* Write the number of blocks */
    aes_ll_set_num_blocks(num_blocks);

    /* Start encrypting/decrypting */
    aes_ll_start_transform();
}

void aes_hal_transform_dma_finish(void)
{
    aes_ll_dma_exit();
    aes_ll_dma_enable(false);
}

void aes_hal_mode_init(esp_aes_mode_t mode)
{
    /* Set the algorith mode CBC, CFB ... */
    aes_ll_set_block_mode(mode);
    /* Presently hard-coding the INC function to 32 bit */
    if (mode == ESP_AES_BLOCK_MODE_CTR) {
        aes_ll_set_inc();
    }
}

void aes_hal_set_iv(const uint8_t *iv)
{
    aes_ll_set_iv(iv);
}

void aes_hal_read_iv(uint8_t *iv)
{
    aes_ll_read_iv(iv);
}

static inline void aes_hal_wait_done(void)
{
    while (aes_ll_get_state() != ESP_AES_STATE_DONE) {}
}

void aes_hal_wait_dma_done(lldesc_t *output)
{
    /* Checking this if interrupt is used also, to avoid
       issues with AES fault injection
    */
    aes_hal_wait_done();

    /* Wait for DMA write operation to complete */
    while (1) {
        if ( aes_hal_dma_done(output) ) {
            break;
        }
    }
}

#endif //SOC_AES_SUPPORT_DMA

#if SOC_AES_SUPPORT_GCM

void aes_hal_gcm_calc_hash(uint8_t *gcm_hash)
{
    aes_ll_dma_enable(true);
    aes_ll_start_transform();

    aes_hal_wait_idle();

    aes_ll_gcm_read_hash(gcm_hash);
}

void aes_hal_transform_dma_gcm_start(const lldesc_t *input, const lldesc_t *output, size_t num_blocks)
{
    aes_hal_dma_init(input, output);

    /* Write the number of blocks */
    aes_ll_set_num_blocks(num_blocks);

    /* Start encrypting/decrypting */
    aes_ll_cont_transform();
}

void aes_hal_gcm_init(size_t aad_num_blocks, size_t num_valid_bit)
{
    aes_ll_gcm_set_aad_num_blocks(aad_num_blocks);
    aes_ll_gcm_set_num_valid_bit(num_valid_bit);
}

void aes_hal_gcm_read_tag(uint8_t *tag, size_t tag_len)
{
    uint8_t tag_res[TAG_BYTES];
    aes_ll_gcm_read_tag(tag_res);
    memcpy(tag, tag_res, tag_len);
}


#endif //SOC_AES_SUPPORT_GCM

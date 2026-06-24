/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for AES

#include "hal/aes_hal.h"
#include "hal/aes_ll.h"
#include <stdlib.h>
#include <string.h>
#include "soc/soc_caps.h"

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

#ifdef SOC_AES_SUPPORT_PSEUDO_ROUND_FUNCTION
void aes_hal_enable_pseudo_rounds(bool enable, uint8_t base, uint8_t increment, uint8_t key_rng_cnt)
{
    if (aes_ll_is_pseudo_rounds_function_supported()) {
        aes_ll_enable_pseudo_rounds(enable, base, increment, key_rng_cnt);
    }
}
#endif // SOC_AES_SUPPORT_PSEUDO_ROUND_FUNCTION

#if SOC_AES_SUPPORT_DMA

void aes_hal_transform_dma_start(size_t num_blocks)
{
    aes_ll_dma_enable(true);

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
    /* Set the algorithm mode CBC, CFB ... */
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

void aes_hal_wait_done()
{
    while (aes_ll_get_state() != ESP_AES_STATE_DONE) {}
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

void aes_hal_transform_dma_gcm_start(size_t num_blocks)
{

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

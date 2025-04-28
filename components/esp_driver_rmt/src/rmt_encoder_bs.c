/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "driver/rmt_encoder.h"
#include "driver/bitscrambler.h"
#include "rmt_private.h"

typedef struct rmt_bs_encoder_t {
    rmt_encoder_t base;       // the base "class", declares the standard encoder interface
    size_t last_byte_index;   // index of the encoding byte in the primary stream
    bitscrambler_handle_t bs; // BitScrambler handle
} rmt_bs_encoder_t;

RMT_ENCODER_FUNC_ATTR
static esp_err_t rmt_bs_encoder_reset(rmt_encoder_t *encoder)
{
    rmt_bs_encoder_t *bs_encoder = __containerof(encoder, rmt_bs_encoder_t, base);
    bs_encoder->last_byte_index = 0;
    bitscrambler_reset(bs_encoder->bs);
    bitscrambler_start(bs_encoder->bs);
    return ESP_OK;
}

RMT_ENCODER_FUNC_ATTR
static size_t rmt_encode_bs(rmt_encoder_t *encoder, rmt_channel_handle_t channel, const void *input_raw,
                            size_t data_size, rmt_encode_state_t *ret_state)
{
    rmt_bs_encoder_t *bs_encoder = __containerof(encoder, rmt_bs_encoder_t, base);
    rmt_tx_channel_t *tx_chan = __containerof(channel, rmt_tx_channel_t, base);
    uint8_t *input_bytes = (uint8_t *)input_raw;
    rmt_encode_state_t state = RMT_ENCODING_RESET;
    rmt_dma_descriptor_t *desc0 = NULL;
    rmt_dma_descriptor_t *desc1 = NULL;

    // bitscrambler encoder must be used with a TX channel with DMA enabled
    assert(tx_chan->base.dma_chan != NULL);

    size_t byte_index = bs_encoder->last_byte_index;
    // how many bytes will be copied by the encoder
    size_t mem_want = (data_size - byte_index);
    // how many bytes we can save for this round
    size_t mem_have = tx_chan->mem_end * sizeof(rmt_symbol_word_t) - tx_chan->mem_off_bytes;
    // target memory buffer to copy to
    uint8_t *mem_to_nc = (uint8_t*)tx_chan->dma_mem_base_nc;
    // how many bytes will be copied in this round
    size_t copy_len = MIN(mem_want, mem_have);
    bool encoding_truncated = mem_have < mem_want;
    bool encoding_space_free = mem_have > mem_want;

    // mark the start descriptor
    if (tx_chan->mem_off_bytes < tx_chan->ping_pong_symbols * sizeof(rmt_symbol_word_t)) {
        desc0 = &tx_chan->dma_nodes_nc[0];
    } else {
        desc0 = &tx_chan->dma_nodes_nc[1];
    }

    size_t len = copy_len;
    while (len > 0) {
        mem_to_nc[tx_chan->mem_off_bytes++] = input_bytes[byte_index++];
        len--;
    }

    // mark the end descriptor
    if (tx_chan->mem_off_bytes < tx_chan->ping_pong_symbols * sizeof(rmt_symbol_word_t)) {
        desc1 = &tx_chan->dma_nodes_nc[0];
    } else {
        desc1 = &tx_chan->dma_nodes_nc[1];
    }

    // cross line, means desc0 has prepared with sufficient data buffer
    if (desc0 != desc1) {
        desc0->dw0.length = tx_chan->ping_pong_symbols * sizeof(rmt_symbol_word_t);
        desc0->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
    }

    if (encoding_truncated) {
        // this encoding has not finished yet, save the truncated position
        bs_encoder->last_byte_index = byte_index;
    } else {
        // reset internal index if encoding session has finished
        bs_encoder->last_byte_index = 0;
        // bitscrambler program will take care of the EOF marker by itself
        // so we don't rely on the TX driver to inject an EOF marker
        state |= RMT_ENCODING_COMPLETE | RMT_ENCODING_WITH_EOF;
    }

    if (!encoding_space_free) {
        // no more free memory, the caller should yield
        state |= RMT_ENCODING_MEM_FULL;
    }

    // reset offset pointer when exceeds maximum range
    if (tx_chan->mem_off_bytes >= tx_chan->ping_pong_symbols * 2 * sizeof(rmt_symbol_word_t)) {
        desc1->dw0.length = tx_chan->ping_pong_symbols * sizeof(rmt_symbol_word_t);
        desc1->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        tx_chan->mem_off_bytes = 0;
    }

    *ret_state = state;
    return copy_len;
}

static esp_err_t rmt_del_bs_encoder(rmt_encoder_t *encoder)
{
    rmt_bs_encoder_t *bs_encoder = __containerof(encoder, rmt_bs_encoder_t, base);
    ESP_RETURN_ON_ERROR(bitscrambler_disable(bs_encoder->bs), TAG, "disable bitscrambler failed");
    bitscrambler_free(bs_encoder->bs);
    free(bs_encoder);
    return ESP_OK;
}

esp_err_t rmt_new_bitscrambler_encoder(const rmt_bs_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder)
{
    esp_err_t ret = ESP_OK;
    rmt_bs_encoder_t *encoder = NULL;
    ESP_RETURN_ON_FALSE(config && ret_encoder, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    // bitscrambler function is used under RMT TX ISR context
    // if the cache is disabled, all functions called by ISR must be in IRAM
#if CONFIG_RMT_TX_ISR_CACHE_SAFE && !CONFIG_BITSCRAMBLER_CTRL_FUNC_IN_IRAM
    ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_STATE, TAG, "CONFIG_BITSCRAMBLER_CTRL_FUNC_IN_IRAM must be enabled");
#endif
    encoder = rmt_alloc_encoder_mem(sizeof(rmt_bs_encoder_t));
    ESP_GOTO_ON_FALSE(encoder, ESP_ERR_NO_MEM, err, TAG, "no mem for bitscrambler encoder");

    bitscrambler_config_t bs_config = {
        .dir = BITSCRAMBLER_DIR_TX,
        .attach_to = SOC_BITSCRAMBLER_ATTACH_RMT,
    };
    ESP_GOTO_ON_ERROR(bitscrambler_new(&bs_config, &encoder->bs), err, TAG, "create bitscrambler failed");
    ESP_GOTO_ON_ERROR(bitscrambler_enable(encoder->bs), err, TAG, "enable bitscrambler failed");
    // load the bitscrambler program
    ESP_GOTO_ON_ERROR(bitscrambler_load_program(encoder->bs, config->program_bin), err, TAG, "load bitscrambler program failed");

    encoder->base.encode = rmt_encode_bs;
    encoder->base.del = rmt_del_bs_encoder;
    encoder->base.reset = rmt_bs_encoder_reset;
    // return general encoder handle
    *ret_encoder = &encoder->base;
    ESP_LOGD(TAG, "new bitscrambler encoder @%p", encoder);
    return ESP_OK;
err:
    if (encoder) {
        if (encoder->bs) {
            bitscrambler_free(encoder->bs);
        }
    }
    return ret;
}

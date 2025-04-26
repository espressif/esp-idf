/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "driver/rmt_encoder.h"
#include "rmt_private.h"

typedef struct rmt_copy_encoder_t {
    rmt_encoder_t base;       // encoder base class
    size_t last_symbol_index; // index of symbol position in the primary stream
} rmt_copy_encoder_t;

RMT_ENCODER_FUNC_ATTR
static esp_err_t rmt_copy_encoder_reset(rmt_encoder_t *encoder)
{
    rmt_copy_encoder_t *copy_encoder = __containerof(encoder, rmt_copy_encoder_t, base);
    copy_encoder->last_symbol_index = 0;
    return ESP_OK;
}

RMT_ENCODER_FUNC_ATTR
static size_t rmt_encode_copy(rmt_encoder_t *encoder, rmt_channel_handle_t channel,
                              const void *input_symbols, size_t data_size, rmt_encode_state_t *ret_state)
{
    rmt_copy_encoder_t *copy_encoder = __containerof(encoder, rmt_copy_encoder_t, base);
    rmt_tx_channel_t *tx_chan = __containerof(channel, rmt_tx_channel_t, base);
    rmt_symbol_word_t *symbols = (rmt_symbol_word_t *)input_symbols;
    rmt_encode_state_t state = RMT_ENCODING_RESET;
    rmt_dma_descriptor_t *desc0 = NULL;
    rmt_dma_descriptor_t *desc1 = NULL;

    size_t symbol_index = copy_encoder->last_symbol_index;
    // how many symbols will be copied by the encoder
    size_t mem_want = (data_size / 4 - symbol_index);
    // how many symbols we can save for this round
    size_t symbol_off = tx_chan->mem_off_bytes / sizeof(rmt_symbol_word_t);
    size_t mem_have = tx_chan->mem_end - symbol_off;
    // where to put the encoded symbols? DMA buffer or RMT HW memory
    rmt_symbol_word_t *mem_to_nc = NULL;
    if (channel->dma_chan) {
        mem_to_nc = tx_chan->dma_mem_base_nc;
    } else {
        mem_to_nc = channel->hw_mem_base;
    }
    // how many symbols will be encoded in this round
    size_t encode_len = MIN(mem_want, mem_have);
    bool encoding_truncated = mem_have < mem_want;
    bool encoding_space_free = mem_have > mem_want;

    if (channel->dma_chan) {
        // mark the start descriptor
        if (symbol_off < tx_chan->ping_pong_symbols) {
            desc0 = &tx_chan->dma_nodes_nc[0];
        } else {
            desc0 = &tx_chan->dma_nodes_nc[1];
        }
    }

    size_t len = encode_len;
    while (len > 0) {
        mem_to_nc[symbol_off++] = symbols[symbol_index++];
        len--;
    }

    if (channel->dma_chan) {
        // mark the end descriptor
        if (symbol_off < tx_chan->ping_pong_symbols) {
            desc1 = &tx_chan->dma_nodes_nc[0];
        } else {
            desc1 = &tx_chan->dma_nodes_nc[1];
        }

        // cross line, means desc0 has prepared with sufficient data buffer
        if (desc0 != desc1) {
            desc0->dw0.length = tx_chan->ping_pong_symbols * sizeof(rmt_symbol_word_t);
            desc0->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        }
    }

    if (encoding_truncated) {
        // this encoding has not finished yet, save the truncated position
        copy_encoder->last_symbol_index = symbol_index;
    } else {
        // reset internal index if encoding session has finished
        copy_encoder->last_symbol_index = 0;
        state |= RMT_ENCODING_COMPLETE;
    }

    if (!encoding_space_free) {
        // no more free memory, the caller should yield
        state |= RMT_ENCODING_MEM_FULL;
    }

    // reset offset pointer when exceeds maximum range
    if (symbol_off >= tx_chan->ping_pong_symbols * 2) {
        if (channel->dma_chan) {
            desc1->dw0.length = tx_chan->ping_pong_symbols * sizeof(rmt_symbol_word_t);
            desc1->dw0.owner = DMA_DESCRIPTOR_BUFFER_OWNER_DMA;
        }
        tx_chan->mem_off_bytes = 0;
    } else {
        tx_chan->mem_off_bytes = symbol_off * sizeof(rmt_symbol_word_t);
    }

    *ret_state = state;
    return encode_len;
}

static esp_err_t rmt_del_copy_encoder(rmt_encoder_t *encoder)
{
    rmt_copy_encoder_t *copy_encoder = __containerof(encoder, rmt_copy_encoder_t, base);
    free(copy_encoder);
    return ESP_OK;
}

esp_err_t rmt_new_copy_encoder(const rmt_copy_encoder_config_t *config,  rmt_encoder_handle_t *ret_encoder)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(config && ret_encoder, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    rmt_copy_encoder_t *encoder = rmt_alloc_encoder_mem(sizeof(rmt_copy_encoder_t));
    ESP_GOTO_ON_FALSE(encoder, ESP_ERR_NO_MEM, err, TAG, "no mem for copy encoder");
    encoder->base.encode = rmt_encode_copy;
    encoder->base.del = rmt_del_copy_encoder;
    encoder->base.reset = rmt_copy_encoder_reset;
    // return general encoder handle
    *ret_encoder = &encoder->base;
    ESP_LOGD(TAG, "new copy encoder @%p", encoder);
err:
    return ret;
}

/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "driver/rmt_encoder.h"
#include "rmt_private.h"

typedef struct rmt_simple_encoder_t {
    rmt_encoder_t base;       // encoder base class
    size_t last_symbol_index; // index of symbol position in the primary stream
    rmt_encode_simple_cb_t callback;  //callback to call to encode
    void *arg;                // opaque callback argument
    rmt_symbol_word_t *ovf_buf;     //overflow buffer
    size_t ovf_buf_size;            //size, in elements, of overflow buffer
    size_t ovf_buf_fill_len;        //how much actual info the overflow buffer has
    size_t ovf_buf_parsed_pos;      //up to where we moved info from the ovf buf to the rmt
    bool callback_done;      //true if we can't call the callback for more data anymore.
} rmt_simple_encoder_t;

RMT_ENCODER_FUNC_ATTR
static esp_err_t rmt_simple_encoder_reset(rmt_encoder_t *encoder)
{
    rmt_simple_encoder_t *simple_encoder = __containerof(encoder, rmt_simple_encoder_t, base);
    simple_encoder->last_symbol_index = 0;
    simple_encoder->ovf_buf_fill_len = 0;
    simple_encoder->ovf_buf_parsed_pos = 0;
    simple_encoder->callback_done = false;
    return ESP_OK;
}

RMT_ENCODER_FUNC_ATTR
static size_t rmt_encode_simple(rmt_encoder_t *encoder, rmt_channel_handle_t channel,
                                const void *data, size_t data_size, rmt_encode_state_t *ret_state)
{
    rmt_simple_encoder_t *simple_encoder = __containerof(encoder, rmt_simple_encoder_t, base);
    rmt_tx_channel_t *tx_chan = __containerof(channel, rmt_tx_channel_t, base);
    rmt_encode_state_t state = RMT_ENCODING_RESET;
    rmt_dma_descriptor_t *desc0 = NULL;
    rmt_dma_descriptor_t *desc1 = NULL;
    size_t symbol_off = tx_chan->mem_off_bytes / sizeof(rmt_symbol_word_t);

    // where to put the encoded symbols? DMA buffer or RMT HW memory
    rmt_symbol_word_t *mem_to_nc = NULL;
    if (channel->dma_chan) {
        mem_to_nc = tx_chan->dma_mem_base_nc;
    } else {
        mem_to_nc = channel->hw_mem_base;
    }

    if (channel->dma_chan) {
        // mark the start descriptor
        if (symbol_off < tx_chan->ping_pong_symbols) {
            desc0 = &tx_chan->dma_nodes_nc[0];
        } else {
            desc0 = &tx_chan->dma_nodes_nc[1];
        }
    }

    // While we're not done, we need to use the callback to fill the RMT memory until it is
    // exactly entirely full. We cannot do that if the RMT memory still has N free spaces
    // but the encoder callback needs more than N spaces to properly encode a symbol.
    // In order to work around that, if we detect that situation we let the encoder
    // encode into an overflow buffer, then we use the contents of that buffer to fill
    // those last N spaces. On the next call, we will first output the rest of the
    // overflow buffer before again using the callback to continue filling the RMT
    // buffer.

    // Note the next code is in a while loop to properly handle 'unsure' callbacks that
    // e.g. return 0 with a free buffer size of M, but then return less than M symbols
    // when then called with a larger buffer.
    size_t encode_len = 0; //total amount of symbols written to rmt memory
    bool is_done = false;
    while (symbol_off < tx_chan->mem_end) {
        if (simple_encoder->ovf_buf_parsed_pos < simple_encoder->ovf_buf_fill_len) {
            // Overflow buffer has data from the previous encoding call. Copy one entry
            // from that.
            mem_to_nc[symbol_off++] = simple_encoder->ovf_buf[simple_encoder->ovf_buf_parsed_pos++];
            encode_len++;
        } else {
            // Overflow buffer is empty, so we don't need to empty that first.

            if (simple_encoder->callback_done) {
                // We cannot call the callback anymore and the overflow buffer
                // is empty, so we're done with the transaction.
                is_done = true;
                break;
            }
            // Try to have the callback write the data directly into RMT memory.
            size_t enc_size = simple_encoder->callback(data, data_size,
                                                       simple_encoder->last_symbol_index,
                                                       tx_chan->mem_end - symbol_off,
                                                       &mem_to_nc[symbol_off],
                                                       &is_done, simple_encoder->arg);
            encode_len += enc_size;
            symbol_off += enc_size;
            simple_encoder->last_symbol_index += enc_size;
            if (is_done) {
                break;    // we're done, no more data to write to RMT memory.
            }
            if (enc_size == 0) {
                // The encoder does not have enough space in RMT memory to encode its thing,
                // but the RMT memory is not filled out entirely. Encode into the overflow
                // buffer so the next iterations of the loop can fill out the RMT buffer
                // from that.
                enc_size = simple_encoder->callback(data, data_size,
                                                    simple_encoder->last_symbol_index,
                                                    simple_encoder->ovf_buf_size,
                                                    simple_encoder->ovf_buf,
                                                    &is_done, simple_encoder->arg);
                simple_encoder->last_symbol_index += enc_size;
                //Note we do *not* update encode_len here as the data isn't going to the RMT yet.
                simple_encoder->ovf_buf_fill_len = enc_size;
                simple_encoder->ovf_buf_parsed_pos = 0;
                if (is_done) {
                    // If the encoder is done, we cannot call the callback anymore, but we still
                    // need to handle any data in the overflow buffer.
                    simple_encoder->callback_done = true;
                } else {
                    if (enc_size == 0) {
                        //According to the callback docs, this is illegal.
                        //Report this. EARLY_LOGE as we're running from an ISR.
                        ESP_EARLY_LOGE(TAG, "rmt_encoder_simple: encoder callback returned 0 when fed a buffer of config::min_chunk_size!");
                        //Then abort the transaction.
                        is_done = true;
                        break;
                    }
                }
            }
        }
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

    if (is_done) {
        // reset internal index if encoding session has finished
        simple_encoder->last_symbol_index = 0;
        state |= RMT_ENCODING_COMPLETE;
    } else {
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

static esp_err_t rmt_del_simple_encoder(rmt_encoder_t *encoder)
{
    rmt_simple_encoder_t *simple_encoder = __containerof(encoder, rmt_simple_encoder_t, base);
    if (simple_encoder->ovf_buf) {
        free(simple_encoder->ovf_buf);
    }
    free(simple_encoder);
    return ESP_OK;
}

esp_err_t rmt_new_simple_encoder(const rmt_simple_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder)
{
    esp_err_t ret = ESP_OK;
    rmt_simple_encoder_t *encoder = NULL;
    ESP_GOTO_ON_FALSE(config && ret_encoder, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");
    encoder = rmt_alloc_encoder_mem(sizeof(rmt_simple_encoder_t));
    ESP_GOTO_ON_FALSE(encoder, ESP_ERR_NO_MEM, err, TAG, "no mem for simple encoder");
    encoder->base.encode = rmt_encode_simple;
    encoder->base.del = rmt_del_simple_encoder;
    encoder->base.reset = rmt_simple_encoder_reset;
    encoder->callback = config->callback;
    encoder->arg = config->arg;

    size_t min_chunk_size = config->min_chunk_size;
    if (min_chunk_size == 0) {
        min_chunk_size = 64;
    }
    encoder->ovf_buf = rmt_alloc_encoder_mem(min_chunk_size * sizeof(rmt_symbol_word_t));
    ESP_GOTO_ON_FALSE(encoder->ovf_buf, ESP_ERR_NO_MEM, err, TAG, "no mem for simple encoder overflow buffer");
    encoder->ovf_buf_size = min_chunk_size;
    encoder->ovf_buf_fill_len = 0;
    encoder->ovf_buf_parsed_pos = 0;

    // return general encoder handle
    *ret_encoder = &encoder->base;
    ESP_LOGD(TAG, "new simple encoder @%p", encoder);
    return ret;
err:
    if (encoder) {
        free(encoder);
    }
    return ret;
}

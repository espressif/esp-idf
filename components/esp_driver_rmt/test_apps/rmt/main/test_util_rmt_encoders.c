/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>
#include "sdkconfig.h"
#include "unity.h"
#include "driver/rmt_encoder.h"
#include "esp_heap_caps.h"
#include "esp_attr.h"

typedef struct {
    rmt_encoder_t base;
    rmt_encoder_t *bytes_encoder;
    rmt_encoder_t *copy_encoder;
    int state;
    rmt_symbol_word_t reset_code;
} rmt_led_strip_encoder_t;

IRAM_ATTR static size_t rmt_encode_led_strip(rmt_encoder_t *encoder, rmt_channel_handle_t channel, const void *primary_data, size_t data_size, rmt_encode_state_t *ret_state)
{
    rmt_led_strip_encoder_t *led_encoder = __containerof(encoder, rmt_led_strip_encoder_t, base);
    rmt_encode_state_t session_state = RMT_ENCODING_RESET;
    rmt_encode_state_t state = RMT_ENCODING_RESET;
    size_t encoded_symbols = 0;
    switch (led_encoder->state) {
    case 0:
        encoded_symbols += led_encoder->bytes_encoder->encode(led_encoder->bytes_encoder, channel, primary_data, data_size, &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            led_encoder->state = 1; // switch to next state when current encoding session finished
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            state |= RMT_ENCODING_MEM_FULL;
            goto out; // yield if there's no free space for encoding artifacts
        }
    // fall-through
    case 1:
        encoded_symbols += led_encoder->copy_encoder->encode(led_encoder->copy_encoder, channel, &led_encoder->reset_code, sizeof(led_encoder->reset_code), &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            state |= RMT_ENCODING_COMPLETE;
            led_encoder->state = RMT_ENCODING_RESET; // back to the initial encoding session
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            state |= RMT_ENCODING_MEM_FULL;
            goto out; // yield if there's no free space for encoding artifacts
        }
    }
out:
    *ret_state = state;
    return encoded_symbols;
}

static esp_err_t rmt_del_led_strip_encoder(rmt_encoder_t *encoder)
{
    rmt_led_strip_encoder_t *led_encoder = __containerof(encoder, rmt_led_strip_encoder_t, base);
    rmt_del_encoder(led_encoder->bytes_encoder);
    rmt_del_encoder(led_encoder->copy_encoder);
    free(led_encoder);
    return ESP_OK;
}

static esp_err_t rmt_led_strip_encoder_reset(rmt_encoder_t *encoder)
{
    rmt_led_strip_encoder_t *led_encoder = __containerof(encoder, rmt_led_strip_encoder_t, base);
    rmt_encoder_reset(led_encoder->bytes_encoder);
    rmt_encoder_reset(led_encoder->copy_encoder);
    led_encoder->state = RMT_ENCODING_RESET;
    return ESP_OK;
}

esp_err_t test_rmt_new_led_strip_encoder(rmt_encoder_handle_t *ret_encoder)
{
    rmt_led_strip_encoder_t *led_encoder = rmt_alloc_encoder_mem(sizeof(rmt_led_strip_encoder_t));
    led_encoder->base.encode = rmt_encode_led_strip;
    led_encoder->base.del = rmt_del_led_strip_encoder;
    led_encoder->base.reset = rmt_led_strip_encoder_reset;
    // different led strip might have its own timing requirements, following parameter is for WS2812
    rmt_bytes_encoder_config_t bytes_encoder_config = {
        .bit0 = {
            .level0 = 1,
            .duration0 = 3, // T0H=0.3us
            .level1 = 0,
            .duration1 = 9, // T0L=0.9us
        },
        .bit1 = {
            .level0 = 1,
            .duration0 = 9, // T1H=0.9us
            .level1 = 0,
            .duration1 = 3, // T1L=0.3us
        },
        .flags.msb_first = 1 // WS2812 transfer bit order: G7...G0R7...R0B7...B0
    };
    TEST_ESP_OK(rmt_new_bytes_encoder(&bytes_encoder_config, &led_encoder->bytes_encoder));
    rmt_copy_encoder_config_t copy_encoder_config = {};
    TEST_ESP_OK(rmt_new_copy_encoder(&copy_encoder_config, &led_encoder->copy_encoder));
    led_encoder->reset_code = (rmt_symbol_word_t) {
        .level0 = 0,
        .duration0 = 250,
        .level1 = 0,
        .duration1 = 250,
    }; // reset duration defaults to 50us
    *ret_encoder = &led_encoder->base;
    return ESP_OK;
}

typedef struct {
    rmt_encoder_t base;
    rmt_encoder_t *copy_encoder;
    rmt_encoder_t *bytes_encoder;
    int state;
} rmt_nec_protocol_encoder_t;

IRAM_ATTR static size_t rmt_encode_nec_protocol(rmt_encoder_t *encoder, rmt_channel_handle_t channel, const void *primary_data, size_t data_size, rmt_encode_state_t *ret_state)
{
    rmt_nec_protocol_encoder_t *nec_encoder = __containerof(encoder, rmt_nec_protocol_encoder_t, base);
    rmt_encode_state_t session_state = RMT_ENCODING_RESET;
    rmt_encode_state_t state = RMT_ENCODING_RESET;
    size_t encoded_symbols = 0;
    const rmt_symbol_word_t nec_leading_symbol = {
        .level0 = 1,
        .duration0 = 9000,
        .level1 = 0,
        .duration1 = 4500
    };
    const rmt_symbol_word_t nec_ending_symbol = {
        .level0 = 1,
        .duration0 = 560,
        .level1 = 0,
        .duration1 = 0x7FFF
    };
    switch (nec_encoder->state) {
    case 0:
        encoded_symbols += nec_encoder->copy_encoder->encode(nec_encoder->copy_encoder, channel, &nec_leading_symbol, sizeof(nec_leading_symbol), &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            nec_encoder->state = 1; // we can only switch to next state when current encoder finished
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            state |= RMT_ENCODING_MEM_FULL;
            goto out; // yield if there's no free space to put other encoding artifacts
        }
    // fall-through
    case 1:
        encoded_symbols += nec_encoder->bytes_encoder->encode(nec_encoder->bytes_encoder, channel, primary_data, data_size, &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            nec_encoder->state = 2; // we can only switch to next state when current encoder finished
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            state |= RMT_ENCODING_MEM_FULL;
            goto out; // yield if there's no free space to put other encoding artifacts
        }
    // fall-through
    case 2:
        encoded_symbols += nec_encoder->copy_encoder->encode(nec_encoder->copy_encoder, channel, &nec_ending_symbol, sizeof(nec_ending_symbol), &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            state |= RMT_ENCODING_COMPLETE;
            nec_encoder->state = RMT_ENCODING_RESET; // back to the initial encoding session
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            state |= RMT_ENCODING_MEM_FULL;
            goto out; // yield if there's no free space to put other encoding artifacts
        }
    }
out:
    *ret_state = state;
    return encoded_symbols;
}

static esp_err_t rmt_del_nec_protocol_encoder(rmt_encoder_t *encoder)
{
    rmt_nec_protocol_encoder_t *nec_encoder = __containerof(encoder, rmt_nec_protocol_encoder_t, base);
    rmt_del_encoder(nec_encoder->copy_encoder);
    rmt_del_encoder(nec_encoder->bytes_encoder);
    free(nec_encoder);
    return ESP_OK;
}

static esp_err_t rmt_nec_protocol_encoder_reset(rmt_encoder_t *encoder)
{
    rmt_nec_protocol_encoder_t *nec_encoder = __containerof(encoder, rmt_nec_protocol_encoder_t, base);
    rmt_encoder_reset(nec_encoder->copy_encoder);
    rmt_encoder_reset(nec_encoder->bytes_encoder);
    nec_encoder->state = RMT_ENCODING_RESET;
    return ESP_OK;
}

esp_err_t test_rmt_new_nec_protocol_encoder(rmt_encoder_handle_t *ret_encoder)
{
    rmt_nec_protocol_encoder_t *nec_encoder = rmt_alloc_encoder_mem(sizeof(rmt_nec_protocol_encoder_t));
    nec_encoder->base.encode = rmt_encode_nec_protocol;
    nec_encoder->base.del = rmt_del_nec_protocol_encoder;
    nec_encoder->base.reset = rmt_nec_protocol_encoder_reset;

    // different IR protocol might have its own timing requirements, following parameter is for NEC
    rmt_bytes_encoder_config_t bytes_encoder_config = {
        .bit0 = {
            .level0 = 1,
            .duration0 = 560, // T0H=560us
            .level1 = 0,
            .duration1 = 560, // T0L=560us
        },
        .bit1 = {
            .level0 = 1,
            .duration0 = 560, // T1H=560us
            .level1 = 0,
            .duration1 = 1690, // T1L=1690us
        },
    };
    rmt_copy_encoder_config_t copy_encoder_config = {};
    TEST_ESP_OK(rmt_new_copy_encoder(&copy_encoder_config, &nec_encoder->copy_encoder));
    TEST_ESP_OK(rmt_new_bytes_encoder(&bytes_encoder_config, &nec_encoder->bytes_encoder));

    *ret_encoder = &nec_encoder->base;
    return ESP_OK;
}

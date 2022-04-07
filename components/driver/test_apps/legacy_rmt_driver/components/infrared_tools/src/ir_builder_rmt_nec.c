/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sys/cdefs.h>
#include "esp_log.h"
#include "ir_tools.h"
#include "ir_timings.h"
#include "driver/rmt.h"

static const char *TAG = "nec_builder";
#define NEC_CHECK(a, str, goto_tag, ret_value, ...)                               \
    do                                                                            \
    {                                                                             \
        if (!(a))                                                                 \
        {                                                                         \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            ret = ret_value;                                                      \
            goto goto_tag;                                                        \
        }                                                                         \
    } while (0)

typedef struct {
    ir_builder_t parent;
    uint32_t buffer_size;
    uint32_t cursor;
    uint32_t flags;
    uint32_t leading_code_high_ticks;
    uint32_t leading_code_low_ticks;
    uint32_t repeat_code_high_ticks;
    uint32_t repeat_code_low_ticks;
    uint32_t payload_logic0_high_ticks;
    uint32_t payload_logic0_low_ticks;
    uint32_t payload_logic1_high_ticks;
    uint32_t payload_logic1_low_ticks;
    uint32_t ending_code_high_ticks;
    uint32_t ending_code_low_ticks;
    bool inverse;
    rmt_item32_t buffer[0];
} nec_builder_t;

static esp_err_t nec_builder_make_head(ir_builder_t *builder)
{
    nec_builder_t *nec_builder = __containerof(builder, nec_builder_t, parent);
    nec_builder->cursor = 0;
    nec_builder->buffer[nec_builder->cursor].level0 = !nec_builder->inverse;
    nec_builder->buffer[nec_builder->cursor].duration0 = nec_builder->leading_code_high_ticks;
    nec_builder->buffer[nec_builder->cursor].level1 = nec_builder->inverse;
    nec_builder->buffer[nec_builder->cursor].duration1 = nec_builder->leading_code_low_ticks;
    nec_builder->cursor += 1;
    return ESP_OK;
}

static esp_err_t nec_builder_make_logic0(ir_builder_t *builder)
{
    nec_builder_t *nec_builder = __containerof(builder, nec_builder_t, parent);
    nec_builder->buffer[nec_builder->cursor].level0 = !nec_builder->inverse;
    nec_builder->buffer[nec_builder->cursor].duration0 = nec_builder->payload_logic0_high_ticks;
    nec_builder->buffer[nec_builder->cursor].level1 = nec_builder->inverse;
    nec_builder->buffer[nec_builder->cursor].duration1 = nec_builder->payload_logic0_low_ticks;
    nec_builder->cursor += 1;
    return ESP_OK;
}

static esp_err_t nec_builder_make_logic1(ir_builder_t *builder)
{
    nec_builder_t *nec_builder = __containerof(builder, nec_builder_t, parent);
    nec_builder->buffer[nec_builder->cursor].level0 = !nec_builder->inverse;
    nec_builder->buffer[nec_builder->cursor].duration0 = nec_builder->payload_logic1_high_ticks;
    nec_builder->buffer[nec_builder->cursor].level1 = nec_builder->inverse;
    nec_builder->buffer[nec_builder->cursor].duration1 = nec_builder->payload_logic1_low_ticks;
    nec_builder->cursor += 1;
    return ESP_OK;
}

static esp_err_t nec_builder_make_end(ir_builder_t *builder)
{
    nec_builder_t *nec_builder = __containerof(builder, nec_builder_t, parent);
    nec_builder->buffer[nec_builder->cursor].level0 = !nec_builder->inverse;
    nec_builder->buffer[nec_builder->cursor].duration0 = nec_builder->ending_code_high_ticks;
    nec_builder->buffer[nec_builder->cursor].level1 = nec_builder->inverse;
    nec_builder->buffer[nec_builder->cursor].duration1 = nec_builder->ending_code_low_ticks;
    nec_builder->cursor += 1;
    nec_builder->buffer[nec_builder->cursor].val = 0;
    nec_builder->cursor += 1;
    return ESP_OK;
}

static esp_err_t nec_build_frame(ir_builder_t *builder, uint32_t address, uint32_t command)
{
    esp_err_t ret = ESP_OK;
    nec_builder_t *nec_builder = __containerof(builder, nec_builder_t, parent);
    if (!nec_builder->flags & IR_TOOLS_FLAGS_PROTO_EXT) {
        uint8_t low_byte = address & 0xFF;
        uint8_t high_byte = (address >> 8) & 0xFF;
        NEC_CHECK(low_byte == (~high_byte & 0xFF), "address not match standard NEC protocol", err, ESP_ERR_INVALID_ARG);
        low_byte = command & 0xFF;
        high_byte = (command >> 8) & 0xFF;
        NEC_CHECK(low_byte == (~high_byte & 0xFF), "command not match standard NEC protocol", err, ESP_ERR_INVALID_ARG);
    }
    builder->make_head(builder);
    // LSB -> MSB
    for (int i = 0; i < 16; i++) {
        if (address & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    for (int i = 0; i < 16; i++) {
        if (command & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    builder->make_end(builder);
    return ESP_OK;
err:
    return ret;
}

static esp_err_t nec_build_repeat_frame(ir_builder_t *builder)
{
    nec_builder_t *nec_builder = __containerof(builder, nec_builder_t, parent);
    nec_builder->cursor = 0;
    nec_builder->buffer[nec_builder->cursor].level0 = !nec_builder->inverse;
    nec_builder->buffer[nec_builder->cursor].duration0 = nec_builder->repeat_code_high_ticks;
    nec_builder->buffer[nec_builder->cursor].level1 = nec_builder->inverse;
    nec_builder->buffer[nec_builder->cursor].duration1 = nec_builder->repeat_code_low_ticks;
    nec_builder->cursor += 1;
    nec_builder_make_end(builder);
    return ESP_OK;
}

static esp_err_t nec_builder_get_result(ir_builder_t *builder, void *result, size_t *length)
{
    esp_err_t ret = ESP_OK;
    nec_builder_t *nec_builder = __containerof(builder, nec_builder_t, parent);
    NEC_CHECK(result && length, "result and length can't be null", err, ESP_ERR_INVALID_ARG);
    *(rmt_item32_t **)result = nec_builder->buffer;
    *length = nec_builder->cursor;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t nec_builder_del(ir_builder_t *builder)
{
    nec_builder_t *nec_builder = __containerof(builder, nec_builder_t, parent);
    free(nec_builder);
    return ESP_OK;
}

ir_builder_t *ir_builder_rmt_new_nec(const ir_builder_config_t *config)
{
    ir_builder_t *ret = NULL;
    NEC_CHECK(config, "nec configuration can't be null", err, NULL);
    NEC_CHECK(config->buffer_size, "buffer size can't be zero", err, NULL);

    uint32_t builder_size = sizeof(nec_builder_t) + config->buffer_size * sizeof(rmt_item32_t);
    nec_builder_t *nec_builder = calloc(1, builder_size);
    NEC_CHECK(nec_builder, "request memory for nec_builder failed", err, NULL);

    nec_builder->buffer_size = config->buffer_size;
    nec_builder->flags = config->flags;
    if (config->flags & IR_TOOLS_FLAGS_INVERSE) {
        nec_builder->inverse = true;
    }

    uint32_t counter_clk_hz = 0;
    NEC_CHECK(rmt_get_counter_clock((rmt_channel_t)config->dev_hdl, &counter_clk_hz) == ESP_OK,
              "get rmt counter clock failed", err, NULL);
    float ratio = (float)counter_clk_hz / 1e6;
    nec_builder->leading_code_high_ticks = (uint32_t)(ratio * NEC_LEADING_CODE_HIGH_US);
    nec_builder->leading_code_low_ticks = (uint32_t)(ratio * NEC_LEADING_CODE_LOW_US);
    nec_builder->repeat_code_high_ticks = (uint32_t)(ratio * NEC_REPEAT_CODE_HIGH_US);
    nec_builder->repeat_code_low_ticks = (uint32_t)(ratio * NEC_REPEAT_CODE_LOW_US);
    nec_builder->payload_logic0_high_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ZERO_HIGH_US);
    nec_builder->payload_logic0_low_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ZERO_LOW_US);
    nec_builder->payload_logic1_high_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ONE_HIGH_US);
    nec_builder->payload_logic1_low_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ONE_LOW_US);
    nec_builder->ending_code_high_ticks = (uint32_t)(ratio * NEC_ENDING_CODE_HIGH_US);
    nec_builder->ending_code_low_ticks = 0x7FFF;
    nec_builder->parent.make_head = nec_builder_make_head;
    nec_builder->parent.make_logic0 = nec_builder_make_logic0;
    nec_builder->parent.make_logic1 = nec_builder_make_logic1;
    nec_builder->parent.make_end = nec_builder_make_end;
    nec_builder->parent.build_frame = nec_build_frame;
    nec_builder->parent.build_repeat_frame = nec_build_repeat_frame;
    nec_builder->parent.get_result = nec_builder_get_result;
    nec_builder->parent.del = nec_builder_del;
    nec_builder->parent.repeat_period_ms = 110;
    return &nec_builder->parent;
err:
    return ret;
}

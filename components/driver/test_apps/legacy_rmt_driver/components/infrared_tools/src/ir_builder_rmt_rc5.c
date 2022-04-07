/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <sys/cdefs.h>
#include "esp_log.h"
#include "ir_tools.h"
#include "ir_timings.h"
#include "driver/rmt.h"

static const char *TAG = "rc5_builder";
#define RC5_CHECK(a, str, goto_tag, ret_value, ...)                               \
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
    uint32_t pulse_duration_ticks;
    uint32_t flags;
    bool toggle;
    bool s2_bit;
    bool inverse;
    rmt_item32_t buffer[0];
} rc5_builder_t;

static esp_err_t rc5_builder_make_head(ir_builder_t *builder)
{
    rc5_builder_t *rc5_builder = __containerof(builder, rc5_builder_t, parent);
    rc5_builder->cursor = 0;
    rc5_builder->toggle = !rc5_builder->toggle;
    // S1 default (not inverse) is 0
    rc5_builder->buffer[rc5_builder->cursor].level0 = rc5_builder->inverse;
    rc5_builder->buffer[rc5_builder->cursor].duration0 = rc5_builder->pulse_duration_ticks;
    rc5_builder->buffer[rc5_builder->cursor].level1 = !rc5_builder->inverse;
    rc5_builder->buffer[rc5_builder->cursor].duration1 = rc5_builder->pulse_duration_ticks;
    rc5_builder->cursor += 1;
    // S2 default (not inverse) is depend on whether use extended protocol
    rc5_builder->buffer[rc5_builder->cursor].level0 = rc5_builder->s2_bit ^ rc5_builder->inverse;
    rc5_builder->buffer[rc5_builder->cursor].duration0 = rc5_builder->pulse_duration_ticks;
    rc5_builder->buffer[rc5_builder->cursor].level1 = !(rc5_builder->s2_bit ^ rc5_builder->inverse);
    rc5_builder->buffer[rc5_builder->cursor].duration1 = rc5_builder->pulse_duration_ticks;
    rc5_builder->cursor += 1;
    // T
    rc5_builder->buffer[rc5_builder->cursor].level0 = rc5_builder->toggle;
    rc5_builder->buffer[rc5_builder->cursor].duration0 = rc5_builder->pulse_duration_ticks;
    rc5_builder->buffer[rc5_builder->cursor].level1 = !rc5_builder->toggle;
    rc5_builder->buffer[rc5_builder->cursor].duration1 = rc5_builder->pulse_duration_ticks;
    rc5_builder->cursor += 1;
    return ESP_OK;
}

static esp_err_t rc5_builder_make_logic0(ir_builder_t *builder)
{
    rc5_builder_t *rc5_builder = __containerof(builder, rc5_builder_t, parent);
    rc5_builder->buffer[rc5_builder->cursor].level0 = !rc5_builder->inverse;
    rc5_builder->buffer[rc5_builder->cursor].duration0 = rc5_builder->pulse_duration_ticks;
    rc5_builder->buffer[rc5_builder->cursor].level1 = rc5_builder->inverse;
    rc5_builder->buffer[rc5_builder->cursor].duration1 = rc5_builder->pulse_duration_ticks;
    rc5_builder->cursor += 1;
    return ESP_OK;
}

static esp_err_t rc5_builder_make_logic1(ir_builder_t *builder)
{
    rc5_builder_t *rc5_builder = __containerof(builder, rc5_builder_t, parent);
    rc5_builder->buffer[rc5_builder->cursor].level0 = rc5_builder->inverse;
    rc5_builder->buffer[rc5_builder->cursor].duration0 = rc5_builder->pulse_duration_ticks;
    rc5_builder->buffer[rc5_builder->cursor].level1 = !rc5_builder->inverse;
    rc5_builder->buffer[rc5_builder->cursor].duration1 = rc5_builder->pulse_duration_ticks;
    rc5_builder->cursor += 1;
    return ESP_OK;
}

static esp_err_t rc5_builder_make_end(ir_builder_t *builder)
{
    rc5_builder_t *rc5_builder = __containerof(builder, rc5_builder_t, parent);
    rc5_builder->buffer[rc5_builder->cursor].val = 0;
    rc5_builder->cursor += 1;
    return ESP_OK;
}

static esp_err_t rc5_build_frame(ir_builder_t *builder, uint32_t address, uint32_t command)
{
    rc5_builder_t *rc5_builder = __containerof(builder, rc5_builder_t, parent);
    if (rc5_builder->flags & IR_TOOLS_FLAGS_PROTO_EXT) {
        // RC5-extended protocol uses S2 bit as a 7th command bit (MSB of a command)
        if (command > 63) {
            rc5_builder->s2_bit = true;
        } else {
            rc5_builder->s2_bit = false;
        }
    }
    builder->make_head(builder);
    // MSB -> LSB
    for (int i = 4; i >= 0; i--) {
        if (address & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    for (int i = 5; i >= 0; i--) {
        if (command & (1 << i)) {
            builder->make_logic1(builder);
        } else {
            builder->make_logic0(builder);
        }
    }
    builder->make_end(builder);
    return ESP_OK;
}

static esp_err_t rc5_build_repeat_frame(ir_builder_t *builder)
{
    // repeat frame is just the latest build frame, so do nothing here
    return ESP_OK;
}

static esp_err_t rc5_builder_get_result(ir_builder_t *builder, void *result, size_t *length)
{
    esp_err_t ret = ESP_OK;
    rc5_builder_t *rc5_builder = __containerof(builder, rc5_builder_t, parent);
    RC5_CHECK(result && length, "result and length can't be null", err, ESP_ERR_INVALID_ARG);
    *(rmt_item32_t **)result = rc5_builder->buffer;
    *length = rc5_builder->cursor;
    return ESP_OK;
err:
    return ret;
}

static esp_err_t rc5_builder_del(ir_builder_t *builder)
{
    rc5_builder_t *rc5_builder = __containerof(builder, rc5_builder_t, parent);
    free(rc5_builder);
    return ESP_OK;
}

ir_builder_t *ir_builder_rmt_new_rc5(const ir_builder_config_t *config)
{
    ir_builder_t *ret = NULL;
    RC5_CHECK(config, "rc5 configuration can't be null", err, NULL);
    RC5_CHECK(config->buffer_size, "buffer size can't be zero", err, NULL);

    uint32_t builder_size = sizeof(rc5_builder_t) + config->buffer_size * sizeof(rmt_item32_t);
    rc5_builder_t *rc5_builder = calloc(1, builder_size);
    RC5_CHECK(rc5_builder, "request memory for rc5_builder failed", err, NULL);

    rc5_builder->buffer_size = config->buffer_size;
    rc5_builder->flags = config->flags;
    if (config->flags & IR_TOOLS_FLAGS_INVERSE) {
        rc5_builder->inverse = true;
    }

    uint32_t counter_clk_hz = 0;
    RC5_CHECK(rmt_get_counter_clock((rmt_channel_t)config->dev_hdl, &counter_clk_hz) == ESP_OK,
              "get rmt counter clock failed", err, NULL);
    float ratio = (float)counter_clk_hz / 1e6;
    rc5_builder->pulse_duration_ticks = (uint32_t)(ratio * RC5_PULSE_DURATION_US);
    rc5_builder->parent.make_head = rc5_builder_make_head;
    rc5_builder->parent.make_logic0 = rc5_builder_make_logic0;
    rc5_builder->parent.make_logic1 = rc5_builder_make_logic1;
    rc5_builder->parent.make_end = rc5_builder_make_end;
    rc5_builder->parent.build_frame = rc5_build_frame;
    rc5_builder->parent.build_repeat_frame = rc5_build_repeat_frame;
    rc5_builder->parent.get_result = rc5_builder_get_result;
    rc5_builder->parent.del = rc5_builder_del;
    rc5_builder->parent.repeat_period_ms = 114;
    return &rc5_builder->parent;
err:
    return ret;
}

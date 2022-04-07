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

static const char *TAG = "rc5_parser";
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

#define RC5_MAX_FRAME_RMT_WORDS (14) // S1+S2+T+ADDR(5)+CMD(6)

typedef struct {
    ir_parser_t parent;
    uint32_t flags;
    uint32_t pulse_duration_ticks;
    uint32_t margin_ticks;
    rmt_item32_t *buffer;
    uint32_t buffer_len;
    uint32_t last_command;
    uint32_t last_address;
    bool last_t_bit;
} rc5_parser_t;

static inline bool rc5_check_in_range(uint32_t raw_ticks, uint32_t target_ticks, uint32_t margin_ticks)
{
    return (raw_ticks < (target_ticks + margin_ticks)) && (raw_ticks > (target_ticks - margin_ticks));
}

static esp_err_t rc5_parser_input(ir_parser_t *parser, void *raw_data, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    rc5_parser_t *rc5_parser = __containerof(parser, rc5_parser_t, parent);
    rc5_parser->buffer = raw_data;
    rc5_parser->buffer_len = length;
    if (length > RC5_MAX_FRAME_RMT_WORDS) {
        ret = ESP_FAIL;
    }
    return ret;
}

static inline bool rc5_duration_one_unit(rc5_parser_t *rc5_parser, uint32_t duration)
{
    return (duration < (rc5_parser->pulse_duration_ticks + rc5_parser->margin_ticks)) &&
           (duration > (rc5_parser->pulse_duration_ticks - rc5_parser->margin_ticks));
}

static inline bool rc5_duration_two_unit(rc5_parser_t *rc5_parser, uint32_t duration)
{
    return (duration < (rc5_parser->pulse_duration_ticks * 2 + rc5_parser->margin_ticks)) &&
           (duration > (rc5_parser->pulse_duration_ticks * 2 - rc5_parser->margin_ticks));
}

static esp_err_t rc5_parser_get_scan_code(ir_parser_t *parser, uint32_t *address, uint32_t *command, bool *repeat)
{
    esp_err_t ret = ESP_FAIL;
    uint32_t parse_result = 0; // 32 bit is enough to hold the parse result of one RC5 frame
    uint32_t addr = 0;
    uint32_t cmd = 0;
    bool s1 = true;
    bool s2 = true;
    bool t = false;
    bool exchange = false;
    rc5_parser_t *rc5_parser = __containerof(parser, rc5_parser_t, parent);
    RC5_CHECK(address && command && repeat, "address, command and repeat can't be null", out, ESP_ERR_INVALID_ARG);
    for (int i = 0; i < rc5_parser->buffer_len; i++) {
        if (rc5_duration_one_unit(rc5_parser, rc5_parser->buffer[i].duration0)) {
            parse_result <<= 1;
            parse_result |= exchange;
            if (rc5_duration_two_unit(rc5_parser, rc5_parser->buffer[i].duration1)) {
                exchange = !exchange;
            }
        } else if (rc5_duration_two_unit(rc5_parser, rc5_parser->buffer[i].duration0)) {
            parse_result <<= 1;
            parse_result |= rc5_parser->buffer[i].level0;
            parse_result <<= 1;
            parse_result |= !rc5_parser->buffer[i].level0;
            if (rc5_duration_one_unit(rc5_parser, rc5_parser->buffer[i].duration1)) {
                exchange = !exchange;
            }
        } else {
            goto out;
        }
    }
    if (!(rc5_parser->flags & IR_TOOLS_FLAGS_INVERSE)) {
        parse_result = ~parse_result;
    }
    s1 = ((parse_result & 0x2000) >> 13) & 0x01;
    s2 = ((parse_result & 0x1000) >> 12) & 0x01;
    t = ((parse_result & 0x800) >> 11) & 0x01;
    // Check S1, must be 1
    if (s1) {
        if (!(rc5_parser->flags & IR_TOOLS_FLAGS_PROTO_EXT) && !s2) {
            // Not standard RC5 protocol, but S2 is 0
            goto out;
        }
        addr = (parse_result & 0x7C0) >> 6;
        cmd = (parse_result & 0x3F);
        if (!s2) {
            cmd |= 1 << 6;
        }
        *repeat = (t == rc5_parser->last_t_bit && addr == rc5_parser->last_address && cmd == rc5_parser->last_command);
        *address = addr;
        *command = cmd;
        rc5_parser->last_address = addr;
        rc5_parser->last_command = cmd;
        rc5_parser->last_t_bit = t;
        ret = ESP_OK;
    }
out:
    return ret;
}

static esp_err_t rc5_parser_del(ir_parser_t *parser)
{
    rc5_parser_t *rc5_parser = __containerof(parser, rc5_parser_t, parent);
    free(rc5_parser);
    return ESP_OK;
}

ir_parser_t *ir_parser_rmt_new_rc5(const ir_parser_config_t *config)
{
    ir_parser_t *ret = NULL;
    RC5_CHECK(config, "rc5 configuration can't be null", err, NULL);

    rc5_parser_t *rc5_parser = calloc(1, sizeof(rc5_parser_t));
    RC5_CHECK(rc5_parser, "request memory for rc5_parser failed", err, NULL);

    rc5_parser->flags = config->flags;

    uint32_t counter_clk_hz = 0;
    RC5_CHECK(rmt_get_counter_clock((rmt_channel_t)config->dev_hdl, &counter_clk_hz) == ESP_OK,
              "get rmt counter clock failed", err, NULL);
    float ratio = (float)counter_clk_hz / 1e6;
    rc5_parser->pulse_duration_ticks = (uint32_t)(ratio * RC5_PULSE_DURATION_US);
    rc5_parser->margin_ticks = (uint32_t)(ratio * config->margin_us);
    rc5_parser->parent.input = rc5_parser_input;
    rc5_parser->parent.get_scan_code = rc5_parser_get_scan_code;
    rc5_parser->parent.del = rc5_parser_del;
    return &rc5_parser->parent;
err:
    return ret;
}

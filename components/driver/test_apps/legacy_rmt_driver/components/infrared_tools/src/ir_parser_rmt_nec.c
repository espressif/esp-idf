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

static const char *TAG = "nec_parser";
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

#define NEC_DATA_FRAME_RMT_WORDS (34)
#define NEC_REPEAT_FRAME_RMT_WORDS (2)

typedef struct {
    ir_parser_t parent;
    uint32_t flags;
    uint32_t leading_code_high_ticks;
    uint32_t leading_code_low_ticks;
    uint32_t repeat_code_high_ticks;
    uint32_t repeat_code_low_ticks;
    uint32_t payload_logic0_high_ticks;
    uint32_t payload_logic0_low_ticks;
    uint32_t payload_logic1_high_ticks;
    uint32_t payload_logic1_low_ticks;
    uint32_t margin_ticks;
    rmt_item32_t *buffer;
    uint32_t cursor;
    uint32_t last_address;
    uint32_t last_command;
    bool repeat;
    bool inverse;
} nec_parser_t;

static inline bool nec_check_in_range(uint32_t raw_ticks, uint32_t target_ticks, uint32_t margin_ticks)
{
    return (raw_ticks < (target_ticks + margin_ticks)) && (raw_ticks > (target_ticks - margin_ticks));
}

static bool nec_parse_head(nec_parser_t *nec_parser)
{
    nec_parser->cursor = 0;
    rmt_item32_t item = nec_parser->buffer[nec_parser->cursor];
    bool ret = (item.level0 == nec_parser->inverse) && (item.level1 != nec_parser->inverse) &&
               nec_check_in_range(item.duration0, nec_parser->leading_code_high_ticks, nec_parser->margin_ticks) &&
               nec_check_in_range(item.duration1, nec_parser->leading_code_low_ticks, nec_parser->margin_ticks);
    nec_parser->cursor += 1;
    return ret;
}

static bool nec_parse_logic0(nec_parser_t *nec_parser)
{
    rmt_item32_t item = nec_parser->buffer[nec_parser->cursor];
    bool ret = (item.level0 == nec_parser->inverse) && (item.level1 != nec_parser->inverse) &&
               nec_check_in_range(item.duration0, nec_parser->payload_logic0_high_ticks, nec_parser->margin_ticks) &&
               nec_check_in_range(item.duration1, nec_parser->payload_logic0_low_ticks, nec_parser->margin_ticks);
    return ret;
}

static bool nec_parse_logic1(nec_parser_t *nec_parser)
{
    rmt_item32_t item = nec_parser->buffer[nec_parser->cursor];
    bool ret = (item.level0 == nec_parser->inverse) && (item.level1 != nec_parser->inverse) &&
               nec_check_in_range(item.duration0, nec_parser->payload_logic1_high_ticks, nec_parser->margin_ticks) &&
               nec_check_in_range(item.duration1, nec_parser->payload_logic1_low_ticks, nec_parser->margin_ticks);
    return ret;
}

static esp_err_t nec_parse_logic(ir_parser_t *parser, bool *logic)
{
    esp_err_t ret = ESP_FAIL;
    bool logic_value = false;
    nec_parser_t *nec_parser = __containerof(parser, nec_parser_t, parent);
    if (nec_parse_logic0(nec_parser)) {
        logic_value = false;
        ret = ESP_OK;
    } else if (nec_parse_logic1(nec_parser)) {
        logic_value = true;
        ret = ESP_OK;
    }
    if (ret == ESP_OK) {
        *logic = logic_value;
    }
    nec_parser->cursor += 1;
    return ret;
}

static bool nec_parse_repeat_frame(nec_parser_t *nec_parser)
{
    nec_parser->cursor = 0;
    rmt_item32_t item = nec_parser->buffer[nec_parser->cursor];
    bool ret = (item.level0 == nec_parser->inverse) && (item.level1 != nec_parser->inverse) &&
               nec_check_in_range(item.duration0, nec_parser->repeat_code_high_ticks, nec_parser->margin_ticks) &&
               nec_check_in_range(item.duration1, nec_parser->repeat_code_low_ticks, nec_parser->margin_ticks);
    nec_parser->cursor += 1;
    return ret;
}

static esp_err_t nec_parser_input(ir_parser_t *parser, void *raw_data, uint32_t length)
{
    esp_err_t ret = ESP_OK;
    nec_parser_t *nec_parser = __containerof(parser, nec_parser_t, parent);
    NEC_CHECK(raw_data, "input data can't be null", err, ESP_ERR_INVALID_ARG);
    nec_parser->buffer = raw_data;
    // Data Frame costs 34 items and Repeat Frame costs 2 items
    if (length == NEC_DATA_FRAME_RMT_WORDS) {
        nec_parser->repeat = false;
    } else if (length == NEC_REPEAT_FRAME_RMT_WORDS) {
        nec_parser->repeat = true;
    } else {
        ret = ESP_FAIL;
    }
    return ret;
err:
    return ret;
}

static esp_err_t nec_parser_get_scan_code(ir_parser_t *parser, uint32_t *address, uint32_t *command, bool *repeat)
{
    esp_err_t ret = ESP_FAIL;
    uint32_t addr = 0;
    uint32_t cmd = 0;
    bool logic_value = false;
    nec_parser_t *nec_parser = __containerof(parser, nec_parser_t, parent);
    NEC_CHECK(address && command && repeat, "address, command and repeat can't be null", out, ESP_ERR_INVALID_ARG);
    if (nec_parser->repeat) {
        if (nec_parse_repeat_frame(nec_parser)) {
            *address = nec_parser->last_address;
            *command = nec_parser->last_command;
            *repeat = true;
            ret = ESP_OK;
        }
    } else {
        if (nec_parse_head(nec_parser)) {
            for (int i = 0; i < 16; i++) {
                if (nec_parse_logic(parser, &logic_value) == ESP_OK) {
                    addr |= (logic_value << i);
                }
            }
            for (int i = 0; i < 16; i++) {
                if (nec_parse_logic(parser, &logic_value) == ESP_OK) {
                    cmd |= (logic_value << i);
                }
            }
            *address = addr;
            *command = cmd;
            *repeat = false;
            // keep it as potential repeat code
            nec_parser->last_address = addr;
            nec_parser->last_command = cmd;
            ret = ESP_OK;
        }
    }
out:
    return ret;
}

static esp_err_t nec_parser_del(ir_parser_t *parser)
{
    nec_parser_t *nec_parser = __containerof(parser, nec_parser_t, parent);
    free(nec_parser);
    return ESP_OK;
}

ir_parser_t *ir_parser_rmt_new_nec(const ir_parser_config_t *config)
{
    ir_parser_t *ret = NULL;
    NEC_CHECK(config, "nec configuration can't be null", err, NULL);

    nec_parser_t *nec_parser = calloc(1, sizeof(nec_parser_t));
    NEC_CHECK(nec_parser, "request memory for nec_parser failed", err, NULL);

    nec_parser->flags = config->flags;
    if (config->flags & IR_TOOLS_FLAGS_INVERSE) {
        nec_parser->inverse = true;
    }

    uint32_t counter_clk_hz = 0;
    NEC_CHECK(rmt_get_counter_clock((rmt_channel_t)config->dev_hdl, &counter_clk_hz) == ESP_OK,
              "get rmt counter clock failed", err, NULL);
    float ratio = (float)counter_clk_hz / 1e6;
    nec_parser->leading_code_high_ticks = (uint32_t)(ratio * NEC_LEADING_CODE_HIGH_US);
    nec_parser->leading_code_low_ticks = (uint32_t)(ratio * NEC_LEADING_CODE_LOW_US);
    nec_parser->repeat_code_high_ticks = (uint32_t)(ratio * NEC_REPEAT_CODE_HIGH_US);
    nec_parser->repeat_code_low_ticks = (uint32_t)(ratio * NEC_REPEAT_CODE_LOW_US);
    nec_parser->payload_logic0_high_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ZERO_HIGH_US);
    nec_parser->payload_logic0_low_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ZERO_LOW_US);
    nec_parser->payload_logic1_high_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ONE_HIGH_US);
    nec_parser->payload_logic1_low_ticks = (uint32_t)(ratio * NEC_PAYLOAD_ONE_LOW_US);
    nec_parser->margin_ticks = (uint32_t)(ratio * config->margin_us);
    nec_parser->parent.input = nec_parser_input;
    nec_parser->parent.get_scan_code = nec_parser_get_scan_code;
    nec_parser->parent.del = nec_parser_del;
    return &nec_parser->parent;
err:
    return ret;
}

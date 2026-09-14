/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "sdkconfig.h"
#include "hal/mcpwm_ll.h"
#include "esp_rom_sys.h"
#include "driver/mcpwm_cap.h"
#include "driver/mcpwm_sync.h"
#include "driver/gpio.h"
#include "test_mcpwm_utils.h"

TEST_CASE("mcpwm_capture_install_uninstall", "[mcpwm]")
{
    printf("install mcpwm capture timers\r\n");
    mcpwm_capture_timer_config_t cap_timer_config = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
    };
    int total_cap_timers = MCPWM_LL_GET(GROUP_NUM) * MCPWM_LL_GET(CAPTURE_TIMERS_PER_GROUP);
    mcpwm_cap_timer_handle_t cap_timers[total_cap_timers];
    int k = 0;
    for (int i = 0; i < MCPWM_LL_GET(GROUP_NUM); i++) {
        cap_timer_config.group_id = i;
        for (int j = 0; j < MCPWM_LL_GET(CAPTURE_TIMERS_PER_GROUP); j++) {
            TEST_ESP_OK(mcpwm_new_capture_timer(&cap_timer_config, &cap_timers[k++]));
        }
        TEST_ESP_ERR(ESP_ERR_NOT_FOUND, mcpwm_new_capture_timer(&cap_timer_config, &cap_timers[0]));
    }

    printf("install mcpwm capture channels\r\n");
    mcpwm_capture_channel_config_t cap_chan_config = {
        .gpio_num = TEST_CAP_GPIO,
        .prescale = 2,
        .flags.pos_edge = true,
    };
    mcpwm_cap_channel_handle_t cap_channels[total_cap_timers][MCPWM_LL_GET(CAPTURE_CHANNELS_PER_TIMER)];
    for (int i = 0; i < total_cap_timers; i++) {
        for (int j = 0; j < MCPWM_LL_GET(CAPTURE_CHANNELS_PER_TIMER); j++) {
            TEST_ESP_OK(mcpwm_new_capture_channel(cap_timers[i], &cap_chan_config, &cap_channels[i][j]));
        }
        TEST_ESP_ERR(ESP_ERR_NOT_FOUND, mcpwm_new_capture_channel(cap_timers[i], &cap_chan_config, &cap_channels[i][0]));
    }

    printf("uninstall mcpwm capture channels and timers\r\n");
    for (int i = 0; i < total_cap_timers; i++) {
        for (int j = 0; j < MCPWM_LL_GET(CAPTURE_CHANNELS_PER_TIMER); j++) {
            TEST_ESP_OK(mcpwm_del_capture_channel(cap_channels[i][j]));
        }
        TEST_ESP_OK(mcpwm_del_capture_timer(cap_timers[i]));
    }
}

TEST_MCPWM_CALLBACK_ATTR
static bool test_capture_callback(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_data_t *edata, void *user_data)
{
    uint32_t *cap_value = (uint32_t *)user_data;
    if (edata->cap_edge == MCPWM_CAP_EDGE_NEG) {
        cap_value[1] = edata->cap_value;
    } else {
        cap_value[0] = edata->cap_value;
    }
    return false;
}

TEST_CASE("mcpwm_capture_ext_gpio", "[mcpwm]")
{
    printf("init a gpio to simulate the external capture signal\r\n");
    const int cap_gpio = TEST_CAP_GPIO;
    gpio_config_t ext_gpio_conf = {
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = BIT(cap_gpio),
    };
    TEST_ESP_OK(gpio_config(&ext_gpio_conf));

    printf("install mcpwm capture timer\r\n");
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    mcpwm_capture_timer_config_t cap_timer_config = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
        .group_id = 0,
        .resolution_hz = 8 * 1000 * 1000,
    };
    TEST_ESP_OK(mcpwm_new_capture_timer(&cap_timer_config, &cap_timer));

    // put the GPIO into a preset state
    gpio_set_level(cap_gpio, 0);

    printf("install mcpwm capture channel\r\n");
    mcpwm_cap_channel_handle_t pps_channel;
    mcpwm_capture_channel_config_t cap_chan_config = {
        .gpio_num = cap_gpio,
        .prescale = 1,
        .flags.pos_edge = true,
        .flags.neg_edge = true,
    };
    TEST_ESP_OK(mcpwm_new_capture_channel(cap_timer, &cap_chan_config, &pps_channel));

    printf("install callback for capture channel\r\n");
    mcpwm_capture_event_callbacks_t cbs = {
        .on_cap = test_capture_callback,
    };
    uint32_t cap_value[2] = {0};
    TEST_ESP_OK(mcpwm_capture_channel_register_event_callbacks(pps_channel, &cbs, cap_value));

    printf("enable capture channel\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_enable(pps_channel));

    printf("check input function before starting capture\r\n");
    gpio_set_level(cap_gpio, 1);
    TEST_ASSERT_EQUAL(1, gpio_get_level(cap_gpio));
    gpio_set_level(cap_gpio, 0);
    TEST_ASSERT_EQUAL(0, gpio_get_level(cap_gpio));

    printf("enable and start capture timer\r\n");
    TEST_ESP_OK(mcpwm_capture_timer_enable(cap_timer));
    TEST_ESP_OK(mcpwm_capture_timer_start(cap_timer));

    printf("simulate GPIO capture signal\r\n");
    gpio_set_level(cap_gpio, 1);
    esp_rom_delay_us(10 * 1000);
    gpio_set_level(cap_gpio, 0);
    esp_rom_delay_us(10 * 1000);
    printf("capture value: Pos=%"PRIu32", Neg=%"PRIu32"\r\n", cap_value[0], cap_value[1]);
    uint32_t clk_src_res;
    TEST_ESP_OK(mcpwm_capture_timer_get_resolution(cap_timer, &clk_src_res));
    clk_src_res /= 1000; // convert to kHz
    printf("timer resolution:%"PRIu32"KHz\r\n", clk_src_res);
    TEST_ASSERT_UINT_WITHIN(1000, 10000, (cap_value[1] - cap_value[0]) * 1000 / clk_src_res);

    printf("uninstall capture channel and timer\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_disable(pps_channel));
    TEST_ESP_OK(mcpwm_del_capture_channel(pps_channel));
    TEST_ESP_OK(mcpwm_capture_timer_disable(cap_timer));
    TEST_ESP_OK(mcpwm_del_capture_timer(cap_timer));

    printf("check input function after removing capture\r\n");
    gpio_set_level(cap_gpio, 1);
    TEST_ASSERT_EQUAL(1, gpio_get_level(cap_gpio));
    gpio_set_level(cap_gpio, 0);
    TEST_ASSERT_EQUAL(0, gpio_get_level(cap_gpio));

    TEST_ESP_OK(gpio_reset_pin(cap_gpio));
}

typedef struct {
    uint32_t cap_data[2];
    int cap_data_index;
} test_soft_catch_user_data_t;

TEST_MCPWM_CALLBACK_ATTR
static bool soft_cap_callback(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_data_t *data, void *user_data)
{
    test_soft_catch_user_data_t *cbdata = (test_soft_catch_user_data_t *)user_data;
    cbdata->cap_data[cbdata->cap_data_index++] = data->cap_value;
    return false;
}

TEST_CASE("mcpwm_capture_software_catch", "[mcpwm]")
{
    printf("install mcpwm capture timer\r\n");
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    mcpwm_capture_timer_config_t cap_timer_config = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
        .group_id = 0,
    };
    TEST_ESP_OK(mcpwm_new_capture_timer(&cap_timer_config, &cap_timer));

    printf("install mcpwm capture channel\r\n");
    mcpwm_cap_channel_handle_t cap_channel = NULL;
    mcpwm_capture_channel_config_t cap_chan_config = {
        .gpio_num = -1, // don't need any GPIO, we use software to trigger a catch
        .prescale = 2,
    };
    test_soft_catch_user_data_t test_callback_data = {};
    TEST_ESP_OK(mcpwm_new_capture_channel(cap_timer, &cap_chan_config, &cap_channel));

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_capture_channel_trigger_soft_catch(cap_channel));

    printf("register event callback for capture channel\r\n");
    mcpwm_capture_event_callbacks_t cbs = {
        .on_cap = soft_cap_callback,
    };
    TEST_ESP_OK(mcpwm_capture_channel_register_event_callbacks(cap_channel, &cbs, &test_callback_data));

    printf("enable capture channel\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_enable(cap_channel));

    printf("enable and start capture timer\r\n");
    TEST_ESP_OK(mcpwm_capture_timer_enable(cap_timer));
    TEST_ESP_OK(mcpwm_capture_timer_start(cap_timer));

    printf("trigger software catch\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_trigger_soft_catch(cap_channel));
    esp_rom_delay_us(10 * 1000);
    TEST_ESP_OK(mcpwm_capture_channel_trigger_soft_catch(cap_channel));
    esp_rom_delay_us(10 * 1000);
    // check user data
    TEST_ASSERT_EQUAL(2, test_callback_data.cap_data_index);
    uint32_t delta = test_callback_data.cap_data[1] - test_callback_data.cap_data[0];
    esp_rom_printf("duration=%" PRIu32 " ticks\r\n", delta);
    uint32_t clk_src_res;
    TEST_ESP_OK(mcpwm_capture_timer_get_resolution(cap_timer, &clk_src_res));
    clk_src_res /= 1000; // convert to kHz
    TEST_ASSERT_UINT_WITHIN(1000, 10000, delta * 1000 / clk_src_res);

    printf("uninstall capture channel and timer\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_disable(cap_channel));
    TEST_ESP_OK(mcpwm_capture_timer_disable(cap_timer));
    TEST_ESP_OK(mcpwm_del_capture_channel(cap_channel));
    TEST_ESP_OK(mcpwm_del_capture_timer(cap_timer));
}

TEST_MCPWM_CALLBACK_ATTR
static bool test_capture_after_sync_callback(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_data_t *data, void *user_data)
{
    uint32_t *cap_data = (uint32_t *)user_data;
    *cap_data = data->cap_value;
    return false;
}

TEST_CASE("mcpwm_capture_timer_sync_phase_lock", "[mcpwm]")
{
    mcpwm_capture_timer_config_t cap_timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
    };
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    TEST_ESP_OK(mcpwm_new_capture_timer(&cap_timer_config, &cap_timer));

    mcpwm_sync_handle_t soft_sync = NULL;
    mcpwm_soft_sync_config_t soft_sync_config = {};
    TEST_ESP_OK(mcpwm_new_soft_sync_src(&soft_sync_config, &soft_sync));

    mcpwm_capture_timer_sync_phase_config_t sync_config = {
        .count_value = 1000,
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .sync_src = soft_sync,
    };
    TEST_ESP_OK(mcpwm_capture_timer_set_phase_on_sync(cap_timer, &sync_config));

    mcpwm_cap_channel_handle_t cap_channel = NULL;
    mcpwm_capture_channel_config_t cap_chan_config = {
        .gpio_num = -1, // don't need any GPIO, we use software to trigger a catch
        .prescale = 1,
    };
    TEST_ESP_OK(mcpwm_new_capture_channel(cap_timer, &cap_chan_config, &cap_channel));

    mcpwm_capture_event_callbacks_t cbs = {
        .on_cap = test_capture_after_sync_callback,
    };
    uint32_t cap_data;
    TEST_ESP_OK(mcpwm_capture_channel_register_event_callbacks(cap_channel, &cbs, &cap_data));

    printf("enable capture channel\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_enable(cap_channel));

    TEST_ESP_OK(mcpwm_capture_channel_trigger_soft_catch(cap_channel));
    vTaskDelay(pdMS_TO_TICKS(10));
    printf("capture data before sync: %"PRIu32"\r\n", cap_data);

    TEST_ESP_OK(mcpwm_soft_sync_activate(soft_sync));
    TEST_ESP_OK(mcpwm_capture_channel_trigger_soft_catch(cap_channel));
    vTaskDelay(pdMS_TO_TICKS(10));
    printf("capture data after sync: %"PRIu32"\r\n", cap_data);
    TEST_ASSERT_EQUAL(1000, cap_data);
    TEST_ESP_OK(mcpwm_capture_channel_disable(cap_channel));
    TEST_ESP_OK(mcpwm_del_capture_channel(cap_channel));
    TEST_ESP_OK(mcpwm_del_capture_timer(cap_timer));
    TEST_ESP_OK(mcpwm_del_sync_src(soft_sync));
}

/**
 * Capture input prescale: step GPIO high/low and see which edges fire a capture.
 * Frequency/pulse-width do not matter; only edge -> capture mapping does.
 *
 * For each cycle: drive rising edge, check; drive falling edge, check.
 * Do enough cycles to observe prescale phase + spacing (prescale * GROUPS).
 */
#define TEST_PRESCALE_GROUPS   8
#define TEST_PRESCALE_SETTLE_US 50
#define TEST_PRESCALE_MAX_CAPS  64

typedef struct {
    uint32_t count;
    mcpwm_capture_edge_t edges[TEST_PRESCALE_MAX_CAPS];
    uint32_t cycles[TEST_PRESCALE_MAX_CAPS];
} test_prescale_ctx_t;

TEST_MCPWM_CALLBACK_ATTR
static bool test_prescale_cap_cb(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_data_t *edata, void *user_data)
{
    test_prescale_ctx_t *ctx = user_data;
    if (ctx->count < TEST_PRESCALE_MAX_CAPS) {
        ctx->edges[ctx->count++] = edata->cap_edge;
    }
    return false;
}

// Drive one edge, wait for ISR, return true if a new capture arrived.
static bool test_prescale_step(int gpio, int level, test_prescale_ctx_t *ctx,
                               uint32_t cycle, mcpwm_capture_edge_t *out_edge)
{
    uint32_t before = ctx->count;
    gpio_set_level(gpio, level);
    esp_rom_delay_us(TEST_PRESCALE_SETTLE_US);
    if (ctx->count == before) {
        return false;
    }
    ctx->cycles[before] = cycle;
    if (out_edge) {
        *out_edge = ctx->edges[before];
    }
    return true;
}

static void test_prescale_check(int gpio, uint32_t prescale, bool pos_edge, bool neg_edge)
{
    const bool both = pos_edge && neg_edge;
    // Enough full high/low cycles to get GROUPS captures per enabled edge type
    const uint32_t cycles = prescale * TEST_PRESCALE_GROUPS;
    const uint32_t expect = both ? (2 * TEST_PRESCALE_GROUPS) : TEST_PRESCALE_GROUPS;

    // Recreate timer+channel every case so group register reset clears prior prescale phase.
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    mcpwm_capture_timer_config_t cap_cfg = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
        .group_id = 0,
    };
    TEST_ESP_OK(mcpwm_new_capture_timer(&cap_cfg, &cap_timer));
    TEST_ESP_OK(mcpwm_capture_timer_enable(cap_timer));
    TEST_ESP_OK(mcpwm_capture_timer_start(cap_timer));

    mcpwm_cap_channel_handle_t chan = NULL;
    mcpwm_capture_channel_config_t cfg = {
        .gpio_num = gpio,
        .prescale = prescale,
        .flags.pos_edge = pos_edge,
        .flags.neg_edge = neg_edge,
    };
    test_prescale_ctx_t ctx = {};
    mcpwm_capture_event_callbacks_t cbs = {.on_cap = test_prescale_cap_cb};

    // Always start low so every mode drives the same rise-then-fall sequence.
    gpio_set_level(gpio, 0);

    TEST_ESP_OK(mcpwm_new_capture_channel(cap_timer, &cfg, &chan));
    TEST_ESP_OK(mcpwm_capture_channel_register_event_callbacks(chan, &cbs, &ctx));
    TEST_ESP_OK(mcpwm_capture_channel_enable(chan));

    printf("prescale=%" PRIu32 " mode=%s cycles=%" PRIu32 " expect_caps=%" PRIu32 "\n",
           prescale, both ? "both" : (pos_edge ? "pos" : "neg"), cycles, expect);

    uint32_t rise_caps = 0;
    uint32_t fall_caps = 0;
    int first_cap_cycle = -1;
    const char *first_cap_via = NULL;

    for (uint32_t c = 0; c < cycles; c++) {
        mcpwm_capture_edge_t edge;
        if (test_prescale_step(gpio, 1, &ctx, c, &edge)) {
            const char *name = (edge == MCPWM_CAP_EDGE_POS) ? "R" : "F";
            printf("  cycle%-2" PRIu32 " rise -> %s\n", c, name);
            if (edge == MCPWM_CAP_EDGE_POS) {
                rise_caps++;
            } else {
                fall_caps++;
            }
            if (first_cap_cycle < 0) {
                first_cap_cycle = (int)c;
                first_cap_via = "rise";
            }
        }
        if (test_prescale_step(gpio, 0, &ctx, c, &edge)) {
            const char *name = (edge == MCPWM_CAP_EDGE_POS) ? "R" : "F";
            printf("  cycle%-2" PRIu32 " fall -> %s\n", c, name);
            if (edge == MCPWM_CAP_EDGE_POS) {
                rise_caps++;
            } else {
                fall_caps++;
            }
            if (first_cap_cycle < 0) {
                first_cap_cycle = (int)c;
                first_cap_via = "fall";
            }
        }
    }

    printf("  caps=%" PRIu32 " (R-reported=%" PRIu32 " F-reported=%" PRIu32 ") first@cycle%d via %s\n",
           ctx.count, rise_caps, fall_caps, first_cap_cycle, first_cap_via ? first_cap_via : "?");

    TEST_ASSERT_EQUAL_UINT32(expect, ctx.count);
    // After a clean timer+channel recreate, cadence and first-cycle phase are both fixed.
    const uint32_t same_gap = (prescale <= 1) ? 1 : (both ? (prescale / 2) : prescale);
    if (both && (prescale <= 1)) {
        for (uint32_t i = 2; i < ctx.count; i++) {
            TEST_ASSERT_EQUAL_UINT32(1, ctx.cycles[i] - ctx.cycles[i - 2]);
        }
    } else {
        for (uint32_t i = 1; i < ctx.count; i++) {
            TEST_ASSERT_EQUAL_UINT32(same_gap, ctx.cycles[i] - ctx.cycles[i - 1]);
        }
    }

    if (pos_edge && !neg_edge) {
        // Rising-only: always reports R, fires on rising GPIO steps.
        // After clean reset: first at cycle (prescale/2 - 1) when prescale > 1.
        TEST_ASSERT_EQUAL_UINT32(expect, rise_caps);
        TEST_ASSERT_EQUAL_UINT32(0, fall_caps);
        TEST_ASSERT_NOT_NULL(first_cap_via);
        TEST_ASSERT_EQUAL_STRING("rise", first_cap_via);
        TEST_ASSERT_EQUAL_INT((prescale <= 1) ? 0 : (int)(prescale / 2 - 1), first_cap_cycle);
    } else if (neg_edge && !pos_edge) {
        // Falling-only path always reports F (cap_edge neg).
        // Bypass: fires on GPIO falling steps at cycle 0.
        // Prescale > 1: fires on GPIO rising steps, first at cycle (prescale - 1).
        TEST_ASSERT_EQUAL_UINT32(expect, fall_caps);
        TEST_ASSERT_EQUAL_UINT32(0, rise_caps);
        TEST_ASSERT_NOT_NULL(first_cap_via);
        if (prescale <= 1) {
            TEST_ASSERT_EQUAL_STRING("fall", first_cap_via);
            TEST_ASSERT_EQUAL_INT(0, first_cap_cycle);
        } else {
            TEST_ASSERT_EQUAL_STRING("rise", first_cap_via);
            TEST_ASSERT_EQUAL_INT((int)(prescale - 1), first_cap_cycle);
        }
    } else {
        // Both edges: equal R/F counts. Bypass maps to real rise/fall.
        // Prescale > 1: rising steps only, alternates R/F from R; first like rising-only.
        TEST_ASSERT_EQUAL_UINT32(TEST_PRESCALE_GROUPS, rise_caps);
        TEST_ASSERT_EQUAL_UINT32(TEST_PRESCALE_GROUPS, fall_caps);
        TEST_ASSERT_NOT_NULL(first_cap_via);
        TEST_ASSERT_EQUAL_STRING("rise", first_cap_via);
        TEST_ASSERT_EQUAL(MCPWM_CAP_EDGE_POS, ctx.edges[0]);
        TEST_ASSERT_EQUAL_INT((prescale <= 1) ? 0 : (int)(prescale / 2 - 1), first_cap_cycle);
    }

    TEST_ESP_OK(mcpwm_capture_channel_disable(chan));
    TEST_ESP_OK(mcpwm_del_capture_channel(chan));
    TEST_ESP_OK(mcpwm_capture_timer_stop(cap_timer));
    TEST_ESP_OK(mcpwm_capture_timer_disable(cap_timer));
    TEST_ESP_OK(mcpwm_del_capture_timer(cap_timer));
}

TEST_CASE("mcpwm_capture_prescale_ratio", "[mcpwm]")
{
    const int gpio = TEST_CAP_GPIO;
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = BIT(gpio),
    };
    TEST_ESP_OK(gpio_config(&io_conf));
    gpio_set_level(gpio, 0);

    // Odd API prescale must be rejected (uses a throwaway timer).
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    mcpwm_capture_timer_config_t cap_cfg = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
        .group_id = 0,
    };
    TEST_ESP_OK(mcpwm_new_capture_timer(&cap_cfg, &cap_timer));
    mcpwm_cap_channel_handle_t bad = NULL;
    mcpwm_capture_channel_config_t bad_cfg = {
        .gpio_num = gpio, .prescale = 3, .flags.pos_edge = true,
    };
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, mcpwm_new_capture_channel(cap_timer, &bad_cfg, &bad));
    TEST_ESP_OK(mcpwm_del_capture_timer(cap_timer));

    const uint32_t expected_prescales[] = {1, 2, 4, 16};
    for (size_t i = 0; i < sizeof(expected_prescales) / sizeof(expected_prescales[0]); i++) {
        printf("------------------------------------------------\r\n");
        test_prescale_check(gpio, expected_prescales[i], true, false);
        test_prescale_check(gpio, expected_prescales[i], false, true);
        test_prescale_check(gpio, expected_prescales[i], true, true);
    }

    TEST_ESP_OK(gpio_reset_pin(gpio));
}

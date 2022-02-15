/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// RMT driver unit test is based on extended NEC protocol
// Please don't use channel number: SOC_RMT_CHANNELS_NUM - 1
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "ir_tools.h"
#include "unity.h"
#include "test_utils.h"

// CI ONLY: Don't connect any other signals to this GPIO
#define RMT_DATA_IO (0) // bind signal RMT_SIG_OUT0_IDX and RMT_SIG_IN0_IDX on the same GPIO

#define RMT_TESTBENCH_FLAGS_ALWAYS_ON (1<<0)
#define RMT_TESTBENCH_FLAGS_CARRIER_ON (1<<1)
#define RMT_TESTBENCH_FLAGS_LOOP_ON (1<<2)

static const char *TAG = "RMT.test";
static ir_builder_t *s_ir_builder = NULL;
static ir_parser_t *s_ir_parser = NULL;

static void rmt_setup_testbench(int tx_channel, int rx_channel, uint32_t flags)
{
    // RMT channel configuration
    if (tx_channel >= 0) {
        rmt_config_t tx_config = RMT_DEFAULT_CONFIG_TX(RMT_DATA_IO, tx_channel);
        if (flags & RMT_TESTBENCH_FLAGS_ALWAYS_ON) {
            tx_config.flags |= RMT_CHANNEL_FLAGS_ALWAYS_ON;
        }
        if (flags & RMT_TESTBENCH_FLAGS_CARRIER_ON) {
            tx_config.tx_config.carrier_en = true;
        }
#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
        if (flags & RMT_TESTBENCH_FLAGS_LOOP_ON) {
            tx_config.tx_config.loop_en = true;
            tx_config.tx_config.loop_count = 10;
        }
#endif
        TEST_ESP_OK(rmt_config(&tx_config));
    }

    if (rx_channel >= 0) {
        rmt_config_t rx_config = RMT_DEFAULT_CONFIG_RX(RMT_DATA_IO, rx_channel);
        if (flags & RMT_TESTBENCH_FLAGS_ALWAYS_ON) {
            rx_config.flags |= RMT_CHANNEL_FLAGS_ALWAYS_ON;
        }
#if SOC_RMT_SUPPORT_RX_DEMODULATION
        if (flags & RMT_TESTBENCH_FLAGS_CARRIER_ON) {
            rx_config.rx_config.rm_carrier = true;
            rx_config.rx_config.carrier_freq_hz = 38000;
            rx_config.rx_config.carrier_duty_percent = 33;
            rx_config.rx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
        }
#endif
        TEST_ESP_OK(rmt_config(&rx_config));
    }

    // Routing internal signals by IO Matrix (bind rmt tx and rx signal on the same GPIO)
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[RMT_DATA_IO], PIN_FUNC_GPIO);
    TEST_ESP_OK(gpio_set_direction(RMT_DATA_IO, GPIO_MODE_INPUT_OUTPUT));
    gpio_matrix_out(RMT_DATA_IO, RMT_SIG_OUT0_IDX + tx_channel, 0, 0);
    gpio_matrix_in(RMT_DATA_IO, RMT_SIG_IN0_IDX + rx_channel, 0);

    // install driver
    if (tx_channel >= 0) {
        TEST_ESP_OK(rmt_driver_install(tx_channel, 0, 0));

        ir_builder_config_t ir_builder_config = IR_BUILDER_DEFAULT_CONFIG((ir_dev_t)tx_channel);
        ir_builder_config.flags = IR_TOOLS_FLAGS_PROTO_EXT;
        s_ir_builder = ir_builder_rmt_new_nec(&ir_builder_config);
        TEST_ASSERT_NOT_NULL(s_ir_builder);
    }

    if (rx_channel >= 0) {
        TEST_ESP_OK(rmt_driver_install(rx_channel, 3000, 0));

        ir_parser_config_t ir_parser_config = IR_PARSER_DEFAULT_CONFIG((ir_dev_t)rx_channel);
        ir_parser_config.flags = IR_TOOLS_FLAGS_PROTO_EXT | IR_TOOLS_FLAGS_INVERSE;
        s_ir_parser = ir_parser_rmt_new_nec(&ir_parser_config);
        TEST_ASSERT_NOT_NULL(s_ir_parser);
    }
}

static void rmt_clean_testbench(int tx_channel, int rx_channel)
{
    if (tx_channel >= 0) {
        TEST_ESP_OK(rmt_driver_uninstall(tx_channel));
        TEST_ESP_OK(s_ir_builder->del(s_ir_builder));
        s_ir_builder = NULL;
    }

    if (rx_channel >= 0) {
        TEST_ESP_OK(rmt_driver_uninstall(rx_channel));
        TEST_ESP_OK(s_ir_parser->del(s_ir_parser));
        s_ir_parser = NULL;
    }
}

TEST_CASE("RMT wrong configuration", "[rmt][error]")
{
    rmt_config_t correct_config = RMT_DEFAULT_CONFIG_TX(RMT_DATA_IO, 0);
    rmt_config_t wrong_config = correct_config;

    wrong_config.clk_div = 0;
    TEST_ASSERT(rmt_config(&wrong_config) == ESP_ERR_INVALID_ARG);

    wrong_config = correct_config;
    wrong_config.channel = SOC_RMT_CHANNELS_NUM;
    TEST_ASSERT(rmt_config(&wrong_config) == ESP_ERR_INVALID_ARG);

    wrong_config = correct_config;
    wrong_config.channel = 2;
    wrong_config.mem_block_num = 8;
    TEST_ASSERT(rmt_config(&wrong_config) == ESP_ERR_INVALID_ARG);
    TEST_ASSERT(rmt_set_mem_block_num(wrong_config.channel, -1) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("RMT miscellaneous functions", "[rmt]")
{
    rmt_channel_t channel = SOC_RMT_CHANNELS_NUM - 2;
    uint8_t div_cnt;
    rmt_source_clk_t src_clk;
    uint8_t memNum;
    uint16_t idle_thres;
    rmt_mem_owner_t owner;

    // TX related functions
    rmt_setup_testbench(channel, -1, 0);

    TEST_ESP_OK(rmt_set_mem_block_num(channel, 2));
    TEST_ESP_OK(rmt_get_mem_block_num(channel, &memNum));
    TEST_ASSERT_EQUAL_UINT8(2, memNum);

    TEST_ESP_OK(rmt_set_clk_div(channel, 160));
    TEST_ESP_OK(rmt_get_clk_div(channel, &div_cnt));
    TEST_ASSERT_EQUAL_UINT8(160, div_cnt);

    TEST_ESP_OK(rmt_set_source_clk(channel, RMT_BASECLK_REF));
    TEST_ESP_OK(rmt_get_source_clk(channel, &src_clk));
    TEST_ASSERT_EQUAL_INT(RMT_BASECLK_REF, src_clk);

    TEST_ESP_OK(rmt_set_memory_owner(channel, RMT_MEM_OWNER_RX));
    TEST_ESP_OK(rmt_get_memory_owner(channel, &owner));
    TEST_ASSERT_EQUAL_INT(RMT_MEM_OWNER_RX, owner);

    TEST_ESP_OK(rmt_set_tx_carrier(channel, 0, 1, 0, 1));
    TEST_ESP_OK(rmt_set_idle_level(channel, 1, 0));

    rmt_clean_testbench(channel, -1);

    // RX related functions
    rmt_setup_testbench(-1, channel, 0);

    TEST_ESP_OK(rmt_set_rx_idle_thresh(channel, 200));
    TEST_ESP_OK(rmt_get_rx_idle_thresh(channel, &idle_thres));
    TEST_ASSERT_EQUAL_UINT16(200, idle_thres);

    TEST_ESP_OK(rmt_set_rx_filter(channel, 1, 100));

    rmt_clean_testbench(-1, channel);
}

TEST_CASE("RMT multiple channels", "[rmt]")
{
    rmt_config_t tx_cfg1 = RMT_DEFAULT_CONFIG_TX(RMT_DATA_IO, 0);

    TEST_ESP_OK(rmt_config(&tx_cfg1));
    TEST_ESP_OK(rmt_driver_install(tx_cfg1.channel, 0, 0));

    rmt_config_t tx_cfg2 = RMT_DEFAULT_CONFIG_TX(RMT_DATA_IO, 1);
    TEST_ESP_OK(rmt_config(&tx_cfg2));
    TEST_ESP_OK(rmt_driver_install(tx_cfg2.channel, 0, 0));

    rmt_config_t tx_cfg3 = RMT_DEFAULT_CONFIG_TX(RMT_DATA_IO, 2);
    TEST_ESP_OK(rmt_config(&tx_cfg3));
    TEST_ESP_OK(rmt_driver_install(tx_cfg3.channel, 0, 0));

    TEST_ESP_OK(rmt_driver_uninstall(2));
    TEST_ESP_OK(rmt_driver_uninstall(1));
    TEST_ESP_OK(rmt_driver_uninstall(0));
}

TEST_CASE("RMT install/uninstall test", "[rmt][pressure]")
{
    rmt_config_t rx_cfg = RMT_DEFAULT_CONFIG_TX(RMT_DATA_IO, SOC_RMT_CHANNELS_NUM - 2);
    TEST_ESP_OK(rmt_config(&rx_cfg));
    for (int i = 0; i < 100; i++) {
        TEST_ESP_OK(rmt_driver_install(rx_cfg.channel, 1000, 0));
        TEST_ESP_OK(rmt_driver_uninstall(rx_cfg.channel));
    }
}

static void do_nec_tx_rx(uint32_t flags)
{
    RingbufHandle_t rb = NULL;
    rmt_item32_t *items = NULL;
    uint32_t length = 0;
    uint32_t addr = 0x10;
    uint32_t cmd = 0x20;
    bool repeat = false;
    int tx_channel = 0;
    int rx_channel = 1;

    // test on different flags combinations
    rmt_setup_testbench(tx_channel, rx_channel, flags);

    // get ready to receive
    TEST_ESP_OK(rmt_get_ringbuf_handle(rx_channel, &rb));
    TEST_ASSERT_NOT_NULL(rb);
    TEST_ESP_OK(rmt_rx_start(rx_channel, true));

    vTaskDelay(pdMS_TO_TICKS(1000));

    // build NEC codes
    cmd = 0x20;
    while (cmd <= 0x30) {
        ESP_LOGI(TAG, "Send command 0x%x to address 0x%x", cmd, addr);
        // Send new key code
        TEST_ESP_OK(s_ir_builder->build_frame(s_ir_builder, addr, cmd));
        TEST_ESP_OK(s_ir_builder->get_result(s_ir_builder, &items, &length));
        if (cmd & 0x01) {
            TEST_ESP_OK(rmt_write_items(tx_channel, items, length, false)); // no wait
            TEST_ESP_OK(rmt_wait_tx_done(tx_channel, portMAX_DELAY));
        } else {
            TEST_ESP_OK(rmt_write_items(tx_channel, items, length, true)); // wait until done
        }
        cmd++;
    }

    // parse NEC codes
    while (rb) {
        items = (rmt_item32_t *) xRingbufferReceive(rb, &length, 1000);
        if (items) {
            length /= 4; // one RMT = 4 Bytes
            if (s_ir_parser->input(s_ir_parser, items, length) == ESP_OK) {
                if (s_ir_parser->get_scan_code(s_ir_parser, &addr, &cmd, &repeat) == ESP_OK) {
                    ESP_LOGI(TAG, "Scan Code %s --- addr: 0x%04x cmd: 0x%04x", repeat ? "(repeat)" : "", addr, cmd);
                }
            }
            vRingbufferReturnItem(rb, (void *) items);
        } else {
            ESP_LOGI(TAG, "done");
            break;
        }
    }

    TEST_ASSERT_EQUAL(0x30, cmd);
    rmt_clean_testbench(tx_channel, rx_channel);
}

// basic nec tx and rx test, using APB source clock, no modulation
TEST_CASE("RMT NEC TX and RX (APB)", "[rmt]")
{
    do_nec_tx_rx(0);
}

// test with RMT_TESTBENCH_FLAGS_ALWAYS_ON will take a long time (REF_TICK is much slower than APB CLOCK)
TEST_CASE("RMT NEC TX and RX (REF_TICK)", "[rmt][timeout=240]")
{
    do_nec_tx_rx(RMT_TESTBENCH_FLAGS_ALWAYS_ON);
}

#if SOC_RMT_SUPPORT_RX_DEMODULATION
// basic nec tx and rx test, using APB source clock, with modulation and demodulation on
TEST_CASE("RMT NEC TX and RX (Modulation/Demodulation)", "[rmt]")
{
    do_nec_tx_rx(RMT_TESTBENCH_FLAGS_CARRIER_ON);
}
#endif

TEST_CASE("RMT TX (SOC_RMT_CHANNEL_MEM_WORDS-1) symbols", "[rmt][boundary]")
{
    int tx_channel = 0;
    rmt_setup_testbench(tx_channel, -1, 0);
    rmt_item32_t *items = malloc(sizeof(rmt_item32_t) * (SOC_RMT_CHANNEL_MEM_WORDS - 1));
    for (int i = 0; i < SOC_RMT_CHANNEL_MEM_WORDS - 1; i++) {
        items[i] = (rmt_item32_t) {
            {{
                    200, 1, 200, 0
                }
            }
        };
    }
    TEST_ESP_OK(rmt_write_items(tx_channel, items, SOC_RMT_CHANNEL_MEM_WORDS - 1, 1));
    free(items);
    rmt_clean_testbench(tx_channel, -1);
}

TEST_CASE("RMT TX stop", "[rmt]")
{
    RingbufHandle_t rb = NULL;
    rmt_item32_t *frames = NULL;
    uint32_t length = 0;
    uint32_t count = 10;
    uint32_t addr = 0x10;
    uint32_t cmd = 0x20;
    bool repeat = false;
    int tx_channel = 0;
    int rx_channel = 1;

    rmt_setup_testbench(tx_channel, rx_channel, 0);

    // re-install ir_builder, to enlarge internal buffer size
    TEST_ESP_OK(s_ir_builder->del(s_ir_builder));
    ir_builder_config_t ir_builder_config = IR_BUILDER_DEFAULT_CONFIG((ir_dev_t)tx_channel);
    ir_builder_config.buffer_size *= count;
    ir_builder_config.flags = IR_TOOLS_FLAGS_PROTO_EXT;
    s_ir_builder = ir_builder_rmt_new_nec(&ir_builder_config);
    TEST_ASSERT_NOT_NULL(s_ir_builder);

    // get ready to receive
    TEST_ESP_OK(rmt_get_ringbuf_handle(rx_channel, &rb));
    TEST_ASSERT_NOT_NULL(rb);
    TEST_ESP_OK(rmt_rx_start(rx_channel, true));

    vTaskDelay(pdMS_TO_TICKS(1000));

    // build NEC codes
    ESP_LOGI(TAG, "Plan to send command 0x%x~0x%x to address 0x%x", cmd, cmd + count, addr);
    for (int i = 0; i <= count; i++) {
        TEST_ESP_OK(s_ir_builder->build_frame(s_ir_builder, addr, cmd));
        cmd++;
    }
    TEST_ESP_OK(s_ir_builder->get_result(s_ir_builder, &frames, &length));

    // send for 1 second and then stop
    TEST_ESP_OK(rmt_write_items(tx_channel, frames, length, true));
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ESP_OK(rmt_tx_stop(tx_channel));

    // parse NEC codes
    uint32_t num = 0;
    while (rb) {
        frames = (rmt_item32_t *) xRingbufferReceive(rb, &length, 1000);
        if (frames) {
            length /= 4; // one RMT = 4 Bytes
            if (s_ir_parser->input(s_ir_parser, frames, length) == ESP_OK) {
                if (s_ir_parser->get_scan_code(s_ir_parser, &addr, &cmd, &repeat) == ESP_OK) {
                    ESP_LOGI(TAG, "Scan Code %s --- addr: 0x%04x cmd: 0x%04x", repeat ? "(repeat)" : "", addr, cmd);
                    num++;
                }
            }
            vRingbufferReturnItem(rb, (void *) frames);
        } else {
            ESP_LOGI(TAG, "done");
            break;
        }
    }

    TEST_ASSERT(num < count);
    rmt_clean_testbench(tx_channel, rx_channel);
}

#if SOC_RMT_SUPPORT_RX_PINGPONG
TEST_CASE("RMT Ping-Pong operation", "[rmt]")
{
    int tx_channel = 0;
    int rx_channel = 1;
    rmt_item32_t frames[SOC_RMT_CHANNEL_MEM_WORDS * 2]; // send two block data using ping-pong
    RingbufHandle_t rb = NULL;
    uint32_t size = sizeof(frames) / sizeof(frames[0]);

    // The design of the following test frame should trigger three rx threshold interrupt and one rx end interrupt
    int i = 0;
    for (i = 0; i < size - 1; i++) {
        frames[i].level0 = 1;
        frames[i].duration0 = 100;
        frames[i].level1 = 0;
        frames[i].duration1 = 100;
    }
    frames[i].level0 = 1;
    frames[i].duration0 = 0;
    frames[i].level1 = 0;
    frames[i].duration1 = 0;

    rmt_setup_testbench(tx_channel, rx_channel, 0);

    // get ready to receive
    TEST_ESP_OK(rmt_get_ringbuf_handle(rx_channel, &rb));
    TEST_ASSERT_NOT_NULL(rb);
    TEST_ESP_OK(rmt_rx_start(rx_channel, true));

    vTaskDelay(pdMS_TO_TICKS(1000));

    for (uint32_t test_count = 0; test_count < 5; test_count++) {
        TEST_ESP_OK(rmt_write_items(tx_channel, frames, size, true));

        // parse received data
        uint32_t length = 0;
        rmt_item32_t *items = (rmt_item32_t *) xRingbufferReceive(rb, &length, 1000);
        if (items) {
            vRingbufferReturnItem(rb, (void *) items);
        }
        TEST_ASSERT_EQUAL(4 * (size - 1), length);
    }

    rmt_clean_testbench(tx_channel, rx_channel);
}
#endif
#if SOC_RMT_SUPPORT_TX_GROUP
static uint32_t tx_end_time0, tx_end_time1;
static void rmt_tx_end_cb(rmt_channel_t channel, void *arg)
{
    if (channel == 0) {
        tx_end_time0 = esp_cpu_get_ccount();
    } else {
        tx_end_time1 = esp_cpu_get_ccount();
    }
}
TEST_CASE("RMT TX simultaneously", "[rmt]")
{
    rmt_item32_t frames[SOC_RMT_CHANNEL_MEM_WORDS];
    uint32_t size = sizeof(frames) / sizeof(frames[0]);
    int channel0 = 0;
    int channel1 = 1;

    int i = 0;
    for (i = 0; i < size - 1; i++) {
        frames[i].level0 = 1;
        frames[i].duration0 = 1000;
        frames[i].level1 = 0;
        frames[i].duration1 = 1000;
    }
    frames[i].level0 = 0;
    frames[i].duration0 = 0;
    frames[i].level1 = 0;
    frames[i].duration1 = 0;

    rmt_config_t tx_config0 = RMT_DEFAULT_CONFIG_TX(12, channel0);
    rmt_config_t tx_config1 = RMT_DEFAULT_CONFIG_TX(13, channel1);
    TEST_ESP_OK(rmt_config(&tx_config0));
    TEST_ESP_OK(rmt_config(&tx_config1));

    TEST_ESP_OK(rmt_driver_install(channel0, 0, 0));
    TEST_ESP_OK(rmt_driver_install(channel1, 0, 0));

    rmt_register_tx_end_callback(rmt_tx_end_cb, NULL);

    TEST_ESP_OK(rmt_add_channel_to_group(channel0));
    TEST_ESP_OK(rmt_add_channel_to_group(channel1));

    TEST_ESP_OK(rmt_write_items(channel0, frames, size, false));
    vTaskDelay(pdMS_TO_TICKS(1000));
    TEST_ESP_OK(rmt_write_items(channel1, frames, size, false));

    TEST_ESP_OK(rmt_wait_tx_done(channel0, portMAX_DELAY));
    TEST_ESP_OK(rmt_wait_tx_done(channel1, portMAX_DELAY));

    ESP_LOGI(TAG, "tx_end_time0=%u, tx_end_time1=%u", tx_end_time0, tx_end_time1);
    TEST_ASSERT_LESS_OR_EQUAL_UINT32(2000, tx_end_time1 - tx_end_time0);

    TEST_ESP_OK(rmt_remove_channel_from_group(channel0));
    TEST_ESP_OK(rmt_remove_channel_from_group(channel1));

    TEST_ESP_OK(rmt_driver_uninstall(channel0));
    TEST_ESP_OK(rmt_driver_uninstall(channel1));

}
#endif

#if SOC_RMT_SUPPORT_TX_LOOP_COUNT
static void rmt_tx_loop_end(rmt_channel_t channel, void *arg)
{
    rmt_tx_stop(channel);
}
TEST_CASE("RMT TX loop", "[rmt]")
{
    RingbufHandle_t rb = NULL;
    rmt_item32_t *items = NULL;
    uint32_t length = 0;
    uint32_t addr = 0x10;
    uint32_t cmd = 0x20;
    bool repeat = false;
    int tx_channel = 0;
    int rx_channel = 1;
    uint32_t count = 0;

    rmt_setup_testbench(tx_channel, rx_channel, RMT_TESTBENCH_FLAGS_LOOP_ON);

    // get ready to receive
    TEST_ESP_OK(rmt_get_ringbuf_handle(rx_channel, &rb));
    TEST_ASSERT_NOT_NULL(rb);
    TEST_ESP_OK(rmt_rx_start(rx_channel, true));

    vTaskDelay(pdMS_TO_TICKS(1000));

    // register callback functions, invoked when tx loop count to ceiling
    rmt_register_tx_end_callback(rmt_tx_loop_end, NULL);
    // build NEC codes
    ESP_LOGI(TAG, "Send command 0x%x to address 0x%x", cmd, addr);
    // Send new key code
    TEST_ESP_OK(s_ir_builder->build_frame(s_ir_builder, addr, cmd));
    TEST_ESP_OK(s_ir_builder->get_result(s_ir_builder, &items, &length));
    TEST_ESP_OK(rmt_write_items(tx_channel, items, length, true)); // wait until done

    // parse NEC codes
    while (rb) {
        items = (rmt_item32_t *) xRingbufferReceive(rb, &length, 1000);
        if (items) {
            length /= 4; // one RMT = 4 Bytes
            if (s_ir_parser->input(s_ir_parser, items, length) == ESP_OK) {
                if (s_ir_parser->get_scan_code(s_ir_parser, &addr, &cmd, &repeat) == ESP_OK) {
                    count++;
                    ESP_LOGI(TAG, "Scan Code %s --- addr: 0x%04x cmd: 0x%04x", repeat ? "(repeat)" : "", addr, cmd);
                }
            }
            vRingbufferReturnItem(rb, (void *) items);
        } else {
            ESP_LOGI(TAG, "done");
            break;
        }
    }

    TEST_ASSERT_EQUAL(10, count);
    rmt_clean_testbench(tx_channel, rx_channel);
}
#endif

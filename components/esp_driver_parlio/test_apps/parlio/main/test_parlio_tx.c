/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/parlio_tx.h"
#include "driver/gpio.h"
#include "hal/parlio_ll.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "test_board.h"

TEST_CASE("parallel_tx_unit_install_uninstall", "[parlio_tx]")
{
    printf("install tx units exhaustively\r\n");
    parlio_tx_unit_handle_t units[SOC_PARLIO_GROUPS * SOC_PARLIO_TX_UNITS_PER_GROUP];
    int k = 0;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH,
        .clk_in_gpio_num = -1, // clock source from internal
        .clk_out_gpio_num = 0,
        .output_clk_freq_hz = 1 * 1000 * 1000,
        .trans_queue_depth = 4,
        .max_transfer_size = 64,
        .valid_gpio_num = -1,
    };
    for (int i = 0; i < SOC_PARLIO_GROUPS; i++) {
        for (int j = 0; j < SOC_PARLIO_TX_UNITS_PER_GROUP; j++) {
            TEST_ESP_OK(parlio_new_tx_unit(&config, &units[k++]));
        }
    }
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, parlio_new_tx_unit(&config, &units[0]));

    for (int i = 0; i < k; i++) {
        TEST_ESP_OK(parlio_del_tx_unit(units[i]));
    }

    printf("install tx unit with valid signal and external core clock\r\n");
    // clock from external
    config.clk_in_gpio_num = 2;
    // failed because of invalid clock source frequency
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, parlio_new_tx_unit(&config, &units[0]));
    config.input_clk_src_freq_hz = 1000000;

    config.valid_gpio_num = 0;
#if PARLIO_LL_TX_DATA_LINE_AS_VALID_SIG
    // failed because of data line conflict with valid signal
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, parlio_new_tx_unit(&config, &units[0]));
#endif // !PARLIO_LL_TX_DATA_LINE_AS_VALID_SIG

    config.data_width = 4;
    TEST_ESP_OK(parlio_new_tx_unit(&config, &units[0]));
    TEST_ESP_OK(parlio_tx_unit_enable(units[0]));
    // delete unit before it's disabled is not allowed
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, parlio_del_tx_unit(units[0]));
    TEST_ESP_OK(parlio_tx_unit_disable(units[0]));
    TEST_ESP_OK(parlio_del_tx_unit(units[0]));
}

TEST_PARLIO_CALLBACK_ATTR
static bool test_parlio_tx_done_callback(parlio_tx_unit_handle_t tx_unit, const parlio_tx_done_event_data_t *edata, void *user_ctx)
{
    BaseType_t high_task_wakeup = pdFALSE;
    TaskHandle_t task = (TaskHandle_t)user_ctx;
    vTaskNotifyGiveFromISR(task, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

TEST_CASE("parallel_tx_unit_trans_done_event", "[parlio_tx]")
{
    printf("install parlio tx unit\r\n");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = 8,
        .clk_in_gpio_num = -1,  // use internal clock source
        .valid_gpio_num = -1,   // don't generate valid signal
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
            TEST_DATA1_GPIO,
            TEST_DATA2_GPIO,
            TEST_DATA3_GPIO,
            TEST_DATA4_GPIO,
            TEST_DATA5_GPIO,
            TEST_DATA6_GPIO,
            TEST_DATA7_GPIO,
        },
        .output_clk_freq_hz = 1 * 1000 * 1000,
        .trans_queue_depth = 8,
        .max_transfer_size = 128,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_LSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
    };
    TEST_ESP_OK(parlio_new_tx_unit(&config, &tx_unit));
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));

    printf("register trans_done event callback\r\n");
    parlio_tx_event_callbacks_t cbs = {
        .on_trans_done = test_parlio_tx_done_callback,
    };
    TEST_ESP_OK(parlio_tx_unit_register_event_callbacks(tx_unit, &cbs, xTaskGetCurrentTaskHandle()));

    printf("send packets and check event is fired\r\n");
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
    };
    __attribute__((aligned(64))) uint8_t payload[64] = {0};
    for (int i = 0; i < 64; i++) {
        payload[i] = i;
    }
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, 64 * sizeof(uint8_t) * 8, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdTRUE, portMAX_DELAY));
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, 64 * sizeof(uint8_t) * 8, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdTRUE, portMAX_DELAY));

    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
};

TEST_CASE("parallel_tx_unit_enable_disable", "[parlio_tx]")
{
    printf("install parlio tx unit\r\n");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = 8,
        .clk_in_gpio_num = -1,  // use internal clock source
        .valid_gpio_num = -1,   // don't generate valid signal
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
            TEST_DATA1_GPIO,
            TEST_DATA2_GPIO,
            TEST_DATA3_GPIO,
            TEST_DATA4_GPIO,
            TEST_DATA5_GPIO,
            TEST_DATA6_GPIO,
            TEST_DATA7_GPIO,
        },
        .output_clk_freq_hz = 1 * 1000 * 1000,
        .trans_queue_depth = 4,
        .max_transfer_size = 256,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_LSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
    };
    TEST_ESP_OK(parlio_new_tx_unit(&config, &tx_unit));
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));

    printf("send packets for multiple times\r\n");
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
    };
    __attribute__((aligned(64))) uint8_t payload[256] = {0};
    for (int i = 0; i < 256; i++) {
        payload[i] = i;
    }
    for (int j = 0; j < 3; j++) {
        TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, 256 * sizeof(uint8_t) * 8, &transmit_config));
    }

    printf("disable the transaction in the middle\r\n");
    while (parlio_tx_unit_disable(tx_unit) != ESP_OK) {
        esp_rom_delay_us(1000);
    }
    vTaskDelay(pdMS_TO_TICKS(10));

    printf("resume the transaction and pending packets should continue\r\n");
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));
    TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, -1));
    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
}

TEST_CASE("parallel_tx_unit_idle_value", "[parlio_tx]")
{
    printf("init a gpio to read parlio_tx output\r\n");
    gpio_config_t test_gpio_conf = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = BIT64(TEST_DATA0_GPIO),
    };
    TEST_ESP_OK(gpio_config(&test_gpio_conf));

    printf("install parlio tx unit\r\n");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = 8,
        .clk_in_gpio_num = -1,  // use internal clock source
        .valid_gpio_num = -1,   // don't generate valid signal
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
            TEST_DATA1_GPIO,
            TEST_DATA2_GPIO,
            TEST_DATA3_GPIO,
            TEST_DATA4_GPIO,
            TEST_DATA5_GPIO,
            TEST_DATA6_GPIO,
            TEST_DATA7_GPIO,
        },
        .output_clk_freq_hz = 1 * 1000 * 1000,
        .trans_queue_depth = 4,
        .max_transfer_size = 64,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_LSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
    };
    TEST_ESP_OK(parlio_new_tx_unit(&config, &tx_unit));
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));

    printf("send packet with different idle_value\r\n");
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
    };
    uint32_t size = 64;
    __attribute__((aligned(64))) uint8_t payload[size];
    for (int i = 0; i < size; i++) {
        payload[i] = i;
    }
    for (int j = 0; j < 16; j++) {
        transmit_config.idle_value = j;
        TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, sizeof(payload) * 8, &transmit_config));
        TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, 100));
        TEST_ASSERT_EQUAL(j & 0x01, gpio_get_level(TEST_DATA0_GPIO));
    }

    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
    TEST_ESP_OK(gpio_reset_pin(TEST_DATA0_GPIO));
}

#if SOC_PARLIO_TX_CLK_SUPPORT_GATING
TEST_CASE("parallel_tx_clock_gating", "[paralio_tx]")
{
    printf("init a gpio to read parlio_tx clk output\r\n");
    gpio_config_t test_gpio_conf = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = BIT64(TEST_CLK_GPIO),
    };
    TEST_ESP_OK(gpio_config(&test_gpio_conf));

    printf("install parlio tx unit\r\n");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = 2,
        .clk_in_gpio_num = -1,  // use internal clock source
        .valid_gpio_num = TEST_DATA7_GPIO, // generate the valid signal
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
            TEST_DATA1_GPIO,
        },
        .output_clk_freq_hz = 1 * 1000 * 1000,
        .trans_queue_depth = 4,
        .max_transfer_size = 64,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
        .flags.clk_gate_en = true, // enable clock gating, controlled by the level of TEST_DATA7_GPIO
    };
    TEST_ESP_OK(parlio_new_tx_unit(&config, &tx_unit));
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));

    printf("send packets and see if the clock is gated when there's no transaction on line\r\n");
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
    };
    uint32_t size = 64;
    __attribute__((aligned(64))) uint8_t payload[size];
    for (int i = 0; i < size; i++) {
        payload[i] = 0x1B; // 8'b00011011, in PARLIO_BIT_PACK_ORDER_MSB, you should see 2'b00, 2'b01, 2'b10, 2'b11 on the data line
    }
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, size * sizeof(uint8_t) * 8, &transmit_config));
    TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, -1));
    // check if the level on the clock line is low
    TEST_ASSERT_EQUAL(0, gpio_get_level(TEST_CLK_GPIO));
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, size * sizeof(uint8_t) * 8, &transmit_config));
    TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, -1));
    TEST_ASSERT_EQUAL(0, gpio_get_level(TEST_CLK_GPIO));
    TEST_ASSERT_EQUAL(0, gpio_get_level(TEST_CLK_GPIO));

    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
    TEST_ESP_OK(gpio_reset_pin(TEST_CLK_GPIO));
}

#if !PARLIO_LL_TX_DATA_LINE_AS_VALID_SIG
TEST_CASE("parallel_tx_clock_gating_and_msb_coexist", "[paralio_tx]")
{
    printf("init a gpio to read parlio_tx clk output\r\n");
    gpio_config_t test_gpio_conf = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = BIT64(TEST_CLK_GPIO) | BIT64(TEST_DATA7_GPIO),
    };
    TEST_ESP_OK(gpio_config(&test_gpio_conf));

    printf("install parlio tx unit\r\n");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = 8,
        .clk_in_gpio_num = -1,  // use internal clock source
        .valid_gpio_num = TEST_VALID_GPIO, // generate the valid signal
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
            TEST_DATA1_GPIO,
            TEST_DATA2_GPIO,
            TEST_DATA3_GPIO,
            TEST_DATA4_GPIO,
            TEST_DATA5_GPIO,
            TEST_DATA6_GPIO,
            TEST_DATA7_GPIO,
        },
        .output_clk_freq_hz = 1 * 1000 * 1000,
        .trans_queue_depth = 4,
        .max_transfer_size = 256,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
        .valid_start_delay = 5,
        .valid_stop_delay = 5,
        .flags.clk_gate_en = true, // enable clock gating, controlled by the CS signal
    };
    TEST_ESP_OK(parlio_new_tx_unit(&config, &tx_unit));
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));

    printf("send packets and see if the clock is gated when there's no transaction on line\r\n");
    parlio_transmit_config_t transmit_config = {
        // set the idle value to 0x80, so that the MSB is high when there's no transaction
        .idle_value = 0x80,
    };
    uint32_t size = 256;
    __attribute__((aligned(64))) uint8_t payload[size];
    for (int i = 0; i < size; i++) {
        payload[i] = i;
    }
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, size * sizeof(uint8_t) * 8, &transmit_config));
    TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, -1));
    // check if the level on the clock line is low
    TEST_ASSERT_EQUAL(0, gpio_get_level(TEST_CLK_GPIO));
    TEST_ASSERT_EQUAL(1, gpio_get_level(TEST_DATA7_GPIO));
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, size * sizeof(uint8_t) * 8, &transmit_config));
    TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, -1));
    TEST_ASSERT_EQUAL(0, gpio_get_level(TEST_CLK_GPIO));
    TEST_ASSERT_EQUAL(0, gpio_get_level(TEST_CLK_GPIO));
    TEST_ASSERT_EQUAL(1, gpio_get_level(TEST_DATA7_GPIO));

    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
    TEST_ESP_OK(gpio_reset_pin(TEST_CLK_GPIO));
}
#endif // !PARLIO_LL_TX_DATA_LINE_AS_VALID_SIG
#endif // SOC_PARLIO_TX_CLK_SUPPORT_GATING

#if SOC_PSRAM_DMA_CAPABLE
TEST_CASE("parlio_tx_can_transmit_PSRAM_buffer", "[parlio_tx]")
{
    printf("install parlio tx unit\r\n");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = 1,
        .clk_in_gpio_num = -1,  // use internal clock source
        .valid_gpio_num = TEST_VALID_GPIO, // generate the valid signal
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
        },
        .output_clk_freq_hz = 10 * 1000 * 1000,
        .trans_queue_depth = 4,
        .max_transfer_size = 65535,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_LSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
        .flags.clk_gate_en = true,
    };

    TEST_ESP_OK(parlio_new_tx_unit(&config, &tx_unit));
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));

    const size_t buffer_size = 160 * 1000;
    const size_t chunk_size = buffer_size / 4; // 40KB per trunk
    uint8_t *buffer = heap_caps_malloc(buffer_size, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(buffer);
    for (int i = 0; i < buffer_size; i++) {
        buffer[i] = i;
    }

    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
    };

    const uint8_t cmd = 0x2C;
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, &cmd, 8, &transmit_config));
    for (int i = 0; i < 20; i++) {
        TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, buffer + (i % 4) * chunk_size, chunk_size * 8, &transmit_config));
    }
    TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, -1));

    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
    free(buffer);
}
#endif // SOC_PSRAM_DMA_CAPABLE

static void test_gpio_simulate_rising_edge(int gpio_sig, size_t times)
{
    while (times--) {
        gpio_set_level(gpio_sig, 0);
        gpio_set_level(gpio_sig, 1);
        gpio_set_level(gpio_sig, 0);
    }
}

static uint8_t test_gpio_get_output_data(gpio_num_t* gpio, size_t gpio_num)
{
    uint8_t result = 0;
    for (size_t i = 0; i < gpio_num; i++) {
        int level = gpio_get_level(gpio[i]);
        result |= level << i;
    }
    return result;
}

static void test_use_external_non_free_running_clock(parlio_tx_unit_handle_t tx_unit, parlio_tx_unit_config_t config, int test_round)
{
    uint32_t clock_div = config.input_clk_src_freq_hz / config.output_clk_freq_hz;
    TEST_ESP_OK(parlio_new_tx_unit(&config, &tx_unit));
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));
    // let core clock running for a while to update the clock divider threshold, this is a hardware limitation
    // the following rising edge count is not a magic value, we just need it to be larger than the clock divider value in the previous test case
    test_gpio_simulate_rising_edge(TEST_EXT_CLK_GPIO, 100);
    esp_rom_delay_us(1000);
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0xAA,
    };
    __attribute__((aligned(64))) uint8_t payload[256] = {0};
    for (int i = 0; i < 256; i++) {
        payload[i] = i;
    }

    for (int round = 0; round < test_round; round++) {
        TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, 256 * sizeof(uint8_t) * 8, &transmit_config));
        for (int i = 0; i < 256; i++) {
            // After "clock_div" times external pulses pass through the internal frequency divider, the parlio core clock generates a single pulse.
            test_gpio_simulate_rising_edge(TEST_EXT_CLK_GPIO, clock_div);
            TEST_ASSERT_EQUAL(i, test_gpio_get_output_data(config.data_gpio_nums, config.data_width));
        }
        // In order to update the idle value, an additional rising edge is required
        test_gpio_simulate_rising_edge(TEST_EXT_CLK_GPIO, clock_div);
        TEST_ASSERT_EQUAL(transmit_config.idle_value, test_gpio_get_output_data(config.data_gpio_nums, config.data_width));
        TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, 100));
    }
    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
}

TEST_CASE("parallel tx unit use external non-free running clock", "[parlio_tx]")
{
    printf("use gpio as external clock source\r\n");
    // configure the data gpio for loopback test
    gpio_config_t gpio_conf = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = BIT64(TEST_DATA0_GPIO) | BIT64(TEST_DATA1_GPIO) | BIT64(TEST_DATA2_GPIO) | BIT64(TEST_DATA3_GPIO) |
        BIT64(TEST_DATA4_GPIO) | BIT64(TEST_DATA5_GPIO) | BIT64(TEST_DATA6_GPIO) | BIT64(TEST_DATA7_GPIO),
    };
    TEST_ESP_OK(gpio_config(&gpio_conf));
    // configure the external clock output gpio
    gpio_conf.mode = GPIO_MODE_OUTPUT;
    gpio_conf.pin_bit_mask = BIT64(TEST_EXT_CLK_GPIO);
    TEST_ESP_OK(gpio_config(&gpio_conf));

    printf("install parlio tx unit\r\n");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_EXTERNAL,
        .data_width = 8,
        .clk_in_gpio_num = TEST_EXT_CLK_GPIO,
        .input_clk_src_freq_hz = 80 * 1000 * 1000, // Note that this is not the real input frequency, we just use it to calculate the clock divider
        .valid_gpio_num = -1,   // don't generate valid signal
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
            TEST_DATA1_GPIO,
            TEST_DATA2_GPIO,
            TEST_DATA3_GPIO,
            TEST_DATA4_GPIO,
            TEST_DATA5_GPIO,
            TEST_DATA6_GPIO,
            TEST_DATA7_GPIO,
        },
        .output_clk_freq_hz = 1 * 1000 * 1000, // For the same reason, this is not the real output frequency
        .trans_queue_depth = 8,
        .max_transfer_size = 256,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_LSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
    };

    uint8_t test_round = 50;
    printf("test input clk freq is greater than output clk freq\r\n");
    test_use_external_non_free_running_clock(tx_unit, config, test_round);

    // changes input clk freq
    config.input_clk_src_freq_hz = 1 * 1000 * 1000;
    printf("test special condition, input clk freq equals to output clk freq\r\n");
    test_use_external_non_free_running_clock(tx_unit, config, test_round);

    TEST_ESP_OK(gpio_reset_pin(TEST_EXT_CLK_GPIO));
    for (int i = 0; i < 8; i++) {
        TEST_ESP_OK(gpio_reset_pin(config.data_gpio_nums[i]));
    }
};

#if SOC_PARLIO_TX_SUPPORT_LOOP_TRANSMISSION
typedef struct {
    uint32_t switch_count;
    void *old_buffer_addr[5];
    void *new_buffer_addr[5];
} test_parlio_tx_buffer_switched_context_t;

TEST_PARLIO_CALLBACK_ATTR
static bool test_parlio_tx_buffer_switched_callback(parlio_tx_unit_handle_t tx_unit, const parlio_tx_buffer_switched_event_data_t *edata, void *user_ctx)
{
    test_parlio_tx_buffer_switched_context_t *context = (test_parlio_tx_buffer_switched_context_t *)user_ctx;
    context->old_buffer_addr[context->switch_count] = edata->old_buffer_addr;
    context->new_buffer_addr[context->switch_count] = edata->new_buffer_addr;
    context->switch_count++;
    return false;
}

TEST_CASE("parlio_tx_loop_transmission", "[parlio_tx]")
{
    printf("install parlio tx unit\r\n");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = 8,
        .clk_in_gpio_num = -1,  // use internal clock source
        .valid_gpio_num = -1,
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
            TEST_DATA1_GPIO,
            TEST_DATA2_GPIO,
            TEST_DATA3_GPIO,
            TEST_DATA4_GPIO,
            TEST_DATA5_GPIO,
            TEST_DATA6_GPIO,
            TEST_DATA7_GPIO,
        },
        .output_clk_freq_hz = 10 * 1000 * 1000,
        .trans_queue_depth = 3,
        .max_transfer_size = 256,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_LSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
    };
    TEST_ESP_OK(parlio_new_tx_unit(&config, &tx_unit));

    printf("register trans_done event callback\r\n");
    parlio_tx_event_callbacks_t cbs = {
        .on_buffer_switched = test_parlio_tx_buffer_switched_callback,
    };
    test_parlio_tx_buffer_switched_context_t context = {
        .switch_count = 0,
    };
    TEST_ESP_OK(parlio_tx_unit_register_event_callbacks(tx_unit, &cbs, &context));

    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));

    printf("send packets and check event is fired\r\n");
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
        .flags.loop_transmission = true,
    };
    __attribute__((aligned(64))) uint8_t payload_loop1[256] = {0};
    __attribute__((aligned(64))) uint8_t payload_loop2[256] = {0};
    __attribute__((aligned(64))) uint8_t payload_oneshot[256] = {0};
    for (int i = 0; i < 256; i++) {
        payload_loop1[i] = i;
        payload_loop2[i] = 255 - i;
        payload_oneshot[i] = i * 2 + 1;
    }

    transmit_config.flags.loop_transmission = true;
    int lopp_count = 3;
    while (lopp_count--) {
        TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload_loop1, 256 * sizeof(uint8_t) * 8, &transmit_config));
        vTaskDelay(pdMS_TO_TICKS(10));
        // Should be sent after the previous frame has been completely sent
        TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload_loop2, 256 * sizeof(uint8_t) * 8, &transmit_config));
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    transmit_config.flags.loop_transmission = false;
    // should be pending in queue
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload_oneshot, 256 * sizeof(uint8_t) * 8, &transmit_config));
    transmit_config.flags.loop_transmission = true;
    // there is a oneshot trans in queue, should also be pending in queue
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload_loop1, 256 * sizeof(uint8_t) * 8, &transmit_config));

    TEST_ESP_ERR(ESP_ERR_TIMEOUT, parlio_tx_unit_wait_all_done(tx_unit, 50));

    // stop infinite loop transmission
    parlio_tx_unit_disable(tx_unit);
    // We should see 1 oneshot frame and 1 loop transmission (both pending in queue)
    parlio_tx_unit_enable(tx_unit);

    vTaskDelay(pdMS_TO_TICKS(10));
    // stop the second infinite loop transmission
    parlio_tx_unit_disable(tx_unit);
    parlio_tx_unit_enable(tx_unit);

    // total 5 switch events
    TEST_ASSERT_EQUAL(5, context.switch_count);
    for (int i = 0; i < context.switch_count; i++) {
        void *old_buffer_addr = context.old_buffer_addr[i];
        void *new_buffer_addr = context.new_buffer_addr[i];
        if (i % 2 == 0) {
            TEST_ASSERT_EQUAL(payload_loop1, old_buffer_addr);
            TEST_ASSERT_EQUAL(payload_loop2, new_buffer_addr);
        } else {
            TEST_ASSERT_EQUAL(payload_loop2, old_buffer_addr);
            TEST_ASSERT_EQUAL(payload_loop1, new_buffer_addr);
        }
    }

    TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, -1));
    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
}
#endif  // SOC_PARLIO_TX_SUPPORT_LOOP_TRANSMISSION

#if SOC_PARLIO_TX_SUPPORT_EOF_FROM_DMA
TEST_CASE("parlio_tx can transmit buffer larger than max_size decided by datalen_eof", "[parlio_tx]")
{
    printf("install parlio tx unit\r\n");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = 4,
        .clk_in_gpio_num = -1,  // use internal clock source
        .valid_gpio_num = TEST_VALID_GPIO, // generate the valid signal
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
            TEST_DATA1_GPIO,
            TEST_DATA2_GPIO,
            TEST_DATA3_GPIO,
        },
        .output_clk_freq_hz = 10 * 1000 * 1000,
        .trans_queue_depth = 1,
        .max_transfer_size = 100 * 1024,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_LSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
        .flags.clk_gate_en = true,
    };

    TEST_ESP_OK(parlio_new_tx_unit(&config, &tx_unit));
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));

    const size_t buffer_size = 100 * 1024; // 100KB, larger than the 65535 bytes limit
    uint8_t *buffer = heap_caps_malloc(buffer_size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(buffer);
    for (int i = 0; i < buffer_size; i++) {
        buffer[i] = i;
    }

    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
    };

    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, buffer, buffer_size * 8, &transmit_config));
    TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, -1));

    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
    free(buffer);
}
#endif // SOC_PARLIO_TX_SUPPORT_EOF_FROM_DMA

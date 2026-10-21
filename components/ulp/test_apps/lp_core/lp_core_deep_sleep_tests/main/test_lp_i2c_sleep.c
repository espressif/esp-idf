/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "unity.h"
#include "test_utils.h"
#include "lp_core_i2c.h"
#include "ulp_lp_core.h"
#include "driver/i2c_slave.h"
#include "hal/i2c_ll.h"
#include "soc/i2c_struct.h"
#include "lp_sleep_common.h"
#include "lp_sleep_payload.h"
#include "lp_core_test_app_i2c_sleep.h"

extern const uint8_t lp_core_test_app_i2c_sleep_bin_start[] asm("_binary_lp_core_test_app_i2c_sleep_bin_start");
extern const uint8_t lp_core_test_app_i2c_sleep_bin_end[]   asm("_binary_lp_core_test_app_i2c_sleep_bin_end");

static const char *TAG = "lp_i2c_sleep";

#define I2C_SLAVE_NUM I2C_NUM_0

extern uint32_t lp_sleep_i2c_timing_reg(void);

static RTC_DATA_ATTR uint32_t s_i2c_timing_before;

static QueueHandle_t s_i2c_slave_event_queue;
static uint8_t s_slave_rx_data[LP_SLEEP_I2C_LEN];
static size_t s_slave_rx_len;

typedef enum {
    I2C_SLAVE_EVT_RX,
} i2c_slave_test_event_t;

static bool IRAM_ATTR i2c_slave_receive_cb(i2c_slave_dev_handle_t i2c_slave,
                                           const i2c_slave_rx_done_event_data_t *evt_data,
                                           void *arg)
{
    (void)i2c_slave;
    (void)arg;
    BaseType_t task_woken = pdFALSE;
    i2c_slave_test_event_t evt = I2C_SLAVE_EVT_RX;
    s_slave_rx_len = evt_data->length;
    if (evt_data->length <= sizeof(s_slave_rx_data)) {
        memcpy(s_slave_rx_data, evt_data->buffer, evt_data->length);
    }
    xQueueSendFromISR(s_i2c_slave_event_queue, &evt, &task_woken);
    return task_woken == pdTRUE;
}

static void i2c_slave_disable_clock_stretch(void)
{
    /* TODO(IDF-15683): remove this workaround once the new I2C slave driver handles LP I2C repeated-start reads. */
    i2c_ll_slave_enable_scl_stretch(&I2C0, false);
    i2c_ll_slave_clear_stretch(&I2C0);
    i2c_ll_update(&I2C0);
}

static void lp_i2c_keep_pins(void)
{
    lp_sleep_keep_pin(LP_I2C_SDA_IO);
    lp_sleep_keep_pin(LP_I2C_SCL_IO);
}

static void lp_i2c_check_transfer(void)
{
    uint32_t timing = lp_sleep_i2c_timing_reg();
    printf("LP I2C SCL low period: before 0x%08" PRIx32 " now 0x%08" PRIx32 "\n", s_i2c_timing_before, timing);
    TEST_ASSERT_MESSAGE(s_i2c_timing_before != 0, "LP I2C timing register was zero after init");
    TEST_ASSERT_EQUAL_HEX32(s_i2c_timing_before, timing);

    printf("LP I2C read ret %ld write ret %ld\n", (long)ulp_i2c_sleep_read_ret, (long)ulp_i2c_sleep_write_ret);
    TEST_ASSERT_EQUAL(ESP_OK, (esp_err_t)ulp_i2c_sleep_read_ret);
    TEST_ASSERT_EQUAL(ESP_OK, (esp_err_t)ulp_i2c_sleep_write_ret);

    uint8_t expected[LP_SLEEP_I2C_LEN];
    lp_sleep_fill_i2c_read(expected);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, (uint8_t *)ulp_i2c_sleep_data_rd, LP_SLEEP_I2C_LEN);
}

static void lp_i2c_sleeper_first(void)
{
    lp_core_i2c_cfg_t i2c_cfg = LP_CORE_I2C_DEFAULT_CONFIG();
    i2c_cfg.i2c_timing_cfg.clk_speed_hz = 100000;
    TEST_ESP_OK(lp_core_i2c_master_init(LP_I2C_NUM_0, &i2c_cfg));
    lp_i2c_keep_pins();

    s_i2c_timing_before = lp_sleep_i2c_timing_reg();
    printf("LP I2C SCL low period after init: 0x%08" PRIx32 "\n", s_i2c_timing_before);

    lp_sleep_run(lp_core_test_app_i2c_sleep_bin_start, lp_core_test_app_i2c_sleep_bin_end,
                 ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, true);
    hp_core_deep_sleep_enter();
}

static void lp_i2c_sleeper_second(void)
{
    lp_sleep_expect_ulp_wakeup();
    lp_i2c_check_transfer();

    /* Second cycle uses the LP I2C configuration retained across deep sleep. */
    lp_i2c_keep_pins();
    lp_sleep_run(lp_core_test_app_i2c_sleep_bin_start, lp_core_test_app_i2c_sleep_bin_end,
                 ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, false);
    hp_core_deep_sleep_enter();
}

static void lp_i2c_sleeper_check(void)
{
    lp_sleep_expect_ulp_wakeup();
    lp_i2c_check_transfer();
}

static void lp_i2c_peer(void)
{
    uint8_t reply[LP_SLEEP_I2C_LEN];
    uint8_t expected_write[LP_SLEEP_I2C_LEN];
    lp_sleep_fill_i2c_read(reply);
    lp_sleep_fill_i2c_write(expected_write);

    i2c_slave_config_t conf_slave = {
        .i2c_port = I2C_SLAVE_NUM,
        .sda_io_num = LP_I2C_SDA_IO,
        .scl_io_num = LP_I2C_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .send_buf_depth = LP_SLEEP_I2C_LEN * 2,
        .receive_buf_depth = LP_SLEEP_I2C_LEN * 2,
        .slave_addr = LP_SLEEP_I2C_ADDR,
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .flags.enable_internal_pullup = true,
    };

    i2c_slave_dev_handle_t slave_handle = NULL;
    s_i2c_slave_event_queue = xQueueCreate(4, sizeof(i2c_slave_test_event_t));
    TEST_ASSERT_NOT_NULL(s_i2c_slave_event_queue);
    TEST_ESP_OK(i2c_new_slave_device(&conf_slave, &slave_handle));
    i2c_slave_disable_clock_stretch();

    i2c_slave_event_callbacks_t cbs = {
        .on_receive = i2c_slave_receive_cb,
    };
    TEST_ESP_OK(i2c_slave_register_event_callbacks(slave_handle, &cbs, NULL));

    for (int cycle = 0; cycle < 2; cycle++) {
        uint32_t queued = 0;
        xQueueReset(s_i2c_slave_event_queue);
        s_slave_rx_len = 0;
        memset(s_slave_rx_data, 0, sizeof(s_slave_rx_data));

        TEST_ESP_OK(i2c_slave_write(slave_handle, reply, LP_SLEEP_I2C_LEN, &queued, 2000));
        TEST_ASSERT_EQUAL_UINT32(LP_SLEEP_I2C_LEN, queued);
        printf("I2C_PEER_ARMED %d\n", cycle);
        fflush(stdout);

        i2c_slave_test_event_t evt;
        TEST_ASSERT_EQUAL(pdTRUE, xQueueReceive(s_i2c_slave_event_queue, &evt, pdMS_TO_TICKS(15000)));
        TEST_ASSERT_EQUAL(I2C_SLAVE_EVT_RX, evt);
        ESP_LOGI(TAG, "Slave read length %u", (unsigned)s_slave_rx_len);
        ESP_LOG_BUFFER_HEX(TAG, s_slave_rx_data, s_slave_rx_len);
        TEST_ASSERT_EQUAL_INT(LP_SLEEP_I2C_LEN, s_slave_rx_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_write, s_slave_rx_data, LP_SLEEP_I2C_LEN);
        printf("I2C_PEER_OK %d\n", cycle);
        fflush(stdout);
    }

    vQueueDelete(s_i2c_slave_event_queue);
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_STAGES(
    "LP I2C master read and write during deep sleep",
    "[lp_core][lp_i2c_sleep][test_env=generic_multi_device][timeout=120][deepsleep]",
    lp_i2c_sleeper_first,
    lp_i2c_sleeper_second,
    lp_i2c_sleeper_check);

TEST_CASE(
    "LP I2C peer during deep sleep",
    "[lp_core][lp_i2c_sleep][test_env=generic_multi_device][timeout=120]")
{
    lp_i2c_peer();
}

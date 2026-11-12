/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "unity.h"
#include "test_utils.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "ulp_lp_core.h"
#include "lp_core_spi.h"
#include "lp_sleep_common.h"
#include "lp_sleep_payload.h"
#include "lp_core_test_app_spi_master_sleep.h"
#include "lp_core_test_app_spi_slave_sleep.h"

extern const uint8_t lp_core_test_app_spi_master_sleep_bin_start[]
asm("_binary_lp_core_test_app_spi_master_sleep_bin_start");
extern const uint8_t lp_core_test_app_spi_master_sleep_bin_end[]
asm("_binary_lp_core_test_app_spi_master_sleep_bin_end");
extern const uint8_t lp_core_test_app_spi_slave_sleep_bin_start[]
asm("_binary_lp_core_test_app_spi_slave_sleep_bin_start");
extern const uint8_t lp_core_test_app_spi_slave_sleep_bin_end[]
asm("_binary_lp_core_test_app_spi_slave_sleep_bin_end");

static const char *TAG = "lp_spi_sleep";

#define LP_SLEEP_SPI_HOST           SPI2_HOST
#define LP_SLEEP_SPI_HZ             10000
#define LP_SLEEP_SPI_MISO           6
#define LP_SLEEP_SPI_MOSI           7
#define LP_SLEEP_SPI_CS             4
#if CONFIG_IDF_TARGET_ESP32S31
#define LP_SLEEP_SPI_SCLK           5
#else
#define LP_SLEEP_SPI_SCLK           8
#endif

extern uint32_t lp_sleep_spi_clock_reg(void);
extern uint32_t lp_sleep_spi_slave_reg(void);
extern uint32_t lp_sleep_spi_slave_mode(void);

static RTC_DATA_ATTR uint32_t s_spi_clock_before;
static RTC_DATA_ATTR uint32_t s_spi_slave_before;

static const lp_spi_host_t s_lp_spi_host = 0;

static void lp_spi_keep_pins(void)
{
    lp_sleep_keep_pin(LP_SLEEP_SPI_MISO);
    lp_sleep_keep_pin(LP_SLEEP_SPI_MOSI);
    lp_sleep_keep_pin(LP_SLEEP_SPI_SCLK);
    lp_sleep_keep_pin(LP_SLEEP_SPI_CS);
}

static void lp_spi_bus(lp_spi_bus_config_t *bus)
{
    bus->mosi_io_num = LP_SLEEP_SPI_MOSI;
    bus->miso_io_num = LP_SLEEP_SPI_MISO;
    bus->sclk_io_num = LP_SLEEP_SPI_SCLK;
}

static void lp_spi_master_init(void)
{
    lp_spi_bus_config_t bus = {};
    lp_spi_device_config_t device = {
        .cs_io_num = LP_SLEEP_SPI_CS,
        .clock_speed_hz = LP_SLEEP_SPI_HZ,
        .spi_mode = 0,
        .duty_cycle = 128,
    };

    lp_spi_bus(&bus);
    TEST_ESP_OK(lp_core_lp_spi_bus_initialize(s_lp_spi_host, &bus));
    TEST_ESP_OK(lp_core_lp_spi_bus_add_device(s_lp_spi_host, &device));
    lp_spi_keep_pins();

    s_spi_clock_before = lp_sleep_spi_clock_reg();
    printf("LP SPI clock after init: 0x%08" PRIx32 "\n", s_spi_clock_before);
}

static void lp_spi_slave_init(void)
{
    lp_spi_bus_config_t bus = {};
    lp_spi_slave_config_t slave = {
        .cs_io_num = LP_SLEEP_SPI_CS,
        .spi_mode = 0,
    };

    lp_spi_bus(&bus);
    TEST_ESP_OK(lp_core_lp_spi_bus_initialize(s_lp_spi_host, &bus));
    TEST_ESP_OK(lp_core_lp_spi_slave_initialize(s_lp_spi_host, &slave));
    lp_spi_keep_pins();

    s_spi_slave_before = lp_sleep_spi_slave_reg();
    printf("LP SPI slave register after init: 0x%08" PRIx32 "\n", s_spi_slave_before);
    TEST_ASSERT_EQUAL_UINT32(1, lp_sleep_spi_slave_mode());
}

static void lp_spi_check_master(void)
{
    uint8_t expected[LP_SLEEP_SPI_LEN];
    uint32_t clock = lp_sleep_spi_clock_reg();

    printf("LP SPI clock: before 0x%08" PRIx32 " now 0x%08" PRIx32 "\n", s_spi_clock_before, clock);
    TEST_ASSERT_MESSAGE(s_spi_clock_before != 0, "LP SPI clock register was zero after init");
    TEST_ASSERT_EQUAL_HEX32(s_spi_clock_before, clock);

    printf("LP SPI master ret %ld\n", (long)ulp_spi_master_sleep_ret);
    TEST_ASSERT_EQUAL(ESP_OK, (esp_err_t)ulp_spi_master_sleep_ret);
    lp_sleep_fill_spi_master_rx(expected);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, (uint8_t *)ulp_spi_master_sleep_rx, LP_SLEEP_SPI_LEN);
}

static void lp_spi_check_slave(void)
{
    uint8_t expected[LP_SLEEP_SPI_LEN];
    uint32_t slave = lp_sleep_spi_slave_reg();

    printf("LP SPI slave register: before 0x%08" PRIx32 " now 0x%08" PRIx32 "\n", s_spi_slave_before, slave);
    TEST_ASSERT_EQUAL_UINT32(1, lp_sleep_spi_slave_mode());
    TEST_ASSERT_EQUAL_HEX32(s_spi_slave_before, slave);

    printf("LP SPI slave ret %ld\n", (long)ulp_spi_slave_sleep_ret);
    TEST_ASSERT_EQUAL(ESP_OK, (esp_err_t)ulp_spi_slave_sleep_ret);
    lp_sleep_fill_spi_slave_rx(expected);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, (uint8_t *)ulp_spi_slave_sleep_rx, LP_SLEEP_SPI_LEN);
}

static void lp_spi_master_sleeper_first(void)
{
    lp_spi_master_init();
    lp_sleep_run(lp_core_test_app_spi_master_sleep_bin_start, lp_core_test_app_spi_master_sleep_bin_end,
                 ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, true);
    hp_core_deep_sleep_enter();
}

static void lp_spi_master_sleeper_second(void)
{
    lp_sleep_expect_ulp_wakeup();
    lp_spi_check_master();

    /* Second cycle uses the LP SPI master configuration retained across deep sleep. */
    lp_spi_keep_pins();
    lp_sleep_run(lp_core_test_app_spi_master_sleep_bin_start, lp_core_test_app_spi_master_sleep_bin_end,
                 ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, false);
    hp_core_deep_sleep_enter();
}

static void lp_spi_master_sleeper_check(void)
{
    lp_sleep_expect_ulp_wakeup();
    lp_spi_check_master();
}

static void lp_spi_slave_sleeper_first(void)
{
    lp_spi_slave_init();
    lp_sleep_run(lp_core_test_app_spi_slave_sleep_bin_start, lp_core_test_app_spi_slave_sleep_bin_end,
                 ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, true);
    hp_core_deep_sleep_enter();
}

static void lp_spi_slave_sleeper_second(void)
{
    lp_sleep_expect_ulp_wakeup();
    lp_spi_check_slave();

    /* Second cycle uses the LP SPI slave configuration retained across deep sleep. */
    lp_spi_keep_pins();
    lp_sleep_run(lp_core_test_app_spi_slave_sleep_bin_start, lp_core_test_app_spi_slave_sleep_bin_end,
                 ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, false);
    hp_core_deep_sleep_enter();
}

static void lp_spi_slave_sleeper_check(void)
{
    lp_sleep_expect_ulp_wakeup();
    lp_spi_check_slave();
}

static void hp_spi_pins(spi_bus_config_t *bus)
{
    *bus = (spi_bus_config_t) {
        .mosi_io_num = LP_SLEEP_SPI_MOSI,
        .miso_io_num = LP_SLEEP_SPI_MISO,
        .sclk_io_num = LP_SLEEP_SPI_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
}

static void lp_spi_master_peer(void)
{
    uint8_t reply[LP_SLEEP_SPI_LEN];
    uint8_t expected[LP_SLEEP_SPI_LEN];
    uint8_t rx[LP_SLEEP_SPI_LEN];
    spi_bus_config_t bus;
    spi_slave_interface_config_t slave = {
        .spics_io_num = LP_SLEEP_SPI_CS,
        .queue_size = 1,
        .mode = 0,
    };

    lp_sleep_fill_spi_master_rx(reply);
    lp_sleep_fill_spi_master_tx(expected);
    hp_spi_pins(&bus);
    TEST_ESP_OK(spi_slave_initialize(LP_SLEEP_SPI_HOST, &bus, &slave, SPI_DMA_DISABLED));

    for (int cycle = 0; cycle < 2; cycle++) {
        spi_slave_transaction_t trans;
        spi_slave_transaction_t *result = NULL;

        printf("SPI_MST_PEER_ARMED %d\n", cycle);
        fflush(stdout);
        /* Queue only after the sleeper has programmed the LP pins. */
        unity_wait_for_signal("queue");

        memset(&trans, 0, sizeof(trans));
        memset(rx, 0, sizeof(rx));
        trans.length = LP_SLEEP_SPI_LEN * 8;
        trans.tx_buffer = reply;
        trans.rx_buffer = rx;
        TEST_ESP_OK(spi_slave_queue_trans(LP_SLEEP_SPI_HOST, &trans, pdMS_TO_TICKS(15000)));
        TEST_ESP_OK(spi_slave_get_trans_result(LP_SLEEP_SPI_HOST, &result, pdMS_TO_TICKS(15000)));
        TEST_ASSERT_NOT_NULL(result);
        ESP_LOGI(TAG, "HP slave trans_len %u", (unsigned)result->trans_len);
        ESP_LOG_BUFFER_HEX(TAG, rx, LP_SLEEP_SPI_LEN);
        TEST_ASSERT_EQUAL_UINT32(LP_SLEEP_SPI_LEN * 8, result->trans_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, rx, LP_SLEEP_SPI_LEN);
        printf("SPI_MST_PEER_OK %d\n", cycle);
        fflush(stdout);
    }

    TEST_ESP_OK(spi_slave_free(LP_SLEEP_SPI_HOST));
}

static void lp_spi_slave_peer(void)
{
    uint8_t tx[LP_SLEEP_SPI_LEN];
    uint8_t expected[LP_SLEEP_SPI_LEN];
    uint8_t rx[LP_SLEEP_SPI_LEN];
    spi_bus_config_t bus;
    spi_device_interface_config_t device = {
        .clock_speed_hz = LP_SLEEP_SPI_HZ,
        .mode = 0,
        .spics_io_num = LP_SLEEP_SPI_CS,
        .queue_size = 1,
        .duty_cycle_pos = 128,
        .cs_ena_posttrans = 3,
    };
    spi_device_handle_t handle = NULL;

    lp_sleep_fill_spi_slave_rx(tx);
    lp_sleep_fill_spi_slave_tx(expected);
    hp_spi_pins(&bus);
    TEST_ESP_OK(spi_bus_initialize(LP_SLEEP_SPI_HOST, &bus, SPI_DMA_DISABLED));
    TEST_ESP_OK(spi_bus_add_device(LP_SLEEP_SPI_HOST, &device, &handle));

    for (int cycle = 0; cycle < 2; cycle++) {
        spi_transaction_t trans;

        printf("SPI_SLV_PEER_ARMED %d\n", cycle);
        fflush(stdout);
        /* Clock only after LP main has armed the slave. */
        unity_wait_for_signal("clock");

        memset(&trans, 0, sizeof(trans));
        memset(rx, 0, sizeof(rx));
        trans.length = LP_SLEEP_SPI_LEN * 8;
        trans.tx_buffer = tx;
        trans.rx_buffer = rx;
        TEST_ESP_OK(spi_device_transmit(handle, &trans));
        ESP_LOG_BUFFER_HEX(TAG, rx, LP_SLEEP_SPI_LEN);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, rx, LP_SLEEP_SPI_LEN);
        printf("SPI_SLV_PEER_OK %d\n", cycle);
        fflush(stdout);
    }

    TEST_ESP_OK(spi_bus_remove_device(handle));
    TEST_ESP_OK(spi_bus_free(LP_SLEEP_SPI_HOST));
}

TEST_CASE_MULTIPLE_STAGES(
    "LP SPI master read and write during deep sleep",
    "[lp_core][lp_spi_master_sleep][test_env=generic_multi_device][timeout=120][deepsleep]",
    lp_spi_master_sleeper_first,
    lp_spi_master_sleeper_second,
    lp_spi_master_sleeper_check);

TEST_CASE(
    "LP SPI master peer during deep sleep",
    "[lp_core][lp_spi_master_sleep][test_env=generic_multi_device][timeout=120]")
{
    lp_spi_master_peer();
}

TEST_CASE_MULTIPLE_STAGES(
    "LP SPI slave read and write during deep sleep",
    "[lp_core][lp_spi_slave_sleep][test_env=generic_multi_device][timeout=120][deepsleep]",
    lp_spi_slave_sleeper_first,
    lp_spi_slave_sleeper_second,
    lp_spi_slave_sleeper_check);

TEST_CASE(
    "LP SPI slave peer during deep sleep",
    "[lp_core][lp_spi_slave_sleep][test_env=generic_multi_device][timeout=120]")
{
    lp_spi_slave_peer();
}

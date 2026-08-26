/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "lp_core_test_app_spi_master.h"
#include "lp_core_test_app_spi_slave.h"
#include "ulp_lp_core.h"
#include "lp_core_spi.h"
#include "driver/rtc_io.h"
#include "hal/lp_spi_ll.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_log.h"
#include "test_shared.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern const uint8_t lp_core_main_spi_master_bin_start[] asm("_binary_lp_core_test_app_spi_master_bin_start");
extern const uint8_t lp_core_main_spi_master_bin_end[]   asm("_binary_lp_core_test_app_spi_master_bin_end");
extern const uint8_t lp_core_main_spi_slave_bin_start[] asm("_binary_lp_core_test_app_spi_slave_bin_start");
extern const uint8_t lp_core_main_spi_slave_bin_end[]   asm("_binary_lp_core_test_app_spi_slave_bin_end");

static const char* TAG = "lp_core_spi_test";

#define TEST_GPIO_PIN_MISO   6
#define TEST_GPIO_PIN_MOSI   7
#if CONFIG_IDF_TARGET_ESP32S31
#define TEST_GPIO_PIN_CLK    5  // S31 only has RTC GPIOs 0-7; GPIO 8 is not a valid LP IO
#else
#define TEST_GPIO_PIN_CLK    8
#endif
#define TEST_GPIO_PIN_CS     4

#define TEST_DATA_LEN_BYTES 42
uint8_t expected_data[100] = {0};

/* ------------------------------------------------------------------ */
/*  Cleanup: stop LP core + module-reset LP SPI + deinit LP GPIOs     */
/* ------------------------------------------------------------------ */

/* Base LP SPI bus settings */
lp_spi_host_t host_id = 0;
lp_spi_bus_config_t bus_config = {
    .miso_io_num = TEST_GPIO_PIN_MISO,
    .mosi_io_num = TEST_GPIO_PIN_MOSI,
    .sclk_io_num = TEST_GPIO_PIN_CLK,
};

/**
 * @brief Reset LP SPI peripheral and GPIO state to a known-clean baseline.
 *
 * Called at the start AND end of every SPI test so the test is
 * self-contained and resilient to whatever ran before it.
 */
static void lp_spi_test_cleanup(void)
{
    ulp_lp_core_stop();
    lp_core_lp_spi_bus_deinit(host_id, &bus_config);
    rtc_gpio_deinit(TEST_GPIO_PIN_CS);

    /* After the module reset, explicitly clear TRANS_DONE to prevent
     * a stale interrupt from being latched before the next slave_arm.
     */
    lp_spi_ll_clear_int_trans_done(LP_SPI_LL_GET_HW());

    /* Brief settle time for the LP peripheral reset to propagate. */
    vTaskDelay(pdMS_TO_TICKS(5));

    ESP_LOGI(TAG, "LP SPI cleanup done");
}

/* ------------------------------------------------------------------ */
/*  Helpers                                                           */
/* ------------------------------------------------------------------ */

static void load_and_start_lp_core_firmware(ulp_lp_core_cfg_t* cfg, const uint8_t* firmware_start, const uint8_t* firmware_end)
{
    TEST_ASSERT(ulp_lp_core_load_binary(firmware_start, (firmware_end - firmware_start)) == ESP_OK);
    TEST_ASSERT(ulp_lp_core_run(cfg) == ESP_OK);
}

static void setup_test_data(void)
{
    uint8_t *tx_data = (uint8_t *)&ulp_spi_master_tx_buf;
    ulp_spi_tx_len = TEST_DATA_LEN_BYTES;

    /* Setup test data */
    for (int i = 0; i < ulp_spi_tx_len; i++) {
        tx_data[i] = (i + 1) % 256;
        expected_data[i] = tx_data[i];
    }
}

static void setup_expected_data(void)
{
    ulp_spi_rx_len = TEST_DATA_LEN_BYTES;

    /* Setup expected data */
    for (int i = 0; i < TEST_DATA_LEN_BYTES; i++) {
        expected_data[i] = (i + 1) % 256;
    }
}

/**
 * @brief Preload the slave's TX buffer so it echoes the same pattern
 *        the master sends on MOSI back on MISO.
 */
static void setup_slave_echo_data(void)
{
    uint8_t *tx_data = (uint8_t *)&ulp_spi_slave_tx_buf;
    ulp_spi_slave_tx_len = TEST_DATA_LEN_BYTES;

    for (int i = 0; i < TEST_DATA_LEN_BYTES; i++) {
        tx_data[i] = (i + 1) % 256;
    }
}

/* Base LP SPI device settings */
lp_spi_device_config_t device = {
    .cs_io_num = TEST_GPIO_PIN_CS,
    .spi_mode = 0,
    .clock_speed_hz = 10 * 1000, // 10 kHz
    .duty_cycle = 128,  // 50% duty cycle
};

/* Base LP SPI slave device settings */
lp_spi_slave_config_t slv_device = {
    .cs_io_num = TEST_GPIO_PIN_CS,
    .spi_mode = 0,
};

static void lp_spi_master_init(int spi_flags, bool setup_master_loop_back)
{
    /* Ensure a clean peripheral state before init */
    lp_spi_test_cleanup();

    /* Initialize LP SPI bus.
     * Setup loop back for tests which do not use an LP SPI slave for looping back the data.
     */
    bus_config.miso_io_num = setup_master_loop_back ? TEST_GPIO_PIN_MOSI : TEST_GPIO_PIN_MISO;
    TEST_ASSERT(lp_core_lp_spi_bus_initialize(host_id, &bus_config) == ESP_OK);

    /* Add LP SPI device */
    device.flags = spi_flags;
    TEST_ASSERT(lp_core_lp_spi_bus_add_device(host_id, &device) == ESP_OK);
}

static void lp_spi_slave_init(int spi_flags)
{
    lp_spi_test_cleanup();

    /* Initialize LP SPI bus */
    TEST_ASSERT(lp_core_lp_spi_bus_initialize(host_id, &bus_config) == ESP_OK);

    /* Add LP SPI slave device */
    slv_device.flags = spi_flags;
    TEST_ASSERT(lp_core_lp_spi_slave_initialize(host_id, &slv_device) == ESP_OK);
}

/* ------------------------------------------------------------------ */
/*  Master-side test execution                                        */
/* ------------------------------------------------------------------ */

static void lp_spi_master_execute_test(bool wait_for_slave_ready, bool verify_rx)
{
    /* Load and run the LP core firmware */
    ulp_lp_core_cfg_t lp_cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    load_and_start_lp_core_firmware(&lp_cfg, lp_core_main_spi_master_bin_start, lp_core_main_spi_master_bin_end);

    if (wait_for_slave_ready) {
        /* Tell the slave that the master's SPI bus and GPIOs are stable.
         * The slave only arms after receiving this signal to avoid
         * spurious TRANS_DONE from SCLK glitches during the master's
         * boot / GPIO init.
         */
        unity_send_signal("LP SPI master initialized");
        /* Wait for the HP SPI device to be initialized */
        unity_wait_for_signal("LP SPI slave ready");
    }

    /* Setup test data */
    setup_test_data();

    /* Start the test */
    ulp_spi_test_cmd = LP_CORE_LP_SPI_WRITE_READ_TEST;

    /* Wait for the test to complete */
    while (ulp_spi_test_cmd != LP_CORE_NO_COMMAND) {
        vTaskDelay(1);
    }

    /* Verify the received data if we expect the data to be looped back from the LP SPI slave */
    uint8_t *rx_data = (uint8_t *)&ulp_spi_master_rx_buf;

    if (verify_rx) {
        bool mismatch = false;
        for (int i = 0; i < TEST_DATA_LEN_BYTES; i++) {
            if (rx_data[i] != expected_data[i]) {
                ESP_LOGE(TAG, "Master RX mismatch [%d]: expected 0x%02x got 0x%02x",
                         i, expected_data[i], rx_data[i]);
                mismatch = true;
            }
        }
        if (!mismatch) {
            ESP_LOGI(TAG, "Master RX: all %d bytes match", TEST_DATA_LEN_BYTES);
        }
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_data, rx_data, ulp_spi_tx_len);
    } else {
        ESP_LOGI(TAG, "Master TX-only test completed (%d bytes)", TEST_DATA_LEN_BYTES);
    }

    lp_spi_test_cleanup();
}

/* ------------------------------------------------------------------ */
/*  Slave-side test execution                                         */
/* ------------------------------------------------------------------ */

static void lp_spi_slave_execute_test(bool provide_echo)
{
    /* Wait until the master's SPI bus and GPIOs are fully initialized
     * and stable before arming the slave.  This prevents spurious
     * TRANS_DONE triggers from SCLK glitches during the master's
     * boot / GPIO init sequence (both boards are reset between tests).
     */
    unity_wait_for_signal("LP SPI master initialized");

    /* Ensure shared-memory handshake variables are in the expected
     * initial state *before* loading the binary.  LP RAM survives HP
     * resets, so stale values from a previous test can fool the
     * handshake if we don't clear them here.
     */
    ulp_spi_slave_armed = 0;
    ulp_spi_test_cmd_reply = LP_CORE_COMMAND_NOK;

    /* Load and run the LP core firmware. The LP core spins on
     * spi_test_cmd_reply == LP_CORE_COMMAND_NOK until we release it.
     */
    ulp_lp_core_cfg_t lp_cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    load_and_start_lp_core_firmware(&lp_cfg, lp_core_main_spi_slave_bin_start, lp_core_main_spi_slave_bin_end);

    /* Give the LP core a moment to boot and enter its handshake spin
     * loop before we write shared-memory buffers.
     */
    vTaskDelay(pdMS_TO_TICKS(10));

    /* Now that the binary is loaded (and the LP core is spinning), fill
     * the shared-memory buffers with test data.
     */
    setup_expected_data();

    if (provide_echo) {
        setup_slave_echo_data();
    } else {
        ulp_spi_slave_tx_len = 0;
    }

    /* Release the LP core: it will read the lengths, build its
     * transaction descriptor, and arm the hardware.
     */
    ulp_spi_test_cmd_reply = LP_CORE_COMMAND_INVALID;

    /* Wait for the slave hardware to be armed before releasing the
     * master.  Bounded wait to avoid hanging the whole test suite if the
     * LP core fails for any reason.
     */
    int armed_wait_ms = 0;
    const int armed_timeout_ms = 5000;
    while (ulp_spi_slave_armed == 0) {
        vTaskDelay(pdMS_TO_TICKS(10));
        armed_wait_ms += 10;
        if (armed_wait_ms >= armed_timeout_ms) {
            ESP_LOGE(TAG, "LP SPI slave arm timed out after %d ms", armed_timeout_ms);
            TEST_FAIL_MESSAGE("LP SPI slave did not arm in time");
        }
    }
    ESP_LOGI(TAG, "LP SPI slave armed after ~%d ms", armed_wait_ms);

    /* Send signal to LP SPI master */
    unity_send_signal("LP SPI slave ready");

    /* Wait for the test to complete */
    int done_wait_ms = 0;
    const int done_timeout_ms = 10000;
    while (ulp_spi_test_cmd_reply != LP_CORE_COMMAND_OK) {
        vTaskDelay(pdMS_TO_TICKS(10));
        done_wait_ms += 10;
        if (done_wait_ms >= done_timeout_ms) {
            ESP_LOGE(TAG, "LP SPI slave transfer timed out after %d ms", done_timeout_ms);
            TEST_FAIL_MESSAGE("LP SPI slave transfer did not complete in time");
        }
    }

    /* Verify the received data */
    uint8_t *rx_data = (uint8_t *)&ulp_spi_slave_rx_buf;
    bool mismatch = false;
    for (int i = 0; i < TEST_DATA_LEN_BYTES; i++) {
        if (rx_data[i] != expected_data[i]) {
            ESP_LOGE(TAG, "Slave RX mismatch [%d]: expected 0x%02x got 0x%02x",
                     i, expected_data[i], rx_data[i]);
            mismatch = true;
        }
    }
    if (!mismatch) {
        ESP_LOGI(TAG, "Slave RX: all %d bytes match", TEST_DATA_LEN_BYTES);
    }

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_data, rx_data, TEST_DATA_LEN_BYTES);

    lp_spi_test_cleanup();
}

/* ================================================================== */
/*  Individual test-case wrappers (master side)                       */
/* ================================================================== */

void test_lp_spi_master(void)
{
    /* Initialize LP SPI in master mode */
    lp_spi_master_init(0, false);

    /* Start the LP SPI master test */
    lp_spi_master_execute_test(true, true);
}

void test_lp_spi_master_3wire(void)
{
    /* Initialize LP SPI in master mode */
    lp_spi_master_init(LP_SPI_DEVICE_3WIRE, false);

    /* In 3-Wire SIO mode the slave does not echo, so the master
     * cannot verify RX data — only the slave side verifies RX.
     */
    lp_spi_master_execute_test(true, false);
}

void test_lp_spi_master_lsbfirst(void)
{
    /* Initialize LP SPI in master mode */
    lp_spi_master_init(LP_SPI_DEVICE_BIT_LSBFIRST, false);

    /* Start the LP SPI master test */
    lp_spi_master_execute_test(true, true);
}

/* ================================================================== */
/*  Individual test-case wrappers (slave side)                        */
/* ================================================================== */

void test_lp_spi_slave(void)
{
    /* Initialize LP SPI in slave mode */
    lp_spi_slave_init(0);

    /* Start the LP SPI slave test */
    lp_spi_slave_execute_test(true);
}

void test_lp_spi_slave_3wire(void)
{
    /* Initialize LP SPI in slave mode */
    lp_spi_slave_init(LP_SPI_DEVICE_3WIRE);

    /* Start the LP SPI slave test */
    lp_spi_slave_execute_test(false);
}

void test_lp_spi_slave_lsbfirst(void)
{
    /* Initialize LP SPI in slave mode */
    lp_spi_slave_init(LP_SPI_DEVICE_BIT_LSBFIRST);

    /* Start the LP SPI slave test */
    lp_spi_slave_execute_test(true);
}

/* ================================================================== */
/*  Loopback tests (single-device, no slave needed)                   */
/* ================================================================== */

/* Test LP-SPI master loopback */
TEST_CASE("LP-Core LP-SPI master loopback test", "[lp_core]")
{
    /* Initialize LP SPI in master mode */
    lp_spi_master_init(0, true);

    /* Start the LP SPI master test */
    lp_spi_master_execute_test(false, true);
}

/* Test LP-SPI master loopback with active high CS line */
TEST_CASE("LP-Core LP-SPI master loopback test with active high CS line", "[lp_core]")
{
    /* Initialize LP SPI in master mode */
    lp_spi_master_init(LP_SPI_DEVICE_CS_ACTIVE_HIGH, true);

    /* Start the LP SPI master test */
    lp_spi_master_execute_test(false, true);
}

/* ================================================================== */
/*  Multi-device tests                                                */
/* ================================================================== */

/* Test LP-SPI master and LP-SPI slave communication */
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-SPI master and LP-SPI slave read write test", "[lp_core_spi][test_env=generic_multi_device][timeout=150]", test_lp_spi_master, test_lp_spi_slave);

/* Test LP-SPI master in 3-Wire SPI mode */
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-SPI master and LP-SPI slave in 3-Wire SPI mode", "[lp_core_spi][test_env=generic_multi_device][timeout=150]", test_lp_spi_master_3wire, test_lp_spi_slave_3wire);

/* Test LP-SPI master and LP-SPI slave in LSB first mode */
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-SPI master and LP-SPI in LSB first SPI mode", "[lp_core_spi][test_env=generic_multi_device][timeout=150]", test_lp_spi_master_lsbfirst, test_lp_spi_slave_lsbfirst);

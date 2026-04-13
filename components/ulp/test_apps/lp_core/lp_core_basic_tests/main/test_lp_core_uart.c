/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "hal/uart_types.h"
#include "lp_core_test_app_uart.h"
#include "portmacro.h"
#include "ulp_lp_core.h"
#include "lp_core_uart.h"

#include "test_shared.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_log.h"

#include "driver/uart.h"
#include "driver/rtc_io.h"
#include "soc/soc_caps.h"
#if SOC_LIGHT_SLEEP_SUPPORTED
#include "esp_sleep.h"
#endif /* SOC_LIGHT_SLEEP_SUPPORTED */

extern const uint8_t lp_core_main_uart_bin_start[] asm("_binary_lp_core_test_app_uart_bin_start");
extern const uint8_t lp_core_main_uart_bin_end[]   asm("_binary_lp_core_test_app_uart_bin_end");

static const char *TAG = "lp_core_uart_test";

static void load_and_start_lp_core_firmware(ulp_lp_core_cfg_t *cfg,
                                            const uint8_t *firmware_start,
                                            const uint8_t *firmware_end)
{
    TEST_ASSERT(ulp_lp_core_load_binary(firmware_start,
                                        (firmware_end - firmware_start)) == ESP_OK);
    TEST_ASSERT(ulp_lp_core_run(cfg) == ESP_OK);
}

/* =========================================================================
 * Single-device: initialisation and parameter validation
 * ========================================================================= */

TEST_CASE("LP-Core LP-UART initialization test", "[lp_core]")
{
    /* Default UART configuration must succeed */
    ESP_LOGI(TAG, "Verifying default LP UART configuration");
    lp_core_uart_cfg_t cfg = LP_CORE_UART_DEFAULT_CONFIG();
    TEST_ASSERT(ESP_OK == lp_core_uart_init(&cfg));

    /* NULL configuration must fail */
    ESP_LOGI(TAG, "Verifying NULL configuration");
    TEST_ASSERT(ESP_OK != lp_core_uart_init(NULL));

    /* RX flow-control threshold > FIFO length must fail */
    ESP_LOGI(TAG, "Verifying LP UART configuration with incorrect Rx Flow Control Threshold");
    lp_core_uart_cfg_t cfg1 = LP_CORE_UART_DEFAULT_CONFIG();
    cfg1.uart_proto_cfg.rx_flow_ctrl_thresh = SOC_LP_UART_FIFO_LEN + 1;
    TEST_ASSERT(ESP_OK != lp_core_uart_init(&cfg1));

#if !SOC_LP_GPIO_MATRIX_SUPPORTED
    ESP_LOGI(TAG, "Verifying LP UART configuration with incorrect Tx IO pin");
    lp_core_uart_cfg_t cfg2 = LP_CORE_UART_DEFAULT_CONFIG();
    cfg2.uart_pin_cfg.tx_io_num++;
    TEST_ASSERT(ESP_OK != lp_core_uart_init(&cfg2));

    ESP_LOGI(TAG, "Verifying LP UART configuration with incorrect Rx IO pin");
    lp_core_uart_cfg_t cfg3 = LP_CORE_UART_DEFAULT_CONFIG();
    cfg3.uart_pin_cfg.rx_io_num--;
    TEST_ASSERT(ESP_OK != lp_core_uart_init(&cfg3));
#else
    ESP_LOGI(TAG, "Verifying LP UART configuration with non-default Tx IO pin");
    lp_core_uart_cfg_t cfg4 = LP_CORE_UART_DEFAULT_CONFIG();
    cfg4.uart_pin_cfg.tx_io_num++;
    TEST_ASSERT(ESP_OK == lp_core_uart_init(&cfg4));

    ESP_LOGI(TAG, "Verifying LP UART configuration with non-default Rx IO pin");
    lp_core_uart_cfg_t cfg5 = LP_CORE_UART_DEFAULT_CONFIG();
    cfg5.uart_pin_cfg.rx_io_num--;
    TEST_ASSERT(ESP_OK == lp_core_uart_init(&cfg5));
#endif /* !SOC_LP_GPIO_MATRIX_SUPPORTED */

    /* Deinit LP GPIO pins so they don't remain in RTC/LP mode */
    rtc_gpio_deinit(LP_UART_DEFAULT_TX_GPIO_NUM);
    rtc_gpio_deinit(LP_UART_DEFAULT_RX_GPIO_NUM);
}

TEST_CASE("LP-Core LP-UART data_bits parameter validation", "[lp_core]")
{
    /* All four valid word lengths must succeed */
    ESP_LOGI(TAG, "Verifying LP UART data_bits valid range");
    const uart_word_length_t valid_bits[] = {
        UART_DATA_5_BITS, UART_DATA_6_BITS, UART_DATA_7_BITS, UART_DATA_8_BITS,
    };
    for (int i = 0; i < (int)(sizeof(valid_bits) / sizeof(valid_bits[0])); i++) {
        lp_core_uart_cfg_t cfg = LP_CORE_UART_DEFAULT_CONFIG();
        cfg.uart_proto_cfg.data_bits = valid_bits[i];
        TEST_ASSERT_EQUAL(ESP_OK, lp_core_uart_init(&cfg));
    }

    /* UART_DATA_BITS_MAX (=4) is the sentinel value and must be rejected */
    ESP_LOGI(TAG, "Verifying LP UART data_bits = UART_DATA_BITS_MAX is rejected");
    lp_core_uart_cfg_t cfg_max = LP_CORE_UART_DEFAULT_CONFIG();
    cfg_max.uart_proto_cfg.data_bits = UART_DATA_BITS_MAX;
    TEST_ASSERT_NOT_EQUAL(ESP_OK, lp_core_uart_init(&cfg_max));

    /* Deinit LP GPIO pins to not leave them in RTC/LP mode */
    rtc_gpio_deinit(LP_UART_DEFAULT_TX_GPIO_NUM);
    rtc_gpio_deinit(LP_UART_DEFAULT_RX_GPIO_NUM);
}

/* =========================================================================
 * LP UART configurations used by multi-device tests
 * =========================================================================
 *
 * All configurations below follow the same LP_UART_DEFAULT_GPIO_CONFIG() pin
 * assignment; the HP UART mirrors whichever config the test passes in, with
 * TX/RX cross-connected so the two UARTs talk to each other.
 *
 * On SOC_LP_GPIO_MATRIX_SUPPORTED chips the test suite also covers the LP GPIO
 * Matrix routing path.  lp_uart_cfg_matrix deliberately swaps the default TX
 * and RX GPIO numbers so that each pin is non-default for the signal it carries
 * (GPIO != upin->default_gpio), forcing lp_uart_config_io() down the matrix
 * branch that calls lp_gpio_connect_in/out_signal().  The same physical
 * cross-wiring used for the default-pin tests is reused:
 *   Board-1 GPIO14 (LP RX via matrix) <-> Board-2 GPIO14 (HP TX)
 *   Board-1 GPIO15 (LP TX via matrix) <-> Board-2 GPIO15 (HP RX)
 */

/* Default: 115200 / 8-bit / no parity / 1 stop */
static lp_core_uart_cfg_t lp_uart_cfg = LP_CORE_UART_DEFAULT_CONFIG();

/* Non-default: 9600 / 8-bit / odd parity / 2 stop */
static lp_core_uart_cfg_t lp_uart_cfg1 = {
    .uart_proto_cfg.baud_rate           = 9600,
    .uart_proto_cfg.data_bits           = UART_DATA_8_BITS,
    .uart_proto_cfg.parity              = UART_PARITY_ODD,
    .uart_proto_cfg.stop_bits           = UART_STOP_BITS_2,
    .uart_proto_cfg.rx_flow_ctrl_thresh = 0,
    .uart_proto_cfg.flow_ctrl           = UART_HW_FLOWCTRL_DISABLE,
    LP_UART_DEFAULT_GPIO_CONFIG()
};

/* 5-bit word length: 115200 / 5-bit / no parity / 1 stop */
static lp_core_uart_cfg_t lp_uart_cfg_5bit = {
    .uart_proto_cfg.baud_rate           = 115200,
    .uart_proto_cfg.data_bits           = UART_DATA_5_BITS,
    .uart_proto_cfg.parity              = UART_PARITY_DISABLE,
    .uart_proto_cfg.stop_bits           = UART_STOP_BITS_1,
    .uart_proto_cfg.rx_flow_ctrl_thresh = 0,
    .uart_proto_cfg.flow_ctrl           = UART_HW_FLOWCTRL_DISABLE,
    LP_UART_DEFAULT_GPIO_CONFIG()
};

/* 6-bit word length: 115200 / 6-bit / no parity / 1 stop */
static lp_core_uart_cfg_t lp_uart_cfg_6bit = {
    .uart_proto_cfg.baud_rate           = 115200,
    .uart_proto_cfg.data_bits           = UART_DATA_6_BITS,
    .uart_proto_cfg.parity              = UART_PARITY_DISABLE,
    .uart_proto_cfg.stop_bits           = UART_STOP_BITS_1,
    .uart_proto_cfg.rx_flow_ctrl_thresh = 0,
    .uart_proto_cfg.flow_ctrl           = UART_HW_FLOWCTRL_DISABLE,
    LP_UART_DEFAULT_GPIO_CONFIG()
};

/* 7-bit word length: 115200 / 7-bit / no parity / 1 stop */
static lp_core_uart_cfg_t lp_uart_cfg_7bit = {
    .uart_proto_cfg.baud_rate           = 115200,
    .uart_proto_cfg.data_bits           = UART_DATA_7_BITS,
    .uart_proto_cfg.parity              = UART_PARITY_DISABLE,
    .uart_proto_cfg.stop_bits           = UART_STOP_BITS_1,
    .uart_proto_cfg.rx_flow_ctrl_thresh = 0,
    .uart_proto_cfg.flow_ctrl           = UART_HW_FLOWCTRL_DISABLE,
    LP_UART_DEFAULT_GPIO_CONFIG()
};

/* Even parity: 115200 / 8-bit / even parity / 1 stop */
static lp_core_uart_cfg_t lp_uart_cfg_even_parity = {
    .uart_proto_cfg.baud_rate           = 115200,
    .uart_proto_cfg.data_bits           = UART_DATA_8_BITS,
    .uart_proto_cfg.parity              = UART_PARITY_EVEN,
    .uart_proto_cfg.stop_bits           = UART_STOP_BITS_1,
    .uart_proto_cfg.rx_flow_ctrl_thresh = 0,
    .uart_proto_cfg.flow_ctrl           = UART_HW_FLOWCTRL_DISABLE,
    LP_UART_DEFAULT_GPIO_CONFIG()
};

#if SOC_LP_GPIO_MATRIX_SUPPORTED
/*
 * LP GPIO Matrix path: swap default TX/RX GPIOs so lp_uart_config_io() takes
 * the matrix branch (upin->default_gpio != pin) for both pins.
 */
static lp_core_uart_cfg_t lp_uart_cfg_matrix = {
    .uart_proto_cfg.baud_rate           = 115200,
    .uart_proto_cfg.data_bits           = UART_DATA_8_BITS,
    .uart_proto_cfg.parity              = UART_PARITY_DISABLE,
    .uart_proto_cfg.stop_bits           = UART_STOP_BITS_1,
    .uart_proto_cfg.rx_flow_ctrl_thresh = 0,
    .uart_proto_cfg.flow_ctrl           = UART_HW_FLOWCTRL_DISABLE,
    .lp_uart_source_clk                 = LP_UART_SCLK_DEFAULT,
    /* TX on the default RX GPIO, RX on the default TX GPIO — both non-default,
     * so lp_uart_config_io() routes through the LP GPIO Matrix. */
    .uart_pin_cfg.tx_io_num             = LP_UART_DEFAULT_RX_GPIO_NUM,
    .uart_pin_cfg.rx_io_num             = LP_UART_DEFAULT_TX_GPIO_NUM,
    .uart_pin_cfg.rts_io_num            = (gpio_num_t)(-1),
                 .uart_pin_cfg.cts_io_num            = (gpio_num_t)(-1),
};
#endif /* SOC_LP_GPIO_MATRIX_SUPPORTED */

/* =========================================================================
 * Global state used by the print test
 * ========================================================================= */
#define TEST_DATA_LEN 234

char test_string[25];
char test_long_string[200];
int test_signed_integer;
uint32_t test_unsigned_integer;
int test_hex;
char test_character;

/* =========================================================================
 * Data helpers
 * ========================================================================= */

/*
 * Fill 'data' with TEST_DATA_LEN bytes whose values are masked to the given
 * word length.  UART_DATA_5_BITS=0 → actual 5 bits → mask 0x1F, ...,
 * UART_DATA_8_BITS=3 → mask 0xFF.  Both the LP side and the HP side call
 * this with the same data_bits so the sent and expected arrays are identical.
 */
static void setup_test_data_nbits(uint8_t *data, uart_word_length_t data_bits)
{
    int bits = (int)data_bits + 5;
    uint8_t mask = (uint8_t)((1 << bits) - 1);
    for (int i = 0; i < TEST_DATA_LEN; i++) {
        data[i] = (uint8_t)((i + 7) & mask);
    }
}

static void setup_test_print_data(void)
{
    strcpy(test_string, "Test printf string");
    strcpy(test_long_string,
           "Print a very loooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooong string");
    test_signed_integer   = -77;
    test_unsigned_integer = 1234567890;
    test_hex              = 0xa;
    test_character        = 'n';
}

/* =========================================================================
 * Shared transport helpers — matched configuration (both sides identical)
 * =========================================================================
 *
 * These helpers accept a pointer to an lp_core_uart_cfg_t so that each
 * concrete test function (required by TEST_CASE_MULTIPLE_DEVICES to have no
 * parameters) is a one-liner wrapper.
 *
 * Data pattern: setup_test_data_nbits() generates TEST_DATA_LEN bytes whose
 * values are masked to the configured word length, ensuring they fit in the
 * frame on both the LP UART and the HP UART.  No start/end sentinel pattern
 * is embedded; instead the HP UART FIFO is clean after uart_driver_install()
 * so exact-length receive is reliable for any word width.
 */

/* Install HP UART driver mirroring the LP config; cross-connect pins */
static void hp_uart_setup_cfg(const lp_core_uart_cfg_t *cfg)
{
    uart_config_t hp_uart_cfg = {
        .baud_rate  = cfg->uart_proto_cfg.baud_rate,
        .data_bits  = cfg->uart_proto_cfg.data_bits,
        .parity     = cfg->uart_proto_cfg.parity,
        .stop_bits  = cfg->uart_proto_cfg.stop_bits,
        .flow_ctrl  = cfg->uart_proto_cfg.flow_ctrl,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;
#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, UART_BUF_SIZE, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &hp_uart_cfg));
    /* Cross-connect: HP TX → LP RX pin; HP RX ← LP TX pin */
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1,
                                 cfg->uart_pin_cfg.rx_io_num,
                                 cfg->uart_pin_cfg.tx_io_num,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

/*
 * HP side for LP-write tests (LP → HP).
 * Waits for LP UART init, installs HP UART with the same config, receives
 * exactly TEST_DATA_LEN bytes and verifies them against the N-bit pattern.
 */
static void hp_uart_read_cfg(const lp_core_uart_cfg_t *cfg)
{
    unity_wait_for_signal("LP UART init done");
    hp_uart_setup_cfg(cfg);
    unity_send_signal("HP UART init done");

    uint8_t rx_data[UART_BUF_SIZE] = {0};
    int bytes_remaining = TEST_DATA_LEN;
    int recv_idx = 0;
    while (bytes_remaining > 0) {
        int n = uart_read_bytes(UART_NUM_1, rx_data + recv_idx,
                                bytes_remaining, 10 / portTICK_PERIOD_MS);
        if (n < 0) {
            TEST_FAIL_MESSAGE("HP UART read error");
        } else if (n > 0) {
            recv_idx        += n;
            bytes_remaining -= n;
        }
    }

    uint8_t expected[TEST_DATA_LEN];
    setup_test_data_nbits(expected, cfg->uart_proto_cfg.data_bits);
    ESP_LOGI(TAG, "Verify Rx data");
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, rx_data, TEST_DATA_LEN);

    uart_driver_delete(UART_NUM_1);
    vTaskDelay(1);
}

/*
 * HP side for LP-read tests (HP → LP).
 * Waits for LP UART init, installs HP UART with the same config, sends
 * TEST_DATA_LEN bytes of N-bit-masked data, then waits for LP done signal.
 */
static void hp_uart_write_cfg(const lp_core_uart_cfg_t *cfg)
{
    unity_wait_for_signal("LP UART init done");
    hp_uart_setup_cfg(cfg);
    unity_send_signal("HP UART init done");
    unity_wait_for_signal("LP UART recv ready");

    uint8_t tx_data[TEST_DATA_LEN];
    setup_test_data_nbits(tx_data, cfg->uart_proto_cfg.data_bits);
    uart_write_bytes(UART_NUM_1, (const char *)tx_data, TEST_DATA_LEN);

    unity_wait_for_signal("LP UART recv data done");
    uart_driver_delete(UART_NUM_1);
    vTaskDelay(1);
}

/*
 * LP side for write tests.
 * Inits LP UART with cfg, loads LP core firmware, fills ulp_tx_data with
 * N-bit-masked test data, and triggers LP_CORE_LP_UART_WRITE_TEST.
 */
static void test_lp_uart_write_cfg(const lp_core_uart_cfg_t *cfg)
{
    TEST_ASSERT_EQUAL(ESP_OK, lp_core_uart_init(cfg));
    unity_send_signal("LP UART init done");
    unity_wait_for_signal("HP UART init done");

    ulp_lp_core_cfg_t lp_cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    load_and_start_lp_core_firmware(&lp_cfg,
                                    lp_core_main_uart_bin_start,
                                    lp_core_main_uart_bin_end);

    setup_test_data_nbits((uint8_t *)&ulp_tx_data, cfg->uart_proto_cfg.data_bits);
    ulp_tx_len = TEST_DATA_LEN;

#if SOC_LIGHT_SLEEP_SUPPORTED
    esp_sleep_enable_ulp_wakeup();
    /* PMU_LP_TRIGGER_HP is a WT (Write-Trigger / edge-triggered) bit.  If the
     * LP core completes the write and fires the wakeup pulse before the HP core
     * has fully entered light sleep the pulse is lost and the HP core would
     * sleep indefinitely.  A 3-second timer wakeup acts as a safety net: when
     * the race does not occur the ULP wakeup fires first (~20 ms); when it is
     * lost the timer fires after 3 s, the HP core checks ulp_test_cmd_reply
     * (already LP_CORE_COMMAND_OK) and the test still passes. */
    esp_sleep_enable_timer_wakeup(3 * 1000 * 1000ULL);
#endif /* SOC_LIGHT_SLEEP_SUPPORTED */

    ESP_LOGI(TAG, "Write test start");
    ulp_test_cmd = LP_CORE_LP_UART_WRITE_TEST;

#if SOC_LIGHT_SLEEP_SUPPORTED
    esp_light_sleep_start();
#endif /* SOC_LIGHT_SLEEP_SUPPORTED */

    TEST_ASSERT_EQUAL(LP_CORE_COMMAND_OK, ulp_test_cmd_reply);
}

/*
 * LP side for read tests.
 * Inits LP UART with cfg, loads LP core firmware, triggers
 * LP_CORE_LP_UART_MULTI_BYTE_READ_TEST to receive exactly TEST_DATA_LEN bytes,
 * waits for completion, then verifies the N-bit-masked pattern.
 */
static void test_lp_uart_read_cfg(const lp_core_uart_cfg_t *cfg)
{
    TEST_ASSERT_EQUAL(ESP_OK, lp_core_uart_init(cfg));
    unity_send_signal("LP UART init done");
    unity_wait_for_signal("HP UART init done");

    ulp_lp_core_cfg_t lp_cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    load_and_start_lp_core_firmware(&lp_cfg,
                                    lp_core_main_uart_bin_start,
                                    lp_core_main_uart_bin_end);

    uint8_t expected[TEST_DATA_LEN];
    setup_test_data_nbits(expected, cfg->uart_proto_cfg.data_bits);
    ulp_rx_len = TEST_DATA_LEN;

    ESP_LOGI(TAG, "Read test start");
    ulp_test_cmd = LP_CORE_LP_UART_MULTI_BYTE_READ_TEST;
    vTaskDelay(10);

    unity_send_signal("LP UART recv ready");

    while (ulp_test_cmd_reply != LP_CORE_COMMAND_OK) {
        vTaskDelay(10);
    }

    ESP_LOGI(TAG, "Verify Rx data");
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, (uint8_t *)&ulp_rx_data, TEST_DATA_LEN);
    unity_send_signal("LP UART recv data done");
}

/* =========================================================================
 * Negative test helpers — deliberate configuration mismatch
 * =========================================================================
 *
 * These tests verify that the LP UART driver detects errors and recovers
 * without hanging or crashing when both sides are not in protocol agreement.
 *
 * Scenario A — LP-RX mismatch (FRAM_ERR path):
 *   LP UART is configured in 5-bit mode.  The HP UART is configured in
 *   8-bit mode and sends values that are 5-bit-masked (all ≤ 0x1F, so data
 *   bit D5 is always 0).  After the LP receiver reads 5 data bits it looks
 *   for a stop bit; it finds D5=0 instead → UART_INTR_FRAM_ERR fires on
 *   every frame.  lp_core_uart_read_bytes() returns -1; the firmware loop
 *   inside LP_CORE_LP_UART_MULTI_BYTE_READ_TEST breaks immediately and the
 *   LP core sets test_cmd_reply = LP_CORE_COMMAND_OK.
 *   The HP side verifies: LP core did not hang, and ulp_rx_data does not
 *   match the expected values (it stays zero-initialised).
 *
 * Scenario B — LP-TX mismatch (garbled receive on HP):
 *   LP UART sends 5-bit frames; HP UART reads in 8-bit mode.  Because the
 *   frame boundaries differ, the HP receives misaligned data that does not
 *   match the expected 5-bit pattern.
 */

/*
 * HP side for scenario A.
 * Installs HP UART with the mismatched (wider) config, sends 5-bit-masked
 * values so that D5=0 on every byte, guaranteeing FRAM_ERR on the LP side.
 */
static void hp_uart_write_mismatch_cfg(const lp_core_uart_cfg_t *lp_cfg,
                                       const lp_core_uart_cfg_t *hp_cfg)
{
    unity_wait_for_signal("LP UART init done");
    hp_uart_setup_cfg(hp_cfg);     /* HP deliberately uses the wrong word length */
    unity_send_signal("HP UART init done");
    unity_wait_for_signal("LP UART recv ready");

    /*
     * Send values masked to the LP's narrower word length so that every byte
     * has D5=0.  The LP receiver interprets D5 as a stop bit; D5=0 is an
     * invalid stop → FRAM_ERR on every frame.
     */
    uint8_t tx_data[TEST_DATA_LEN];
    setup_test_data_nbits(tx_data, lp_cfg->uart_proto_cfg.data_bits);
    uart_write_bytes(UART_NUM_1, (const char *)tx_data, TEST_DATA_LEN);

    unity_wait_for_signal("LP UART recv data done");
    uart_driver_delete(UART_NUM_1);
    vTaskDelay(1);
}

/*
 * LP side for scenario A.
 * Configures LP UART in 5-bit mode, triggers the multi-byte read, then
 * verifies that (a) the LP core replied without hanging, and (b) the receive
 * buffer does not contain the expected data (FRAM_ERR aborted the read).
 */
static void test_lp_uart_read_mismatch_cfg(const lp_core_uart_cfg_t *lp_cfg)
{
    TEST_ASSERT_EQUAL(ESP_OK, lp_core_uart_init(lp_cfg));
    unity_send_signal("LP UART init done");
    unity_wait_for_signal("HP UART init done");

    ulp_lp_core_cfg_t lp_core_cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    load_and_start_lp_core_firmware(&lp_core_cfg,
                                    lp_core_main_uart_bin_start,
                                    lp_core_main_uart_bin_end);

    uint8_t expected[TEST_DATA_LEN];
    setup_test_data_nbits(expected, lp_cfg->uart_proto_cfg.data_bits);
    ulp_rx_len = TEST_DATA_LEN;

    ESP_LOGI(TAG, "Mismatch read test start (expect FRAM_ERR, driver must recover)");
    ulp_test_cmd = LP_CORE_LP_UART_MULTI_BYTE_READ_TEST;
    vTaskDelay(10);

    unity_send_signal("LP UART recv ready");

    /* LP core must reply without hanging — FRAM_ERR causes read loop to break */
    int wait_count = 0;
    while (ulp_test_cmd_reply != LP_CORE_COMMAND_OK && wait_count < 100) {
        vTaskDelay(10);
        wait_count++;
    }
    TEST_ASSERT_EQUAL_MESSAGE(LP_CORE_COMMAND_OK, ulp_test_cmd_reply,
                              "LP core hung — driver did not recover from framing error");

    /*
     * ulp_rx_data starts zero-initialised (re-loaded with firmware).
     * expected[0] = (0+7)&0x1F = 7, so any match would be surprising.
     * A single differing byte is enough to confirm the error was detected.
     */
    ESP_LOGI(TAG, "Verify Rx buffer is corrupt (FRAM_ERR aborted read early)");
    bool data_matches = (memcmp((uint8_t *)&ulp_rx_data, expected, TEST_DATA_LEN) == 0);
    TEST_ASSERT_FALSE_MESSAGE(data_matches,
                              "LP UART received correct data despite word-length mismatch");

    unity_send_signal("LP UART recv data done");
}

/*
 * HP side for scenario B.
 * Installs HP UART in the wrong (wider) word length and tries to receive
 * TEST_DATA_LEN bytes.  With mismatched framing the HP receives garbled data;
 * the test verifies that the received content does not match the 5-bit pattern.
 */
static void hp_uart_read_mismatch_cfg(const lp_core_uart_cfg_t *lp_cfg,
                                      const lp_core_uart_cfg_t *hp_cfg)
{
    unity_wait_for_signal("LP UART init done");
    hp_uart_setup_cfg(hp_cfg);     /* HP deliberately uses the wrong word length */
    unity_send_signal("HP UART init done");

    /* Collect whatever the HP receives within a bounded window */
    uint8_t rx_data[UART_BUF_SIZE] = {0};
    int recv_idx  = 0;
    int idle_count = 0;
    while (recv_idx < TEST_DATA_LEN && idle_count < 20) {
        int n = uart_read_bytes(UART_NUM_1, rx_data + recv_idx,
                                TEST_DATA_LEN - recv_idx, 10 / portTICK_PERIOD_MS);
        if (n > 0) {
            recv_idx  += n;
            idle_count = 0;
        } else {
            idle_count++;
            vTaskDelay(10);
        }
    }

    uint8_t expected[TEST_DATA_LEN];
    setup_test_data_nbits(expected, lp_cfg->uart_proto_cfg.data_bits);

    /*
     * With mismatched framing the HP cannot receive the same sequence the LP
     * sent.  Either fewer bytes arrive (HP sees framing errors) or the values
     * are reinterpreted across different bit boundaries.  Either way the data
     * must differ from the expected 5-bit pattern.
     */
    bool all_match = (recv_idx == TEST_DATA_LEN) &&
                     (memcmp(rx_data, expected, TEST_DATA_LEN) == 0);
    ESP_LOGI(TAG, "Received %d bytes; all_match=%d (expected: false)", recv_idx, (int)all_match);
    TEST_ASSERT_FALSE_MESSAGE(all_match,
                              "HP received correct data despite word-length mismatch");

    uart_driver_delete(UART_NUM_1);
    vTaskDelay(1);
}

/* =========================================================================
 * Print test helpers (standalone: not parameterisable over word length)
 * ========================================================================= */

static void hp_uart_read_print(void)
{
    unity_wait_for_signal("LP UART init done");

    uart_config_t hp_uart_cfg = {
        .baud_rate  = lp_uart_cfg.uart_proto_cfg.baud_rate,
        .data_bits  = lp_uart_cfg.uart_proto_cfg.data_bits,
        .parity     = lp_uart_cfg.uart_proto_cfg.parity,
        .stop_bits  = lp_uart_cfg.uart_proto_cfg.stop_bits,
        .flow_ctrl  = lp_uart_cfg.uart_proto_cfg.flow_ctrl,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;
#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, UART_BUF_SIZE, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &hp_uart_cfg));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1,
                                 lp_uart_cfg.uart_pin_cfg.rx_io_num,
                                 lp_uart_cfg.uart_pin_cfg.tx_io_num,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    unity_send_signal("HP UART init done");

    setup_test_print_data();

    uint8_t rx_data[UART_BUF_SIZE];
    int recv_idx  = 0;
    int idle_count = 0;
    while (1) {
        int n = uart_read_bytes(UART_NUM_1, rx_data + recv_idx,
                                UART_BUF_SIZE, 10 / portTICK_PERIOD_MS);
        if (n < 0) {
            TEST_FAIL_MESSAGE("HP UART read error");
        } else if (n > 0) {
            recv_idx += n;
        } else {
            idle_count++;
            vTaskDelay(10);
            if (idle_count > 10) {
                break;
            }
        }
    }
    rx_data[UART_BUF_SIZE - 1] = '\0';

    char expected_string[TEST_DATA_LEN];
    snprintf(expected_string, TEST_DATA_LEN, "%s", test_string);
    TEST_ASSERT_NOT_NULL(strstr((const char *)rx_data, expected_string));

    snprintf(expected_string, TEST_DATA_LEN, "%s", test_long_string);
    TEST_ASSERT_NOT_NULL(strstr((const char *)rx_data, expected_string));

    snprintf(expected_string, TEST_DATA_LEN, "Test printf signed integer %d", test_signed_integer);
    TEST_ASSERT_NOT_NULL(strstr((const char *)rx_data, expected_string));

    snprintf(expected_string, TEST_DATA_LEN, "Test printf unsigned integer %lu", test_unsigned_integer);
    TEST_ASSERT_NOT_NULL(strstr((const char *)rx_data, expected_string));

    snprintf(expected_string, TEST_DATA_LEN, "Test printf hex %x", test_hex);
    TEST_ASSERT_NOT_NULL(strstr((const char *)rx_data, expected_string));

    snprintf(expected_string, TEST_DATA_LEN, "Test printf character %c", test_character);
    TEST_ASSERT_NOT_NULL(strstr((const char *)rx_data, expected_string));

    uart_driver_delete(UART_NUM_1);
    vTaskDelay(1);
}

static void test_lp_uart_print(void)
{
    TEST_ASSERT(ESP_OK == lp_core_uart_init(&lp_uart_cfg));
    unity_send_signal("LP UART init done");
    unity_wait_for_signal("HP UART init done");

    ulp_lp_core_cfg_t lp_cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    load_and_start_lp_core_firmware(&lp_cfg,
                                    lp_core_main_uart_bin_start,
                                    lp_core_main_uart_bin_end);

    setup_test_print_data();
    strcpy((char *)&ulp_test_string, test_string);
    strcpy((char *)&ulp_test_long_string, test_long_string);
    ulp_test_signed_integer   = test_signed_integer;
    ulp_test_unsigned_integer = test_unsigned_integer;
    ulp_test_hex              = test_hex;
    ulp_test_character        = test_character;

#if SOC_LIGHT_SLEEP_SUPPORTED
    esp_sleep_enable_ulp_wakeup();
    /* Same safety-net timer as in test_lp_uart_write_cfg: handles the race
     * where the LP core fires the edge-triggered wakeup pulse before the HP
     * core has entered light sleep. */
    esp_sleep_enable_timer_wakeup(3 * 1000 * 1000ULL);
#endif /* SOC_LIGHT_SLEEP_SUPPORTED */

    ESP_LOGI(TAG, "LP Core print test start");
    ulp_test_cmd = LP_CORE_LP_UART_PRINT_TEST;

#if SOC_LIGHT_SLEEP_SUPPORTED
    esp_light_sleep_start();
#endif /* SOC_LIGHT_SLEEP_SUPPORTED */

    TEST_ASSERT_EQUAL(LP_CORE_COMMAND_OK, ulp_test_cmd_reply);
}

/* =========================================================================
 * Concrete (no-argument) wrappers — required by TEST_CASE_MULTIPLE_DEVICES
 * ========================================================================= */

/* --- Default config (115200 / 8N1) --- */
static void test_lp_uart_write_default(void)
{
    test_lp_uart_write_cfg(&lp_uart_cfg);
}
static void hp_uart_read_default(void)
{
    hp_uart_read_cfg(&lp_uart_cfg);
}
static void test_lp_uart_read_default(void)
{
    test_lp_uart_read_cfg(&lp_uart_cfg);
}
static void hp_uart_write_default(void)
{
    hp_uart_write_cfg(&lp_uart_cfg);
}

/* --- Non-default config (9600 / 8 / odd / 2-stop) --- */
static void test_lp_uart_write_options(void)
{
    test_lp_uart_write_cfg(&lp_uart_cfg1);
}
static void hp_uart_read_options(void)
{
    hp_uart_read_cfg(&lp_uart_cfg1);
}
static void test_lp_uart_read_options(void)
{
    test_lp_uart_read_cfg(&lp_uart_cfg1);
}
static void hp_uart_write_options(void)
{
    hp_uart_write_cfg(&lp_uart_cfg1);
}

/* --- 5-bit word length --- */
static void test_lp_uart_write_5bit(void)
{
    test_lp_uart_write_cfg(&lp_uart_cfg_5bit);
}
static void hp_uart_read_5bit(void)
{
    hp_uart_read_cfg(&lp_uart_cfg_5bit);
}
static void test_lp_uart_read_5bit(void)
{
    test_lp_uart_read_cfg(&lp_uart_cfg_5bit);
}
static void hp_uart_write_5bit(void)
{
    hp_uart_write_cfg(&lp_uart_cfg_5bit);
}

/* --- 6-bit word length --- */
static void test_lp_uart_write_6bit(void)
{
    test_lp_uart_write_cfg(&lp_uart_cfg_6bit);
}
static void hp_uart_read_6bit(void)
{
    hp_uart_read_cfg(&lp_uart_cfg_6bit);
}
static void test_lp_uart_read_6bit(void)
{
    test_lp_uart_read_cfg(&lp_uart_cfg_6bit);
}
static void hp_uart_write_6bit(void)
{
    hp_uart_write_cfg(&lp_uart_cfg_6bit);
}

/* --- 7-bit word length --- */
static void test_lp_uart_write_7bit(void)
{
    test_lp_uart_write_cfg(&lp_uart_cfg_7bit);
}
static void hp_uart_read_7bit(void)
{
    hp_uart_read_cfg(&lp_uart_cfg_7bit);
}
static void test_lp_uart_read_7bit(void)
{
    test_lp_uart_read_cfg(&lp_uart_cfg_7bit);
}
static void hp_uart_write_7bit(void)
{
    hp_uart_write_cfg(&lp_uart_cfg_7bit);
}

/* --- Even parity (115200 / 8E1) --- */
static void test_lp_uart_write_even_parity(void)
{
    test_lp_uart_write_cfg(&lp_uart_cfg_even_parity);
}
static void hp_uart_read_even_parity(void)
{
    hp_uart_read_cfg(&lp_uart_cfg_even_parity);
}
static void test_lp_uart_read_even_parity(void)
{
    test_lp_uart_read_cfg(&lp_uart_cfg_even_parity);
}
static void hp_uart_write_even_parity(void)
{
    hp_uart_write_cfg(&lp_uart_cfg_even_parity);
}

#if SOC_LP_GPIO_MATRIX_SUPPORTED
/* --- LP GPIO Matrix path (non-default pins, swapped TX/RX GPIOs) --- */
static void test_lp_uart_write_matrix(void)
{
    test_lp_uart_write_cfg(&lp_uart_cfg_matrix);
}
static void hp_uart_read_matrix(void)
{
    hp_uart_read_cfg(&lp_uart_cfg_matrix);
}
static void test_lp_uart_read_matrix(void)
{
    test_lp_uart_read_cfg(&lp_uart_cfg_matrix);
}
static void hp_uart_write_matrix(void)
{
    hp_uart_write_cfg(&lp_uart_cfg_matrix);
}
#endif /* SOC_LP_GPIO_MATRIX_SUPPORTED */

/* --- Negative: 5-bit LP RX, 8-bit HP TX (FRAM_ERR recovery) --- */
static void test_lp_uart_read_mismatch_wl(void)
{
    test_lp_uart_read_mismatch_cfg(&lp_uart_cfg_5bit);
}
static void hp_uart_write_mismatch_wl(void)
{
    hp_uart_write_mismatch_cfg(&lp_uart_cfg_5bit, &lp_uart_cfg);
}

/* --- Negative: 5-bit LP TX, 8-bit HP RX (garbled data on HP) --- */
static void test_lp_uart_write_mismatch_wl(void)
{
    test_lp_uart_write_cfg(&lp_uart_cfg_5bit);
}
static void hp_uart_read_mismatch_wl(void)
{
    hp_uart_read_mismatch_cfg(&lp_uart_cfg_5bit, &lp_uart_cfg);
}

/* =========================================================================
 * Test case registrations
 * ========================================================================= */

/* Write tests (LP → HP) */
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART write test - default config",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_write_default, hp_uart_read_default);
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART write test - optional config",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_write_options, hp_uart_read_options);
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART write test - 5-bit data",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_write_5bit, hp_uart_read_5bit);
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART write test - 6-bit data",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_write_6bit, hp_uart_read_6bit);
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART write test - 7-bit data",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_write_7bit, hp_uart_read_7bit);
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART write test - even parity",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_write_even_parity, hp_uart_read_even_parity);

/* Read tests (HP → LP) */
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART read test - default config",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_read_default, hp_uart_write_default);
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART read test - optional config",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_read_options, hp_uart_write_options);
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART read test - 5-bit data",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_read_5bit, hp_uart_write_5bit);
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART read test - 6-bit data",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_read_6bit, hp_uart_write_6bit);
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART read test - 7-bit data",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_read_7bit, hp_uart_write_7bit);
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART read test - even parity",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_read_even_parity, hp_uart_write_even_parity);

/* Negative tests */
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART read test - word length mismatch FRAM_ERR recovery",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_read_mismatch_wl, hp_uart_write_mismatch_wl);
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART write test - word length mismatch garbled HP receive",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_write_mismatch_wl, hp_uart_read_mismatch_wl);

/* LP GPIO Matrix path tests (SOC_LP_GPIO_MATRIX_SUPPORTED chips only) */
#if SOC_LP_GPIO_MATRIX_SUPPORTED
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART write test - LP GPIO Matrix routing",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_write_matrix, hp_uart_read_matrix);
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART read test - LP GPIO Matrix routing",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_read_matrix, hp_uart_write_matrix);
#endif /* SOC_LP_GPIO_MATRIX_SUPPORTED */

/* Print test */
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART print test",
                           "[lp_core][uart][test_env=generic_multi_device][timeout=150]",
                           test_lp_uart_print, hp_uart_read_print);

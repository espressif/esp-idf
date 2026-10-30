/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "unity.h"
#include "test_utils.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "ulp_lp_core.h"
#include "hal/lp_core_ll.h"
#include "esp_attr.h"
#include "lp_sleep_common.h"
#include "lp_sleep_payload.h"
#include "lp_core_test_app_gpio_io_sleep.h"
#include "lp_core_test_app_gpio_isr_sleep.h"
#if CONFIG_SOC_RTC_GPIO_EDGE_WAKEUP_SUPPORTED
#include "lp_core_test_app_gpio_wkup_sleep.h"
#endif

extern const uint8_t lp_core_test_app_gpio_io_sleep_bin_start[] asm("_binary_lp_core_test_app_gpio_io_sleep_bin_start");
extern const uint8_t lp_core_test_app_gpio_io_sleep_bin_end[]   asm("_binary_lp_core_test_app_gpio_io_sleep_bin_end");
extern const uint8_t lp_core_test_app_gpio_isr_sleep_bin_start[] asm("_binary_lp_core_test_app_gpio_isr_sleep_bin_start");
extern const uint8_t lp_core_test_app_gpio_isr_sleep_bin_end[]   asm("_binary_lp_core_test_app_gpio_isr_sleep_bin_end");
#if CONFIG_SOC_RTC_GPIO_EDGE_WAKEUP_SUPPORTED
extern const uint8_t lp_core_test_app_gpio_wkup_sleep_bin_start[] asm("_binary_lp_core_test_app_gpio_wkup_sleep_bin_start");
extern const uint8_t lp_core_test_app_gpio_wkup_sleep_bin_end[]   asm("_binary_lp_core_test_app_gpio_wkup_sleep_bin_end");
#endif

#define LP_SLEEP_GPIO_PIN ((gpio_num_t)CONFIG_TEST_LP_GPIO_SLEEP_PIN)
#define GPIO_PEER_LOW_MS 300
#define GPIO_PEER_PULSE_WAIT_MS 5000

static volatile uint32_t s_peer_falling_edges;

static void gpio_peer_drive(int level)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LP_SLEEP_GPIO_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    TEST_ESP_OK(gpio_config(&io_conf));
    TEST_ESP_OK(gpio_set_level(LP_SLEEP_GPIO_PIN, level));
}

static void gpio_peer_release_high(void)
{
    TEST_ESP_OK(gpio_set_pull_mode(LP_SLEEP_GPIO_PIN, GPIO_PULLUP_ONLY));
    TEST_ESP_OK(gpio_set_direction(LP_SLEEP_GPIO_PIN, GPIO_MODE_INPUT));
}

static void IRAM_ATTR gpio_peer_count_edge(void *arg)
{
    (void)arg;
    s_peer_falling_edges++;
}

static void gpio_peer_arm_pulse_count(void)
{
    s_peer_falling_edges = 0;
    TEST_ESP_OK(gpio_set_intr_type(LP_SLEEP_GPIO_PIN, GPIO_INTR_NEGEDGE));
    TEST_ESP_OK(gpio_intr_enable(LP_SLEEP_GPIO_PIN));
}

static uint32_t gpio_peer_wait_pulses(void)
{
    for (int waited = 0; waited < GPIO_PEER_PULSE_WAIT_MS && s_peer_falling_edges < LP_SLEEP_GPIO_PULSES;
            waited += 100) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    TEST_ESP_OK(gpio_intr_disable(LP_SLEEP_GPIO_PIN));
    return s_peer_falling_edges;
}

static void gpio_sleeper_input(bool full_init)
{
    if (full_init) {
        TEST_ESP_OK(rtc_gpio_init(LP_SLEEP_GPIO_PIN));
        TEST_ESP_OK(rtc_gpio_pulldown_dis(LP_SLEEP_GPIO_PIN));
        TEST_ESP_OK(rtc_gpio_pullup_dis(LP_SLEEP_GPIO_PIN));
    }
    TEST_ESP_OK(rtc_gpio_set_direction(LP_SLEEP_GPIO_PIN, RTC_GPIO_MODE_INPUT_ONLY));
    lp_sleep_keep_pin(LP_SLEEP_GPIO_PIN);
}

static void lp_gpio_io_check(void)
{
    printf("LP GPIO saw low %" PRIu32 ", high %" PRIu32 ", drove %" PRIu32 " pulses\n",
           ulp_gpio_io_saw_low, ulp_gpio_io_saw_high, ulp_gpio_io_pulses);
    TEST_ASSERT_MESSAGE(ulp_gpio_io_saw_low, "LP core did not see the peer drive the pin low");
    TEST_ASSERT_MESSAGE(ulp_gpio_io_saw_high, "LP core did not see the peer release the pin");
    TEST_ASSERT_EQUAL_UINT32(LP_SLEEP_GPIO_PULSES, ulp_gpio_io_pulses);
}

static void lp_gpio_io_sleeper_first(void)
{
    gpio_sleeper_input(true);
    lp_sleep_run(lp_core_test_app_gpio_io_sleep_bin_start, lp_core_test_app_gpio_io_sleep_bin_end,
                 ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, true);
    hp_core_deep_sleep_enter();
}

static void lp_gpio_io_sleeper_second(void)
{
    lp_sleep_expect_ulp_wakeup();
    lp_gpio_io_check();

    /* Second cycle keeps the RTC GPIO block from the first cycle. */
    gpio_sleeper_input(false);
    lp_sleep_run(lp_core_test_app_gpio_io_sleep_bin_start, lp_core_test_app_gpio_io_sleep_bin_end,
                 ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, false);
    hp_core_deep_sleep_enter();
}

static void lp_gpio_io_sleeper_check(void)
{
    lp_sleep_expect_ulp_wakeup();
    lp_gpio_io_check();
}

static void lp_gpio_io_peer(void)
{
    TEST_ESP_OK(gpio_install_isr_service(0));
    TEST_ESP_OK(gpio_isr_handler_add(LP_SLEEP_GPIO_PIN, gpio_peer_count_edge, NULL));

    for (int cycle = 0; cycle < 2; cycle++) {
        gpio_peer_drive(1);
        printf("GPIO_IO_PEER_ARMED %d\n", cycle);
        fflush(stdout);
        unity_wait_for_signal("drive");

        TEST_ESP_OK(gpio_set_level(LP_SLEEP_GPIO_PIN, 0));
        vTaskDelay(pdMS_TO_TICKS(GPIO_PEER_LOW_MS));
        /* Arm before release. The LP core starts pulsing as soon as it sees
         * the pin go high, so a counter enabled afterwards misses the first edge. */
        gpio_peer_arm_pulse_count();
        gpio_peer_release_high();

        uint32_t pulses = gpio_peer_wait_pulses();
        printf("Peer counted %" PRIu32 " LP GPIO pulses\n", pulses);
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(LP_SLEEP_GPIO_PULSES, pulses, "LP GPIO pulses not seen while HP was asleep");
        printf("GPIO_IO_PEER_OK %d\n", cycle);
        fflush(stdout);
    }

    TEST_ESP_OK(gpio_isr_handler_remove(LP_SLEEP_GPIO_PIN));
    gpio_uninstall_isr_service();
}

static void lp_gpio_isr_sleeper_enter(void)
{
    gpio_sleeper_input(true);
    lp_sleep_run(lp_core_test_app_gpio_isr_sleep_bin_start, lp_core_test_app_gpio_isr_sleep_bin_end,
                 ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, true);
    hp_core_deep_sleep_enter();
}

static void lp_gpio_isr_sleeper_check(void)
{
    lp_sleep_expect_ulp_wakeup();
    printf("LP GPIO ISR count: %" PRIu32 "\n", ulp_gpio_isr_counter);
    TEST_ASSERT_MESSAGE(ulp_gpio_isr_counter >= 1, "LP GPIO interrupt did not run during deep sleep");
}

static void lp_gpio_isr_peer(void)
{
    gpio_peer_drive(1);
    printf("GPIO_ISR_PEER_ARMED\n");
    fflush(stdout);
    unity_wait_for_signal("edge");
    TEST_ESP_OK(gpio_set_level(LP_SLEEP_GPIO_PIN, 0));
    printf("GPIO_ISR_PEER_LOW\n");
    fflush(stdout);
}

#if CONFIG_SOC_RTC_GPIO_EDGE_WAKEUP_SUPPORTED
static void lp_gpio_wkup_sleeper_enter(void)
{
    TEST_ESP_OK(rtc_gpio_init(LP_SLEEP_GPIO_PIN));
    TEST_ESP_OK(rtc_gpio_set_direction(LP_SLEEP_GPIO_PIN, RTC_GPIO_MODE_INPUT_ONLY));
    TEST_ESP_OK(rtc_gpio_pulldown_dis(LP_SLEEP_GPIO_PIN));
    TEST_ESP_OK(rtc_gpio_pullup_en(LP_SLEEP_GPIO_PIN));
    TEST_ESP_OK(rtc_gpio_wakeup_enable(LP_SLEEP_GPIO_PIN, GPIO_INTR_NEGEDGE));
    /* Leave sleep-sel enabled. Disabling it keeps the HP mux, which P4 powers
     * down, so the LP IO wakeup detector never sees the falling edge. */

    lp_sleep_run(lp_core_test_app_gpio_wkup_sleep_bin_start, lp_core_test_app_gpio_wkup_sleep_bin_end,
                 ULP_LP_CORE_WAKEUP_SOURCE_LP_IO, true);
    hp_core_deep_sleep_enter();
}

static void lp_gpio_wkup_sleeper_check(void)
{
    lp_sleep_expect_ulp_wakeup();
    printf("LP GPIO wakeup cause: 0x%" PRIx32 "\n", ulp_gpio_wkup_cause);
    TEST_ASSERT_EQUAL_HEX32(LP_CORE_LL_WAKEUP_SOURCE_LP_IO, ulp_gpio_wkup_cause);
}

static void lp_gpio_wkup_peer(void)
{
    gpio_peer_drive(1);
    printf("GPIO_WKUP_PEER_ARMED\n");
    fflush(stdout);
    unity_wait_for_signal("edge");
    TEST_ESP_OK(gpio_set_level(LP_SLEEP_GPIO_PIN, 0));
    printf("GPIO_WKUP_PEER_LOW\n");
    fflush(stdout);
}
#endif

TEST_CASE_MULTIPLE_STAGES(
    "LP GPIO input and output during deep sleep",
    "[lp_core][lp_gpio_io_sleep][test_env=generic_multi_device][timeout=120][deepsleep]",
    lp_gpio_io_sleeper_first,
    lp_gpio_io_sleeper_second,
    lp_gpio_io_sleeper_check);

TEST_CASE(
    "LP GPIO IO peer during deep sleep",
    "[lp_core][lp_gpio_io_sleep][test_env=generic_multi_device][timeout=120]")
{
    lp_gpio_io_peer();
}

TEST_CASE_MULTIPLE_STAGES(
    "LP GPIO interrupt during deep sleep",
    "[lp_core][lp_gpio_isr_sleep][test_env=generic_multi_device][timeout=120][deepsleep]",
    lp_gpio_isr_sleeper_enter,
    lp_gpio_isr_sleeper_check);

TEST_CASE(
    "LP GPIO interrupt peer during deep sleep",
    "[lp_core][lp_gpio_isr_sleep][test_env=generic_multi_device][timeout=120]")
{
    lp_gpio_isr_peer();
}

#if CONFIG_SOC_RTC_GPIO_EDGE_WAKEUP_SUPPORTED
TEST_CASE_MULTIPLE_STAGES(
    "LP GPIO wakeup from deep sleep",
    "[lp_core][lp_gpio_wkup_sleep][test_env=generic_multi_device][timeout=120][deepsleep]",
    lp_gpio_wkup_sleeper_enter,
    lp_gpio_wkup_sleeper_check);

TEST_CASE(
    "LP GPIO wakeup peer during deep sleep",
    "[lp_core][lp_gpio_wkup_sleep][test_env=generic_multi_device][timeout=120]")
{
    lp_gpio_wkup_peer();
}
#endif

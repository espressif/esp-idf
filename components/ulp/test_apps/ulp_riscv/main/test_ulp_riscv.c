/*
 * SPDX-FileCopyrightText: 2010-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "esp_sleep.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc_periph.h"
#include "ulp_riscv.h"
#include "ulp_riscv_lock.h"
#include "ulp_adc.h"
#include "ulp_test_app.h"
#include "ulp_test_app2.h"
#include "ulp_test_shared.h"
#include "unity.h"
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_oneshot.h"
#include "ulp_riscv_cpu_freq_shared.h"
#include "esp_timer.h"

#define ULP_WAKEUP_PERIOD 1000000 // 1 second

// First ULP firmware
extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_test_app_bin_start");
extern const size_t ulp_main_bin_length asm("ulp_test_app_bin_length");
static bool  firmware_loaded = false;

// Second ULP firmware
extern const uint8_t ulp_test_app2_bin_start[] asm("_binary_ulp_test_app2_bin_start");
extern const size_t ulp_test_app2_bin_length asm("ulp_test_app2_bin_length");

// Faulty ULP firmware
extern const uint8_t ulp_test_app3_bin_start[] asm("_binary_ulp_test_app3_bin_start");
extern const size_t ulp_test_app3_bin_length asm("ulp_test_app3_bin_length");

static void load_and_start_ulp_firmware(const uint8_t* ulp_bin, size_t ulp_bin_len)
{
    if (!firmware_loaded) {
        TEST_ASSERT(ulp_riscv_load_binary(ulp_bin, ulp_bin_len) == ESP_OK);
        TEST_ASSERT(ulp_set_wakeup_period(0, ULP_WAKEUP_PERIOD) == ESP_OK);
        TEST_ASSERT(ulp_riscv_run() == ESP_OK);

        firmware_loaded = true;
        printf("New ULP firmware loaded\n");
    }
}

TEST_CASE("ULP-RISC-V and main CPU are able to exchange data", "[ulp]")
{
    const uint32_t test_data = 0x12345678;
    struct timeval start, end;

    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    load_and_start_ulp_firmware(ulp_main_bin_start, ulp_main_bin_length);

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    /* Setup test data */
    ulp_riscv_test_data_in = test_data ^ XOR_MASK;
    ulp_main_cpu_command = RISCV_READ_WRITE_TEST;

    /* Enter Light Sleep */
    esp_light_sleep_start();

    /* Wait till we receive the correct command response */
    gettimeofday(&start, NULL);
    while (ulp_command_resp != RISCV_READ_WRITE_TEST)
        ;
    gettimeofday(&end, NULL);
    printf("Response time %jd ms\n", ((intmax_t)end.tv_sec - (intmax_t)start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000);

    /* Verify test data */
    TEST_ASSERT(ulp_command_resp == RISCV_READ_WRITE_TEST);
    TEST_ASSERT(ulp_main_cpu_reply == RISCV_COMMAND_OK);
    printf("data out: 0x%" PRIx32 ", expected: 0x%" PRIx32 " \n", ulp_riscv_test_data_out, test_data);
    TEST_ASSERT(test_data == ulp_riscv_test_data_out);

    /* Clear test data */
    ulp_main_cpu_command = RISCV_NO_COMMAND;
}

TEST_CASE("ULP-RISC-V is able to wakeup main CPU from light sleep", "[ulp]")
{
    struct timeval start, end;

    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    load_and_start_ulp_firmware(ulp_main_bin_start, ulp_main_bin_length);

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    /* Setup test data */
    ulp_main_cpu_command = RISCV_LIGHT_SLEEP_WAKEUP_TEST;

    /* Enter Light Sleep */
    esp_light_sleep_start();

    /* Wait till we receive the correct command response */
    gettimeofday(&start, NULL);
    while (ulp_command_resp != RISCV_LIGHT_SLEEP_WAKEUP_TEST)
        ;
    gettimeofday(&end, NULL);
    printf("Response time 1st: %jd ms\n", ((intmax_t)end.tv_sec - (intmax_t)start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000);

    /* Verify test data */
    TEST_ASSERT(ulp_command_resp == RISCV_LIGHT_SLEEP_WAKEUP_TEST);
    TEST_ASSERT(ulp_main_cpu_reply == RISCV_COMMAND_OK);

    /* Enter Light Sleep again */
    esp_light_sleep_start();

    /* Wait till we receive the correct command response */
    gettimeofday(&start, NULL);
    while (ulp_command_resp != RISCV_LIGHT_SLEEP_WAKEUP_TEST)
        ;
    gettimeofday(&end, NULL);
    printf("Response time 2nd: %jd ms\n", ((intmax_t)end.tv_sec - (intmax_t)start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000);

    /* Verify test data */
    TEST_ASSERT(ulp_command_resp == RISCV_LIGHT_SLEEP_WAKEUP_TEST);
    TEST_ASSERT(ulp_main_cpu_reply == RISCV_COMMAND_OK);

    /* Clear test data */
    ulp_main_cpu_command = RISCV_NO_COMMAND;
}

static bool ulp_riscv_is_running(uint32_t *counter_variable)
{
    uint32_t start_cnt = *counter_variable;

    /* Wait a few ULP wakeup cycles to ensure ULP has run */
    vTaskDelay((5 * ULP_WAKEUP_PERIOD / 1000) / portTICK_PERIOD_MS);

    uint32_t end_cnt = *counter_variable;
    printf("start run count: %" PRIu32 ", end run count %" PRIu32 "\n", start_cnt, end_cnt);

    /* If the ulp is running the counter should have been incremented */
    return (start_cnt != end_cnt);
}

TEST_CASE("ULP-RISC-V can be stopped and resumed from main CPU", "[ulp]")
{
    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    load_and_start_ulp_firmware(ulp_main_bin_start, ulp_main_bin_length);

    TEST_ASSERT(ulp_riscv_is_running(&ulp_riscv_counter));

    printf("Stopping the ULP\n");
    ulp_riscv_timer_stop();
    ulp_riscv_halt();

    TEST_ASSERT(!ulp_riscv_is_running(&ulp_riscv_counter));

    printf("Resuming the ULP\n");
    ulp_riscv_timer_resume();

    TEST_ASSERT(ulp_riscv_is_running(&ulp_riscv_counter));
}

TEST_CASE("ULP-RISC-V can be loaded with and run multiple firmwares", "[ulp]")
{
    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    load_and_start_ulp_firmware(ulp_main_bin_start, ulp_main_bin_length);

    TEST_ASSERT(ulp_riscv_is_running(&ulp_riscv_counter));

    printf("Stopping the ULP\n");
    ulp_riscv_timer_stop();
    ulp_riscv_halt();

    TEST_ASSERT(!ulp_riscv_is_running(&ulp_riscv_counter));

    printf("Loading second firmware on the ULP\n");
    firmware_loaded = false;
    load_and_start_ulp_firmware(ulp_test_app2_bin_start, ulp_test_app2_bin_length);

    TEST_ASSERT(ulp_riscv_is_running(&ulp_riscv_counter2));

    printf("Stopping the ULP\n");
    ulp_riscv_timer_stop();
    ulp_riscv_halt();

    TEST_ASSERT(!ulp_riscv_is_running(&ulp_riscv_counter2));

    printf("Loading the first firmware again on the ULP\n");
    firmware_loaded = false;
    load_and_start_ulp_firmware(ulp_main_bin_start, ulp_main_bin_length);

    TEST_ASSERT(ulp_riscv_is_running(&ulp_riscv_counter));
}

TEST_CASE("ULP-RISC-V can be reloaded with a good firmware after a crash", "[ulp]")
{
    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    load_and_start_ulp_firmware(ulp_main_bin_start, ulp_main_bin_length);

    TEST_ASSERT(ulp_riscv_is_running(&ulp_riscv_counter));

    printf("Stopping the ULP\n");
    ulp_riscv_timer_stop();
    ulp_riscv_halt();

    TEST_ASSERT(!ulp_riscv_is_running(&ulp_riscv_counter));

    /* Enable ULP wakeup */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    printf("Loading faulty firmware on the ULP and go into light sleep\n");
    firmware_loaded = false;
    load_and_start_ulp_firmware(ulp_test_app3_bin_start, ulp_test_app3_bin_length);
    esp_light_sleep_start();

    /* Verify that main CPU wakes up by a COCPU trap signal trigger */
    TEST_ASSERT(esp_sleep_get_wakeup_causes() & BIT(ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG));

    printf("Resetting the ULP\n");
    ulp_riscv_reset();

    esp_rom_delay_us(20);

    printf("Loading the good firmware on ULP\n");
    firmware_loaded = false;
    load_and_start_ulp_firmware(ulp_main_bin_start, ulp_main_bin_length);

    TEST_ASSERT(ulp_riscv_is_running(&ulp_riscv_counter));
}

TEST_CASE("ULP-RISC-V can stop itself and be resumed from the main CPU", "[ulp]")
{
    volatile riscv_test_commands_t *command_resp = (riscv_test_commands_t*)&ulp_command_resp;

    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    load_and_start_ulp_firmware(ulp_main_bin_start, ulp_main_bin_length);

    TEST_ASSERT(ulp_riscv_is_running(&ulp_riscv_counter));

    printf("Stopping the ULP\n");
    /* Setup test data */
    ulp_main_cpu_command = RISCV_STOP_TEST;

    while (*command_resp != RISCV_STOP_TEST) {
    }

    /* Wait a bit to ensure ULP finished shutting down */
    vTaskDelay(100 / portTICK_PERIOD_MS);

    TEST_ASSERT(!ulp_riscv_is_running(&ulp_riscv_counter));

    printf("Resuming the ULP\n");
    ulp_main_cpu_command = RISCV_NO_COMMAND;
    ulp_riscv_timer_resume();

    TEST_ASSERT(ulp_riscv_is_running(&ulp_riscv_counter));
}

TEST_CASE("ULP-RISC-V mutex", "[ulp]")
{
    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    load_and_start_ulp_firmware(ulp_main_bin_start, ulp_main_bin_length);

    /* Setup test data */
    ulp_riscv_incrementer = 0;
    ulp_main_cpu_reply = RISCV_NO_COMMAND;
    ulp_main_cpu_command = RISCV_MUTEX_TEST;

    ulp_riscv_lock_t *lock = (ulp_riscv_lock_t*)&ulp_lock;

    for (int i = 0; i < MUTEX_TEST_ITERATIONS; i++) {
        ulp_riscv_lock_acquire(lock);
        ulp_riscv_incrementer++;
        ulp_riscv_lock_release(lock);
    }

    while (ulp_main_cpu_reply != RISCV_COMMAND_OK) {
        // Wait for ULP to finish
    }

    /* If the variable is protected there should be no race conditions
       results should be the sum of increments made by ULP and by main CPU
    */
    TEST_ASSERT_EQUAL(2 * MUTEX_TEST_ITERATIONS, ulp_riscv_incrementer);
}

static void do_ulp_wakeup_deepsleep(riscv_test_commands_t ulp_cmd, bool rtc_periph_pd)
{
    if (!rtc_periph_pd) {
        // Force RTC peripheral power domain to be on
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    }

    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    load_and_start_ulp_firmware(ulp_main_bin_start, ulp_main_bin_length);

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    /* Setup test data */
    ulp_main_cpu_command = ulp_cmd;

    /* Enter Deep Sleep */
    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}

static void check_reset_reason_ulp_wakeup(void)
{
    TEST_ASSERT_EQUAL(BIT(ESP_SLEEP_WAKEUP_ULP), esp_sleep_get_wakeup_causes() & BIT(ESP_SLEEP_WAKEUP_ULP));
}

static void do_ulp_wakeup_after_long_delay_deepsleep(void)
{
    do_ulp_wakeup_deepsleep(RISCV_DEEP_SLEEP_WAKEUP_LONG_DELAY_TEST, true);
}

/* Certain erroneous wake-up triggers happen only after sleeping for a few seconds  */
TEST_CASE_MULTIPLE_STAGES("ULP-RISC-V is able to wakeup main CPU from deep sleep after a long delay", "[ulp]",
                          do_ulp_wakeup_after_long_delay_deepsleep,
                          check_reset_reason_ulp_wakeup);

static void do_ulp_wakeup_after_long_delay_deepsleep_rtc_perip_on(void)
{
    do_ulp_wakeup_deepsleep(RISCV_DEEP_SLEEP_WAKEUP_LONG_DELAY_TEST, false);
}

TEST_CASE_MULTIPLE_STAGES("ULP-RISC-V is able to wakeup main CPU from deep sleep after a long delay, RTC periph powerup", "[ulp]",
                          do_ulp_wakeup_after_long_delay_deepsleep_rtc_perip_on,
                          check_reset_reason_ulp_wakeup);

static void do_ulp_wakeup_after_short_delay_deepsleep(void)
{
    do_ulp_wakeup_deepsleep(RISCV_DEEP_SLEEP_WAKEUP_SHORT_DELAY_TEST, true);
}

TEST_CASE_MULTIPLE_STAGES("ULP-RISC-V is able to wakeup main CPU from deep sleep after a short delay", "[ulp]",
                          do_ulp_wakeup_after_short_delay_deepsleep,
                          check_reset_reason_ulp_wakeup);

static void do_ulp_wakeup_after_short_delay_deepsleep_rtc_perip_on(void)
{
    do_ulp_wakeup_deepsleep(RISCV_DEEP_SLEEP_WAKEUP_SHORT_DELAY_TEST, false);
}

TEST_CASE_MULTIPLE_STAGES("ULP-RISC-V is able to wakeup main CPU from deep sleep after a short delay, RTC periph powerup", "[ulp]",
                          do_ulp_wakeup_after_short_delay_deepsleep_rtc_perip_on,
                          check_reset_reason_ulp_wakeup);

typedef struct {
    SemaphoreHandle_t ulp_isr_sw_sem;
    SemaphoreHandle_t ulp_isr_trap_sem;
} TestSemaphore_t;

static void ulp_riscv_isr(void *arg)
{
    BaseType_t yield = 0;
    TestSemaphore_t *sem = (TestSemaphore_t *)arg;

    uint32_t status = READ_PERI_REG(RTC_CNTL_INT_ST_REG);

    if (status & ULP_RISCV_SW_INT) {
        xSemaphoreGiveFromISR(sem->ulp_isr_sw_sem, &yield);
    } else if (status & ULP_RISCV_TRAP_INT) {
        xSemaphoreGiveFromISR(sem->ulp_isr_trap_sem, &yield);
    }

    if (yield) {
        portYIELD_FROM_ISR();
    }
}

TEST_CASE("ULP-RISC-V interrupt signals can be handled via ISRs on the main core", "[ulp]")
{
    /* Create test semaphores */
    TestSemaphore_t test_sem_cfg;

    test_sem_cfg.ulp_isr_sw_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(test_sem_cfg.ulp_isr_sw_sem);

    test_sem_cfg.ulp_isr_trap_sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(test_sem_cfg.ulp_isr_trap_sem);

    /* Register ULP RISC-V signal ISR */
    TEST_ASSERT_EQUAL(ESP_OK, ulp_riscv_isr_register(ulp_riscv_isr, (void *)&test_sem_cfg,
                                                     (ULP_RISCV_SW_INT | ULP_RISCV_TRAP_INT)));

    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    printf("Loading good ULP firmware\n");
    firmware_loaded = false;
    load_and_start_ulp_firmware(ulp_main_bin_start, ulp_main_bin_length);

    /* Setup test data. We only need the ULP to send a wakeup signal to the main CPU. */
    ulp_main_cpu_command = RISCV_READ_WRITE_TEST;

    /* Wait for the ISR to be called */
    printf("Waiting for ULP wakeup signal ...\n");
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(test_sem_cfg.ulp_isr_sw_sem, portMAX_DELAY));
    printf("ULP wakeup signal interrupt triggered\n");

    /* Reset the ULP command */
    ulp_main_cpu_command = RISCV_NO_COMMAND;

    /* Load ULP RISC-V with faulty firmware and restart the ULP RISC-V Coprocessor.
     * This should cause a cocpu trap signal interrupt.
     */
    printf("Loading faulty ULP firmware\n");
    firmware_loaded = false;
    load_and_start_ulp_firmware(ulp_test_app3_bin_start, ulp_test_app3_bin_length);

    /* Wait for the ISR to be called */
    printf("Waiting for ULP trap signal ...\n");
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(test_sem_cfg.ulp_isr_trap_sem, portMAX_DELAY));
    printf("ULP trap signal interrupt triggered\n");

    /* Deregister the ISR */
    TEST_ASSERT_EQUAL(ESP_OK, ulp_riscv_isr_deregister(ulp_riscv_isr, (void *)&test_sem_cfg,
                                                       (ULP_RISCV_SW_INT | ULP_RISCV_TRAP_INT)));

    /* Delete test semaphores */
    vSemaphoreDelete(test_sem_cfg.ulp_isr_sw_sem);
    vSemaphoreDelete(test_sem_cfg.ulp_isr_trap_sem);

    /* Make sure ULP has a good firmware running before exiting the test */
    ulp_riscv_reset();
    firmware_loaded = false;
    load_and_start_ulp_firmware(ulp_main_bin_start, ulp_main_bin_length);
}

#define ATTEN 3
#define WIDTH 0
#define CHANNEL 6
#define ADC_UNIT ADC_UNIT_1

TEST_CASE("ULP ADC can init-deinit-init", "[ulp]")
{

    ulp_adc_cfg_t riscv_adc_cfg = {
        .adc_n    = ADC_UNIT,
        .channel  = CHANNEL,
        .width    = WIDTH,
        .atten    = ATTEN,
        .ulp_mode = ADC_ULP_MODE_FSM,
    };

    TEST_ASSERT_EQUAL(ESP_OK, ulp_adc_init(&riscv_adc_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, ulp_adc_deinit());

    /* Check that we can init one-shot ADC */
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT,
    };
    TEST_ASSERT_EQUAL(ESP_OK, adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = WIDTH,
        .atten = ATTEN,
    };
    TEST_ASSERT_EQUAL(ESP_OK, adc_oneshot_config_channel(adc1_handle, CHANNEL, &config));
    TEST_ASSERT_EQUAL(ESP_OK, adc_oneshot_del_unit(adc1_handle));

    /* Re-init ADC for ULP */
    TEST_ASSERT_EQUAL(ESP_OK, ulp_adc_init(&riscv_adc_cfg));
    TEST_ASSERT_EQUAL(ESP_OK, ulp_adc_deinit());
}

TEST_CASE("Test ULP RISCV delay", "[ulp]")
{
    int64_t start, diff;
    const uint32_t delay_period_us = 5000000;
    const uint32_t delta_us = 500000; // RTC FAST is not very accurate

    volatile uint32_t *command_resp = (volatile uint32_t *)&ulp_command_resp;
    volatile uint32_t *main_cpu_command = (volatile uint32_t *)&ulp_main_cpu_command;
    volatile uint32_t *main_cpu_reply = (volatile uint32_t *)&ulp_main_cpu_reply;
    volatile uint32_t *test_data_in = (volatile uint32_t *)&ulp_riscv_test_data_in;

    /* Load ULP RISC-V firmware and start the coprocessor */
    load_and_start_ulp_firmware(ulp_main_bin_start, ulp_main_bin_length);

    /* Setup test data */
    *command_resp = RISCV_NO_COMMAND;
    *main_cpu_reply = RISCV_NO_COMMAND;
    *test_data_in = delay_period_us;
    *main_cpu_command = RISCV_DELAY_TEST;

    /* Wait till we receive the correct command response */
    while (*command_resp != RISCV_DELAY_TEST) {
    }

    start = esp_timer_get_time();

    /* Wait till we receive COMMAND_OK reply */
    while (*main_cpu_reply != RISCV_COMMAND_OK) {
    }

    diff = esp_timer_get_time() - start;

    printf("Waited for %" PRIi64 "us, expected: %" PRIu32 "us\n", diff, delay_period_us);
    TEST_ASSERT_INT_WITHIN(delta_us, delay_period_us, diff);

    /* Clear test data */
    *main_cpu_command = RISCV_NO_COMMAND;
}

static void print_delay_table_header(void)
{
    printf("%10s %10s %10s %10s %10s\n",
           "----------", "----------", "----------", "----------", "----------");
    printf("%10s %10s %10s %10s %10s\n",
           "delay(us)", "expec_tk", "avg_tk", "error_tk", "in_range");
    printf("%10s %10s %10s %10s %10s\n",
           "----------", "----------", "----------", "----------", "----------");
}

static void run_delay_calibration_test(riscv_test_commands_t delay_calibration_cmd, const char *delay_api_name)
{
    volatile uint32_t *command_resp = (volatile uint32_t *)&ulp_command_resp;
    volatile uint32_t *main_cpu_command = (volatile uint32_t *)&ulp_main_cpu_command;
    volatile uint32_t *delay_sub_command = (volatile uint32_t *)&ulp_delay_sub_command;
    volatile uint32_t *test_data_in = (volatile uint32_t *)&ulp_riscv_test_data_in;
    const uint32_t test_delays_us[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        30, 40, 50, 70, 90,
        100, 200, 500, 1000, 2000, 5000,
        10000, 20000, 50000, 100000,
    };
    const uint32_t ulp_cpu_freq_hz = ULP_RISCV_CPU_FREQUENCY_HZ;
    const double ulp_tick_time_us = 1000000.0 / ulp_cpu_freq_hz;
    const double ulp_ticks_per_us = 1.0 / ulp_tick_time_us;
#if CONFIG_IDF_TARGET_ESP32S2
    const uint32_t warmup_exclude_ticks = 25;
    const double error_limit_time_us = 2.0;
#else // ESP32S3
    const uint32_t warmup_exclude_ticks = 17;
    const double error_limit_time_us = 1.0;
#endif
    const int32_t error_limit_ticks = (int32_t)(error_limit_time_us * ulp_ticks_per_us + 0.5);
    int32_t correction_ticks_checked = 0;
    uint32_t out_of_range_count = 0;
    uint32_t checked_measurements = 0;
    uint32_t skipped_warmup_measurements = 0;
    printf("Delay API under test: %s\n", delay_api_name);
    printf("ULP CPU frequency: %" PRIu32 " Hz, tick time: %.6f us, ticks/us: %.3f\n",
           ulp_cpu_freq_hz, ulp_tick_time_us, ulp_ticks_per_us);
    printf("Tolerance: +/- %" PRId32 " ticks (%.3f us)\n", error_limit_ticks, error_limit_time_us);
    printf("Skipping delay calibration points with expected ticks <= %" PRIu32 " ticks (%.3f us) as warmup\n",
           warmup_exclude_ticks, warmup_exclude_ticks * ulp_tick_time_us);

    load_and_start_ulp_firmware(ulp_main_bin_start, ulp_main_bin_length);

    *delay_sub_command = RISCV_DELAY_SUBCMD_NONE;
    *command_resp = RISCV_NO_COMMAND;
    *main_cpu_command = delay_calibration_cmd;

    while (*command_resp != delay_calibration_cmd) { };

    print_delay_table_header();
    const uint32_t num_delays = sizeof(test_delays_us) / sizeof(test_delays_us[0]);
    for (uint32_t delay_idx = 0; delay_idx < num_delays; delay_idx++) {
        const uint32_t requested_delay_us = test_delays_us[delay_idx];
        uint64_t sum_delay_ticks = 0;
        const uint32_t samples = 3;
        const uint32_t expected_ticks = requested_delay_us * ulp_ticks_per_us;

        for (uint32_t sample_idx = 0; sample_idx < samples; sample_idx++) {

            *test_data_in = (delay_calibration_cmd == RISCV_DELAY_CYCLES_CALIBRATION_TEST) ? expected_ticks : requested_delay_us;
            while (*delay_sub_command != RISCV_DELAY_SUBCMD_READY) { };
            *delay_sub_command = RISCV_DELAY_SUBCMD_RUN;

            /* Wait for the ULP to finish its delay before reading the cycle counters from RTC memory.
             * The HP core delay must be at least as long as the requested ULP delay, with some margin
             * to ensure that we do not read RTC memory while the ULP is running the delay calibration code. */
            vTaskDelay(((requested_delay_us / 1000 + portTICK_PERIOD_MS + 1) / portTICK_PERIOD_MS) * 3);
            sum_delay_ticks += ulp_delay_end_cycles - ulp_delay_start_cycles;
        }

        const uint64_t avg_delay_ticks = sum_delay_ticks / samples;
        const int32_t error_tk = expected_ticks - avg_delay_ticks;
        const char *range_status = "PASS";

        if (expected_ticks <= warmup_exclude_ticks) {
            skipped_warmup_measurements++;
            range_status = "SKIP";
        } else {
            checked_measurements++;
            correction_ticks_checked += error_tk;
            if ((error_tk < -error_limit_ticks) || (error_tk > error_limit_ticks)) {
                out_of_range_count++;
                range_status = "FAIL";
            }
        }

        printf("%10" PRIu32 " %10" PRIu32 " %10" PRIu64 " %10" PRId32 " %10s\n",
               requested_delay_us, expected_ticks, avg_delay_ticks, error_tk, range_status);
    }
    print_delay_table_header();

    // Release the ULP from the delay test loop
    *main_cpu_command = RISCV_NO_COMMAND;
    *test_data_in = 0;
    *delay_sub_command = RISCV_DELAY_SUBCMD_RUN;

    TEST_ASSERT_MESSAGE(checked_measurements > 0, "No delay calibration points were checked");
    double total_correct_ticks = (double)correction_ticks_checked / (double)checked_measurements;
    printf("Average correction ticks: %.1f (%.3f us)\n", total_correct_ticks, total_correct_ticks * ulp_tick_time_us);
    printf("Range-check summary (%s): checked=%" PRIu32 ", skipped_warmup=%" PRIu32 ", out_of_range=%" PRIu32
           ", limit=+/- %" PRId32 " ticks\n",
           delay_api_name, checked_measurements, skipped_warmup_measurements, out_of_range_count, error_limit_ticks);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, out_of_range_count, "Delay calibration has out-of-range error_tk values");
    TEST_ASSERT_DOUBLE_WITHIN(7.0, 0.0, total_correct_ticks);
}

TEST_CASE("Test delay calibration for ulp_riscv_delay_cycles", "[ulp]")
{
    run_delay_calibration_test(RISCV_DELAY_CYCLES_CALIBRATION_TEST, "ulp_riscv_delay_cycles");
}

TEST_CASE("Test delay calibration for ulp_riscv_delay_us", "[ulp]")
{
    run_delay_calibration_test(RISCV_DELAY_US_CALIBRATION_TEST, "ulp_riscv_delay_us");
}

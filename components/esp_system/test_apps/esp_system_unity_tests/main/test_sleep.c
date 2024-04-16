/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include <sys/time.h>
#include <sys/param.h>
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "soc/gpio_periph.h"
#include "hal/uart_types.h"
#include "hal/uart_ll.h"
#include "driver/uart.h"
#include "soc/rtc.h"            // for wakeup trigger defines
#include "soc/rtc_periph.h"     // for read rtc registers directly (cause)
#include "soc/soc.h"            // for direct register read macros
#include "esp_newlib.h"
#include "test_utils.h"
#include "sdkconfig.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "esp_private/esp_clk.h"
#include "esp_private/uart_share_hw_ctrl.h"
#include "esp_random.h"
#include "nvs_flash.h"
#include "nvs.h"

#if SOC_DEEP_SLEEP_SUPPORTED
#if SOC_PMU_SUPPORTED
#include "esp_private/esp_pmu.h"
#else
#include "hal/rtc_cntl_ll.h"
#endif

#define ESP_EXT0_WAKEUP_LEVEL_LOW 0
#define ESP_EXT0_WAKEUP_LEVEL_HIGH 1
__attribute__((unused)) static struct timeval tv_start, tv_stop;

static void check_sleep_reset(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_DEEPSLEEP, esp_reset_reason());
}

#ifndef CONFIG_FREERTOS_UNICORE
static void deep_sleep_task(void *arg)
{
    esp_deep_sleep_start();
}

static void do_deep_sleep_from_app_cpu(void)
{
    esp_sleep_enable_timer_wakeup(2000000);

    xTaskCreatePinnedToCore(&deep_sleep_task, "ds", 2048, NULL, 5, NULL, 1);

#if ( ( CONFIG_FREERTOS_SMP ) && ( !CONFIG_FREERTOS_UNICORE ) )
    //Note: Scheduler suspension behavior changed in FreeRTOS SMP
    vTaskPreemptionDisable(NULL);
#else
    // keep running some non-IRAM code
    vTaskSuspendAll();
#endif // #if ( ( CONFIG_FREERTOS_SMP ) && ( !CONFIG_FREERTOS_UNICORE ) )

    while (true) {
        ;
    }
}

TEST_CASE_MULTIPLE_STAGES("enter deep sleep on APP CPU and wake up using timer", "[deepsleep][reset=DEEPSLEEP_RESET]",
                          do_deep_sleep_from_app_cpu,
                          check_sleep_reset)

#endif

static void do_deep_sleep_timer(void)
{
    esp_sleep_enable_timer_wakeup(2000000);
    esp_deep_sleep_start();
}

TEST_CASE_MULTIPLE_STAGES("wake up from deep sleep using timer", "[deepsleep][reset=DEEPSLEEP_RESET]",
                          do_deep_sleep_timer,
                          check_sleep_reset)

static void do_light_sleep_deep_sleep_timer(void)
{
    esp_sleep_enable_timer_wakeup(1000000);
    esp_light_sleep_start();
    esp_deep_sleep_start();
}

TEST_CASE_MULTIPLE_STAGES("light sleep followed by deep sleep", "[deepsleep][reset=DEEPSLEEP_RESET]",
                          do_light_sleep_deep_sleep_timer,
                          check_sleep_reset)

TEST_CASE("wake up from light sleep using timer", "[deepsleep]")
{
    esp_sleep_enable_timer_wakeup(2000000);
    struct timeval tv_start, tv_stop;
    gettimeofday(&tv_start, NULL);
    esp_light_sleep_start();
    gettimeofday(&tv_stop, NULL);
    float dt = (tv_stop.tv_sec - tv_start.tv_sec) * 1e3f +
               (tv_stop.tv_usec - tv_start.tv_usec) * 1e-3f;
    TEST_ASSERT_INT32_WITHIN(500, 2000, (int) dt);
}

//NOTE: Explained in IDF-1445 | MR !14996
#if !(CONFIG_SPIRAM) || (CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL >= 16384)
static void test_light_sleep(void* arg)
{
    vTaskDelay(2);
    for (int i = 0; i < 1000; ++i) {
        printf("%d %d\n", xPortGetCoreID(), i);
        fflush(stdout);
        esp_light_sleep_start();
    }
    SemaphoreHandle_t done = (SemaphoreHandle_t) arg;
    xSemaphoreGive(done);
    vTaskDelete(NULL);
}

TEST_CASE("light sleep stress test", "[deepsleep]")
{
    SemaphoreHandle_t done = xSemaphoreCreateCounting(2, 0);
    esp_sleep_enable_timer_wakeup(1000);
    xTaskCreatePinnedToCore(&test_light_sleep, "ls0", 4096, done, UNITY_FREERTOS_PRIORITY + 1, NULL, 0);
#if CONFIG_FREERTOS_NUMBER_OF_CORES == 2
    xTaskCreatePinnedToCore(&test_light_sleep, "ls1", 4096, done, UNITY_FREERTOS_PRIORITY + 1, NULL, 1);
#endif
    xSemaphoreTake(done, portMAX_DELAY);
#if CONFIG_FREERTOS_NUMBER_OF_CORES == 2
    xSemaphoreTake(done, portMAX_DELAY);
#endif
    vSemaphoreDelete(done);
}

static void timer_func(void* arg)
{
    esp_rom_delay_us(50);
}

TEST_CASE("light sleep stress test with periodic esp_timer", "[deepsleep]")
{
    SemaphoreHandle_t done = xSemaphoreCreateCounting(2, 0);
    esp_sleep_enable_timer_wakeup(1000);
    esp_timer_handle_t timer;
    esp_timer_create_args_t config = {
        .callback = &timer_func,
    };
    TEST_ESP_OK(esp_timer_create(&config, &timer));
    esp_timer_start_periodic(timer, 500);
    xTaskCreatePinnedToCore(&test_light_sleep, "ls1", 4096, done, UNITY_FREERTOS_PRIORITY + 1, NULL, 0);
#if CONFIG_FREERTOS_NUMBER_OF_CORES == 2
    xTaskCreatePinnedToCore(&test_light_sleep, "ls1", 4096, done, UNITY_FREERTOS_PRIORITY + 1, NULL, 1);
#endif
    xSemaphoreTake(done, portMAX_DELAY);
#if CONFIG_FREERTOS_NUMBER_OF_CORES == 2
    xSemaphoreTake(done, portMAX_DELAY);
#endif
    vSemaphoreDelete(done);
    esp_timer_stop(timer);
    esp_timer_delete(timer);
}
#endif // !(CONFIG_SPIRAM) || (CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL >= 16384)

#if defined(CONFIG_ESP_SYSTEM_RTC_EXT_XTAL)
#define MAX_SLEEP_TIME_ERROR_US 200
#else
#define MAX_SLEEP_TIME_ERROR_US 100
#endif

TEST_CASE("light sleep duration is correct", "[deepsleep][ignore]")
{
    // don't power down XTAL — powering it up takes different time on
    // different boards
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_ON);

    // run one light sleep without checking timing, to warm up the cache
    esp_sleep_enable_timer_wakeup(1000);
    esp_light_sleep_start();

    const int sleep_intervals_ms[] = {
        1, 1, 2, 3, 4, 5, 6, 7, 8, 10, 15,
        20, 25, 50, 100, 200, 500,
    };

    const int sleep_intervals_count = sizeof(sleep_intervals_ms) / sizeof(sleep_intervals_ms[0]);
    for (int i = 0; i < sleep_intervals_count; ++i) {
        uint64_t sleep_time = sleep_intervals_ms[i] * 1000;
        esp_sleep_enable_timer_wakeup(sleep_time);
        for (int repeat = 0; repeat < 5; ++repeat) {
            uint64_t start = esp_clk_rtc_time();
            int64_t start_hs = esp_timer_get_time();
            esp_light_sleep_start();
            int64_t stop_hs = esp_timer_get_time();
            uint64_t stop = esp_clk_rtc_time();

            int diff_us = (int)(stop - start);
            int diff_hs_us = (int)(stop_hs - start_hs);
            printf("%lld %d\n", sleep_time, (int)(diff_us - sleep_time));
            int32_t threshold = MAX(sleep_time / 100, MAX_SLEEP_TIME_ERROR_US);
            TEST_ASSERT_INT32_WITHIN(threshold, sleep_time, diff_us);
            TEST_ASSERT_INT32_WITHIN(threshold, sleep_time, diff_hs_us);
            fflush(stdout);
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

TEST_CASE("light sleep and frequency switching", "[deepsleep]")
{
#ifndef CONFIG_PM_ENABLE
    uart_sclk_t clk_source = UART_SCLK_DEFAULT;
#if SOC_UART_SUPPORT_REF_TICK
    clk_source = UART_SCLK_REF_TICK;
#elif SOC_UART_SUPPORT_XTAL_CLK
    clk_source = UART_SCLK_XTAL;
#endif
    HP_UART_SRC_CLK_ATOMIC() {
        uart_ll_set_sclk(UART_LL_GET_HW(CONFIG_ESP_CONSOLE_UART_NUM), (soc_module_clk_t)clk_source);
    }
    uint32_t sclk_freq;
    TEST_ESP_OK(uart_get_sclk_freq(clk_source, &sclk_freq));
    HP_UART_SRC_CLK_ATOMIC() {
        uart_ll_set_baudrate(UART_LL_GET_HW(CONFIG_ESP_CONSOLE_UART_NUM), CONFIG_ESP_CONSOLE_UART_BAUDRATE, sclk_freq);
    }
#endif

    rtc_cpu_freq_config_t config_xtal, config_default;
    rtc_clk_cpu_freq_get_config(&config_default);
    rtc_clk_cpu_freq_mhz_to_config(esp_clk_xtal_freq() / MHZ, &config_xtal);

    esp_sleep_enable_timer_wakeup(1000);
    for (int i = 0; i < 1000; ++i) {
        if (i % 2 == 0) {
            rtc_clk_cpu_freq_set_config_fast(&config_xtal);
        } else {
            rtc_clk_cpu_freq_set_config_fast(&config_default);
        }
        printf("%d\n", i);
        fflush(stdout);
        esp_light_sleep_start();
    }
}

static void do_deep_sleep(void)
{
    esp_sleep_enable_timer_wakeup(100000);
    esp_deep_sleep_start();
}

static void check_sleep_reset_and_sleep(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_DEEPSLEEP, esp_reset_reason());
    esp_sleep_enable_timer_wakeup(100000);
    esp_deep_sleep_start();
}

TEST_CASE_MULTIPLE_STAGES("enter deep sleep more than once", "[deepsleep][reset=DEEPSLEEP_RESET,DEEPSLEEP_RESET,DEEPSLEEP_RESET]",
                          do_deep_sleep,
                          check_sleep_reset_and_sleep,
                          check_sleep_reset_and_sleep,
                          check_sleep_reset);

static void do_abort(void)
{
    abort();
}

static void check_abort_reset_and_sleep(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_PANIC, esp_reset_reason());
    esp_sleep_enable_timer_wakeup(100000);
    esp_deep_sleep_start();
}

TEST_CASE_MULTIPLE_STAGES("enter deep sleep after abort", "[deepsleep][reset=abort,SW_CPU_RESET,DEEPSLEEP_RESET]",
                          do_abort,
                          check_abort_reset_and_sleep,
                          check_sleep_reset);

#if ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB
static RTC_DATA_ATTR uint32_t s_wake_stub_var;

static RTC_IRAM_ATTR void wake_stub(void)
{
    esp_default_wake_deep_sleep();
    s_wake_stub_var = (uint32_t) &wake_stub;
}

static void prepare_wake_stub(void)
{
    esp_set_deep_sleep_wake_stub(&wake_stub);
    esp_sleep_enable_timer_wakeup(100000);
    esp_deep_sleep_start();
}

static void check_wake_stub(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_DEEPSLEEP, esp_reset_reason());
    TEST_ASSERT_EQUAL_HEX32((uint32_t) &wake_stub, s_wake_stub_var);
#if !CONFIG_IDF_TARGET_ESP32S3 && !CONFIG_IDF_TARGET_ESP32C6 && !CONFIG_IDF_TARGET_ESP32H2
    /* ROM code clears wake stub entry address */
    TEST_ASSERT_NULL(esp_get_deep_sleep_wake_stub());
#endif
}

TEST_CASE_MULTIPLE_STAGES("can set sleep wake stub", "[deepsleep][reset=DEEPSLEEP_RESET]",
                          prepare_wake_stub,
                          check_wake_stub);

#if CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP

/* Version of prepare_wake_stub() that sets up the deep sleep call while running
   from RTC memory as stack, with a high frequency timer also writing RTC FAST
   memory.

   This is important because the ROM code (ESP32 & ESP32-S2) requires software
   trigger a CRC calculation (done in hardware) for the entire RTC FAST memory
   before going to deep sleep and if it's invalid then the stub is not
   run. Also, while the CRC is being calculated the RTC FAST memory is not
   accessible by the CPU (reads all zeros).
*/

static void increment_rtc_memory_cb(void *arg)
{
    static volatile RTC_FAST_ATTR unsigned counter;
    counter++;
}

static void prepare_wake_stub_from_rtc(void)
{
    /* RTC memory can be used as heap, however there is no API call that returns this as
       a memory capability (as it's an implementation detail). So to test this we need to allocate
       the stack statically.
    */
#define STACK_SIZE 1500
#if CONFIG_IDF_TARGET_ESP32S3
    uint8_t *sleep_stack = (uint8_t *)heap_caps_malloc(STACK_SIZE, MALLOC_CAP_RTCRAM);
    TEST_ASSERT((uint32_t)sleep_stack >= SOC_RTC_DRAM_LOW && (uint32_t)sleep_stack < SOC_RTC_DRAM_HIGH);
#else
    static RTC_FAST_ATTR uint8_t sleep_stack[STACK_SIZE];
#endif
    static RTC_FAST_ATTR StaticTask_t sleep_task;

    /* normally BSS like sleep_stack will be cleared on reset, but RTC memory is not cleared on
     * wake from deep sleep. So to ensure unused stack is different if test is re-run without a full reset,
     * fill with some random bytes
     */
    esp_fill_random(sleep_stack, STACK_SIZE);

    /* to make things extra sure, start a periodic timer to write to RTC FAST RAM at high frequency */
    const esp_timer_create_args_t timer_args = {
        .callback = increment_rtc_memory_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "Write RTC MEM"
    };
    esp_timer_handle_t timer;
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer, 200));

    printf("Creating test task with stack %p\n", sleep_stack);
    TEST_ASSERT_NOT_NULL(xTaskCreateStatic((void *)prepare_wake_stub, "sleep", STACK_SIZE, NULL,
                                           UNITY_FREERTOS_PRIORITY, sleep_stack, &sleep_task));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_FAIL_MESSAGE("Should be asleep by now");
}

TEST_CASE_MULTIPLE_STAGES("can set sleep wake stub from stack in RTC RAM", "[deepsleep][reset=DEEPSLEEP_RESET]",
                          prepare_wake_stub_from_rtc,
                          check_wake_stub);

#endif // CONFIG_ESP_SYSTEM_ALLOW_RTC_FAST_MEM_AS_HEAP
#endif // ESP_ROM_SUPPORT_DEEP_SLEEP_WAKEUP_STUB

#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

__attribute__((unused)) static float get_time_ms(void)
{
    gettimeofday(&tv_stop, NULL);

    float dt = (tv_stop.tv_sec - tv_start.tv_sec) * 1e3f +
               (tv_stop.tv_usec - tv_start.tv_usec) * 1e-3f;
    return fabs(dt);
}

__attribute__((unused)) static uint32_t get_cause(void)
{
#if SOC_PMU_SUPPORTED
    uint32_t wakeup_cause = pmu_ll_hp_get_wakeup_cause(&PMU);
#else
    uint32_t wakeup_cause = rtc_cntl_ll_get_wakeup_cause();
#endif
    return wakeup_cause;
}

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3) && SOC_PM_SUPPORT_EXT0_WAKEUP
// Fails on S2 IDF-2903

// This test case verifies deactivation of trigger for wake up sources
TEST_CASE("disable source trigger behavior", "[deepsleep]")
{
    float dt = 0;

    printf("Setup timer and ext0 to wake up immediately from GPIO_13 \n");

    // Setup ext0 configuration to wake up almost immediately
    // The wakeup time is proportional to input capacitance * pullup resistance
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, ESP_EXT0_WAKEUP_LEVEL_HIGH));

    // Setup timer to wakeup with timeout
    esp_sleep_enable_timer_wakeup(2000000);

    // Save start time
    gettimeofday(&tv_start, NULL);
    esp_light_sleep_start();

    dt = get_time_ms();
    printf("Ext0 sleep time = %d \n", (int) dt);

    // Check wakeup from Ext0 using time measurement because wakeup cause is
    // not available in light sleep mode
    TEST_ASSERT_INT32_WITHIN(100, 100, (int) dt);

    TEST_ASSERT((get_cause() & RTC_EXT0_TRIG_EN) != 0);

    // Disable Ext0 source. Timer source should be triggered
    ESP_ERROR_CHECK(esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_EXT0));
    printf("Disable ext0 trigger and leave timer active.\n");

    gettimeofday(&tv_start, NULL);
    esp_light_sleep_start();

    dt = get_time_ms();
    printf("Timer sleep time = %d \n", (int) dt);

    TEST_ASSERT_INT32_WITHIN(500, 2000, (int) dt);

    // Additionally check wakeup cause
    TEST_ASSERT((get_cause() & RTC_TIMER_TRIG_EN) != 0);

    // Disable timer source.
    ESP_ERROR_CHECK(esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER));

    // Setup ext0 configuration to wake up immediately
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, ESP_EXT0_WAKEUP_LEVEL_HIGH));

    printf("Disable timer trigger to wake up from ext0 source.\n");

    gettimeofday(&tv_start, NULL);
    esp_light_sleep_start();

    dt = get_time_ms();
    printf("Ext0 sleep time = %d \n", (int) dt);

    TEST_ASSERT_INT32_WITHIN(100, 100, (int) dt);
    TEST_ASSERT((get_cause() & RTC_EXT0_TRIG_EN) != 0);

    // Check error message when source is already disabled
    esp_err_t err_code = esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
    TEST_ASSERT(err_code == ESP_ERR_INVALID_STATE);

    // Disable ext0 wakeup source, as this might interfere with other tests
    ESP_ERROR_CHECK(esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_EXT0));
}
#endif // !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32S3)

#endif //SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

static void trigger_deepsleep(void)
{
    struct timeval start;

    // Use NVS instead of RTC mem to store the start time of deep sleep
    // Because not all esp chips support RTC mem(such as esp32c2)
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        TEST_ESP_OK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    TEST_ESP_OK(err);

    nvs_handle_t nvs_handle;
    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");
    }

    printf("Trigger deep sleep. Waiting for 10 sec ...\n");

    // Simulate the dispersion of the calibration coefficients at start-up.
    // Corrupt the calibration factor.
    esp_clk_slowclk_cal_set(esp_clk_slowclk_cal_get() / 2);
    esp_set_time_from_rtc();

    // Delay for time error accumulation.
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    // Save start time. Deep sleep.
    gettimeofday(&start, NULL);

    TEST_ESP_OK(nvs_set_i32(nvs_handle, "start_sec", start.tv_sec));
    TEST_ESP_OK(nvs_set_i32(nvs_handle, "start_usec", start.tv_usec));
    TEST_ESP_OK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
    // Deinit NVS to prevent Unity from complaining "The test leaked too much memory"
    TEST_ESP_OK(nvs_flash_deinit());

    esp_sleep_enable_timer_wakeup(5000);
    // In function esp_deep_sleep_start() uses function esp_sync_timekeeping_timers()
    // to prevent a negative time after wake up.
    esp_deep_sleep_start();
}

static void check_time_deepsleep(void)
{
    struct timeval start, stop;

    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        TEST_ESP_OK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    TEST_ESP_OK(err);

    nvs_handle_t nvs_handle;
    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");
    }

    // Get start time of deep sleep
    TEST_ESP_OK(nvs_get_i32(nvs_handle, "start_sec", (int32_t *)&start.tv_sec));
    TEST_ESP_OK(nvs_get_i32(nvs_handle, "start_usec", (int32_t *)&start.tv_usec));
    nvs_close(nvs_handle);
    // Deinit NVS to prevent Unity from complaining "The test leaked too much memory"
    TEST_ESP_OK(nvs_flash_deinit());

    // Reset must be caused by deep sleep
    soc_reset_reason_t reason = esp_rom_get_reset_reason(0);
    TEST_ASSERT(reason == RESET_REASON_CORE_DEEP_SLEEP);

    // Time dt_ms must in any case be positive.
    gettimeofday(&stop, NULL);
    int dt_ms = (stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec) / 1000;
    printf("delta time = %d \n", dt_ms);
    TEST_ASSERT_MESSAGE(dt_ms > 0, "Time in deep sleep is negative");
}

TEST_CASE_MULTIPLE_STAGES("check a time after wakeup from deep sleep", "[deepsleep][reset=DEEPSLEEP_RESET]", trigger_deepsleep, check_time_deepsleep);

#endif // SOC_DEEP_SLEEP_SUPPORTED

#include "unity.h"
#include <sys/time.h>
#include <sys/param.h>
#include "esp_sleep.h"
#include "esp32/clk.h"
#include "driver/rtc_io.h"
#include "esp32/rom/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "soc/gpio_periph.h"
#include "soc/uart_periph.h"
#include "soc/rtc.h"            // for wakeup trigger defines
#include "soc/rtc_periph.h"     // for read rtc registers directly (cause)
#include "soc/soc.h"            // for direct register read macros
#include "esp32/rom/rtc.h"
#include "esp_newlib.h"
#include "test_utils.h"
#include "sdkconfig.h"


#define ESP_EXT0_WAKEUP_LEVEL_LOW 0
#define ESP_EXT0_WAKEUP_LEVEL_HIGH 1

static struct timeval tv_start, tv_stop;

#ifndef CONFIG_FREERTOS_UNICORE
static void deep_sleep_task(void *arg)
{
    esp_deep_sleep_start();
}

static void do_deep_sleep_from_app_cpu(void)
{
    xTaskCreatePinnedToCore(&deep_sleep_task, "ds", 2048, NULL, 5, NULL, 1);

    // keep running some non-IRAM code
    vTaskSuspendAll();

    while (true) {
        ;
    }
}
#endif

TEST_CASE("wake up from deep sleep using timer", "[deepsleep][reset=DEEPSLEEP_RESET]")
{
    esp_sleep_enable_timer_wakeup(2000000);
    esp_deep_sleep_start();
}

TEST_CASE("light sleep followed by deep sleep", "[deepsleep][reset=DEEPSLEEP_RESET]")
{
    esp_sleep_enable_timer_wakeup(1000000);
    esp_light_sleep_start();
    esp_deep_sleep_start();
}

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
#if portNUM_PROCESSORS == 2
    xTaskCreatePinnedToCore(&test_light_sleep, "ls1", 4096, done, UNITY_FREERTOS_PRIORITY + 1, NULL, 1);
#endif
    xSemaphoreTake(done, portMAX_DELAY);
#if portNUM_PROCESSORS == 2
    xSemaphoreTake(done, portMAX_DELAY);
#endif
    vSemaphoreDelete(done);
}

TEST_CASE("light sleep stress test with periodic esp_timer", "[deepsleep]")
{
    void timer_func(void* arg)
    {
        ets_delay_us(50);
    }

    SemaphoreHandle_t done = xSemaphoreCreateCounting(2, 0);
    esp_sleep_enable_timer_wakeup(1000);
    esp_timer_handle_t timer;
    esp_timer_create_args_t config = {
            .callback = &timer_func,
    };
    TEST_ESP_OK(esp_timer_create(&config, &timer));
    esp_timer_start_periodic(timer, 500);
    xTaskCreatePinnedToCore(&test_light_sleep, "ls1", 4096, done, UNITY_FREERTOS_PRIORITY + 1, NULL, 0);
#if portNUM_PROCESSORS == 2
    xTaskCreatePinnedToCore(&test_light_sleep, "ls1", 4096, done, UNITY_FREERTOS_PRIORITY + 1, NULL, 1);
#endif
    xSemaphoreTake(done, portMAX_DELAY);
#if portNUM_PROCESSORS == 2
    xSemaphoreTake(done, portMAX_DELAY);
#endif
    vSemaphoreDelete(done);
    esp_timer_stop(timer);
    esp_timer_delete(timer);
}


#ifdef CONFIG_ESP32_RTC_CLK_SRC_EXT_CRYS
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

    const int sleep_intervals_count = sizeof(sleep_intervals_ms)/sizeof(sleep_intervals_ms[0]);
    for (int i = 0; i < sleep_intervals_count; ++i) {
        uint64_t sleep_time = sleep_intervals_ms[i] * 1000;
        esp_sleep_enable_timer_wakeup(sleep_time);
        for (int repeat = 0; repeat < 5; ++repeat) {
            uint64_t start = esp_clk_rtc_time();
            int64_t start_hs = esp_timer_get_time();
            esp_light_sleep_start();
            int64_t stop_hs = esp_timer_get_time();
            uint64_t stop = esp_clk_rtc_time();

            int diff_us = (int) (stop - start);
            int diff_hs_us = (int) (stop_hs - start_hs);
            printf("%lld %d\n", sleep_time, (int) (diff_us - sleep_time));
            int32_t threshold = MAX(sleep_time / 100, MAX_SLEEP_TIME_ERROR_US);
            TEST_ASSERT_INT32_WITHIN(threshold, sleep_time, diff_us);
            TEST_ASSERT_INT32_WITHIN(threshold, sleep_time, diff_hs_us);
            fflush(stdout);
        }

        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}


TEST_CASE("light sleep and frequency switching", "[deepsleep]")
{
#ifndef CONFIG_PM_ENABLE
    const int uart_clk_freq = REF_CLK_FREQ;
    CLEAR_PERI_REG_MASK(UART_CONF0_REG(CONFIG_ESP_CONSOLE_UART_NUM), UART_TICK_REF_ALWAYS_ON);
    uart_div_modify(CONFIG_ESP_CONSOLE_UART_NUM, (uart_clk_freq << 4) / CONFIG_ESP_CONSOLE_UART_BAUDRATE);
#endif

    rtc_cpu_freq_config_t config_xtal, config_default;
    rtc_clk_cpu_freq_get_config(&config_default);
    rtc_clk_cpu_freq_mhz_to_config((int) rtc_clk_xtal_freq_get(), &config_xtal);

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

#ifndef CONFIG_FREERTOS_UNICORE
TEST_CASE("enter deep sleep on APP CPU and wake up using timer", "[deepsleep][reset=DEEPSLEEP_RESET]")
{
    esp_sleep_enable_timer_wakeup(2000000);
    do_deep_sleep_from_app_cpu();
}
#endif

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

static void check_sleep_reset(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_DEEPSLEEP, esp_reset_reason());
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
    /* ROM code clears wake stub entry address */
    TEST_ASSERT_NULL(esp_get_deep_sleep_wake_stub());
}

TEST_CASE_MULTIPLE_STAGES("can set sleep wake stub", "[deepsleep][reset=DEEPSLEEP_RESET]",
        prepare_wake_stub,
        check_wake_stub);


#if CONFIG_ESP32_ALLOW_RTC_FAST_MEM_AS_HEAP || CONFIG_ESP32S2_ALLOW_RTC_FAST_MEM_AS_HEAP \
    || CONFIG_ESP32S3_ALLOW_RTC_FAST_MEM_AS_HEAP
#if CONFIG_FREERTOS_SUPPORT_STATIC_ALLOCATION

/* Version of prepare_wake_stub() that sets up the deep sleep call while running
   from RTC memory as stack, with a high frequency timer also writing RTC FAST
   memory.

   This is important because the ROM code (ESP32 & ESP32-S2) requires software
   trigger a CRC calculation (done in hardware) for the entire RTC FAST memory
   before going to deep sleep and if it's invalid then the stub is not
   run. Also, while the CRC is being calculated the RTC FAST memory is not
   accesible by the CPU (reads all zeros).
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
    static RTC_FAST_ATTR uint8_t sleep_stack[1024];
    static RTC_FAST_ATTR StaticTask_t sleep_task;

    /* normally BSS like sleep_stack will be cleared on reset, but RTC memory is not cleared on
     * wake from deep sleep. So to ensure unused stack is different if test is re-run without a full reset,
     * fill with some random bytes
     */
    esp_fill_random(sleep_stack, sizeof(sleep_stack));

    /* to make things extra sure, start a periodic timer to write to RTC FAST RAM at high frequency */
    const esp_timer_create_args_t timer_args = {
                                          .callback = increment_rtc_memory_cb,
                                          .arg = NULL,
                                          .dispatch_method = ESP_TIMER_TASK,
                                          .name = "Write RTC MEM"
    };
    esp_timer_handle_t timer;
    ESP_ERROR_CHECK( esp_timer_create(&timer_args, &timer) );
    ESP_ERROR_CHECK( esp_timer_start_periodic(timer, 200) );

    printf("Creating test task with stack %p\n", sleep_stack);
    TEST_ASSERT_NOT_NULL(xTaskCreateStatic( (void *)prepare_wake_stub, "sleep", sizeof(sleep_stack), NULL,
                                            UNITY_FREERTOS_PRIORITY, sleep_stack, &sleep_task));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_FAIL_MESSAGE("Should be asleep by now");
}

TEST_CASE_MULTIPLE_STAGES("can set sleep wake stub from stack in RTC RAM", "[deepsleep][reset=DEEPSLEEP_RESET]",
        prepare_wake_stub_from_rtc,
        check_wake_stub);

#endif // CONFIG_FREERTOS_SUPPORT_STATIC_ALLOCATION
#endif // CONFIG_xyz_ALLOW_RTC_FAST_MEM_AS_HEAP

TEST_CASE("wake up using ext0 (13 high)", "[deepsleep][ignore]")
{
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, ESP_EXT0_WAKEUP_LEVEL_HIGH));
    esp_deep_sleep_start();
}

TEST_CASE("wake up using ext0 (13 low)", "[deepsleep][ignore]")
{
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, ESP_EXT0_WAKEUP_LEVEL_LOW));
    esp_deep_sleep_start();
}

TEST_CASE("wake up using ext1 when RTC_PERIPH is off (13 high)", "[deepsleep][ignore]")
{
    // This test needs external pulldown
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT(GPIO_NUM_13), ESP_EXT1_WAKEUP_ANY_HIGH));
    esp_deep_sleep_start();
}

TEST_CASE("wake up using ext1 when RTC_PERIPH is off (13 low)", "[deepsleep][ignore]")
{
    // This test needs external pullup
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT(GPIO_NUM_13), ESP_EXT1_WAKEUP_ALL_LOW));
    esp_deep_sleep_start();
}

TEST_CASE("wake up using ext1 when RTC_PERIPH is on (13 high)", "[deepsleep][ignore]")
{
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON));
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT(GPIO_NUM_13), ESP_EXT1_WAKEUP_ANY_HIGH));
    esp_deep_sleep_start();
}

TEST_CASE("wake up using ext1 when RTC_PERIPH is on (13 low)", "[deepsleep][ignore]")
{
    ESP_ERROR_CHECK(rtc_gpio_init(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pullup_en(GPIO_NUM_13));
    ESP_ERROR_CHECK(gpio_pulldown_dis(GPIO_NUM_13));
    ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON));
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(BIT(GPIO_NUM_13), ESP_EXT1_WAKEUP_ALL_LOW));
    esp_deep_sleep_start();
}

static float get_time_ms(void)
{
    gettimeofday(&tv_stop, NULL);

    float dt = (tv_stop.tv_sec - tv_start.tv_sec) * 1e3f +
                (tv_stop.tv_usec - tv_start.tv_usec) * 1e-3f;
    return fabs(dt);
}

static uint32_t get_cause(void)
{
    uint32_t wakeup_cause = REG_GET_FIELD(RTC_CNTL_WAKEUP_STATE_REG, \
                                            RTC_CNTL_WAKEUP_CAUSE);
    return wakeup_cause;
}

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
}

static RTC_DATA_ATTR struct timeval start;
static void trigger_deepsleep(void)
{
    printf("Trigger deep sleep. Waiting for 10 sec ...\n");

    // Simulate the dispersion of the calibration coefficients at start-up.
    // Corrupt the calibration factor.
    esp_clk_slowclk_cal_set(esp_clk_slowclk_cal_get() / 2);
    esp_set_time_from_rtc();

    // Delay for time error accumulation.
    vTaskDelay(10000/portTICK_RATE_MS);

    // Save start time. Deep sleep.
    gettimeofday(&start, NULL);
    esp_sleep_enable_timer_wakeup(1000);
    // In function esp_deep_sleep_start() uses function esp_sync_counters_rtc_and_frc()
    // to prevent a negative time after wake up.
    esp_deep_sleep_start();
}

static void check_time_deepsleep(void)
{
    struct timeval stop;
    RESET_REASON reason = rtc_get_reset_reason(0);
    TEST_ASSERT(reason == DEEPSLEEP_RESET);
    gettimeofday(&stop, NULL);
    // Time dt_ms must in any case be positive.
    int dt_ms = (stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec) / 1000;
    printf("delta time = %d \n", dt_ms);
    TEST_ASSERT_MESSAGE(dt_ms > 0, "Time in deep sleep is negative");
}

TEST_CASE_MULTIPLE_STAGES("check a time after wakeup from deep sleep", "[deepsleep][reset=DEEPSLEEP_RESET]", trigger_deepsleep, check_time_deepsleep);

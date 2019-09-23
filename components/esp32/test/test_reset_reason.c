#include "unity.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "esp_attr.h"
#include "soc/rtc_periph.h"
#include "driver/timer.h"
#include "esp32/rom/rtc.h"
#include "esp_sleep.h"

#define RTC_BSS_ATTR __attribute__((section(".rtc.bss")))

#define CHECK_VALUE 0x89abcdef

static __NOINIT_ATTR uint32_t s_noinit_val;
static RTC_NOINIT_ATTR uint32_t s_rtc_noinit_val;
static RTC_DATA_ATTR uint32_t s_rtc_data_val;
static RTC_BSS_ATTR uint32_t s_rtc_bss_val;
/* There is no practical difference between placing something into RTC_DATA and
 * RTC_RODATA. This only checks a usage pattern where the variable has a non-zero
 * initializer (should be initialized by the bootloader).
 */
static RTC_RODATA_ATTR uint32_t s_rtc_rodata_val = CHECK_VALUE;
static RTC_FAST_ATTR uint32_t s_rtc_force_fast_val;
static RTC_SLOW_ATTR uint32_t s_rtc_force_slow_val;


static void setup_values(void)
{
    s_noinit_val = CHECK_VALUE;
    s_rtc_noinit_val = CHECK_VALUE;
    s_rtc_data_val = CHECK_VALUE;
    s_rtc_bss_val = CHECK_VALUE;
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(CHECK_VALUE, s_rtc_rodata_val,
            "s_rtc_rodata_val should already be set up");
    s_rtc_force_fast_val = CHECK_VALUE;
    s_rtc_force_slow_val = CHECK_VALUE;
}

/* This test needs special test runners: rev1 silicon, and SPI flash with
 * fast start-up time. Otherwise reset reason will be RTCWDT_RESET.
 */
TEST_CASE("reset reason ESP_RST_POWERON", "[reset][ignore]")
{
    TEST_ASSERT_EQUAL(ESP_RST_POWERON, esp_reset_reason());
}

static void do_deep_sleep(void)
{
    setup_values();
    esp_sleep_enable_timer_wakeup(10000);
    esp_deep_sleep_start();
}

static void check_reset_reason_deep_sleep(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_DEEPSLEEP, esp_reset_reason());

    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_data_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_bss_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_rodata_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_force_fast_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_force_slow_val);
}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_DEEPSLEEP", "[reset_reason][reset=DEEPSLEEP_RESET]",
        do_deep_sleep,
        check_reset_reason_deep_sleep);

static void do_exception(void)
{
    setup_values();
    *(int*) (0x40000001) = 0;
}

static void do_abort(void)
{
    setup_values();
    abort();
}

static void check_reset_reason_panic(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_PANIC, esp_reset_reason());

    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_noinit_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_data_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_bss_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_rodata_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_fast_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_slow_val);
}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_PANIC after exception", "[reset_reason][reset=LoadStoreError,SW_CPU_RESET]",
        do_exception,
        check_reset_reason_panic);

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_PANIC after abort", "[reset_reason][reset=abort,SW_CPU_RESET]",
        do_abort,
        check_reset_reason_panic);

static void do_restart(void)
{
    setup_values();
    esp_restart();
}

#if portNUM_PROCESSORS > 1
static void do_restart_from_app_cpu(void)
{
    setup_values();
    xTaskCreatePinnedToCore((TaskFunction_t) &do_restart, "restart", 2048, NULL, 5, NULL, 1);
    vTaskDelay(2);
}
#endif

static void check_reset_reason_sw(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_SW, esp_reset_reason());

    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_noinit_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_data_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_bss_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_rodata_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_fast_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_slow_val);
}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_SW after restart", "[reset_reason][reset=SW_CPU_RESET]",
        do_restart,
        check_reset_reason_sw);

#if portNUM_PROCESSORS > 1
TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_SW after restart from APP CPU", "[reset_reason][reset=SW_CPU_RESET]",
        do_restart_from_app_cpu,
        check_reset_reason_sw);
#endif


static void do_int_wdt(void)
{
    setup_values();
    portENTER_CRITICAL_NESTED();
    while(1);
}

static void do_int_wdt_hw(void)
{
    setup_values();
    XTOS_SET_INTLEVEL(XCHAL_NMILEVEL);
    while(1);
}

static void check_reset_reason_int_wdt(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_INT_WDT, esp_reset_reason());
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_INT_WDT after interrupt watchdog (panic)",
        "[reset_reason][reset=Interrupt wdt timeout on CPU0,SW_CPU_RESET]",
        do_int_wdt,
        check_reset_reason_int_wdt);

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_INT_WDT after interrupt watchdog (hw)",
        "[reset_reason][reset=TG1WDT_SYS_RESET]",
        do_int_wdt_hw,
        check_reset_reason_int_wdt);

static void do_task_wdt(void)
{
    setup_values();
    esp_task_wdt_init(1, true);
    esp_task_wdt_add(xTaskGetIdleTaskHandleForCPU(0));
    while(1);
}

static void check_reset_reason_task_wdt(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_TASK_WDT, esp_reset_reason());

    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_noinit_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_data_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_bss_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_rodata_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_fast_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_slow_val);
}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_TASK_WDT after task watchdog",
        "[reset_reason][reset=abort,SW_CPU_RESET]",
        do_task_wdt,
        check_reset_reason_task_wdt);

static void do_rtc_wdt(void)
{
    setup_values();
    WRITE_PERI_REG(RTC_CNTL_WDTWPROTECT_REG, RTC_CNTL_WDT_WKEY_VALUE);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_SYS_RESET_LENGTH, 7);
    REG_SET_FIELD(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_STG0, RTC_WDT_STG_SEL_RESET_SYSTEM);
    WRITE_PERI_REG(RTC_CNTL_WDTCONFIG1_REG, 10000);
    REG_SET_BIT(RTC_CNTL_WDTCONFIG0_REG, RTC_CNTL_WDT_FLASHBOOT_MOD_EN);
    while(1);
}

static void check_reset_reason_any_wdt(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_WDT, esp_reset_reason());
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_WDT after RTC watchdog",
        "[reset_reason][reset=RTCWDT_RTC_RESET]",
        do_rtc_wdt,
        check_reset_reason_any_wdt);


static void do_brownout(void)
{
    setup_values();
    printf("Manual test: lower the supply voltage to cause brownout\n");
    vTaskSuspend(NULL);
}

static void check_reset_reason_brownout(void)
{
    TEST_ASSERT_EQUAL(ESP_RST_BROWNOUT, esp_reset_reason());

    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_noinit_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_noinit_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_data_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_bss_val);
    TEST_ASSERT_EQUAL_HEX32(CHECK_VALUE, s_rtc_rodata_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_fast_val);
    TEST_ASSERT_EQUAL_HEX32(0, s_rtc_force_slow_val);
}

TEST_CASE_MULTIPLE_STAGES("reset reason ESP_RST_BROWNOUT after brownout event",
        "[reset_reason][ignore][reset=SW_CPU_RESET]",
        do_brownout,
        check_reset_reason_brownout);


// The following test cases are used to check if the timer_group fix works.
// Some applications use a software reset, at the reset time, timer_group happens to generate an interrupt.
// but software reset does not clear interrupt status, this is not safe for application when enable the interrupt of timer_group.
// This case will check under this fix, whether the interrupt status is cleared after timer_group initialization.
static void timer_group_test_init(void)
{
    static const uint32_t time_ms = 100; //Alarm value 100ms.
    static const uint16_t timer_div = 10; //Timer prescaler
    static const uint32_t ste_val = time_ms * (TIMER_BASE_CLK / timer_div / 1000);
    timer_config_t config = {
        .divider = timer_div,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .intr_type = TIMER_INTR_LEVEL,
        .auto_reload = true,
    };
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, ste_val);
    //Now the timer is ready.
    //We only need to check the interrupt status and don't have to register a interrupt routine.
}

static void timer_group_test_first_stage(void)
{
    RESET_REASON rst_res = rtc_get_reset_reason(0);
    if(rst_res != POWERON_RESET){
        printf("Not power on reset\n");
    }
    TEST_ASSERT_EQUAL(POWERON_RESET, rst_res);
    static uint8_t loop_cnt = 0;
    timer_group_test_init();
    //Start timer
    timer_start(TIMER_GROUP_0, TIMER_0);
    //Waiting for timer_group to generate an interrupt
    while( !(timer_group_intr_get_in_isr(TIMER_GROUP_0) & TIMER_INTR_T0) &&
            loop_cnt++ < 100) {
        vTaskDelay(200);
    }
    //TIMERG0.int_raw.t0 == 1 means an interruption has occurred
    TEST_ASSERT(timer_group_intr_get_in_isr(TIMER_GROUP_0) & TIMER_INTR_T0);
    esp_restart();
}

static void timer_group_test_second_stage(void)
{
    RESET_REASON rst_res = rtc_get_reset_reason(0);
    if(rst_res != SW_CPU_RESET){
        printf("Not software reset\n");
    }
    TEST_ASSERT_EQUAL(SW_CPU_RESET, rst_res);
    timer_group_test_init();
    //After the timer_group is initialized, TIMERG0.int_raw.t0 should be cleared.
    TEST_ASSERT_EQUAL(0, TIMERG0.int_raw.t0);
}

TEST_CASE_MULTIPLE_STAGES("timer_group software reset test",
        "[intr_status][intr_status = 0]",
        timer_group_test_first_stage,
        timer_group_test_second_stage);

/* Not tested here: ESP_RST_SDIO */

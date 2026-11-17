/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#if CONFIG_ESP_ENABLE_PVT && CONFIG_PM_ENABLE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_pm.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "esp_rom_sys.h"
#include "esp_private/esp_clk.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"
#include "esp_private/sleep_cpu.h"
#include "driver/uart.h"
#include "hal/efuse_hal.h"
#include "hal/pmu_ll.h"
#include "hal/wdt_hal.h"
#include "soc/soc_caps.h"
#include "soc/rtc.h"
#if CONFIG_ESP_TASK_WDT_EN
#include "esp_task_wdt.h"
#endif

#define MHZ                 (1000000)
/* Auto-dbias updates once per PVT_TARGET clocks of the selected PVT func PLL */
#if CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32H21 || CONFIG_IDF_TARGET_ESP32H4
#define PVT_CLK_HZ          (96 * MHZ)
#else
#define PVT_CLK_HZ          (160 * MHZ)
#endif
#define PVT_PERIOD_US       ((uint32_t)((PVT_TARGET * 1000000ULL + PVT_CLK_HZ / 2) / PVT_CLK_HZ))
#define HP_VOL_SAMPLE_N     10
#define DROP_STREAK_FAIL    3
#define HP_VOL_TOLERANCE    3
#define TEST_ROUNDS         20
#define LS_SLEEP_US         (200 * 1000)
#define DS_SLEEP_US         (1000 * 1000)
#define RTC_WDT_TIMEOUT_MS  5000

static wdt_hal_context_t s_rtc_wdt;
static bool s_rtc_wdt_enabled;

/* pmu_init() only hands HP dbias to PVT when eFuse blk_version is high enough
 * Otherwise dbias_sel stays 1 and hp_dbias_vol is static — event checks would always pass. */
static void require_pvt_controlling_hp_dbias(void)
{
    uint32_t blk_version = efuse_hal_blk_version();
    uint32_t dbias_sel = PMU.hp_sys[PMU_MODE_HP_ACTIVE].regulator0.dbias_sel;

    printf("efuse_hal_blk_version=%" PRIu32 " HP_ACTIVE.dbias_sel=%" PRIu32 "\n",
           blk_version, dbias_sel);
    if (dbias_sel != 0) {
        TEST_IGNORE_MESSAGE("PVT has not taken over HP dbias (dbias_sel != 0)");
    }
}

static void rtc_wdt_disable(void)
{
    if (!s_rtc_wdt_enabled) {
        return;
    }
    wdt_hal_write_protect_disable(&s_rtc_wdt);
    wdt_hal_disable(&s_rtc_wdt);
    wdt_hal_write_protect_enable(&s_rtc_wdt);
    s_rtc_wdt_enabled = false;
}

static void rtc_wdt_enable(void)
{
    uint32_t ticks = (uint32_t)((uint64_t)RTC_WDT_TIMEOUT_MS * rtc_clk_slow_freq_get_hz() / 1000ULL);
    wdt_hal_init(&s_rtc_wdt, WDT_RWDT, 0, false);
    wdt_hal_write_protect_disable(&s_rtc_wdt);
    wdt_hal_config_stage(&s_rtc_wdt, WDT_STAGE0, ticks, WDT_STAGE_ACTION_RESET_SYSTEM);
    wdt_hal_enable(&s_rtc_wdt);
    wdt_hal_write_protect_enable(&s_rtc_wdt);
    s_rtc_wdt_enabled = true;
    TEST_ASSERT_TRUE(wdt_hal_is_enabled(&s_rtc_wdt));
}

static void rtc_wdt_feed(void)
{
    if (!s_rtc_wdt_enabled) {
        return;
    }
    wdt_hal_write_protect_disable(&s_rtc_wdt);
    wdt_hal_feed(&s_rtc_wdt);
    wdt_hal_write_protect_enable(&s_rtc_wdt);
}

static inline uint32_t read_hp_vol(void)
{
    return pmu_ll_hp_get_regulator_hp_dbias_voltage(&PMU, PMU_MODE_HP_ACTIVE);
}

static void flush_hp_vol_log(void)
{
    fflush(stdout);
    fsync(fileno(stdout));
}

/* Compare post-event hp_dbias_vol to the value captured before the event. */
static void assert_hp_vol_within(const char *tag, uint32_t before, uint32_t after)
{
    uint32_t delta = (after > before) ? (after - before) : (before - after);
    printf("[%s] hp_vol before=%" PRIu32 " after=%" PRIu32 " delta=%" PRIu32 "\n",
           tag, before, after, delta);
    flush_hp_vol_log();
    if (delta > HP_VOL_TOLERANCE) {
        rtc_wdt_disable();
    }
    TEST_ASSERT_UINT_WITHIN(HP_VOL_TOLERANCE, before, after);
}

/* After DFS, dbias may settle to a new level; sample ~1 PVT period apart and
 * fail only on a consecutive descending streak (slow runaway). */
static void sample_assert_no_runaway(const char *tag)
{
    uint32_t streak = 0;
    uint32_t prev = read_hp_vol();

    printf("[%s] hp_vol: %" PRIu32, tag, prev);
    flush_hp_vol_log();
    for (int i = 1; i < HP_VOL_SAMPLE_N; i++) {
        esp_rom_delay_us(PVT_PERIOD_US);
        uint32_t v = read_hp_vol();
        printf(" %" PRIu32, v);
        flush_hp_vol_log();
        if (v < prev) {
            streak++;
            if (streak >= DROP_STREAK_FAIL) {
                printf("\n");
                flush_hp_vol_log();
                rtc_wdt_disable();
                TEST_FAIL_MESSAGE("hp_vol consecutive descending streak >= 3");
            }
        } else {
            streak = 0;
        }
        prev = v;
    }
    printf("\n");
    flush_hp_vol_log();
}

static void dfs_switch_mhz(int mhz)
{
    esp_pm_config_t pm_config = {
        .max_freq_mhz = mhz,
        .min_freq_mhz = mhz,
        .light_sleep_enable = false,
    };
    TEST_ESP_OK(esp_pm_configure(&pm_config));
    TEST_ASSERT_EQUAL_UINT32(mhz, esp_clk_cpu_freq() / MHZ);
}

static void assert_sleep_flags(esp_sleep_context_t *ctx, uint32_t mask, uint32_t expect)
{
    uint32_t got = ctx->sleep_flags & mask;
    if (ctx->sleep_request_result != 0 || got != expect) {
        rtc_wdt_disable();
    }
    TEST_ASSERT_EQUAL(0, ctx->sleep_request_result);
    TEST_ASSERT_EQUAL(expect, got);
}

static void do_timer_light_sleep(uint32_t sleep_us)
{
    uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(sleep_us));
    TEST_ESP_OK(esp_light_sleep_start());
}

static void pd_domains_restore_auto(void)
{
    TEST_ESP_OK(esp_sleep_pd_config(ESP_PD_DOMAIN_CPU, ESP_PD_OPTION_AUTO));
    TEST_ESP_OK(esp_sleep_pd_config(ESP_PD_DOMAIN_TOP, ESP_PD_OPTION_AUTO));
    TEST_ESP_OK(sleep_cpu_configure(false));
}

TEST_CASE("PVT: DFS max<->XTAL keeps hp_vol stable", "[pvt]")
{
    require_pvt_controlling_hp_dbias();

    int xtal_mhz = esp_clk_xtal_freq() / MHZ;
    int max_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ;
    int orig_mhz = esp_clk_cpu_freq() / MHZ;

    rtc_wdt_enable();
    for (int i = 0; i < TEST_ROUNDS; i++) {
        rtc_wdt_feed();
        dfs_switch_mhz(max_mhz);
        /* Do not baseline against pre-switch value: dbias tracks CPU frequency. */
        sample_assert_no_runaway("dfs_max");

        dfs_switch_mhz(xtal_mhz);
        sample_assert_no_runaway("dfs_xtal");
    }
    rtc_wdt_disable();
    dfs_switch_mhz(orig_mhz);
}

TEST_CASE("PVT: light sleep without CPU/TOP PD keeps hp_vol stable", "[pvt]")
{
    require_pvt_controlling_hp_dbias();

    /* Keep TOP on; do not allow CPU PD (no sleep_cpu_configure). */
    TEST_ESP_OK(esp_sleep_pd_config(ESP_PD_DOMAIN_TOP, ESP_PD_OPTION_ON));
    TEST_ASSERT_FALSE(cpu_domain_pd_allowed());

    esp_sleep_context_t sleep_ctx = {};
    esp_sleep_set_sleep_context(&sleep_ctx);

    rtc_wdt_enable();
    for (int i = 0; i < TEST_ROUNDS; i++) {
        rtc_wdt_feed();
        uint32_t before = read_hp_vol();
        do_timer_light_sleep(LS_SLEEP_US);
        assert_sleep_flags(&sleep_ctx, PMU_SLEEP_PD_CPU | PMU_SLEEP_PD_TOP, 0);
        assert_hp_vol_within("ls_no_pd", before, read_hp_vol());
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    rtc_wdt_disable();

    esp_sleep_set_sleep_context(NULL);
    pd_domains_restore_auto();
}

TEST_CASE("PVT: PD_CPU light sleep (TOP on) keeps hp_vol stable", "[pvt]")
{
    require_pvt_controlling_hp_dbias();

    TEST_ESP_OK(sleep_cpu_configure(true));
    TEST_ASSERT_TRUE(cpu_domain_pd_allowed());
    TEST_ESP_OK(esp_sleep_pd_config(ESP_PD_DOMAIN_CPU, ESP_PD_OPTION_OFF));
    TEST_ESP_OK(esp_sleep_pd_config(ESP_PD_DOMAIN_TOP, ESP_PD_OPTION_ON));

    esp_sleep_context_t sleep_ctx = {};
    esp_sleep_set_sleep_context(&sleep_ctx);

    rtc_wdt_enable();
    for (int i = 0; i < TEST_ROUNDS; i++) {
        rtc_wdt_feed();
        uint32_t before = read_hp_vol();
        do_timer_light_sleep(LS_SLEEP_US);
        assert_sleep_flags(&sleep_ctx, PMU_SLEEP_PD_CPU | PMU_SLEEP_PD_TOP, PMU_SLEEP_PD_CPU);
        assert_hp_vol_within("ls_pd_cpu", before, read_hp_vol());
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    rtc_wdt_disable();

    esp_sleep_set_sleep_context(NULL);
    pd_domains_restore_auto();
}

TEST_CASE("PVT: PD_TOP light sleep keeps hp_vol stable", "[pvt]")
{
    require_pvt_controlling_hp_dbias();

    TEST_ESP_OK(sleep_cpu_configure(true));
    TEST_ASSERT_TRUE(cpu_domain_pd_allowed());
    TEST_ESP_OK(esp_sleep_pd_config(ESP_PD_DOMAIN_TOP, ESP_PD_OPTION_OFF));

    esp_sleep_context_t sleep_ctx = {};
    esp_sleep_set_sleep_context(&sleep_ctx);

    rtc_wdt_enable();
    for (int i = 0; i < TEST_ROUNDS; i++) {
        rtc_wdt_feed();
        uint32_t before = read_hp_vol();
        do_timer_light_sleep(LS_SLEEP_US);
        assert_sleep_flags(&sleep_ctx, PMU_SLEEP_PD_TOP, PMU_SLEEP_PD_TOP);
        assert_hp_vol_within("ls_pd_top", before, read_hp_vol());
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    rtc_wdt_disable();

    esp_sleep_set_sleep_context(NULL);
    pd_domains_restore_auto();
}

/* After reboot: confirm reset cause, PVT still owns HP dbias, no slow runaway. */
static void verify_pvt_after_reset(const char *tag, esp_reset_reason_t expected)
{
    require_pvt_controlling_hp_dbias();
    printf("[%s] esp_reset_reason=%d\n", tag, (int)esp_reset_reason());
    flush_hp_vol_log();
    TEST_ASSERT_EQUAL(expected, esp_reset_reason());
    sample_assert_no_runaway(tag);
}

#if SOC_DEEP_SLEEP_SUPPORTED
static void pvt_enter_deep_sleep(void)
{
    require_pvt_controlling_hp_dbias();
    flush_hp_vol_log();
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(DS_SLEEP_US));
    esp_deep_sleep_start();
}

static void pvt_check_after_deep_sleep(void)
{
    verify_pvt_after_reset("post_deepsleep", ESP_RST_DEEPSLEEP);
}

TEST_CASE_MULTIPLE_STAGES("PVT: deep sleep wake keeps PVT alive",
                          "[pvt][reset=DEEPSLEEP_RESET]",
                          pvt_enter_deep_sleep,
                          pvt_check_after_deep_sleep)
#endif /* SOC_DEEP_SLEEP_SUPPORTED */

static void pvt_do_sw_restart(void)
{
    require_pvt_controlling_hp_dbias();
    flush_hp_vol_log();
    esp_restart();
}

static void pvt_check_after_sw_restart(void)
{
    verify_pvt_after_reset("post_sw_reset", ESP_RST_SW);
}

TEST_CASE_MULTIPLE_STAGES("PVT: software reset keeps PVT alive",
                          "[pvt][reset=SW_CPU_RESET]",
                          pvt_do_sw_restart,
                          pvt_check_after_sw_restart)

static void pvt_do_rtc_wdt_reset(void)
{
    require_pvt_controlling_hp_dbias();
    flush_hp_vol_log();

    wdt_hal_context_t rtc_wdt_ctx;
    wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, false);
    uint32_t stage_timeout_ticks = rtc_clk_slow_freq_get_hz() / 10; /* ~100 ms */
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_SYSTEM);
    wdt_hal_set_flashboot_en(&rtc_wdt_ctx, true);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    while (1) {
    }
}

static void pvt_check_after_rtc_wdt_reset(void)
{
    verify_pvt_after_reset("post_rtc_wdt", ESP_RST_WDT);
}

TEST_CASE_MULTIPLE_STAGES("PVT: RTC WDT reset keeps PVT alive",
                          "[pvt][reset=RTCWDT_SYS_RESET]",
                          pvt_do_rtc_wdt_reset,
                          pvt_check_after_rtc_wdt_reset)

#if CONFIG_ESP_TASK_WDT_EN
static void pvt_do_task_wdt_reset(void)
{
    require_pvt_controlling_hp_dbias();
    flush_hp_vol_log();

    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = 1000,
        .idle_core_mask = (1 << 0),
        .trigger_panic = true,
    };
    TEST_ESP_OK(esp_task_wdt_init(&twdt_config));
    while (1) {
    }
}

static void pvt_check_after_task_wdt_reset(void)
{
    verify_pvt_after_reset("post_task_wdt", ESP_RST_TASK_WDT);
}

TEST_CASE_MULTIPLE_STAGES("PVT: task WDT reset keeps PVT alive",
                          "[pvt][reset=SW_CPU_RESET]",
                          pvt_do_task_wdt_reset,
                          pvt_check_after_task_wdt_reset)
#endif /* CONFIG_ESP_TASK_WDT_EN */

static void pvt_do_panic_reset(void)
{
    require_pvt_controlling_hp_dbias();
    flush_hp_vol_log();
    abort();
}

static void pvt_check_after_panic_reset(void)
{
    verify_pvt_after_reset("post_panic", ESP_RST_PANIC);
}

TEST_CASE_MULTIPLE_STAGES("PVT: panic reset keeps PVT alive",
                          "[pvt][reset=abort,SW_CPU_RESET]",
                          pvt_do_panic_reset,
                          pvt_check_after_panic_reset)

#if CONFIG_ESP_INT_WDT
static void pvt_do_int_wdt_reset(void)
{
    require_pvt_controlling_hp_dbias();
    flush_hp_vol_log();
#if CONFIG_FREERTOS_SMP
    (void)portDISABLE_INTERRUPTS();
#else
    (void)portSET_INTERRUPT_MASK_FROM_ISR();
#endif
    while (1) {
    }
}

static void pvt_check_after_int_wdt_reset(void)
{
    verify_pvt_after_reset("post_int_wdt", ESP_RST_INT_WDT);
}

TEST_CASE_MULTIPLE_STAGES("PVT: interrupt WDT reset keeps PVT alive",
                          "[pvt][reset=Interrupt wdt timeout on CPU0,SW_CPU_RESET]",
                          pvt_do_int_wdt_reset,
                          pvt_check_after_int_wdt_reset)
#endif /* CONFIG_ESP_INT_WDT */

#endif

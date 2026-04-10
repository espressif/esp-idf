/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdio.h>

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "hal/timer_ll.h"

#include "esp_cpu.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"

#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk_tree_common.h"

#include "test_rv_utils.h"

#include "unity.h"
#include "sdkconfig.h"

#include "test_pms_priv.h"
#include "test_cpu_intr_priv.h"
#include "test_cpu_intr_params.h"

#if SOC_RCC_IS_INDEPENDENT
#define TIMER_RCC_ATOMIC()
#else /* !SOC_RCC_IS_INDEPENDENT */
#define TIMER_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#endif /* SOC_RCC_IS_INDEPENDENT */

#define TEST_INTR_MAX_COUNT       (8)

/* ---------------------------------------------------- Utility - TIMER ---------------------------------------------------- */

#define TEST_TIMER_GROUP          (0)
#define TEST_TIMER_ID             (0)
#define TEST_TIMER_DIVIDER        (80)

#define TEST_TIMER_RESOLUTION_HZ  (1000000ULL) // 1MHz, 1 tick = 1us
#define TEST_TIMER_PERIOD_S       (0.01f)      // 10ms @ resolution 1MHz
#define TEST_INTPRI_PERIOD_S      (0.10f)      // 100ms (intpri trigger interval)
#define TEST_TIMER_EXP_COUNT      (TEST_INTR_MAX_COUNT * (TEST_INTPRI_PERIOD_S / TEST_TIMER_PERIOD_S))

static timg_dev_t *timg_dev = TIMER_LL_GET_HW(TEST_TIMER_GROUP);

/* ---------------------------------------------------- Utility - Common ---------------------------------------------------- */

#define TEST_INTR_TK_INIT            0x8BADF00D
#define TEST_INTR_TK_DONE            0xDEFEC8ED

typedef struct {
    volatile int id;
    volatile int curr_count;
    volatile int max_count;
    volatile int token;
    volatile int expected_mode;
} test_intr_args_ctx_t;

IRAM_ATTR static void test_timer_isr(void *arg)
{
    test_intr_args_ctx_t *ctx = (test_intr_args_ctx_t *)arg;
    ctx->curr_count++;
    TEST_ASSERT_EQUAL(ctx->expected_mode, esp_cpu_get_curr_privilege_level());

    /* Clear interrupt and re-enable the alarm */
    timer_ll_clear_intr_status(timg_dev, TIMER_LL_EVENT_ALARM(TEST_TIMER_ID));
    timer_ll_enable_alarm(timg_dev, TEST_TIMER_ID, true);
}

static void test_timer_deinit(void)
{
    const uint32_t timer_id = TEST_TIMER_ID;

    // Disable and clear timer state
    timer_ll_enable_counter(timg_dev, timer_id, false);
    timer_ll_enable_auto_reload(timg_dev, timer_id, false);
    timer_ll_enable_alarm(timg_dev, timer_id, false);

    timer_ll_enable_intr(timg_dev, TIMER_LL_EVENT_ALARM(timer_id), false);
    timer_ll_clear_intr_status(timg_dev, TIMER_LL_EVENT_ALARM(timer_id));

    // Reset the counter
    uint64_t prev_val = timer_ll_get_reload_value(timg_dev, timer_id);
    timer_ll_set_reload_value(timg_dev, timer_id, 0);
    timer_ll_trigger_soft_reload(timg_dev, timer_id);
    timer_ll_set_reload_value(timg_dev, timer_id, prev_val);

    TIMER_RCC_ATOMIC() {
        timer_ll_enable_clock(TEST_TIMER_GROUP, timer_id, false);
    }
}

static void test_timer_init(int mode, int priority, test_intr_args_ctx_t *arg)
{
    const uint32_t group_id = TEST_TIMER_GROUP;
    const uint32_t timer_id = TEST_TIMER_ID;

    test_timer_deinit();

    // Enable peripheral clock and reset hardware
    _timg_ll_enable_bus_clock(group_id, true);
    _timg_ll_reset_register(group_id);

    // Select clock source and enable module clock
    // Enable the default clock source PLL_F80M
    esp_clk_tree_enable_src(SOC_MOD_CLK_PLL_F80M, true);
    TIMER_RCC_ATOMIC() {
        timer_ll_set_clock_source(group_id, timer_id, GPTIMER_CLK_SRC_DEFAULT);
        timer_ll_enable_clock(group_id, timer_id, true);
    }
    timer_ll_set_clock_prescale(timg_dev, timer_id, TEST_TIMER_DIVIDER);
    timer_ll_set_count_direction(timg_dev, timer_id, GPTIMER_COUNT_UP);

    // Register ISR
    test_intr_alloc(mode, priority, TG0_T0_INTR_SRC, &test_timer_isr, (void *)arg);
    timer_ll_enable_intr(timg_dev, TIMER_LL_EVENT_ALARM(timer_id), true);

    // Configure and enable timer alarm
    timer_ll_set_alarm_value(timg_dev, timer_id, TEST_TIMER_PERIOD_S * TEST_TIMER_RESOLUTION_HZ);
    timer_ll_enable_auto_reload(timg_dev, timer_id, true);
    timer_ll_enable_alarm(timg_dev, timer_id, true);
    timer_ll_enable_counter(timg_dev, timer_id, true);
}

/* ---------------------------------------------------- Utility - INTPRI ---------------------------------------------------- */

FORCE_INLINE_ATTR void intpri_cpu_clr_intr(uint32_t n)
{
    WRITE_PERI_REG(CPU_INTR_FROM_CPU_N_REG(n), 0);
}

FORCE_INLINE_ATTR void intpri_cpu_trig_intr(uint32_t n)
{
    WRITE_PERI_REG(CPU_INTR_FROM_CPU_N_REG(n), 1);
}

FORCE_INLINE_ATTR uint32_t intpri_cpu_get_intr_state(uint32_t n)
{
    return READ_PERI_REG(CPU_INTR_FROM_CPU_N_REG(n));
}

IRAM_ATTR static void test_intpri_isr(void *arg)
{
    test_intr_args_ctx_t *ctx = (test_intr_args_ctx_t *)arg;

    /* Clear interrupt */
    intpri_cpu_clr_intr(ctx->id);

    ctx->curr_count++;
    if (ctx->curr_count >= ctx->max_count) {
        ctx->token = TEST_INTR_TK_DONE;
    }

    TEST_ASSERT_EQUAL(ctx->expected_mode, esp_cpu_get_curr_privilege_level());
}

/* ---------------------------------------------------- Test-cases ---------------------------------------------------- */

void test_m_mode_intr_in_m_mode(void)
{
#if SOC_APM_CTRL_FILTER_SUPPORTED
    test_apm_ctrl_reset_all();
#endif
    test_intr_utils_init();

    test_intr_args_ctx_t ctx1 = {
        .id = 0,
        .curr_count = 0,
        .max_count = TEST_INTR_MAX_COUNT,
        .token = TEST_INTR_TK_INIT,
        .expected_mode = PRV_M,
    };
    test_intr_alloc(PRV_M, 1, CPU_FROM_CPU_N_INTR_SRC(ctx1.id), &test_intpri_isr, &ctx1);

    test_intr_args_ctx_t ctx2 = {
        .curr_count = 0,
        .expected_mode = PRV_M,
    };
    test_timer_init(PRV_M, 2, &ctx2);

    while (ctx1.token != TEST_INTR_TK_DONE) {
        intpri_cpu_trig_intr(ctx1.id);
        test_delay_ms(TEST_INTPRI_PERIOD_S * 1000U);
    }
    TEST_ASSERT(ctx1.curr_count == TEST_INTR_MAX_COUNT);

    test_timer_deinit();
    TEST_ASSERT(ctx2.curr_count >= TEST_TIMER_EXP_COUNT);

    test_intr_free_all();
    test_intr_utils_deinit();
}

void test_u_mode_intr_in_u_mode(void)
{
#if SOC_APM_CTRL_FILTER_SUPPORTED
    test_apm_ctrl_reset_all();
#endif
    test_intr_utils_init();

    test_intr_args_ctx_t ctx1 = {
        .id = 0,
        .curr_count = 0,
        .max_count = TEST_INTR_MAX_COUNT,
        .token = TEST_INTR_TK_INIT,
        .expected_mode = PRV_U,
    };
    test_intr_alloc(PRV_U, 1, CPU_FROM_CPU_N_INTR_SRC(ctx1.id), &test_intpri_isr, &ctx1);

    test_intr_args_ctx_t ctx2 = {
        .curr_count = 0,
        .expected_mode = PRV_U,
    };
    test_timer_init(PRV_U, 2, &ctx2);

    test_m2u_switch();
    while (ctx1.token != TEST_INTR_TK_DONE) {
        intpri_cpu_trig_intr(ctx1.id);
        test_delay_ms(TEST_INTPRI_PERIOD_S * 1000U);
    }
    test_u2m_switch();
    TEST_ASSERT(ctx1.curr_count == TEST_INTR_MAX_COUNT);

    test_timer_deinit();
    TEST_ASSERT(ctx2.curr_count >= TEST_TIMER_EXP_COUNT);

    test_intr_free_all();
    test_intr_utils_deinit();
}

void test_m_mode_intr_in_u_mode(void)
{
#if SOC_APM_CTRL_FILTER_SUPPORTED
    test_apm_ctrl_reset_all();
#endif
    test_intr_utils_init();

    test_intr_alloc(PRV_M, 1, ETS_MAX_INTR_SOURCE, NULL, NULL);

    test_intr_args_ctx_t ctx1 = {
        .id = 0,
        .curr_count = 0,
        .max_count = TEST_INTR_MAX_COUNT,
        .token = TEST_INTR_TK_INIT,
        .expected_mode = PRV_M,
    };
    test_intr_alloc(PRV_M, 1, CPU_FROM_CPU_N_INTR_SRC(ctx1.id), &test_intpri_isr, &ctx1);

    test_intr_args_ctx_t ctx2 = {
        .curr_count = 0,
        .expected_mode = PRV_U,
    };
    test_timer_init(PRV_U, 2, &ctx2);

    test_m2u_switch();
    while (ctx1.token != TEST_INTR_TK_DONE) {
        intpri_cpu_trig_intr(ctx1.id);
        test_delay_ms(TEST_INTPRI_PERIOD_S * 1000U);
    }
    test_u2m_switch();
    TEST_ASSERT(ctx1.curr_count == TEST_INTR_MAX_COUNT);

    test_timer_deinit();
    TEST_ASSERT(ctx2.curr_count >= TEST_TIMER_EXP_COUNT);

    test_intr_free_all();
    test_intr_utils_deinit();
}

void test_u_mode_intr_in_m_mode(void)
{
#if SOC_APM_CTRL_FILTER_SUPPORTED
    test_apm_ctrl_reset_all();
#endif
    test_intr_utils_init();

    test_intr_alloc(PRV_M, 1, ETS_MAX_INTR_SOURCE, NULL, NULL);

    test_intr_args_ctx_t ctx1 = {
        .id = 0,
        .curr_count = 0,
        .max_count = TEST_INTR_MAX_COUNT,
        .token = TEST_INTR_TK_INIT,
        .expected_mode = PRV_U,
    };
    test_intr_alloc(PRV_U, 1, CPU_FROM_CPU_N_INTR_SRC(ctx1.id), &test_intpri_isr, &ctx1);

    test_intr_args_ctx_t ctx2 = {
        .curr_count = 0,
        .expected_mode = PRV_M,
    };
    test_timer_init(PRV_M, 2, &ctx2);

    while (ctx1.token != TEST_INTR_TK_DONE) {
        intpri_cpu_trig_intr(ctx1.id);
        test_delay_ms(TEST_INTPRI_PERIOD_S * 1000U);
    }
    TEST_ASSERT(ctx1.curr_count == TEST_INTR_MAX_COUNT);

    test_timer_deinit();
    TEST_ASSERT(ctx2.curr_count >= TEST_TIMER_EXP_COUNT);

    test_intr_free_all();
    test_intr_utils_deinit();
}

/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include "soc/soc.h"
#include "soc/soc_caps.h"

#include "soc/intpri_reg.h"
#include "soc/pcr_reg.h"
#include "hal/timer_ll.h"

#include "esp_cpu.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"

#include "test_rv_utils.h"

#include "unity.h"
#include "sdkconfig.h"

#include "test_pms_priv.h"
#include "test_cpu_intr_priv.h"
#include "test_cpu_intr_params.h"

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

IRAM_ATTR static void test_timer_isr(void *arg)
{
    uint32_t *intr_count = (uint32_t *)arg;
    *intr_count = *intr_count + 1;
    esp_rom_printf("[mode: %d] TIMER-%d interrupt triggered (%d)\n", esp_cpu_get_curr_privilege_level(), TEST_TIMER_GROUP, *intr_count);

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

    timer_ll_enable_clock(TEST_TIMER_GROUP, timer_id, false);
}

static void test_timer_init(int mode, volatile uint32_t *arg)
{
    const uint32_t group_id = TEST_TIMER_GROUP;
    const uint32_t timer_id = TEST_TIMER_ID;

    test_timer_deinit();

    // Enable peripheral clock and reset hardware
    _timg_ll_enable_bus_clock(group_id, true);
    _timg_ll_reset_register(group_id);

    // Select clock source and enable module clock
    // Enable the default clock source PLL_F80M
    REG_SET_BIT(PCR_PLL_DIV_CLK_EN_REG, PCR_PLL_80M_CLK_EN);
    timer_ll_set_clock_source(group_id, timer_id, GPTIMER_CLK_SRC_DEFAULT);
    timer_ll_enable_clock(group_id, timer_id, true);
    timer_ll_set_clock_prescale(timg_dev, timer_id, TEST_TIMER_DIVIDER);
    timer_ll_set_count_direction(timg_dev, timer_id, GPTIMER_COUNT_UP);

    // Register ISR
    test_intr_alloc(mode, 2, TG0_T0_INTR_SRC, &test_timer_isr, (void *)arg);
    timer_ll_enable_intr(timg_dev, TIMER_LL_EVENT_ALARM(timer_id), true);

    // Configure and enable timer alarm
    timer_ll_set_alarm_value(timg_dev, timer_id, TEST_TIMER_PERIOD_S * TEST_TIMER_RESOLUTION_HZ);
    timer_ll_enable_auto_reload(timg_dev, timer_id, true);
    timer_ll_enable_alarm(timg_dev, timer_id, true);
    timer_ll_enable_counter(timg_dev, timer_id, true);
}

/* ---------------------------------------------------- Utility - INTPRI ---------------------------------------------------- */

#define TEST_INTR_TK_INIT            0x8BADF00D
#define TEST_INTR_TK_DONE            0xDEFEC8ED

#define CPU_INTR_FROM_CPU_N_REG(n)         (INTPRI_CPU_INTR_FROM_CPU_0_REG + 4 * (n))

typedef struct {
    volatile int id;
    volatile int curr_count;
    volatile int max_count;
    volatile int token;
} test_intr_args_ctx_t;

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

    esp_rom_printf("[mode: %d] INTPRI-%d interrupt triggered (%d)\n", esp_cpu_get_curr_privilege_level(), ctx->id, ctx->curr_count);
}

/* ---------------------------------------------------- Test-cases ---------------------------------------------------- */

void test_m_mode_intr_in_m_mode(void)
{
    test_apm_ctrl_reset_all();
    test_intr_utils_init();

    test_intr_args_ctx_t ctx = {
        .id = 0,
        .curr_count = 0,
        .max_count = TEST_INTR_MAX_COUNT,
        .token = TEST_INTR_TK_INIT,
    };

    test_intr_alloc(PRV_M, 1, CPU_FROM_CPU_N_INTR_SRC(ctx.id), &test_intpri_isr, &ctx);

    volatile uint32_t intr_count = 0;
    test_timer_init(PRV_M, &intr_count);

    while (ctx.token != TEST_INTR_TK_DONE) {
        intpri_cpu_trig_intr(ctx.id);
        test_delay_ms(TEST_INTPRI_PERIOD_S * 1000U);
    }
    TEST_ASSERT(ctx.curr_count == TEST_INTR_MAX_COUNT);

    test_timer_deinit();
    TEST_ASSERT(intr_count >= TEST_TIMER_EXP_COUNT);

    test_intr_free_all();
    test_intr_utils_deinit();
}

void test_u_mode_intr_in_u_mode(void)
{
    test_apm_ctrl_reset_all();
    test_intr_utils_init();

    test_intr_args_ctx_t ctx = {
        .id = 0,
        .curr_count = 0,
        .max_count = TEST_INTR_MAX_COUNT,
        .token = TEST_INTR_TK_INIT,
    };

    test_intr_alloc(PRV_U, 1, CPU_FROM_CPU_N_INTR_SRC(ctx.id), &test_intpri_isr, &ctx);

    volatile uint32_t intr_count = 0;
    test_timer_init(PRV_U, &intr_count);

    test_m2u_switch();
    while (ctx.token != TEST_INTR_TK_DONE) {
        intpri_cpu_trig_intr(ctx.id);
        test_delay_ms(TEST_INTPRI_PERIOD_S * 1000U);
    }
    test_u2m_switch();
    TEST_ASSERT(ctx.curr_count == TEST_INTR_MAX_COUNT);

    test_timer_deinit();
    TEST_ASSERT(intr_count >= TEST_TIMER_EXP_COUNT);

    test_intr_free_all();
    test_intr_utils_deinit();
}

void test_m_mode_intr_in_u_mode(void)
{
    test_apm_ctrl_reset_all();
    test_intr_utils_init();

    test_intr_args_ctx_t ctx = {
        .id = 0,
        .curr_count = 0,
        .max_count = TEST_INTR_MAX_COUNT,
        .token = TEST_INTR_TK_INIT,
    };

    test_intr_alloc(PRV_M, 1, CPU_FROM_CPU_N_INTR_SRC(ctx.id), &test_intpri_isr, &ctx);

    test_intr_alloc(PRV_M, 1, ETS_MAX_INTR_SOURCE, NULL, NULL);

    volatile uint32_t intr_count = 0;
    test_timer_init(PRV_U, &intr_count);

    test_m2u_switch();
    while (ctx.token != TEST_INTR_TK_DONE) {
        intpri_cpu_trig_intr(ctx.id);
        test_delay_ms(TEST_INTPRI_PERIOD_S * 1000U);
    }
    test_u2m_switch();
    TEST_ASSERT(ctx.curr_count == TEST_INTR_MAX_COUNT);

    test_timer_deinit();
    TEST_ASSERT(intr_count >= TEST_TIMER_EXP_COUNT);

    test_intr_free_all();
    test_intr_utils_deinit();
}

void test_u_mode_intr_in_m_mode(void)
{
    test_apm_ctrl_reset_all();
    test_intr_utils_init();

    test_intr_alloc(PRV_M, 1, ETS_MAX_INTR_SOURCE, NULL, NULL);

    test_intr_args_ctx_t ctx = {
        .id = 0,
        .curr_count = 0,
        .max_count = TEST_INTR_MAX_COUNT,
        .token = TEST_INTR_TK_INIT,
    };

    test_intr_alloc(PRV_U, 1, CPU_FROM_CPU_N_INTR_SRC(ctx.id), &test_intpri_isr, &ctx);

    volatile uint32_t intr_count = 0;
    test_timer_init(PRV_M, &intr_count);

    while (ctx.token != TEST_INTR_TK_DONE) {
        intpri_cpu_trig_intr(ctx.id);
        test_delay_ms(TEST_INTPRI_PERIOD_S * 1000U);
    }
    TEST_ASSERT(ctx.curr_count == TEST_INTR_MAX_COUNT);

    test_timer_deinit();
    TEST_ASSERT(intr_count >= TEST_TIMER_EXP_COUNT);

    test_intr_free_all();
    test_intr_utils_deinit();
}

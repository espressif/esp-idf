/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_cpu.h"
#include "esp_log.h"
#include "riscv/csr.h"

#include "hal/timer_ll.h"
#include "hal/clk_gate_ll.h"

#include "soc/timer_group_reg.h"
#include "soc/interrupt_matrix_reg.h"

#include "esp_tee.h"
#include "esp_tee_intr.h"
#include "esp_tee_test.h"

#define TIMER_DIVIDER         80        //  Hardware timer clock divider
#define TIMER_RESOLUTION_HZ   1000000  // 1MHz resolution
#define TIMER_ALARM_PERIOD_S  0.10     // sample test interval for the first timer

/* TEE uses Group0 Timer0 */
#define TEE_SECURE_GROUP 0
#define TEE_SECURE_TIMER 0

static const char *TAG = "esp_tee_intr_test";

static timg_dev_t *timg_hw = (&TIMERG0);

uint32_t *psecure_int_count = NULL;

static void IRAM_ATTR timer_group0_isr(void *arg)
{
    ESP_LOGI(TAG, "Timer ISR Handler from World %d!", esp_cpu_get_curr_privilege_level());

    /* For interrupt test. */
    *psecure_int_count = *psecure_int_count + 1;

    /* Clear interrupt */
    timer_ll_clear_intr_status(timg_hw, TIMER_LL_EVENT_ALARM(TEE_SECURE_TIMER));

    /* Re-enable the alarm. */
    timer_ll_enable_alarm(timg_hw, TEE_SECURE_TIMER, true);
}

static void tee_timer_enable(void)
{
    struct vector_desc_t timer_vd = { 0, NULL, NULL, NULL };

    // init timer_vc
    timer_vd.source = ETS_TG0_T0_LEVEL_INTR_SOURCE;
    timer_vd.isr = timer_group0_isr;

    ESP_LOGI(TAG, "Enabling test timer from secure world");

    /* Enable TG0 peripheral module */
    _timer_ll_enable_bus_clock(TEE_SECURE_GROUP, true);
    _timer_ll_reset_register(TEE_SECURE_GROUP);

    /* Stop counter, alarm, auto-reload at first place */
    timer_ll_enable_clock(TEE_SECURE_GROUP, TEE_SECURE_TIMER, true);
    timer_ll_enable_counter(timg_hw, TEE_SECURE_TIMER, false);
    timer_ll_enable_auto_reload(timg_hw, TEE_SECURE_TIMER, false);
    timer_ll_enable_alarm(timg_hw, TEE_SECURE_TIMER, false);

    // Set clock source
    timer_ll_set_clock_source(TEE_SECURE_GROUP, TEE_SECURE_TIMER, GPTIMER_CLK_SRC_DEFAULT);
    timer_ll_set_clock_prescale(timg_hw, TEE_SECURE_TIMER, TIMER_DIVIDER);

    // Initialize counter value to zero
    timer_ll_set_reload_value(timg_hw, TEE_SECURE_TIMER, 0);
    timer_ll_trigger_soft_reload(timg_hw, TEE_SECURE_TIMER);

    // set counting direction
    timer_ll_set_count_direction(timg_hw, TEE_SECURE_TIMER, GPTIMER_COUNT_UP);

    // disable interrupt
    timer_ll_enable_intr(timg_hw, TIMER_LL_EVENT_ALARM(TEE_SECURE_TIMER), false);
    // clear pending interrupt event
    timer_ll_clear_intr_status(timg_hw, TIMER_LL_EVENT_ALARM(TEE_SECURE_TIMER));

    esp_tee_intr_register((void *)&timer_vd);
    timer_ll_enable_intr(timg_hw, TIMER_LL_EVENT_ALARM(TEE_SECURE_TIMER), true);

    timer_ll_set_reload_value(timg_hw, TEE_SECURE_TIMER, 0);

    // enable timer interrupt
    timer_ll_enable_intr(timg_hw, TIMER_LL_EVENT_ALARM(TEE_SECURE_TIMER), true);

    // set timer alarm
    uint64_t alarm_value = (TIMER_ALARM_PERIOD_S * TIMER_RESOLUTION_HZ);
    timer_ll_set_alarm_value(timg_hw, TEE_SECURE_TIMER, alarm_value);

    timer_ll_enable_auto_reload(timg_hw, TEE_SECURE_TIMER, true);
    timer_ll_enable_alarm(timg_hw, TEE_SECURE_TIMER, true);
    timer_ll_enable_counter(timg_hw, TEE_SECURE_TIMER, true);
}

static void tee_timer_disable(void)
{
    ESP_LOGI(TAG, "Disabling test timer from secure world");

    /* Init timer interrupt vector descriptor */
    struct vector_desc_t timer_vd = { 0, NULL, NULL, NULL };
    timer_vd.source = ETS_TG0_T0_LEVEL_INTR_SOURCE;
    timer_vd.isr = timer_group0_isr;

    esp_tee_intr_deregister((void *)&timer_vd);

    /* Disable timer */
    timer_ll_enable_counter(timg_hw, TEE_SECURE_TIMER, false);
    timer_ll_enable_auto_reload(timg_hw, TEE_SECURE_TIMER, false);
    timer_ll_enable_alarm(timg_hw, TEE_SECURE_TIMER, false);

    /* Disable and clear interrupt */
    timer_ll_enable_intr(timg_hw, TIMER_LL_EVENT_ALARM(TEE_SECURE_TIMER), false);
    timer_ll_clear_intr_status(timg_hw, TIMER_LL_EVENT_ALARM(TEE_SECURE_TIMER));

    /* Disable TG0 peripheral module */
    // periph_ll_disable_clk_set_rst(PERIPH_TIMG0_MODULE);
}

void _ss_esp_tee_test_timer_init(bool enable)
{
    if (enable) {
        tee_timer_enable();
    } else {
        tee_timer_disable();
    }
}

/**
 * Secure interrupt in secure world test.
 */
int _ss_esp_tee_secure_int_test(void)
{
    ESP_LOGD(TAG, "In WORLD %d", esp_cpu_get_curr_privilege_level());
    volatile uint32_t secure_int_count = 0;
    psecure_int_count = (uint32_t *)&secure_int_count;

    _ss_esp_tee_test_timer_init(true);
    while (secure_int_count < TEE_TEST_INT_COUNT);
    _ss_esp_tee_test_timer_init(false);

    ESP_LOGD(TAG, "Exiting WORLD %d", esp_cpu_get_curr_privilege_level());
    return secure_int_count;
}

/**
 * Non-Secure interrupt in secure world test.
 */
int _ss_esp_tee_non_secure_int_test(volatile uint32_t *volatile ns_int_count)
{
    ESP_LOGD(TAG, "In WORLD %d", esp_cpu_get_curr_privilege_level());

    uint32_t count = 0;
    count = *ns_int_count;

    while ((*ns_int_count < TEE_TEST_INT_COUNT)) {
        if (*ns_int_count > count) {
            count = *ns_int_count;
            ESP_LOGI(TAG, "Interrupt count %d", count);
        }
    }

    ESP_LOGD(TAG, "Exiting WORLD %d", esp_cpu_get_curr_privilege_level());
    return 0;
}

int _ss_esp_tee_test_int_count(uint32_t *secure_int_count)
{
    psecure_int_count = secure_int_count;
    return (*psecure_int_count);
}

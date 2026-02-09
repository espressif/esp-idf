/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>

#include "esp_cpu.h"
#include "esp_rom_sys.h"

#include "soc/soc.h"
#include "soc/interrupts.h"
#include "soc/interrupt_matrix_reg.h"
#include "soc/pcr_reg.h"
#include "hal/timer_ll.h"

#include "esp_tee.h"
#include "esp_tee_intr.h"
#include "esp_tee_test.h"

static const char *TAG __attribute__((unused)) = "esp_tee_intr_test";

#if CONFIG_IDF_TARGET_ESP32C61
#define TIMER_INTR_SRC ETS_TG0_T0_INTR_SOURCE
#else
#define TIMER_INTR_SRC ETS_TG0_T0_LEVEL_INTR_SOURCE
#endif

/* ---------------------------------------------------- Utility functions ---------------------------------------------------- */

#define TEST_TIMER_GROUP          (0)
#define TEST_TIMER_ID             (0)
#define TEST_TIMER_DIVIDER        (80)

#define TEST_TIMER_RESOLUTION_HZ  (1000000ULL) // 1MHz, 1 tick = 1us
#define TIMER_ALARM_PERIOD_S      (0.25f)      // 250ms @ resolution 1MHz

static timg_dev_t *timg_dev = TIMER_LL_GET_HW(TEST_TIMER_GROUP);

static void IRAM_ATTR test_timer_isr(void *arg)
{
    uint32_t *intr_count = (uint32_t *)arg;
    *intr_count = *intr_count + 1;
    esp_rom_printf("[mode: %d] Interrupt triggered (%d)\n", esp_cpu_get_curr_privilege_level(), *intr_count);

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

    // Deregister ISR
    struct vector_desc_t timer_vd = {
        .source = TIMER_INTR_SRC,
    };
    esp_tee_intr_deregister((void *)&timer_vd);
    timer_ll_enable_clock(TEST_TIMER_GROUP, timer_id, false);
}

static void test_timer_init(volatile uint32_t *arg)
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
    struct vector_desc_t timer_vd = {
        .source = TIMER_INTR_SRC,
        .isr = test_timer_isr,
        .arg = (void *)arg,
    };
    esp_tee_intr_register((void *)&timer_vd);
    timer_ll_enable_intr(timg_dev, TIMER_LL_EVENT_ALARM(timer_id), true);

    // Configure and enable timer alarm
    timer_ll_set_alarm_value(timg_dev, timer_id, TIMER_ALARM_PERIOD_S * TEST_TIMER_RESOLUTION_HZ);
    timer_ll_enable_auto_reload(timg_dev, timer_id, true);
    timer_ll_enable_alarm(timg_dev, timer_id, true);
    timer_ll_enable_counter(timg_dev, timer_id, true);
}

/* ---------------------------------------------------- Test cases ---------------------------------------------------- */

uint32_t _ss_esp_tee_test_tee_intr_in_tee(void)
{
    esp_cpu_priv_mode_t mode = esp_cpu_get_curr_privilege_level();
    assert((mode == ESP_CPU_S_MODE) && "Incorrect privilege mode!");

    volatile uint32_t tee_intr_count = 0;
    test_timer_init(&tee_intr_count);

    while (tee_intr_count < ESP_TEE_TEST_INTR_ITER) {
        esp_rom_delay_us(10 * 1000U);
    }

    test_timer_deinit();

    mode = esp_cpu_get_curr_privilege_level();
    assert((mode == ESP_CPU_S_MODE) && "Incorrect privilege mode!");
    return tee_intr_count;
}

uint32_t _ss_esp_tee_test_tee_intr_in_ree(int stage, volatile uint32_t *volatile intr_count)
{
    esp_cpu_priv_mode_t mode = esp_cpu_get_curr_privilege_level();
    assert((mode == ESP_CPU_S_MODE) && "Incorrect privilege mode!");

    switch (stage) {
    case 0:
        test_timer_init(intr_count);
        break;
    case 1:
        test_timer_deinit();
        break;
    default:
        assert(false && "Invalid stage!");
        break;
    }

    mode = esp_cpu_get_curr_privilege_level();
    assert((mode == ESP_CPU_S_MODE) && "Incorrect privilege mode!");
    return 0;
}

uint32_t _ss_esp_tee_test_ree_intr_in_tee(volatile uint32_t *volatile intr_count)
{
    esp_cpu_priv_mode_t mode = esp_cpu_get_curr_privilege_level();
    assert((mode == ESP_CPU_S_MODE) && "Incorrect privilege mode!");

    uint32_t prev_count = 0;
    while (true) {
        uint32_t curr_count = *intr_count;
        if (curr_count > prev_count) {
            prev_count = curr_count;
            esp_rom_printf("[mode: %d] Interrupt received (%d)\n", esp_cpu_get_curr_privilege_level(), curr_count);
        }
        if (curr_count >= ESP_TEE_TEST_INTR_ITER) {
            break;
        }
        esp_rom_delay_us(1000U);
    }

    mode = esp_cpu_get_curr_privilege_level();
    assert((mode == ESP_CPU_S_MODE) && "Incorrect privilege mode!");
    return 0;
}

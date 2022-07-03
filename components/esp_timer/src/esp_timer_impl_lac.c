/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sys/param.h"
#include "esp_timer_impl.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_task.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "esp_private/esp_clk.h"
#include "esp_private/periph_ctrl.h"
#include "soc/soc.h"
#include "soc/timer_group_reg.h"
#include "soc/rtc.h"
#include "freertos/FreeRTOS.h"

/**
 * @file esp_timer_lac.c
 * @brief Implementation of chip-specific part of esp_timer
 *
 * This implementation uses TG0 LAC timer of the ESP32. This timer is
 * a 64-bit up-counting timer, with a programmable compare value (called 'alarm'
 * hereafter). When the timer reaches compare value, interrupt is raised.
 * The timer can be configured to produce an edge or a level interrupt.
 */

/* Selects which Timer Group peripheral to use */
#define LACT_MODULE     0

#if LACT_MODULE == 0
#define INTR_SOURCE_LACT ETS_TG0_LACT_LEVEL_INTR_SOURCE
#define PERIPH_LACT PERIPH_TIMG0_MODULE
#elif LACT_MODULE == 1
#define INTR_SOURCE_LACT ETS_TG1_LACT_LEVEL_INTR_SOURCE
#define PERIPH_LACT PERIPH_TIMG1_MODULE
#else
#error "Incorrect the number of LACT module (only 0 or 1)"
#endif

/* Desired number of timer ticks per microsecond.
 * This value should be small enough so that all possible APB frequencies
 * could be divided by it without remainder.
 * On the other hand, the smaller this value is, the longer we need to wait
 * after setting UPDATE_REG before the timer value can be read.
 * If TICKS_PER_US == 1, then we need to wait up to 1 microsecond, which
 * makes esp_timer_impl_get_time function take too much time.
 * The value TICKS_PER_US == 2 allows for most of the APB frequencies, and
 * allows reading the counter quickly enough.
 */
#define TICKS_PER_US    2

/* Shorter register names, used in this file */
#define CONFIG_REG      (TIMG_LACTCONFIG_REG(LACT_MODULE))
#define RTC_STEP_REG    (TIMG_LACTRTC_REG(LACT_MODULE))
#define ALARM_LO_REG    (TIMG_LACTALARMLO_REG(LACT_MODULE))
#define ALARM_HI_REG    (TIMG_LACTALARMHI_REG(LACT_MODULE))
#define COUNT_LO_REG    (TIMG_LACTLO_REG(LACT_MODULE))
#define COUNT_HI_REG    (TIMG_LACTHI_REG(LACT_MODULE))
#define UPDATE_REG      (TIMG_LACTUPDATE_REG(LACT_MODULE))
#define LOAD_REG        (TIMG_LACTLOAD_REG(LACT_MODULE))
#define LOAD_LO_REG     (TIMG_LACTLOADLO_REG(LACT_MODULE))
#define LOAD_HI_REG     (TIMG_LACTLOADHI_REG(LACT_MODULE))
#define INT_ENA_REG     (TIMG_INT_ENA_TIMERS_REG(LACT_MODULE))
#define INT_ST_REG      (TIMG_INT_ST_TIMERS_REG(LACT_MODULE))
#define INT_CLR_REG     (TIMG_INT_CLR_TIMERS_REG(LACT_MODULE))

/* Helper type to convert between a 64-bit value and a pair of 32-bit values without shifts and masks */
typedef struct {
    union {
        struct {
            uint32_t lo;
            uint32_t hi;
        };
        uint64_t val;
    };
} timer_64b_reg_t;

static const char* TAG = "esp_timer_impl";

/* Interrupt handle returned by the interrupt allocator */
static intr_handle_t s_timer_interrupt_handle;

/* Function from the upper layer to be called when the interrupt happens.
 * Registered in esp_timer_impl_init.
 */
static intr_handler_t s_alarm_handler = NULL;

/* Spinlock used to protect access to the hardware registers. */
portMUX_TYPE s_time_update_lock = portMUX_INITIALIZER_UNLOCKED;


void esp_timer_impl_lock(void)
{
    portENTER_CRITICAL(&s_time_update_lock);
}

void esp_timer_impl_unlock(void)
{
    portEXIT_CRITICAL(&s_time_update_lock);
}

uint64_t IRAM_ATTR esp_timer_impl_get_counter_reg(void)
{
    uint32_t lo, hi;
    uint32_t lo_start = REG_READ(COUNT_LO_REG);
    uint32_t div = REG_GET_FIELD(CONFIG_REG, TIMG_LACT_DIVIDER);
    /* The peripheral doesn't have a bit to indicate that the update is done, so we poll the
     * lower 32 bit part of the counter until it changes, or a timeout expires.
     */
    REG_WRITE(UPDATE_REG, 1);
    do {
        lo = REG_READ(COUNT_LO_REG);
    } while (lo == lo_start && div-- > 0);

    /* Since this function is called without a critical section, verify that LO and HI
     * registers are consistent. That is, if an interrupt happens between reading LO and
     * HI registers, and esp_timer_impl_get_time is called from an ISR, then try to
     * detect this by the change in LO register value, and re-read both registers.
     */
    do {
        lo_start = lo;
        hi = REG_READ(COUNT_HI_REG);
        lo = REG_READ(COUNT_LO_REG);
    } while (lo != lo_start);

    timer_64b_reg_t result = {
        .lo = lo,
        .hi = hi
    };
    return result.val;
}

int64_t IRAM_ATTR esp_timer_impl_get_time(void)
{
    return esp_timer_impl_get_counter_reg() / TICKS_PER_US;
}

int64_t esp_timer_get_time(void) __attribute__((alias("esp_timer_impl_get_time")));

void IRAM_ATTR esp_timer_impl_set_alarm_id(uint64_t timestamp, unsigned alarm_id)
{
    static uint64_t timestamp_id[2] = { UINT64_MAX, UINT64_MAX };
    portENTER_CRITICAL_SAFE(&s_time_update_lock);
    timestamp_id[alarm_id] = timestamp;
    timestamp = MIN(timestamp_id[0], timestamp_id[1]);
    if (timestamp != UINT64_MAX) {
        int64_t offset = TICKS_PER_US * 2;
        uint64_t now_time = esp_timer_impl_get_counter_reg();
        timer_64b_reg_t alarm = { .val = MAX(timestamp * TICKS_PER_US, now_time + offset) };
        do {
            REG_CLR_BIT(CONFIG_REG, TIMG_LACT_ALARM_EN);
            REG_WRITE(ALARM_LO_REG, alarm.lo);
            REG_WRITE(ALARM_HI_REG, alarm.hi);
            REG_SET_BIT(CONFIG_REG, TIMG_LACT_ALARM_EN);
            now_time = esp_timer_impl_get_counter_reg();
            int64_t delta = (int64_t)alarm.val - (int64_t)now_time;
            if (delta <= 0 && REG_GET_FIELD(INT_ST_REG, TIMG_LACT_INT_ST) == 0) {
                // new alarm is less than the counter and the interrupt flag is not set
                offset += llabs(delta) + TICKS_PER_US * 2;
                alarm.val = now_time + offset;
            } else {
                // finish if either (alarm > counter) or the interrupt flag is already set.
                break;
            }
        } while(1);
    }
    portEXIT_CRITICAL_SAFE(&s_time_update_lock);
}

void IRAM_ATTR esp_timer_impl_set_alarm(uint64_t timestamp)
{
    esp_timer_impl_set_alarm_id(timestamp, 0);
}

static void IRAM_ATTR timer_alarm_isr(void *arg)
{
    /* Clear interrupt status */
    REG_WRITE(INT_CLR_REG, TIMG_LACT_INT_CLR);
    /*  Call the upper layer handler */
    (*s_alarm_handler)(arg);
}

void IRAM_ATTR esp_timer_impl_update_apb_freq(uint32_t apb_ticks_per_us)
{
    portENTER_CRITICAL(&s_time_update_lock);
    assert(apb_ticks_per_us >= 3 && "divider value too low");
    assert(apb_ticks_per_us % TICKS_PER_US == 0 && "APB frequency (in MHz) should be divisible by TICK_PER_US");
    REG_SET_FIELD(CONFIG_REG, TIMG_LACT_DIVIDER, apb_ticks_per_us / TICKS_PER_US);
    portEXIT_CRITICAL(&s_time_update_lock);
}

void esp_timer_impl_set(uint64_t new_us)
{
    portENTER_CRITICAL(&s_time_update_lock);
    timer_64b_reg_t dst = { .val = new_us * TICKS_PER_US };
    REG_WRITE(LOAD_LO_REG, dst.lo);
    REG_WRITE(LOAD_HI_REG, dst.hi);
    REG_WRITE(LOAD_REG, 1);
    portEXIT_CRITICAL(&s_time_update_lock);
}

void esp_timer_impl_advance(int64_t time_diff_us)
{
    uint64_t now = esp_timer_impl_get_time();
    esp_timer_impl_set(now + time_diff_us);
}

esp_err_t esp_timer_impl_early_init(void)
{
    periph_module_enable(PERIPH_LACT);

    REG_WRITE(CONFIG_REG, 0);
    REG_WRITE(LOAD_LO_REG, 0);
    REG_WRITE(LOAD_HI_REG, 0);
    REG_WRITE(ALARM_LO_REG, UINT32_MAX);
    REG_WRITE(ALARM_HI_REG, UINT32_MAX);
    REG_WRITE(LOAD_REG, 1);
    REG_SET_BIT(INT_CLR_REG, TIMG_LACT_INT_CLR);
    REG_SET_FIELD(CONFIG_REG, TIMG_LACT_DIVIDER, APB_CLK_FREQ / 1000000 / TICKS_PER_US);
    REG_SET_BIT(CONFIG_REG, TIMG_LACT_INCREASE |
        TIMG_LACT_LEVEL_INT_EN |
        TIMG_LACT_EN);

    return ESP_OK;
}

esp_err_t esp_timer_impl_init(intr_handler_t alarm_handler)
{
    s_alarm_handler = alarm_handler;

    const int interrupt_lvl = (1 << CONFIG_ESP_TIMER_INTERRUPT_LEVEL) & ESP_INTR_FLAG_LEVELMASK;
    esp_err_t err = esp_intr_alloc(INTR_SOURCE_LACT,
            ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_IRAM | interrupt_lvl,
            &timer_alarm_isr, NULL, &s_timer_interrupt_handle);

    if (err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "esp_intr_alloc failed (0x%0x)", err);
        return err;
    }

    /* In theory, this needs a shared spinlock with the timer group driver.
     * However since esp_timer_impl_init is called early at startup, this
     * will not cause issues in practice.
     */
    REG_SET_BIT(INT_ENA_REG, TIMG_LACT_INT_ENA);

    esp_timer_impl_update_apb_freq(esp_clk_apb_freq() / 1000000);

    // Set the step for the sleep mode when the timer will work
    // from a slow_clk frequency instead of the APB frequency.
    uint32_t slowclk_ticks_per_us = esp_clk_slowclk_cal_get() * TICKS_PER_US;
    REG_SET_FIELD(RTC_STEP_REG, TIMG_LACT_RTC_STEP_LEN, slowclk_ticks_per_us);

    ESP_ERROR_CHECK( esp_intr_enable(s_timer_interrupt_handle) );

    return ESP_OK;
}

void esp_timer_impl_deinit(void)
{
    REG_WRITE(CONFIG_REG, 0);
    REG_SET_BIT(INT_CLR_REG, TIMG_LACT_INT_CLR);
    /* TODO: also clear TIMG_LACT_INT_ENA; however see the note in esp_timer_impl_init. */

    esp_intr_disable(s_timer_interrupt_handle);
    esp_intr_free(s_timer_interrupt_handle);
    s_timer_interrupt_handle = NULL;
}

/* FIXME: This value is safe for 80MHz APB frequency, should be modified to depend on clock frequency. */
uint64_t IRAM_ATTR esp_timer_impl_get_min_period_us(void)
{
    return 50;
}

uint64_t esp_timer_impl_get_alarm_reg(void)
{
    portENTER_CRITICAL_SAFE(&s_time_update_lock);
    timer_64b_reg_t alarm = {
        .lo = REG_READ(ALARM_LO_REG),
        .hi = REG_READ(ALARM_HI_REG)
    };
    portEXIT_CRITICAL_SAFE(&s_time_update_lock);
    return alarm.val;
}

void esp_timer_private_update_apb_freq(uint32_t apb_ticks_per_us) __attribute__((alias("esp_timer_impl_update_apb_freq")));
void esp_timer_private_set(uint64_t new_us) __attribute__((alias("esp_timer_impl_set")));
void esp_timer_private_advance(int64_t time_diff_us) __attribute__((alias("esp_timer_impl_advance")));
void esp_timer_private_lock(void) __attribute__((alias("esp_timer_impl_lock")));
void esp_timer_private_unlock(void) __attribute__((alias("esp_timer_impl_unlock")));

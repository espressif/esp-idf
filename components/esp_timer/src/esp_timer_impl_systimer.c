// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <sys/param.h>
#include "esp_timer_impl.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "soc/rtc.h"
#include "soc/systimer_reg.h"
#include "soc/periph_defs.h"
#include "freertos/FreeRTOS.h"

/**
 * @file esp_timer_systimer.c
 * @brief Implementation of chip-specific part of esp_timer
 *
 * This implementation uses SYSTIMER of the ESP32-S2. This timer is
 * a 64-bit up-counting timer, with a programmable compare value (called 'alarm'
 * hereafter). When the timer reaches compare value, interrupt is raised.
 * The timer can be configured to produce an edge or a level interrupt.
 */

/* esp_timer uses the 2 compare unit of SYSTIMER. */
#define INTR_SOURCE_LACT                 (ETS_SYSTIMER_TARGET2_EDGE_INTR_SOURCE)
// Registers
#define COUNT_LO_REG                     (SYSTIMER_VALUE_LO_REG)
#define COUNT_HI_REG                     (SYSTIMER_VALUE_HI_REG)
#define LOAD_LO_REG                      (SYSTIMER_LOAD_LO_REG)
#define LOAD_HI_REG                      (SYSTIMER_LOAD_HI_REG)
#define ALARM_LO_REG                     (SYSTIMER_TARGET2_LO_REG)
#define ALARM_HI_REG                     (SYSTIMER_TARGET2_HI_REG)
// Macros
#define ENABLE_CLK()                     (REG_SET_BIT(SYSTIMER_CONF_REG, SYSTIMER_CLK_EN))
#define ENABLE_INT()                     (REG_SET_BIT(SYSTIMER_INT_ENA_REG, SYSTIMER_INT2_ENA))
#define DISABLE_INT()                    (REG_CLR_BIT(SYSTIMER_INT_ENA_REG, SYSTIMER_INT2_ENA))
#define GET_INT_FLAG()                   (REG_GET_FIELD(SYSTIMER_INT_RAW_REG, SYSTIMER_INT2_RAW))
#define CLEAR_INT()                      (REG_WRITE(SYSTIMER_INT_CLR_REG, SYSTIMER_INT2_CLR))
#define DISABLE_COMPARE_UNIT()           (REG_WRITE(SYSTIMER_TARGET2_CONF_REG, 0))
#define ENABLE_COMPARE_UNIT()            (REG_WRITE(SYSTIMER_TARGET2_CONF_REG, SYSTIMER_TARGET2_WORK_EN))
#define APPLY_LOADED_VAL()               (REG_SET_BIT(SYSTIMER_LOAD_REG, SYSTIMER_TIMER_LOAD))
#define SETTING_STEP_FOR_PLL_SRC(step)   (REG_SET_FIELD(SYSTIMER_STEP_REG, SYSTIMER_TIMER_PLL_STEP, step))
#define SETTING_STEP_FOR_XTAL_SRC(step)  (REG_SET_FIELD(SYSTIMER_STEP_REG, SYSTIMER_TIMER_XTAL_STEP, step))
#define UPDATE_COUNT_REG()               (REG_WRITE(SYSTIMER_UPDATE_REG, SYSTIMER_TIMER_UPDATE))
#define GET_FLAG_UPDATED_COUNT_REG()     (REG_GET_BIT(SYSTIMER_UPDATE_REG, SYSTIMER_TIMER_VALUE_VALID))

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
static intr_handler_t s_alarm_handler;

/* Number of timer ticks per microsecond. */
#define TICKS_PER_US    (APB_CLK_FREQ / 1000000)

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
    uint32_t lo, lo_start, hi;
    /* Set the "update" bit and wait for acknowledgment */
    UPDATE_COUNT_REG();
    while (GET_FLAG_UPDATED_COUNT_REG() == 0) {
        ;
    }
    /* Read LO, HI, then LO again, check that LO returns the same value.
     * This accounts for the case when an interrupt may happen between reading
     * HI and LO values, and this function may get called from the ISR.
     * In this case, the repeated read will return consistent values.
     */
    lo_start = REG_READ(COUNT_LO_REG);
    do {
        lo = lo_start;
        hi = REG_READ(COUNT_HI_REG);
        lo_start = REG_READ(COUNT_LO_REG);
    } while (lo_start != lo);

    timer_64b_reg_t result = {
        .lo = lo,
        .hi = hi
    };
    return result.val;
}

uint64_t IRAM_ATTR esp_timer_impl_get_time(void)
{
    return esp_timer_impl_get_counter_reg() / TICKS_PER_US;
}

void IRAM_ATTR esp_timer_impl_set_alarm(uint64_t timestamp)
{
    portENTER_CRITICAL_SAFE(&s_time_update_lock);
    int64_t offset = TICKS_PER_US * 2;
    uint64_t now_time = esp_timer_impl_get_counter_reg();
    timer_64b_reg_t alarm = { .val = MAX(timestamp * TICKS_PER_US, now_time + offset) };
    do {
        DISABLE_COMPARE_UNIT();
        REG_WRITE(ALARM_LO_REG, alarm.lo);
        REG_WRITE(ALARM_HI_REG, alarm.hi);
        ENABLE_COMPARE_UNIT();
        now_time = esp_timer_impl_get_counter_reg();
        int64_t delta = (int64_t)alarm.val - (int64_t)now_time;
        if (delta <= 0 && GET_INT_FLAG() == 0) {
            // new alarm is less than the counter and the interrupt flag is not set
            offset += abs((int)delta) + TICKS_PER_US * 2;
            alarm.val = now_time + offset;
        } else {
            // finish if either (alarm > counter) or the interrupt flag is already set.
            break;
        }
    } while(1);
    portEXIT_CRITICAL_SAFE(&s_time_update_lock);
}

static void IRAM_ATTR timer_alarm_isr(void *arg)
{
    // clear the interrupt
    CLEAR_INT();
    /* Call the upper layer handler */
    (*s_alarm_handler)(arg);
}

void IRAM_ATTR esp_timer_impl_update_apb_freq(uint32_t apb_ticks_per_us)
{
    /* If this function was called when switching APB clock to PLL, don't need
     * do anything: the SYSTIMER_TIMER_PLL_STEP is already correct.
     * If this was called when switching APB clock to XTAL, need to adjust
     * XTAL_STEP value accordingly.
     */
    if (apb_ticks_per_us != TICKS_PER_US) {
        assert((TICKS_PER_US % apb_ticks_per_us) == 0 && "TICK_PER_US should be divisible by APB frequency (in MHz)");
        SETTING_STEP_FOR_XTAL_SRC(TICKS_PER_US / apb_ticks_per_us);
    }
}

void esp_timer_impl_advance(int64_t time_us)
{
    portENTER_CRITICAL_SAFE(&s_time_update_lock);
    timer_64b_reg_t new_count = { .val = esp_timer_impl_get_counter_reg() + time_us * TICKS_PER_US };
    REG_WRITE(LOAD_LO_REG, new_count.lo);
    REG_WRITE(LOAD_HI_REG, new_count.hi);
    APPLY_LOADED_VAL();
    portEXIT_CRITICAL_SAFE(&s_time_update_lock);
}

esp_err_t esp_timer_impl_init(intr_handler_t alarm_handler)
{
    s_alarm_handler = alarm_handler;

    esp_err_t err = esp_intr_alloc(INTR_SOURCE_LACT,
            ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_EDGE,
            &timer_alarm_isr, NULL, &s_timer_interrupt_handle);

    if (err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "esp_intr_alloc failed (0x%0x)", err);
        return err;
    }
    ENABLE_CLK();
    /* Configure the counter:
     * - increment by 1 when running from PLL (80 ticks per microsecond),
     * - increment by 2 when running from XTAL (40 ticks per microsecond).
     * Note that if the APB frequency is derived from XTAL with divider != 1,
     * XTAL_STEP needs to be adjusted accordingly. For example, if
     * the APB frequency is XTAL/4 = 10 MHz, then XTAL_STEP should be set to 8.
     * This is handled in esp_timer_impl_update_apb_freq function above.
     */
    assert(rtc_clk_xtal_freq_get() == 40 && TICKS_PER_US == 80
        && "update the following code to support other XTAL:APB frequency ratios");
    SETTING_STEP_FOR_PLL_SRC(1);
    SETTING_STEP_FOR_XTAL_SRC(2);
    /* TODO: if SYSTIMER is used for anything else, access to SYSTIMER_INT_ENA_REG has to be
     * protected by a shared spinlock. Since this code runs as part of early startup, this
     * is practically not an issue. Same applies to SYSTIMER_CLK_EN above.
     */
    ENABLE_INT();
    ESP_ERROR_CHECK(esp_intr_enable(s_timer_interrupt_handle));
    return ESP_OK;
}

void esp_timer_impl_deinit(void)
{
    esp_intr_disable(s_timer_interrupt_handle);
    DISABLE_COMPARE_UNIT();
    /* TODO: may need a spinlock, see the note related to SYSTIMER_INT_ENA_REG in esp_timer_impl_init */
    DISABLE_INT();
    esp_intr_free(s_timer_interrupt_handle);
    s_timer_interrupt_handle = NULL;
}

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
void esp_timer_private_advance(int64_t time_us) __attribute__((alias("esp_timer_impl_advance")));
void esp_timer_private_lock(void) __attribute__((alias("esp_timer_impl_lock")));
void esp_timer_private_unlock(void) __attribute__((alias("esp_timer_impl_unlock")));

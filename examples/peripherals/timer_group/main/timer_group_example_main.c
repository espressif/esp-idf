/* Timer group-hardware timer example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

#define TIMER_INTR_SEL TIMER_INTR_LEVEL  /*!< Timer level interrupt */
#define TIMER_GROUP    TIMER_GROUP_0     /*!< Test on timer group 0 */
#define TIMER_DIVIDER   16               /*!< Hardware timer clock divider */
#define TIMER_SCALE    (TIMER_BASE_CLK / TIMER_DIVIDER)  /*!< used to calculate counter value */
#define TIMER_FINE_ADJ   (1.4*(TIMER_BASE_CLK / TIMER_DIVIDER)/1000000) /*!< used to compensate alarm value */
#define TIMER_INTERVAL0_SEC   (3.4179)   /*!< test interval for timer 0 */
#define TIMER_INTERVAL1_SEC   (5.78)   /*!< test interval for timer 1 */
#define TEST_WITHOUT_RELOAD   0   /*!< example of auto-reload mode */
#define TEST_WITH_RELOAD   1      /*!< example without auto-reload mode */

typedef struct {
    int type;                  /*!< event type */
    int group;                 /*!< timer group */
    int idx;                   /*!< timer number */
    uint64_t counter_val;      /*!< timer counter value */
} timer_event_t;

xQueueHandle timer_queue;

/*
 * @brief Print a uint64_t value
 */
static void inline print_u64(uint64_t val)
{
    printf("0x%08x%08x\n", (uint32_t) (val >> 32), (uint32_t) (val));
}

static void timer_example_evt_task(void *arg)
{
    while(1) {
        timer_event_t evt;
        xQueueReceive(timer_queue, &evt, portMAX_DELAY);
        if(evt.type == TEST_WITHOUT_RELOAD) {
            printf("\n\n   example of count-up-timer \n");
        } else if(evt.type == TEST_WITH_RELOAD) {
            printf("\n\n   example of reload-timer \n");

        }
        /*Show timer event from interrupt*/
        printf("-------INTR TIME EVT--------\n");
        printf("TG[%d] timer[%d] alarm evt\n", evt.group, evt.idx);
        printf("reg: ");
        print_u64(evt.counter_val);

        double time = (double) evt.counter_val / (TIMER_BASE_CLK / TIMERG0.hw_timer[evt.idx].config.divider);
        printf("time: %.8f S\n", time);
        /*Read timer value from task*/
        printf("======TASK TIME======\n");
        uint64_t timer_val;
        timer_get_counter_value(evt.group, evt.idx, &timer_val);
        timer_get_counter_time_sec(evt.group, evt.idx, &time);
        printf("TG[%d] timer[%d] alarm evt\n", evt.group, evt.idx);
        printf("reg: ");
        print_u64(timer_val);
        printf("time: %.8f S\n", time);
    }
}

/*
 * @brief timer group0 ISR handler
 */
void IRAM_ATTR timer_group0_isr(void *para)
{
    int timer_idx = (int) para;
    uint32_t intr_status = TIMERG0.int_st_timers.val;
    timer_event_t evt;
    if((intr_status & BIT(timer_idx)) && timer_idx == TIMER_0) {
        /*Timer0 is an example that doesn't reload counter value*/
        TIMERG0.hw_timer[timer_idx].update = 1;

        /* We don't call a API here because they are not declared with IRAM_ATTR.
           If we're okay with the timer irq not being serviced while SPI flash cache is disabled,
           we can alloc this interrupt without the ESP_INTR_FLAG_IRAM flag and use the normal API. */
        TIMERG0.int_clr_timers.t0 = 1;
        uint64_t timer_val = ((uint64_t) TIMERG0.hw_timer[timer_idx].cnt_high) << 32
            | TIMERG0.hw_timer[timer_idx].cnt_low;

        /*Post an event to out example task*/
        evt.type = TEST_WITHOUT_RELOAD;
        evt.group = 0;
        evt.idx = timer_idx;
        evt.counter_val = timer_val;
        xQueueSendFromISR(timer_queue, &evt, NULL);

        /*For a timer that will not reload, we need to set the next alarm value each time. */
        timer_val +=
            (uint64_t) (TIMER_INTERVAL0_SEC * (TIMER_BASE_CLK / TIMERG0.hw_timer[timer_idx].config.divider));
        /*Fine adjust*/
        timer_val -= TIMER_FINE_ADJ;
        TIMERG0.hw_timer[timer_idx].alarm_high = (uint32_t) (timer_val >> 32);
        TIMERG0.hw_timer[timer_idx].alarm_low = (uint32_t) timer_val;
        /*After set alarm, we set alarm_en bit if we want to enable alarm again.*/
        TIMERG0.hw_timer[timer_idx].config.alarm_en = 1;

    } else if((intr_status & BIT(timer_idx)) && timer_idx == TIMER_1) {
        /*Timer1 is an example that will reload counter value*/
        TIMERG0.hw_timer[timer_idx].update = 1;
        /*We don't call a API here because they are not declared with IRAM_ATTR*/
        TIMERG0.int_clr_timers.t1 = 1;
        uint64_t timer_val = ((uint64_t) TIMERG0.hw_timer[timer_idx].cnt_high) << 32
            | TIMERG0.hw_timer[timer_idx].cnt_low;
        /*Post an event to out example task*/
        evt.type = TEST_WITH_RELOAD;
        evt.group = 0;
        evt.idx = timer_idx;
        evt.counter_val = timer_val;
        xQueueSendFromISR(timer_queue, &evt, NULL);
        /*For a auto-reload timer, we still need to set alarm_en bit if we want to enable alarm again.*/
        TIMERG0.hw_timer[timer_idx].config.alarm_en = 1;
    }
}

/*
 * @brief timer group0 hardware timer0 init
 */
static void example_tg0_timer0_init()
{
    int timer_group = TIMER_GROUP_0;
    int timer_idx = TIMER_0;
    timer_config_t config;
    config.alarm_en = 1;
    config.auto_reload = 0;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = TIMER_DIVIDER;
    config.intr_type = TIMER_INTR_SEL;
    config.counter_en = TIMER_PAUSE;
    /*Configure timer*/
    timer_init(timer_group, timer_idx, &config);
    /*Stop timer counter*/
    timer_pause(timer_group, timer_idx);
    /*Load counter value */
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
    /*Set alarm value*/
    timer_set_alarm_value(timer_group, timer_idx, TIMER_INTERVAL0_SEC * TIMER_SCALE - TIMER_FINE_ADJ);
    /*Enable timer interrupt*/
    timer_enable_intr(timer_group, timer_idx);
    /*Set ISR handler*/
    timer_isr_register(timer_group, timer_idx, timer_group0_isr, (void*) timer_idx, ESP_INTR_FLAG_IRAM, NULL);
    /*Start timer counter*/
    timer_start(timer_group, timer_idx);
}

/*
 * @brief timer group0 hardware timer1 init
 */
static void example_tg0_timer1_init()
{
    int timer_group = TIMER_GROUP_0;
    int timer_idx = TIMER_1;
    timer_config_t config;
    config.alarm_en = 1;
    config.auto_reload = 1;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = TIMER_DIVIDER;
    config.intr_type = TIMER_INTR_SEL;
    config.counter_en = TIMER_PAUSE;
    /*Configure timer*/
    timer_init(timer_group, timer_idx, &config);
    /*Stop timer counter*/
    timer_pause(timer_group, timer_idx);
    /*Load counter value */
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
    /*Set alarm value*/
    timer_set_alarm_value(timer_group, timer_idx, TIMER_INTERVAL1_SEC * TIMER_SCALE);
    /*Enable timer interrupt*/
    timer_enable_intr(timer_group, timer_idx);
    /*Set ISR handler*/
    timer_isr_register(timer_group, timer_idx, timer_group0_isr, (void*) timer_idx, ESP_INTR_FLAG_IRAM, NULL);
    /*Start timer counter*/
    timer_start(timer_group, timer_idx);
}

/**
 * @brief In this test, we will test hardware timer0 and timer1 of timer group0.
 */
void app_main()
{
    timer_queue = xQueueCreate(10, sizeof(timer_event_t));
    example_tg0_timer0_init();
    example_tg0_timer1_init();
    xTaskCreate(timer_example_evt_task, "timer_evt_task", 2048, NULL, 5, NULL);
}


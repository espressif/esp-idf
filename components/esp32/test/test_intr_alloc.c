/*
 Tests for the interrupt allocator.
*/

#include <esp_types.h>
#include <stdio.h>
#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/uart_reg.h"
#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "esp_intr_alloc.h"
#include "driver/timer.h"


#define TIMER_DIVIDER   16               /*!< Hardware timer clock divider */
#define TIMER_SCALE    (TIMER_BASE_CLK / TIMER_DIVIDER)  /*!< used to calculate counter value */
#define TIMER_INTERVAL0_SEC   (3.4179)   /*!< test interval for timer 0 */
#define TIMER_INTERVAL1_SEC   (5.78)   /*!< test interval for timer 1 */


static void my_timer_init(int timer_group, int timer_idx, int ival)
{
    timer_config_t config;
    config.alarm_en = 1;
    config.auto_reload = 1;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = TIMER_DIVIDER;
    config.intr_type = TIMER_INTR_LEVEL;
    config.counter_en = TIMER_PAUSE;
    /*Configure timer*/
    timer_init(timer_group, timer_idx, &config);
    /*Stop timer counter*/
    timer_pause(timer_group, timer_idx);
    /*Load counter value */
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
    /*Set alarm value*/
    timer_set_alarm_value(timer_group, timer_idx, ival);
    /*Enable timer interrupt*/
    timer_enable_intr(timer_group, timer_idx);
}

static volatile int count[4]={0,0,0,0};


static void timer_isr(void *arg)
{
    int timer_idx = (int)arg;
    count[timer_idx]++;
    if (timer_idx==0) {
        TIMERG0.int_clr_timers.t0 = 1;
        TIMERG0.hw_timer[0].update=1;
        TIMERG0.hw_timer[0].config.alarm_en = 1;
    }
    if (timer_idx==1) {
        TIMERG0.int_clr_timers.t1 = 1;
        TIMERG0.hw_timer[1].update=1;
        TIMERG0.hw_timer[1].config.alarm_en = 1;
    }
    if (timer_idx==2) {
        TIMERG1.int_clr_timers.t0 = 1;
        TIMERG1.hw_timer[0].update=1;
        TIMERG1.hw_timer[0].config.alarm_en = 1;
    }
    if (timer_idx==3) {
        TIMERG1.int_clr_timers.t1 = 1;
        TIMERG1.hw_timer[1].update=1;
        TIMERG1.hw_timer[1].config.alarm_en = 1;
    }
//  ets_printf("int %d\n", timer_idx);
}


static void timer_test(int flags) {
    int x;
    timer_isr_handle_t inth[4];
    my_timer_init(TIMER_GROUP_0, TIMER_0, 110000);
    my_timer_init(TIMER_GROUP_0, TIMER_1, 120000);
    my_timer_init(TIMER_GROUP_1, TIMER_0, 130000);
    my_timer_init(TIMER_GROUP_1, TIMER_1, 140000);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_isr, (void*)0, flags|ESP_INTR_FLAG_INTRDISABLED, &inth[0]);
    timer_isr_register(TIMER_GROUP_0, TIMER_1, timer_isr, (void*)1, flags, &inth[1]);
    timer_isr_register(TIMER_GROUP_1, TIMER_0, timer_isr, (void*)2, flags, &inth[2]);
    timer_isr_register(TIMER_GROUP_1, TIMER_1, timer_isr, (void*)3, flags, &inth[3]);
    timer_start(TIMER_GROUP_0, TIMER_0);
    timer_start(TIMER_GROUP_0, TIMER_1);
    timer_start(TIMER_GROUP_1, TIMER_0);
    timer_start(TIMER_GROUP_1, TIMER_1);

    for (x=0; x<4; x++) count[x]=0;
    printf("Interrupts allocated: %d (dis) %d %d %d\n",
            esp_intr_get_intno(inth[0]), esp_intr_get_intno(inth[1]),
            esp_intr_get_intno(inth[2]), esp_intr_get_intno(inth[3]));
    printf("Timer values on start: %d %d %d %d\n", count[0], count[1], count[2], count[3]);
    vTaskDelay(1000 / portTICK_RATE_MS);
    printf("Timer values after 1 sec: %d %d %d %d\n", count[0], count[1], count[2], count[3]);
    TEST_ASSERT(count[0]==0);
    TEST_ASSERT(count[1]!=0);
    TEST_ASSERT(count[2]!=0);
    TEST_ASSERT(count[3]!=0);

    printf("Disabling timers 1 and 2...\n");
    esp_intr_enable(inth[0]);
    esp_intr_disable(inth[1]);
    esp_intr_disable(inth[2]);
    for (x=0; x<4; x++) count[x]=0;
    vTaskDelay(1000 / portTICK_RATE_MS);
    printf("Timer values after 1 sec: %d %d %d %d\n", count[0], count[1], count[2], count[3]);
    TEST_ASSERT(count[0]!=0);
    TEST_ASSERT(count[1]==0);
    TEST_ASSERT(count[2]==0);
    TEST_ASSERT(count[3]!=0);
    printf("Disabling other half...\n");
    esp_intr_enable(inth[1]);
    esp_intr_enable(inth[2]);
    esp_intr_disable(inth[0]);
    esp_intr_disable(inth[3]);
    for (x=0; x<4; x++) count[x]=0;
    vTaskDelay(1000 / portTICK_RATE_MS);
    printf("Timer values after 1 sec: %d %d %d %d\n", count[0], count[1], count[2], count[3]);
    TEST_ASSERT(count[0]==0);
    TEST_ASSERT(count[1]!=0);
    TEST_ASSERT(count[2]!=0);
    TEST_ASSERT(count[3]==0);
    printf("Done.\n");
    esp_intr_free(inth[0]);
    esp_intr_free(inth[1]);
    esp_intr_free(inth[2]);
    esp_intr_free(inth[3]);
}

static volatile int int_timer_ctr;


void int_timer_handler(void *arg) {
    xthal_set_ccompare(1, xthal_get_ccount()+8000000);
    int_timer_ctr++;
}

void local_timer_test()
{
    intr_handle_t ih;
    esp_err_t r;
    r=esp_intr_alloc(ETS_INTERNAL_TIMER1_INTR_SOURCE, 0, int_timer_handler, NULL, &ih);
    TEST_ASSERT(r==ESP_OK);
    printf("Int timer 1 intno %d\n", esp_intr_get_intno(ih));
    xthal_set_ccompare(1, xthal_get_ccount()+8000000);
    int_timer_ctr=0;
    vTaskDelay(1000 / portTICK_RATE_MS);
    printf("Timer val after 1 sec: %d\n", int_timer_ctr);
    TEST_ASSERT(int_timer_ctr!=0);
    printf("Disabling int\n");
    esp_intr_disable(ih);
    int_timer_ctr=0;
    vTaskDelay(1000 / portTICK_RATE_MS);
    printf("Timer val after 1 sec: %d\n", int_timer_ctr);
    TEST_ASSERT(int_timer_ctr==0);
    printf("Re-enabling\n");
    esp_intr_enable(ih);
    vTaskDelay(1000 / portTICK_RATE_MS);
    printf("Timer val after 1 sec: %d\n", int_timer_ctr);
    TEST_ASSERT(int_timer_ctr!=0);

    printf("Free int, re-alloc disabled\n");
    r=esp_intr_free(ih);
    TEST_ASSERT(r==ESP_OK);
    r=esp_intr_alloc(ETS_INTERNAL_TIMER1_INTR_SOURCE, ESP_INTR_FLAG_INTRDISABLED, int_timer_handler, NULL, &ih);
    TEST_ASSERT(r==ESP_OK);
    int_timer_ctr=0;
    vTaskDelay(1000 / portTICK_RATE_MS);
    printf("Timer val after 1 sec: %d\n", int_timer_ctr);
    TEST_ASSERT(int_timer_ctr==0);
    printf("Re-enabling\n");
    esp_intr_enable(ih);
    vTaskDelay(1000 / portTICK_RATE_MS);
    printf("Timer val after 1 sec: %d\n", int_timer_ctr);
    TEST_ASSERT(int_timer_ctr!=0);
    r=esp_intr_free(ih);
    TEST_ASSERT(r==ESP_OK);
    printf("Done.\n");
}


TEST_CASE("Intr_alloc test, CPU-local int source", "[esp32]")
{
    local_timer_test();
}

TEST_CASE("Intr_alloc test, private ints", "[esp32]")
{
    timer_test(0);
}

TEST_CASE("Intr_alloc test, shared ints", "[esp32]")
{
    timer_test(ESP_INTR_FLAG_SHARED);
}

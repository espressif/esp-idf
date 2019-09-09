/*
 Tests for the interrupt allocator.
*/

#include <esp_types.h>
#include <stdio.h>
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/rom/ets_sys.h"
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "soc/uart_periph.h"
#include "soc/dport_reg.h"
#include "soc/gpio_periph.h"
#include "esp_intr_alloc.h"
#include "driver/periph_ctrl.h"
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
        timer_group_intr_clr_in_isr(TIMER_GROUP_0, TIMER_0);
        timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
    }
    if (timer_idx==1) {
        timer_group_intr_clr_in_isr(TIMER_GROUP_0, TIMER_1);
        timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_1);
    }
    if (timer_idx==2) {
        timer_group_intr_clr_in_isr(TIMER_GROUP_1, TIMER_0);
        timer_group_enable_alarm_in_isr(TIMER_GROUP_1, TIMER_0);
    }
    if (timer_idx==3) {
        timer_group_intr_clr_in_isr(TIMER_GROUP_1, TIMER_1);
        timer_group_enable_alarm_in_isr(TIMER_GROUP_1, TIMER_1);
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
    vTaskDelay(1000 / portTICK_PERIOD_MS);
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
    vTaskDelay(1000 / portTICK_PERIOD_MS);
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
    vTaskDelay(1000 / portTICK_PERIOD_MS);
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

void local_timer_test(void)
{
    intr_handle_t ih;
    esp_err_t r;
    r=esp_intr_alloc(ETS_INTERNAL_TIMER1_INTR_SOURCE, 0, int_timer_handler, NULL, &ih);
    TEST_ASSERT(r==ESP_OK);
    printf("Int timer 1 intno %d\n", esp_intr_get_intno(ih));
    xthal_set_ccompare(1, xthal_get_ccount()+8000000);
    int_timer_ctr=0;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Timer val after 1 sec: %d\n", int_timer_ctr);
    TEST_ASSERT(int_timer_ctr!=0);
    printf("Disabling int\n");
    esp_intr_disable(ih);
    int_timer_ctr=0;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Timer val after 1 sec: %d\n", int_timer_ctr);
    TEST_ASSERT(int_timer_ctr==0);
    printf("Re-enabling\n");
    esp_intr_enable(ih);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Timer val after 1 sec: %d\n", int_timer_ctr);
    TEST_ASSERT(int_timer_ctr!=0);

    printf("Free int, re-alloc disabled\n");
    r=esp_intr_free(ih);
    TEST_ASSERT(r==ESP_OK);
    r=esp_intr_alloc(ETS_INTERNAL_TIMER1_INTR_SOURCE, ESP_INTR_FLAG_INTRDISABLED, int_timer_handler, NULL, &ih);
    TEST_ASSERT(r==ESP_OK);
    int_timer_ctr=0;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Timer val after 1 sec: %d\n", int_timer_ctr);
    TEST_ASSERT(int_timer_ctr==0);
    printf("Re-enabling\n");
    esp_intr_enable(ih);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
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

TEST_CASE("Can allocate IRAM int only with an IRAM handler", "[esp32]")
{
    void dummy(void* arg)
    {
    }
    IRAM_ATTR void dummy_iram(void* arg)
    {
    }
    RTC_IRAM_ATTR void dummy_rtc(void* arg)
    {
    }
    intr_handle_t ih;
    esp_err_t err = esp_intr_alloc(ETS_INTERNAL_PROFILING_INTR_SOURCE,
            ESP_INTR_FLAG_IRAM, &dummy, NULL, &ih);
    TEST_ASSERT_EQUAL_INT(ESP_ERR_INVALID_ARG, err);
    err = esp_intr_alloc(ETS_INTERNAL_PROFILING_INTR_SOURCE,
            ESP_INTR_FLAG_IRAM, &dummy_iram, NULL, &ih);
    TEST_ESP_OK(err);
    err = esp_intr_free(ih);
    TEST_ESP_OK(err);
    err = esp_intr_alloc(ETS_INTERNAL_PROFILING_INTR_SOURCE,
            ESP_INTR_FLAG_IRAM, &dummy_rtc, NULL, &ih);
    TEST_ESP_OK(err);
    err = esp_intr_free(ih);
    TEST_ESP_OK(err);
}


#include "soc/spi_periph.h"
typedef struct {
    bool flag1;
    bool flag2;
    bool flag3;
    bool flag4;
} intr_alloc_test_ctx_t;

void IRAM_ATTR int_handler1(void* arg)
{
    intr_alloc_test_ctx_t* ctx=(intr_alloc_test_ctx_t*)arg;
    ets_printf("handler 1 called.\n");
    if ( ctx->flag1 ) {
        ctx->flag3 = true;
    } else {
        ctx->flag1 = true;
    }
    SPI2.slave.trans_done = 0;
}

void IRAM_ATTR int_handler2(void* arg)
{
    intr_alloc_test_ctx_t* ctx = (intr_alloc_test_ctx_t*)arg;
    ets_printf("handler 2 called.\n");
    if ( ctx->flag2 ) {
        ctx->flag4 = true;
    } else {
        ctx->flag2 = true;
    }
}

TEST_CASE("allocate 2 handlers for a same source and remove the later one","[esp32]")
{
    intr_alloc_test_ctx_t ctx = {false, false, false, false };
    intr_handle_t handle1, handle2;

    //enable HSPI(spi2)
    periph_module_enable(PERIPH_HSPI_MODULE);

    esp_err_t r;
    r=esp_intr_alloc(ETS_SPI2_INTR_SOURCE, ESP_INTR_FLAG_SHARED, int_handler1, &ctx, &handle1);
    TEST_ESP_OK(r);
    //try an invalid assign first
    r=esp_intr_alloc(ETS_SPI2_INTR_SOURCE, 0, int_handler2, NULL, &handle2);
    TEST_ASSERT_EQUAL_INT(r, ESP_ERR_NOT_FOUND );
    //assign shared then
    r=esp_intr_alloc(ETS_SPI2_INTR_SOURCE, ESP_INTR_FLAG_SHARED, int_handler2, &ctx, &handle2);
    TEST_ESP_OK(r);
    SPI2.slave.trans_inten = 1;

    printf("trigger first time.\n");
    SPI2.slave.trans_done = 1;

    vTaskDelay(100);
    TEST_ASSERT( ctx.flag1 && ctx.flag2 );

    printf("remove intr 1.\n");
    r=esp_intr_free(handle2);

    printf("trigger second time.\n");
    SPI2.slave.trans_done = 1;

    vTaskDelay(500);
    TEST_ASSERT( ctx.flag3 && !ctx.flag4 );
    printf("test passed.\n");
}

#ifndef CONFIG_FREERTOS_UNICORE

void isr_free_task(void *param)
{
    esp_err_t ret = ESP_FAIL;
    intr_handle_t *test_handle = (intr_handle_t *)param;
    if(*test_handle != NULL) {
        ret = esp_intr_free(*test_handle);
        if(ret == ESP_OK) {
            *test_handle = NULL;
        }
    }
    vTaskDelete(NULL);
}

void isr_alloc_free_test(void)
{
    intr_handle_t test_handle = NULL;
    esp_err_t ret = esp_intr_alloc(ETS_SPI2_INTR_SOURCE, 0, int_handler1, NULL, &test_handle);
    if(ret != ESP_OK) {
        printf("alloc isr handle fail\n");
    } else {
        printf("alloc isr handle on core %d\n",esp_intr_get_cpu(test_handle));
    }
    TEST_ASSERT(ret == ESP_OK);
    xTaskCreatePinnedToCore(isr_free_task, "isr_free_task", 1024*2, (void *)&test_handle, 10, NULL, !xPortGetCoreID());
    vTaskDelay(1000/portTICK_RATE_MS);
    TEST_ASSERT(test_handle == NULL);
    printf("test passed\n");
}

TEST_CASE("alloc and free isr handle on different core", "[esp32]")
{
    isr_alloc_free_test();
}

#endif

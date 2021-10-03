/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 Tests for the interrupt allocator.
*/

#include <stdio.h>
#include "esp_types.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "unity.h"
#include "esp_intr_alloc.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "soc/soc_caps.h"
#include "soc/spi_periph.h"
#include "hal/spi_ll.h"
#include "sdkconfig.h"

#define TIMER_DIVIDER (16)          /*!< Hardware timer clock divider */
#define TIMER_SCALE   (APB_CLK_FREQ / TIMER_DIVIDER)  /*!< used to calculate counter value */
#define TIMER_INTERVAL0_SEC   (3)   /*!< test interval for timer 0 */
#define TIMER_INTERVAL1_SEC   (5)   /*!< test interval for timer 1 */

static void my_timer_init(int timer_group, int timer_idx, uint64_t alarm_value)
{
    timer_config_t config = {
        .alarm_en = 1,
        .auto_reload = 1,
        .counter_dir = TIMER_COUNT_UP,
        .divider = TIMER_DIVIDER,
    };
    /*Configure timer*/
    timer_init(timer_group, timer_idx, &config);
    /*Stop timer counter*/
    timer_pause(timer_group, timer_idx);
    /*Load counter value */
    timer_set_counter_value(timer_group, timer_idx, 0);
    /*Set alarm value*/
    timer_set_alarm_value(timer_group, timer_idx, alarm_value);
    /*Enable timer interrupt*/
    timer_enable_intr(timer_group, timer_idx);
}

static volatile int count[SOC_TIMER_GROUP_TOTAL_TIMERS] = {0};

static void timer_isr(void *arg)
{
    int timer_idx = (int)arg;
    int group_id = timer_idx / SOC_TIMER_GROUP_TIMERS_PER_GROUP;
    int timer_id = timer_idx % SOC_TIMER_GROUP_TIMERS_PER_GROUP;
    count[timer_idx]++;

    timer_group_clr_intr_status_in_isr(group_id, timer_id);
    timer_group_enable_alarm_in_isr(group_id, timer_id);
}

static void timer_test(int flags)
{
    timer_isr_handle_t inth[SOC_TIMER_GROUP_TOTAL_TIMERS];
    for (int i = 0; i < SOC_TIMER_GROUPS; i++) {
        for (int j = 0; j < SOC_TIMER_GROUP_TIMERS_PER_GROUP; j++) {
            my_timer_init(i, j, 100000 + 10000 * (i * SOC_TIMER_GROUP_TIMERS_PER_GROUP + j + 1));
        }
    }
    timer_isr_register(0, 0, timer_isr, (void *)0, flags | ESP_INTR_FLAG_INTRDISABLED, &inth[0]);
    printf("Interrupts allocated: %d (dis)\r\n", esp_intr_get_intno(inth[0]));

    for (int j = 1; j < SOC_TIMER_GROUP_TIMERS_PER_GROUP; j++) {
        timer_isr_register(0, j, timer_isr, (void *)1, flags, &inth[j]);
        printf("Interrupts allocated: %d\r\n", esp_intr_get_intno(inth[j]));
    }
    for (int i = 1; i < SOC_TIMER_GROUPS; i++) {
        for (int j = 0; j < SOC_TIMER_GROUP_TIMERS_PER_GROUP; j++) {
            timer_isr_register(i, j, timer_isr, (void *)(i * SOC_TIMER_GROUP_TIMERS_PER_GROUP + j), flags, &inth[i * SOC_TIMER_GROUP_TIMERS_PER_GROUP + j]);
            printf("Interrupts allocated: %d\r\n", esp_intr_get_intno(inth[i * SOC_TIMER_GROUP_TIMERS_PER_GROUP + j]));
        }
    }
    for (int i = 0; i < SOC_TIMER_GROUPS; i++) {
        for (int j = 0; j < SOC_TIMER_GROUP_TIMERS_PER_GROUP; j++) {
            timer_start(i, j);
        }
    }

    printf("Timer values on start:");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        count[i] = 0;
        printf(" %d", count[i]);
    }
    printf("\r\n");

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Timer values after 1 sec:");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        printf(" %d", count[i]);
    }
    printf("\r\n");

    TEST_ASSERT(count[0] == 0);
    for (int i = 1; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ASSERT(count[i] != 0);
    }

    printf("Disabling half of timers' interrupt...\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS / 2; i++) {
        esp_intr_disable(inth[i]);
    }
    for (int i = SOC_TIMER_GROUP_TOTAL_TIMERS / 2; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        esp_intr_enable(inth[i]);
    }
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        count[i] = 0;
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Timer values after 1 sec:");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        printf(" %d", count[i]);
    }
    printf("\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS / 2; i++) {
        TEST_ASSERT(count[i] == 0);
    }
    for (int i = SOC_TIMER_GROUP_TOTAL_TIMERS / 2; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ASSERT(count[i] != 0);
    }

    printf("Disabling another half...\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS / 2; i++) {
        esp_intr_enable(inth[i]);
    }
    for (int i = SOC_TIMER_GROUP_TOTAL_TIMERS / 2; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        esp_intr_disable(inth[i]);
    }
    for (int x = 0; x < SOC_TIMER_GROUP_TOTAL_TIMERS; x++) {
        count[x] = 0;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Timer values after 1 sec:");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        printf(" %d", count[i]);
    }
    printf("\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS / 2; i++) {
        TEST_ASSERT(count[i] != 0);
    }
    for (int i = SOC_TIMER_GROUP_TOTAL_TIMERS / 2; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ASSERT(count[i] == 0);
    }
    printf("Done.\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        esp_intr_free(inth[i]);
    }
}

TEST_CASE("Intr_alloc test, private ints", "[intr_alloc]")
{
    timer_test(0);
}

TEST_CASE("Intr_alloc test, shared ints", "[intr_alloc]")
{
    timer_test(ESP_INTR_FLAG_SHARED);
}

typedef struct {
    bool flag1;
    bool flag2;
    bool flag3;
    bool flag4;
} intr_alloc_test_ctx_t;

void IRAM_ATTR int_handler1(void *arg)
{
    intr_alloc_test_ctx_t *ctx = (intr_alloc_test_ctx_t *)arg;
    esp_rom_printf("handler 1 called.\n");
    if ( ctx->flag1 ) {
        ctx->flag3 = true;
    } else {
        ctx->flag1 = true;
    }

#ifdef CONFIG_IDF_TARGET_ESP32
    spi_ll_clear_int_stat(&SPI2);
#else
    spi_ll_clear_int_stat(&GPSPI2);
#endif
}

void IRAM_ATTR int_handler2(void *arg)
{
    intr_alloc_test_ctx_t *ctx = (intr_alloc_test_ctx_t *)arg;
    esp_rom_printf("handler 2 called.\n");
    if ( ctx->flag2 ) {
        ctx->flag4 = true;
    } else {
        ctx->flag2 = true;
    }
}

TEST_CASE("allocate 2 handlers for a same source and remove the later one", "[intr_alloc]")
{
    intr_alloc_test_ctx_t ctx = {false, false, false, false };
    intr_handle_t handle1, handle2;

    // enable SPI2
    periph_module_enable(spi_periph_signal[1].module);

    esp_err_t r;
    r = esp_intr_alloc(spi_periph_signal[1].irq, ESP_INTR_FLAG_SHARED, int_handler1, &ctx, &handle1);
    TEST_ESP_OK(r);
    //try an invalid assign first
    r = esp_intr_alloc(spi_periph_signal[1].irq, 0, int_handler2, NULL, &handle2);
    TEST_ASSERT_EQUAL_INT(ESP_ERR_NOT_FOUND, r);
    //assign shared then
    r = esp_intr_alloc(spi_periph_signal[1].irq, ESP_INTR_FLAG_SHARED, int_handler2, &ctx, &handle2);
    TEST_ESP_OK(r);

#ifdef CONFIG_IDF_TARGET_ESP32
    spi_ll_enable_int(&SPI2);
#else
    spi_ll_enable_int(&GPSPI2);
#endif

    printf("trigger first time.\n");

#ifdef CONFIG_IDF_TARGET_ESP32
    spi_ll_set_int_stat(&SPI2);
#else
    spi_ll_set_int_stat(&GPSPI2);
#endif

    vTaskDelay(100);
    TEST_ASSERT( ctx.flag1 && ctx.flag2 );

    printf("remove intr 1.\n");
    r = esp_intr_free(handle2);

    printf("trigger second time.\n");

#ifdef CONFIG_IDF_TARGET_ESP32
    spi_ll_set_int_stat(&SPI2);
#else
    spi_ll_set_int_stat(&GPSPI2);
#endif

    vTaskDelay(500);
    TEST_ASSERT( ctx.flag3 && !ctx.flag4 );
    printf("test passed.\n");
    esp_intr_free(handle1);
}

static void dummy(void *arg)
{
}
static IRAM_ATTR void dummy_iram(void *arg)
{
}
static RTC_IRAM_ATTR void dummy_rtc(void *arg)
{
}

TEST_CASE("Can allocate IRAM int only with an IRAM handler", "[intr_alloc]")
{
    intr_handle_t ih;
    esp_err_t err = esp_intr_alloc(spi_periph_signal[1].irq,
                                   ESP_INTR_FLAG_IRAM, &dummy, NULL, &ih);
    TEST_ASSERT_EQUAL_INT(ESP_ERR_INVALID_ARG, err);
    err = esp_intr_alloc(spi_periph_signal[1].irq,
                         ESP_INTR_FLAG_IRAM, &dummy_iram, NULL, &ih);
    TEST_ESP_OK(err);
    err = esp_intr_free(ih);
    TEST_ESP_OK(err);
    err = esp_intr_alloc(spi_periph_signal[1].irq,
                         ESP_INTR_FLAG_IRAM, &dummy_rtc, NULL, &ih);
    TEST_ESP_OK(err);
    err = esp_intr_free(ih);
    TEST_ESP_OK(err);
}

#ifndef CONFIG_FREERTOS_UNICORE
void isr_free_task(void *param)
{
    esp_err_t ret = ESP_FAIL;
    intr_handle_t *test_handle = (intr_handle_t *)param;
    if (*test_handle != NULL) {
        ret = esp_intr_free(*test_handle);
        if (ret == ESP_OK) {
            *test_handle = NULL;
        }
    }
    vTaskDelete(NULL);
}

void isr_alloc_free_test(void)
{
    intr_handle_t test_handle = NULL;
    esp_err_t ret = esp_intr_alloc(spi_periph_signal[1].irq, 0, int_handler1, NULL, &test_handle);
    if (ret != ESP_OK) {
        printf("alloc isr handle fail\n");
    } else {
        printf("alloc isr handle on core %d\n", esp_intr_get_cpu(test_handle));
    }
    TEST_ASSERT(ret == ESP_OK);
    xTaskCreatePinnedToCore(isr_free_task, "isr_free_task", 1024 * 2, (void *)&test_handle, 10, NULL, !xPortGetCoreID());
    vTaskDelay(1000 / portTICK_RATE_MS);
    TEST_ASSERT(test_handle == NULL);
    printf("test passed\n");
}

TEST_CASE("alloc and free isr handle on different core", "[intr_alloc]")
{
    isr_alloc_free_test();
}
#endif

#if __XTENSA__
static volatile int int_timer_ctr;

void int_timer_handler(void *arg)
{
    xthal_set_ccompare(1, xthal_get_ccount() + 8000000);
    int_timer_ctr++;
}

static void local_timer_test(void)
{
    intr_handle_t ih;
    esp_err_t r;
    r = esp_intr_alloc(ETS_INTERNAL_TIMER1_INTR_SOURCE, 0, int_timer_handler, NULL, &ih);
    TEST_ASSERT(r == ESP_OK);
    printf("Int timer 1 intno %d\n", esp_intr_get_intno(ih));
    xthal_set_ccompare(1, xthal_get_ccount() + 8000000);
    int_timer_ctr = 0;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Timer val after 1 sec: %d\n", int_timer_ctr);
    TEST_ASSERT(int_timer_ctr != 0);
    printf("Disabling int\n");
    esp_intr_disable(ih);
    int_timer_ctr = 0;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Timer val after 1 sec: %d\n", int_timer_ctr);
    TEST_ASSERT(int_timer_ctr == 0);
    printf("Re-enabling\n");
    esp_intr_enable(ih);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Timer val after 1 sec: %d\n", int_timer_ctr);
    TEST_ASSERT(int_timer_ctr != 0);

    printf("Free int, re-alloc disabled\n");
    r = esp_intr_free(ih);
    TEST_ASSERT(r == ESP_OK);
    r = esp_intr_alloc(ETS_INTERNAL_TIMER1_INTR_SOURCE, ESP_INTR_FLAG_INTRDISABLED, int_timer_handler, NULL, &ih);
    TEST_ASSERT(r == ESP_OK);
    int_timer_ctr = 0;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Timer val after 1 sec: %d\n", int_timer_ctr);
    TEST_ASSERT(int_timer_ctr == 0);
    printf("Re-enabling\n");
    esp_intr_enable(ih);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Timer val after 1 sec: %d\n", int_timer_ctr);
    TEST_ASSERT(int_timer_ctr != 0);
    r = esp_intr_free(ih);
    TEST_ASSERT(r == ESP_OK);
    printf("Done.\n");
}


TEST_CASE("Intr_alloc test, CPU-local int source", "[intr_alloc]")
{
    local_timer_test();
}

#endif // #if __XTENSA__

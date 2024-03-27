/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 Tests for the interrupt allocator.
*/

#include <stdio.h>
#include "sdkconfig.h"
#include "esp_types.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "unity.h"
#include "esp_intr_alloc.h"
#include "driver/gptimer.h"
#include "soc/soc_caps.h"
#include "soc/spi_periph.h"
#include "hal/spi_ll.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/gptimer.h"

static bool on_timer_alarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    volatile int *count = (volatile int *)user_ctx;
    (*count)++;
    return false;
}

static void timer_test(int flags)
{
    static int count[SOC_TIMER_GROUP_TOTAL_TIMERS] = {0};
    gptimer_handle_t gptimers[SOC_TIMER_GROUP_TOTAL_TIMERS];
    intr_handle_t inth[SOC_TIMER_GROUP_TOTAL_TIMERS];

    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
        .flags.intr_shared = (flags & ESP_INTR_FLAG_SHARED) == ESP_INTR_FLAG_SHARED,
    };
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_new_timer(&config, &gptimers[i]));
    }
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 100000,
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_timer_alarm,
    };

    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_register_event_callbacks(gptimers[i], &cbs, &count[i]));
        alarm_config.alarm_count += 10000 * i;
        TEST_ESP_OK(gptimer_set_alarm_action(gptimers[i], &alarm_config));
        TEST_ESP_OK(gptimer_enable(gptimers[i]));
        TEST_ESP_OK(gptimer_start(gptimers[i]));
        TEST_ESP_OK(gptimer_get_intr_handle(gptimers[i], &inth[i]));
        printf("Interrupts allocated: %d\r\n", esp_intr_get_intno(inth[i]));
    }

    if ((flags & ESP_INTR_FLAG_SHARED)) {
        /* Check that the allocated interrupts are acutally shared */
        int intr_num = esp_intr_get_intno(inth[0]);
        for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
            TEST_ASSERT_EQUAL(intr_num, esp_intr_get_intno(inth[i]));
        }
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Timer values after 1 sec:");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        printf(" %d", count[i]);
    }
    printf("\r\n");

    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ASSERT_NOT_EQUAL(0, count[i]);
    }

    printf("Disabling timers' interrupt...\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        esp_intr_disable(inth[i]);
        count[i] = 0;
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    printf("Timer values after 1 sec:");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        printf(" %d", count[i]);
    }
    printf("\r\n");
    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ASSERT_EQUAL(0, count[i]);
    }

    for (int i = 0; i < SOC_TIMER_GROUP_TOTAL_TIMERS; i++) {
        TEST_ESP_OK(gptimer_stop(gptimers[i]));
        TEST_ESP_OK(gptimer_disable(gptimers[i]));
        TEST_ESP_OK(gptimer_del_timer(gptimers[i]));
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

void static test_isr(void*arg)
{
    /* ISR should never be called */
    abort();
}


TEST_CASE("Allocate previously freed interrupt, with different flags", "[intr_alloc]")
{
    intr_handle_t intr;
#if CONFIG_IDF_TARGET_ESP32P4
    int test_intr_source = ETS_GPIO_INTR0_SOURCE;
#else
    int test_intr_source = ETS_GPIO_INTR_SOURCE;
#endif
    int isr_flags = ESP_INTR_FLAG_LEVEL2;

    TEST_ESP_OK(esp_intr_alloc(test_intr_source, isr_flags, test_isr, NULL, &intr));
    TEST_ESP_OK(esp_intr_free(intr));

    isr_flags = ESP_INTR_FLAG_LEVEL3;
    TEST_ESP_OK(esp_intr_alloc(test_intr_source, isr_flags, test_isr, NULL, &intr));
    TEST_ESP_OK(esp_intr_free(intr));
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

#if !SOC_RCC_IS_INDEPENDENT
#define TEST_BUS_RCC_CLOCK_ATOMIC() PERIPH_RCC_ATOMIC()
#else
#define TEST_BUS_RCC_CLOCK_ATOMIC()
#endif
TEST_CASE("allocate 2 handlers for a same source and remove the later one", "[intr_alloc]")
{
    intr_alloc_test_ctx_t ctx = {false, false, false, false };
    intr_handle_t handle1, handle2;

    // enable SPI2
    TEST_BUS_RCC_CLOCK_ATOMIC() {
        spi_ll_enable_bus_clock(1, true);
        spi_ll_reset_register(1);
    }

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

// RTC not supported on all target (e.g., esp32c2)
#if SOC_RTC_FAST_MEM_SUPPORTED
static RTC_IRAM_ATTR void dummy_rtc(void *arg)
{
}
#endif

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

// RTC not supported on all target (e.g., esp32c2)
#if SOC_RTC_FAST_MEM_SUPPORTED
    err = esp_intr_alloc(spi_periph_signal[1].irq,
                         ESP_INTR_FLAG_IRAM, &dummy_rtc, NULL, &ih);
    TEST_ESP_OK(err);
    err = esp_intr_free(ih);
    TEST_ESP_OK(err);
#endif
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

void isr_alloc_free_test(bool isr_free_task_no_affinity)
{
    intr_handle_t test_handle = NULL;
    esp_err_t ret = esp_intr_alloc(spi_periph_signal[1].irq, 0, int_handler1, NULL, &test_handle);
    if (ret != ESP_OK) {
        printf("alloc isr handle fail\n");
    } else {
        printf("alloc isr handle on core %d\n", esp_intr_get_cpu(test_handle));
    }
    TEST_ESP_OK(ret);
    if (isr_free_task_no_affinity) {
        xTaskCreate(isr_free_task, "isr_free_task", 1024 * 2, (void *)&test_handle, 3, NULL);
        esp_rom_delay_us(500);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    } else {
        xTaskCreatePinnedToCore(isr_free_task, "isr_free_task", 1024 * 2, (void *)&test_handle, 10, NULL, !xPortGetCoreID());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    TEST_ASSERT_NULL(test_handle);
    printf("test passed\n");
}

TEST_CASE("alloc and free isr handle on different core", "[intr_alloc]")
{
    isr_alloc_free_test(false);
}

TEST_CASE("alloc and free isr handle on different core when isr_free_task is NO_AFFINITY", "[intr_alloc]")
{
    isr_alloc_free_test(true);
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
